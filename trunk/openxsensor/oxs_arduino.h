#ifndef OXS_ARDUINO_h
#define OXS_ARDUINO_h

#include "Arduino.h"
#include "oxs_config.h"

struct ARDUINODATA {
  bool available;    // to remove afterward
  uint16_t vrefMilliVolts;          // in mV the internal measured voltage Reference ; to remove afterward
  
  
  int32_t mVolt[6] ;             // in mV 
  bool mVoltAvailable[6] ;
  
  byte mVoltPin[6] ;            // Arduino pin number to use to read each voltage (See hardware setting in oxs_config.h)  
  int offset[6] ;               // offset to apply while converting ADC to millivolt (See setting in oxs_config.h)  
  float mVoltPerStep[6] ;       // rate to apply while converting ADC to millivolt (See setting in oxs_config.h)  
  bool atLeastOneVoltage ;      // true if there is at least one voltage to measure
  
  int32_t sumVoltage[6] ;       // use to calculate average voltage     

  uint32_t mVoltCell_1_2 ;
  bool mVoltCell_1_2_Available ;
  uint32_t mVoltCell_3_4 ;
  bool mVoltCell_3_4_Available ;
  uint32_t mVoltCell_5_6 ;
  bool mVoltCell_5_6_Available ;
};

//#define VOLT_BUFFER_LENGTH 20   // not used anymore;averages are calculated at fix time

class OXS_ARDUINO {
  public:
#ifdef DEBUG  
    OXS_ARDUINO(HardwareSerial &print);
#else
    OXS_ARDUINO( uint8_t x );
#endif
    ARDUINODATA arduinoData ;
	void setupDivider( void );
	void readSensor();
	void resetValues();
    
  private:
#ifdef DEBUG  
     HardwareSerial* printer;
#endif
     int readVoltage( int value) ;  // read the voltage from the sensor specify by value
     void voltageNrIncrease() ; 
     uint32_t calculateCell(int32_t V0 , int32_t V1 , int32_t V2 , int cellId) ;  
};

#endif

