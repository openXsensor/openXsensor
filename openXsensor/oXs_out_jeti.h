#ifndef OXS_OUT_JETI_h
#define OXS_OUT_JETI_h

#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"
#include "oXs_ms5611.h" // we need the variodata struct
#include "oXs_4525.h" // we need the airspeeddata struct
#include "oXs_curr.h" // we need the currentdata struct
#include "oXs_voltage.h" // we need the arduinodata struct
//#include <Arduino.h>
#include "oXs_general.h"
#include "oXs_gps.h"

#if defined(PROTOCOL) &&  (PROTOCOL == JETI) 


struct t_mbOneData {
	uint8_t volatile active;
	uint8_t volatile response[3];
} ;

struct t_mbAllData {
  struct t_mbOneData mbData[16] ;
} ;  



#define MB_CMD_RESET			0x5A
#define MB_CMD_RESERVED_MIN		0x80
#define MB_CMD_RESERVED_MAX		0x8F

#define MB_NOVALUE		0x8000

#define MB_MAX_ADRESS	15

// list of Jeti type being used
#define JETI_14 0b00000001  // jeti TYpe = 1
#define JETI_22 0b00000100  // jeti TYpe = 4
#define JETI_30 0b00001000  // jeti TYpe = 8

// List of used combination of jeti type of data and number of decimals (in bits 5 and 6)

#define JETI14_0D 0b00000001  // 0 decimal, jeti TYpe = 1
#define JETI14_1D 0b00100001  // 1 decimal, jeti TYpe = 1  
#define JETI14_2D 0b01000001  // 2 decimal, jeti TYpe = 1
#define JETI22_0D 0b00000100  // 0 decimal, jeti TYpe = 1
#define JETI22_1D 0b00100100  // 1 decimal, jeti TYpe = 1
#define JETI_GPS  0b00001001  // special GPS format in 4 bytes, type = 9 in decimal

//  This is the list of oXs codes for each available measurements
//#define ALTIMETER       1        
#define VERTICAL_SPEED  2        
//#define SENSITIVITY     3        
//#define ALT_OVER_10_SEC 4        // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
#define VOLT_1           5        
#define VOLT_2           6        
#define VOLT_3           7        
#define VOLT_4           8        
#define VOLT_5           9        
#define VOLT_6           10       
#define CURRENTMA       11        
#define MILLIAH         12        
#define GPS_COURSE       13       
#define GPS_SPEED        14       
#define GPS_ALTITUDE     15       
#define RPM             16        
#define GPS_DISTANCE       17     
#define GPS_BEARING        18     
//#define SENSITIVITY_2      19     
//#define ALT_OVER_10_SEC_2  20      
#define AIR_SPEED          21      
//#define PRANDTL_COMPENSATION 22     
//#define PPM_VSPEED         23       
//#define PPM                24       
//#define PRANDTL_DTE        25       
//#define TEST_1              26      
//#define TEST_2             27       
//#define TEST_3             28       
//#define VERTICAL_SPEED_A  29
#define REL_ALTIMETER     30
//#define REL_ALTIMETER_2   31
#define CELL_1            32
#define CELL_2            33
#define CELL_3            34
#define CELL_4            35
#define CELL_5            36
#define CELL_6            37
#define CELL_MIN          38
#define CELL_TOT          39
#define ALTIMETER_MAX     40
#define GPS_LONG          41
#define GPS_LAT           42
#define FLOW_ACTUAL       43
#define FLOW_REMAIN       44
#define FLOW_PERCENT      45
#define TEMPERATURE       46
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
    void setup();
    void sendData();
    
  private:
// used by both protocols  
    uint8_t _pinTx;
#ifdef DEBUG  
    HardwareSerial* printer;
#endif

    void formatAllJetiData() ; 
    uint8_t formatOneValue ( uint8_t currentFieldToSend ) ;
    void setJetiNewData(  uint16_t id, int32_t value  , uint8_t alarm) ;
    void initJetiListOfFields() ;
    boolean retrieveFieldIfAvailable(uint8_t fieldId , int32_t * fieldValue , uint8_t * dataType) ;
    boolean tryToAddFieldToJetiBuffer (void) ;
    void addFieldToJetiBuffer(int32_t fieldValue , uint8_t dataType ) ; 
  //  uint8_t jeti_crc8 (uint8_t *exbuf, unsigned char framelen) ;
  void fillJetiBufferWithText() ;
  void mergeLabelUnit( const uint8_t identifier , const char * label, const char * unit ) ;
  uint32_t formatGpsLongLat (int32_t longLat, boolean isLong ) ; // return the long or latitude in Jeti format
};

//extern int ppm ; 
//extern bool ppmAvailable ;
extern struct ONE_MEASUREMENT ppm ;
extern struct ONE_MEASUREMENT mainVspeed ;           // used to transmit the main Vspeed(calculated based on all set up in config)
extern struct ONE_MEASUREMENT compensatedClimbRate ; // used to transmit the compensated Vspeed
extern struct ONE_MEASUREMENT switchVSpeed ;         // used to transmit the selected Vspeed
extern struct ONE_MEASUREMENT averageVSpeed ;        // used to transmit the average Vspeed
extern struct ONE_MEASUREMENT vSpeedImu ;            // used to transmit the Vspeedcalculated based on IMU


extern struct ONE_MEASUREMENT test1 ;
extern struct ONE_MEASUREMENT test2 ;
extern struct ONE_MEASUREMENT test3 ;
extern struct ONE_MEASUREMENT gliderRatio ;
extern struct ONE_MEASUREMENT sport_rpm ;

extern uint8_t selectedVario ;

extern struct ONE_MEASUREMENT yaw ;
extern struct ONE_MEASUREMENT pitch ;
extern struct ONE_MEASUREMENT roll ;


#ifdef MEASURE_RPM
extern volatile uint16_t RpmValue ;
extern bool RpmAvailable ;
#endif // MEASURE_RPM

extern OXS_ADS1115 oXs_ads1115 ;

extern volatile uint8_t debug01 ;
extern volatile uint8_t debug02 ;
extern volatile uint8_t debug03 ;
extern volatile uint8_t debug04 ;

 
void setJetiNewData( struct t_sportData * volatile pdata, uint16_t id, uint32_t value ) ;
void initJetiUart( struct t_mbAllData * volatile pdata ) ;


extern volatile bool RpmSet ;
extern volatile uint16_t RpmValue ;

extern int32_t GPS_lon;               // longitude in degree with 7 decimals, (neg for S)
extern int32_t GPS_lat;               // latitude   in degree with 7 decimals, (neg for ?)
extern bool    GPS_latAvailable;
extern int32_t GPS_altitude;              // altitude in mm
extern uint16_t GPS_speed_3d;              // speed in cm/s
extern uint16_t GPS_speed_2d;                 // speed in cm/s
extern uint32_t GPS_ground_course ;     // degrees with 5 decimals
extern uint8_t GPS_numSat ;
extern uint16_t GPS_hdop;           // Compute GPS quality signal
extern uint8_t GPS_fix_type ;
extern int16_t GPS_distance ;
extern int16_t GPS_bearing ; 
extern uint8_t GPS_numSat;
extern uint16_t GPS_hdop ;           // Compute GPS quality signal
extern bool GPS_fix ; // true if gps data are available.



// UART's state.
#define   IDLE               0        // Idle state, both transmit and receive possible.
#define   TRANSMIT           1        // Transmitting byte.
#define   TRANSMIT_STOP_BIT  2        // Transmitting stop bit.
#define   RECEIVE            3        // Receiving byte.
#define   TxPENDING          4
#define   WAITING            5




//This section chooses the correct timer values for Jeti protocol = 9700 baud.(in fact between 9600 and 9800)
// Assumes a 16MHz clock
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    // Sinan: Not tested
    #define TICKS2COUNTJETI         (348*6)  // Ticks between two bits.
    #define TICKS2WAITONEJETI       (348*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFJETI  (520*6)    // Wait one and a half bit period.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define TICKS2COUNTJETI         (275*6)  // Ticks between two bits. For 9600 => (278*6)
    #define TICKS2WAITONEJETI       (275*6)  // Wait one bit period. For 9600 => (278*6)
    #define TICKS2WAITONE_HALFJETI  (411*6)  // Wait one and a half bit period. For 9600 => (416*6)
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define TICKS2COUNTJETI         (139*6)  // Ticks between two bits.
    #define TICKS2WAITONEJETI       (139*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFJETI  (208*6)    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif


//#define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
//#define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
// INTERRUPT_EARLY_BIAS is to bias the sample point a bit early in case
// the Timer 0 interrupt (5.5uS) delays the start bit detection
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
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

// this section define some delays used ; values can be used by any protocol
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define DELAY_4000  ((uint16_t)4000.0 * 20.0 /16.0 )
    #define DELAY_3500  ((uint16_t)3500.0 * 20.0 /16.0 )    
    #define DELAY_2000  ((uint16_t)2000.0 * 20.0 /16.0 )
    #define DELAY_1600  ((uint16_t)1600.0 * 20.0 /16.0 )    
    #define DELAY_400  ((uint16_t)400.0 * 20.0 /16.0 )
    #define DELAY_100  ((uint16_t)100.0 * 20.0 /16.0 )
    
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define DELAY_4000 ((uint16_t) (4000L * 16) )     
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
#define GET_RX_PIN( )    ( TRXPIN & ( 1 << PIN_SERIALTX ) )

#define SET_TX_PIN_MB( ) ( TRXDDR &= ~( 1 << PIN_SERIALTX ) ) // in fact we put the port in high impedance because the rx has a pull up resistance
#define CLEAR_TX_PIN_MB( ) ( TRXDDR |= ( 1 << PIN_SERIALTX ) ) // in fact we put the port in output mode. PORT is already set to 0 during initialisation

#define SET_TX_PIN_TO_OUTPUT( ) (TRXDDR |= ( 1 << PIN_SERIALTX ) ) // put TX pin as output
#define SET_TX_PIN_TO_INPUT( ) (TRXDDR &= ~( 1 << PIN_SERIALTX ) ; ( TRXPORT |= ( 1 << PIN_SERIALTX ) ) // put TX pin as input with pullup activated

// values used by the state ".active"
#define NOT_ACTIVE     0
#define NOT_AVAILABLE  1
#define LOCKED         2
#define AVAILABLE      3

// Jeti protocol is 9 bits, 2 stops bits, odd parity, 9600-9800 bauds


#endif // End of JETI
#endif // OXS_OUT_JETI_h






