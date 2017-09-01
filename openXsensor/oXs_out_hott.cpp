// File for Hott
#include "oXs_out_hott.h"
#if defined(PROTOCOL) &&  (PROTOCOL == HOTT) 

#ifdef DEBUG_BLINK
    #define DEBUG_BLINK_UPLOAD_HOTT_DATA
#endif

#ifdef DEBUG
// ************************* here Several parameters to help debugging
  //#define DEBUGHOTT
#endif

extern OXS_MS5611 oXs_MS5611 ;
extern OXS_VOLTAGE oXs_Voltage ; 
extern OXS_CURRENT oXs_Current ;
extern OXS_4525 oXs_4525 ;


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

//volatile uint8_t sendStatus ;   
volatile uint8_t flagUpdateHottBuffer ; // When a polling for oXs is received, it says that main loop must update the buffer (within the 4mes)  
volatile uint8_t debugStatus ; 

// Transmit buffer
volatile static union {              // union is a easy way to access the data in several ways (name of field + index of byte)
    volatile HOTT_GAM_MSG gamMsg ;   // structured general air module
    volatile HOTT_GPS_MSG gpsMsg ;
    volatile uint8_t txBuffer[TXHOTTDATA_BUFFERSIZE] ;
}  TxHottData;



volatile uint8_t state ;                  //!< Holds the state of the UART.
static volatile unsigned char SwUartTXData ;     //!< Data to be transmitted.
static volatile unsigned char SwUartTXBitCount ; //!< TX bit counter.
static volatile uint8_t SwUartRXData ;           //!< Storage for received bits.
static volatile uint8_t SwUartRXBitCount ;       //!< RX bit counter.
static volatile uint8_t TxCount ;
static volatile uint8_t LastRx ;                 // last byte received (allows to check on the second byte received for polling)
volatile uint8_t debugUartRx ;
static uint8_t delayTxPendingCount ;             // Used to register the number of extra delay(each 1msec) to wait in TxPending (so before replying)
volatile uint8_t ppmInterrupted ; // This flag is activated at the end of handling interrupt on Timer 1 Compare A if during this interrupt handling an interrupt on pin change (INT0 or INT1) occurs
                         // in this case, ppm will be wrong and has to be discarded       

static uint8_t convertGpsFix[5] = {0x2d , 0x2d , 0x32 , 0x33 , 0x44 } ; 

#ifdef DEBUG  
OXS_OUT::OXS_OUT(uint8_t pinTx,HardwareSerial &print)
#else
OXS_OUT::OXS_OUT(uint8_t pinTx)
#endif
{
  _pinTx = pinTx ;    
#ifdef DEBUG 
  printer = &print; //operate on the address of print
#endif
} // end constructor


// **************** Setup the OutputLib *********************
void OXS_OUT::setup() {
//    flagUpdateHottBuffer = 0 ; // does not allow to fill directly the buffer with data to transmit; in fact, filling occurs only after a polling
    // fill the buffer with 0 
//    for ( uint8_t i = 0 ; i <= TXHOTTDATA_BUFFERSIZE ; i++ ) {       // first fill the buffer with 0 
//       TxHottData.txBuffer[i] = 0 ;
//    }
    // fill fixed values in the buffer
//    TxHottData.gamMsg.start_byte    = 0x7c ;
//    TxHottData.gamMsg.gam_sensor_id = 0x8d ; //GENRAL AIR MODULE = HOTT_TELEMETRY_GAM_SENSOR_ID
//    TxHottData.gamMsg.sensor_id     = 0xd0 ;
//    TxHottData.gamMsg.stop_byte     = 0x7D ;

    //initilalise PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input, tri-stated.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is input, tri-stated.

  // Activate pin change interupt on Tx pin
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;			              // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif
    PCIFR = (1<<PCIF2) ;               	// clear pending interrupt
    initHottUart( ) ;                   // initialise UART 
	
#ifdef DEBUGHOTT
      printer->print(F("Hott Output Module: TX Pin="));
      printer->println(_pinTx);
#endif
}

#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 0) && defined(CELL_UNDERVOLTAGE_WARNING)
 
 byte OXS_OUT::warning_beeps_Hott(void) {       // Determine whether a warning has to be sent to the transmitter. 
                                                // Define the state variables for the state machine controlling the warning generation.
                                                // W_IDLE is the IDLE state but that name cannot be used because IDLE is already defined in oXs_out_hott.h.
     static enum {W_IDLE = 0, WARNING, DEADTIME} state = W_IDLE;
     static unsigned long warning_start_time;
     static uint8_t current_warning;
      // In order not to flood the transmitter with warnings, we transmit our warnings only every 3 seconds.
      // If the dead time is over, fall back into the idle state.
     if (state == DEADTIME && millis() - warning_start_time > 3000)  state = W_IDLE;
      // State WARNING indicates that we just started to transmit a warning.
      // Repeat it for 500ms to make sure the transmitter can receive it.
      // After 500ms we stop transmitting the warning and disable new warnings for a while.
     if (state == WARNING && millis() - warning_start_time > 500) {
       state = DEADTIME;
       current_warning = 0;
     }
     // In the idle state, we are ready to accept new warnings.
     if (state == W_IDLE) {
        if (voltageData->mVoltCellMin < CELL_UNDERVOLTAGE_WARNING) {
            warning_start_time = millis();
            state = WARNING;
            current_warning = 17; /* Cell undervoltage warning */
        }
     }
     return current_warning;
 }
#endif

void OXS_OUT::sendData() {
#ifdef DEBUGHOTT
//      printer->print(F("F= ")); printer->print(flagUpdateHottBuffer);
//      printer->print(F(" S=")); printer->print(state);
//      printer->print(F(" LR=")); printer->print(LastRx);
//      printer->print(F(" Tc=")); printer->println(TxCount);
#endif
    if ( flagUpdateHottBuffer ) {        // this flag is set to true when UART get a polling of the device. Then measurement must be filled in the buffer
#ifdef DEBUG_BLINK_UPLOAD_HOTT_DATA
      blinkLed(5) ; // blink every 500 ms at least
#endif 
        // fill the buffer with 0 
        for ( uint8_t i = 0 ; i < TXHOTTDATA_BUFFERSIZE ; i++ ) {       // first fill the buffer with 0 
           TxHottData.txBuffer[i] = 0 ;
        }
        if ( flagUpdateHottBuffer == 0x8d  ) {        // this flag is set to true when UART get a polling of the GAM device. Then measurement must be filled in the buffer
              TxHottData.gamMsg.start_byte    = 0x7c ;
              TxHottData.gamMsg.gam_sensor_id = 0x8d ; //GENRAL AIR MODULE = HOTT_TELEMETRY_GAM_SENSOR_ID
              TxHottData.gamMsg.sensor_id     = 0xd0 ;
              TxHottData.gamMsg.stop_byte     = 0x7D ;

// in general air module data to fill are:
#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 1) 
              TxHottData.gamMsg.cell[0] =  voltageData->mVoltCell[0] /20 ; // Volt Cell 1 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 2) 
              TxHottData.gamMsg.cell[1] =  voltageData->mVoltCell[1] /20 ; // Volt Cell 2 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 3) 
              TxHottData.gamMsg.cell[2] =  voltageData->mVoltCell[2] /20 ; // Volt Cell 3 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 4) 
             TxHottData.gamMsg.cell[3] =  voltageData->mVoltCell[3] /20 ; // Volt Cell 4 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 5) 
              TxHottData.gamMsg.cell[4] =  voltageData->mVoltCell[4] /20 ; // Volt Cell 5 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 6) 
              TxHottData.gamMsg.cell[5] =  voltageData->mVoltCell[5] /20 ; // Volt Cell 6 (in 2 mV increments, 210 == 4.20 V)
#endif
#if defined(BATTERY_1_SOURCE) && ( (BATTERY_1_SOURCE == VOLT_1) || (BATTERY_1_SOURCE == VOLT_2) || (BATTERY_1_SOURCE == VOLT_3) || (BATTERY_1_SOURCE == VOLT_4) || (BATTERY_1_SOURCE == VOLT_5) || (BATTERY_1_SOURCE == VOLT_6) ) && defined(PIN_VOLTAGE)
              TxHottData.gamMsg.Battery1 = voltageData->mVolt[BATTERY_1_SOURCE - VOLT_1].value / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif
#if defined(BATTERY_1_SOURCE) && ( (BATTERY_1_SOURCE == ADS_VOLT_1) || (BATTERY_1_SOURCE == ADS_VOLT_2) || (BATTERY_1_SOURCE == ADS_VOLT_3) || (BATTERY_1_SOURCE == ADS_VOLT_4) ) && defined(ADS_MEASURE) && defined(ADS1115_INSTALLED)
              TxHottData.gamMsg.Battery1 = ads_Conv[BATTERY_1_SOURCE - ADS_VOLT_1].value / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif
#if defined(BATTERY_2_SOURCE) && ( (BATTERY_2_SOURCE == VOLT_1) || (BATTERY_2_SOURCE == VOLT_2) || (BATTERY_2_SOURCE == VOLT_3) || (BATTERY_2_SOURCE == VOLT_4) || (BATTERY_2_SOURCE == VOLT_5) || (BATTERY_2_SOURCE == VOLT_6) ) && defined(PIN_VOLTAGE)
              TxHottData.gamMsg.Battery2 = voltageData->mVolt[BATTERY_2_SOURCE - VOLT_1].value / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif
#if defined(BATTERY_2_SOURCE) && ( (BATTERY_2_SOURCE == ADS_VOLT_1) || (BATTERY_2_SOURCE == ADS_VOLT_2) || (BATTERY_2_SOURCE == ADS_VOLT_3) || (BATTERY_2_SOURCE == ADS_VOLT_4) ) && defined(ADS_MEASURE) && defined(ADS1115_INSTALLED)
              TxHottData.gamMsg.Battery1 = ads_Conv[BATTERY_2_SOURCE - ADS_VOLT_1].value / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif

#if defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == TEST_1 )
              TxHottData.gamMsg.temperature1 = test1.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == TEST_2 )
              TxHottData.gamMsg.temperature1 = test2.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == TEST_3 )
              TxHottData.gamMsg.temperature1 = test3.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == GLIDER_RATIO ) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
              TxHottData.gamMsg.temperature1 = gliderRatio.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == SENSITIVITY ) && defined(VARIO)
              TxHottData.gamMsg.temperature1 = oXs_MS5611.varioData.sensitivity.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && (TEMPERATURE_1_SOURCE == PPM ) && defined(PIN_PPM)
              TxHottData.gamMsg.temperature1 = ppm.value + 120 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_1_SOURCE) && ( (TEMPERATURE_1_SOURCE == VOLT_1 ) || (TEMPERATURE_1_SOURCE == VOLT_2 ) || (TEMPERATURE_1_SOURCE == VOLT_3 ) || (TEMPERATURE_1_SOURCE == VOLT_4 ) || (TEMPERATURE_1_SOURCE == VOLT_5 ) || (TEMPERATURE_1_SOURCE == VOLT_6 ) )  && defined(PIN_VOLTAGE)
              TxHottData.gamMsg.temperature1 = (voltageData->mVolt[TEMPERATURE_1_SOURCE - VOLT_1].value ) + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#else
              TxHottData.gamMsg.temperature1 = 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#endif

#if defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == TEST_1 )
              TxHottData.gamMsg.temperature2 = test1.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == TEST_2 )
              TxHottData.gamMsg.temperature2 = test2.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == TEST_3 )
              TxHottData.gamMsg.temperature2 = test3.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == GLIDER_RATIO ) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
              TxHottData.gamMsg.temperature2 = gliderRatio.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == SENSITIVITY ) && defined(VARIO)
              TxHottData.gamMsg.temperature2 = oXs_MS5611.varioData.sensitivity.value + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && (TEMPERATURE_2_SOURCE == PPM ) && defined(PIN_PPM)
              TxHottData.gamMsg.temperature2 = ppm.value + 120 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#elif defined(TEMPERATURE_2_SOURCE) && ( (TEMPERATURE_2_SOURCE == VOLT_1 ) || (TEMPERATURE_2_SOURCE == VOLT_2 ) || (TEMPERATURE_2_SOURCE == VOLT_3 ) || (TEMPERATURE_2_SOURCE == VOLT_4 ) || (TEMPERATURE_2_SOURCE == VOLT_5 ) || (TEMPERATURE_2_SOURCE == VOLT_6 ) ) && defined(PIN_VOLTAGE)
              TxHottData.gamMsg.temperature2 = (voltageData->mVolt[TEMPERATURE_2_SOURCE - VOLT_1].value ) + 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#else
              TxHottData.gamMsg.temperature2 = 20 ; // Hott applies an offset of 20. A value of 20 = 0°C    
#endif

              TxHottData.gamMsg.rpm++ ;
              if ( TxHottData.gamMsg.rpm > 1000) TxHottData.gamMsg.rpm = 1 ; 
#ifdef MEASURE_RPM 
              TxHottData.gamMsg.rpm  = RpmValue /10 ;                      //#22 RPM in 10 RPM steps. 300 = 3000rpm
#endif
#ifdef VARIO       
              TxHottData.gamMsg.altitude =  ((varioData->relativeAlt.value ) / 100 ) + 500 ;  //altitude in meters. offset of 500, 500 = 0m
              TxHottData.gamMsg.climbrate_L = mainVspeed.value + 30000 ;          //climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
#else
              TxHottData.gamMsg.altitude =  500 ;  //altitude in meters. offset of 500, 500 = 0m
              TxHottData.gamMsg.climbrate_L = 30000 ;          //climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
#endif
              TxHottData.gamMsg.climbrate3s = 120 ;                     //#28 climb rate in m/3sec. Value of 120 = 0m/3sec
#if defined(PIN_CURRENTSENSOR)
              TxHottData.gamMsg.current =  currentData->milliAmps.value /100;               //current in 0.1A steps 100 == 10,0A
#endif
#if defined(MAIN_BATTERY_SOURCE) && ( (MAIN_BATTERY_SOURCE == VOLT_1) || (MAIN_BATTERY_SOURCE == VOLT_2) || (MAIN_BATTERY_SOURCE == VOLT_3) || (MAIN_BATTERY_SOURCE == VOLT_4) || (MAIN_BATTERY_SOURCE == VOLT_5) || (MAIN_BATTERY_SOURCE == VOLT_6) ) && defined(PIN_VOLTAGE)
              TxHottData.gamMsg.main_voltage = voltageData->mVolt[MAIN_BATTERY_SOURCE - VOLT_1].value / 100;          //Main power voltage using 0.1V steps 100 == 10,0V] / 100
#endif
#if defined(PIN_CURRENTSENSOR)
              TxHottData.gamMsg.batt_cap =  currentData->consumedMilliAmps.value / 10 ;   // used battery capacity in 10mAh steps
#endif
#ifdef AIRSPEED       
               TxHottData.gamMsg.speed =  airSpeedData->airSpeed.value  ;                  //  Km/h 
#endif
#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 0) 
              TxHottData.gamMsg.min_cell_volt =  voltageData->mVoltCellMin /20 ; // minimum cell voltage in 2mV steps. 124 = 2,48V
#endif
#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 0) && defined(CELL_UNDERVOLTAGE_WARNING)
             TxHottData.gamMsg.warning_beeps = warning_beeps_Hott();   // Transmitter warning message
#endif

// field of msg not implemented
//  byte climbrate3s;                     //#28 climb rate in m/3sec. Value of 120 = 0m/3sec
//  byte min_cell_volt_num;               //#38 number of the cell with the lowest voltage
//  uint16_t rpm2;                        //#39 LSB 2nd RPM in 10 RPM steps. 100 == 1000rpm
//  byte general_error_number;            //#41 General Error Number (Voice Error == 12) TODO: more documentation
//  byte pressure;                        //#42 High pressure up to 16bar. 0,1bar scale. 20 == 2.0bar

            } 
#ifdef GPS_INSTALLED            
              else {
            // here the code for GPS
                    TxHottData.gpsMsg.startByte    = 0x7c ;
                    TxHottData.gpsMsg.sensorID     = HOTT_TELEMETRY_GPS_SENSOR_ID ; //0x8A
                    TxHottData.gpsMsg.sensorTextID     = HOTTV4_GPS_SENSOR_TEXT_ID ; // 0xA0
                    TxHottData.gpsMsg.endByte     = 0x7D ;
                    uint16_t altitudeHott = 500 ;                   // Hott uses an offset of 500 (m)
                    if ( (GPS_fix_type == 3 ) || (GPS_fix_type == 4 ) ) {
//                    if( GPS_latAvailable ) {             // test if data are available (GPS fix) ; if available, fill the buffer
//                        GPS_latAvailable = false ;       // reset the flag     
                        TxHottData.gpsMsg.flightDirection = GPS_ground_course / 200000 ; // convert from degre * 100000 to 1/2 degree; Flightdir./dir. 1 = 2°; 0° (North), 90° (East), 180° (South), 270° (West)
                        static uint16_t speedHott ;
#ifdef GPS_SPEED_3D
                        speedHott = ((uint32_t) GPS_speed_3d) * 36 /1000 ;       // convert from cm/sec to km/h
#else
                        speedHott = ((uint32_t) GPS_speed_2d) * 36 /1000 ;       // convert from cm/sec to km/h
#endif
                        TxHottData.gpsMsg.GPSSpeedLow = speedHott ;                     
                        TxHottData.gpsMsg.GPSSpeedHigh = speedHott >> 8 ;               
                        uint16_t degMin ;
                        uint16_t decimalMin ;
                        TxHottData.gpsMsg.LatitudeNS = (GPS_lat < 0) ;                        // Byte 10: 000 = N = 48°39’0988 
                        convertLonLat_Hott(GPS_lat, & degMin , & decimalMin ) ;              // convert to 2 fields (one beging deg*100+min, the other being the decimal part of min with 4 decimals
                        TxHottData.gpsMsg.LatitudeMinLow = degMin ;                           // Byte 11: 231 = 0xE7 <= 0x12E7 = 4839 
                        TxHottData.gpsMsg.LatitudeMinHigh = degMin >> 8 ;                     // Byte 12: 018 = 0x12 <= 0x12E7 = 4839
                        TxHottData.gpsMsg.LatitudeSecLow = decimalMin ;                           // Byte 13: 220 = 0xDC <= 0x03DC = 0988
                        TxHottData.gpsMsg.LatitudeSecHigh = decimalMin >> 8 ;                     // Byte 14: 003 = 0x03 <= 0x03DC = 0988
                        
                        TxHottData.gpsMsg.longitudeEW = (GPS_lon < 0) ;                        // Byte 15: 000  = E= 9° 25’9360
                        convertLonLat_Hott(GPS_lon, &degMin , &decimalMin ) ;              // convert to 2 fields (one beging deg*100+min, the other being the decimal part of min with 4 decimals
                        TxHottData.gpsMsg.longitudeMinLow = degMin ;                           // Byte 16: 157 = 0x9D <= 0x039D = 0925
                        TxHottData.gpsMsg.longitudeMinHigh = degMin >> 8 ;                     // Byte 17: 003 = 0x03 <= 0x039D = 0925
                        TxHottData.gpsMsg.longitudeSecLow = decimalMin ;                           // Byte 18: 144 = 0x90 <= 0x2490 = 9360
                        TxHottData.gpsMsg.longitudeSecHigh = decimalMin >> 8 ;                     // Byte 19: 036 = 0x24 <= 0x2490 = 9360
                        TxHottData.gpsMsg.distanceLow = GPS_distance ;                             // Byte 20: 027 123 = /distance low byte 6 = 6 m
                        TxHottData.gpsMsg.distanceHigh = GPS_distance >> 8 ;                       // Byte 21: 036 35 = /distance high byte
                        TxHottData.gpsMsg.HomeDirection = GPS_bearing / 2 ;                        //Byte 29: HomeDirection (direction from starting point to Model position) (1 byte) 2degree = 1
                        altitudeHott += (GPS_altitude / 1000)  ;                                 // convert from mm to m (keep the ofsset of 500 m)
                   } 
                  
 /* not yet implemented
  uint8_t resolutionLow;           // Byte 24: 48 = Low Byte m/s resolution 0.01m 48 = 30000 = 0.00m/s (1=0.01m/s) 
  uint8_t resolutionHigh;          // Byte 25: 117 = High Byte m/s resolution 0.01m 
  uint8_t unknow1;                 // Byte 26: 120 = 0m/3s 
  uint8_t GPSNumSat;               // Byte 27: GPS.Satelites (number of satelites) (1 byte) 
  uint8_t GPSFixChar;              // Byte 28: GPS.FixChar. (GPS fix character. display, if DGPS, 2D oder 3D) (1 byte) 
  uint8_t HomeDirection;           // Byte 29: HomeDirection (direction from starting point to Model position) (1 byte) 
  uint8_t angleXdirection;         // Byte 30: angle x-direction (1 byte) 
  uint8_t angleYdirection;         // Byte 31: angle y-direction (1 byte) 
  uint8_t angleZdirection;         // Byte 32: angle z-direction (1 byte) 
  uint8_t gyroXLow;                // Byte 33: gyro x low byte (2 bytes) 
  uint8_t gyroXHigh;               // Byte 34: gyro x high byte 
  uint8_t gyroYLow;                // Byte 35: gyro y low byte (2 bytes) 
  uint8_t gyroYHigh;               // Byte 36: gyro y high byte 
  uint8_t gyroZLow;                // Byte 37: gyro z low byte (2 bytes) 
  uint8_t gyroZHigh;               // Byte 38: gyro z high byte 
  uint8_t vibration;               // Byte 39: vibration (1 bytes) 
  uint8_t Ascii4;                  // Byte 40: 00 ASCII Free Character [4] 
  uint8_t Ascii5;                  // Byte 41: 00 ASCII Free Character [5] 
  uint8_t GPS_fix;                 // Byte 42: 00 ASCII Free Character [6], we use it for GPS FIX 
  uint8_t version;                 // Byte 43: 00 version number 
  uint8_t endByte;                 // Byte 44: 0x7D Ende byte 
  uint8_t chksum;                  // Byte 45: Parity Byte 
*/            
                if (GPS_fix_type > 4 ) GPS_fix_type = 0 ;
                TxHottData.gpsMsg.GPS_fix = TxHottData.gpsMsg.GPSFixChar =  convertGpsFix[GPS_fix_type] ;
                TxHottData.gpsMsg.GPSNumSat = GPS_numSat;
                TxHottData.gpsMsg.altitudeLow = altitudeHott ; 
                TxHottData.gpsMsg.altitudeHigh = altitudeHott >> 8 ;
                uint16_t varioHott = 30000 ;
#ifdef VARIO
                varioHott += mainVspeed.value ;  // put vario vertical speed in GPS data
#endif                
                TxHottData.gpsMsg.resolutionLow = varioHott ;          //climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
                TxHottData.gpsMsg.resolutionHigh = varioHott >> 8;
                TxHottData.gpsMsg.unknow1 = 120 ;                                       // Byte 26: 120 = 0m/3s
              
            }  // end else => flagUpdateHottBuffer == GPS
#endif         // end of GPS_Installed            
            // calculate the check sum on first bytes
            TxHottData.txBuffer[TXHOTTDATA_BUFFERSIZE-1] = 0 ;
            for(uint8_t i = 0; i < TXHOTTDATA_BUFFERSIZE-1; i++){  // one byte less because the last byte is the checksum
              TxHottData.txBuffer[TXHOTTDATA_BUFFERSIZE-1] += TxHottData.txBuffer[i];
            }  // end for
            flagUpdateHottBuffer = 0 ;       // reset the flag to say that all data have been updated and that UART can transmit the buffer            
#ifdef DEBUGHOTT
//            for(uint8_t i = 0; i < TXHOTTDATA_BUFFERSIZE; i++){  // include the last byte (checksum)
//                 printer->print(TxHottData.txBuffer[i], HEX); printer->print(F(" "));
//            } // end for    
//            printer->print(tempFlag , HEX) ;
//            printer->println(F(" "));
#endif
        
    }   // end ( flagUpdateHottBuffer )
}


  

#ifdef GPS_INSTALLED
void convertLonLat_Hott( int32_t GPS_LatLon, uint16_t * degMin , uint16_t * decimalMin ) {
  static uint32_t GPS_LatLonAbs ;
  static uint16_t degre0decimals ;
  static uint32_t minute4decimals ;
  static uint16_t minute0decimals ;
  GPS_LatLonAbs = ( GPS_LatLon < 0 ? - GPS_LatLon : GPS_LatLon) / 100  ; // remove 2 decimals from original value which contains degre with 7 decimals (so next calculation are smaller)
  degre0decimals = GPS_LatLonAbs / 100000 ;                              // extract the degre without  decimal
  minute4decimals = ( GPS_LatLonAbs - ( ((uint32_t) degre0decimals) * 100000l ) ) * 6 ; // keep the decimal of degree and convert them in minutes (*60) and remove 1 decimal (/10) in order to keep 4 decimals 
  minute0decimals = minute4decimals / 10000 ;                                        // extract the minutes (without decimals)
  *degMin = degre0decimals * 100 + minute0decimals ;                                  // put degree and minutes toegether in a special format
  *decimalMin = minute4decimals - ( minute0decimals * 10000 ) ;                       // Extract the decimal part of the minutes (4 decimals) 
}
#endif
//***************************** here code to modify for GPS
/*
#ifdef GPS_INSTALLED
 //!! shared with Aserial
extern uint8_t volatile gpsSendStatus ; 
extern uint8_t volatile gpsSportDataLock ;
extern uint8_t volatile gpsSportData[7] ;
#define GPS_DATA_COUNT 5

void OXS_OUT::FrSkySportSensorGpsSend(void)
{
  // gpsSendStatus can be TO_LOAD, LOADED, SENDING, SEND ; it is managed here and in Aserial
  // new data is uploaded only when gpsSendStatus == SEND or TO_LOAD
  // each GPS data is loaded in sequence but only if available (otherwise this data is skipped)
  static uint8_t gpsDataIdx ;
  static uint16_t gpsSportId ;
  static uint32_t gpsSportValue ;
  static uint32_t gpsLastLoadedMillis ;
#ifdef DEBUGSIMULATEGPS
  static uint8_t gpsSimulateCount ;
#endif  
//   Serial.println(F("S gdps"));

  if  ((gpsSendStatus == SEND || gpsSendStatus == TO_LOAD) && (millis() - gpsLastLoadedMillis > 200 ) ){  // send only one data per 200 msec (to test if it help locking found on the Tx log)
            gpsDataIdx++;  // handle next GPS data; if not available, this field will be skipped.
            if(gpsDataIdx >= GPS_DATA_COUNT) {
              gpsDataIdx = 0;
            }
            switch(gpsDataIdx)
            {
              case 0: //longitude
                if (!GPS_lonAvailable) return ;
                GPS_lonAvailable = false ;
                gpsSportId = GPS_LONG_LATI_FIRST_ID ;
#ifdef DEBUGSIMULATEGPS
                gpsSportValue = ((( (((uint32_t)( GPS_lon < 0 ? -GPS_lon : GPS_lon)) * 6 ) / 100 ) + gpsSimulateCount++ )& 0x3FFFFFFF) | 0x80000000;
#else                
                gpsSportValue = (( (((uint32_t)( GPS_lon < 0 ? -GPS_lon : GPS_lon)) * 6 )/ 100 ) & 0x3FFFFFFF)  | 0x80000000;
#endif                
                if(GPS_lon < 0) gpsSportValue |= 0x40000000;
                break;
              case 1: //latitude
                if (!GPS_latAvailable) return ;
                GPS_latAvailable = false ;
                gpsSportId = GPS_LONG_LATI_FIRST_ID ;
                gpsSportValue = ((  (((uint32_t)( GPS_lat < 0 ? -GPS_lat : GPS_lat)) * 6 )/ 100 ) & 0x3FFFFFFF ) ;
                if(GPS_lat < 0) gpsSportValue |= 0x40000000;
                break;
              case 2: // GPS_altitude
                if (!GPS_altitudeAvailable) return ;
                GPS_altitudeAvailable = false ;
                gpsSportId = GPS_ALT_FIRST_ID ;
#ifdef DEBUGSIMULATEGPS
                gpsSportValue = (GPS_altitude / 10) + gpsSimulateCount++; // convert mm in cm 
#else                
                gpsSportValue = GPS_altitude / 10; // convert mm in cm 
#endif                
                break;
              case 3: // GPS_speed_3d  // could be 2D
#ifdef GPS_SPEED_3D
                if (!GPS_speed_3dAvailable) return ; 
                GPS_speed_3dAvailable = false ;
                gpsSportId = GPS_SPEED_FIRST_ID ;
#ifdef GPS_SPEED_IN_KMH
                gpsSportValue = ( ((uint32_t) GPS_speed_3d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
#else                                
                gpsSportValue = ( ((uint32_t) GPS_speed_3d) * 700 ) / 36  ; // convert cm/s in 1/100 of knots (factor = 19.44)
#endif // end of GPS_SPEED_IN_KMH
                break;
#else                   // use gps_Speed_2d
                if (!GPS_speed_2dAvailable) return ; 
                GPS_speed_2dAvailable = false ;
                gpsSportId = GPS_SPEED_FIRST_ID ;
#ifdef GPS_SPEED_IN_KMH
                gpsSportValue = ( ((uint32_t) GPS_speed_2d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
#else                                
                gpsSportValue = ( ((uint32_t) GPS_speed_2d) * 700 ) / 36 ; // convert cm/s in 1/1000 of knots (factor = 19.44)
                Serial.print(F("2d Knot:"));Serial.println(gpsSportValue); 
#endif // end of GPS_SPEED_IN_KMH
                break;
#endif //  enf of GPS_SPEED_3D             
              case 4: //GPS_ground_courseAvailable
                if (!GPS_ground_courseAvailable) return ;
                GPS_ground_courseAvailable = false ;
                gpsSportId = GPS_COURS_FIRST_ID ;
                gpsSportValue = GPS_ground_course / 1000; // convert from degree * 100000 to degree * 100 
                break;
              default:
                return ;
            } // end case    
            gpsSportDataLock = 1 ;
            gpsSportData[0] = 0x10 ;
            gpsSportData[1] = gpsSportId ; // low byte
            gpsSportData[2] = gpsSportId >> 8 ; // hight byte
            gpsSportData[3] = gpsSportValue ;
            gpsSportData[4] = gpsSportValue >> 8 ;
            gpsSportData[5] = gpsSportValue >> 16 ;
            gpsSportData[6] = gpsSportValue >> 24 ;
            gpsSportDataLock = 0 ;
#ifdef DEBUGSENDGPS
  Serial.print(F("ID: "));
  Serial.println(gpsSportId , HEX);
#endif

            gpsSendStatus = LOADED ; // from here data can be sent by the interrupt in Aserial
  } // end test on gpsSendStatus == SEND or TOLOAD          
} // end function

#endif // of of GPS_INSTALLED
*/




/*
void convert_to_degrees_minutes_seconds(float val, int *deg_sec, int *degMin){
  int16_t deg = (int)val;
  double sec = (val - deg);
  int8_t min = (int) (sec * 60);
  
  *deg_sec = abs((int) (((sec * 60) - min) * 10000.0f));
  *degMin = abs((int)(deg * 100 + min));
}
*/
/*tinygps FORMAT dd.ddddd
void convert_to_degrees_minutes_seconds(float val, int *deg_sec, int *degMin){
  int16_t deg = (int)val;    //extract degrees
  float sec = (val - deg);    
  int8_t min = (int) (sec * 60);//extract decimal deg
  
  *deg_sec = abs((int) (((sec * 60) - min) * 100000.0f));
  *degMin = abs((int)(deg * 100 + min));
}*/
/*
  static void hottV4GPSUpdate() {
    //number of Satelites
    HoTTV4GPSModule.GPSNumSat=MultiHoTTModule.GPS_numSat;
    if (MultiHoTTModule.GPS_fix > 0) {
      // GPS fix 
      HoTTV4GPSModule.GPS_fix = 0x33; // Dgps: '0x44' 2D = '0x32' 3D = '0x33' nofix = '0x2d'
      
      int16_t deg_sec;
      int16_t degMin;
    
      //latitude
      convert_to_degrees_minutes_seconds(MultiHoTTModule.GPS_latitude, &deg_sec, &degMin);
      HoTTV4GPSModule.LatitudeNS=(MultiHoTTModule.GPS_latitude<0);  
      HoTTV4GPSModule.LatitudeMinLow = degMin;
      HoTTV4GPSModule.LatitudeMinHigh = degMin >> 8;
      HoTTV4GPSModule.LatitudeSecLow = deg_sec;
      HoTTV4GPSModule.LatitudeSecHigh = deg_sec >> 8;
    
    
      //longitude
      convert_to_degrees_minutes_seconds(MultiHoTTModule.GPS_longitude, &deg_sec, &degMin);
      HoTTV4GPSModule.longitudeEW=(MultiHoTTModule.GPS_longitude<0);
      HoTTV4GPSModule.longitudeMinLow = degMin;
      HoTTV4GPSModule.longitudeMinHigh = degMin >> 8;
      HoTTV4GPSModule.longitudeSecLow = deg_sec;
      HoTTV4GPSModule.longitudeSecHigh = deg_sec >> 8;
    
    
      // GPS Speed in km/h 
      uint16_t speed = MultiHoTTModule.GPS_speed;  // in km/h
      HoTTV4GPSModule.GPSSpeedLow = speed & 0x00FF;
      HoTTV4GPSModule.GPSSpeedHigh = speed >> 8;
      // Distance to home       
      HoTTV4GPSModule.distanceLow = MultiHoTTModule.GPS_distanceToHome & 0x00FF;
      HoTTV4GPSModule.distanceHigh = MultiHoTTModule.GPS_distanceToHome >> 8;
      // Altitude 
      HoTTV4GPSModule.altitudeLow = MultiHoTTModule.GPS_altitude & 0x00FF;
      HoTTV4GPSModule.altitudeHigh = MultiHoTTModule.GPS_altitude >> 8;
      // Home Direction
      HoTTV4GPSModule.HomeDirection = MultiHoTTModule.GPS_directionToHome;
      // Flightdirection
      HoTTV4GPSModule.flightDirection = MultiHoTTModule.GPS_flightDirection;
      
      //VARIO  not implemented yet, should be a BMP085
      //m/s
      HoTTV4GPSModule.resolutionLow = MultiHoTTModule.GPS_distanceToHome & 0x00FF;
      HoTTV4GPSModule.resolutionHigh = MultiHoTTModule.GPS_distanceToHome >> 8;
      //m/3s
      HoTTV4GPSModule.unknow1 = MultiHoTTModule.GPS_flightDirection;
      
      HoTTV4GPSModule.alarmTone = MultiHoTTModule.GPS_alarmTone;
      
    } else {
      HoTTV4GPSModule.GPS_fix = 0x2d; // Displays a ' ' to show nothing or clear the old value
    }
  }
*/

/*
 // Sends HoTTv4 capable GPS telemetry frame.
  static void hottV4SendGPS() {
    // Minimum data set for EAM     
    HoTTV4GPSModule.startByte = 0x7C;
    HoTTV4GPSModule.sensorID = HOTTV4_GPS_SENSOR_ID;
    HoTTV4GPSModule.sensorTextID = HOTTV4_GPS_SENSOR_TEXT_ID;
    HoTTV4GPSModule.endByte = 0x7D;
   
    
    hottV4GPSUpdate();

    if (is_set_home == 0)
    {
      HoTTV4GPSModule.alarmTone = 0x08; //alarm tone if no fix
    toggle_LED();           //Let the led blink
    }else
    {
      HoTTV4GPSModule.alarmTone = 0x0;
    }

    // Clear output buffer
    memset(&outBuffer, 0, sizeof(outBuffer));

    // Copy GPS data to output buffer
    memcpy(&outBuffer, &HoTTV4GPSModule, kHoTTv4BinaryPacketSize);

    // Send data from output buffer
    hottV4SendData(outBuffer, kHoTTv4BinaryPacketSize);
  }

*/
// **************************** End of code for GPS (still to modify




//---------------------------------- Here the code to handle the UART  
#ifdef DEBUG
//#define DEBUGSETNEWDATA
//#define DEBUGASERIAL
#endif

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))


ISR(TIMER1_COMPA_vect)
{
  switch (state)
  {
      case TRANSMIT :   // startbit has been sent, it is time to output now 8 bits and 1 stop bit
#if DEBUGASERIAL
          PORTC |= 1 ;
#endif
          if( SwUartTXBitCount < 8 ) {            // If not 8 bits have been sent
                if( SwUartTXData & 0x01 ) {           // If the LSB of the TX buffer is 1:
                  SET_TX_PIN_MB() ;
                } else {                                // Otherwise:
                  CLEAR_TX_PIN_MB() ;                     // Send a logic 0 on the TX_PIN
                }
                SwUartTXData = SwUartTXData >> 1 ;    // Bitshift the TX buffer and
                SwUartTXBitCount += 1 ;               // increment TX bit counter.
          } else {    //Send stop bit.
                SET_TX_PIN_MB() ;                             // Output a logic 1. (in high impedance) = put stop bit
                state = TRANSMIT_STOP_BIT;
                OCR1A += DELAY_2000;                  // Normally Add 2 msec to the stop bit (required by Hott protocol) 
          }
          OCR1A += TICKS2WAITONEHOTT ;  // Count one period into the future.
#if DEBUGASERIAL
        PORTC &= ~1 ;
#endif
          break ;

    case TRANSMIT_STOP_BIT:                    //************************************* handling after the stop bit has been sent
        if (  ++TxCount < TXHOTTDATA_BUFFERSIZE  ) {   
              SwUartTXData = TxHottData.txBuffer[TxCount] ;
              CLEAR_TX_PIN_MB();                     // Send a logic 0 on the TX_PIN as start bit  
              OCR1A = TCNT1 + TICKS2WAITONEHOTT  - INTERRUPT_BETWEEN_TRANSMIT ;   // Count one period into the future.
              SwUartTXBitCount = 0 ;
              state = TRANSMIT ;
        } else {                                        // all bytes have already been sent
              state = WAITING ;
              OCR1A += DELAY_2000;                      // 2mS gap before listening
              TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
              TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
        }
        break ;

        case RECEIVE :                                        // Start bit has been received and we will read bits of data receiving, LSB first.     
              OCR1A += TICKS2WAITONEHOTT ;                    // Count one period after the falling edge is trigged.
              uint8_t data ;                                  // Use a temporary local storage (it save some bytes (and perhaps time)
              data = SwUartRXBitCount ;
              if( data < 8 ) {                                //If 8 bits are not yet read
                  SwUartRXBitCount = data + 1 ;
                  data = SwUartRXData ;
                  data >>= 1 ;                                // Shift due to receiving LSB first.
                  if( !(GET_RX_PIN( ) == 0 )) data |= 0x80 ;  // If a logical 1 is read, let the data mirror this.
                  SwUartRXData = data ;
               } else {                                       //Done receiving =  8 bits are in SwUartRXData
                  if ( ( LastRx == HOTT_BINARY_MODE_REQUEST_ID ) && ( ( SwUartRXData == HOTT_TELEMETRY_GAM_SENSOR_ID)    // if the previous byte identifies a polling for a reply in binary format and current is oXs sensor ID
#ifdef GPS_INSTALLED                           
                              || (  SwUartRXData == HOTT_TELEMETRY_GPS_SENSOR_ID ) 
#endif                                                                                
                                                                             ) ) {       // the sensor has to reply (if it has data; here we assume it has always data and the data will be in the Hott buffer)
                               flagUpdateHottBuffer = SwUartRXData ;         // flag to say to send function that the buffer must be filled. It is expected that send function is called fast enough (so main loop may not be blocked) 
                               state = TxPENDING ;
                               OCR1A += ( DELAY_4000 - TICKS2WAITONEHOTT) ;                   // 4ms gap before sending; normally Hott protocols says to wait 5 msec but this is too much for timer1
                               delayTxPendingCount  = 1 ;            //  ask for 1 more delay of 1ms in order to reach the total of 5msec                 
                  } else {                                // Previous code is not equal to HOTT_BINARY_MODE_REQUEST_ID , enter to iddle mode (so we will accept to read another byte)                                 
                      DISABLE_TIMER_INTERRUPT() ;         // Stop the timer interrupts.
                      state = IDLE ;                      // Go back to idle.
                      PCIFR = ( 1<<PCIF2 ) ;              // clear pending interrupt
                      PCICR |= ( 1<<PCIE2 ) ;             // pin change interrupt enabled
                  }
                  LastRx = SwUartRXData ;                 // save the byte being received; so next time a byte is received, comparison is possible.
               } // End receiving  1 byte (8 bits) (and 1 bit)
        break ;
  
      case TxPENDING :                                         //End of delay before sending data has occurs
            if ( delayTxPendingCount ) {                       // if additional delay is requested, perform it      
                delayTxPendingCount--;
                OCR1A += DELAY_1000 ; 
            } else {
                if ( flagUpdateHottBuffer ) {                     // it is expected that the main loop will update the buffer and set this flag to true within the delay
                    OCR1A += DELAY_1000 ;                          // if it is not yet done, stay TxPENDING for 1 msec more
//                    state = WAITING ;
                } else {
                    CLEAR_TX_PIN_MB() ;                                // Send a start bit (logic 0 on the TX_PIN).
                    OCR1A = TCNT1 + TICKS2WAITONEHOTT  - INTERRUPT_ENTRY_TRANSMIT ;                // Count one period into the future (less due to time to enter ISR)
                    SwUartTXBitCount = 0 ;
                    SwUartTXData = TxHottData.txBuffer[0] ;
                    TxCount = 0 ;
                    state = TRANSMIT ;
                }
            }    
            break ;

    case WAITING :                                  // At the end of wait time, stop timer interrupt but allow pin change interrupt in order to allow to detect incoming data
           DISABLE_TIMER_INTERRUPT() ;              // Stop the timer interrupts.
           state = IDLE ;                           // Go back to idle.
           CLEAR_PIN_CHANGE_INTERRUPT() ;           // clear pending pin change interrupt
           ENABLE_PIN_CHANGE_INTERRUPT() ;          // pin change interrupt enabled
            break ;

  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif

} // End of ISR


// ***********************************  initialise UART for Hott protocol *********************************************
//  This function will set up pins to transmit and receive on. Control of Timer1 and pin change interrupt 0.
void initHottUart( )           //*************** initialise UART for Multiplex
{
    //PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is tri-stated.

  // External interrupt
  
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;                    // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif
    CLEAR_PIN_CHANGE_INTERRUPT() ;
    ENABLE_PIN_CHANGE_INTERRUPT() ; 
    state = IDLE ;     // Internal State Variable

#if DEBUGASERIAL
    DDRC = 0x01 ;   // PC0 as o/p debug = pin A0 !!!!
    PORTC = 0 ; 
#endif
}


// ! \brief  External interrupt service routine.  ********************
//  Interrupt on Pin Change to detect change on level on Hott signal (= could be a start bit)
//
// The falling edge in the beginning of the start
//  bit will trig this interrupt. The state will
//  be changed to RECEIVE, and the timer interrupt
//  will be set to trig one and a half bit period
//  from the falling edge. At that instant the
//  code should sample the first data bit.
//
//  note  initHottUart( void ) must be called in advance.
//
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt
// on this port

ISR(PCINT2_vect)                                  // There is a start bit.
{
  if (!( TRXPIN & ( 1 << PIN_SERIALTX ) )) {      // Pin is low = start bit 
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
            state = RECEIVE ;                 // Change state
            DISABLE_TIMER_INTERRUPT() ;       // Disable timer to change its registers.
            OCR1A = TCNT1 + TICKS2WAITONE_HALFHOTT - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.
#if DEBUGASERIAL
            PORTC |= 1 ;
#endif
            SwUartRXBitCount = 0 ;            // Clear received bit counter.
            CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
            ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
  }
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif

}  // end ISR pin change


// -------------------------End of HOTT protocol--------------------------------------------------------------------------------------

#endif // END of HOTT

