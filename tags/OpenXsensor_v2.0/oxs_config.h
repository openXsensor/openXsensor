#ifndef OXS_CONFIG_h
#define OXS_CONFIG_h

//***********************************************************************************************************************
// Another file in this project (see oxs_config_description.h) provides detailed explanations on how to set up this file.
//***********************************************************************************************************************

//**** 1 FrSky device ID when Sport protocol is used 
#define SENSOR_ID    0x1B  

//**** 2 Hardware settings 
//****   2.1 Digital pins 
#define PIN_SERIALTX      4
#define PIN_PPM           2    // default is 2 but my own device use 3
#define PPM_MIN_100       980  //    1500 - 512 ; // pulse width (usec) when TX send a channel =  -100 
#define PPM_PLUS_100      1990 //    1500 + 512 ; // pulse width (usec) when TX send a channel =  +100 
//#define PIN_PUSHBUTTON    2   // default is 10 but my own device is 2 
//#define PIN_ANALOG_VSPEED 3

//**** 2.2 Analog Pins for voltages
#define PIN_VOLTAGE_1 1    
#define PIN_VOLTAGE_2 2    
//#define PIN_VOLTAGE_3 3    
//#define PIN_VOLTAGE_4 1    
//#define PIN_VOLTAGE_5 2    
//#define PIN_VOLTAGE_6 3    

//**** 2.3 Analog pin used for curent sensor
#define PIN_CURRENTSENSOR   3


//**** 3 Fields to transmit
//**** 3.5 General set up to define wich measurements are transmitted and how
#define SETUP_DATA_TO_SEND    \
                        DEFAULTFIELD , ALTIMETER , 1 , 1 , 0 ,\
                        VSpd , PPM_VSPEED , 1 , 1 ,0 , \
                        DEFAULTFIELD , AIR_SPEED , 1 , 1 ,0 , \
                        AccX , VERTICAL_SPEED , 1 , 1 ,0 , \
                        AccZ , PRANDTL_DTE , 1 , 1 ,0 ,\
                        T1 , VOLT1 , 1 , 1 ,0 , \
                        T2 , VOLT2 , 1, 1, 0 , \
                        DEFAULTFIELD , CURRENTMA , 1, 1, 0 , \
                        DEFAULTFIELD , MILLIAH, 1, 1, 0 , \
                        Fuel , SENSITIVITY , 1, 1, 0
                        

// **** 4 - Set up for vario
#define VARIO // set as comment if there is no vario  
//#define VARIO2 // set as comment if there is no second vario

#define SENSITIVITY_MIN_AT 100   
#define SENSITIVITY_MAX_AT 1000
#define SENSITIVITY_MIN 50 
#define SENSITIVITY_MAX 300 

#define SENSITIVITY_MIN_AT_PPM 10  // sensitivity will be changed by OXS only when PPM signal is between the specified range enlarged by -5 / +5
#define SENSITIVITY_MAX_AT_PPM 40 
#define SENSITIVITY_PPM_MIN  20    // common value for vario is 20
#define SENSITIVITY_PPM_MAX 100     // common value for vario is 100

#define VARIOHYSTERESIS 5

#define ANALOG_VSPEED_MIN -3 
#define ANALOG_VSPEED_MAX  3 

#define VARIO_PRIMARY       2 // 0 means first ms5611, 1 means second ms5611 , 2 means vario based on vario 1 + compensation from airspeed
#define VARIO_SECONDARY     0  // 0 means first ms5611, 1 means second ms5611 , 2 means vario based on vario 1 + compensation from airspeed
#define SWITCH_VARIO_MIN_AT_PPM 10 //
#define SWITCH_VARIO_MAX_AT_PPM 90 //


//**** 5 Set up for current sensor & 6 voltages
//**** 5.1 - Select the reference (VCC or 1.1 internal voltage reference)
//#define USE_INTERNAL_REFERENCE

//**** 5.2 Calibration parameters for current sensor
#define OFFSET_CURRENT_STEPS         0    
#define MAMP_PER_STEP                0.9775

//***** 5.3 - Calibration parameters for voltage measurements
#define OFFSET_1             0   
#define MVOLT_PER_STEP_1       4.89    
#define OFFSET_2             0    
#define MVOLT_PER_STEP_2       10.1    
//#define OFFSET_3             0       
//#define MVOLT_PER_STEP_3       1
//#define OFFSET_4             0        
//#define MVOLT_PER_STEP_4       1        
//#define OFFSET_5             0        
//#define MVOLT_PER_STEP_5       1        
//#define OFFSET_6             0        
//#define MVOLT_PER_STEP_6       1        

//**** 5.4 Number of Lipo cell to measure  (and transmit to Tx)
//#define NUMBEROFCELLS 3  // keep this line but set value to 0 (zero) if you do not want to transmit cell voltage.


//**** 6 - Set up for RPM (rotation per minute)
//#define MEASURE_RPM	1


//**** 7 - Set up of Persistent memory
//#define SAVE_TO_EEPROM      


//**** 8 - Set up of Airspeed
#define AIRSPEED    MS4525      

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90  
#define COMPENSATION_PPM_MIN  80  
#define COMPENSATION_PPM_MAX 140


//**** 9 Reserve for developer. DEBUG must be activated here when we want to debug one or several functions in some other files.
//#define DEBUG

#ifdef DEBUG
#include "HardwareSerial.h"
#endif

#endif// End define OXS_CONFIG_h


