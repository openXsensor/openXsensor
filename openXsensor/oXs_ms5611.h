#ifndef OXS_MS5611_h
#define OXS_MS5611_h


#include "Arduino.h"
#include "oXs_config.h"
#include "I2C.h"

struct VARIODATA {
  int32_t temperature;     // in 1/10 Celsius

  int64_t rawPressure ;  // in 1/10000 mBar so = Pa * 10000
  
  byte SensorState ;
  int32_t rawAltitude ; // in cm * 100  
  int32_t absoluteAlt;     // in cm  
  bool absoluteAltAvailable ;
  bool altitudeAt20MsecAvailable   ;  // use to say to readsensors() that an altitude is available and that dte can be calculated.
  int32_t altOffset ;     // in cm
  bool relativeAltAvailable ;   
  int32_t relativeAlt ;     // in cm
  float delaySmooth ; // smoothed delay between 2 altitude calculations
  
  int32_t prevAlt[20] ;   // table contains the 20 latest altitude
  byte idxPrevAlt ;       // index of last entry in table
  int32_t vSpeed10Sec; // Altitude gain/loose between 10 sec (is calculated and send every 500 msec)
  bool vSpeed10SecAvailable ;
  
  float climbRateFloat  ;  // in cm/sec but as float
  int32_t climbRate;       // in cm /sec = vertical speed
  bool climbRateAvailable ;
  bool switchClimbRateAvailable ; // use to say to the readsensors loop that that a climbrate is available (to select the one being send)
  bool averageClimbRateAvailable ; // use to say to the readsensors loop that that a climbrate is available (to select the one being send)
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
  //int64_t rawPressure; // in 1/10000 mBar
  void setup();
  void  readSensor();

  void resetValues();

private:
  uint8_t _addr;
  unsigned int _calibrationData[7]; // The factory calibration data of the ms5611
  uint8_t errorI2C ; //error code returned by I2C::Write and I2C::Read; 0 = no error
  bool errorCalibration ; // (true = error)
  void SendCommand(byte command);
  long getData(byte command, byte del);
  unsigned int low, high;
    int32_t D1 ;  
    int32_t D2 ;
    int32_t D2Prev ;
    int32_t D2Apply ;
    int64_t dT  ;
    int32_t TEMP  ;
    int64_t OFF, SENS;

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
}; // end class OXS_MS5611


#endif // OXS_MS5611




