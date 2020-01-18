#ifndef OXS_CURRENT_h
#define OXS_CURRENT_h

#include <Arduino.h>
#include "oXs_config_basic.h"
#include "oXs_config_advanced.h"
#include "oXs_config_macros.h"

struct CURRENTDATA {
  struct ONE_MEASUREMENT milliAmps;       // in mA
  struct ONE_MEASUREMENT consumedMilliAmps;       // in mA
  //int32_t consumedMilliAmps; // in mA
  //bool consumedMilliAmpsAvailable;
};



class OXS_CURRENT {
public:
#ifdef DEBUG  
  OXS_CURRENT(uint8_t pinCurrent, HardwareSerial &print);
#else
  OXS_CURRENT(uint8_t pinCurrent) ;
#endif
  CURRENTDATA currentData ;
  void setupCurrent() ;
  void readSensor();
  void resetValues();

private:
#ifdef DEBUG  
  HardwareSerial* printer;
#endif
  byte _pinCurrent;
  float offsetCurrentSteps ;
  float mAmpPerStep ; 
  float floatConsumedMilliAmps; // in mA
  int32_t sumCurrent ;
};

#endif // OXS_CURRENT_h




