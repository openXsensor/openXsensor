// OpenXsensor https://github.com/openXsensor/
// started by Rainer Schlosshan and maintained by Michel Strens

// This is version : 8.0.6 (8 march 2017)

//******************************************************************************************************************************************************* //
//                                                                                                                                                        //
// ! IMPORTANT NOTE !!!!!!!!!!   Another file in this project provides detailed explanations on how to set up this file (see oXs_config_description.h).   //
//                                                                                                                                                        //
//******************************************************************************************************************************************************* //
//  Note: the oXs_config.h file present on the GitHub site is not always meaningful.
//       It could be that the combination of active/non active parameters is not consistent.
//       This is just the result of many updates and tests in this document.
//       So take always care to set up the oXs_config.h file according to your needs and check carefully all options.
//  For old version 2 and 3 of oXs, there was a program named "configurator" running on PC that allowed easier editing of this oXs_config.h file
//       Still this configurator is not developped anymore. So currently, this oXs_config.h file has to be edited manually. 


#ifndef OXS_CONFIG_h
#define OXS_CONFIG_h


// --------- 1 - Telemetry protocol ---------
#define PROTOCOL  FRSKY_SPORT  // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT, JETI

// --------- 2 - Serial data pin choice ---------
#define PIN_SERIALTX      4    // The pin which transmits the serial data to the telemetry receiver, Usually pin 4 (otherwise pin 2)

// --------- 3 - PPM settings ---------
//#define PIN_PPM           3       // default is 2 but my own device use 3
#define PPM_MIN_100       980     // default 1500 - 512 ; // pulse width (usec) when TX sends a channel = -100
#define PPM_PLUS_100      1990    // default 1500 + 512 ; // pulse width (usec) when TX sends a channel = +100

// --------- 4 - Vario settings ---------

// ***** 4.1 - Connecting 1 or 2 barometric sensor(s)  *****
#define VARIO // set as comment if there is no vario
//#define SENSOR_IS_BMP180 // set as comment if first baro sensor is a BMP180 or BMP085 (instead of a MS5611)
//#define SENSOR_IS_BMP280 // set as comment if first baro sensor is a BMP280 (instead of a MS5611)
//#define VARIO2 // set as comment if there is no second vario

// ***** 4.2 - Sensitivity predefined by program *****
#define SENSITIVITY_MIN 50
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000

// ***** 4.3 - Sensitivity adjusted from the TX *****
#define SENSITIVITY_MIN_AT_PPM 10    // sensitivity will be changed by OXS only when PPM signal is between the specified range enlarged by -5 / +5
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20      // common value for vario is 20
#define SENSITIVITY_PPM_MAX 100      // common value for vario is 100

// ***** 4.4 - Hysteresis parameter & Alt compensation based on sensor temp *****
#define VARIOHYSTERESIS 5
//#define ALT_TEMP_COMPENSATION 800

// ***** 4.5 - Vertical speeds calculations *****
#define VARIO_PRIMARY       FIRST_BARO          // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define VARIO_SECONDARY     FIRST_BARO          // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define SWITCH_VARIO_MIN_AT_PPM 10
#define SWITCH_VARIO_MAX_AT_PPM 90

// ***** 4.6 - Analog vertical speed *****
//#define PIN_ANALOG_VSPEED 3
#define ANALOG_VSPEED_MIN -3
#define ANALOG_VSPEED_MAX  3

// ***** 4.7  - Calculating glider ratio, average sink/climb rate ******************************
//#define GLIDER_RATIO_CALCULATED_AFTER_X_SEC 1  // value must be higher or equal to 1, set line as comment if no calculation must be performed
#define SPEED_TOLERANCE   5              // in % of speed
#define VSPEED_MIN_TOLERANCE -200        // out of tolerance when Vspeed is lower than this value  (cm/sec)
#define VSPEED_MAX_TOLERANCE  -10        // out of tolerance when Vspeed is upper than this value  (cm/sec)
#define GLIDER_RATIO_ON_AT_PPM -100      // Glider ratio is calculated when PPM has this value ; keep as comment when ppm is not used 
// --------- 5 - Airspeed settings ---------
//#define AIRSPEED    MS4525
#define AIRSPEED_AT_SEA_LEVEL_AND_15C // if this line is commented, airspeed is calculated using baro pressure and temperature (so being "true" airspeed instead of normalised airspeed)      
//#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h ( except some old versions, openTx expects knot/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 140

// --------- 6 - Voltages & Current sensor settings ---------

// ***** 6.1 - Voltage Reference to measure voltages and current *****
#define USE_INTERNAL_REFERENCE  // uncomment this line if you use 1.1 volt internal reference instead of Vcc
//#define USE_EXTERNAL_REFERENCE  // uncomment this line if you use an external reference instead of Vcc
//#define REFERENCE_VOLTAGE 4970    // set value in milliVolt; if commented, oXs will use or 1100 (if internal ref is used) or 5000 (if internal ref is not used) 

// ***** 6.2 - Voltage parameters *****
//#define PIN_VOLTAGE         2  , 3     , 1   , 8    , 8   , 8               // set this line as comment if no one voltage have to be measured, set the value to 8 for the voltage(s) not to be measured.
#define RESISTOR_TO_GROUND  36 , 20    , 30  , 19.8 , 50  , 0               // set value to 0 when no divider is used for a voltage, can contains decimals 
#define RESISTOR_TO_VOLTAGE 670 , 100.1 , 200 , 39   , 500 , 0              // set value to 0 when no divider is used for a voltage, can contains decimals 
#define OFFSET_VOLTAGE      0 , 0     , 0   , 0    , 0   , 0                // optionnal, can be negative, must be integer
#define SCALE_VOLTAGE      0.985 , 1     ,  1  , 1.004, 1   , 204.6                // optionnal, can be negative, can have decimals

// ***** 6.3 - Max number of Lipo cells to measure (and transmit to Tx) *****
#define NUMBEROFCELLS 3   // Put this line as comment or set value to 0 (zero) if you do not want to transmit cell voltages.

// ***** 6.4 - Current parameters  *****
//#define PIN_CURRENTSENSOR   3  // uncomment when Arduino pin is used to measure the voltage provided by a current sensor
#define MVOLT_AT_ZERO_AMP              2500    // in millivolt
#define MVOLT_PER_AMP                  60      // in milliVolt per Amp
#define RESISTOR_TO_GROUND_FOR_CURRENT  19.8   // put as comment or set to 0 if no divider is used
#define RESISTOR_TO_CURRENT_SENSOR      39     // put as comment or set to 0 if no divider is used

// ***** 6.5 - Ads1115 parameters  *****
#define ADS_MEASURE A0_TO_A1 ,  A1_TO_GND , A2_TO_GND , ADS_OFF // uncomment when ADS1115 is used; select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV2048, MV4096, MV6144, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 1, 1, 1, 1 // can be a float
#define ADS_RATE  MS2 , MS9, MS9 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 10 , 20, 30, 50 // number of values used for averaging (must be between 1 and 254) 
//#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT_1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
// --------- 7 - RPM (rotations per minute) settings ---------
//#define MEASURE_RPM

// --------- 8 - Persistent memory settings ---------
//#define SAVE_TO_EEPROM
//#define PIN_PUSHBUTTON    2   // default is 10 but my own device is 2

// --------- 9 - Data to transmit ---------
#define VSPEED_SOURCE  FIRST_BARO     // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU or PPM_SELECTION
// ***** 9.1 - Frsky data *****
//#define VFAS_SOURCE   VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
//#define FUEL_SOURCE   ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define A3_SOURCE     ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define A4_SOURCE     ADS_VOLT_3                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define ACCX_SOURCE     TEST_1                   //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
//#define ACCY_SOURCE     TEST_2                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define ACCZ_SOURCE     TEST_3                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define T1_SOURCE       ADS_VOLT_1                   //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define T2_SOURCE       ADS_VOLT_4                    //  select between TEST_1, TEST_2, TEST_3, , GLIDER_RATIO, SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4

// ***** 9.2 - Hott data *****
#define CELL_UNDERVOLTAGE_WARNING 3300            // Warning threshold in mV; 
#define BATTERY_1_SOURCE          ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define BATTERY_2_SOURCE          ADS_VOLT_2                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define MAIN_BATTERY_SOURCE       ADS_VOLT_3                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define TEMPERATURE_1_SOURCE      TEST_1          //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
//#define TEMPERATURE_2_SOURCE      PPM             //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY, PPM


// ***** 9.3 - Multiplex data *****
#define SETUP_MULTIPLEX_DATA_TO_SEND    \
                        3 , REL_ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \
                        4 , VERTICAL_SPEED , 1 , 1 , 0, -500 , 500  , \
                        5 , ALTIMETER_MAX , 1 , 1 , 0 , -16384 , 16383 ,\
                        6 , CURRENTMA , 1 , 1, 0 , -16384 , 16383 ,\
                        7 , MILLIAH  , 1 , 1, 0 , -16384 , 16383
                        // 7 , CELL_TOT , 1 , 1 , 0 , -16384 , 16383 , \
                        // 4 , VOLT_4 , 1 , 1 , 0 , -16384 , 16383 , \
                        // 9 , PPM , 1 , 1 , 0 , -16384 , 16383 
                        // 3 , ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \

// ***** 9.4 - Jeti data *****
#define VOLTAGE_SOURCE   VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6

// --------- 10 - Sequencer ---------
//#define SEQUENCE_OUTPUTS 0b100000  
#define SEQUENCE_UNIT 100
#define SEQUENCE_m100    1 , 0b100000 , 1 , 0b000000 , 1 , 0b100000 , 1 , 0b000000 
#define SEQUENCE_m75     1 , 0b100000 , 1 , 0b000000 , 2 , 0b100000 , 2 , 0b000000
#define SEQUENCE_m50     5 , 0b100000 , 5 , 0b000000
#define SEQUENCE_m25     5 , 0b100000 , 5 , 0b000000 , 0 , 0b100000
#define SEQUENCE_0       3 , 0b100000 , 1 , 0b000000
#define SEQUENCE_25      2 , 0b100000 , 2 , 0b000000
#define SEQUENCE_50      5 , 0b100000 , 5 , 0b000000
#define SEQUENCE_75      7 , 0b100000 , 7 , 0b000000
#define SEQUENCE_100     8 , 0b100000 , 8 , 0b000000
#define SEQUENCE_LOW    10 , 0b100000 ,10 , 0b000000   // sequence for Low voltage
//#define SEQUENCE_MIN_VOLT_6 4000 // sequence_100 will be activated if voltage 6 is lower that the value.
#define SEQUENCE_MIN_CELL   3000 // sequence_100 will be activated if lowest cell is lower that the value.

// --------- 11 - GPS ------------------------------------------------------------------------------------------------
//#define GPS_INSTALLED     // uncomment this line if a GPS is connected
//#define GPS_SPEED_IN_KMH  // uncomment this line if GPS speed has to be sent in km/h instead of knot/h (only for Frsky protocol)
#define GPS_SPEED_3D      // uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test) 

// --------- 12 - IMU 6050 --- (accelerometer + gyro -----------------------------------------------------------------
//#define USE_6050 // uncomment this line if a IMU 6050 is connected
#define PIN_INT_6050 3   // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM)
#define ACC_OFFSET_X 0 // fill here the first value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the second value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the third value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)
//#define DISPLAY_ACC_OFFSET // used ONLY in order to display the acceleration offset on pc terminal; KEEP AS COMMENT once offsets have been setup 

// --------- xx - Reserved for developer. DEBUG must be activated here when we want to debug one or several functions in some other files. ---------
//#define DEBUG
//#define DEBUG_BLINK   // use by developper in order to blink the led without using uart for debugging


#define BASED_ON_AIRSPEED 0
#define BASED_ON_GPS_SPEED 1
#define AVERAGING_DELAY_MILLISEC  AVERAGING_TOLERANCE * 100  
#if defined( DISPLAY_ACC_OFFSET ) && defined( USE_6050 )
  #define DEBUG
#endif

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

#define SECONDS_SINCE_T0        32
#define AVERAGE_VSPEED_SINCE_TO 33 

#ifdef DEBUG
//#include "HardwareSerial.h"
#endif

#ifdef GPS_INSTALLED
//#include "HardwareSerial.h"
#endif

#endif// End define OXS_CONFIG_h


