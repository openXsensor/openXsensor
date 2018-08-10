#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_voltage.h"
#include "oXs_ms5611.h"
#include "oXs_bmp180.h"
#include "oXs_bmp280.h"
#include "oXs_4525.h"
#include "oXs_ads1115.h"
#include "oXs_curr.h"
#include "oXs_out_frsky.h"
#include "oXs_out_multiplex.h"
#include "oXs_out_hott.h"
#include "oXs_out_jeti.h"
#include "oXs_general.h"
#include "oXs_gps.h"

#ifdef USE_6050
  #include "oXs_imu.h"
  #include "KalmanFilter.h"
  #include "oXs_hmc5883.h"
#endif
  
#if defined (SAVE_TO_EEPROM ) and ( SAVE_TO_EEPROM == YES ) 
  #include <EEPROM.h>
  #include "EEPROMAnything.h"
#endif

#if  ! defined(PROTOCOL)
    #error The parameter PROTOCOL in config_basic.h is not defined
#elif ! ( (PROTOCOL == FRSKY_SPORT) or (PROTOCOL == FRSKY_HUB) or (PROTOCOL == FRSKY_SPORT_HUB) or (PROTOCOL == HOTT) or (PROTOCOL == MULTIPLEX)  or (PROTOCOL == JETI))    
    #error The parameter PROTOCOL in config_basic.h is NOT valid
#endif

#if  defined(VARIO) and (! defined(VSPEED_SOURCE))
    #error The parameter VSPEED_SOURCE in config_basic.h is not defined while a type of baro sensor is defined
#elif  defined(VARIO) and  defined(VSPEED_SOURCE) and ( ! ( (VSPEED_SOURCE == FIRST_BARO) or (VSPEED_SOURCE == SECOND_BARO) or (VSPEED_SOURCE == AVERAGE_FIRST_SECOND) \
                                                            or (VSPEED_SOURCE == AIRSPEED_COMPENSATED) or (VSPEED_SOURCE == BARO_AND_IMU) or (VSPEED_SOURCE == PPM_SELECTION) ) )
    #error The parameter VSPEED_SOURCE in config_basic.h is NOT valid
#endif    

#if defined( ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) and defined (AN_ADS1115_IS_CONNECTED) and (AN_ADS1115_IS_CONNECTED == YES) and defined(ADS_MEASURE) and defined(ADS_CURRENT_BASED_ON)
  #error It is not allowed to ask for current calculation based both on arduino Adc and on ads1115; define only or ARDUINO_MEASURES_A_CURRENT or ADS_CURRENT_BASED_ON
#endif            

#if defined (PIN_PPM ) && defined ( USE_6050 ) &&  ( PIN_INT_6050 == PIN_PPM )
  #error Error in oXs_config_advanced.h : PIN_PPM may not be equal to PIN_INT_6050
#endif    

#if defined( VFAS_SOURCE ) && ( !  ( ( VFAS_SOURCE == VOLT_1) || ( VFAS_SOURCE == VOLT_2) || ( VFAS_SOURCE == VOLT_3) || ( VFAS_SOURCE == VOLT_4) || ( VFAS_SOURCE == VOLT_5) || ( VFAS_SOURCE == VOLT_6) || ( VFAS_SOURCE == ADS_VOLT_1) || ( VFAS_SOURCE == ADS_VOLT_2) || ( VFAS_SOURCE == ADS_VOLT_3) || ( VFAS_SOURCE == ADS_VOLT_4)) ) 
 #error When defined, VFAS_SOURCE must be one of following values VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
#endif
#if defined( FUEL_SOURCE ) && ( !  ( ( FUEL_SOURCE == VOLT_1) || ( FUEL_SOURCE == VOLT_2) || ( FUEL_SOURCE == VOLT_3) || ( FUEL_SOURCE == VOLT_4) || ( FUEL_SOURCE == VOLT_5) || ( FUEL_SOURCE == VOLT_6) || ( VFAS_SOURCE == ADS_VOLT_1) || ( VFAS_SOURCE == ADS_VOLT_2) || ( VFAS_SOURCE == ADS_VOLT_3) || ( VFAS_SOURCE == ADS_VOLT_4)) ) 
 #error When defined, FUEL_SOURCE must be one of following values VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#endif
#if defined( A3_SOURCE ) && ( !  ( ( A3_SOURCE == VOLT_1) || ( A3_SOURCE == VOLT_2) || ( A3_SOURCE == VOLT_3) || ( A3_SOURCE == VOLT_4) || ( A3_SOURCE == VOLT_5) || ( A3_SOURCE == VOLT_6) || ( A3_SOURCE == ADS_VOLT_1) || ( A3_SOURCE == ADS_VOLT_2) || ( A3_SOURCE == ADS_VOLT_3) || ( A3_SOURCE == ADS_VOLT_4) ) ) 
 #error When defined, A3_SOURCE must be one of following values VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#endif
#if defined( A4_SOURCE ) && ( !  ( ( A4_SOURCE == VOLT_1) || ( A4_SOURCE == VOLT_2) || ( A4_SOURCE == VOLT_3) || ( A4_SOURCE == VOLT_4) || ( A4_SOURCE == VOLT_5) || ( A4_SOURCE == VOLT_6) || ( A3_SOURCE == ADS_VOLT_1) || ( A3_SOURCE == ADS_VOLT_2) || ( A3_SOURCE == ADS_VOLT_3) || ( A3_SOURCE == ADS_VOLT_4) ) ) 
 #error When defined, A4_SOURCE must be one of following values VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#endif
#if defined( ACCX_SOURCE ) && ( !  ( ( ACCX_SOURCE == VOLT_1) || ( ACCX_SOURCE == VOLT_2) || ( ACCX_SOURCE == VOLT_3) || ( ACCX_SOURCE == VOLT_4) || ( ACCX_SOURCE == VOLT_5) || ( ACCX_SOURCE == VOLT_6) \
    || ( ACCX_SOURCE == ADS_VOLT_1) || ( ACCX_SOURCE == ADS_VOLT_2) || ( ACCX_SOURCE == ADS_VOLT_3) || ( ACCX_SOURCE == ADS_VOLT_4) \
    || ( ACCX_SOURCE == TEST_1) || ( ACCX_SOURCE == TEST_2) || ( ACCX_SOURCE == TEST_3) || ( ACCX_SOURCE == GLIDER_RATIO) || ( ACCX_SOURCE == SECONDS_SINCE_T0) || ( ACCX_SOURCE == AVERAGE_VSPEED_SINCE_TO) || ( ACCX_SOURCE == PITCH) || ( ACCX_SOURCE == ROLL) || ( ACCX_SOURCE == YAW) ) )
 #error When defined, ACCX_SOURCE must be one of following values TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4, PITCH, ROLL, YAW
#endif
#if defined( ACCY_SOURCE ) && ( !  ( ( ACCY_SOURCE == VOLT_1) || ( ACCY_SOURCE == VOLT_2) || ( ACCY_SOURCE == VOLT_3) || ( ACCY_SOURCE == VOLT_4) || ( ACCY_SOURCE == VOLT_5) || ( ACCY_SOURCE == VOLT_6) \
    || ( ACCY_SOURCE == ADS_VOLT_1) || ( ACCY_SOURCE == ADS_VOLT_2) || ( ACCY_SOURCE == ADS_VOLT_3) || ( ACCY_SOURCE == ADS_VOLT_4) \
    || ( ACCY_SOURCE == TEST_1) || ( ACCY_SOURCE == TEST_2) || ( ACCY_SOURCE == TEST_3) || ( ACCY_SOURCE == GLIDER_RATIO) || ( ACCY_SOURCE == SECONDS_SINCE_T0) || ( ACCY_SOURCE == AVERAGE_VSPEED_SINCE_TO) || ( ACCY_SOURCE == PITCH) || ( ACCY_SOURCE == ROLL) || ( ACCY_SOURCE == YAW) ) )
 #error When defined, ACCY_SOURCE must be one of following values TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4, PITCH, ROLL, YAW
#endif
#if defined( ACCZ_SOURCE ) && ( !  ( ( ACCZ_SOURCE == VOLT_1) || ( ACCZ_SOURCE == VOLT_2) || ( ACCZ_SOURCE == VOLT_3) || ( ACCZ_SOURCE == VOLT_4) || ( ACCZ_SOURCE == VOLT_5) || ( ACCZ_SOURCE == VOLT_6) \
    || ( ACCZ_SOURCE == ADS_VOLT_1) || ( ACCZ_SOURCE == ADS_VOLT_2) || ( ACCZ_SOURCE == ADS_VOLT_3) || ( ACCZ_SOURCE == ADS_VOLT_4) \
    || ( ACCZ_SOURCE == TEST_1) || ( ACCZ_SOURCE == TEST_2) || ( ACCZ_SOURCE == TEST_3) || ( ACCZ_SOURCE == GLIDER_RATIO) || ( ACCZ_SOURCE == SECONDS_SINCE_T0) || ( ACCZ_SOURCE == AVERAGE_VSPEED_SINCE_TO) || ( ACCZ_SOURCE == PITCH) || ( ACCZ_SOURCE == ROLL) || ( ACCZ_SOURCE == YAW) ) )
 #error When defined, ACCZ_SOURCE must be one of following values TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4, PITCH, ROLL, YAW
#endif
#if defined( T1_SOURCE ) && ( !  ( ( T1_SOURCE == PPM) || ( T1_SOURCE == GLIDER_RATIO) || ( T1_SOURCE == SECONDS_SINCE_T0) || ( T1_SOURCE == AVERAGE_VSPEED_SINCE_TO) || ( T1_SOURCE == SENSITIVITY) \
    || ( T1_SOURCE == VOLT_1) || ( T1_SOURCE == VOLT_2) || ( T1_SOURCE == VOLT_3) || ( T1_SOURCE == VOLT_4) || ( T1_SOURCE == VOLT_5) || ( T1_SOURCE == VOLT_6) \
    || ( T1_SOURCE == ADS_VOLT_1) || ( T1_SOURCE == ADS_VOLT_2) || ( T1_SOURCE == ADS_VOLT_3) || ( T1_SOURCE == ADS_VOLT_4) \
    || ( T1_SOURCE == TEST_1) || ( T1_SOURCE == TEST_2) || ( T1_SOURCE == TEST_3) ) )
 #error When defined, T1_SOURCE must be one of following values TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4, 
#endif
#if defined( T2_SOURCE ) && ( !  ( ( T2_SOURCE == PPM) || ( T2_SOURCE == GLIDER_RATIO) || ( T2_SOURCE == SECONDS_SINCE_T0) || ( T2_SOURCE == AVERAGE_VSPEED_SINCE_TO) || ( T2_SOURCE == SENSITIVITY) \
    || ( T2_SOURCE == VOLT_1) || ( T2_SOURCE == VOLT_2) || ( T2_SOURCE == VOLT_3) || ( T2_SOURCE == VOLT_4) || ( T2_SOURCE == VOLT_5) || ( T2_SOURCE == VOLT_6)\
    || ( T2_SOURCE == ADS_VOLT_1) || ( T2_SOURCE == ADS_VOLT_2) || ( T2_SOURCE == ADS_VOLT_3) || ( T2_SOURCE == ADS_VOLT_4) \
    || ( T2_SOURCE == TEST_1) || ( T2_SOURCE == TEST_2) || ( T2_SOURCE == TEST_3) ) )
 #error When defined, T2_SOURCE must be one of following values TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4, 
#endif
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES) && (!defined(ADS_MEASURE))
  #error When AN_ADS1115_IS_CONNECTED is set on YES, ADS_MEASURE (in oXs_config_advanced.h) must be uncommented and contains 4 values
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && (!defined(PIN_VOLTAGE))
  #error When ARDUINO_MEASURES_VOLTAGES is set on YES, PIN_VOLTAGE must be uncommented and contains 6 values
#endif
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) && (!defined(PIN_CURRENTSENSOR))
  #error When ARDUINO_MEASURES_A_CURRENT is set on YES, PIN_CURRENTSENSOR must be uncommented and must specify the analog pin that is connected to the current sensor
#endif

#if defined ( ADS_AIRSPEED_BASED_ON ) && ( ! ( ( ADS_AIRSPEED_BASED_ON == ADS_VOLT_1) || ( ADS_AIRSPEED_BASED_ON == ADS_VOLT_2) || ( ADS_AIRSPEED_BASED_ON == ADS_VOLT_3) || ( ADS_AIRSPEED_BASED_ON == ADS_VOLT_4) ) )
 #error When defined, ADS_AIRSPEED_BASED_ON must be one of following values ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#endif
#if defined(GPS_REFRESH_RATE) && ( ! ( (GPS_REFRESH_RATE == 1) || (GPS_REFRESH_RATE == 5) || (GPS_REFRESH_RATE == 10) ))      
  #error When defined GPS_REFRESH_RATE must be 1, 5 or 10
#endif
  
#ifdef PIN_PPM
 #if PIN_PPM == 2
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp
	#define PPM_INT_MASK			0x03
	#define PPM_INT_EDGE			0x01
	#define PPM_PIN_HEX			  0x02
	#define PPM_INT_BIT		  	0x01
 #endif

 #if PIN_PPM == 3
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp
	#define PPM_INT_MASK			0x0C
	#define PPM_INT_EDGE			0x04
	#define PPM_PIN_HEX		  	0x04
	#define PPM_INT_BIT		  	0x02
 #endif
#endif


//*************** There is normally no reason changing the 2 next parameters
#define I2C_MS5611_Add           0x77 // 0x77 The I2C Address of the MS5611 breakout board 
                               // (normally 0x76 or 0x77 configured on the MS5611 module 
                               // via a solder pin or fixed)
                               
#define I2C_4525_Add        0x28 // 0x28 is the default I2C adress of a 4525DO sensor

#define I2C_ADS_Add 0x48 // default I2C address of ads1115 when addr pin is connected to ground

#define PIN_LED            13  // The Signal LED (default=13=onboard LED)


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
//static unsigned long extendedMicros ;

#ifdef DEBUG_BLINK  // this does not require that DEBUG is active.; Use only one of the blink 
   //DEBUG_BLINK_MAINLOOP
   //DEBUG_BLINK_CLIMBRATE
#endif

#ifdef DEBUG
//#define DEBUGCOMPENSATEDCLIMBRATE
//#define DEBUGOUTDATATOSERIAL
//#define DEBUGENTERLOOP
//#define DEBUG_ENTER_READSENSORS
//#define DEBUG_CALCULATE_FIELDS
//#define DEBUGSEQUENCE
//#define DEBUG_PPM_AVAILABLE_FROM_INTERRUPT
//#define DEBUGPPMVALUE
//#define DEBUGFORCEPPM
//#define DEBUG_SELECTED_VARIO
//#define DEBUG_VARIO_TIME
//#define DEBUG_VOLTAGE_TIME
//#define DEBUG_READ_SPORT
//#define DEBUG_SIMULATE_FLOW_SENSOR
//#define DEBUG_FLOW_SENSOR
//#define DEBUG_RPM
#endif


// ************ declare some functions being used ***************
int freeRam () ;
void checkButton() ;
void readSensors() ;
void Reset1SecButtonPress() ;
void Reset3SecButtonPress() ; 
void Reset10SecButtonPress() ;
void SaveToEEProm() ;
void LoadFromEEProm() ; 
void ProcessPPMSignal() ;
//unsigned int ReadPPM() ;
void ReadPPM() ;
bool checkFreeTime() ;
void setNewSequence() ;
void checkSequence() ;
void blinkLed( uint8_t blinkType ) ;
void checkFlowParam() ;

// *********** declare some variables *****************************
#ifdef VARIO
bool newVarioAvailable ;
struct ONE_MEASUREMENT mainVspeed ;
#endif
#ifdef VARIO2
bool newVarioAvailable2 ;
#endif

#if defined (VARIO) && ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined( ADS_AIRSPEED_BASED_ON ) ) )  
struct ONE_MEASUREMENT compensatedClimbRate ;
bool switchCompensatedClimbRateAvailable ;
float rawCompensatedClimbRate ; 

#endif

#if defined (VARIO) && ( defined (VARIO2) || ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) ) || defined (USE_6050) )
struct ONE_MEASUREMENT switchVSpeed ;
#endif

#if defined (VARIO) && defined (VARIO2)
struct ONE_MEASUREMENT averageVSpeed ;
float averageVSpeedFloat ;
#endif

#if defined  (VARIO) && defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC) && GLIDER_RATIO_CALCULATED_AFTER_X_SEC >= 1 
struct ONE_MEASUREMENT gliderRatio ;
struct ONE_MEASUREMENT secFromT0 ; // in 1/10 sec
struct ONE_MEASUREMENT averageVspeedSinceT0 ; //in cm/sec
void calculateAverages();
boolean gliderRatioPpmOn = false ;
#endif

#ifdef USE_6050
  KalmanFilter kalman ;
  float zTrack ;
  float vTrack ;
  struct ONE_MEASUREMENT vSpeedImu ;
  bool vTrackAvailable ;
  bool switchVTrackAvailable ;
  extern float linear_acceleration_x ;
  extern float linear_acceleration_y ;
  extern float linear_acceleration_z ;
  extern float world_linear_acceleration_z ;
  extern bool newImuAvailable;
  float altitudeToKalman ;
  int countAltitudeToKalman = 100 ;
  int32_t altitudeOffsetToKalman ;
  extern volatile uint32_t lastImuInterruptMillis ;
#ifdef USE_HMC5883 
  extern float magHeading ;
  extern boolean newMagHeading; 
#endif
  #ifdef DEBUG_KALMAN_TIME  
    int delayKalman[5] ;
  #endif
#endif // end of USE_6050
 


#if defined(ADS_AIRSPEED_BASED_ON) and (ADS_AIRSPEED_BASED_ON >= ADS_VOLT1) and (ADS_AIRSPEED_BASED_ON <= ADS_VOLT_4)
  extern float ads_sumDifPressureAdc_0 ;
  extern uint8_t ads_cntDifPressureAdc_0 ;
#endif

uint16_t ppmus ; // duration of ppm in usec
int prevPpm ; //^previous ppm
struct ONE_MEASUREMENT ppm ; // duration of pulse in range -100 / + 100 ; can exceed those limits
struct ONE_MEASUREMENT newPpm ; // keep the ppm value received via sport before it is processed in other part of the code

#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)  // we will use interrupt PCINT0 
  volatile uint16_t flowMeterCnt ;            // counter of pin change connected to the flow sensor (increased in the interrupt. reset every X sec when flow is processed)
  float currentFlow  ;                     // count the consumed ml/min during the last x sec
  float consumedML   ;                     // total of consumed ml since the last reset (or restart if reset is not activated); can be saved in EEPROM
  struct ONE_MEASUREMENT actualFlow ;             // in ml/min
  struct ONE_MEASUREMENT remainingFuelML ;         // in ml
  struct ONE_MEASUREMENT fuelPercent ;             // in % of tank capacity
  boolean newFlowAvailable = false ;
  int16_t flowParam[8] =  { INIT_FLOW_PARAM } ;                      // table that contains the parameters to correct the ml/pulse depending on the flow ; can be loaded by SPORT in EEPROM; can also be defined in a parameter 
  uint16_t tankCapacity =  TANK_CAPACITY ;     // capacity of fuel tank
  uint16_t consumedMLEeprom   ;                 //Last value saved in eeprom
  uint16_t tankCapacityEeprom ;                 //Last value saved in eeprom
  int16_t flowParamEeprom[8]  ;                 //Last value saved in eeprom
#endif



#ifdef SEQUENCE_OUTPUTS
#ifdef SEQUENCE_m100
  uint8_t sequence_m100 [] = { SEQUENCE_m100 } ;
#else
  uint8_t sequence_m100 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m75
  uint8_t sequence_m75 [] = { SEQUENCE_m75 } ;
#else
  uint8_t sequence_m75 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m50
  uint8_t sequence_m50 [] = { SEQUENCE_m50 } ;
#else
  uint8_t sequence_m50 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m25
  uint8_t sequence_m25 [] = { SEQUENCE_m25 } ;
#else
  uint8_t sequence_m25 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_0
  uint8_t sequence_0 [] = { SEQUENCE_0 } ;
#else
  uint8_t sequence_0 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_25
  uint8_t sequence_25 [] = { SEQUENCE_25 } ;
#else
  uint8_t sequence_25 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_50
  uint8_t sequence_50 [] = { SEQUENCE_50 } ;
#else
  uint8_t sequence_50 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_75
  uint8_t sequence_75 [] = { SEQUENCE_75 } ;
#else
  uint8_t sequence_75 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_100
  uint8_t sequence_100 [] = { SEQUENCE_100 } ;
#else
  uint8_t sequence_100 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_LOW
  uint8_t sequence_low [] = { SEQUENCE_LOW } ;
#else
  uint8_t sequence_low [2] = { 0 , 0 } ;
#endif


    uint16_t sequenceMaxNumber[10] ;  // this array contains the number of events of each sequence
    uint8_t * sequencePointer[10] ; // this is an array of pointers; each pointer refers to the first element of a 2 dim array.
    
    uint8_t *seqRef  ;  // seqRef contains a pointer to the first item of the selected sequence array
    uint16_t seqMax ; // number of sequences in the selected array
    uint8_t seqState ; // say if the sequence is starting (0), running (1) or stopped (2)
    uint16_t seqStep ;  // says the current step in the sequence , first step = 0 ;
    uint8_t sequenceOutputs = SEQUENCE_OUTPUTS & 0b00111111 ; // Mask to define which pin of portB are outputs
#ifdef SEQUENCE_UNIT
    uint16_t sequenceUnit = SEQUENCE_UNIT * 10 ; 
#else
    uint16_t sequenceUnit = 10 ; 
#endif // SEQUENCE_UNIT
#endif // end of SEQUENCE_OUTPUTS

int8_t prevPpmMain = -100 ; // this value is unusual; so it will forced a change at first call
bool lowVoltage = false ;
bool prevLowVoltage = false ;
//uint32_t currentLoopMillis ;

volatile bool RpmSet  ;
volatile uint16_t RpmValue ;
unsigned long lastRpmMillis ;
#if defined (MEASURE_RPM)
  struct ONE_MEASUREMENT sport_rpm ;
#endif

int PWRValue; // calculation field for Vertical speed on PWR
unsigned long lastMillisPWR ;

float actualPressure  ; // default pressure in pascal; to actualise if vario exist; is used in airspeed calculation.
int sensitivityPpmMapped ;
int compensationPpmMapped ;
struct ONE_MEASUREMENT test1 ; // used in order to test the transmission of any value
struct ONE_MEASUREMENT test2 ; // used in order to test the transmission of any value
struct ONE_MEASUREMENT test3 ; // used in order to test the transmission of any value
//int32_t test1Value ;// used in order to test the transmission of any value
//bool test1ValueAvailable ;
//int32_t test2Value ;// used in order to test the transmission of any value
//bool test2ValueAvailable ;
//int32_t test3Value ;// used in order to test the transmission of any value
//bool test3ValueAvailable ;
#ifdef VARIO_PRIMARY
uint8_t selectedVario = VARIO_PRIMARY ; // identify the vario to be used when switch vario with PPM is active (1 = first MS5611) 
#endif

// to read SPORT (for Frsky protocol
extern uint8_t  volatile TxData[8] ;
extern uint8_t  volatile TxDataIdx ;


//******************* Create instances of the used classes ******************************************
#ifdef VARIO
  #ifdef SENSOR_IS_BMP180
      #ifdef DEBUG  
        OXS_BMP180 oXs_MS5611 = OXS_BMP180(Serial);
      #else
        OXS_BMP180 oXs_MS5611 = OXS_BMP180();
      #endif  //DEBUG
  #elif defined(SENSOR_IS_BMP280 )
      #ifdef DEBUG  
        OXS_BMP280 oXs_MS5611 = OXS_BMP280(Serial);
      #else
        OXS_BMP280 oXs_MS5611 = OXS_BMP280();
      #endif  //DEBUG
  #else // not a BMP180 or BMP280
      #ifdef DEBUG  
        OXS_MS5611 oXs_MS5611(I2C_MS5611_Add,Serial);
      #else
        OXS_MS5611 oXs_MS5611(I2C_MS5611_Add);
      #endif  //DEBUG
  #endif // BMP180 or MS5611 sensor
#endif // VARIO


#ifdef VARIO2
  #ifdef DEBUG  
    OXS_MS5611 oXs_MS5611_2(I2C_MS5611_Add - 1,Serial);
  #else
    OXS_MS5611 oXs_MS5611_2(I2C_MS5611_Add - 1);
  #endif  //DEBUG
#endif

#ifdef AIRSPEED // differential pressure
  #ifdef DEBUG  
    OXS_4525 oXs_4525(I2C_4525_Add ,Serial);
  #else
    OXS_4525 oXs_4525(I2C_4525_Add);
  #endif  //DEBUG
#endif

#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES)
  #ifdef DEBUG  
    OXS_VOLTAGE oXs_Voltage(Serial);
  #else
    OXS_VOLTAGE oXs_Voltage(0);
  #endif  //DEBUG
#endif

#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  #ifdef DEBUG  
    OXS_CURRENT oXs_Current(PIN_CURRENTSENSOR,Serial);
  #else
    OXS_CURRENT oXs_Current(PIN_CURRENTSENSOR);
  #endif //DEBUG
#endif

#ifdef GPS_INSTALLED
  #ifdef DEBUG
    OXS_GPS oXs_Gps(Serial);
  #else
    OXS_GPS oXs_Gps(0);
  #endif //DEBUG
#endif

#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE)
  #ifdef DEBUG
    OXS_ADS1115 oXs_ads1115(  I2C_ADS_Add , Serial);
  #else
    OXS_ADS1115 oXs_ads1115( I2C_ADS_Add );
  #endif //DEBUG
#endif


//Create a class used for telemetry ;content of class depends on the selected protocol (managed via #ifdef in different files)
#ifdef DEBUG  
  OXS_OUT oXs_Out(PIN_SERIALTX,Serial);
#else
  OXS_OUT oXs_Out(PIN_SERIALTX);
#endif
// *********************************** End of create objects *************************************************

                            // Mike I do not understand this instruction; could you explain
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))


//******************************************* Setup () *******************************************************

void setup(){
// set up the UART speed (38400 if GPS installed else 115200)
#ifdef GPS_INSTALLED
  Serial.begin(38400); // when GPS is used, baudrate is reduced because main loop must have the time to read the received char.
#endif
#ifdef DEBUG 
#ifndef GPS_INSTALLED
  Serial.begin(115200L); // when GPS is not used, baudrate can be 115200
#endif
  Serial.println(F("openXsensor starting.."));
  Serial.print(F(" milli="));  
  Serial.println(millis());
  Serial.print(F("freeRam="));  
  Serial.println(freeRam());
#endif 

// set up pins
#ifdef DEBUG_SETUP_PIN 
    pinMode(DEBUG_SETUP_PIN, OUTPUT); // Set the start signal to high to say that set up start
    digitalWrite(DEBUG_SETUP_PIN, HIGH);
#endif  

  #ifdef DEBUG_SPORT_PIN 
    pinMode(DEBUG_SPORT_PIN, OUTPUT); // Set the pulse used during SPORT detection to LOW because detection is not yet started
    digitalWrite(DEBUG_SPORT_PIN, LOW);
#endif  

#ifdef DEBUG_BLINK
  pinMode(PIN_LED, OUTPUT); // The signal LED (used for the function debug loop)
#endif

#ifdef PIN_PUSHBUTTON  
  pinMode(PIN_PUSHBUTTON, INPUT_PULLUP);
#endif

  pinMode(PIN_LED, OUTPUT); // The signal LED (used for the function push button)

//  sensitivityPpmMapped = 0 ;
  compensationPpmMapped = 100 ;
//  test1.value = 0 ;
//  test2.value = 0 ;
//  test3.value = 0 ;
//  test1.available = false ;
//  test2.available = false ;
//  test3.available = false ;
  actualPressure = 101325 ; // default pressure in pascal; to actualise if vario exist; is used in airspeed calcualtion.


  // ******** Invoke all setup methods and set reference
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES)
  oXs_Voltage.setupVoltage(); 
  oXs_Out.voltageData=&oXs_Voltage.voltageData; 
#endif

#ifdef VARIO
#ifdef DEBUG 
  Serial.println(F("vario setting up.."));
  delay(1000);
#endif 
  oXs_MS5611.setup();
#ifdef DEBUG 
  Serial.println(F("vario is up.."));
  delay(1000);
#endif 

  oXs_Out.varioData=&oXs_MS5611.varioData; 
  #ifdef PIN_ANALOG_VSPEED
    lastMillisPWR = 3500 ; // So we will wait for 3.5 sec before generating a Vertical speed on PWM
    analogWrite(PIN_ANALOG_VSPEED,255/5*1.6); // initialize the output pin 
  #endif
#endif // vario

#ifdef VARIO2
  oXs_MS5611_2.setup();
  oXs_Out.varioData_2=&oXs_MS5611_2.varioData; 
#endif // vario

#ifdef AIRSPEED
  oXs_4525.setup();
  oXs_Out.airSpeedData=&oXs_4525.airSpeedData; 
#endif // end AIRSPEED

#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  oXs_Current.setupCurrent( );
  oXs_Out.currentData=&oXs_Current.currentData;
#endif

#if defined (VARIO) &&  ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) )
  compensatedClimbRate.available = false;
//  compensatedClimbRate = 0;
#endif

#ifdef GPS_INSTALLED
  oXs_Gps.setupGps();
#endif

  oXs_Out.setup();

#ifdef USE_6050
    setupImu() ;
#endif

#if defined( USE_HMC5883 ) && defined (USE_6050)
  setup_hmc5883() ;  // set up magnetometer
#endif

#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE)
  oXs_ads1115.setup() ;
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
  oXs_Out.currentData=&oXs_ads1115.adsCurrentData;
#endif            
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_AIRSPEED_BASED_ON)
  oXs_Out.airSpeedData=&oXs_ads1115.adsAirSpeedData;
#endif            
#endif

#if defined (SAVE_TO_EEPROM ) and ( SAVE_TO_EEPROM == YES )
  LoadFromEEProm();
#endif

#ifdef PIN_PPM
//    ppmus = 0 ;
//    prevPpm = 0 ;
    ppm.available = false ;
#endif

#ifdef PPM_INTERRUPT      // PPM use INT0 (pin 2) or INT1 (pin 3) that are on port D
	PORTD |= PPM_PIN_HEX ;	// Pullup resistor
	DDRD &= ~PPM_PIN_HEX ;	// Input
	EICRA |= PPM_INT_MASK ;		// Interrupt on rising edge
	EIFR = PPM_INT_BIT ;			// Clear interrupt flag
	EIMSK |= PPM_INT_BIT ;		// Enable interrupt
#endif

#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)  // we will use interrupt PCINT0 
  PORTB |= 0X2 ;  // set pullup resistor on pin 9 (= PB1)
  DDRB &= ~ 0X2 ; // set pin as input on pin 9 (= PB1)
  PCMSK0 = 0X2 ; // prepare to allow pin change interrupt on only pin 9 ( = PB1)
  PCICR |= (1<<PCIE0)   ;     // enable pin change interrupt 0 so on pin from PB0 to PB7 = e.g. pins 8 to 13 to collect pin changes for flow sensor 
#endif

  RpmSet = false ;
//  RpmValue = 0 ;
//  RpmAvailable = false ;

//************************* set up of sequence outputs
#ifdef SEQUENCE_OUTPUTS
    sequenceMaxNumber[0] = sizeof(sequence_m100) ; // sizeof(sequence_m100[0]) ;
    sequenceMaxNumber[1] = sizeof(sequence_m75) ;// sizeof(sequence_m75[0]) ;
    sequenceMaxNumber[2] = sizeof(sequence_m50) ;// sizeof(sequence_m50[0]) ;
    sequenceMaxNumber[3] = sizeof(sequence_m25) ;// sizeof(sequence_m25[0]) ;
    sequenceMaxNumber[4] = sizeof(sequence_0) ;// sizeof(sequence_0[0]) ;
    sequenceMaxNumber[5] = sizeof(sequence_25) ;// sizeof(sequence_25[0]) ;
    sequenceMaxNumber[6] = sizeof(sequence_50) ;// sizeof(sequence_50[0]) ;
    sequenceMaxNumber[7] = sizeof(sequence_75) ;// sizeof(sequence_75[0]) ;
    sequenceMaxNumber[8] = sizeof(sequence_100) ;// sizeof(sequence_100[0]) ;
    sequenceMaxNumber[9] = sizeof(sequence_low) ;// sizeof(sequence_100[0]) ;
    sequencePointer[0] = &sequence_m100[0] ;
    sequencePointer[1] = &sequence_m75[0] ;
    sequencePointer[2] = &sequence_m50[0] ;
    sequencePointer[3] = &sequence_m25[0] ;
    sequencePointer[4] = &sequence_0[0] ;
    sequencePointer[5] = &sequence_25[0] ;
    sequencePointer[6] = &sequence_50[0] ;
    sequencePointer[7] = &sequence_75[0] ;
    sequencePointer[8] = &sequence_100[0] ;
    sequencePointer[9] = &sequence_low[0] ;     
#ifdef DEBUG
    seqRef = sequencePointer[0] ;  // seqTab contains pointers to the sequence array
    seqMax = sequenceMaxNumber[0] ;
    Serial.print(F("SeqRef="));  Serial.println( (uint16_t) seqRef) ;
    Serial.print(F("SeqMax="));  Serial.println( (uint16_t) seqMax) ;
    Serial.print(F("Sequence_m100="));    
    for (uint8_t idxDebugSeq = 0 ; idxDebugSeq < (seqMax) ; idxDebugSeq++ ) {
      Serial.print( *(seqRef + idxDebugSeq ) );
      Serial.print(F(" , "));
    }
    Serial.println(F(" "));
#endif    
    seqState = 2 ; // declare that sequence is stopped
    PORTB &= ~sequenceOutputs ; // set all output to LOW
    DDRB |= sequenceOutputs ; // set pin to output mode
    
//#ifdef DEBUGSEQUENCE
    ppm.value = -100 ; // fix the sequence to be used by default (e.g. when no PPM signal is present);  
    setNewSequence( ) ; 
//#endif
#endif //****************************** end of SEQUENCE_OUTPUTS

#ifdef DEBUG_SPORT_PIN 
    digitalWrite(DEBUG_SPORT_PIN, LOW);
//    pinMode(DEBUG_SPORT_PIN, INPUT); // Set the pin in input mode because it is not used anymore
#endif  
#ifdef DEBUG_SETUP_PIN 
    digitalWrite(DEBUG_SETUP_PIN, LOW); // Set the setup signal to low to say that set up is done
//    pinMode(DEBUG_SETUP_PIN, OUTPUT); 
#endif  

#ifdef DEBUG
  Serial.println(F("End of general set up"));
#endif

} // ******************** end of Setup *****************************************************


//*******************************************************************************************
//                                Main loop                                               ***
//*******************************************************************************************
void loop(){ 
/*
uint8_t flagMillis ;
static uint32_t lastLoop20Millis ;
static uint32_t lastLoop50Millis ;
static uint32_t lastLoop200Millis ;
static uint32_t lastLoop500Millis ;

#define BIT20MILLIS 1 ;
#define BIT50MILLIS 2 ;
#define BIT200MILLIS 4 ;
#define BIT500MILLIS 8 ;
*/
//currentLoopMillis = millis() ;
/*
if ( currentLoopMillis - lastLoop20Millis > 20 ) {
  lastLoop20Millis = currentLoopMillis ;
  flagMillis = BIT20MILLIS ;
}
if ( currentLoopMillis - lastLoop50Millis > 50 ) {
  lastLoop50Millis = currentLoopMillis ;
  flagMillis |= BIT50MILLIS ;
}
if ( currentLoopMillis - lastLoop200Millis > 200 ) {
  lastLoop200Millis = currentLoopMillis ;
  flagMillis |= BIT200MILLIS ;
}
if ( currentLoopMillis - lastLoop500Millis > 500 ) {
  lastLoop500Millis = currentLoopMillis ;
  flagMillis |= BIT500MILLIS ;
}
*/
#ifdef DEBUGENTERLOOP
  Serial.print(F("in loop="));  
  Serial.println(millis());
#endif 
#ifdef DEBUG_BLINK_MAINLOOP
    blinkLed(1) ;
#endif
  //extern volatile uint8_t state ; 
  //Serial.println(state) ;
    // Check if a button has been pressed
#ifdef PIN_PUSHBUTTON
#if defined (VARIO) || defined (VARIO2)
    if (checkFreeTime()) checkButton(); // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#else
    checkButton();
#endif    
#endif 

    // read all sensors
    readSensors(); 
#ifdef DEBUG_CALCULATE_FIELDS
  Serial.print("call calculateAllFields at") ; Serial.println(millis()) ;
#endif
    // Calculate all fields
    calculateAllFields(); 

#ifdef DEBUG_CALCULATE_FIELDS
  Serial.print("end of call calculateAllFields at") ; Serial.println(millis()) ;
#endif

    // prepare the telemetry data to be sent (nb: data are prepared but not sent)
    if ( millis() > 1500 ) oXs_Out.sendData(); 
   
// PPM Processing = Read the ppm Signal from receiver  or use the SPORT ppm value from readSensors and process it 
#if defined ( PIN_PPM ) || (  defined(PPM_VIA_SPORT) && ( (PROTOCOL  == FRSKY_SPORT) || (PROTOCOL == FRSKY_SPORT_HUB) ) )
#if defined (VARIO) || defined (VARIO2)
    if (checkFreeTime()) ProcessPPMSignal();
#else
    ProcessPPMSignal();
#endif      
#endif //PIN_PPM

// if analog vario, generate the PWR value
#if defined (VARIO) && defined(PIN_ANALOG_VSPEED)
    if (millis() > lastMillisPWR + 100) {
        if (checkFreeTime()) { // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
            PWRValue=map( (long) oXs_MS5611.varioData.climbRate.value ,ANALOG_VSPEED_MIN*100,ANALOG_VSPEED_MAX * 100,0,255/5*3.2);
            PWRValue=constrain(PWRValue, 0, 255/5*3.2 ) ;
            analogWrite(PIN_ANALOG_VSPEED,(int)PWRValue);
            lastMillisPWR = millis() ; 
        }
    }  
#endif // analog vario 

// if a sequence is set up
#ifdef SEQUENCE_OUTPUTS
#if defined( SEQUENCE_MIN_VOLT_6) || defined ( SEQUENCE_MIN_CELL ) 
  if ( (lowVoltage == true) && (prevLowVoltage == false) ) {
    prevLowVoltage = true ;
    ppm.value = 125 ; // fix the sequence to be used when voltage is low  ; it is sequence +125
    prevPpmMain = -10 ; // will force a new sequence because ppmMain will be different from prevPpmMain
    setNewSequence() ;
    #ifdef DEBUG
        Serial.println(F("Start sequence low voltage"));
    #endif
  }  else if ( (lowVoltage == false) && (prevLowVoltage == true) ) {
    prevLowVoltage = false ;
    prevPpmMain = -10 ; //will force a new sequence because ppmMain will be different from prevPpmMain
#ifdef PIN_PPM
    if ( ppmus == 0) ppm.value = -100 ; // force ppm to -100 (default) when no ppm signal is received ,  else it will use the current ppm value
#else 
    ppm.value = -100 ; // if ppm is not configure force to go back to sequence -100 (= default);
#endif  // end PIN_PPM def or not
    setNewSequence() ; 
    #ifdef DEBUG
        Serial.println(F("End sequence low voltage"));
    #endif
  }  
#endif // end (defined( SEQUENCE_MIN_VOLT_6) || defined ( SEQUENCE_MIN_CELL ) 
  checkSequence() ; // check if outputs have to be modified because time expired
#endif  // SEQUENCE_OUTPUTS

// Save Persistant Data To EEProm every 10 seconds
#if defined (SAVE_TO_EEPROM ) and ( SAVE_TO_EEPROM == YES )
  static unsigned long LastEEPromMs=millis();
  if (millis() > LastEEPromMs+10000){ 
    LastEEPromMs=millis();
    SaveToEEProm();
  }
#endif
}          // ****************   end of main loop *************************************






//**********************************************************************************************************
//***                                            Read all the sensors / Inputs                          ****
//**********************************************************************************************************
extern uint16_t i2cPressureError ;
extern uint16_t i2cTemperatureError ;
extern uint16_t i2cReadCount ;

void readSensors() {  
   
#ifdef AIRSPEED
  oXs_4525.readSensor(); // Read a first time the differential pressure on 4525DO, calculate airspeed; note airspeed is read a second time in the loop in order to reduce response time
#endif

#ifdef VARIO
#ifdef DEBUG_ENTER_READSENSORS
  Serial.print( "read baro 1 at ") ; Serial.println(millis()); 
#endif

  newVarioAvailable = oXs_MS5611.readSensor(); // Read pressure & temperature on MS5611, calculate Altitude and vertical speed; 
  if ( oXs_MS5611.varioData.absoluteAlt.available == true and oXs_MS5611.varioData.rawPressure > 100000.0f ) actualPressure = oXs_MS5611.varioData.rawPressure / 10000.0 ; // this value can be used when calculating the Airspeed
#endif

#ifdef VARIO2
#ifdef DEBUG_ENTER_READSENSORS
  Serial.print( "read baro 2 at ") ; Serial.println(millis()); 
#endif

  newVarioAvailable2 = oXs_MS5611_2.readSensor(); // Read pressure & temperature on MS5611, calculate Altitude and vertical speed
#endif

#ifdef AIRSPEED
  oXs_4525.readSensor(); // Read again the sensor in order to reduce response time/noise
#endif 

#ifdef USE_6050
    newImuAvailable = read6050() ;
#endif // USE_6050

#if defined( USE_HMC5883 ) && defined (USE_6050)
    read_hmc5883() ;
#endif

#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES)
    oXs_Voltage.readSensor();    // read voltage only if there enough time to avoid delaying vario reading; It takes about 750 usec to go through the read sensor.
#endif   // end voltage

#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
    oXs_Current.readSensor() ; // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#endif             // End defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)

#ifdef GPS_INSTALLED
#ifdef DEBUG_ENTER_READSENSORS
  Serial.print( "read gps at ") ; Serial.println(millis()); 
#endif

    oXs_Gps.readGps() ; // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#endif             // End GPS_INSTALLED

#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE)
    if( oXs_ads1115.readSensor() ) { // return true when a new average is available; it means that the new value has to be stored/processed.
    }
#endif             // End ADS_MEASURE


#ifdef MEASURE_RPM
  if (millis() > ( lastRpmMillis + 200) ){  // allow transmission of RPM only once every 200 msec
        if (RpmSet == true) {               // rpm is set 
            RpmSet = false ;
        } else {
            RpmValue = 0 ;
        }
        sport_rpm.value = RpmValue ;
        sport_rpm.available = true ;    
        lastRpmMillis = millis() ;
#ifdef DEBUG_RPM
       Serial.print( "RPM ") ; Serial.println(sport_rpm.value); 
#endif       
  }      
#endif

#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)  
    processFlowMeterCnt () ;
#endif 


#if ( defined ( PPM_VIA_SPORT ) || ( defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES) ) ) && defined (PROTOCOL ) && ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) ) // if it is allowed to read SPORT
    uint8_t oReg = SREG ; // save status register
    cli() ;
    if ( TxDataIdx == 8 ) {
      TxDataIdx++ ; // Increase the number of data in order to avoid to read twice
      uint16_t txField = ( TxData[2] << 8 ) + TxData[1] ;                                                      // id of the field being received
      uint32_t txValue = ( ( ( ( (TxData[6] << 8) + TxData[5] ) << 8 ) + TxData[4] ) << 8 ) + TxData[3] ;      // value of the field being received
      SREG = oReg ;  // restore the status register
#ifdef DEBUG_READ_SPORT
      Serial.print("At: ") ; Serial.print(millis()) ;
      Serial.print(" "); Serial.print(TxData[0], HEX);  
      Serial.print(" "); Serial.print(TxData[1], HEX);  
      Serial.print(" "); Serial.print(TxData[2], HEX);  
      Serial.print(" "); Serial.print(TxData[3], HEX);  
      Serial.print(" "); Serial.print(TxData[4], HEX);  
      Serial.print(" "); Serial.print(TxData[5], HEX);  
      Serial.print(" "); Serial.print(TxData[6], HEX);
      Serial.print(" "); Serial.print(TxData[7], HEX);
      Serial.println(" ");    
#endif      
#if  ( defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES) )
      if (  txField <= TX_FIELD_FLOW_CORR_4)  {  // save values for flow meter parameter in memory
        flowParam[txField] = txValue ;
      } else if (  txField ==  TX_FIELD_TANK_CAPACITY) {  // save values for capacity in memory
        tankCapacity = txValue ;                          // note: values will be saved in eeprom in EEPROM function
      } else if (  txField ==  TX_FIELD_RESET_FUEL ) {
        consumedML = 0 ;                                  // reset consumption                                               
      }
#endif
#if  defined ( PPM_VIA_SPORT ) &&   ( ( PROTOCOL  == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )   
      if ( txField == TX_FIELD_PPM )  {                   // save the value for ppm 
          newPpm.value =  ((int32_t) txValue) * 100 / 1024 ;
          newPpm.available = true ;
      }   
#endif
    } else {
      SREG = oReg ;  // restore the status register      
    }
#endif
 
}                  // ************** end of readSensors ********************************************


//#define ALTITUDE_AVAILABLE_BIT 1
//#define VSPEED_AVAILABLE_BIT 2 
// ********************************* Calculate all fields that could be sent
void calculateAllFields () {

// compensated Vpeed based on MS4525
#if defined(VARIO) && ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) ) ) 
    if ( newVarioAvailable ) calculateDte() ; 
#endif 

// average Vpeed
#if defined (VARIO) &&  defined (VARIO2)
  if( (newVarioAvailable ) || ( newVarioAvailable2 ) ) {
    averageVSpeedFloat = ( oXs_MS5611.varioData.climbRateFloat + oXs_MS5611_2.varioData.climbRateFloat ) / 2 ;
    if ( abs((int32_t)  averageVSpeedFloat - averageVSpeed.value) > VARIOHYSTERESIS ) {
          averageVSpeed.value = (int32_t)  averageVSpeedFloat ;    // update only when difference is greater than hysteresis
    }    
    averageVSpeed.available = true ;
  }  
#endif  

// calculate vSpeedImu & vSpeedImuAvailable 
#if defined (VARIO) && defined (USE_6050)

        if (newImuAvailable) { // newImuAvailable says that a new world_linear_acceleration is available (flag has been returned by read6050()
          newImuAvailable = false ;   // reset the flag saying a new world_linear_acceleration is available
         
  #ifdef DEBUG_KALMAN
            unsigned long beginKalman = micros();
  #endif  
            if ( countAltitudeToKalman != 0) {                                   // this calculate the initial altitude
                if( oXs_MS5611.varioData.rawAltitude != 0) {
                  countAltitudeToKalman-- ;
                  altitudeOffsetToKalman = oXs_MS5611.varioData.rawAltitude ;
                }        
            }
            altitudeToKalman = (oXs_MS5611.varioData.rawAltitude - altitudeOffsetToKalman ) / 100 ; // convert from * 100cm to cm
            kalman.Update((float) altitudeToKalman  , world_linear_acceleration_z ,  &zTrack, &vTrack);  // Altitude and acceleration are in cm
            vSpeedImu.value = vTrack ;
            vSpeedImu.available = true ;
            switchVTrackAvailable = true ;

  //#define FILL_TEST_3_WITH_EXPECTED_ALT    // force a calculation of an expected altitude x seconds in the future
  #ifdef FILL_TEST_3_WITH_EXPECTED_ALT
            #define EXPECTED_AT_SEC 0.2        // number of sec used for the projection; can have decimals; NB: expected alt = Alt at time 0 + Vspeed at time 0 * seconds  + 0.5 * Accz at time 0 * seconds * seconds
            test3.value = ( zTrack - oXs_MS5611.varioData.altOffset ) + ( vTrack *  EXPECTED_AT_SEC ) + ( 0.5 *  world_linear_acceleration_z * EXPECTED_AT_SEC * EXPECTED_AT_SEC ) ;
            test3.available = true ; 
  #endif



            
  //#define FILL_TEST_1_2_3_WITH_LINEAR_ACC
  #ifdef FILL_TEST_1_2_3_WITH_LINEAR_ACC                                                  
            test1.value = linear_acceleration_x * 981 ; 
            test1.available = true ; 
            test2.value =  linear_acceleration_y * 981; 
            test2.available = true ; 
            test3.value = linear_acceleration_Z * 981; 
            test3.available = true ; 
  #endif

  #ifdef DEBUG_KALMAN_TIME
///           Serial.print(F("delay Kal ")) ; Serial.print( millis() - beginKalman ) ;Serial.print(F(", "));
//            Serial.print( (int) world_linear_acceleration_z ) ; Serial.print(F(", "));Serial.print( (int) altitudeToKalman) ; Serial.print(F(", ")); Serial.print(oXs_MS5611.varioData.climbRate) ; Serial.print(F(", "));Serial.println(( int )vSpeedImu.value) ;
///           Serial.print( millis() ) ; Serial.print(F(", ")); Serial.print( (int)  ) ; Serial.print(F(", "));Serial.print( (int) world_linear_acceleration_z ) ; Serial.print(F(", "));Serial.print( (int) altitudeToKalman) ; Serial.print(F(", ")); Serial.print(oXs_MS5611.varioData.climbRate) ; Serial.print(F(", "));Serial.println(( int )vSpeedImu.value) ;
              Serial.print("delayK ") ; Serial.print( delayKalman[0] ) ; Serial.print(" ") ; Serial.print( delayKalman[1] ) ;
              Serial.print(" ") ; Serial.print( delayKalman[2] ) ; Serial.print(" ") ; Serial.print( delayKalman[3] ) ;Serial.print(" ") ; Serial.println( delayKalman[4] ) ;
#endif  
        } // end of newimuAvailable 
#endif


// calculate selected Vspeed based on ppm
#if defined (VARIO) && ( defined (VARIO2) || defined (AIRSPEED) || defined (USE_6050 ) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON)  ) ) && defined (VARIO_SECONDARY ) && defined( VARIO_PRIMARY )  && defined (PIN_PPM)
  if (( selectedVario == FIRST_BARO ) && ( newVarioAvailable ) )  {
      switchVSpeed.value = oXs_MS5611.varioData.climbRate.value ;
      switchVSpeed.available = true ;
  } 
  #if  defined (VARIO2)
  else if ( ( selectedVario == SECOND_BARO ) && ( newVarioAvailable2 )) {
      switchVSpeed.value = oXs_MS5611_2.varioData.climbRate.value ;
      switchVSpeed.available = true ;
  }
  else if ( ( selectedVario == AVERAGE_FIRST_SECOND ) && ( averageVSpeed.available == true )) {
      switchVSpeed.value = averageVSpeed.value ;
      switchVSpeed.available = true ;
  }
  #endif // end of VARIO2
  #if  defined (AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) )
  else if ( ( selectedVario == AIRSPEED_COMPENSATED ) && ( newVarioAvailable )) {
      switchVSpeed.value = compensatedClimbRate.value ;
      switchVSpeed.available = true ;  
   #if defined (SWITCH_VARIO_GET_PRIO)
      switchCompensatedClimbRateAvailable = true ; // avoid to reset the value on false in order to continue to send the same value as often as possible
   #else
      switchCompensatedClimbRateAvailable = false ; // this is the normal process in order to avoid sending twice the same data.
   #endif  // end  defined (SWITCH_VARIO_GET_PRIO)   
  } 
  #endif // end  defined (AIRSPEED) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) )
  #if  defined (USE_6050)
  else if ( ( selectedVario == BARO_AND_IMU ) && ( switchVTrackAvailable )) {
      switchVSpeed.value = vSpeedImu.value ;
      switchVSpeed.available = true ;
  }
  #endif  // end USE_6050
#endif // end  defined (VARIO) && ( defined (VARIO2) || defined (AIRSPEED) || defined (USE_6050 ) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) && defined (VARIO_SECONDARY ) && defined( VARIO_PRIMARY ) && defined (VARIO_SECONDARY) && defined (PIN_PPM)


// mainVSpeed (calculated based on the setup in config)
#if defined(VARIO) &&  ( (!defined(VSPEED_SOURCE)) || (defined(VSPEED_SOURCE) && (VSPEED_SOURCE == FIRST_BARO) ) )
    mainVspeed.value = oXs_MS5611.varioData.climbRate.value ;
    mainVspeed.available = oXs_MS5611.varioData.climbRate.available ;
#elif defined(VARIO) && defined(VARIO2) && (VSPEED_SOURCE == SECOND_BARO)
    mainVspeed.value = oXs_MS5611_2.varioData.climbRate.value ;
    mainVspeed.available = oXs_MS5611_2.varioData.climbRate.available ;
#elif defined(VARIO) && defined(VARIO2) && (VSPEED_SOURCE == AVERAGE_FIRST_SECOND)
    mainVspeed.value = averageVSpeed.value ;
        mainVspeed.available = averageVSpeed.available ;
#elif defined(VARIO) && ( defined(AIRSPEED) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) )  && (VSPEED_SOURCE == AIRSPEED_COMPENSATED)
    mainVspeed.value = compensatedClimbRate.value ;
    mainVspeed.available = compensatedClimbRate.available ;
#elif defined(VARIO) && defined(USE_6050) && (VSPEED_SOURCE == BARO_AND_IMU)
    mainVspeed.value = vSpeedImu.value ;
    mainVspeed.available = vSpeedImu.available ;
#elif defined(VARIO) && ( defined (VARIO2) || defined (AIRSPEED) || defined (USE_6050) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) )&& defined (VARIO_SECONDARY ) && defined( VARIO_PRIMARY )  && defined (PIN_PPM)  && (VSPEED_SOURCE == PPM_SELECTION)
    mainVspeed.value = switchVSpeed.value   ;
    mainVspeed.available = switchVSpeed.available   ;
#if defined (DEBUG_SELECTED_VARIO)
static uint32_t next_debug_vspeeds_millis ;
static boolean next_debug_vspeeds_flag = true ;
  if (millis() > next_debug_vspeeds_millis) {
    if (next_debug_vspeeds_flag) {
      Serial.println("vspeeds= first, compensated, switched , main");
      next_debug_vspeeds_flag = false ;
    } else {
      Serial.print("vspeeds= ");Serial.print(oXs_MS5611.varioData.climbRate.value); Serial.print(",");Serial.print(compensatedClimbRate.value); 
      Serial.print(",");Serial.print(switchVSpeed.value); Serial.print(",");Serial.print(mainVspeed.value); Serial.println(" ");
      next_debug_vspeeds_millis = millis() + 500 ;
    }
  }
#endif //  end DEBUG_SELECTED_VARIO
    
#endif


// calculate glider ratio and average vertical speed (based on first vario)
#if defined  (VARIO) && defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC) && GLIDER_RATIO_CALCULATED_AFTER_X_SEC >= 1
        calculateAverages();
#endif        

#if defined (VARIO) && ( defined (VARIO2) ) // fill test1 and test2 with Vspeed and relative Alt
//#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO
#ifdef FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO
    test1.value = oXs_MS5611_2.varioData.climbRate.value ;
    test1.available = oXs_MS5611_2.varioData.climbRate.available ;
    test2.value = oXs_MS5611_2.varioData.relativeAlt.value ;
    test2.available = oXs_MS5611_2.varioData.relativeAlt.available ;
#endif
#endif

//  fill test1 and test2 with DTE and PPM_COMPENSATION
//#define FILL_TEST_1_WITH_DTE
//#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION
#if defined(VARIO) && ( defined(AIRSPEED) || (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) ) )
#ifdef FILL_TEST_1_WITH_DTE
  test1.value = compensatedClimbRate.value ;
  test1.available = compensatedClimbRate.available ; 
#endif
#if  defined(FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION) && defined(PIN_PPM)
  static uint32_t lastPpmCompensationMillis ;
  uint32_t PpmCompensationMillis = millis() ;
  if ( ( PpmCompensationMillis - lastPpmCompensationMillis ) > 200 ) {
    test2.value =  compensationPpmMapped ; 
    test2.available = true ;
    lastPpmCompensationMillis = PpmCompensationMillis ;
  }
#endif
#endif 

#if defined (VARIO) &&  defined (USE_6050)
//#define FILL_TEST_1_WITH_YAWRATE
#ifdef FILL_TEST_1_WITH_YAWRATE
static int32_t  previousYaw ;
static uint32_t previousYawRateMillis ;
 uint32_t currentMillis ;
        currentMillis = millis() ;
      if (currentMillis >= previousYawRateMillis + 200 ) {
        test1.value = (yaw.value - previousYaw) * 1000 / (int32_t) (currentMillis - previousYawRateMillis )  ; // calculate yaw rate in degree / sec
        test1.available = true ;
//#define DEBUGYAWRATE
#ifdef DEBUGYAWRATE
        //Serial.print(lastImuInterruptMillis) ;  Serial.print(" ") ; Serial.print( previousYawRateMillis ) ;Serial.print(" ") ; Serial.print( yaw.value ) ; Serial.print(" ") ; Serial.print( test1.value ) ; Serial.print(" ") ; Serial.println(oXs_MS5611.varioData.absoluteAlt.value) ;
         Serial.print( currentMillis ) ;Serial.print(" ") ; Serial.print( yaw.value ) ; Serial.print(" ") ; Serial.print( test1.value ) ; Serial.print(" ") ; Serial.println(oXs_MS5611.varioData.absoluteAlt.value) ;
#endif

        previousYaw = yaw.value ;
        previousYawRateMillis = currentMillis ;
     
      }      
//    test1.value = oXs_MS5611.varioData.climbRate.value ;
//    test1.available = oXs_MS5611.varioData.climbRate.available ;
//    test2.value = vSpeedImu.value ;
//    test2.available = vSpeedImu.available ;
#endif
#endif

//#define DEBUG_CHANGE_IN_TEST12
#if defined( DEBUG_CHANGE_IN_TEST12 )
      static uint32_t millisLastChangeTest ;
      if (millis() > millisLastChangeTest ) {
        millisLastChangeTest += 100 ;
        test1.value += 1 ;
        test1.available = true ;
        test2.value += 1 ;
        test2.available = true ;
      }  
#endif

//#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER
#if defined ( USE_HMC5883 )  && defined (USE_6050) && defined (FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER)
      if ( newMagHeading ) {
        newMagHeading = false ;
        test1.value = magHeading ;
        test1.available = true ; 
      }
#endif



#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)&& defined (FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION)
  if( newFlowAvailable ) {
    test1.value = actualFlow.value ; 
    test2.value = remainingFuelML.value ; 
    test3.value = fuelPercent.value ; 
    test1.available = test2.available = test3.available = true ;
    newFlowAvailable = false ;
  }  
#endif

//  test1.value = oXs_MS5611.varioData.absoluteAlt.value/10 ;
//  test1.available = true ;

//#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
//if ( currentData.consumedMilliAmps.available) {
// test1.value = currentData.consumedMilliAmps.value ;
// test1.available =  true ;
// currentData.consumedMilliAmps.available = false ;
//} 
//#endif

//#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
//if ( oXs_ads1115.adsCurrentData.MilliAmps.available) {
// test2.value = oXs_ads1115.adsCurrentData.MilliAmps.value ;
// test2.available =  true ;
// oXs_ads1115.adsCurrentData.MilliAmps.available = false ;
//} 
//if ( oXs_ads1115.adsCurrentData.consumedMilliAmps.available) {
// test3.value = oXs_ads1115.adsCurrentData.consumedMilliAmps.value ;
// test3.available =  true ;
// oXs_ads1115.adsCurrentData.consumedMilliAmps.available = false ;
//} 
//#endif

//#if ( defined(AIRSPEED) && (defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE) && defined (ADS_AIRSPEED_BASED_ON) )
//	if ( oXs_ads1115.adsAirSpeedData.airSpeed.available == true ) {
//     test1.value = oXs_ads1115.adsAirSpeedData.airSpeed.value ;
//	 test1.available = true ;
//	 oXs_ads1115.adsAirSpeedData.airSpeed.available = false ;
//	}
//#endif

} // end of calculate all fields



//***************** checkFreeTime ********* if there is at least 2000 usec before the next MS5611 read (in order to avoid delaying it 
bool checkFreeTime() { // return true if there is no vario or if the vario sensor must not be read within a short time.
                       // return false if a vario must be read within a short time   
#if defined (VARIO) || defined (VARIO2)
//        extendedMicros = micros() ;
  #ifdef VARIO
        return ( micros() - oXs_MS5611.varioData.lastCommandMicros < 7000 ) ;   // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)
  #else // only VARIO2
        return ( micros() - oXs_MS5611_2.varioData.lastCommandMicros < 7000 ) ;   // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)
  #endif // enf of at least one vario
#else // No vario at all
  return true ;
#endif  
}  // ******************************* end of checkFreeTime *****************************

// ********************************** Calculate dTE based on rawAltitude and differential pressure
#if defined (VARIO) && ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) ) ) 
#define SMOOTHING_DTE_MIN SENSITIVITY_MIN
#define SMOOTHING_DTE_MAX SENSITIVITY_MAX
#define SMOOTHING_DTE_MIN_AT SENSITIVITY_MIN_AT
#define SMOOTHING_DTE_MAX_AT SENSITIVITY_MAX_AT

void calculateDte () {  // is calculated about every 2O ms each time that an altitude is available
  static float rawTotalEnergy ;
  static float abs_deltaCompensatedClimbRate ;
  static float smoothingDteMin =  SMOOTHING_DTE_MIN ;
  static float expoSmoothDte_auto ;
  static float smoothCompensatedClimbRate ;
  static float rawCompensation ;
  static float totalEnergyLowPass ;
  static float totalEnergyHighPass ;

          // for 4525:
               //  difPressure (in PSI) = difPressureAdc * 0.0001525972 because 1 PSI = (8192 - 1638) = 6554 steps
               //  difPressure (Pa) =  difPressure (in PSI) * 6894.757f  = difPressureAdc * 6894.757 *  0.0001525972 = difPressureAdc * 1.0520
               // airspeed = sqr(2 * differential_pressure / air_density) ; air density = pressure  pa / (287.05 * (Temp celcius + 273.15))
               // so airspeed m/sec =sqr( 2 * 287.05 * differential_pressure pa * (temperature Celsius + 273.15) / pressure pa
               // total energy = (m * g * altitude) + (m * airspeed * airspeed / 2) => m * 9.81 * (altitude + airspeed * airspeed / 2 /9.81)
               // compensation (m/sec) = airspeed * airspeed / 2 / 9.81 =
               //                      = 2 * 287.05 * difPressureAdc * 1.0520  * (temperature Celsius + 273.15) / pressure pa /2 /9.81 (m/sec) = 30.78252803 * difPressureAdc * Temp(kelv) / Press (Pa)
               // compensation (cm/sec) =  3078.252803 * difPressureAdc * Temp(kelv) / Press (Pa)
          // for 7002
              // difPressure (Pa) = difPressureAdc * 0.061035156 because 2 kpa = 32768 steps ; so 1 step = 2000 / 32768 = 0.061035156
              // compensation (m/sec) = airspeed * airspeed / 2 / 9.81 =
              //                      = 2 * 287.05 * difPressureAdc * 0.061035156  * (temperature Celsius + 273.15) / pressure pa /2 /9.81 (m/sec) = 1.785947149 * difPressureAdc * Temp(kelv) / Press (Pa)
              // compensation (cm/sec) = 178.5947149 * difPressureAdc * Temp(kelv) / Press (Pa)
//#define DEBUG_DTE
#if defined  ( DEBUG_DTE ) && ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) )
                  static bool firstDteData = true ;
                  if ( firstDteData ) {
                          Serial.println(F("at , difPressADC_0 , cnt , rawAlt , rawComp , rawEnerg ")) ;
                        firstDteData = false ;
                  } 
                  Serial.print( millis() ); Serial.print(F(" , "));
                  Serial.print( ads_sumDifPressureAdc_0 ); Serial.print(F(" , "));
                  Serial.print( ads_cntDifPressureAdc_0); Serial.print(F(" , "));
#endif


#if defined ( AIRSPEED) // when 4525 is used
  rawCompensation = 3078.25 * oXs_4525.airSpeedData.difPressureAdc_zero * oXs_4525.airSpeedData.temperature4525  /  actualPressure    ; // 3078.25 = comp = 2 * 287.05 / 2 / 9.81 * 1.0520 * 100 * Temp / Pressure  
#else // when 7002 is used
    if ( ads_cntDifPressureAdc_0 > 0 ) {
        ads_sumDifPressureAdc_0 = ads_sumDifPressureAdc_0 / ads_cntDifPressureAdc_0 ; // we use the average of airspeed pressure when possible and we keep the average as first value for next loop
        ads_cntDifPressureAdc_0 = 1 ;  // so cnt is reset to 1 and not to 0
        rawCompensation = 178.5947149 * ads_sumDifPressureAdc_0 * ( 293 )   /  actualPressure    ; //  293 could be replaced by the temperature from mS5611  
    }
#endif   
  rawTotalEnergy = (oXs_MS5611.varioData.rawAltitude * 0.01) + rawCompensation * compensationPpmMapped * 0.0115; // 0.01 means 100% compensation but we add 15% because it seems that it is 15% undercompensated. 
  if (totalEnergyLowPass == 0) { 
    totalEnergyLowPass = totalEnergyHighPass = rawTotalEnergy ; 
  }
#if defined  ( DEBUG_DTE ) && ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) )
                  Serial.print( oXs_MS5611.varioData.rawAltitude * 0.01 ); Serial.print(F(" , ")); 
                  Serial.print( rawCompensation ); Serial.print(F(" , ")); 
                  Serial.print( rawTotalEnergy ); Serial.print(F(" , "));
                 
                  Serial.println(" ") ; 
                         
#endif

//  test1.value = rawCompensation; 
//  test1.available = true ; 
  
  //test2.value = rawTotalEnergy; 
  //test2.available = true ; 
  
  totalEnergyLowPass += 0.085 * ( rawTotalEnergy - totalEnergyLowPass) ;
  totalEnergyHighPass += 0.1 * ( rawTotalEnergy - totalEnergyHighPass) ;
  rawCompensatedClimbRate = ((totalEnergyHighPass - totalEnergyLowPass )  * 566667.0 ) /     oXs_MS5611.varioData.delaySmooth; // 0.566667 is the parameter to be used for 0.085 and 0.1 filtering if delay is in sec

//  test3.value = rawCompensatedClimbRate ; 
//  test3.available = true ; 
  abs_deltaCompensatedClimbRate =  abs(rawCompensatedClimbRate - smoothCompensatedClimbRate) ;
  if ( sensitivityPpmMapped  > 0) smoothingDteMin =   sensitivityPpmMapped  ; // a value of sensitivityPpmMapped = 50 becomes a smoothing factor 0.1
    if ( (abs_deltaCompensatedClimbRate <= SMOOTHING_DTE_MIN_AT) || (smoothingDteMin >= SMOOTHING_DTE_MAX ) ){
     expoSmoothDte_auto = smoothingDteMin ;  
  } else if (abs_deltaCompensatedClimbRate >= SMOOTHING_DTE_MAX_AT)  {
     expoSmoothDte_auto = SMOOTHING_DTE_MAX ; 
  } else {
     expoSmoothDte_auto = smoothingDteMin + ( SMOOTHING_DTE_MAX - smoothingDteMin ) * (abs_deltaCompensatedClimbRate - SMOOTHING_DTE_MIN_AT) / (SMOOTHING_DTE_MAX_AT - SMOOTHING_DTE_MIN_AT) ;
  }
  smoothCompensatedClimbRate += expoSmoothDte_auto * (  rawCompensatedClimbRate -  smoothCompensatedClimbRate ) * 0.001; 
  if ( abs( ((int32_t)  smoothCompensatedClimbRate) - compensatedClimbRate.value) > VARIOHYSTERESIS ) {
      compensatedClimbRate.value = smoothCompensatedClimbRate  ;
  } 
  compensatedClimbRate.available = true ; // allows SPORT protocol to transmit the value every 20 ms
  switchCompensatedClimbRateAvailable = true ; ; // inform readsensors() that a switchable vspeed is available
//  oXs_MS5611.varioData.altitudeAt20MsecAvailable = false ; // avoid to handel it each time.
 
#ifdef DEBUGCOMPENSATEDCLIMBRATE
  static bool firstCompensatedClimbRate = true ;
  if ( firstCompensatedClimbRate ) {
        Serial.println(F("at, altitude, difPressureADC_zero, rawCompensation , rawTotalEnergy , delaySmooth , rawCompensatedClimbRate , compensatedClimbRate ")) ;
        firstCompensatedClimbRate = false ;
  } else {
  Serial.print( micros() ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_MS5611.varioData.rawAltitude ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_4525.airSpeedData.difPressureAdc_zero ) ;Serial.print(F(" , ")) ;
  Serial.print( rawCompensation ) ;Serial.print(F(" , ")) ;
  Serial.print( rawTotalEnergy ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_MS5611.varioData.delaySmooth ) ;Serial.print(F(" , ")) ;
  Serial.print( rawCompensatedClimbRate ) ;Serial.print(F(" , ")) ;
  Serial.print( compensatedClimbRate.value) ;
  Serial.println(F(" ")) ; 
  } 
#endif    
   
} // end calculateDte  
#endif    // #if defined (VARIO) ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) ) )  
// ***************************** end calculate Dte ***********************************************


//****************************** Calculate averages and glider ratio ********************************************
#if defined (VARIO) && defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC ) && (GLIDER_RATIO_CALCULATED_AFTER_X_SEC >= 1 )  && defined ( PIN_PPM ) && defined (GLIDER_RATIO_ON_AT_PPM )
void calculateAverages( ){
        static int32_t altitudeAtT0 ; // in cm
        static int32_t distanceSinceT0 ; // in cm
//        static int32_t averageVspeedSinceT0 ; //in cm/sec
        static int16_t aSpeedAtT0 ;
//        static uint16_t secFromT0 ;  // in 1/10 sec
        static uint32_t millisAtT0 ;
        static unsigned long prevAverageAltMillis  ; // wait 5 sec before calculating those data ; // save when AverageAltitude has to be calculated
        int32_t altitudeDifference ;
        unsigned long currentGliderMillis ;
        bool aSpeedWithinTolerance = true ;
        static boolean previousGliderRatioPpmOn = false ;   
        currentGliderMillis = millis() ;
        if (  prevAverageAltMillis == 0 ) { 
          prevAverageAltMillis =  currentGliderMillis + 5000  ; 
          millisAtT0 = currentGliderMillis ;
        }
        if ( (uint16_t) (currentGliderMillis - prevAverageAltMillis) >   500 ) { // check on tolerance has to be done
             if ( ( gliderRatioPpmOn) && (previousGliderRatioPpmOn) ) {
              altitudeDifference = oXs_MS5611.varioData.absoluteAlt.value -altitudeAtT0  ; // in cm
              secFromT0.value =  ( currentGliderMillis - millisAtT0 ) / 100 ;            // in 1/10 of sec
              averageVspeedSinceT0.value = altitudeDifference * 10 / secFromT0.value  ;      // * 10 because secFromT0 is in 1/10 of sec
#ifdef AIRSPEED
                distanceSinceT0 += oXs_4525.airSpeedData.smoothAirSpeed / (1000 /  500) ;  // to adapt if delay is different.
#endif                
                if (  secFromT0.value >=  GLIDER_RATIO_CALCULATED_AFTER_X_SEC * 10 ) {         // *10 because secFromT0 is in 1/10 of sec 
#ifdef AIRSPEED
                    gliderRatio.value = distanceSinceT0  * -10 / altitudeDifference  ;        // when gliderRatio is > (50.0 *10) it it not realistic (*10 is done in order to add a decimal)
                    if ( gliderRatio.value > 500) gliderRatio.value = 0 ;                                                   // 
#endif                    
                    averageVspeedSinceT0.value = altitudeDifference * 10 / secFromT0.value  ;      // * 10 because secFromT0 is in 1/10 of sec
                }
              
            } else {
              altitudeAtT0 = oXs_MS5611.varioData.absoluteAlt.value   ;
              millisAtT0 = currentGliderMillis ;
              secFromT0.value = 0 ;
              averageVspeedSinceT0.value = 0 ; 
#ifdef AIRSPEED
                distanceSinceT0 = 0 ;
                gliderRatio.value = 0 ;
#endif                
            }  

            prevAverageAltMillis = currentGliderMillis  ; 
            gliderRatio.available = true;
            secFromT0.available = true ; 
            averageVspeedSinceT0.available = true ;
            previousGliderRatioPpmOn = gliderRatioPpmOn ; 
        }  // end check on 500 ms
}        // end Calculate averages





#else
#if defined  (VARIO) && defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC) && GLIDER_RATIO_CALCULATED_AFTER_X_SEC >= 1 
#if defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC) && GLIDER_RATIO_CALCULATED_AFTER_X_SEC < 1
#error  when defined, GLIDER_RATIO_CALCULATED_AFTER_X_SEC must be greater or equal to 1 (sec)
#endif
void calculateAverages( ){
        static int32_t altitudeAtT0 ; // in cm
        static int32_t distanceSinceT0 ; // in cm
//        static int32_t averageVspeedSinceT0 ; //in cm/sec
        static int16_t aSpeedAtT0 ;
//        static uint16_t secFromT0 ;  // in 1/10 sec
        static uint32_t millisAtT0 ;
        static unsigned long prevAverageAltMillis =  millis() + 5000 ; // wait 5 sec before calculating those data ; // save when AverageAltitude has to be calculated
        int32_t altitudeDifference ;
        unsigned long currentGliderMillis = millis() ;
        bool aSpeedWithinTolerance = true ;  

        if ( (uint16_t) (currentGliderMillis - prevAverageAltMillis) >   500 ) { // check on tolerance has to be done
            altitudeDifference = oXs_MS5611.varioData.absoluteAlt.value -altitudeAtT0  ; // in cm
            secFromT0.value =  ( currentGliderMillis - millisAtT0 ) / 100 ;            // in 1/10 of sec
#ifdef DEBUG_GLIDER8RATIO
            serial.print((F("secFromT0: ")); serial.println( secFromT0.value ) ;
#endif
#ifdef AIRSPEED
            if ( (aSpeedAtT0 > 300) && ( oXs_4525.airSpeedData.smoothAirSpeed > 300 ) ) {
                aSpeedWithinTolerance = ( (abs( oXs_4525.airSpeedData.smoothAirSpeed - aSpeedAtT0) * 100L ) / aSpeedAtT0 ) <= SPEED_TOLERANCE ;
            } else {
                aSpeedWithinTolerance = false ;
            }
#endif            
            if (  ( oXs_MS5611.varioData.climbRate.value <  VSPEED_MIN_TOLERANCE ) || ( oXs_MS5611.varioData.climbRate.value >  VSPEED_MAX_TOLERANCE ) \
                || ( altitudeDifference > -10 ) || ( aSpeedWithinTolerance == false ) ) {    // reset all when out of tolerance
                altitudeAtT0 = oXs_MS5611.varioData.absoluteAlt.value ;
#ifdef AIRSPEED                
                aSpeedAtT0 = oXs_4525.airSpeedData.smoothAirSpeed ;
                distanceSinceT0 = 0 ;
#endif                
                secFromT0.value = 0 ;
                millisAtT0 = currentGliderMillis ;
                averageVspeedSinceT0.value = 0 ;
                gliderRatio.value = 0 ;
#ifdef DEBUG_GLIDER8RATIO
            serial.println((F("Reset")); 
#endif

            } else {                                                      // within tolerance, calculate glider ratio and average sinking  
#ifdef AIRSPEED
                distanceSinceT0 += oXs_4525.airSpeedData.smoothAirSpeed / (1000 /  500) ;  // to adapt if delay is different.
#endif                
                if (  secFromT0.value >=  GLIDER_RATIO_CALCULATED_AFTER_X_SEC * 10 ) {         // *10 because secFromT0 is in 1/10 of sec 
#ifdef AIRSPEED
                    gliderRatio.value = distanceSinceT0  * -10 / altitudeDifference  ;        // when gliderRatio is > (50.0 *10) it it not realistic (*10 is done in order to add a decimal)
                    if ( gliderRatio.value > 500) gliderRatio.value = 0 ;                                                   // 
#endif                    
                    averageVspeedSinceT0.value = altitudeDifference * 10 / secFromT0.value  ;      // * 10 because secFromT0 is in 1/10 of sec
                }
             }
            prevAverageAltMillis += 500  ; 
            gliderRatio.available = true;
            secFromT0.available = true ; 
            averageVspeedSinceT0.available = true ; 
        }
}        
#endif
#endif // End of #if defined (VARIO) && defined (GLIDER_RATIO_BASED_ON_PPM )
//********end calculate glider ratio************************************************************************************************





#ifdef PIN_PUSHBUTTON   // // ****************** check button
void checkButton() {
  static int lastSensorVal=HIGH;
  static unsigned int buttonDownMs;
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(PIN_PUSHBUTTON);
  if (sensorVal == HIGH) {
    // button not pressed released
    digitalWrite(PIN_LED, LOW);
  }
  else {
    //button is currently being pressed down
    unsigned int buttonPressDuration=millis()-buttonDownMs;

    if( (buttonPressDuration>1000) and (buttonPressDuration<1200) )
      digitalWrite(PIN_LED, LOW); // Blink after 1 second

    else if( (buttonPressDuration>3000) and (buttonPressDuration<3200) )
      digitalWrite(PIN_LED, LOW); // Blink 1 after 3 second
    else if( (buttonPressDuration>3300) and (buttonPressDuration<3400) )
      digitalWrite(PIN_LED, LOW); // Blink 2 after 3 second

    else if( (buttonPressDuration>10000) and (buttonPressDuration<10200) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else if( (buttonPressDuration>10300) and (buttonPressDuration<10400) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else if( (buttonPressDuration>10500) and (buttonPressDuration<10600) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else digitalWrite(PIN_LED, HIGH);
  }
  if( (lastSensorVal==HIGH) && (sensorVal==LOW))
  { // Button has been pressed down
    buttonDownMs=millis();
  }
  if( (lastSensorVal==LOW) && (sensorVal==HIGH))
  { // Button has been released
    unsigned int buttonPressDuration=millis()-buttonDownMs;
    //Serial.print(F("Button Released after ms:"));
    //Serial.println(buttonPressDuration);
    // Do Something after certain times the button has been pressed for....
    if( (buttonPressDuration>=100) and (buttonPressDuration<3000) )
      Reset1SecButtonPress();
    else if( (buttonPressDuration>=3000) and (buttonPressDuration<8000) )
      Reset3SecButtonPress();
    //else if( (buttonPressDuration>=5000) and (buttonPressDuration<10000) )
    else if( (buttonPressDuration>=10000) and (buttonPressDuration<15000) )
      Reset10SecButtonPress();
  }

  lastSensorVal=sensorVal;
}  // End checkButton 

//ToDo: implement different reset actions on button press
void Reset1SecButtonPress()
{
#ifdef DEBUG
  Serial.println(F(" Reset 0.1-3 sec"));
#endif

#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  struct CURRENTDATA *cd = &oXs_Current.currentData ;
//  FORCE_INDIRECT(cd) ;
//  cd->maxMilliAmps=cd->milliAmps;
//  cd->minMilliAmps=cd->milliAmps;
#endif

//#ifdef VARIO
//  oXs_MS5611.resetValues() ;
//#endif

//#ifdef VARIO2
//  oXs_MS5611_2.resetValues() ;
//#endif

}

void Reset3SecButtonPress()
{
#ifdef DEBUG
  Serial.println(F(" Reset 3-5 sec"));
#endif
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  oXs_Current.resetValues();
#elif defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined (ADS_MEASURE)  && defined (ADS_CURRENT_BASED_ON)
  oXs_ads1115.floatConsumedMilliAmps = 0 ;
  oXs_ads1115.adsCurrentData.consumedMilliAmps.value = 0 ;
#endif
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)
  consumedML = 0 ;
#endif
}

void Reset10SecButtonPress()
{
#ifdef DEBUG
  Serial.println(F("Reset 5-10")) ;
#endif
}

#endif



#if defined (SAVE_TO_EEPROM ) and ( SAVE_TO_EEPROM == YES )
/****************************************/
/* SaveToEEProm => save persistant Data */
/****************************************/
void SaveToEEProm(){
  static uint8_t caseWriteEeprom = 0;
#define CASE_MAX_EEPROM 2  // to adapt if thee are more data to save.   
#ifdef DEBUG
  Serial.print(F("Saving to EEProm:"));    
  Serial.println(caseWriteEeprom);    
#endif
  switch (caseWriteEeprom ) {
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  case 0 :
    EEPROM_writeAnything( 0 , oXs_Current.currentData.consumedMilliAmps);
    break ;
#endif
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES) 
  case ( 1 ) : 
    if ( ((uint16_t) consumedML) != consumedMLEeprom ) {                            // write in EEPROM only if there is a change
      consumedMLEeprom = consumedML ;  
      EEPROM_writeAnything( 4 , consumedMLEeprom );
    }   
    break;  
  case ( 2 ) : 
    if ( tankCapacity != tankCapacityEeprom ) {                            // write in EEPROM only if there is a change
      tankCapacityEeprom = tankCapacity ; 
      EEPROM_writeAnything( 8 , tankCapacityEeprom ); 
    }
    for (uint8_t idxWrite = 0 ; idxWrite < 8 ; idxWrite++ ) { 
      if ( flowParam[idxWrite] != flowParamEeprom[idxWrite] ) {
        flowParamEeprom[idxWrite] = flowParam[idxWrite] ;
        EEPROM_writeAnything( 12 + (idxWrite * 4 ) , flowParamEeprom[idxWrite] ); 
      }
    }
    break;  
#endif  // end A_FLOW_SENSOR_IS_CONNECTED
  }     // end of switch
  caseWriteEeprom++;
  if(caseWriteEeprom > CASE_MAX_EEPROM) caseWriteEeprom = 0;

// to do : add values from ads1115 about current consumption; to do also in LoadFromEEprom()
  
} // end SaveToEEPom

/******************************************/
/* LoadFromEEProm => load persistant Data */
/******************************************/
void LoadFromEEProm(){
  // Load the last saved value
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  EEPROM_readAnything( 0 , oXs_Current.currentData.consumedMilliAmps);
  #ifdef DEBUG
    Serial.println(F("Restored consumed mA"));
  #endif  
#endif
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES ) 
    EEPROM_readAnything(4, consumedMLEeprom  ) + 4 ;
    consumedML = consumedMLEeprom  ; 
    EEPROM_readAnything( 8, tankCapacityEeprom  );
    tankCapacity = tankCapacityEeprom ;
    for (uint8_t idxRead = 0 ; idxRead < 8 ; idxRead++ ) {
        EEPROM_readAnything( 12 + ( idxRead * 4) , flowParamEeprom[idxRead] );
        flowParam[idxRead] = flowParamEeprom[idxRead] ;
    }
    checkFlowParam() ;
  #ifdef DEBUG  
    Serial.print(F("Restored consumed= ")); Serial.print(consumedMLEeprom) ; Serial.print(F("  Tank=")) ; Serial.print(tankCapacityEeprom) ; Serial.print(F(" Param=")) ;
    for (uint8_t idxRead = 0 ; idxRead < 8 ; idxRead++ ) {
      Serial.print(flowParamEeprom[idxRead]) ; Serial.print(" ") ;  
    }
   Serial.println(" ");
   
   Serial.print(F("          consumed= ")); Serial.print(consumedML) ; Serial.print(F("  Tank=")) ; Serial.print(tankCapacity) ; Serial.print(F(" Param=")) ;
    for (uint8_t idxRead = 0 ; idxRead < 8 ; idxRead++ ) {
      Serial.print(flowParam[idxRead]) ; Serial.print(" ") ;  
    }
   Serial.println(" ");
  #endif  
#endif
}
#endif //SAVE_TO_EEPROM


/***************************************************************/
/* ProcessPPMSignal => read PPM signal from receiver and       */
/*   use its value to adjust sensitivity and other parameters  */
/***************************************************************/
#if defined ( PIN_PPM ) || (  defined ( PPM_VIA_SPORT ) && ( (PROTOCOL  == FRSKY_SPORT) || (PROTOCOL  == FRSKY_SPORT_HUB) ) )
volatile uint16_t time1 ;
void ProcessPPMSignal(){
  boolean getNewPpm = false ; // become true if a new ppm value has been acquired  
#if defined( PIN_PPM )  // when ppm is read from a rx channel
  ReadPPM(); // set ppmus to 0 if ppm is not available or has not been collected X time, other fill ppmus with the (max) pulse duration in usec 
#ifdef DEBUGFORCEPPM
//for debuging ppm without having a connection to ppm; force ppm to a value
  ppmus = ( PPM_MIN_100 + PPM_PLUS_100) / 2 ; // force a ppm equal to 0 (neutral)
#endif // end of DEBUGFORCEPPM 
  if (ppmus>0){  // if a value has been defined for ppm (in microseconds)
    ppm.value = map( ppmus , PPM_MIN_100, PPM_PLUS_100, -100 , 100 ) ; // map ppm in order to calibrate between -100 and 100
    ppm.available = true ;
    getNewPpm = true ;
  } 
#else   // so when done via SPORT (defined ( PPM_VIA_SPORT ) && ( PROTOCOL  == FRSKY_SPORT ) || (PROTOCOL  == FRSKY_SPORT_HUB) ) )
        // newPpm.value and .available are already filled in readSensor() when new values are received
      if (newPpm.available ){
          ppm.value = newPpm.value ;
          ppm.available = true ;
          newPpm.available = false ;
          getNewPpm = true ;
      }
#endif  
  if  (getNewPpm ) { 
    getNewPpm = false ; // reset the indicator saying there is a new ppm to handle. 
#ifdef DEBUGPPMVALUE
       Serial.print(micros()); Serial.print(F(" ppm="));  Serial.println(ppm.value);
#endif
#ifdef SEQUENCE_OUTPUTS
    if ( ( ( ppm.value - prevPpm) > 3 ) || (( prevPpm - ppm.value) > 3 )  )  { // handel ppm only if it changes by more than 3 
        prevPpm = ppm.value ;
        setNewSequence( ) ;
    }  
#else // so if Sequence is not used and so PPM is used for Vario sensitivity , vario compensation , airspeed reset , glider ratio and/or vario source selection 
    if ( ( ( ppm.value - prevPpm) < 4 ) && ( ( prevPpm - ppm.value) < 4 ) ) {  // test if new value is quite close to previous in order to avoid unstabel handling 
    
#if defined ( VARIO_PRIMARY) && defined ( VARIO_SECONDARY)  && defined (VARIO ) && ( defined (VARIO2) || ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) ) )  || defined (USE_6050) )  && defined (PIN_PPM)
        if ( (ppm.value >= (SWITCH_VARIO_MIN_AT_PPM - 4)) && (ppm.value <= (SWITCH_VARIO_MAX_AT_PPM + 4)) ) {
          selectedVario = VARIO_PRIMARY ;
        } else if ( ( ppm.value <= (4 - SWITCH_VARIO_MIN_AT_PPM)) && (ppm.value >= (- 4 - SWITCH_VARIO_MAX_AT_PPM)) ) {
            selectedVario = VARIO_SECONDARY ; 
        }  
#ifdef DEBUG_SELECTED_VARIO
    Serial.print(F("selected vario="));  Serial.println(selectedVario);
#endif
#endif // endif defined ( VARIO_PRIMARY) && defined ( VARIO_SECONDARY)  && ...

#if defined (VARIO) || defined (VARIO2) 
        if ( (abs(ppm.value) >= (SENSITIVITY_MIN_AT_PPM - 4)) && ( abs(ppm.value) <= (SENSITIVITY_MAX_AT_PPM + 4)) ) {
            sensitivityPpmMapped =  map( constrain(abs(ppm.value), SENSITIVITY_MIN_AT_PPM , SENSITIVITY_MAX_AT_PPM ), SENSITIVITY_MIN_AT_PPM , SENSITIVITY_MAX_AT_PPM , SENSITIVITY_PPM_MIN , SENSITIVITY_PPM_MAX); // map value and change stepping to 10
#if defined( VARIO) 
            oXs_MS5611.varioData.sensitivityPpm = sensitivityPpmMapped ; // adjust sensitivity when abs PPM is within range
#endif
#if defined( VARIO2) 
            oXs_MS5611_2.varioData.sensitivityPpm = sensitivityPpmMapped ;
#endif
        }
#endif  // end else defined (VARIO) || defined (VARIO2) 
        

#if ( defined ( AIRSPEED) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined ( ADS_MEASURE ) && defined( ADS_AIRSPEED_BASED_ON ) ) )   // adjust compensation
        if ( (abs(ppm.value) >= (COMPENSATION_MIN_AT_PPM - 4)) && ( abs(ppm.value) <= (COMPENSATION_MAX_AT_PPM + 4)) ) {
            compensationPpmMapped =  map( constrain(abs(ppm.value), COMPENSATION_MIN_AT_PPM , COMPENSATION_MAX_AT_PPM ), COMPENSATION_MIN_AT_PPM , COMPENSATION_MAX_AT_PPM , COMPENSATION_PPM_MIN , COMPENSATION_PPM_MAX); // map value and change stepping to 10
            if (compensationPpmMapped == COMPENSATION_PPM_MIN ) compensationPpmMapped = 0 ; // force compensation to 0 when compensation = min
        }
        if ( ( ppm.value >= (AIRSPEED_RESET_AT_PPM - 4)) && ( ppm.value <= (AIRSPEED_RESET_AT_PPM + 4)) ) {
#if defined (AIRSPEED) // if 4525 is used
            oXs_4525.airSpeedData.airspeedReset = true ; // allow a recalculation of offset 4525
#else  // when 7002 is used
            oXs_ads1115.adsAirSpeedData.airspeedReset = true ; // allow a recalculation of offset mvxp7002 connected to ads1115
#endif
        }    
#endif  // end AIRSPEED or  ADS_AIRSPEED_BASED_ON

#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)  
        if ( ( ppm.value >= (FLOW_SENSOR_RESET_AT_PPM - 4)) && ( ppm.value <= (FLOW_SENSOR_RESET_AT_PPM + 4)) ) {
            consumedML = 0 ;     // reset the flow counter
        }
#endif


#if defined (GLIDER_RATIO_CALCULATED_AFTER_X_SEC ) && defined ( GLIDER_RATIO_ON_AT_PPM )
         if ( (ppm.value >= (GLIDER_RATIO_ON_AT_PPM - 4)) && (ppm.value <= (GLIDER_RATIO_ON_AT_PPM + 4)) ) {
          gliderRatioPpmOn = true ;
        } else {
            gliderRatioPpmOn = false ; 
        } 
#endif    
    
    } // end ppm == prePpm
    prevPpm = ppm.value ;  
#endif  // end of #ifdef SEQUENCE_OUTPUTS & #else
  }  // end if  (ppm.available ) 
  
}  // end processPPMSignal


/**********************************************************/
/* ReadPPM => read PPM signal from receiver               */
/*   pre-evaluate its value for validity                  */
/**********************************************************/

#ifdef PPM_INTERRUPT
uint16_t StartTime ;
//uint16_t EndTime ;
  
volatile uint8_t PulseTime ;		// A byte to avoid 
volatile boolean  PulseTimeAvailable = false ;

#if PIN_PPM == 2 // When pin 2 is used, arduino handle INT0 (see datasheet)
ISR(INT0_vect, ISR_NOBLOCK)  // NOBLOCK allows other interrupts to be served when this one is activated 
#else            //// When pin 3 is used, arduino handle INT1
ISR(INT1_vect, ISR_NOBLOCK)
#endif
{
	uint8_t oReg = SREG; // store SREG value 
	cli() ;
	uint16_t time = TCNT1 ;	// Read timer 1
	SREG = oReg ; // restore SREG value
	if ( EICRA & PPM_INT_EDGE ) // a rising edge occurs
	{
		StartTime = time ;              // save the value of the timer
		EICRA &= ~PPM_INT_EDGE ;				// allow a falling edge to generate next interrupt
	}
	else                       // a falling edge occurs   
	{
//		EndTime = time ;		
		time -= StartTime ;
#if F_CPU == 20000000L   // 20MHz clock 
   #error Unsupported clock speed
#elif F_CPU == 16000000L  // 16MHz clock                                                  
		time >>= 4 ;		// delay in usec
#elif F_CPU == 8000000L   // 8MHz clock
		time >>= 3 ;		// delay in usec
#else
    #error Unsupported clock speed
#endif
		
//                if (  ppmInterrupted == 0  ) { // do not handle PulseTime if pin change interrupt has been delayed by another interrupt (Timer 1 compare A handling)
		  time1 = time ; 
      PulseTimeAvailable = true ;
//		} else {
//                ppmInterruptedCopy++ ;  // used in order to test the value outside ISR
//                }
//                ppmInterrupted = 0 ;                            // reset indicator about pin change interrupt delay  
		EICRA |= PPM_INT_EDGE ;				//  allow a Rising edge to generate next interrupt
	}
}

void ReadPPM() {    // set ppmus to 0 if ppm is not available or has not been collected X time, other fill ppmus with the (max) pulse duration in usec 
         static uint8_t ppmIdx ;
         static uint16_t ppmTemp ;
         static uint16_t ppmMax ; // highest value of ppmTemp received ; Some ppm measurement are wrong (to low) because there are some interrupt some 
         ppmus = 0 ;
#if defined DEBUG_PPM_AVAILABLE_FROM_INTERRUPT
        Serial.print("Read ppm at ") ; Serial.println(millis()) ;
#endif

         if ( PulseTimeAvailable ) { // if new pulse is available
#define PPM_COUNT_MAX 5 // select the max of 5 ppm (so once every 100 msec
           uint8_t oReg = SREG ; // save Status register
           cli() ; 
            ppmTemp = time1 ;    // use values from interrupt
            PulseTimeAvailable = false ;
            SREG = oReg ;  // restore Status register  
#if defined DEBUG_PPM_AVAILABLE_FROM_INTERRUPT
        Serial.print("ppm time1= ") ; Serial.println(time1) ;
#endif
            if ( ppmIdx >= PPM_COUNT_MAX ) {
                ppmus = ppmMax ; // we keep the highest value
                ppmIdx = 0 ;
                ppmMax = ppmTemp ; 
            } else {
                ppmIdx++ ;
                if( ppmTemp > ppmMax ) ppmMax = ppmTemp ;  // save the highest value
            }  // end test on ppmIdx   
        } // end test on PulseTimeAvailable
} //end ReadPPM()
#endif // end #ifdef PPM_INTERRUPT

#endif //PIN_PPM


#if defined ( A_FLOW_SENSOR_IS_CONNECTED )
  #if ( A_FLOW_SENSOR_IS_CONNECTED == YES)  // flowMeterCnt is updated by interrupt PCINT0 



float mapFlowCorr( uint8_t idx) {                             // calculate correction factor depending on current flow
  uint8_t flowCorrIdx = idx + TX_FIELD_FLOW_CORR_1 ;
  return ( flowParam[flowCorrIdx ] + ( currentFlow - flowParam[idx]) / (flowParam[idx + 1] - flowParam[idx]) * (flowParam[flowCorrIdx + 1] - flowParam[flowCorrIdx ] ) ) ;
}
  
void checkFlowParam() {
  if ( (tankCapacity % 50) != 0) tankCapacity = ( tankCapacity / 50 ) * 50 ; // set tank capacity in steps of 50 ml
  tankCapacity = constrain( tankCapacity , 100 , 3000 ) ;
  for (uint8_t i = 0; i < 4 ; i++) {
      if ( ( flowParam[i] % 5) != 0 ) flowParam[i] = ( flowParam[i] / 5 ) * 5 ;
      flowParam[i] = constrain( flowParam[i] , 30 , 800 + (5 * i)) ;
      flowParam[ i+ 4] = constrain( flowParam[i+4] , -100 , +100 ) ;
  }
  for (uint8_t i= 1 ; i < 4 ; i++) {
      if ( flowParam[i] <= flowParam[i - 1] )  flowParam[i] = flowParam[i - 1] + 5 ;
  }
}

void processFlowMeterCnt () {    // get the flowmeter counter once per X seconds, convert it in mili liter and activate the flag saying a new value is available
                                 // save the result in eeprom is done in main loop for all data to be saved in EEPROM
#define FLOW_DELAY 2000                                   // calculates once per 2 sec
  float flowCorr ;
  uint16_t flowMeterCntCopy ;
  static uint32_t prevFlowMillis ;
  uint32_t currentFlowMillis = millis() ;
  if ( ( currentFlowMillis - prevFlowMillis ) > FLOW_DELAY ) {   
    prevFlowMillis = currentFlowMillis ;
    uint8_t oReg = SREG ; // save status register
    cli() ;  // avoid interrupt to ensure that counter is consistent
    flowMeterCntCopy = flowMeterCnt ; 
    flowMeterCnt = 0 ;
    SREG = oReg ; // restore status register
    sei() ;  // allow interrupt again
#ifdef DEBUG_SIMULATE_FLOW_SENSOR
    flowMeterCntCopy = 100 ;        // force a fix value in order to test without a flow sensor
#endif    
#define CONVERT_TO_ML_PER_MIN  30.0 / ( PULSES_PER_ML * FLOW_DELAY / 1000.0 ) //  30 = 60 /2 : 60 = 60 sec/min, 1000 = msec instead of sec , 2 because counter is increased by all changes (rise and fall)
    currentFlow  =  ((float) flowMeterCntCopy) * CONVERT_TO_ML_PER_MIN ;                                          // expected value should be between 15 and 800 ml/min for the conrad flow meter   
    if (currentFlow < flowParam[TX_FIELD_FLOW_1] ) {
      flowCorr = flowParam[TX_FIELD_FLOW_CORR_1] ;               // flow crrection is supposed to be in %
    } else if (currentFlow < flowParam[TX_FIELD_FLOW_2] ) { 
      flowCorr = mapFlowCorr(  TX_FIELD_FLOW_1 ) ;
    } else if (currentFlow < flowParam[TX_FIELD_FLOW_3] ) { 
      flowCorr = mapFlowCorr(  TX_FIELD_FLOW_2 ) ;
    } else if (currentFlow < flowParam[TX_FIELD_FLOW_4] ) { 
      flowCorr = mapFlowCorr(  TX_FIELD_FLOW_3 ) ;
    }  else {
      flowCorr = flowParam[TX_FIELD_FLOW_CORR_4] ;
    }  
    currentFlow = ((float) currentFlow ) * ( 100.0 + flowCorr) * 0.01   ; // 0.01 because flowCorr is in %)  
    actualFlow.value = currentFlow ;
    actualFlow.available= true ;
    consumedML += currentFlow  * FLOW_DELAY / 60000.0 ; 
    if ( consumedML > tankCapacity ) consumedML = tankCapacity ;
    remainingFuelML.value = tankCapacity - consumedML ;
    remainingFuelML.available= true ;
    fuelPercent.value =  remainingFuelML.value * 100 / tankCapacity ; // in percent
    fuelPercent.available= true ;
    newFlowAvailable = true ;   // this is use to fill TEST_1, 2, 3 to avoid conflict with JETI protocol which uses 3 individual flags 
#if defined ( DEBUG) && defined (DEBUG_FLOW_SENSOR)
    Serial.print(" At " ); Serial.print( prevFlowMillis ) ;
    Serial.print(" flcnt " ); Serial.print( flowMeterCntCopy ) ;
    Serial.print(" remain " ); Serial.print( remainingFuelML.value ) ;
    Serial.print(" ml/min " ); Serial.print( currentFlow ) ;
    Serial.print(" cons " ); Serial.println( consumedML ) ;
#endif
  
  } 
}      // end processFlowMeterCnt


ISR(PCINT0_vect, )  {       // a pin change interrupt occurs on the pin being connected to the flow sensor; we do not use ISR_NOBLOCK because the interrupt is very short
  flowMeterCnt++ ;            // we increase the counter of pin change.
}


  #endif
#endif // end A_FLOW_SENSOR_IS_CONNECTED 



#ifdef SEQUENCE_OUTPUTS
void setNewSequence() {                   // **********  setNewSequence( ) **************  handle a new ppm value
//    int8_t prevPpmMain = -100 ; // this value is unusual; so it will forced a change at first call
    int16_t ppmAbs = ppm.value + 114 ;
    int8_t ppmMain = ppmAbs / 25 ;
    int8_t ppmRest = ppmAbs % 25 ;
    if ( (ppmRest > 4) && (ppmMain >= 0) && (ppmMain <= 9) && (ppmMain != prevPpmMain) ) { // process only if valid and different from previous
#ifdef DEBUGSEQUENCE
    Serial.println(F(" "));
    Serial.print(F("ppmMain="));  Serial.println(ppmMain );  
    //Serial.println(F(" "));
#endif
       prevPpmMain = ppmMain ;
       seqRef = sequencePointer[ppmMain] ;  // seqTab contains pointers to the sequence array
       seqMax = sequenceMaxNumber[ppmMain] ; 
       seqState = 0 ;
       seqStep = 0 ;
#ifdef DEBUGSEQUENCE
       test1.value = ppmMain; 
       test1.available = true ; 
#endif
       checkSequence() ;
    }    
}

void checkSequence() {
  uint8_t portbTemp ;
  uint8_t portbTempValue ;
  static uint16_t seqDelay ;
  static unsigned long  currentMillis ;
  static unsigned long  nextSequenceMillis ;
#ifdef DEBUGSEQUENCE
//    Serial.print(F("seqState="));  Serial.println(seqState );  
#endif

  if ( seqState == 0 ) {
    portbTempValue = *(seqRef + 1); 
    seqDelay = *(seqRef ) * sequenceUnit; //
    portbTemp =  (~sequenceOutputs) & PORTB; // reset the bit that are controlled
    PORTB = portbTemp | ( portbTempValue & sequenceOutputs ); // set the bit as defined in the sequence if they have to be controlled
#ifdef DEBUGSEQUENCE
    Serial.print(F("seqStep="));  Serial.println( seqStep );
    Serial.print(F("portbTempValue="));  Serial.println( portbTempValue );
    Serial.print(F("seqDelay="));  Serial.println( seqDelay );
    Serial.print(F("portbTemp="));  Serial.println( portbTemp );
    Serial.print(F("PORTB="));  Serial.println(  portbTempValue & sequenceOutputs );  
#endif
    
    if (seqDelay == 0 ) {
      seqState = 2 ; // set state to sequence stopped if delay is 0
    } else {   
      seqState = 1 ; //says that sequence is running
      nextSequenceMillis = millis() +  seqDelay ;
    }  
  } else if ( seqState == 1 ) { // when sequence is running, check the timestamp to activate next sequence step
    currentMillis = millis() ;
    if ( currentMillis >  nextSequenceMillis  ) {
       seqStep += 2 ; // increase by 2 because there are 2 parameters per step
      if ( (seqStep + 1)  >=  seqMax  ) seqStep = 0 ; // return to 0 if end is reached
      portbTempValue = *(seqRef + 1 + seqStep ); 
      seqDelay = *(seqRef + seqStep) * sequenceUnit ;
      portbTemp =  (~sequenceOutputs) & PORTB; // reset the bit that are controlled
      PORTB = portbTemp | ( portbTempValue & sequenceOutputs ); // set the bit as defined in the sequence if they have to be controlled
      if (seqDelay == 0 ) {
          seqState = 2 ; // set state to sequence stopped if delay is 0
      } else {   
          seqState = 1 ; //says that sequence is running
          nextSequenceMillis = currentMillis  +  seqDelay ;
      }
#ifdef DEBUGSEQUENCE
      Serial.print(F("At"));  Serial.print( currentMillis );
      Serial.print(F(" seqStep="));  Serial.println( seqStep );
      Serial.print(F("portbTempValue="));  Serial.println( portbTempValue );
      Serial.print(F("seqDelay="));  Serial.println( seqDelay );
      Serial.print(F("portbTemp="));  Serial.println( portbTemp );
      Serial.print(F("PORTB="));  Serial.println( portbTempValue & sequenceOutputs );  
#endif
     
    } // end  ( currentMillis >  nextSequenceMillis  )
  }   // end  ( seqState == 0 ) or ( seqState == 1 ) 
}     // end checkSequence() 
#endif // end SEQUENCE_OUTPUTS



#ifdef DEBUG
//************************************************************   OutputToSerial
void OutputToSerial(){
//#define DEBUGMINMAX 0
//#define DEBUGVREF 0
//#define DEBUGDIVIDER 0
//#define DEBUGTEMP 0



#ifdef VARIO
  Serial.print(F(" Sensitivity PPM="));    
  Serial.print( oXs_MS5611.varioData.sensitivityPpm);
  Serial.print(F(" ;absAlt="));  
  Serial.print( ( (float)oXs_MS5611.varioData.absoluteAlt.value)/100);
  Serial.print(F(" ;Vspd="));    
  Serial.print( ( (float)oXs_MS5611.varioData.climbRate.value)/100);
  Serial.print(F(" ;Temp="));    
  Serial.print((float)oXs_MS5611.varioData.temperature/10);
#endif // VARIO

#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
  Serial.print(F(" ;mA="));    
  Serial.print( ( ((float)(int32_t)(oXs_Current.currentData.milliAmps.value))) );
  Serial.print("(");    
  Serial.print(F(" ;mAh="));  
  Serial.print( oXs_Current.currentData.consumedMilliAmps.value);
#endif // defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
#ifdef HOTT
  Serial.println("H.");
#endif  
  Serial.println();
  
}

/***********************************************/
/* freeRam => Cook coffee and vaccuum the flat */
/***********************************************/
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif // End DEBUG




