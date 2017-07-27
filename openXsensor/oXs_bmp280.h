#ifndef OXS_BMP280_h
#define OXS_BMP280_h


#include "Arduino.h"
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "I2C.h"
#include "oXs_ms5611.h"

#define BMP280_ADR 0x77 // I2C address of BMP280 (can also be 0x76

/*=========================================================================
    CALIBRATION DATA for BMP280
    -----------------------------------------------------------------------*/
    typedef struct
    {
      uint16_t  dig_T1;
      int16_t  dig_T2;
      int16_t  dig_T3;
      uint16_t dig_P1; 
      int16_t dig_P2;
      int16_t dig_P3;
      int16_t  dig_P4;
      int16_t  dig_P5;
      int16_t  dig_P6;
      int16_t  dig_P7;
      int16_t  dig_P8;
      int16_t  dig_P9;
    } BMP280_CALIB_DATA;
/*=========================================================================*/

class OXS_BMP280 {
public:
#ifdef DEBUG  
  OXS_BMP280( HardwareSerial &print);
#else
  OXS_BMP280( void ) ;
#endif
  VARIODATA varioData ;
  void setup();
  bool  readSensor();
  void resetValues();

private:
  uint8_t _addr;
  
  uint8_t errorI2C ; //error code returned by I2C::Write and I2C::Read; 0 = no error
  bool errorCalibration ; // (true = error)
  void SendCommand(byte command);
  long getData(byte command, byte del);
  void calculateVario() ;
   uint16_t low, high;
/*
    int32_t D1 ;  
    int32_t D2 ;
    int32_t D2Prev ;
    int32_t D2Apply ;
    int64_t dT  ;
    int32_t TEMP  ;
    int64_t OFF, SENS;
    int16_t   alt_temp_compensation ;
*/
    int32_t altitude  ; // in cm * 100 
    int32_t altitudeLowPass  ;
    int32_t altitudeHighPass  ;
    int sensitivityMin ;
    

    unsigned long extended2Micros ; // used to temporarilly save micros() >> 1
    unsigned long pressureMicros ; // save time when program send command asking the MS5611 to get the pressure
    unsigned long pressureMicrosPrev1 ; // save the previous pressureMicros
    unsigned long pressureMicrosPrev2 ; // save the previous of the previous pressureMicros
    unsigned long altMillis ;
    unsigned long nextAltMillis   ;  // save when Altitude has to be calculated; altitude is available only after 3200 in order to get a stable value (less temperature drift)
    unsigned long nextAverageAltMillis  ; // save when AverageAltitude has to be calculated
      
    float climbRate2AltFloat  ;
    
    float abs_deltaClimbRate ;
      
#ifdef DEBUG
  HardwareSerial* printer;
#endif
}; // end class OXS_BMP280


#endif // OXS_BMP280




