#ifndef OXS_OUT_MULTIPLEX_h
#define OXS_OUT_MULTIPLEX_h

#include "oXs_config.h"
#include "oXs_ms5611.h" // we need the variodata struct
#include "oXs_4525.h" // we need the airspeeddata struct
#include "oXs_curr.h" // we need the currentdata struct
#include "oXs_voltage.h" // we need the arduinodata struct
#include "Aserial.h"

#ifdef MULTIPLEX


struct t_mbOneData {
	uint8_t active;
	uint8_t response[3];
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
// Endof list of all telemetry units supported by Multiplex protocol  


#define MULTIPLEX_UNITS MU_LEVEL , MU_ALT , MU_VSPD , MU_LEVEL , MU_ALT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT ,\
                        MU_VOLT , MU_CURR , MU_MAH , MU_VOLT , MU_VOLT ,MU_VOLT , MU_RPM , MU_ALT , MU_VSPD ,  MU_LEVEL ,\
                        MU_ALT , MU_ASPD , MU_VSPD , MU_LEVEL , MU_LEVEL , MU_VSPD , MU_LEVEL , MU_LEVEL , MU_LEVEL , MU_ALT ,\
                        MU_ALT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT , MU_VOLT 
  

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
#define REL_ALTIMETER     29
#define REL_ALTIMETER_2   30
#define CELL_1            31
#define CELL_2            32
#define CELL_3            33
#define CELL_4            34
#define CELL_5            35
#define CELL_6            36
#define CELL_MIN          37
#define CELL_TOT          38

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


class OXS_OUT_FRSKY {
  public:
#ifdef DEBUG  
    OXS_OUT_FRSKY(uint8_t pinTx,HardwareSerial &print);
#else
    OXS_OUT_FRSKY(uint8_t pinTx);
#endif
    VARIODATA* varioData ;
    VARIODATA* varioData_2 ;
    AIRSPEEDDATA* airSpeedData ;
    CURRENTDATA* currentData ;
    VOLTAGEDATA* voltageData ;
//    MB_DATA1_T mb_data[16];
 //   uint8_t currentValueType ; //e.g. = ALTIMETER, VERTICAL_SPEED, = field_Id to transmit  
    void setup();
    void sendData();
    
  private:
// used by both protocols  
    uint8_t _pinTx;
#ifdef DEBUG  
    HardwareSerial* printer;
#endif

// used by Multiplex protocol
//void  formatMultiplexData( ) ;
void formatAllMultiplexData() ; 
uint8_t readStatusValue( uint8_t currentValueType) ;
uint8_t formatOneValue ( uint8_t currentFieldToSend ) ;
void setMultiplexNewData(  uint16_t id, int32_t value  , uint8_t alarm) ;


// used by SPORT protocol
 //  void sendMultiplexData() ;
//    uint8_t readStatusValue( uint8_t currentValueType) ;
//    void loadSportValueToSend(  uint8_t ValueTypeToLoad) ;
//    uint8_t nextFieldToSend(  uint8_t indexField) ;


};

extern int ppm ; 
extern bool ppmAvailable ;
extern int32_t compensatedClimbRate ;
extern bool compensatedClimbRateAvailable ;
extern int32_t switchVSpeed ; // used to transmit the selected Vspeed
extern bool switchVSpeedAvailable ;
extern int32_t test1Value ;// used in order to test the transmission of any value
extern bool test1ValueAvailable ;
extern int32_t test2Value ;// used in order to test the transmission of any value
extern bool test2ValueAvailable ;
extern int32_t test3Value ;// used in order to test the transmission of any value
extern bool test3ValueAvailable ;
extern uint8_t selectedVario ;

#ifdef MEASURE_RPM
extern volatile uint16_t RpmValue ;
extern bool RpmAvailable ;
#endif // MEASURE_RPM

#endif // End of MULTIPLEX
#endif // OXS_OUT_MULTIPLEX_h






