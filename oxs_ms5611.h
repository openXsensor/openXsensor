#ifndef OXS_MS5611_h
#define OXS_MS5611_h

#include "Arduino.h"
#include "oxs_config.h"

struct VARIODATA {
  bool available;          // true if data is available
//  int64_t pressure;          // in 1/10000 mBar 
//  int16_t temperature;     // in 1/10 Celsius

  
  byte SensorState ;
  int32_t absoluteAlt;     // in cm * 100
  bool absoluteAltAvailable ;
  int32_t relativeAlt;	   // in cm
  int32_t maxRelAlt;       // in cm
  int32_t minRelAlt;       // in cm
  int32_t maxAbsAlt;       // in cm
  int32_t minAbsAlt;       // in cm
  
  int16_t prevAlt[20] ;   // table contains the 20 latest altitude
  byte idxPrevAlt ;       // index of last entry in table
  int32_t vSpeed10Sec; // Altitude gain/loose between 10 sec (is calculated and send every 500 msec)
  bool vSpeed10SecAvailable ;
  

  int32_t climbRate;       // in cm 
  bool climbRateAvailable ;
  int32_t minClimbRate;    // in cm
  int32_t maxClimbRate;    // in cm

  int32_t altOffset;       // in cm    
  
  bool sensitivityAvailable ;  //used to decide if sensivityPpm can be sent or not
  int sensitivityPpm ;      // sensivity to apply when PPM is used. Value has to be divided by 1000 in order to calculate the smoothing parameter
  int sensitivity ; 
  
  unsigned long lastCommand2Micros ; // used to avoid some task (reading voltage sensor, currentsensor, ..) when barometric data should be soon available for reading 
                                      // = micro divided by 2 to avoid overflow on comparison
};

class OXS_MS5611 {
public:
#ifdef DEBUG  
  OXS_MS5611(uint8_t addr, HardwareSerial &print);
#else
  OXS_MS5611(uint8_t addr) ;
#endif
  VARIODATA varioData ;
  int64_t rawPressure; // in 1/10000 mBar
  void setup();
  void  readSensor();

  void resetValues();

private:
  uint8_t _addr;
  unsigned int _calibrationData[7]; // The factory calibration data of the ms5611
  void SendCommand(byte command);
  long getData(byte command, byte del);
#ifdef DEBUG
  HardwareSerial* printer;
#endif
}; // end class OXS_MS5611


#endif // OXS_MS5611



