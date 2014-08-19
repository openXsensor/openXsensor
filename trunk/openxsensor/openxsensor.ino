#include "oxs_config.h"
#include "oxs_arduino.h"
#include "oxs_ms5611.h"
#include "oxs_curr.h"
#include "oxs_out_frsky.h"

#ifdef SAVE_TO_EEPROM
  #include <EEPROM.h>
  #include "EEPROMAnything.h"
#endif

#include "Aserial.h"

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
static unsigned long extended2Micros ;

                             // Mike, I propose to use a general DEBUG in config.h to say if Serial will be used or not   
                             // Then each file could use is ouwn #define DEBUGxxX
//#define DEBUG

                            // Mike better to define VARIO in config.h
                            
// Mike a user had compiling error if the functions were not defined first. So I added those lines
int freeRam () ;
void checkButton() ;
void readSensors() ;
void Reset1SecButtonPress() ;
void Reset3SecButtonPress() ; 
void Reset10SecButtonPress() ;
void SaveToEEProm() ;
void LoadFromEEProm() ; 
void ProcessPPMSignal() ;
unsigned int ReadPPM() ;

volatile bool RpmSet = false ;
volatile uint16_t Rpm = 60 ;
unsigned long lastRpmMillis ;
bool RpmAvailable = false ;

int PWRValue; // calculation field for Vertical speed on PWR
unsigned long lastMillisPWR ;

// Create instances of the used classes
                        // Mike, there is no need creating this class if there is no voltage to measure because this version does not read Internal ref regarding VCC anymore (even for measuring current)
  // to do : adapt the test on defined (PIN_Voltage...) because actual config.h expects now that all PIN_Voltage are defined ; a value > 7 means it is not used. 
#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 ) 
#ifdef DEBUG  
OXS_ARDUINO oxs_Arduino(Serial);
#else
OXS_ARDUINO oxs_Arduino(0);
#endif  //DEBUG
#endif

#ifdef VARIO
#ifdef DEBUG  
OXS_MS5611 oxs_MS5611(I2CAdd,Serial);
#else
OXS_MS5611 oxs_MS5611(I2CAdd);
#endif  //DEBUG
#endif

#ifdef PIN_CurrentSensor
#ifdef DEBUG  
OXS_CURRENT oxs_Current(PIN_CurrentSensor,Serial);
#else
OXS_CURRENT oxs_Current(PIN_CurrentSensor);
#endif //DEBUG
#endif

#ifdef DEBUG  
OXS_OUT_FRSKY oxs_OutFrsky(PIN_SerialTX,Serial);
#else
OXS_OUT_FRSKY oxs_OutFrsky(PIN_SerialTX);
#endif

                            // Mike I do not understand this instruction; could you explain
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))

//************************************************************************************************** Setup()
void setup(){

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("openXsensor starting..");
  Serial.print("freeRam=");  
  Serial.println(freeRam());
#endif 

#ifdef PIN_PushButton  
  pinMode(PIN_PushButton, INPUT_PULLUP);
#endif
  pinMode(PIN_LED, OUTPUT); // The signal LED (used for the function push button)

  // Invoke all setup methods and set reference
  // to do : adapt the test on defined (PIN_Voltage...) because actual config.h expects now that all PIN_Voltage are defined ; a value > 7 means it is not used. 
#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 ) 
  oxs_Arduino.setupDivider(); 
  oxs_OutFrsky.arduinoData=&oxs_Arduino.arduinoData; 
#endif


#ifdef VARIO
  oxs_MS5611.setup();
  oxs_OutFrsky.varioData=&oxs_MS5611.varioData; 
#ifdef PIN_AnalogClimbRate
  lastMillisPWR = 3500 ; // So we will wait for 3.5 sec before generating a Vertical speed on PWM
  analogWrite(PIN_AnalogClimbRate,255/5*1.6); // initialize the output pin 
#endif
#endif // vario

//micros();

#ifdef PIN_CurrentSensor
  oxs_Current.setupCurrent( );
  oxs_OutFrsky.currentData=&oxs_Current.currentData;
#endif

  oxs_OutFrsky.setup();

#ifdef SAVE_TO_EEPROM
  LoadFromEEProm();
#endif

#ifdef PPM_INTERRUPT
	PORTD |= PPM_PIN_HEX ;	// Pullup resistor
	DDRD &= ~PPM_PIN_HEX ;	// Input
	EICRA |= PPM_INT_MASK ;		// Interrupt on rising edge
	EIFR = PPM_INT_BIT ;			// Clear interrupt flag
	EIMSK |= PPM_INT_BIT ;		// Enable interrupt
#endif
#ifdef DEBUG
  Serial.println("End of general set up");
#endif
} // ******************** end of Setup



void loop(){  //******************************************************************************** Loop()

//#ifdef DEBUG
//  Serial.println("in main loop");  
//#endif 

//delay(100);
  
#ifdef PIN_PushButton
  // Check if a button has been pressed
#ifdef VARIO
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < oxs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros < (oxs_MS5611.varioData.lastCommand2Micros + 3500)  ) checkButton(); // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#else
    checkButton();
#endif    
#endif 
    
    readSensors(); // read all sensors)
    oxs_OutFrsky.sendData(); // choice which data can be send base on availability and some priority logic 
    
#ifdef DEBUG
    //OutputToSerial() ; remove commend to debug all data in sensors
#endif
    
    // PPM Processing
    // Read the ppm Signal from receiver
#ifdef VARIO
#ifdef PIN_PPM 
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < oxs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros < (oxs_MS5611.varioData.lastCommand2Micros + 3500)  ) ProcessPPMSignal();  // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#endif //PIN_PPM

#ifdef PIN_AnalogClimbRate
    if (millis() > lastMillisPWR + 100) {
        extended2Micros = micros() >> 1 ;
        if (extended2Micros < oxs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
        if ( extended2Micros < (oxs_MS5611.varioData.lastCommand2Micros + 3500)  ) { // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
            PWRValue=map( (long) oxs_MS5611.varioData.climbRate,OutputClimbRateMin*100,OutputClimbRateMax*100,0,255/5*3.2);
            PWRValue=constrain(PWRValue, 0, 255/5*3.2 ) ;
            analogWrite(PIN_AnalogClimbRate,(int)PWRValue);
            lastMillisPWR = millis() ; 
        }
    }  
#endif
#endif //VARIO 

#ifdef SAVE_TO_EEPROM
  static unsigned long LastEEPromMs=millis();
  if (millis()>LastEEPromMs+10000){ // Save Persistant Data To EEProm every 10 seconds
    LastEEPromMs=millis();
    SaveToEEProm();
  }
#endif
}          // ****************   end of main loop *************************************


void readSensors() {   // ****************** Read all the sensors / Inputs

#ifdef VARIO
  oxs_MS5611.readSensor(); // Read pressure & temperature on MS5611, calculate Altitude and vertical speed
#endif

//#ifdef DEBUG
//  Serial.println("in readSensors");  
//#endif 

  if (oxs_Arduino.arduinoData.atLeastOneVoltage ) {
#ifdef VARIO
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < oxs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros < (oxs_MS5611.varioData.lastCommand2Micros + 3500)   )  { // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
        oxs_Arduino.readSensor();
    }    // read voltage only if there enough time to avoid delaying vario reading
#else
    oxs_Arduino.readSensor(); 
#endif
  }


#ifdef PIN_CurrentSensor
#ifdef VARIO
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < oxs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros < (oxs_MS5611.varioData.lastCommand2Micros + 3500)   ) oxs_Current.readSensor() ; // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#else
  oxs_Current.readSensor(); // Read current
#endif             // End Vario  
#endif             // End PIN_CurrentSensor

#ifdef MEASURE_RPM
  if (millis() > ( lastRpmMillis + 200) ){  // allow transmission of RPM only once every 200 msec
        if (RpmSet = true) {
            RpmSet = false ;
        } else {
            Rpm = 0 ;
        }
        RpmAvailable = true ;    
        lastRpmMillis = millis() ;
  }      
#endif
    
    


  //Serial.println("Go out of read sensor");
}                  // **************end of readSensors


#ifdef PIN_PushButton   // // ****************** check button
void checkButton() {
  static int lastSensorVal=HIGH;
  static unsigned int buttonDownMs;
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(PIN_PushButton);
  if (sensorVal == HIGH) {
    // button not pressed released
    digitalWrite(PIN_LED, LOW);
  }
  else {
    //button is currently being pressed down
    unsigned int buttonPressDuration=millis()-buttonDownMs;

    if( (buttonPressDuration>1000) and (buttonPressDuration<1200) )
      digitalWrite(PIN_LED, LOW); // Blink after 1 second

    else if( (buttonPressDuration>3000) and (buttonPressDuration<3200) )
      digitalWrite(PIN_LED, LOW); // Blink 1 after 3 second
    else if( (buttonPressDuration>3300) and (buttonPressDuration<3400) )
      digitalWrite(PIN_LED, LOW); // Blink 2 after 3 second

    else if( (buttonPressDuration>10000) and (buttonPressDuration<10200) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else if( (buttonPressDuration>10300) and (buttonPressDuration<10400) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else if( (buttonPressDuration>10500) and (buttonPressDuration<10600) )
      digitalWrite(PIN_LED, LOW); // Blink after 10 second
    else digitalWrite(PIN_LED, HIGH);
  }
  if( (lastSensorVal==HIGH) && (sensorVal==LOW))
  { // Button has been pressed down
    buttonDownMs=millis();
  }
  if( (lastSensorVal==LOW) && (sensorVal==HIGH))
  { // Button has been released
    unsigned int buttonPressDuration=millis()-buttonDownMs;
    //Serial.print("Button Released after ms:");
    //Serial.println(buttonPressDuration);
    // Do Something after certain times the button has been pressed for....
    if( (buttonPressDuration>=100) and (buttonPressDuration<3000) )
      Reset1SecButtonPress();
    else if( (buttonPressDuration>=3000) and (buttonPressDuration<5000) )
      Reset3SecButtonPress();
    //else if( (buttonPressDuration>=5000) and (buttonPressDuration<10000) )
    else if( (buttonPressDuration>=10000) and (buttonPressDuration<15000) )
      Reset10SecButtonPress();
  }

  lastSensorVal=sensorVal;
}  // End checkButton 

//ToDo: implement different reset actions on button press
void Reset1SecButtonPress()
{
#ifdef DEBUG
  Serial.println("================================================== Reset 0.1-3 sec");
#endif

#ifdef PIN_CurrentSensor
  struct CURRENTDATA *cd = &oxs_Current.currentData ;
  FORCE_INDIRECT(cd) ;
  cd->maxMilliAmps=cd->milliAmps;
  cd->minMilliAmps=cd->milliAmps;
#endif

#ifdef VARIO
  oxs_MS5611.resetValues() ;
#endif

#ifdef PIN_VOLTAGE_DIVIDER
  oxs_Arduino.resetValues();
#endif
}

void Reset3SecButtonPress()
{
#ifdef DEBUG
  Serial.println("================================================== Reset 3-5 sec");
#endif
#ifdef PIN_CurrentSensor
  oxs_Current.currentData.consumedMilliAmps = 0;
  oxs_Current.currentData.floatConsumedMilliAmps = 0 ;
#endif
}

void Reset10SecButtonPress()
{
#ifdef DEBUG
  Serial.println("Reset 5-10");
#endif
}

#endif



#ifdef SAVE_TO_EEPROM
/****************************************/
/* SaveToEEProm => save persistant Data */
/****************************************/
void SaveToEEProm(){
  static byte state=0;
  static int adr=0;
#ifdef DEBUG
  Serial.print("++++++++++++ SAving to EEProm:");    
  Serial.println(state);    
#endif
  switch (state){
  case 0:
#ifdef PIN_CurrentSensor
    adr+=EEPROM_writeAnything(adr, oxs_Current.currentData.consumedMilliAmps);
#endif
  case 1:
#ifdef PIN_CurrentSensor
    adr+=EEPROM_writeAnything(adr, oxs_Current.currentData.maxMilliAmps);
#endif
  case 2:
#ifdef PIN_CurrentSensor
    adr+=EEPROM_writeAnything(adr, oxs_Current.currentData.minMilliAmps);
#endif
#ifdef VARIO
  case 3:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.maxRelAlt);
  case 4:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.minRelAlt);
  case 5:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.maxAbsAlt);
  case 6:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.minAbsAlt);
  case 7:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.maxClimbRate);
  case 8:
    adr+=EEPROM_writeAnything(adr, oxs_MS5611.varioData.minClimbRate);
#endif //VARIO
  }

  state++;
  if(state >8){
    state=0;
    adr=0;
  }
}

/******************************************/
/* LoadFromEEProm => load persistant Data */
/******************************************/
void LoadFromEEProm(){
  int adr=0;

  // Store the last known value from the ppm signal to the eeprom
  Serial.println("-------------- Restored from EEProm: ---------------");
#ifdef PIN_CurrentSensor
  adr+=EEPROM_readAnything(adr, oxs_Current.currentData.consumedMilliAmps);
  oxs_Current.currentData.floatConsumedMilliAmps = oxs_Current.currentData.consumedMilliAmps ;
  Serial.print(" mAh=");    
  Serial.print( oxs_Current.currentData.consumedMilliAmps);

  adr+=EEPROM_readAnything(adr, oxs_Current.currentData.maxMilliAmps);
  Serial.print(" maxMilliAmps=");    
  Serial.print( oxs_Current.currentData.maxMilliAmps);

  adr+=EEPROM_readAnything(adr, oxs_Current.currentData.minMilliAmps);
  Serial.print(" minMilliAmps=");    
  Serial.print( oxs_Current.currentData.minMilliAmps);
#endif
#ifdef VARIO
  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.maxRelAlt);
  Serial.print(" maxRelAlt=");    
  Serial.print( oxs_MS5611.varioData.maxRelAlt);

  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.minRelAlt);
  Serial.print(" minRelAlt=");    
  Serial.print( oxs_MS5611.varioData.minRelAlt);


  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.maxAbsAlt);
  Serial.print(" maxAbsAlt=");    
  Serial.print( oxs_MS5611.varioData.maxAbsAlt);

  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.minAbsAlt);
  Serial.print(" minAbsAlt=");    
  Serial.print( oxs_MS5611.varioData.minAbsAlt);

  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.maxClimbRate);
  Serial.print(" minClimbRate=");    
  Serial.print( oxs_MS5611.varioData.minClimbRate);

  adr+=EEPROM_readAnything(adr, oxs_MS5611.varioData.minClimbRate);
  Serial.print(" maxClimbRate=");    
  Serial.print( oxs_MS5611.varioData.maxClimbRate);
#endif //VARIO
}
#endif //saveToEeprom


/**********************************************************/
/* ProcessPPMSignal => read PPM signal from receiver and  */
/*   use its value to adjust sensitivity                  */
/**********************************************************/
#ifdef PIN_PPM
void ProcessPPMSignal(){
  unsigned int ppm= ReadPPM();
  if (ppm>0){
#ifdef VARIO
    ppm =constrain(ppm, PPM_RANGE_MIN,PPM_RANGE_MAX ) ; 
    oxs_MS5611.varioData.sensitivityPpm=map(ppm, PPM_RANGE_MIN,PPM_RANGE_MAX,SENSITIVITY_PPM_MIN,SENSITIVITY_PPM_MAX); // map value and change stepping to 10
#endif
  }
}


/**********************************************************/
/* ReadPPM => read PPM signal from receiver               */
/*   pre-evaluate its value for validity                  */
/**********************************************************/

#ifdef PPM_INTERRUPT
uint16_t StartTime ;
uint16_t EndTime ;
uint8_t PulseTime ;		// A byte to avoid 

#if PIN_PPM == 2
ISR(INT0_vect, ISR_NOBLOCK)
#else
ISR(INT1_vect, ISR_NOBLOCK)
#endif
{
	cli() ;
	uint16_t time = TCNT1 ;	// Read timer 1
	sei() ;
	if ( EICRA & PPM_INT_EDGE )
	{
		StartTime = time ;
		EICRA &= ~PPM_INT_EDGE ;				// falling edge
	}
	else
	{
		EndTime = time ;		
		time -= StartTime ;
		time >>= 7 ;		// Nominal 125 to 250
		time -= 60 ;		// Nominal 65 to 190
		if ( time > 255 )
		{
			time = 255 ;			
		}
		PulseTime = time ;
		EICRA |= PPM_INT_EDGE ;				// Rising edge
	}
}

unsigned int ReadPPM()
{
	unsigned int ppm = PulseTime ;
	ppm += 60 ;
	ppm <<= 3 ;	// To microseconds
	if ((ppm>2500) || (ppm<500)) ppm=0 ; // no signal!
  
	return ppm ;
}

#endif


#endif //PIN_PPM

#ifdef DEBUG
//************************************************************   OutputToSerial
void OutputToSerial(){
//#define DEBUGMINMAX 0
//#define DEBUGVREF 0
//#define DEBUGDIVIDER 0
//#define DEBUGTEMP 0

#ifdef DEBUGMINMAX
  Serial.print("(");  
  Serial.print( ( (float)oxs_Arduino.arduinoData.minVrefMilliVolts)/1000);
  Serial.print("..");  
  Serial.print( ( (float)oxs_Arduino.arduinoData.maxVrefMilliVolts)/1000);
  Serial.print(")");
#endif //DEBUGMINMAX

#ifdef DEBUGDIVIDER
  Serial.print(" ;divV=");    
  Serial.print((float)oxs_Arduino.arduinoData.dividerMilliVolts /1000);
#ifdef DEBUGMINMAX
  Serial.print("(");  
  Serial.print( ( (float)oxs_Arduino.arduinoData.minDividerMilliVolts)/1000);
  Serial.print("..");  
  Serial.print( ( (float)oxs_Arduino.arduinoData.maxDividerMilliVolts)/1000);
  Serial.print(")");
#endif //DEBUGMINMAX
#endif //DEBUGDIVIDER

#ifdef VARIO
  Serial.print(" Sensitivity PPM=");    
  Serial.print( oxs_MS5611.varioData.sensitivityPpm);
  Serial.print(";relAlt=");  
  Serial.print( ( (float)oxs_MS5611.varioData.relativeAlt)/100);
#ifdef DEBUGMINMAX
  Serial.print("(");  
  Serial.print( ( (float)oxs_MS5611.varioData.minRelAlt)/100);
  Serial.print("..");  
  Serial.print( ( (float)oxs_MS5611.varioData.maxRelAlt)/100);
  Serial.print(")");
#endif //DEBUGMINMAX
  Serial.print(" ;absAlt=");  
  Serial.print( ( (float)oxs_MS5611.varioData.absoluteAlt)/100);
#ifdef DEBUGMINMAX
  Serial.print("(");  
  Serial.print( ( (float)oxs_MS5611.varioData.minAbsAlt)/100);
  Serial.print("..");  
  Serial.print( ( (float)oxs_MS5611.varioData.maxAbsAlt)/100);
  Serial.print(")");
#endif  //DEBUGMINMAX

  Serial.print(" ;Vspd=");    
  Serial.print( ( (float)oxs_MS5611.varioData.climbRate)/100);
#ifdef DEBUGMINMAX
  Serial.print("(");    
  Serial.print( ( (float)oxs_MS5611.varioData.minClimbRate)/100);
  Serial.print("..");    
  Serial.print( ( (float)oxs_MS5611.varioData.maxClimbRate)/100);
  Serial.print(")");
#endif //DEBUGMINMAX
#ifdef DEBUGTEMP
  Serial.print(" ;Temp=");    
  Serial.print((float)oxs_MS5611.varioData.temperature/10);
#endif //DEBUGTEMP
#endif // VARIO

#ifdef PIN_CurrentSensor
  Serial.print(" ;mA=");    
  Serial.print( ( ((float)(int32_t)(oxs_Current.currentData.milliAmps))) );
  Serial.print("(");    
#ifdef DEBUGMINMAX
  Serial.print(  ( ((float)(int32_t)(oxs_Current.currentData.minMilliAmps))) );
  Serial.print("..");    
  Serial.print( ( ((float)(int32_t)(oxs_Current.currentData.maxMilliAmps))) );
  Serial.print(")");
#endif //DEBUGMINMAX
  Serial.print(" ;mAh=");  
  Serial.print( oxs_Current.currentData.consumedMilliAmps);
#endif // PIN_CurrentSensor
  Serial.println();
}

/***********************************************/
/* freeRam => Cook coffee and vaccuum the flat */
/***********************************************/
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif // End DEBUG



