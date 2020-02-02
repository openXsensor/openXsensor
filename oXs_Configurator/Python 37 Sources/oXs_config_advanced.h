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

#define PPM_VIA_SPORT 

#define SECOND_BARO_SENSOR_USE     NO_BARO

#define SENSITIVITY_MIN 80
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000
#define SENSITIVITY_MIN_AT_PPM 10
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20
#define SENSITIVITY_PPM_MAX 100

#define VARIOHYSTERESIS 5

#endif
