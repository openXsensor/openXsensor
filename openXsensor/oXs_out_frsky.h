// For FRSKY
#ifndef OXS_OUT_FRSKY_h
#define OXS_OUT_FRSKY_h

#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"
#include "oXs_ms5611.h" // we need the variodata struct
#include "oXs_4525.h" // we need the airspeeddata struct
#include "oXs_curr.h" // we need the currentdata struct
#include "oXs_voltage.h" // we need the arduinodata struct
#include "oXs_ads1115.h" // we need the data
//#include <Arduino.h>
#include "oXs_general.h"
// this file is used only for FRSKY
#if defined(PROTOCOL) && ( (PROTOCOL == FRSKY_SPORT) || ( PROTOCOL == FRSKY_HUB ) || (PROTOCOL == FRSKY_SPORT_HUB ) ) //if Frsky protocol is used

//#define DEBUG_SETUP_PIN 5  // allows to send a pulse on an output pin during the enlapsed time that Arduino runs the set up 
//#define DEBUG_SPORT_PIN 6  // allows to send a pulse on an output pin during the enlapsed time that Arduino checks for SPORT 

#define INTERVAL_FRAME1 200
#define INTERVAL_FRAME2 1000  // used by GPS

// default SPORT_SENSOR_ID
//#define DATA_ID_VARIO  0x00  // = sensor 0 used for Alt and Vspeed
//#define DATA_ID_FLVSS  0xA1  //          1
//#define DATA_ID_FAS    0x22  //          2
//#define DATA_ID_GPS    0x83  //          3 used for all GPS data
//#define DATA_ID_RPM    0xE4  //          4
//#define DATA_ID_ACC    0x67  //          7
//list of 28 device ID codes is (in sequence)
// 0x00,0xA1,0x22,0x83,0xE4,0x45,0xC6,0x67,0x48,0xE9,0x6A,0xCB,0xAC,0x0D,0x8E,0x2F,0xD0,0x71,0xF2,0x53,0x34,0x95,0x16,0xB7,0x98,0x39,0xBA,0x1B


// FrSky new DATA IDs (2 bytes) (copied from openTX telemetry/frsky_sport.cpp on 11 jul 2014) // those values are not used directly but bits 4 up to 11 are stored in an array in oXs_out_frsky.cpp 
#define ALT_FIRST_ID            0x0100
#define ALT_LAST_ID             0x010f
#define VARIO_FIRST_ID          0x0110
#define VARIO_LAST_ID           0x011f
#define CURR_FIRST_ID           0x0200
#define CURR_LAST_ID            0x020f
#define VFAS_FIRST_ID           0x0210
#define VFAS_LAST_ID            0x021f
#define CELLS_FIRST_ID          0x0300
#define CELLS_SECOND_ID         0x0301
#define CELLS_THIRD_ID          0x0302
#define CELLS_LAST_ID           0x030f
#define T1_FIRST_ID             0x0400
#define T1_LAST_ID              0x040f
#define T2_FIRST_ID             0x0410
#define T2_LAST_ID              0x041f
#define RPM_FIRST_ID            0x0500
#define RPM_LAST_ID             0x050f
#define FUEL_FIRST_ID           0x0600
#define FUEL_LAST_ID            0x060f
#define ACCX_FIRST_ID           0x0700
#define ACCX_LAST_ID            0x070f
#define ACCY_FIRST_ID           0x0710      
#define ACCY_LAST_ID            0x071f
#define ACCZ_FIRST_ID           0x0720
#define ACCZ_LAST_ID            0x072f
#define GPS_LONG_LATI_FIRST_ID  0x0800
#define GPS_LONG_LATI_LAST_ID   0x080f
#define GPS_ALT_FIRST_ID        0x0820
#define GPS_ALT_LAST_ID         0x082f
#define GPS_SPEED_FIRST_ID      0x0830
#define GPS_SPEED_LAST_ID       0x083f
#define GPS_COURS_FIRST_ID      0x0840
#define GPS_COURS_LAST_ID       0x084f
#define GPS_TIME_DATE_FIRST_ID  0x0850
#define GPS_TIME_DATE_LAST_ID   0x085f
#define A3_FIRST_ID             0x0900
#define A3_LAST_ID              0x090f
#define A4_FIRST_ID             0x0910
#define A4_LAST_ID              0x091f
#define AIR_SPEED_FIRST_ID      0x0a00
#define AIR_SPEED_LAST_ID       0x0a0f
#define RSSI_ID                 0xf101  // please do not use this code because it is already used by the receiver
#define ADC1_ID                 0xf102  // please do not use this code because it is already used by the receiver
#define ADC2_ID                 0xf103
#define BATT_ID                 0xf104
#define SWR_ID                  0xf105  // please do not use this code because it is already used by the receiver
// End of list of all telemetry fields supported by SPORT  (defined by Frsky) 

//list of all telemetry fields supported by Hub protocol (defined by Frsky) 
#define FRSKY_USERDATA_GPS_ALT_B    0x01
#define FRSKY_USERDATA_TEMP1        0x02
#define FRSKY_USERDATA_RPM          0x03
#define FRSKY_USERDATA_FUEL         0x04
#define FRSKY_USERDATA_TEMP2        0x05
#define FRSKY_USERDATA_CELL_VOLT    0x06
#define FRSKY_USERDATA_GPS_ALT_A    0x09
#define FRSKY_USERDATA_BARO_ALT_B   0x10
#define FRSKY_USERDATA_GPS_SPEED_B  0x11
#define FRSKY_USERDATA_GPS_LONG_B   0x12
#define FRSKY_USERDATA_GPS_LAT_B    0x13
#define FRSKY_USERDATA_GPS_CURSE_B  0x14
#define FRSKY_USERDATA_GPS_DM       0x15
#define FRSKY_USERDATA_GPS_YEAR     0x16
#define FRSKY_USERDATA_GPS_HM       0x17
#define FRSKY_USERDATA_GPS_SEC      0x18
#define FRSKY_USERDATA_GPS_SPEED_A  0x19
#define FRSKY_USERDATA_GPS_LONG_A   0x1A
#define FRSKY_USERDATA_GPS_LAT_A    0x1B
#define FRSKY_USERDATA_GPS_CURSE_A  0x1C
#define FRSKY_USERDATA_BARO_ALT_A   0x21
#define FRSKY_USERDATA_GPS_LONG_EW  0x22
#define FRSKY_USERDATA_GPS_LAT_EW   0x23
#define FRSKY_USERDATA_ACC_X        0x24
#define FRSKY_USERDATA_ACC_Y        0x25
#define FRSKY_USERDATA_ACC_Z        0x26
#define FRSKY_USERDATA_CURRENT      0x28
#define FRSKY_USERDATA_VERT_SPEED   0x30 // open9x Vario Mode Only
#define FRSKY_USERDATA_ALT_MIN      0x31 // open9x Vario Mode Only
#define FRSKY_USERDATA_ALT_MAX      0x32 // open9x Vario Mode Only
#define FRSKY_USERDATA_RPM_MAX      0x33 // open9x Vario Mode Only
#define FRSKY_USERDATA_T1_MAX       0x34 // open9x Vario Mode Only
#define FRSKY_USERDATA_T2_MAX       0x35 // open9x Vario Mode Only
#define FRSKY_USERDATA_GPS_SPEED_MAX  0x36 // open9x Vario Mode Only
#define FRSKY_USERDATA_GPS_DIS_MAX  0x37 // open9x Vario Mode Only
#define FRSKY_USERDATA_VFAS_NEW     0x39 // Use this field in order to display the value on VFAS on Tx, Take care that for a voltage, the value must be in 1/10 of Volt and not in mVolt
#define FRSKY_USERDATA_VOLTAGE_B    0x3A // do not use this code to transmit a voltage. It requires a special formatting that is not implemented. Use VFAS_NEW instead
#define FRSKY_USERDATA_VOLTAGE_A    0x3B // do not use this code to transmit a voltage. It requires a special formatting that is not implemented. Use VFAS_NEW instead
#define FRSKY_USERDATA_GPS_DIST     0x3C
#define FRSKY_USERDATA_FUELPERCENT  0x3D
// Endof list of all telemetry fields supported by Hub protocol (defined by Frsky) 


/*  
// mapping of fields to send
#define DEFAULTFIELD  0x00  // value to let OXS select automatically the best telemetry field.
#define   Alt      1       
#define   VSpd     2
#define   Curr     3
#define   Vfas     4
#define   T1       5
#define   T2       6
#define   Rpm      7
#define   Fuel     8
#define   AccX     9
#define   AccY     10
#define   AccZ     11
#define   A3       12
#define   A4       13
#define   ASpd     14
*/

//  This is the list of codes for each available measurements
#define ALTIMETER       1        
#define VERTICAL_SPEED  2        
#define SENSITIVITY     3       
#define ALT_OVER_10_SEC 4        
#define VOLT_1           5       
#define VOLT_2           6       
#define VOLT_3           7       
#define VOLT_4           8       
#define VOLT_5           9       
#define VOLT_6           10      
#define CURRENTMA       11       
#define MILLIAH         12       
#define CELLS_1_2       13       
#define CELLS_3_4       14       
#define CELLS_5_6       15       
#define RPM             16       
#define ALTIMETER_2        17    
#define VERTICAL_SPEED_2   18    
#define SENSITIVITY_2      19    
#define ALT_OVER_10_SEC_2  20    
#define AIR_SPEED          21    
#define PRANDTL_COMPENSATION 22  
#define PPM_VSPEED         23    
#define PPM                24    
#define PRANDTL_DTE        25    
#define TEST_1              26   
#define TEST_2              27   
#define TEST_3              28   
#define VERTICAL_SPEED_A   29    
#define VERTICAL_SPEED_I   30    
#define GLIDER_RATIO       31
#define SECONDS_SINCE_T0        32
#define AVERAGE_VSPEED_SINCE_TO 33
#define PITCH               34  
#define ROLL                35
#define YAW                 36
#define ADS_1               37
#define ADS_2               38
#define ADS_3               39
#define ADS_4               40

// to do : add alt min, alt max ,  rpm max? , current max (not sure that it is neaded because it can be calculated on TX side
// End of list of type of available measurements

  
 
#define UNKNOWN false
#define KNOWN true

/***************************************************************************************/
/* Transmission status                                                                 */ 
/***************************************************************************************/
#define TO_LOAD     0
#define LOADED      1
#define SENDING     2
#define SEND        3



class OXS_OUT {
  public:
#ifdef DEBUG  
    OXS_OUT(uint8_t pinTx,HardwareSerial &print);
#else
    OXS_OUT(uint8_t pinTx);
#endif
    VARIODATA* varioData ;
    VARIODATA* varioData_2 ;
    AIRSPEEDDATA* airSpeedData ;
    CURRENTDATA* currentData ;
    VOLTAGEDATA* voltageData ;
 //   uint8_t currentValueType ; //e.g. = ALTIMETER, VERTICAL_SPEED, = field_Id to transmit  
    void setup();
    void sendData();
    
  private:
// used by both protocols  
    uint8_t _pinTx;
#ifdef DEBUG  
    HardwareSerial* printer;
#endif
 
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )    
// used by SPORT protocol
    void sendSportData() ;
    uint8_t readStatusValue( uint8_t currentValueType) ;
    void loadSportValueToSend(  uint8_t ValueTypeToLoad) ;
    uint8_t nextFieldToSend(  uint8_t indexField) ;
    void FrSkySportSensorGpsSend() ;
#endif

#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_HUB ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )  
// used by Hub protocol
    void sendHubData() ;
    void loadHubValueToSend(  uint8_t ValueTypeToLoad) ;
    void SendFrame1();
    void SendFrame2();
    void SendValue(uint8_t ID, uint16_t Value);
    void SendCellVoltage(uint32_t voltage);
    void SendGPSDist(uint16_t dist);
    void SendTemperature1(int16_t tempc);
    void SendTemperature2(int16_t tempc);
    void SendAlt(long altcm);
    void SendGPSAlt(long altcm);
    void SendFuel(uint16_t fuel);
    void SendCurrentMilliAmps(int32_t milliamps);
    void SendGPSSpeed(long speedknots) ;
    void sendHubByte( uint8_t byte ) ;
    void SendVoltX( uint8_t VoltToSend ,  uint8_t indexFieldToSend ) ;
#endif
};

//extern int ppm ; 
//extern bool ppmAvailable ;
extern struct ONE_MEASUREMENT ppm ; 
extern struct ONE_MEASUREMENT mainVspeed ;           // used to transmit the main Vspeed(calculated based on all set up in config)
extern struct ONE_MEASUREMENT compensatedClimbRate ; // used to transmit the compensated Vspeed
extern struct ONE_MEASUREMENT switchVSpeed ;         // used to transmit the selected Vspeed
extern struct ONE_MEASUREMENT averageVSpeed ;        // used to transmit the average Vspeed
extern struct ONE_MEASUREMENT vSpeedImu ;            // used to transmit the Vspeedcalculated based on IMU

#if defined(VFAS_SOURCE) 
extern struct ONE_MEASUREMENT vfas ; 
#endif

extern struct ONE_MEASUREMENT test1 ;
extern struct ONE_MEASUREMENT test2 ;
extern struct ONE_MEASUREMENT test3 ;
extern struct ONE_MEASUREMENT gliderRatio ;
extern struct ONE_MEASUREMENT secFromT0 ; 
extern struct ONE_MEASUREMENT averageVspeedSinceT0 ;     

extern uint8_t selectedVario ;

extern struct ONE_MEASUREMENT yaw ;
extern struct ONE_MEASUREMENT pitch ;
extern struct ONE_MEASUREMENT roll ;


#ifdef MEASURE_RPM
extern volatile uint16_t RpmValue ;
extern bool RpmAvailable ;
#endif // MEASURE_RPM

extern OXS_ADS1115 oXs_ads1115 ;



#ifdef GPS_INSTALLED
// **********************
// GPS data being read
// **********************
extern int32_t GPS_lon;               // longitude in degree with 7 decimals, (neg for S)
extern bool    GPS_lonAvailable; 
extern int32_t GPS_lat;               // latitude   in degree with 7 decimals, (neg for ?)
extern bool    GPS_latAvailable;

extern int32_t GPS_altitude;              // altitude in mm
extern bool    GPS_altitudeAvailable;
extern uint16_t GPS_speed_3d;              // speed in cm/s
extern bool    GPS_speed_3dAvailable;
extern uint16_t GPS_speed_2d;              // speed in cm/s
extern bool    GPS_speed_2dAvailable;
extern uint16_t GPS_speed;                 // speed in cm/s
extern bool    GPS_speedAvailable ;
extern uint32_t GPS_ground_course ;     // degrees with 5 decimals
extern bool    GPS_ground_courseAvailable;

extern uint8_t GPS_numSat;
extern uint16_t GPS_hdop ;           // Compute GPS quality signal
//uint16_t GPS_packetCount = 0;
//uint32_t GPS_svInfoReceivedCount = 0; // SV = Space Vehicle, counter increments each time SV info is received.
//uint8_t GPS_update = 0;             // it's a binary toggle to distinct a GPS position update
//uint8_t GPS_numCh;                          // Number of channels
//uint8_t GPS_svinfo_chn[GPS_SV_MAXSATS];     // Channel number
//uint8_t GPS_svinfo_svid[GPS_SV_MAXSATS];    // Satellite ID
//uint8_t GPS_svinfo_quality[GPS_SV_MAXSATS]; // Bitfield Qualtity
//uint8_t GPS_svinfo_cno[GPS_SV_MAXSATS];     // Carrier to Noise Ratio (Signal Strength)
extern bool GPS_fix ; // true if gps data are available.
#endif // End of GPS_INSTALLED

//******************* part used for handling of UART with Receiver ************************
extern volatile uint8_t debug01 ;
extern volatile uint8_t debug02 ;
extern volatile uint8_t debug03 ;
extern volatile uint8_t debug04 ;


// used by FRSKY_SPORT protocol
void setSportNewData( uint16_t id, uint32_t value ) ;
void initSportUart() ;
void initMeasurement() ;

// used by Hub protocol 
#define MAXSIZEBUFFER 70  // max size of the buffer used to store the data to be sent in the hub protocol
void initHubUart(  ) ;
void setHubNewData(  ) ;
void startHubTransmit( void ) ;

// used in all protocol   // not sure it is used here
extern volatile bool RpmSet ;
extern volatile uint16_t RpmValue ;

extern volatile bool sportAvailable ;

uint32_t micros( void ) ;
uint32_t millis( void ) ;

// UART's state.
#define   IDLE               0        // Idle state, both transmit and receive possible.
#define   TRANSMIT           1        // Transmitting byte.
#define   TRANSMIT_STOP_BIT  2        // Transmitting stop bit.
#define   RECEIVE            3        // Receiving byte.
#define    TxPENDING          4
#define   WAITING            5

// 57600 = Desired baudrate for Sport protocol = 17 micro sec per bit.
// 9600   =  Desired baudrate for Hub protocol
//This section chooses the correct timer values for the Sport protocol = 57600 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         278  // Ticks between two bits.
//#define TICKS2WAITONE       278  // Wait one bit period.
//#define TICKS2WAITONE_HALF  416  // Wait one and a half bit period.
  #if F_CPU == 20000000L   // 20MHz clock 
    // Sinan: Not tested                                                     
    #define TICKS2COUNTSPORT         348  // Ticks between two bits.
    #define TICKS2WAITONESPORT       348  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  520    // Wait one and a half bit period.
  #elif F_CPU == 16000000L  // 16MHz clock                                                  
    #define TICKS2COUNTSPORT         278  // Ticks between two bits.
    #define TICKS2WAITONESPORT       278  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  416    // Wait one and a half bit period.
  #elif F_CPU == 8000000L   // 8MHz clock
    // Assumes a 8MHz clock                                                   
    #define TICKS2COUNTSPORT         139  // Ticks between two bits.
    #define TICKS2WAITONESPORT       139  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  208    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif

//This section chooses the correct timer values for Hub protocol = 9600 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         (278*6)  // Ticks between two bits.
//#define TICKS2WAITONE       (278*6)  // Wait one bit period.
//#define TICKS2WAITONE_HALF  (416*6)  // Wait one and a half bit period.
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    // Sinan: Not tested
    #define TICKS2COUNTHUB         (348*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (348*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (520*6)    // Wait one and a half bit period.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define TICKS2COUNTHUB         (278*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (278*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (416*6)    // Wait one and a half bit period.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define TICKS2COUNTHUB         (139*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (139*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (208*6)    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif

//#define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
//#define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
// INTERRUPT_EARLY_BIAS is to bias the sample point a bit early in case
// the Timer 0 interrupt (5.5uS) delays the start bit detection
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define INTERRUPT_EXEC_CYCL   112       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  40       // Cycles to allow of other interrupts.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
  #else
    #error Unsupported clock speed
  #endif

    #define INTERRUPT_ENTRY_TRANSMIT  59   // Cycles in ISR before sending first bit from first byte; Without this correction, first bit is sent 7.4 usec to late at 8 Mhz (so it takes 59 cycles = 7.4 usec * 8)
    #define INTERRUPT_BETWEEN_TRANSMIT  64   // Cycles in ISR before sending first bit from 2d, 3rd... bytes; Without this correction, first bit is sent 4 usec to late at 16 Mhz (so it takes 64 cycles = 4 usec * 16)
    
// this section define some delays used in Aserial; values can be used by any protocol
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define DELAY_4000  ((uint16_t)4000.0 * 20.0 /16.0 )
    #define DELAY_3500  ((uint16_t)3500.0 * 20.0 /16.0 )    
    #define DELAY_2000  ((uint16_t)2000.0 * 20.0 /16.0 )
    #define DELAY_1600  ((uint16_t)1600.0 * 20.0 /16.0 )    
    #define DELAY_400  ((uint16_t)400.0 * 20.0 /16.0 )
    #define DELAY_100  ((uint16_t)100.0 * 20.0 /16.0 )
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define DELAY_4000 ((uint16_t) (3999L * 16) )     
    #define DELAY_3500 ((uint16_t) (3500L * 16) )         
    #define DELAY_2000 ((uint16_t) (2000L * 16) )     
    #define DELAY_1600 ((uint16_t) (1600L * 16) )     
    #define DELAY_400 ((uint16_t) (400 * 16) )     
    #define DELAY_100 ((uint16_t) (100 * 16) )     
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define  DELAY_4000 ((uint16_t)4000L * 8 )
    #define  DELAY_3500 ((uint16_t)3500L * 8 )    
    #define  DELAY_2000 ((uint16_t)2000 * 8 )
    #define  DELAY_1600 ((uint16_t)1600 * 8 )    
    #define  DELAY_400 ((uint16_t)400 * 8 )
    #define  DELAY_100 ((uint16_t)100 * 8 )    
  #else
    #error Unsupported clock speed
  #endif

#define TCCR             TCCR1A             //!< Timer/Counter Control Register
#define TCCR_P           TCCR1B             //!< Timer/Counter Control (Prescaler) Register
#define OCR              OCR1A              //!< Output Compare Register
#define EXT_IFR          EIFR               //!< External Interrupt Flag Register
#define EXT_ICR          EICRA              //!< External Interrupt Control Register

#define TRXDDR  DDRD
#define TRXPORT PORTD
#define TRXPIN  PIND

#define SET_TX_PIN( )    ( TRXPORT |= ( 1 << PIN_SERIALTX ) )
#define CLEAR_TX_PIN( )  ( TRXPORT &= ~( 1 << PIN_SERIALTX ) )

#if PIN_SERIALTX == 7
#define GET_RX_PIN( )    ( ACSR & ( 1 << ACO ) )
#else
#define GET_RX_PIN( )    ( TRXPIN & ( 1 << PIN_SERIALTX ) )
#endif

//******************* End of part used for handling of UART with Receiver

#endif // Enf of ndef MULTIPLEX

#endif // OXS_OUT_h






