#ifndef OXS_sdp3x_h
#define OXS_sdp3x_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"
#include "I2C.h"
#include "oXs_4525.h"

/*
struct AIRSPEEDDATA {
  float temperatureSdp3x;     // in Celsius , used when compensation is calculated
  float difPressure;          // in Pa
  struct ONE_MEASUREMENT airSpeed ;        // in 1/10 km/h or 1/10 knots (no decimal)
  float smoothAirSpeed ;    //cm/sec ; use in glider ratio
  //unsigned long lastCommand2Micros ; // used to avoid some task (reading voltage sensor, currentsensor, ..) when barometric data should be soon available for reading 
                                     // = micro divided by 2 to avoid overflow on comparison  
};
*/

extern float actualPressure ; 

class OXS_SDP3X {
public:
#ifdef DEBUG  
  OXS_SDP3X(uint8_t addr, HardwareSerial &print);
#else
  OXS_SDP3X(uint8_t addr) ;
#endif
  AIRSPEEDDATA airSpeedData ;
  void setup();
  void  readSensor();


private:
   uint8_t _addr;
   uint8_t I2CErrorCodeSdp3x ;
   float dpScaleSdp3x;      // differential pressure scale factor
   uint8_t data[2];                 // get 2 bytes returned by SDP3X
   float temperatureKelvinSdp3x;     // in Kelvin
//   float difPressure;          // in Pa
  
   float smoothDifPressure ;  // in Pa
   float expoSmooth_auto ;
   unsigned long  nextAirSpeedMillis ; //next time that airspeed must be available
   unsigned long  nextPressureReadMillis ; // next time that pressure can be read (datasheet says O.5msec)
  
#ifdef DEBUG
  HardwareSerial* printer;
#endif
}; // end class OXS_SDP3X

extern int32_t test1Value ;// used in order to test the transmission of any value
extern bool test1ValueAvailable ;
extern int32_t test2Value ;// used in order to test the transmission of any value
extern bool test2ValueAvailable ;
extern int32_t test3Value ;// used in order to test the transmission of any value
extern bool test3ValueAvailable ;


#endif // OXS_SDP3X


