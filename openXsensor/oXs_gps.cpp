#include "oXs_gps.h"
#include <avr/pgmspace.h>

#ifdef DEBUG
//#define DEBUGGPS
#define DEBUGPARSEGPS
//#define DEBUGSENDGPS
//#define DEBUG_FORWARD_GPS_MSG_TO_PC
#endif

//#define DEBUGSIMULATEGPS

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

// **********************
// GPS data being read
// **********************
int32_t GPS_lon;               // longitude in degree with 7 decimals, (neg for S)
bool    GPS_lonAvailable; 
int32_t GPS_lat;               // latitude   in degree with 7 decimals, (neg for ?)
bool    GPS_latAvailable;

int32_t GPS_altitude;              // altitude in mm
bool    GPS_altitudeAvailable;
uint16_t GPS_speed_3d;              // speed in cm/s
bool    GPS_speed_3dAvailable;
uint16_t GPS_speed_2d;                 // speed in cm/s
bool    GPS_speed_2dAvailable ;
uint32_t GPS_ground_course ;     // degrees with 5 decimals
bool    GPS_ground_courseAvailable;

//uint8_t GPS_numSat;
//uint16_t GPS_hdop = 9999;           // Compute GPS quality signal
uint16_t GPS_packetCount = 0;
//uint32_t GPS_svInfoReceivedCount = 0; // SV = Space Vehicle, counter increments each time SV info is received.
//uint8_t GPS_update = 0;             // it's a binary toggle to distinct a GPS position update
//uint8_t GPS_numCh;                          // Number of channels
//uint8_t GPS_svinfo_chn[GPS_SV_MAXSATS];     // Channel number
//uint8_t GPS_svinfo_svid[GPS_SV_MAXSATS];    // Satellite ID
//uint8_t GPS_svinfo_quality[GPS_SV_MAXSATS]; // Bitfield Qualtity
//uint8_t GPS_svinfo_cno[GPS_SV_MAXSATS];     // Carrier to Noise Ratio (Signal Strength)
bool GPS_fix ; // true if gps data are available.
static uint8_t _msg_id; //used to identify the message type when reading the gps, is used also when buffer is parsed 

// Receive buffer
static union {
    ubx_nav_posllh posllh;
    ubx_nav_status status;
    ubx_nav_solution solution;
    ubx_nav_velned velned;
    ubx_nav_svinfo svinfo;
    uint8_t bytes[UBLOX_BUFFER_SIZE];
} _buffer;


#ifdef DEBUG  
OXS_GPS::OXS_GPS(HardwareSerial &print)
#else
OXS_GPS::OXS_GPS( uint8_t x )
#endif
{
  // constructor
#ifdef DEBUG  
  printer = &print; //operate on the address of print
#endif
}

// **************** Setup the GPS sensor *********************
#define SOFT_TX_BIT 6  // define which port is used to send the set up to the GPS (here pin arduino 6 = Port D6 is used 
#define SOFT_TX_DDR DDRD  // specify the port being used for this pin
#define SOFT_TX_PORT PORTD  // specify the port being used for this pin

#define MICROSECONDS_OVERHEAD_ADJUST 0

#define MICROSECONDS_PER_BIT_9600 ((1000000ul / 9600) - MICROSECONDS_OVERHEAD_ADJUST)
#define MICROSECONDS_PER_BIT_38400 ((1000000ul / 38400ul) - MICROSECONDS_OVERHEAD_ADJUST)


void gps_putchar (char c, uint16_t usPerBit) {
  uint8_t  bit_mask;
  SOFT_TX_PORT &= ~(1<<SOFT_TX_BIT); // start bit
  delayMicroseconds(usPerBit);
  // data bits
  for (bit_mask=0x01; bit_mask; bit_mask<<=1) {
    if (c & bit_mask) {
      SOFT_TX_PORT |= (1<<SOFT_TX_BIT);  // set output to 1     
    }
    else {
      SOFT_TX_PORT &= ~(1<<SOFT_TX_BIT); // set output to 0
    }
    delayMicroseconds(usPerBit);
  }  
  SOFT_TX_PORT |= (1<<SOFT_TX_BIT);   // 2 stop bits
  delayMicroseconds(usPerBit * 2);
  return ;
}



void OXS_GPS::setupGps( ) {
      const static uint8_t initGps1[] PROGMEM = { 
        0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, // activate NAV-POSLLH message
        0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x17,0xDA, //        NAV-SOL
        0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x12,0x00,0x01,0x00,0x00,0x00,0x00,0x23,0x2E, //        NAV-VELNED
//        0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39  // NAV-RATE for 1 hz
        0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, // NAV-RATE for 5 hz
//        0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, // NAV-RATE for 10 hz
        0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0x96, //        CFG-PRT : Set port to output only UBX (so deactivate NMEA msg) and baud = 38400.
        0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x91,0x84 
      }  ;   
      uint8_t initGpsIdx = 0 ;
  SOFT_TX_PORT |= (1<<SOFT_TX_BIT);   // put 1 in output port
  SOFT_TX_DDR |= (1<<SOFT_TX_BIT);    // set port as output
  delay(1000) ; // wait to be sure that GPS has started
  while (initGpsIdx < sizeof( initGps1)) {
//    Serial.println( pgm_read_byte_near(initGps1 + initGpsIdx ), HEX) ;    
    gps_putchar( pgm_read_byte_near(initGps1 + initGpsIdx++ ), MICROSECONDS_PER_BIT_9600) ; // Send initialisation command
    delay(10) ;
  }
  delay(100) ;
#ifdef DEBUG
  Serial.println(F("End of GPS setup")) ;
#endif  
}


// **************** Read the GPS    *********************

void OXS_GPS::readGps() { // read and process GPS data. do not send them.
#ifdef DEBUGSIMULATEGPS
      const static uint8_t simulateGpsBuffer[] PROGMEM = { // POS message
        0xB5,0x62,0x01,0x02,0x1C,0x00,0x58,0xF2,0x1A,0x14,0xB0,0x4D,0xA1,0xCF,0xAC,0xD5,0x1F,0x1D,0xE0,0x93,0x04,0x00,0xE0,0x93,0x04,0x00,0x06,0x17,0x00,0x00,0x3C,0x24,0x00,0x00,0x2C,0xF5 
       ,0xB5,0x62,0x01,0x12,0x24,0x00,0x58,0xF2,0x1A,0x14,0xFC,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x06,0x00,0x00,0xFC,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x01,0x00,0x00,0x00,0x51,0x25,0x02,0x3F,0x00 
      }  ;   
      static uint8_t simuIdx= 0 ; 
//      printer->print(simuIdx) ;printer->print(",") ;
       gpsNewFrameUBLOX( pgm_read_byte_near(simulateGpsBuffer + simuIdx) );
//  printer->print(F("Gps char: "));
//  printer->println( pgm_read_byte_near(simulateGpsBuffer[simuIdx]),hex );  
        simuIdx++ ;
        if ( simuIdx >= sizeof( simulateGpsBuffer) ) simuIdx = 0  ;   
#else
       static uint8_t gpsChar ;
       while  (Serial.available() ) { // read all bytes in Serial Rx buffer.
          gpsChar = Serial.read() ;
#ifdef DEBUG_FORWARD_GPS_MSG_TO_PC
          if (gpsChar == 0xB5 ) { 
              Serial.println() ; Serial.print(gpsChar , HEX) ;
          } else {
              Serial.print(F(",")) ; Serial.print(gpsChar , HEX) ; 
          }    
#endif // end DEBUG_FORWARD_GPS_MSG_TO_PC           
          // process each incoming byte and so fill GPS data when a frame is received and correct
          gpsNewFrameUBLOX( gpsChar );
      } 
#endif  
}

bool OXS_GPS::gpsNewFrameUBLOX(uint8_t data) // handle one byte and if this byte is the last one of a valid frame, parse the whole frame to GPS data 
{
  static uint16_t gpsDataErrors;

  static uint8_t _ck_a;// Packet checksum accumulators
  static uint8_t _ck_b;// Packet checksum accumulators
  static bool _skip_packet;
  static uint8_t _step;
  static uint8_t _class;
  static uint16_t _payload_length;
  static uint16_t _payload_counter;


    bool parsed = false;
//    printer->print(_step);  printer->print(" "); printer->println(data , HEX);
    switch (_step) {
        case 0: // Sync char 1 (0xB5)
            if ( 0xB5 == data ) { // UBX sync char 1
                _skip_packet = false;
                _step++;
            }
            break;
        case 1: // Sync char 2 (0x62)
            if ( 0x62 != data) { // UBX sync char 1
                _step = 0;
                break;
            }
            _step++;
            break;
        case 2: // Class
            _step++;
            _class = data;  // normally we should check that the class is the expected (otherwise, frame should be skipped)
            _ck_b = _ck_a = data;   // reset the checksum accumulators
            if ( 0x01 != data ) { // we handle only message type = 0x01 = NAVigation message.
                _skip_packet = true; // when skip packet = true, then the wholepacket will be read but discarded.
            }            
            break;
        case 3: // Id
            _step++;
            _ck_b += (_ck_a += data);       // checksum byte
            _msg_id = data;
            break;
        case 4: // Payload length (part 1)
            _step++;
            _ck_b += (_ck_a += data);       // checksum byte
            _payload_length = data; // payload length low byte
            break;
        case 5: // Payload length (part 2)
            _step++;
            _ck_b += (_ck_a += data);       // checksum byte
            _payload_length += (uint16_t)(data << 8);
            if (_payload_length > UBLOX_PAYLOAD_SIZE) {
                _skip_packet = true; // when skip packet = true, then the wholepacket will be read but discarded.
            }
            _payload_counter = 0;   // prepare to receive payload
            if (_payload_length == 0) {
                _step = 7;
            }
            break;
        case 6:
            _ck_b += (_ck_a += data);       // checksum byte
            if (_payload_counter < UBLOX_BUFFER_SIZE) {
                _buffer.bytes[_payload_counter] = data; // save the content of the payload
//                printer->print(data , HEX);
              }
            _payload_counter++ ;  
            if (_payload_counter >= _payload_length) {
                _step++;
//                printer->println(" ");
            }
            break;
        case 7:
            _step++;
            if (_ck_a != data) {
                _skip_packet = true;          // bad checksum
                gpsDataErrors++;
            }
            break;
        case 8:
            _step = 0;
            if (_ck_b != data) {
                gpsDataErrors++;
                break;              // bad checksum
            }

            GPS_packetCount++;
//            printer->print("pac : ");  printer->print(GPS_packetCount); printer->print(",err: "); printer->print(gpsDataErrors); printer->print(",skip: "); printer->println(_skip_packet) ;

            if (_skip_packet) {
                break;   // do not parde the packet to be skipped
            }
                         // if we arive here, it means that a valid frame has been received and that the gpsBuffer contains the data to be parsed
            if (UBLOX_parse_gps() && (_class == 0x01) ) {
                parsed = true; 
            }
    }  // end of case
    return parsed; // return true if position AND speed have been received (not sure if this is usefull)
}

bool OXS_GPS::UBLOX_parse_gps(void) // move the data from buffer to the different fields
{
// do we have new position information?
static bool _new_position;

// do we have new speed information?
static bool _new_speed;
static bool next_fix;
  
    switch (_msg_id) {
    case MSG_POSLLH:
        //i2c_dataset.time                = _buffer.posllh.time;
        GPS_lon = _buffer.posllh.longitude;           // in degree with 7 decimals
        GPS_lat = _buffer.posllh.latitude;            // in degree with 7 decimals
        GPS_altitude = _buffer.posllh.altitude_msl ;  //alt in mm
        if (next_fix) {                               // enable state if a position has been received after a positieve STATUS or SOL
            GPS_fix = true;
        } else {
            GPS_fix = false;
        }
        GPS_lonAvailable = GPS_latAvailable = GPS_altitudeAvailable = GPS_fix; 
        _new_position = true;
#ifdef DEBUGPARSEGPS
  printer->print(F("Gps fix: "));
  printer->print(GPS_fix);
  printer->print(F(" long: "));
  printer->print(GPS_lon);
  printer->print(F(" lat: "));
  printer->print(GPS_lat);
  printer->print(F(" alt: "));
  printer->println(GPS_altitude);
#endif        
        break;
    case MSG_STATUS:                              // !!!!!!!!! I do not see real need of this message because same (and more) data are in SOL
        next_fix = (_buffer.status.fix_status & NAV_STATUS_FIX_VALID) && (_buffer.status.fix_type == FIX_3D); // si valid position and speed or 3D fix
        if (!next_fix)
             GPS_fix = false;
        break;
    case MSG_SOL:                                // !!!! here we could also use vertical speed which is I4 in cm/sec)
        next_fix = (_buffer.solution.fix_status & NAV_STATUS_FIX_VALID) && (_buffer.solution.fix_type == FIX_3D);
        if (!next_fix)
             GPS_fix = false;
//        GPS_numSat = _buffer.solution.satellites;  Not used currently
//        GPS_hdop = _buffer.solution.position_DOP;  Not used currently
        break;
    case MSG_VELNED:   // here we should add the 3d speed (if accurate enough
        GPS_speed_3d  = _buffer.velned.speed_3d;  // cm/s
        GPS_speed_3dAvailable = GPS_fix ;
        GPS_speed_2d = _buffer.velned.speed_2d;    // cm/s
        GPS_speed_2dAvailable = GPS_fix ;
        GPS_ground_course = _buffer.velned.heading_2d ;     // Heading 2D deg with 5 decimals
        GPS_ground_courseAvailable = GPS_fix ;
        _new_speed = true;
#ifdef DEBUGPARSEGPS
  printer->print(F("Gps spd 3d: "));
  printer->print(GPS_speed_3d);
  printer->print(F(" 2d: "));
  printer->print(GPS_speed_2d);
  printer->print(F(" course: "));
  printer->println(GPS_ground_course);
#endif        
        break;
    default:
        return false;
    } // end of case

    // we only return true when we get new position and speed data
    // this ensures we don't use stale data
    if (_new_position && _new_speed) {
        _new_speed = _new_position = false;
        return true;
    }
    return false;
}


