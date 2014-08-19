
#include "Arduino.h"
#include "oxs_curr.h"
//#include "oxs_config.h" // already in .h file

//#define DEBUGCURRENT

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#ifdef DEBUG  
OXS_CURRENT::OXS_CURRENT(uint8_t pinCurrent, HardwareSerial &print)
#else
OXS_CURRENT::OXS_CURRENT(uint8_t pinCurrent)
#endif
{
  // constructor
#ifdef DEBUG  
  printer = &print; //operate on the address of print
#endif
  _pinCurrent=pinCurrent;
  pinMode(pinCurrent,INPUT); 
#ifdef DEBUG  
  printer->print("Current sensor on pin:");
  printer->println(pinCurrent);
#endif
}

// **************** Setup the Current sensor *********************
void OXS_CURRENT::setupCurrent( ) {
  currentData.milliAmpsAvailable = false;
  currentData.consumedMilliAmpsAvailable = false;
  currentData.sumCurrent = 0 ;
  resetValues();
}

// **************** Read the Current sensor *********************
#ifdef PIN_CurrentSensor
void OXS_CURRENT::readSensor() {
  static int cnt = 0;
//  static int cntMAmp =0;
  static unsigned long lastCurrentMillis = millis() ; 
//  static unsigned long UpdateMs=0;
  static unsigned long milliTmp = millis() ;
#ifdef USE_INTERNAL_REFERENCE
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use VRef as max, 1111 = measure ground level)
#else
  ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 01 00 1111 (01 = use Vcc as max, 1111 = measure ground level)
#endif
    delayMicroseconds(200); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done


  analogRead(_pinCurrent) ; // make a first read to let ADCMux to set up
  delayMicroseconds(200) ; // wait to be sure
  currentData.sumCurrent += analogRead(_pinCurrent) ; 
  cnt++ ;
  milliTmp = millis() ;
  if(  milliTmp > ( lastCurrentMillis + 200) ){   // calculate average once per 200 millisec
      currentData.milliAmps = ((currentData.sumCurrent / cnt) - offsetCurrentSteps ) * mAmpPerStep ;
       if (currentData.milliAmps < 0) currentData.milliAmps = 0 ;
	  currentData.milliAmpsAvailable = true ;
      if(currentData.minMilliAmps>currentData.milliAmps)currentData.minMilliAmps=currentData.milliAmps;
      if(currentData.maxMilliAmps<currentData.milliAmps)currentData.maxMilliAmps=currentData.milliAmps;
      currentData.sumCurrent = 0;
      currentData.floatConsumedMilliAmps += ((float) currentData.milliAmps) * (milliTmp - lastCurrentMillis ) / 3600.0 /1000.0 ;   // Mike , is this ok when millis() overrun????
      currentData.consumedMilliAmps = (int32_t) currentData.floatConsumedMilliAmps ;
      currentData.consumedMilliAmpsAvailable = true ;
      lastCurrentMillis =  milliTmp ;
#ifdef DEBUGCURRENT
      printer->print("At time  = ");
      printer->print(milliTmp);
      printer->print(" Cnt = ");
      printer->print(cnt);
      printer->print(" average current =  ");
      printer->print(currentData.milliAmps);
      printer->print(" consumed milliAmph =  ");
      printer->println(currentData.consumedMilliAmps);
#endif
      cnt = 0;
  }  
}
#endif


void OXS_CURRENT::resetValues(){
  currentData.consumedMilliAmps=0;
  currentData.floatConsumedMilliAmps=0;
  currentData.maxMilliAmps= 0;    // it is better to reset to 0 instead of the new value
  currentData.minMilliAmps= 0 ;   // it is better to reset to 0 instead of the new value
}




