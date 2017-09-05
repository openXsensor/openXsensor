#include "oXs_curr.h"

#ifdef DEBUG
//#define DEBUGCURRENT
#endif

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
}

// **************** Setup the Current sensor *********************
void OXS_CURRENT::setupCurrent( ) {
  uint16_t tempRef ; 
  float currentDivider = 1.0 ;
#ifdef USE_INTERNAL_REFERENCE   
  analogReference(INTERNAL) ;
#elif defined(USE_EXTERNAL_REFERENCE)
    analogReference(EXTERNAL) ;
#endif
#if defined(USE_INTERNAL_REFERENCE) && defined(REFERENCE_VOLTAGE) && REFERENCE_VOLTAGE < 2000
  tempRef = REFERENCE_VOLTAGE  ;
#elif defined(USE_INTERNAL_REFERENCE) && defined(REFERENCE_VOLTAGE)
  #error REFERENCE_VOLTAGE must be less than 2000 when USE_INTERNAL_REFERENCE is defined
#elif defined(USE_EXTERNAL_REFERENCE)
#ifndef REFERENCE_VOLTAGE
  #error REFERENCE_VOLTAGE must be defined when USE_EXTERNAL_REFERENCE is defined
#else
  tempRef = REFERENCE_VOLTAGE  ;
#endif
#elif defined(USE_INTERNAL_REFERENCE)
  tempRef = 1100 ;
#elif defined(REFERENCE_VOLTAGE) && REFERENCE_VOLTAGE > 2000
  tempRef = REFERENCE_VOLTAGE  ;
#elif defined(REFERENCE_VOLTAGE)
  #error REFERENCE_VOLTAGE must be greater than 2000 when USE_INTERNAL_REFERENCE is not defined
#else 
  tempRef = 5000 ;
#endif  
#if defined(RESISTOR_TO_GROUND_FOR_CURRENT) && defined(RESISTOR_TO_CURRENT_SENSOR)
  if ( RESISTOR_TO_GROUND_FOR_CURRENT > 0 && RESISTOR_TO_CURRENT_SENSOR > 0) {
    currentDivider = 1.0 * (RESISTOR_TO_GROUND_FOR_CURRENT + RESISTOR_TO_CURRENT_SENSOR ) / RESISTOR_TO_GROUND_FOR_CURRENT ;
  }
#endif 
  offsetCurrentSteps =  1023.0 * MVOLT_AT_ZERO_AMP / tempRef / currentDivider;
  mAmpPerStep =  currentDivider * tempRef / MVOLT_PER_AMP / 1.023 ; 

  currentData.milliAmps.available = false;
  currentData.consumedMilliAmps.available = false;
//  currentData.sumCurrent = 0 ;
  resetValues();
#ifdef DEBUG  
  printer->print("Current sensor on pin:");
  printer->println(_pinCurrent);
  printer->print("Reference voltage:");
  printer->println(tempRef);
  printer->print("Offset for current:");
  printer->println(offsetCurrentSteps);
  printer->print("mAmp per step:");
  printer->println(mAmpPerStep);
  printer->print(" milli=");  
  printer->println(millis());
#endif
  
}


// **************** Read the Current sensor *********************
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES)
void OXS_CURRENT::readSensor() {
  static int cnt = 0;
//  static int cntMAmp =0;
  static unsigned long lastCurrentMillis = millis() ; 
//  static unsigned long UpdateMs=0;
  static unsigned long milliTmp = millis() ;
#ifdef USE_INTERNAL_REFERENCE
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use internal VRef as max, 1111 = measure ground level)
#elif defined(USE_EXTERNAL_REFERENCE)
  ADMUX =  _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);                          // binary = 00 00 1111 (00 = use external VRef as max, 1111 = measure ground level)
#else
  ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);             // binary = 01 00 1111 (01 = use Vcc as max, 1111 = measure ground level)
#endif
  delayMicroseconds(200); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done


  analogRead(_pinCurrent) ; // make a first read to let ADCMux to set up
  delayMicroseconds(200) ; // wait to be sure
  //currentData.sumCurrent += analogRead(_pinCurrent) ; 
  sumCurrent += analogRead(_pinCurrent) ; 
  cnt++ ;
  milliTmp = millis() ;
  if(  ( milliTmp - lastCurrentMillis) > 200 ) {   // calculate average once per 200 millisec
      currentData.milliAmps.value = ((sumCurrent / cnt) - offsetCurrentSteps ) * mAmpPerStep ;
//      if (currentData.milliAmps.value < 0) currentData.milliAmps.value = 0 ;
	  currentData.milliAmps.available = true ;
//      if(currentData.minMilliAmps>currentData.milliAmps)currentData.minMilliAmps=currentData.milliAmps;
//      if(currentData.maxMilliAmps<currentData.milliAmps)currentData.maxMilliAmps=currentData.milliAmps;
      sumCurrent = 0;
      floatConsumedMilliAmps += ((float) currentData.milliAmps.value) * (milliTmp - lastCurrentMillis ) / 3600.0 /1000.0 ;   // Mike , is this ok when millis() overrun????
      currentData.consumedMilliAmps.value = (int32_t) floatConsumedMilliAmps ;
      currentData.consumedMilliAmps.available = true ;
      lastCurrentMillis =  milliTmp ;
#ifdef DEBUGCURRENT
      printer->print("At time  = ");
      printer->print(milliTmp);
      printer->print(" Cnt = ");
      printer->print(cnt);
      printer->print(" average current =  ");
      printer->print(currentData.milliAmps.value);
      printer->print(" consumed milliAmph =  ");
      printer->println(currentData.consumedMilliAmps.value);
#endif
      cnt = 0;
  }  
}
#endif // end of readSensor

void OXS_CURRENT::resetValues(){
  currentData.consumedMilliAmps.value=0;
  floatConsumedMilliAmps=0;
}





