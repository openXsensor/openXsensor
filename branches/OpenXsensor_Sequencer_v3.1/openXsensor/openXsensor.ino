#include "oXs_config.h"
#include "oXs_voltage.h"
#include "oXs_ms5611.h"
#include "oXs_4525.h"
#include "oXs_curr.h"
#include "oXs_out_frsky.h"
#include "oXs_out_multiplex.h"
#include "oXs_general.h"
#include "Aserial.h"

#ifdef SAVE_TO_EEPROM
  #include <EEPROM.h>
  #include "EEPROMAnything.h"
#endif

#include "Aserial.h"

#ifdef PIN_PPM
 #if PIN_PPM == 2
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp
	#define PPM_INT_MASK			0x03
	#define PPM_INT_EDGE			0x01
	#define PPM_PIN_HEX			0x02
	#define PPM_INT_BIT			0x01
 #endif

 #if PIN_PPM == 3
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp
	#define PPM_INT_MASK			0x0C
	#define PPM_INT_EDGE			0x04
	#define PPM_PIN_HEX			0x04
	#define PPM_INT_BIT			0x02
 #endif
#endif

//*************** There is normally no reason changing the 2 next parameters
#define I2C_MS5611_Add           0x77 // 0x77 The I2C Address of the MS5611 breakout board 
                               // (normally 0x76 or 0x77 configured on the MS5611 module 
                               // via a solder pin or fixed)
                               
#define I2C_4525_Add        0x28 // 0x28 is the default I2C adress of a 4525DO sensor

#define PIN_LED            13  // The Signal LED (default=13=onboard LED)


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
static unsigned long extended2Micros ;

#ifdef DEBUG
//#define DEBUGCOMPENSATEDCLIMBRATE
//#define DEBUGOUTDATATOSERIAL
//#define DEBUGENTERLOOP
//#define DEBUGSEQUENCE
//#define DEBUGPPM
#endif

int freeRam () ;
void checkButton() ;
void readSensors() ;
void Reset1SecButtonPress() ;
void Reset3SecButtonPress() ; 
void Reset10SecButtonPress() ;
void SaveToEEProm() ;
void LoadFromEEProm() ; 
void ProcessPPMSignal() ;
//unsigned int ReadPPM() ;
void ReadPPM() ;
bool checkFreeTime() ;

#if defined (VARIO) && defined ( AIRSPEED)
bool compensatedClimbRateAvailable ;
bool switchCompensatedClimbRateAvailable ;
float rawCompensatedClimbRate ; 
int32_t compensatedClimbRate ;
#endif

#if defined (VARIO) && ( defined (VARIO2) || defined ( AIRSPEED))
int32_t switchVSpeed ;
bool switchVSpeedAvailable ;
#endif

#if defined (VARIO) && defined (VARIO2)
float averageVSpeedFloat ;
int32_t averageVSpeed ;
bool averageVSpeedAvailable ;
#endif

#ifdef PIN_PPM
uint16_t ppmus ; // duration of ppm in usec
int prevPpm ; //^previous ppm
int ppm ; // duration of pulse in range -100 / + 100 ; can exceed those limits
bool ppmAvailable  ;
//extern volatile uint8_t ppmInterrupted ;
//volatile uint8_t ppmInterruptedCopy ;
#endif

#ifdef SEQUENCE_OUTPUTS
#ifdef SEQUENCE_m100
  uint8_t sequence_m100 [] = { SEQUENCE_m100 } ;
#else
  uint8_t sequence_m100 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m75
  uint8_t sequence_m75 [] = { SEQUENCE_m75 } ;
#else
  uint8_t sequence_m75 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m50
  uint8_t sequence_m50 [] = { SEQUENCE_m50 } ;
#else
  uint8_t sequence_m50 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_m25
  uint8_t sequence_m25 [] = { SEQUENCE_m25 } ;
#else
  uint8_t sequence_m25 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_0
  uint8_t sequence_0 [] = { SEQUENCE_0 } ;
#else
  uint8_t sequence_0 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_25
  uint8_t sequence_25 [] = { SEQUENCE_25 } ;
#else
  uint8_t sequence_25 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_50
  uint8_t sequence_50 [] = { SEQUENCE_50 } ;
#else
  uint8_t sequence_50 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_75
  uint8_t sequence_75 [] = { SEQUENCE_75 } ;
#else
  uint8_t sequence_75 [2] = { 0 , 0 } ;
#endif
#ifdef SEQUENCE_100
  uint8_t sequence_100 [] = { SEQUENCE_100 } ;
#else
  uint8_t sequence_100 [2] = { 0 , 0 } ;
#endif

    uint16_t sequenceMaxNumber[9] ;  // this array contains the number of events of each sequence
    uint8_t * sequencePointer[9] ; // this is an array of pointers; each pointer refers to the first element of a 2 dim array.
    
    uint8_t *seqRef  ;  // seqRef contains a pointer to the first item of the selected sequence array
    uint16_t seqMax ; // number of sequences in the selected array
    uint8_t seqState ; // say if the sequence is starting (0), running (1) or stopped (2)
    uint16_t seqStep ;  // says the current step in the sequence , first step = 0 ;
    uint8_t sequenceOutputs = SEQUENCE_OUTPUTS & 0b00111111 ; // Mask to define which pin of portB are outputs
#ifdef SEQUENCE_UNIT
    uint16_t sequenceUnit = SEQUENCE_UNIT * 10 ; 
#else
    uint16_t sequenceUnit = 10 ; 
#endif // SEQUENCE_UNIT
#endif // end of SEQUENCE_OUTPUTS



volatile bool RpmSet  ;
volatile uint16_t RpmValue ;
unsigned long lastRpmMillis ;
bool RpmAvailable  ;

int PWRValue; // calculation field for Vertical speed on PWR
unsigned long lastMillisPWR ;

float actualPressure  ; // default pressure in pascal; to actualise if vario exist; is used in airspeed calculation.
int sensitivityPpmMapped ;
int compensationPpmMapped ;
int32_t test1Value ;// used in order to test the transmission of any value
bool test1ValueAvailable ;
int32_t test2Value ;// used in order to test the transmission of any value
bool test2ValueAvailable ;
int32_t test3Value ;// used in order to test the transmission of any value
bool test3ValueAvailable ;

uint8_t selectedVario ; // identify the vario to be used when switch vario with PPM is active (0 = first MS5611) 

// Create instances of the used classes
                        // Mike, there is no need creating this class if there is no voltage to measure because this version does not read Internal ref regarding VCC anymore (even for measuring current)
  // to do : adapt the test on defined (PIN_VOLTAGE_...) because actual config.h expects now that all PIN_VOLTAGE_ are defined ; a value > 7 means it is not used. 
#if defined( PIN_VOLTAGE_1 ) || defined( PIN_VOLTAGE_2 ) || defined( PIN_VOLTAGE_3 ) || defined( PIN_VOLTAGE_4 ) || defined( PIN_VOLTAGE_5 ) || defined( PIN_VOLTAGE_6 ) 
#ifdef DEBUG  
OXS_VOLTAGE oXs_Voltage(Serial);
#else
OXS_VOLTAGE oXs_Voltage(0);
#endif  //DEBUG
#endif

#ifdef VARIO
#ifdef DEBUG  
OXS_MS5611 oXs_MS5611(I2C_MS5611_Add,Serial);
#else
OXS_MS5611 oXs_MS5611(I2C_MS5611_Add);
#endif  //DEBUG
#endif


#ifdef VARIO2
#ifdef DEBUG  
OXS_MS5611 oXs_MS5611_2(I2C_MS5611_Add - 1,Serial);
#else
OXS_MS5611 oXs_MS5611_2(I2C_MS5611_Add - 1);
#endif  //DEBUG
#endif

#ifdef AIRSPEED // differential pressure
#ifdef DEBUG  
OXS_4525 oXs_4525(I2C_4525_Add ,Serial);
#else
OXS_4525 oXs_4525(I2C_4525_Add);
#endif  //DEBUG
#endif




#ifdef PIN_CURRENTSENSOR
#ifdef DEBUG  
OXS_CURRENT oXs_Current(PIN_CURRENTSENSOR,Serial);
#else
OXS_CURRENT oXs_Current(PIN_CURRENTSENSOR);
#endif //DEBUG
#endif

#ifdef DEBUG  
OXS_OUT_FRSKY oXs_OutFrsky(PIN_SERIALTX,Serial);
#else
OXS_OUT_FRSKY oXs_OutFrsky(PIN_SERIALTX);
#endif

                            // Mike I do not understand this instruction; could you explain
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))

//************************************************************************************************** Setup()
void setup(){

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println(F("openXsensor starting.."));
  Serial.print(F(" milli="));  
  Serial.println(millis());
  Serial.print(F("freeRam="));  
  Serial.println(freeRam());
#endif 

//  sensitivityPpmMapped = 0 ;
  compensationPpmMapped = 100 ;
//  test1Value = 0 ;
//  test2Value = 0 ;
//  test3Value = 0 ;
  test1ValueAvailable = false ;
  test2ValueAvailable = false ;
  test3ValueAvailable = false ;
  actualPressure = 101325 ; // default pressure in pascal; to actualise if vario exist; is used in airspeed calcualtion.

#ifdef PIN_PUSHBUTTON  
  pinMode(PIN_PUSHBUTTON, INPUT_PULLUP);
#endif
  pinMode(PIN_LED, OUTPUT); // The signal LED (used for the function push button)

  // Invoke all setup methods and set reference
  // to do : adapt the test on defined (PIN_VOLTAGE_...) because actual config.h expects now that all PIN_VOLTAGE_ are defined ; a value > 7 means it is not used. 
#if (defined PIN_VOLTAGE_1 && PIN_VOLTAGE_1 < 8) || (defined PIN_VOLTAGE_2 && PIN_VOLTAGE_2 < 8) ||(defined PIN_VOLTAGE_3 && PIN_VOLTAGE_3 < 8) ||(defined PIN_VOLTAGE_4 && PIN_VOLTAGE_4 < 8) ||(defined PIN_VOLTAGE_5 && PIN_VOLTAGE_5 < 8) ||(defined PIN_VOLTAGE_6 && PIN_VOLTAGE_6 < 8)  
  oXs_Voltage.setupDivider(); 
  oXs_OutFrsky.voltageData=&oXs_Voltage.voltageData; 
#endif


#ifdef VARIO
  oXs_MS5611.setup();
  oXs_OutFrsky.varioData=&oXs_MS5611.varioData; 
#ifdef PIN_ANALOG_VSPEED
  lastMillisPWR = 3500 ; // So we will wait for 3.5 sec before generating a Vertical speed on PWM
  analogWrite(PIN_ANALOG_VSPEED,255/5*1.6); // initialize the output pin 
#endif
#endif // vario

#ifdef VARIO2
  oXs_MS5611_2.setup();
  oXs_OutFrsky.varioData_2=&oXs_MS5611_2.varioData; 
#endif // vario


#ifdef AIRSPEED
  oXs_4525.setup();
  oXs_OutFrsky.airSpeedData=&oXs_4525.airSpeedData; 
#endif // end AIRSPEED

#ifdef PIN_CURRENTSENSOR
  oXs_Current.setupCurrent( );
  oXs_OutFrsky.currentData=&oXs_Current.currentData;
#endif

#if defined (VARIO) && defined ( AIRSPEED)
  compensatedClimbRateAvailable = false;
  compensatedClimbRate = 0;
#endif

  oXs_OutFrsky.setup();

#ifdef SAVE_TO_EEPROM
  LoadFromEEProm();
#endif

#ifdef PIN_PPM
//    ppmus = 0 ;
//    prevPpm = 0 ;
    ppmAvailable = false ;
#endif

#ifdef PPM_INTERRUPT
	PORTD |= PPM_PIN_HEX ;	// Pullup resistor
	DDRD &= ~PPM_PIN_HEX ;	// Input
	EICRA |= PPM_INT_MASK ;		// Interrupt on rising edge
	EIFR = PPM_INT_BIT ;			// Clear interrupt flag
	EIMSK |= PPM_INT_BIT ;		// Enable interrupt
#endif

  RpmSet = false ;
//  RpmValue = 0 ;
  RpmAvailable = false ;

#ifdef SEQUENCE_OUTPUTS
    sequenceMaxNumber[0] = sizeof(sequence_m100) ; // sizeof(sequence_m100[0]) ;
    sequenceMaxNumber[1] = sizeof(sequence_m75) ;// sizeof(sequence_m75[0]) ;
    sequenceMaxNumber[2] = sizeof(sequence_m50) ;// sizeof(sequence_m50[0]) ;
    sequenceMaxNumber[3] = sizeof(sequence_m25) ;// sizeof(sequence_m25[0]) ;
    sequenceMaxNumber[4] = sizeof(sequence_0) ;// sizeof(sequence_0[0]) ;
    sequenceMaxNumber[5] = sizeof(sequence_25) ;// sizeof(sequence_25[0]) ;
    sequenceMaxNumber[6] = sizeof(sequence_50) ;// sizeof(sequence_50[0]) ;
    sequenceMaxNumber[7] = sizeof(sequence_75) ;// sizeof(sequence_75[0]) ;
    sequenceMaxNumber[8] = sizeof(sequence_100) ;// sizeof(sequence_100[0]) ;
    sequencePointer[0] = &sequence_m100[0] ;
    sequencePointer[1] = &sequence_m75[0] ;
    sequencePointer[2] = &sequence_m50[0] ;
    sequencePointer[3] = &sequence_m25[0] ;
    sequencePointer[4] = &sequence_0[0] ;
    sequencePointer[5] = &sequence_25[0] ;
    sequencePointer[6] = &sequence_50[0] ;
    sequencePointer[7] = &sequence_75[0] ;
    sequencePointer[8] = &sequence_100[0] ;

     
#ifdef DEBUG
    seqRef = sequencePointer[0] ;  // seqTab contains pointers to the sequence array
    seqMax = sequenceMaxNumber[0] ;
    Serial.print(F("SeqRef="));  Serial.println( (uint16_t) seqRef) ;
    Serial.print(F("SeqMax="));  Serial.println( (uint16_t) seqMax) ;
    Serial.print(F("Sequence_m100="));    
    for (uint8_t idxDebugSeq = 0 ; idxDebugSeq < (seqMax) ; idxDebugSeq++ ) {
      Serial.print( *(seqRef + idxDebugSeq ) );
      Serial.print(F(" , "));
    }
    Serial.println(F(" "));
#endif    
 
 
    seqState = 2 ; // declare that sequence is stopped
    PORTB &= ~sequenceOutputs ; // set all output to LOW
    DDRB |= sequenceOutputs ; // set pin to output mode
    
#ifdef DEBUGSEQUENCE
    ppm = -100 ; // fix the sequence to be used for test when no PPM signal is present; value is between -100 and 100 
    setNewSequence( ) ; 
#endif

#endif // end of SEQUENCE_OUTPUTS

#ifdef DEBUG
  Serial.println(F("End of general set up"));
#endif
} // ******************** end of Setup


//*******************************************************************************************
//                                Main loop                                               ***
//*******************************************************************************************
void loop(){  
#ifdef DEBUGENTERLOOP
  Serial.print(F("in loop="));  
  Serial.println(millis());
#endif 

 
#ifdef PIN_PUSHBUTTON
  // Check if a button has been pressed
#if defined (VARIO) || defined (VARIO2)
    if (checkFreeTime()) checkButton(); // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
//    extended2Micros = micros() >> 1 ;
//    if (extended2Micros < oXs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
//    if ( extended2Micros < (oXs_MS5611.varioData.lastCommand2Micros + 3500)  ) checkButton(); // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#else
    checkButton();
#endif    
#endif 
    
    readSensors(); // read all sensors)
    oXs_OutFrsky.sendData(); // choice which data can be send base on availability and some priority logic 
    
#if defined ( DEBUG ) && defined (DEBUGOUTDATATOSERIAL)
    OutputToSerial() ; 
#endif
    
// PPM Processing = Read the ppm Signal from receiver and process it 
#ifdef PIN_PPM 
    if (checkFreeTime()) ProcessPPMSignal(); 
#endif //PIN_PPM

#ifdef VARIO
#ifdef PIN_ANALOG_VSPEED
    if (millis() > lastMillisPWR + 100) {
        if (checkFreeTime()) { // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
            PWRValue=map( (long) oXs_MS5611.varioData.climbRate,ANALOG_VSPEED_MIN*100,ANALOG_VSPEED_MAX * 100,0,255/5*3.2);
            PWRValue=constrain(PWRValue, 0, 255/5*3.2 ) ;
            analogWrite(PIN_ANALOG_VSPEED,(int)PWRValue);
            lastMillisPWR = millis() ; 
        }
    }  
#endif
#endif //VARIO 

#ifdef SEQUENCE_OUTPUTS 
  checkSequence() ; // check if outputs have to be modified because time expired
#endif

#ifdef SAVE_TO_EEPROM
  static unsigned long LastEEPromMs=millis();
  if (millis()>LastEEPromMs+10000){ // Save Persistant Data To EEProm every 10 seconds
    LastEEPromMs=millis();
    SaveToEEProm();
  }
#endif
}          // ****************   end of main loop *************************************

//**********************************************************************************************************
//***                                            Read all the sensors / Inputs                          ****
//**********************************************************************************************************
void readSensors() {   
//#ifdef DEBUG
//  Serial.println(F("in readSensors"));  
//#endif 
#ifdef AIRSPEED
  oXs_4525.readSensor(); // Read a the differential pressure on 4525DO, calculate airspeed
#endif


#ifdef VARIO
  oXs_MS5611.readSensor(); // Read pressure & temperature on MS5611, calculate Altitude and vertical speed
  if ( oXs_MS5611.varioData.absoluteAltAvailable == true and oXs_MS5611.varioData.rawPressure > 100000.0f ) actualPressure = oXs_MS5611.varioData.rawPressure / 10000.0 ; // this value can be used when calculating the Airspeed
#endif

#ifdef VARIO2
  oXs_MS5611_2.readSensor(); // Read pressure & temperature on MS5611, calculate Altitude and vertical speed
#endif


#ifdef AIRSPEED
  oXs_4525.readSensor(); // Read again the sensor in order to reduce response time/noise
#if defined (VARIO) && defined ( AIRSPEED) 
  if ( oXs_MS5611.varioData.altitudeAt20MsecAvailable == true ) {  // compensation is calculated only when a new altitude is available
    calculateDte() ; 
  }  
#endif  
#endif // end #ifdef AIRSPEED

#if defined (VARIO) &&  defined (VARIO2)
  if( (oXs_MS5611.varioData.averageClimbRateAvailable == true) || ( oXs_MS5611_2.varioData.averageClimbRateAvailable == true) ) {
    averageVSpeedFloat = ( oXs_MS5611.varioData.climbRateFloat + oXs_MS5611_2.varioData.climbRateFloat ) / 2 ;
    if ( abs((int32_t)  averageVSpeedFloat - averageVSpeed) > VARIOHYSTERESIS ) {
          averageVSpeed = (int32_t)  averageVSpeedFloat ;
    }    
    averageVSpeedAvailable = true ;
    oXs_MS5611.varioData.averageClimbRateAvailable = false ;
    oXs_MS5611_2.varioData.averageClimbRateAvailable = false ;
  }  
#endif  

#if defined (VARIO) && ( defined (VARIO2) || defined (AIRSPEED) ) && defined (VARIO_SECONDARY ) && defined( VARIO_PRIMARY )  && defined (PIN_PPM)
  if (( selectedVario == 0) && ( oXs_MS5611.varioData.switchClimbRateAvailable == true ) )  {
      switchVSpeed = oXs_MS5611.varioData.climbRate ;
      switchVSpeedAvailable = true ;
      oXs_MS5611.varioData.switchClimbRateAvailable = false ;
  } 
#if  defined (VARIO2)
  else if ( ( selectedVario == 1) && ( oXs_MS5611_2.varioData.switchClimbRateAvailable == true )) {
      switchVSpeed = oXs_MS5611_2.varioData.climbRate ;
      switchVSpeedAvailable = true ;
      oXs_MS5611_2.varioData.switchClimbRateAvailable = false ;
  }
  else if ( ( selectedVario == 3) && ( averageVSpeedAvailable == true )) {
      switchVSpeed = averageVSpeed ;
      switchVSpeedAvailable = true ;
      averageVSpeedAvailable = false ;
  }
#endif // end of VARIO2
#if  defined (AIRSPEED)
  else if ( ( selectedVario == 2) && ( switchCompensatedClimbRateAvailable == true )) {
      switchVSpeed = compensatedClimbRate ;
      switchVSpeedAvailable = true ;  
#if defined (SWITCH_VARIO_GET_PRIO)
      switchCompensatedClimbRateAvailable = true ; // avoid to reset the value on false in order to continue to send the same value as often as possible
#else
      switchCompensatedClimbRateAvailable = false ; // this is the normal process in order to avoid sending twice the same data.
#endif  // end  defined (SWITCH_VARIO_GET_PRIO)   
  } 
#endif // end  defined (AIRSPEED) 
#endif // end  defined (VARIO) && ( defined (VARIO2) || defined (AIRSPEED) ) && defined (VARIO_SECONDARY ) && defined( VARIO_PRIMARY ) && defined (VARIO_SECONDARY) && defined (PIN_PPM)


#if (defined PIN_VOLTAGE_1 && PIN_VOLTAGE_1 < 8) || (defined PIN_VOLTAGE_2 && PIN_VOLTAGE_2 < 8) ||(defined PIN_VOLTAGE_3 && PIN_VOLTAGE_3 < 8) ||(defined PIN_VOLTAGE_4 && PIN_VOLTAGE_4 < 8) ||(defined PIN_VOLTAGE_5 && PIN_VOLTAGE_5 < 8) ||(defined PIN_VOLTAGE_6 && PIN_VOLTAGE_6 < 8)  
    if (checkFreeTime()) oXs_Voltage.readSensor();    // read voltage only if there enough time to avoid delaying vario reading
#endif   // end voltage

#ifdef PIN_CURRENTSENSOR
    if (checkFreeTime()) oXs_Current.readSensor() ; // Do not perform calculation if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
#endif             // End PIN_CURRENTSENSOR

#ifdef MEASURE_RPM
  if (millis() > ( lastRpmMillis + 200) ){  // allow transmission of RPM only once every 200 msec
        if (RpmSet == true) {
            RpmSet = false ;
        } else {
            RpmValue = 0 ;
        }
        RpmAvailable = true ;    
        lastRpmMillis = millis() ;
  }      
#endif
    
  //Serial.println(F("Go out of read sensor"));
}                  // ************** end of readSensors ********************************************


//***************** checkFreeTime ********* if there is at least 2000 usec before the next MS5611 read (in order to avoid delaying it 
bool checkFreeTime() { // return true if there is no vario or if the vario sensor must not be read within a short time.
                       // return false if a vario must be read within a short time   
#if defined (VARIO) || defined (VARIO2)
        extended2Micros = micros() >> 1 ;
#ifdef VARIO
        if (extended2Micros < oXs_MS5611.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
        if ( extended2Micros < (oXs_MS5611.varioData.lastCommand2Micros + 3500)  ) { // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
            return true ;
        } else {
            return false  ;
        }    
#else // only VARIO2
        if (extended2Micros < oXs_MS5611_2.varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
        if ( extended2Micros < (oXs_MS5611_2.varioData.lastCommand2Micros + 3500)  ) { // Do not change PWM if there is less than 2000 usec before MS5611 ADC is available =  (9000 - 2000)/2
            return true ; 
        } else {
            return false ;
        }    
#endif // enf of at least one vario
#else // No vario at all
  return true ;
#endif  
}  // ******************************* end of checkFreeTime *****************************

// ********************************** Calculate dTE based on rawAltitude and differential pressure
#if defined (VARIO) && defined ( AIRSPEED) 
#define SMOOTHING_DTE_MIN SENSITIVITY_MIN
#define SMOOTHING_DTE_MAX SENSITIVITY_MAX
#define SMOOTHING_DTE_MIN_AT SENSITIVITY_MIN_AT
#define SMOOTHING_DTE_MAX_AT SENSITIVITY_MAX_AT

void calculateDte () {  // is calculated about every 2O ms each time that an altitude is available
  static float rawTotalEnergy ;
  static float abs_deltaCompensatedClimbRate ;
  static float smoothingDteMin =  SMOOTHING_DTE_MIN ;
  static float expoSmoothDte_auto ;
  static float smoothCompensatedClimbRate ;
  static float rawCompensation ;
  static float totalEnergyLowPass ;
  static float totalEnergyHighPass ;
  
               //  difPressure (in PSI) = difPressureAdc * 0.0001525972 because 1 PSI = (8192 - 1638) = 6554 steps
               //  difPressure (Pa) =  difPressure (in PSI) * 6894.757f  = difPressureAdc * 6894.757 *  0.0001525972 = difPressureAdc * 1.0520
               // airspeed = sqr(2 * differential_pressure / air_density) ; air density = pressure  pa / (287.05 * (Temp celcius + 273.15))
               // so airspeed m/sec =sqr( 2 * 287.05 * differential_pressure pa * (temperature Celsius + 273.15) / pressure pa
               // total energy = (m * g * altitude) + (m * airspeed * airspeed / 2) => m * 9.81 * (altitude + airspeed * airspeed / 2 /9.81)
               // compensation (m/sec) = airspeed * airspeed / 2 / 9.81 =
               //                      = 2 * 287.05 * difPressureAdc * 1.0520  * (temperature Celsius + 273.15) / pressure pa /2 /9.81 (m/sec) = 30.78252803 * difPressureAdc * Temp(kelv) / Press (Pa)
               // compensation (cm/sec) =  3078.252803 * difPressureAdc * Temp(kelv) / Press (Pa)
 
  rawCompensation = 3078.25 * oXs_4525.airSpeedData.difPressureAdc_zero * oXs_4525.airSpeedData.temperature4525  /  actualPressure    ; // 3078.25 = comp = 2 * 287.05 / 2 / 9.81 * 1.0520 * 100 * Temp / Pressure  
  rawTotalEnergy = (oXs_MS5611.varioData.rawAltitude * 0.01) + rawCompensation * compensationPpmMapped * 0.0115; // 0.01 means 100% compensation but we add 15% because it seems that it is 15% undercompensated. 
  if (totalEnergyLowPass == 0) { 
    totalEnergyLowPass = totalEnergyHighPass = rawTotalEnergy ; 
  }
  //test1Value = rawCompensation; 
//  test1Value = 1000; 
//  test1ValueAvailable = true ; 
  
  //test2Value = rawTotalEnergy; 
  test2Value = 2000; 
  test2ValueAvailable = true ; 
  
  totalEnergyLowPass += 0.085 * ( rawTotalEnergy - totalEnergyLowPass) ;
  totalEnergyHighPass += 0.1 * ( rawTotalEnergy - totalEnergyHighPass) ;
  rawCompensatedClimbRate = ((totalEnergyHighPass - totalEnergyLowPass )  * 566667.0 ) /     oXs_MS5611.varioData.delaySmooth; // 0.566667 is the parameter to be used for 0.085 and 0.1 filtering if delay is in sec

//  test3Value = rawCompensatedClimbRate ; 
//  test3ValueAvailable = true ; 
  abs_deltaCompensatedClimbRate =  abs(rawCompensatedClimbRate - smoothCompensatedClimbRate) ;
  if ( sensitivityPpmMapped  > 0) smoothingDteMin =   sensitivityPpmMapped  ; // a value of sensitivityPpmMapped = 50 becomes a smoothing factor 0.1
    if ( (abs_deltaCompensatedClimbRate <= SMOOTHING_DTE_MIN_AT) || (smoothingDteMin >= SMOOTHING_DTE_MAX ) ){
     expoSmoothDte_auto = smoothingDteMin ;  
  } else if (abs_deltaCompensatedClimbRate >= SMOOTHING_DTE_MAX_AT)  {
     expoSmoothDte_auto = SMOOTHING_DTE_MAX ; 
  } else {
     expoSmoothDte_auto = smoothingDteMin + ( SMOOTHING_DTE_MAX - smoothingDteMin ) * (abs_deltaCompensatedClimbRate - SMOOTHING_DTE_MIN_AT) / (SMOOTHING_DTE_MAX_AT - SMOOTHING_DTE_MIN_AT) ;
  }
  smoothCompensatedClimbRate += expoSmoothDte_auto * (  rawCompensatedClimbRate -  smoothCompensatedClimbRate ) * 0.001; 
  if ( abs( ((int32_t)  smoothCompensatedClimbRate) - compensatedClimbRate) > VARIOHYSTERESIS ) {
      compensatedClimbRate = smoothCompensatedClimbRate  ;
  } 
  compensatedClimbRateAvailable = true ; // allows SPORT protocol to transmit the value every 20 ms
  switchCompensatedClimbRateAvailable = true ; ; // inform readsensors() that a switchable vspeed is available
  oXs_MS5611.varioData.altitudeAt20MsecAvailable = false ; // avoid to handel it each time.
 
#ifdef DEBUGCOMPENSATEDCLIMBRATE
  static bool firstCompensatedClimbRate = true ;
  if ( firstCompensatedClimbRate ) {
        Serial.println(F("at, altitude, difPressureADC_zero, rawCompensation , rawTotalEnergy , delaySmooth , rawCompensatedClimbRate , compensatedClimbRate ")) ;
        firstCompensatedClimbRate = false ;
  } else {
  Serial.print( micros() ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_MS5611.varioData.rawAltitude ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_4525.airSpeedData.difPressureAdc_zero ) ;Serial.print(F(" , ")) ;
  Serial.print( rawCompensation ) ;Serial.print(F(" , ")) ;
  Serial.print( rawTotalEnergy ) ;Serial.print(F(" , ")) ;
  Serial.print( oXs_MS5611.varioData.delaySmooth ) ;Serial.print(F(" , ")) ;
  Serial.print( rawCompensatedClimbRate ) ;Serial.print(F(" , ")) ;
  Serial.print( compensatedClimbRate) ;
  Serial.println(F(" ")) ; 
  } 
#endif    
   
} // end calculateDte  
#endif    // #if defined (VARIO) && defined ( AIRSPEED) 
// ***************************** end calculate Dte ***********************************************




#ifdef PIN_PUSHBUTTON   // // ****************** check button
void checkButton() {
  static int lastSensorVal=HIGH;
  static unsigned int buttonDownMs;
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(PIN_PUSHBUTTON);
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
    //Serial.print(F("Button Released after ms:"));
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
  Serial.println(F(" Reset 0.1-3 sec"));
#endif

#ifdef PIN_CURRENTSENSOR
  struct CURRENTDATA *cd = &oXs_Current.currentData ;
//  FORCE_INDIRECT(cd) ;
//  cd->maxMilliAmps=cd->milliAmps;
//  cd->minMilliAmps=cd->milliAmps;
#endif

//#ifdef VARIO
//  oXs_MS5611.resetValues() ;
//#endif

//#ifdef VARIO2
//  oXs_MS5611_2.resetValues() ;
//#endif

//#ifdef PIN_VOLTAGE_DIVIDER
//  oXs_Voltage.resetValues();
//#endif
}

void Reset3SecButtonPress()
{
#ifdef DEBUG
  Serial.println(F(" Reset 3-5 sec"));
#endif
#ifdef PIN_CURRENTSENSOR
  oXs_Current.currentData.consumedMilliAmps = 0;
  oXs_Current.currentData.floatConsumedMilliAmps = 0 ;
#endif
}

void Reset10SecButtonPress()
{
#ifdef DEBUG
  Serial.println(F("Reset 5-10")) ;
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
#define ADR_MAX 0   
#ifdef DEBUG
  Serial.print(F("SAving to EEProm:"));    
  Serial.println(state);    
#endif
#ifdef PIN_CURRENTSENSOR
  switch (state){
  case 0:
    adr+=EEPROM_writeAnything(adr, oXs_Current.currentData.consumedMilliAmps);
//  case 1:
//    adr+=EEPROM_writeAnything(adr, oXs_Current.currentData.maxMilliAmps);
//  case 2:
//    adr+=EEPROM_writeAnything(adr, oXs_Current.currentData.minMilliAmps);
//#endif // PIN_CURRENTSENSOR

/*
#ifdef VARIO
  case 3:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.maxRelAlt);
  case 4:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.minRelAlt);
  case 5:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.maxAbsAlt);
  case 6:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.minAbsAlt);
  case 7:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.maxClimbRate);
  case 8:
    adr+=EEPROM_writeAnything(adr, oXs_MS5611.varioData.minClimbRate);
#endif //VARIO
*/
  }
#endif // PIN_CURRENTSENSOR
  state++;
  if(state > ADR_MAX){
    state=0;
    adr=0;
  }
}

/******************************************/
/* LoadFromEEProm => load persistant Data */
/******************************************/
void LoadFromEEProm(){
  int adr=0;

  // Store the last known value to the eeprom
  Serial.println(F("Restored from EEProm:"));
#ifdef PIN_CURRENTSENSOR
  adr+=EEPROM_readAnything(adr, oXs_Current.currentData.consumedMilliAmps);
  oXs_Current.currentData.floatConsumedMilliAmps = oXs_Current.currentData.consumedMilliAmps ;
  Serial.print(F(" mAh="));    
  Serial.print( oXs_Current.currentData.consumedMilliAmps);

//  adr+=EEPROM_readAnything(adr, oXs_Current.currentData.maxMilliAmps);
//  Serial.print(F(" maxMilliAmps="));    
//  Serial.print( oXs_Current.currentData.maxMilliAmps);

//  adr+=EEPROM_readAnything(adr, oXs_Current.currentData.minMilliAmps);
//  Serial.print(F(" minMilliAmps="));    
//  Serial.print( oXs_Current.currentData.minMilliAmps);
#endif
/*
#ifdef VARIO
  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.maxRelAlt);
  Serial.print(F(" maxRelAlt="));    
  Serial.print( oXs_MS5611.varioData.maxRelAlt);

  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.minRelAlt);
  Serial.print(F(" minRelAlt="));    
  Serial.print( oXs_MS5611.varioData.minRelAlt);


  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.maxAbsAlt);
  Serial.print(F(" maxAbsAlt="));    
  Serial.print( oXs_MS5611.varioData.maxAbsAlt);

  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.minAbsAlt);
  Serial.print(F(" minAbsAlt="));    
  Serial.print( oXs_MS5611.varioData.minAbsAlt);

  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.maxClimbRate);
  Serial.print(F(" minClimbRate="));    
  Serial.print( oXs_MS5611.varioData.minClimbRate);

  adr+=EEPROM_readAnything(adr, oXs_MS5611.varioData.minClimbRate);
  Serial.print(F(" maxClimbRate="));    
  Serial.print( oXs_MS5611.varioData.maxClimbRate);
#endif //VARIO
*/
}
#endif //saveToEeprom


/**********************************************************/
/* ProcessPPMSignal => read PPM signal from receiver and  */
/*   use its value to adjust sensitivity                  */
/**********************************************************/
#ifdef PIN_PPM
volatile uint16_t time1 ;
//uint16_t time3 ;
void ProcessPPMSignal(){
//#ifdef DEBUGPPM
//    if ( ppmInterruptedCopy > 0)  {
//        Serial.print(F("I= ")); Serial.println(ppmInterruptedCopy) ;
//    }  
//#endif
  ReadPPM(); // set ppmus to 0 if ppm is not available or has not been colletect X time, other fill ppmus with the (max) pulse duration in usec 
  if (ppmus>0){  // if a value has been defined for ppm (in microseconds)
    ppm = map( ppmus , PPM_MIN_100, PPM_PLUS_100, -100 , 100 ) ; // map ppm in order to calibrate between -100 and 100
//    ppm = 82 ; // test only
    ppmAvailable = true ;
#ifdef DEBUGPPM
    static uint16_t ppmCount ;
    if ( (((int) ppm) - ((int ) prevPpm) > 3 ) || (((int) prevPpm) - ((int ) ppm) > 3 )  )  {
        Serial.print(F(" us="));  Serial.print(ppmus); Serial.print(F(" c="));  Serial.println(ppmCount);
        ppmCount = 0 ;
    }  else {
      ppmCount++;
    }  
#endif

#ifdef SEQUENCE_OUTPUTS
    if ( ( ( ppm - prevPpm) > 3 ) || (( prevPpm - ppm) > 3 )  )  { // handel ppm only if it change by more than 3 
        setNewSequence( ) ;
    }  
#else // so if Sequence is not used and so PPM is used for Vario sensitivity 
    if (ppm == prevPpm) {  // test if new value is equal to previous in order to avoid unstabel handling 
    
#if defined ( VARIO_PRIMARY) && defined ( VARIO_SECONDARY)  && defined (VARIO ) && ( defined (VARIO2) || defined (AIRSPEED) )  && defined (PIN_PPM)
        if ( (ppm >= (SWITCH_VARIO_MIN_AT_PPM - 4)) && (ppm <= (SWITCH_VARIO_MAX_AT_PPM + 4)) ) {
          selectedVario = VARIO_PRIMARY ;
        } else if ( ( ppm <= (4 - SWITCH_VARIO_MIN_AT_PPM)) && (ppm >= (- 4 - SWITCH_VARIO_MAX_AT_PPM)) ) {
            selectedVario = VARIO_SECONDARY ; 
        }  
#endif

#if defined (VARIO) || defined (VARIO2) 
        if ( (abs(ppm) >= (SENSITIVITY_MIN_AT_PPM - 4)) && ( abs(ppm) <= (SENSITIVITY_MAX_AT_PPM + 4)) ) {
            sensitivityPpmMapped =  map( constrain(abs(ppm), SENSITIVITY_MIN_AT_PPM , SENSITIVITY_MAX_AT_PPM ), SENSITIVITY_MIN_AT_PPM , SENSITIVITY_MAX_AT_PPM , SENSITIVITY_PPM_MIN , SENSITIVITY_PPM_MAX); // map value and change stepping to 10
#if defined( VARIO) 
            oXs_MS5611.varioData.sensitivityPpm = sensitivityPpmMapped ; // adjust sensitivity when abs PPM is within range
#endif
#if defined( VARIO2) 
            oXs_MS5611_2.varioData.sensitivityPpm = sensitivityPpmMapped ;
#endif
        }
#endif  // end else defined (VARIO) || defined (VARIO2) 
        

#if defined (AIRSPEED)  // adjust compensation
        if ( (abs(ppm) >= (COMPENSATION_MIN_AT_PPM - 4)) && ( abs(ppm) <= (COMPENSATION_MAX_AT_PPM + 4)) ) {
            compensationPpmMapped =  map( constrain(abs(ppm), COMPENSATION_MIN_AT_PPM , COMPENSATION_MAX_AT_PPM ), COMPENSATION_MIN_AT_PPM , COMPENSATION_MAX_AT_PPM , COMPENSATION_PPM_MIN , COMPENSATION_PPM_MAX); // map value and change stepping to 10
            if (compensationPpmMapped == COMPENSATION_PPM_MIN ) compensationPpmMapped = 0 ; // force compensation to 0 when compensation = min
        }
        if ( ( ppm >= (AIRSPEED_RESET_AT_PPM - 4)) && ( ppm <= (AIRSPEED_RESET_AT_PPM + 4)) ) {
            oXs_4525.airSpeedData.airspeedReset = true ; // allow a recalculation of offset 4525
        }    
#endif
    } // end ppm == prePpm
#endif  // end of #ifdef SEQUENCE_OUTPUTS & #else
  }  // end ppm > 0
  prevPpm = ppm ;
}  // end processPPMSignal


/**********************************************************/
/* ReadPPM => read PPM signal from receiver               */
/*   pre-evaluate its value for validity                  */
/**********************************************************/

#ifdef PPM_INTERRUPT
uint16_t StartTime ;
//uint16_t EndTime ;
 

 
volatile uint8_t PulseTime ;		// A byte to avoid 
volatile boolean  PulseTimeAvailable = false ;

#if PIN_PPM == 2 // When pin 2 is used, arduino handle INT0 (see datasheet)
ISR(INT0_vect, ISR_NOBLOCK)  // NOBLOCK allows other interrupts to be served when this one is activated 
#else            //// When pin 3 is used, arduino handle INT1
ISR(INT1_vect, ISR_NOBLOCK)
#endif
{
	cli() ;
	uint16_t time = TCNT1 ;	// Read timer 1
	sei() ;
	if ( EICRA & PPM_INT_EDGE ) // a rising edge occurs
	{
		StartTime = time ;
		EICRA &= ~PPM_INT_EDGE ;				// allow a falling edge to generate next interrupt
	}
	else
	{
//		EndTime = time ;		
		time -= StartTime ;
#if F_CPU == 20000000L   // 20MHz clock 
   #error Unsupported clock speed
#elif F_CPU == 16000000L  // 16MHz clock                                                  
		time >>= 4 ;		// delay in usec
#elif F_CPU == 8000000L   // 8MHz clock
		time >>= 3 ;		// delay in usec
#else
    #error Unsupported clock speed
#endif
		
//                if (  ppmInterrupted == 0  ) { // do not handle PulseTime if pin change interrupt has been delayed by another interrupt (Timer 1 compare A handling)
		  time1 = time ; 
                  PulseTimeAvailable = true ;
//		} else {
//                ppmInterruptedCopy++ ;  // used in order to test the value outside ISR
//                }
//                ppmInterrupted = 0 ;                            // reset indicator about pin change interrupt delay  
		EICRA |= PPM_INT_EDGE ;				//  allow a Rising edge to generate next interrupt
	}
}

void ReadPPM() {
         static uint8_t ppmIdx ;
         static uint16_t ppmTemp ;
         static uint16_t ppmMax ; // highest value of ppmTemp received ; Some ppm measurement are wrong (to low) because there are some interrupt some 
#define PPM_COUNT_MAX 10 // select the max of 10 ppm
        cli() ;
        if ( !PulseTimeAvailable ) { // if no new pulse is available just exit with ppmus = 0
              ppmus = 0 ;
              sei() ;  
        } else { 
            ppmTemp = time1 ; 
            PulseTimeAvailable = false ;
            sei() ;
            if ( ppmIdx >= PPM_COUNT_MAX ) {
                ppmus = ppmMax ; // we keep the highest value
                ppmIdx = 0 ;
                ppmMax = ppmTemp ; 
            } else {
                ppmIdx++ ;
                if( ppmTemp > ppmMax ) ppmMax = ppmTemp ;  
            }  // end test on ppmIdx   
        } // end test on PulseTimeAvailable
} //end ReadPPM()



#ifdef SEQUENCE_OUTPUTS
void setNewSequence( ) {                   // **********  setNewSequence( ) **************  handle a new ppm value
    static int8_t prevPpmMain = -100 ;
    int16_t ppmAbs = ppm + 114 ;
    int8_t ppmMain = ppmAbs / 25 ;
    int8_t ppmRest = ppmAbs % 25 ;
    if ( (ppmRest > 4) && (ppmMain >= 0) && (ppmMain <= 8) && (ppmMain != prevPpmMain) ) { // process only if valid and different from previous
#ifdef DEBUGSEQUENCE
    Serial.println(F(" "));
    Serial.print(F("ppmMain="));  Serial.println(ppmMain );  
    Serial.println(F(" "));
#endif
       prevPpmMain = ppmMain ;
       seqRef = sequencePointer[ppmMain] ;  // seqTab contains pointers to the sequence array
       seqMax = sequenceMaxNumber[ppmMain] ; 
       seqState = 0 ;
       seqStep = 0 ;
#ifdef DEBUGSEQUENCE
       test1Value = ppmMain; 
       test1ValueAvailable = true ; 
#endif
       checkSequence() ;
    }    
}

void checkSequence() {
  uint8_t portbTemp ;
  uint8_t portbTempValue ;
  static uint16_t seqDelay ;
  static unsigned long  currentMillis ;
  static unsigned long  nextSequenceMillis ;
#ifdef DEBUGSEQUENCE
//    Serial.print(F("seqState="));  Serial.println(seqState );  
#endif

  if ( seqState == 0 ) {
    portbTempValue = *(seqRef + 1); 
    seqDelay = *(seqRef ) * sequenceUnit; //
    portbTemp =  (~sequenceOutputs) & PORTB; // reset the bit that are controlled
    PORTB = portbTemp | ( portbTempValue & sequenceOutputs ); // set the bit as defined in the sequence if they have to be controlled
#ifdef DEBUGSEQUENCE
    Serial.print(F("seqStep="));  Serial.println( seqStep );
    Serial.print(F("portbTempValue="));  Serial.println( portbTempValue );
    Serial.print(F("seqDelay="));  Serial.println( seqDelay );
    Serial.print(F("portbTemp="));  Serial.println( portbTemp );
    Serial.print(F("PORTB="));  Serial.println(  portbTempValue & sequenceOutputs );  
#endif
    
    if (seqDelay == 0 ) {
      seqState = 2 ; // set state to sequence stopped if delay is 0
    } else {   
      seqState = 1 ; //says that sequence is running
      nextSequenceMillis = millis() +  seqDelay ;
    }  
  } else if ( seqState == 1 ) { // when sequence is running, check the timestamp to activate next sequence step
    currentMillis = millis() ;
    if ( currentMillis >  nextSequenceMillis  ) {
       seqStep += 2 ; // increase by 2 because there are 2 parameters per step
      if ( (seqStep + 1)  >=  seqMax  ) seqStep = 0 ; // return to 0 if end is reached
      portbTempValue = *(seqRef + 1 + seqStep ); 
      seqDelay = *(seqRef + seqStep) * sequenceUnit ;
      portbTemp =  (~sequenceOutputs) & PORTB; // reset the bit that are controlled
      PORTB = portbTemp | ( portbTempValue & sequenceOutputs ); // set the bit as defined in the sequence if they have to be controlled
      if (seqDelay == 0 ) {
          seqState = 2 ; // set state to sequence stopped if delay is 0
      } else {   
          seqState = 1 ; //says that sequence is running
          nextSequenceMillis = currentMillis  +  seqDelay ;
      }
#ifdef DEBUGSEQUENCE
      Serial.print(F("seqStep="));  Serial.println( seqStep );
      Serial.print(F("portbTempValue="));  Serial.println( portbTempValue );
      Serial.print(F("seqDelay="));  Serial.println( seqDelay );
      Serial.print(F("portbTemp="));  Serial.println( portbTemp );
      Serial.print(F("PORTB="));  Serial.println( portbTempValue & sequenceOutputs );  
#endif
     
    }  
  }
}  

  
#endif // end SEQUENCE_OUTPUTS

#endif // end #ifdef PPM_INTERRUPT

#endif //PIN_PPM



#ifdef DEBUG
//************************************************************   OutputToSerial
void OutputToSerial(){
//#define DEBUGMINMAX 0
//#define DEBUGVREF 0
//#define DEBUGDIVIDER 0
//#define DEBUGTEMP 0



#ifdef VARIO
  Serial.print(F(" Sensitivity PPM="));    
  Serial.print( oXs_MS5611.varioData.sensitivityPpm);
  Serial.print(F(" ;absAlt="));  
  Serial.print( ( (float)oXs_MS5611.varioData.absoluteAlt)/100);
  Serial.print(F(" ;Vspd="));    
  Serial.print( ( (float)oXs_MS5611.varioData.climbRate)/100);
  Serial.print(F(" ;Temp="));    
  Serial.print((float)oXs_MS5611.varioData.temperature/10);
#endif // VARIO

#ifdef PIN_CURRENTSENSOR
  Serial.print(F(" ;mA="));    
  Serial.print( ( ((float)(int32_t)(oXs_Current.currentData.milliAmps))) );
  Serial.print("(");    
  Serial.print(F(" ;mAh="));  
  Serial.print( oXs_Current.currentData.consumedMilliAmps);
#endif // PIN_CURRENTSENSOR
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




