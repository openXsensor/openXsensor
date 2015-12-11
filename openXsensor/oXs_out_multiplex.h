#ifndef OXS_OUT_MULTIPLEX_h
#define OXS_OUT_MULTIPLEX_h

#include "oXs_config.h"
#include "oXs_ms5611.h" // we need the variodata struct
#include "oXs_4525.h" // we need the airspeeddata struct
#include "oXs_curr.h" // we need the currentdata struct
#include "oXs_voltage.h" // we need the arduinodata struct
//#include <Arduino.h>
#include "oXs_general.h"


#if defined(PROTOCOL) &&  (PROTOCOL == MULTIPLEX) 


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



//list of all telemetry units supported by Multiplex protocol
#define MU_ALT     0x08  // 1m       (-500 2000)
#define MU_VSPD    0x03  // 0,1 m/s (-500 500) 
#define MU_CURR    0x02  // 0,1 A   (-1000 1000)
#define MU_VOLT    0x01  // 0,1 V   (-600 600)
#define MU_TEMP    0x06  // 0,1 C   (-250 7000)
#define MU_RPM     0x05  // 100t/m?? ou 10t/min
#define MU_MAH     0x0B  // 1mAh    (-16000 16000)
#define MU_ASPD    0x04  // 0,1km/h (0-6000)
#define MU_LEVEL   0x09  // 1%      (0-100)
#define MU_DIR     0x07  // 0,1 degrés  (0 3600)
#define MU_LIQUID  0x0C  // 1ml      (0-16000)
#define MU_DIST    0x0D  // 0,1 km   (0-16000)
// End of list of all telemetry units supported by Multiplex protocol  


#define MULTIPLEX_UNITS MU_LEVEL , MU_ALT , MU_VSPD , MU_LEVEL , MU_ALT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT ,\
                        MU_VOLT , MU_CURR , MU_MAH , MU_VOLT , MU_VOLT ,MU_VOLT , MU_RPM , MU_ALT , MU_VSPD ,  MU_LEVEL ,\
                        MU_ALT , MU_ASPD , MU_VSPD , MU_LEVEL , MU_LEVEL , MU_VSPD , MU_LEVEL , MU_LEVEL , MU_LEVEL , MU_VSPD , \
                        MU_ALT , MU_ALT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , \
                        MU_ALT
  

//  This is the list of codes for each available measurements
#define ALTIMETER       1        // DEFAULTFIELD can be used in SPORT protocol (is then the same as ALT_FIRST_ID);  it MUST be used in Hub protocol because meters and centimeters are send in different fileds
#define VERTICAL_SPEED  2        // DEFAULTFIELD can be used
#define SENSITIVITY     3        // DEFAULTFIELD can NOT be used
#define ALT_OVER_10_SEC 4        // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
                                 // there is no telemetry field for this; it is possible to use e.g. T1 or T2; then you can use a custom function "play value" on Tx side
#define VOLT1           5        // DEFAULTFIELD can NOT be used
#define VOLT2           6        // DEFAULTFIELD can NOT be used
#define VOLT3           7        // DEFAULTFIELD can NOT be used
#define VOLT4           8        // DEFAULTFIELD can NOT be used
#define VOLT5           9        // DEFAULTFIELD can NOT be used
#define VOLT6           10        // DEFAULTFIELD can NOT be used
#define CURRENTMA       11        // DEFAULTFIELD can be used
#define MILLIAH         12        // if value must be sent as percentage, then uncomment the line "#define SEND_mAhPercentageAsFuel 4000" (see below)
#define CELLS_1_2       13        // Only DEFAULTFIELD can be used
#define CELLS_3_4       14        // Only DEFAULTFIELD can be used
#define CELLS_5_6       15        // Only DEFAULTFIELD can be used
#define RPM             16        // Only DEFAULTFIELD can be used
#define ALTIMETER_2        17      // DEFAULTFIELD can be used in SPORT protocol (is then the same as ALT_FIRST_ID);  it MUST be used in Hub protocol because meters and centimeters are send in different fileds
#define VERTICAL_SPEED_2   18      // DEFAULTFIELD can be used
#define SENSITIVITY_2      19      // DEFAULTFIELD can NOT be used
#define ALT_OVER_10_SEC_2  20      // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
#define AIR_SPEED          21      // DEFAULTFIELD can be used in SPORT only
#define PRANDTL_COMPENSATION 22      // DEFAULTFIELD can NOT be used;use e.g. Temperature 1 or 2
#define PPM_VSPEED         23       // DEFAULTFIELD can be used; Vpseed from first or second MS5611 or compensatedVspeed will be sent; to be used only when VARIO_SECONDARY is defined and PIN_PPM is defined
#define PPM                24       // DEFAULTFIELD can NOT be used ;
#define PRANDTL_DTE        25       // DEFAULTFIELD can be used
#define TEST1              26       // reserved : only for debugging
#define TEST2             27       // reserved : only for debugging
#define TEST3             28       // reserved : only for debugging
#define VERTICAL_SPEED_A  29
#define REL_ALTIMETER     30
#define REL_ALTIMETER_2   31
#define CELL_1            32
#define CELL_2            33
#define CELL_3            34
#define CELL_4            35
#define CELL_5            36
#define CELL_6            37
#define CELL_MIN          38
#define CELL_TOT          39
#define ALTIMETER_MAX     40
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

    void formatAllMultiplexData() ; 
    uint8_t formatOneValue ( uint8_t currentFieldToSend ) ;
    void setMultiplexNewData(  uint16_t id, int32_t value  , uint8_t alarm) ;
};

extern int ppm ; 
extern bool ppmAvailable ;
extern struct ONE_MEASUREMENT mainVspeed ;           // used to transmit the main Vspeed(calculated based on all set up in config)
extern struct ONE_MEASUREMENT compensatedClimbRate ; // used to transmit the compensated Vspeed
extern struct ONE_MEASUREMENT switchVSpeed ;         // used to transmit the selected Vspeed
extern struct ONE_MEASUREMENT averageVSpeed ;        // used to transmit the average Vspeed
extern struct ONE_MEASUREMENT vSpeedImu ;            // used to transmit the Vspeedcalculated based on IMU


extern struct ONE_MEASUREMENT test1 ;
extern struct ONE_MEASUREMENT test2 ;
extern struct ONE_MEASUREMENT test3 ;
extern struct ONE_MEASUREMENT gliderRatio ;

extern uint8_t selectedVario ;

#ifdef MEASURE_RPM
extern volatile uint16_t RpmValue ;
extern bool RpmAvailable ;
#endif // MEASURE_RPM

extern volatile uint8_t debug01 ;
extern volatile uint8_t debug02 ;
extern volatile uint8_t debug03 ;
extern volatile uint8_t debug04 ;

 
void setMultiplexNewData( struct t_sportData * volatile pdata, uint16_t id, uint32_t value ) ;
void initMultiplexUart( struct t_mbAllData * volatile pdata ) ;


extern volatile bool RpmSet ;
extern volatile uint16_t RpmValue ;


// UART's state.
#define   IDLE               0        // Idle state, both transmit and receive possible.
#define   TRANSMIT           1        // Transmitting byte.
#define   TRANSMIT_STOP_BIT  2        // Transmitting stop bit.
#define   RECEIVE            3        // Receiving byte.
#define   TxPENDING          4
#define   WAITING            5


//This section chooses the correct timer values for Multiplex protocol = 38400 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         (278*6)  // Ticks between two bits.
//#define TICKS2WAITONE       (278*6)  // Wait one bit period.
//#define TICKS2WAITONE_HALF  (416*6)  // Wait one and a half bit period.
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    // Sinan: Not tested
    #define TICKS2COUNTMULTIPLEX         (521)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (521)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (781)    // Wait one and a half bit period.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define TICKS2COUNTMULTIPLEX         (417)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (417)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (625)    // Wait one and a half bit period.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define TICKS2COUNTMULTIPLEX         (208)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (208)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (313)    // Wait one and a half bit period.
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
    #define INTERRUPT_EXEC_CYCL   45       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  16       // Cycles to allow of other interrupts.
  #else
    #error Unsupported clock speed
  #endif

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

#define MB_MAX_ADRESS  15

// values used by the state ".active"
#define NOT_ACTIVE     0
#define NOT_AVAILABLE  1
#define LOCKED         2
#define AVAILABLE      3

#endif // End of MULTIPLEX
#endif // OXS_OUT_MULTIPLEX_h






