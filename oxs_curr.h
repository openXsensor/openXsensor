#ifndef OXS_CURRENT_h
#define OXS_CURRENT_h

#include "Arduino.h"
#include "oxs_config.h"

struct CURRENTDATA {
//  bool available;          // true if data is available
  int32_t milliAmps;       // in mA
  bool milliAmpsAvailable;
  int32_t consumedMilliAmps; // in mA
  float floatConsumedMilliAmps; // in mA
  bool consumedMilliAmpsAvailable;
  
  int32_t sumCurrent ;
//  int32_t sumForMampH ;
 
  int32_t maxMilliAmps;       // in mA
  int32_t minMilliAmps;       // in mA
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

};

#endif // OXS_CURRENT_h



