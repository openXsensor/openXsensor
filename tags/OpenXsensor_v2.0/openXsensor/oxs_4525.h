#ifndef OXS_4525_h
#define OXS_4525_h

#include <Arduino.h>
#include "oxs_config.h"
#include "I2C.h"

//#define CALCULATEINTEGER

struct AIRSPEEDDATA {
  //bool available;          // true if data is available
  float smoothDifPressureAdc ;  // in steps ADC
  float temperature4525;     // in Celsius
  float rawAirSpeed ;       // cm/sec
  int32_t airSpeed ; // in km/h (no decimal)
  bool airSpeedAvailable ;
  int32_t compensation ; // in km/h (no decimal)
  bool compensationAvailable ;
  bool airspeedReset ;



   
  unsigned long lastCommand2Micros ; // used to avoid some task (reading voltage sensor, currentsensor, ..) when barometric data should be soon available for reading 

                                     // = micro divided by 2 to avoid overflow on comparison
  float difPressureAdc_zero ; 

};

extern float actualPressure ;

class OXS_4525 {
public:
#ifdef DEBUG  
  OXS_4525(uint8_t addr, HardwareSerial &print);
#else
  OXS_4525(uint8_t addr) ;
#endif
  AIRSPEEDDATA airSpeedData ;
//  int64_t rawPressure; // in 1/10000 mBar
  void setup();
  void  readSensor();


private:
  
//  unsigned int _calibrationData[7]; // The factory calibration data of the ms5611

  void SendCommand(byte command);
   uint8_t _addr;
   uint8_t I2CErrorCode4525 ;
   long getData(byte command, byte del);
   bool calibrated4525 ;
   int calibrateCount4525 ;
   int32_t difPressureSum ;
   
//   unsigned long extended2Micros ; // used to temporarilly save micros() >> 1
   uint8_t data[4];
   int32_t difPressureAdc;          // in steps ADC 
   int32_t temperature4525Adc ;   // in steps ADC
   
   float offset4525 ; 
   float difPressureAdc_0 ;
   float abs_deltaDifPressureAdc ;

   float expoSmooth4525_adc_auto ;
   float smoothAirSpeed ;    //cm/sec

   unsigned long  airSpeedMillis ; //save time when airspeed is made available
   unsigned long  nextAirSpeedMillis ; //next time that airspeed must be available
  
#ifdef DEBUG
  HardwareSerial* printer;
#endif
}; // end class OXS_4525

extern int32_t test1Value ;// used in order to test the transmission of any value
extern bool test1ValueAvailable ;
extern int32_t test2Value ;// used in order to test the transmission of any value
extern bool test2ValueAvailable ;
extern int32_t test3Value ;// used in order to test the transmission of any value
extern bool test3ValueAvailable ;


#endif // OXS_4525


