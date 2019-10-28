from oxsConfig import *

#define ADS_MEASURE A0_TO_A1 ,  ADS_OFF , ADS_OFF , ADS_OFF // select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV2048, MV4096, MV6144, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 1, 1, 1, 1 // can be a float
#define ADS_RATE  MS2 , MS9, MS9 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 10 , 20, 30, 50 // number of values used for averaging (must be between 1 and 254) 
#//#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT_1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
