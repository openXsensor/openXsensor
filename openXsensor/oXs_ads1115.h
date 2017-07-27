#ifndef OXS_ADS1115_h
#define OXS_ADS1115_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "I2C.h"
#include "oXs_curr.h"
#include "oXs_4525.h"

// values to say how to use a ads measurement
#define ADS_VOLT_1 0
#define ADS_VOLT_2 1
#define ADS_VOLT_3 2
#define ADS_VOLT_4 3
#define ADS_VOLT_5 4
#define ADS_VOLT_6 5
#define ADS_AIRSPEED 6
#define ADS_CURRENT 7 
#define ADS_NOT_USED 8

// values to configure the ads multiplexer
#define A0_TO_A1 0
#define A0_TO_A3 1
#define A1_TO_A3 2
#define A2_TO_A3 3
#define A0_TO_GND 4
#define A1_TO_GND 5
#define A2_TO_GND 6
#define A3_TO_GND 7
#define ADS_OFF 8

// values to configure the programable ads gain amplifier
#define MV6144 0 
#define MV4096 1
#define MV2048 2
#define MV1024 3
#define MV512 4
#define MV256 5

// value to configure the rate of ads conversion (value is the delay in msec that must be wait before getting the conversion ; this take care of the tolerance of 10% foreseen in the datasheet 
#define MS137 0
#define MS69 1
#define MS35 2
#define MS18 3 
#define MS9 4 
#define MS5 5 
#define MS3 6
#define MS2 7


  extern const uint8_t ads_Measure[4] ;
  extern const uint8_t ads_Gain[4] ;
  extern const uint8_t ads_Rate[4] ;
  extern const float ads_Offset[4] ;
  extern const float ads_Scale[4] ;
  extern const uint8_t ads_MaxCount[4] ;
  

  extern uint32_t ads_MilliAskConv  ; 
  extern uint8_t ads_Addr;
  extern uint8_t ads_CurrentIdx ;
  extern uint8_t I2CErrorCodeAds1115 ; 
  extern int32_t ads_SumOfConv[4] ; // summarise all conversion in order to calculate average 
  extern uint8_t ads_Counter[4]  ;
  extern struct ONE_MEASUREMENT ads_Conv[4] ; //averaged conversion including offset and scale
  extern uint8_t ads_Last_Conv_Idx ;

  

class OXS_ADS1115 {
public:
#ifdef DEBUG  
  OXS_ADS1115(uint8_t addr, HardwareSerial &print);
#else
  OXS_ADS1115(uint8_t addr) ;
#endif
  
  void setup();
  boolean  readSensor( void ); //return true if an averaged has just been calculated  
  void ads_requestNextConv(void) ; 
  void ads_calculateCurrent(void) ;
  void ads_calculate_airspeed( int16_t ads_difPressureADC ) ;
#if defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
  struct CURRENTDATA adsCurrentData ;
  float floatConsumedMilliAmps ; // in mA
#endif
#if defined(ADS_MEASURE) && defined(ADS_AIRSPEED_BASED_ON)
  struct AIRSPEEDDATA adsAirSpeedData ;
#endif
  
private: 
#ifdef DEBUG
  HardwareSerial* printer;
#endif
}; // end class OXS_ADS1115



#endif // OXS_ADS1115


