#ifndef OXS_CONFIG_MACROS_h
#define OXS_CONFIG_MACROS_h

// --------- xx - Reserved for developer. DEBUG must be activated here when we want to debug one or several functions in some other files. ---------
#define DEBUG
///#define DEBUG_BLINK   // use by developper in order to blink the led without using uart for debugging


#define BASED_ON_AIRSPEED 0
#define BASED_ON_GPS_SPEED 1
#define AVERAGING_DELAY_MILLISEC  AVERAGING_TOLERANCE * 100  

#define FIRST_BARO 1
#define SECOND_BARO 2
#define AVERAGE_FIRST_SECOND 4
#define AIRSPEED_COMPENSATED 3
#define BARO_AND_IMU 5
#define PPM_SELECTION 6

#include <Arduino.h>
struct ONE_MEASUREMENT {
  uint8_t available ;
  int32_t value ;
} ;

#define FRSKY_SPORT      1
#define FRSKY_HUB        2
#define FRSKY_SPORT_HUB  3
#define MULTIPLEX        4
#define HOTT             5
#define JETI             6

#define NO_BARO 1
#define MS5611  2
#define GY86    3
#define BMP085  4
#define BMP180  5
#define BMP280  6
#define GY87    7

#define NO_AIRSPEED 1
#define MS4525      2
#define MVPX7002    3
#define SDP3X       4

#define NO  1
#define YES 2

#define NTC 20

#define SECONDS_SINCE_T0        32
#define AVERAGE_VSPEED_SINCE_TO 33 

/***************************************************************************************/
/* oXs code used by Flow sensor and for SPORT Lua scripts                                                        */ 
/***************************************************************************************/
#define TX_FIELD_FLOW_1     0
#define TX_FIELD_FLOW_2     1
#define TX_FIELD_FLOW_3     2
#define TX_FIELD_FLOW_4     3
#define TX_FIELD_FLOW_CORR_1    4
#define TX_FIELD_FLOW_CORR_2    5
#define TX_FIELD_FLOW_CORR_3    6
#define TX_FIELD_FLOW_CORR_4    7
#define TX_FIELD_TANK_CAPACITY  8
#define TX_FIELD_RESET_FUEL     9
#define TX_FIELD_PPM          0x10


#ifdef DEBUG
//#include "HardwareSerial.h"
#endif

#ifdef GPS_INSTALLED
//#include "HardwareSerial.h"
#endif



#ifdef FIRST_BARO_SENSOR_USE
  #if (FIRST_BARO_SENSOR_USE ==  MS5611 ) || (FIRST_BARO_SENSOR_USE ==  GY86 ) || (FIRST_BARO_SENSOR_USE ==  BMP085 ) || (FIRST_BARO_SENSOR_USE ==  BMP180 ) || (FIRST_BARO_SENSOR_USE ==  BMP280 ) || (FIRST_BARO_SENSOR_USE ==  GY87 )
    #define VARIO
  #endif
  #if  (FIRST_BARO_SENSOR_USE ==  BMP085 ) || (FIRST_BARO_SENSOR_USE ==  BMP180 )  || (FIRST_BARO_SENSOR_USE ==  GY87 )
    #define SENSOR_IS_BMP180
  #endif
  #if  (FIRST_BARO_SENSOR_USE ==  BMP280 ) 
    #define SENSOR_IS_BMP280
  #endif
#endif

#ifdef SECOND_BARO_SENSOR_USE
  #if SECOND_BARO_SENSOR_USE ==  MS5611  
    #define VARIO2
  #endif
#endif  

#if ( defined( FIRST_BARO_SENSOR_USE ) && ( (FIRST_BARO_SENSOR_USE ==  MS5611 ) || (FIRST_BARO_SENSOR_USE ==  GY86 ) )) || ( defined( SECOND_BARO_SENSOR_USE ) && ( (SECOND_BARO_SENSOR_USE ==  MS5611 ) || (SECOND_BARO_SENSOR_USE ==  GY86 ) ))
  #define SENSOR_IS_MS5611
#endif

#ifdef  AIRSPEED_SENSOR_USE 
  #if ( AIRSPEED_SENSOR_USE == MS4525 )
    #define AIRSPEED
    #define AIRSPEED_IS_USED 
    #define AIRSPEED_4525
  #elif ( AIRSPEED_SENSOR_USE == MPXV7002 )
    #define AIRSPEED_IS_USED
    #define AIRSPEED_7002
  #elif ( AIRSPEED_SENSOR_USE == SDP3X )
    #define AIRSPEED_IS_USED
    #define AIRSPEED_SDP3X
  #endif
#endif  


#ifdef CALCULATE_RPM 
  #if ( CALCULATE_RPM == YES )
    #define MEASURE_RPM
  #endif
#endif  


#ifdef  A_GPS_IS_CONNECTED 
  #if ( A_GPS_IS_CONNECTED == YES )
    #define GPS_INSTALLED
  #endif
#endif  


#ifdef  A_MPU6050_IS_CONNECTED 
  #if ( A_MPU6050_IS_CONNECTED == YES )
    #define USE_6050 
  #endif
#endif  

#ifdef  A_MPU6050_IS_CONNECTED 
  #if ( A_MPU6050_IS_CONNECTED == YES ) 
    #ifdef CALCULATE_YAW_WITH_HMC5883
      #if ( CALCULATE_YAW_WITH_HMC5883 == YES )
        #define USE_HMC5883
      #endif
    #endif    
  #endif
#endif  

#if defined( DISPLAY_ACC_OFFSET ) && defined ( A_MPU6050_IS_CONNECTED ) && ( A_MPU6050_IS_CONNECTED == YES )
  #define DEBUG
#endif
#if defined( GENERATE_MAG_CALIBRATION_DATA ) && defined ( A_MPU6050_IS_CONNECTED ) && ( A_MPU6050_IS_CONNECTED == YES ) && defined ( USE_HMC5883 )   
  #define DEBUG
#endif


#endif
