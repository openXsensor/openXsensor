
//******************************************************************************************************************************************************* //
//                                                                                                                                                        //
// ! IMPORTANT NOTE !!!!!!!!!!   Another file in this project provides detailed explanations on how to set up this file (see oXs_config_description.h).   //
//                                                                                                                                                        //
//******************************************************************************************************************************************************* //
//  Note: the oXs_config_advanced.h file present on the GitHub site is not always meaningful.
//       It could be that the combination of active/non active parameters is not consistent.
//       This is just the result of many updates and tests in this document.
//       So take always care to set up the oXs_config_basic.h and oXs_config_advanced.h files according to your needs and check carefully all options.

#ifndef OXS_CONFIG_ADVANCED_h
#define OXS_CONFIG_ADVANCED_h
//#include "oXs_config_basic.h"


// --------- 1 - Telemetry protocol ---------  Protocol is defined in oXs_config_basic.h file

// ****** 1.1 - Pin connected to Rx ********
#define PIN_SERIALTX      4                 // The pin which transmits the serial data to the telemetry receiver, Usually pin 4 (otherwise pin 2)

// ***** 1.2 - SPORT_SENSOR_ID used (only for Frsky Sport protocol)  *****   See list of available values in oXs_config_descripion.h 
#define         DATA_ID_VARIO  0x00  // = sensor 0 used for Alt and Vspeed
#define         DATA_ID_FLVSS  0xA1  //          1 used for Cell values
#define         DATA_ID_FAS    0x22  //          2 used for vfas , current and fuel
#define         DATA_ID_GPS    0x83  //          3 used for GPS data
#define         DATA_ID_RPM    0xE4  //          4 used for rpm, T1, T2, airspeed
#define         DATA_ID_ACC    0x67  //          7 used for Acc X, Y, Z
#define         DATA_ID_TX     0x0D  //           used to read data sent by Tx in order to adjust some oXs parameters (flow sensor or ppm)
// --------- 2 - Data to transmit ---------                   
// ***** 2.1 - Frsky data *****                              see oXs_config_basic.h file
// ***** 2.2 - Hott data *****                               see oXs_config_basic.h file
// ***** 2.3 - Multiplex data *****                          see oXs_config_basic.h file   
// ***** 2.4 - Jeti data *****                               see oXs_config_basic.h file

// ***** 2.5 - How to fill TEST_1, TEST_2, TEST_3  ******    
//define FILL_TEST_3_WITH_EXPECTED_ALT                        // uncomment this line if oXs has to calculate an expected Alt in the future based on current Alt, Vspeed and vertical Acc
//#define EXPECTED_ALT_AT_SEC 0.2                               // time (in sec) for expected Alt (this line must be uncommented wen previous is uncommented

//#define FILL_TEST_1_2_3_WITH_LINEAR_ACC                     // uncomment this line if oXs has to fill TEST_1, TEST_2, TEST_3 with linear Acc

//#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO  // uncomment to activate this option

//#define FILL_TEST_1_WITH_DTE                                 // uncomment to activate this option

//#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION           // uncomment to activate this option

//#define FILL_TEST_1_WITH_YAWRATE                             // uncomment to activate this option

//#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER              // uncomment to activate this option

#define FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION             // uncomment to activate this option

// --------- 3 - PPM settings ---------
//#define PIN_PPM           2     // Uncomment this line in order to use a Rx channel to control oXs; default is 2 but my own device use 3
#define PPM_MIN_100       988     // default 1500 - 512 ; // pulse width (usec) when TX sends a channel = -100
#define PPM_PLUS_100      2012    // default 1500 + 512 ; // pulse width (usec) when TX sends a channel = +100

//#define PPM_VIA_SPORT             // uncomment this line to get ppm data over SPORT protocol instead of from a PWM channel (it requires a Tx with openTx running LUA script)

// --------- 4 - Vario settings ---------    Type of baro is defined in oXs_config_basic.h file

// ***** 4.1 - Connecting 1 or 2 barometric sensor(s)  ***** 
#define SECOND_BARO_SENSOR_USE    NO_BARO                    // select between NO_BARO , MS5611

// ***** 4.2 - Type of Vspeed to transmit  *****             Is defined only in oXs_config_basic.h file                       

// ***** 4.3 - Sensitivity predefined by program *****
#define SENSITIVITY_MIN 80          // normal value for MS5611; for BMP, it is probably better to use a lower value like 20
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000

// ***** 4.4 - Sensitivity adjusted from the TX *****
#define SENSITIVITY_MIN_AT_PPM 10    // sensitivity will be changed by OXS only when PPM signal is between the specified range enlarged by -5 / +5
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20      // common value for vario is 20
#define SENSITIVITY_PPM_MAX 100      // common value for vario is 100

// ***** 4.5 - Hysteresis parameter & Alt compensation based on sensor temp *****
#define VARIOHYSTERESIS 5
//#define ALT_TEMP_COMPENSATION 800

// ***** 4.6 - Vertical speeds calculations based on PPM *****
#define VARIO_PRIMARY       FIRST_BARO          // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define VARIO_SECONDARY     AIRSPEED_COMPENSATED          // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define SWITCH_VARIO_MIN_AT_PPM 10
#define SWITCH_VARIO_MAX_AT_PPM 90

// ***** 4.7 - Analog vertical speed *****
//#define PIN_ANALOG_VSPEED 3                 // Uncomment this line to get an analog voltage related to Vspeed
#define ANALOG_VSPEED_MIN -3
#define ANALOG_VSPEED_MAX  3

// ***** 4.8  - Calculating glider ratio, average sink/climb rate ******************************
//#define GLIDER_RATIO_CALCULATED_AFTER_X_SEC 1  // Uncomment this line as comment if a calculation must be performed ; value must be higher or equal to 1 (sec)
#define SPEED_TOLERANCE   5              // in % of speed
#define VSPEED_MIN_TOLERANCE -200        // out of tolerance when Vspeed is lower than this value  (cm/sec)
#define VSPEED_MAX_TOLERANCE  -10        // out of tolerance when Vspeed is upper than this value  (cm/sec)
#define GLIDER_RATIO_ON_AT_PPM -100      // Glider ratio is calculated when PPM has this value ; keep as comment when ppm is not used 

// --------- 5 - Airspeed settings ---------
//#define AIRSPEED_AT_SEA_LEVEL_AND_15C // if this line is commented, airspeed is calculated using baro pressure and temperature (so being "true" airspeed instead of normalised airspeed)      
//#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h ( except some old versions, openTx expects knot/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 140

// --------- 6 - Voltages & Current sensor settings ---------

// ***** 6.1 - Voltage Reference to measure voltages and current *****
#define USE_INTERNAL_REFERENCE  // uncomment this line if you use 1.1 volt internal reference instead of Vcc (voltage divider mst be used to reduce voltages to 1.1 volt max)
//#define USE_EXTERNAL_REFERENCE  // uncomment this line if you use an external reference instead of Vcc
//#define REFERENCE_VOLTAGE 4970    // set value in milliVolt; if commented, oXs will use or 1100 (if internal ref is used) or 5000 (if internal ref is not used) 

// ***** 6.2 - Voltage parameters *****
// Each of following lines contains 6 parameters, the first value is for VOLT_1, the second for VOLT_2, ... up to the sixth for VOLT_6 
#define PIN_VOLTAGE        6  , 8     , 8   , 8    , 8   , 8               //  Fill 6 values; set to 0 up to 7 for analog pins A0 up to A7 ; set the value to 8 for the voltage(s) not to be measured.
#define RESISTOR_TO_GROUND  2.95 , 10    , 10  , 10 , 0  , 18               // set value to 0 when no divider is used for a voltage; can contains decimals 
#define RESISTOR_TO_VOLTAGE 46.9 , 8.7 , 22 , 27  , 0 , 47              // set value to 0 when no divider is used for a voltage; can contains decimals 
#define OFFSET_VOLTAGE      0   , 0     , 0    , 0    , 0   , 0                // optionnal, can be negative, must be integer, in principe in mv
#define SCALE_VOLTAGE       1.00 , 1.0   , 1.0  , 1.0  , 1.0 , 1.0              // optionnal, can be negative, can have decimals

// ***** 6.3 - Max number of Lipo cells to measure (and transmit to Tx) *****      Is defined only in oXs_config_basic.h file

// ***** 6.4 - Convert voltage to temperature (° Celcius) *****     
//#define FIRST_NTC_ON_VOLT_NR 1   // uncomment this line when thermistor are used; specify index of first voltage being used for conversion to temperature (e.g. 5 means VOLT_5)
#define LAST_NTC_ON_VOLT_NR 1    // specify index of last voltage being used for conversion to temperature (e.g. 6 means VOLT_6)
#define SERIE_RESISTOR 4700      // resistance connected to Arduino Vcc (in Ohm)
#define STEINHART_A 7.00111E-4   // these parameters are specific to the NTC being used.
#define STEINHART_B 2.1644E-4
#define STEINHART_C 1.0619E-07

//#define TERMISTOR_NOMINAL 100000 // nominal resistor of NTC (in Ohm)                    Those 3 parameters are not used anymore (replaced by STEINHART8A B and C
//#define TEMPERATURE_NOMINAL 25 // nominal temperature of NTC (in degree Celcius)
//#define B_COEFFICIENT 3950 // B coefficient of NTC

// ***** 6.5 - Current parameters  *****
#define PIN_CURRENTSENSOR   6  //      Arduino pin used to measure the voltage provided by a current sensor
#define MVOLT_AT_ZERO_AMP              2500    // in millivolt
#define MVOLT_PER_AMP                  60      // in milliVolt per Amp
#define RESISTOR_TO_GROUND_FOR_CURRENT  0   // put as comment or set to 0 if no divider is used (e.g. 19.8 for 1.1 internal ref)
#define RESISTOR_TO_CURRENT_SENSOR      0   // put as comment or set to 0 if no divider is used (e.g  39   for 1.1 internal ref)

// ***** 6.6 - Ads1115 parameters  *****
#define ADS_MEASURE A0_TO_A1 ,  ADS_OFF , ADS_OFF , ADS_OFF // select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV2048, MV4096, MV6144, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 1, 1, 1, 1 // can be a float
#define ADS_RATE  MS2 , MS9, MS9 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 10 , 20, 30, 50 // number of values used for averaging (must be between 1 and 254) 
//#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT_1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4

// --------- 7 - RPM (rotations per minute) settings ---------                      
#define PULSES_PER_ROTATION 2

// --------- 8 - Persistent memory settings ---------
//#define PIN_PUSHBUTTON    2   // default is 10 but my own device is 2

// --------- 9 - GPS ------------------------------------------------------------------------------------------------
//#define GPS_SPEED_IN_KMH  // uncomment this line if GPS speed has to be sent in km/h instead of knot/h (only for Frsky protocol)
#define GPS_SPEED_3D      // uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test) 
#define GPS_REFRESH_RATE  5      // rate at which GPS sent new data; select between 1, 5 or 10 (Hz). Default = 5 Hz; Ublox NEO6 does not support 10 hz  

// --------- 10 - IMU 6050 --- (accelerometer + gyro) and HMC5883 (magnetometer) --------------------------------------
// ***** 10.1 - IMU 6050 *****
#define PIN_INT_6050 2   // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM)

//#define DISPLAY_ACC_OFFSET // used ONLY in order to display the acceleration offset on pc terminal; KEEP AS COMMENT once offsets have been setup 

#define ACC_OFFSET_X 0 // fill here the first value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the second value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the third value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)

// ***** 10.2 - HMC5883 *****
//#define GENERATE_MAG_CALIBRATION_DATA   // uncomment this line when HMC5883 calibration has to be performed. Set back as comment once calibration parameters have been introduced 

#define    XMAG_OFFSET 2.4683     // must be an integer
#define    YMAG_OFFSET -1.3694     // must be an integer
#define    ZMAG_OFFSET 138.9683     // must be an integer
#define    XXMAG_CORRECTION  0.122082   // can have decimals
#define    XYMAG_CORRECTION  -0.00204026
#define    XZMAG_CORRECTION  0.00377534 
#define    YXMAG_CORRECTION  -0.00204026
#define    YYMAG_CORRECTION  0.130413
#define    YZMAG_CORRECTION  -0.00491189
#define    ZXMAG_CORRECTION  0.00377534
#define    ZYMAG_CORRECTION  -0.00491189
#define    ZZMAG_CORRECTION  0.138038

// --------- 11 - Flow sensor ---------
#define PULSES_PER_ML                    10.0                 // number of pulses per milli liter (depends on sensor); can have decimals
#define TANK_CAPACITY                    1000                 // tank capacity in ml
#define INIT_FLOW_PARAM  30 , 100 , 500 , 700 , 0 , 0, 0, 0   // define at 4 levels of flow (in mliter/min) (e.g. 30, 100, 500, 700) 4 correction parameters (in %; e.g. 20, 10, -5, 15); flow levels have to be sorted from low to high
#define FLOW_SENSOR_RESET_AT_PPM         95                   // when absolute value of ppm is greater than this, flow counter is reset.

// --------- 20 - Sequencer ---------
//#define SEQUENCE_OUTPUTS 0b100000  
#define SEQUENCE_UNIT 2
#define SEQUENCE_m100    1 , 0b100000 , 3 , 0b000000 , 1 , 0b100000 , 3 , 0b000000 
//#define SEQUENCE_m75     1 , 0b100000 , 1 , 0b000000 , 2 , 0b100000 , 2 , 0b000000
//#define SEQUENCE_m50     5 , 0b100000 , 5 , 0b000000
//#define SEQUENCE_m25     5 , 0b100000 , 5 , 0b000000 , 0 , 0b100000
#define SEQUENCE_0       3 , 0b100000 , 1 , 0b000000
//#define SEQUENCE_25      2 , 0b100000 , 2 , 0b000000
//#define SEQUENCE_50      5 , 0b100000 , 5 , 0b000000
//#define SEQUENCE_75      7 , 0b100000 , 7 , 0b000000
#define SEQUENCE_100     8 , 0b100000 , 16 , 0b000000 , 24 , 0b100000 , 32 , 0b000000  
//#define SEQUENCE_LOW    10 , 0b100000 ,10 , 0b000000   // sequence for Low voltage
//#define SEQUENCE_MIN_VOLT_6 4000 // sequence_100 will be activated if voltage 6 is lower that the value.
//#define SEQUENCE_MIN_CELL   3000 // sequence_100 will be activated if lowest cell is lower that the value.


// --------- xx - Reserved for developer. DEBUG must be activated here when we want to debug one or several functions in some other files. ---------
//#define DEBUG
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


#endif// End define OXS_CONFIG_ADVANCED_h


