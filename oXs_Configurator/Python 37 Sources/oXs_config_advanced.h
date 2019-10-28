#ifndef OXS_CONFIG_ADVANCED_h
#define OXS_CONFIG_ADVANCED_h
#define PIN_SERIALTX 4

#define         DATA_ID_VARIO  0x00
#define         DATA_ID_FLVSS  0xA1
#define         DATA_ID_FAS    0x22
#define         DATA_ID_GPS    0x83
#define         DATA_ID_RPM    0xE4
#define         DATA_ID_ACC    0x67
#define         DATA_ID_TX     0x0D

#define SECOND_BARO_SENSOR_USE    NO_BARO

#define SENSITIVITY_MIN 80
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000

#define VARIOHYSTERESIS 5

#define PIN_ANALOG_VSPEED 3
#define ANALOG_VSPEED_MIN  -3.0
#define ANALOG_VSPEED_MAX  3.0

#define USE_INTERNAL_REFERENCE

#define PIN_VOLTAGE  0 ,  1 , 8 , 8 , 8 , 8
#define RESISTOR_TO_GROUND 11.0 , 1.1 , 0.0 , 0.0 , 0.0 , 0.0
#define RESISTOR_TO_VOLTAGE 12.0 , 1.2 , 0.0 , 0.0 , 0.0 , 0.0
#define OFFSET_VOLTAGE 14.0 , 1.4 , 0.0 , 0.0 , 0.0 , 0.0
#define SCALE_VOLTAGE 13.0 , 1.3 , 1.0 , 1.0 , 1.0 , 1.0

#define PIN_CURRENTSENSOR 2
#define MVOLT_AT_ZERO_AMP 2.1
#define MVOLT_PER_AMP     2.2
#define RESISTOR_TO_GROUND_FOR_CURRENT 2.3
#define RESISTOR_TO_CURRENT_SENSOR     2.4

#define GPS_SPEED_3D
#define GPS_REFRESH_RATE 10

#endif
