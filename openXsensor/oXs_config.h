// OpenXsensor https://github.com/openXsensor/
// started by Rainer Schlosshan

//******************************************************************************************************************************************************* //
//                                                                                                                                                        //
// ! IMPORTANT NOTE !!!!!!!!!!   Another file in this project (see oXs_config_description.h) provides detailed explanations on how to set up this file.   //
//                                                                                                                                                        //
//******************************************************************************************************************************************************* //
//  Note: the oXs_config.h file present on the GitHub site is not always meaningful.
//       It could be that the combination of active/non active parameters is not consistent.
//       This is just the result of many updates and tests in this document.
//       So take always care to set up the oXs_config.h file according to your needs and check carefully all options.
//       You can also use the oXs configurator in order to generate automatically a valid file.


#ifndef OXS_CONFIG_h
#define OXS_CONFIG_h


// --------- 1 - Telemetry protocol ---------
#define PROTOCOL  MULTIPLEX  // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT

// --------- 2 - Serial data pin choice ---------
#define PIN_SERIALTX      4    // The pin which transmits the serial data to the FrSky telemetry receiver, Usually pin 4

// --------- 3 - PPM settings ---------
//#define PIN_PPM           3       // default is 2 but my own device use 3
#define PPM_MIN_100       980     // default 1500 - 512 ; // pulse width (usec) when TX sends a channel = -100
#define PPM_PLUS_100      1990    // default 1500 + 512 ; // pulse width (usec) when TX sends a channel = +100

// --------- 4 - Vario settings ---------

// ***** 4.1 - Connecting 1 or 2 MS5611 barometric sensor *****
#define VARIO // set as comment if there is no vario
#define SENSOR_IS_BMP180 // set as comment if baro sensor is MS5611
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
#define VARIOHYSTERESIS 0
//#define ALT_TEMP_COMPENSATION 800

// ***** 4.5 - Vertical speeds calculations *****
#define VARIO_PRIMARY       BARO_AND_IMU        // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
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

// --------- 5 - Airspeed settings ---------
//#define AIRSPEED    MS4525
#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h (openTx 2.0 expect knot/h while openTx 2.1 expect km/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 140

// --------- 6 - Voltages & Current sensor settings ---------

// ***** 6.1 - Voltage Reference to measure voltages and current *****
//#define USE_INTERNAL_REFERENCE  // uncomment this line if you use 1.1 volt internal reference instead of Vcc
//#define USE_EXTERNAL_REFERENCE  // uncomment this line if you use an external reference instead of Vcc
#define REFERENCE_VOLTAGE 4970    // set value in milliVolt; if commented, oXs will use or 1100 (if internal ref is used) or 5000 (if internal ref is not used) 

// ***** 6.2 - Voltage parameters *****
//#define PIN_VOLTAGE         0  , 1     , 2   , 8    , 8   , 8               // set this line as comment if no one voltage have to be measured, set a value to 8 for the voltage(s) not to be measured.
#define RESISTOR_TO_GROUND  12 , 20    , 30  , 19.8 , 50  , 60               // set value to 0 when no divider is used for a voltage, can contains decimals 
#define RESISTOR_TO_VOLTAGE 50 , 100.1 , 200 , 39   , 500 , 600              // set value to 0 when no divider is used for a voltage, can contains decimals 
#define OFFSET_VOLTAGE      0 , 0     , 0   , 0    , 0   , 0                // optionnal, can be negative, must be integer
#define SCALE_VOLTAGE      1.1 , 1     ,  1  , 1.004, 1   , 1                // optionnal, can be negative, can have decimals

// ***** 6.3 - Max number of Lipo cells to measure (and transmit to Tx) *****
//#define NUMBEROFCELLS 3   // Put this line as comment or set value to 0 (zero) if you do not want to transmit cell voltages.

// ***** 6.4 - Current parameters  *****
//#define PIN_CURRENTSENSOR   3
#define MVOLT_AT_ZERO_AMP              2500    // in millivolt
#define MVOLT_PER_AMP                  60      // in milliVolt per Amp
#define RESISTOR_TO_GROUND_FOR_CURRENT  19.8   // put as comment or set to 0 if no divider is used
#define RESISTOR_TO_CURRENT_SENSOR      39     // put as comment or set to 0 if no divider is used

// --------- 7 - RPM (rotations per minute) settings ---------
//#define MEASURE_RPM

// --------- 8 - Persistent memory settings ---------
//#define SAVE_TO_EEPROM
//#define PIN_PUSHBUTTON    2   // default is 10 but my own device is 2

// --------- 9 - Data to transmit ---------
#define VSPEED_SOURCE  FIRST_BARO       // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU or PPM_SELECTION
// ***** 9.1 - Frsky data *****
//#define VFAS_SOURCE     VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
//#define ACCX_SOURCE     TEST_3                   //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6 
//#define ACCY_SOURCE     VOLT_3                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
//#define ACCZ_SOURCE     TEST_3                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
#define T1_SOURCE       TEST_1                   //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
#define T2_SOURCE       TEST_2                      //  select between TEST_1, TEST_2, TEST_3, , GLIDER_RATIO, SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6

// ***** 9.2 - Hott data *****
//#define BATTERY_1_SOURCE          VOLT_4                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
//#define BATTERY_2_SOURCE          VOLT_2                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
//#define MAIN_BATTERY_SOURCE       VOLT_5                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6
//#define TEMPERATURE_1_SOURCE      TEST_1          //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
//#define TEMPERATURE_2_SOURCE      PPM             //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY, PPM


// ***** 9.3 - Multiplex data *****
#define SETUP_MULTIPLEX_DATA_TO_SEND    \
                        3 , ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \
                        6 , VERTICAL_SPEED , 1 , 1 , 0, -500 , 500 , \
                        5 , REL_ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \
                        7 , CELL_TOT , 1 , 1 , 0 , -16384 , 16383 , \
                        8 , ALTIMETER_MAX , 1 , 1 , 0 , -16384 , 16383 , \
                        4 , VOLT_4 , 1 , 1 , 0 , -16384 , 16383 , \
                        9 , PPM , 1 , 1 , 0 , -16384 , 16383 

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
#define GPS_SPEED_IN_KMH  // uncomment this line if GPS speed has to be sent in km/h instead of knot/h (only for Frsky protocol)
#define GPS_SPEED_3D      // uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test) 

// --------- 12 - IMU 6050 --- (accelerometer + gyro -----------------------------------------------------------------
//#define USE_6050 // uncomment this line if a IMU 6050 is connected
#define PIN_INT_6050 2    // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM)
#define ACC_OFFSET_X -160 // fill here the first value reported when DISPLAY_ACC_OFFSET is activated
#define ACC_OFFSET_Y -150 // fill here the second value reported when DISPLAY_ACC_OFFSET is activated
#define ACC_OFFSET_Z -1100 // fill here the third value reported when DISPLAY_ACC_OFFSET is activated
//#define DISPLAY_ACC_OFFSET // used ONLY in order to display the acceleration offset on pc terminal; KEEP AS COMMENT once offsets has been setup 

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

#define SECONDS_SINCE_T0        32
#define AVERAGE_VSPEED_SINCE_TO 33 



#ifdef DEBUG
//#include "HardwareSerial.h"
#endif

#ifdef GPS_INSTALLED
//#include "HardwareSerial.h"
#endif

#endif// End define OXS_CONFIG_h


