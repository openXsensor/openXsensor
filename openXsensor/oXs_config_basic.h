// OpenXsensor https://github.com/openXsensor/
// started by Rainer Schlosshan and maintained by Michel Strens

// This is version : 8.2.0 (18 april 2017)

//******************************************************************************************************************************************************* //
//                                                                                                                                                        //
// ! IMPORTANT NOTE !!!!!!!!!!   Another file in this project provides detailed explanations on how to set up this file (see oXs_config_description.h).   //
//                                                                                                                                                        //
//******************************************************************************************************************************************************* //
//  Note: the oXs_config_basic.h and oXs_config_advanced.h files present on the GitHub site are not always meaningful.
//       It could be that the combination of active/non active parameters is not consistent.
//       This is just the result of many updates and tests in this document.
//       So take always care to set up the files according to your needs and check carefully all options.
//  For old version 2 and 3 of oXs, there was a program named "configurator" running on PC that allowed easier editing of a oXs_config.h file
//       Still this configurator is not developped anymore. So this version request manual editing. 


#ifndef OXS_CONFIG_BASIC_h
#define OXS_CONFIG_BASIC_h

// --------- 1 - Telemetry protocol ---------  
#define PROTOCOL  FRSKY_SPORT  // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT, JETI

// ***** 1.1 - Pin connected to Rx ************                              See oXs_config_advanced.h (normally no need to change it) (default is arduino pin 4 connected to RX) 
// ***** 1.2 - SPORT_SENSOR_ID (used only for Frsky Sport protocol)  *****   See oXs_config_advanced.h (normally no need to change it) 

// --------- 2 - Data to transmit ---------   uncomment the lines below if you want to transmit some extra oXs measurement into some telemetry fields
// ***** 2.1 - Frsky data *****                
//#define VFAS_SOURCE   VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
//#define FUEL_SOURCE   ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define A3_SOURCE     ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define A4_SOURCE     ADS_VOLT_3                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define ACCX_SOURCE     TEST_1                   //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
//#define ACCY_SOURCE     TEST_2                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define ACCZ_SOURCE     TEST_3                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL , YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define T1_SOURCE       ADS_VOLT_1                   //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define T2_SOURCE       ADS_VOLT_4                    //  select between TEST_1, TEST_2, TEST_3, , GLIDER_RATIO, SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY, PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4

// ***** 2.2 - Hott data *****
#define CELL_UNDERVOLTAGE_WARNING 3300            // Warning threshold in mV; 
#define BATTERY_1_SOURCE          ADS_VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define BATTERY_2_SOURCE          ADS_VOLT_2                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define MAIN_BATTERY_SOURCE       ADS_VOLT_3                 // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
//#define TEMPERATURE_1_SOURCE      TEST_1          //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
//#define TEMPERATURE_2_SOURCE      PPM             //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY, PPM

// ***** 2.3 - Multiplex data *****
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

// ***** 2.4 - Jeti data *****
#define VOLTAGE_SOURCE   VOLT_1                     // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6

// ***** 2.5 - How to fill TEST_1, TEST_2, TEST_3  ******     see oXs_config_advanced.h (required only when some measurements have to be filled in TEST_1, TEST_2 or TEST_3

// --------- 3 - PPM settings ---------            see oXs_config_advanced.h (default, this option is not active)

// --------- 4 - Vario settings ---------
// ***** 4.1 - Connecting 1 or 2 barometric sensor(s)  *****
#define FIRST_BARO_SENSOR_USE     NO_BARO         // select between NO_BARO , MS5611, GY86 , BMP085 , BMP180 , BMP280  

// ***** 4.2 - Type of Vspeed to transmit  *****                                    
#define VSPEED_SOURCE  FIRST_BARO     // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU or PPM_SELECTION

// ***** 4.3 - Sensitivity predefined by program *****                               see oXs_config_advanced.h (normally no need to change it)
// ***** 4.4 - Sensitivity adjusted from the TX *****                                see oXs_config_advanced.h (normally no need to change it)
// ***** 4.5 - Hysteresis parameter & Alt compensation based on sensor temp *****    see oXs_config_advanced.h (normally no need to change it)
// ***** 4.6 - Vertical speeds calculations based on PPM  ****                       see oXs_config_advanced.h (change only if PPM_SELECTION is used)
// ***** 4.7 - Analog vertical speed *****                                           see oXs_config_advanced.h (change if you want to activate it)
// ***** 4.8  - Calculating glider ratio, average sink/climb rate **************     see oXs_config_advanced.h (change if you want to get those measurement)
// --------- 5 - Airspeed settings ---------                                         see oXs_config_advanced.h for additionnal parameters (normally no need to change them)
#define AIRSPEED_SENSOR_USE       NO_AIRSPEED       // select between NO_AIRSPEED , MS4525 , MPXV7002

// --------- 6 - Voltages & Current sensor settings ---------

// ***** 6.1 - Voltage Reference to measure voltages and current *****              see oXs_config_advanced.h when voltage reference is not Vcc and 5 volt

// ***** 6.2 - Voltage parameters *****                                             see oXs_config_advanced.h for additionnal parameters (scale , offset)
#define PIN_VOLTAGE         8  , 8     , 8   , 8    , 8   , 8               // set this line as comment if no one voltage has to be measured, set the value to 8 for the voltage(s) not to be measured.
#define RESISTOR_TO_GROUND  36 , 20    , 0  , 0 , 0  , 0               // set value to 0 when no divider is used for a voltage; can contains decimals 
#define RESISTOR_TO_VOLTAGE 670 , 100.1 , 200 , 39   , 500 , 0              // set value to 0 when no divider is used for a voltage; can contains decimals 

// ***** 6.3 - Max number of Lipo cells to measure (and transmit to Tx) *****
#define NUMBEROFCELLS 0                                                 // Put this line as comment or set value to 0 (zero) if you do not want to transmit cell voltages.

// ***** 6.4 - Convert voltage to temperature (° Celcius) *****                    see oXs_config_advanced.h if you want to measure temperature(s) with thermistor

// ***** 6.5 - Current parameters  *****                                           see oXs_config_advanced.h if you want to measure current with a current sensor

// ***** 6.6 - Ads1115 parameters  *****                                           see oXs_config_advanced.hif you want to measure voltages with higher accuracy using n ads1115 module

// --------- 7 - RPM (rotations per minute) settings ---------
#define CALCULATE_RPM     NO                                               // select between YES , NO       

// --------- 8 - Persistent memory settings ---------                              see oXs_config_advanced.h (normally not used)

// --------- 9 - GPS ---------------                                       see oXs_config_advanced.h for additionnal parameters (normally no need to change them)
#define A_GPS_IS_CONNECTED      YES                   // select between YES , NO

// --------- 10 - IMU 6050 --- (accelerometer + gyro)  and HMC5883 (magnetometer) --  see oXs_config_advanced.h for additionnal parameters e.g. about calibration
// ***** 10.1 - IMU 6050 *****
#define A_MPU6050_IS_CONNECTED      YES               // select between YES , NO

// ***** 10.2 - HMC5883 *****
#define CALCULATE_YAW_WITH_HMC5883   YES             // select between YES , NO ; YES requires that A_MPU6050_IS_CONNECTED is YES here above

// --------- 11 - Flow sensor ---------------                                       see oXs_config_advanced.h 

// --------- 20 - Sequencer ---------                                       see oXs_config_advanced.h (only when oXs has to generate signals in sequence)


#endif// End define OXS_CONFIG_BASIC_h


