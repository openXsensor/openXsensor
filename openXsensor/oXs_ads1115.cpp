#include "oXs_ads1115.h"

#ifdef DEBUG
  //#define DEBUGADS1115SCAN
  //#define DEBUGADS1115EACHREAD
  //#define DEBUGADS1115REQUESTCONV
  //#define DEBUGADS1115MVOLT
  //#define DEBUGADSAIRSPEEDDATA
  //#define DEBUGCURRENT 
#endif

//#define DEBUG_FORCE_ADS_VOLT_1_4_WITHOUT_ADS1115
//#define DEBUG_AIRSPEED_WITH_DUMMY_ADS_DATA

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined( ADS_MEASURE) 
  const uint8_t ads_Measure[4] = {ADS_MEASURE} ; //  how to configure the multiplexer
  const uint8_t ads_Gain[4] = { ADS_FULL_SCALE_VOLT }; //  how to configure the programmable gain amplifier
  const uint8_t ads_Rate[4] = { ADS_RATE }; // how to configure the time of conversion
  const float ads_Offset[4] = { ADS_OFFSET };
  const float ads_Scale[4] = { ADS_SCALE };
  const uint8_t ads_MaxCount[4] = { ADS_AVERAGING_ON } ; //number of conversion before averaging
  
  uint32_t ads_MilliAskConv  ; 
  uint8_t ads_Addr;
  uint8_t ads_CurrentIdx ;
  uint8_t I2CErrorCodeAds1115 ; 
  int32_t ads_SumOfConv[4] ; // summarise all conversion in order to calculate average 
  uint8_t ads_Counter[4]  ;
  struct ONE_MEASUREMENT ads_Conv[4] ; //averaged conversion including offset and scale
  uint8_t ads_Last_Conv_Idx ;


#ifdef DEBUG  
OXS_ADS1115::OXS_ADS1115(uint8_t addr, HardwareSerial &print)
#else
OXS_ADS1115::OXS_ADS1115(uint8_t addr)
#endif
{  // constructor
  ads_Addr=addr; 
#ifdef DEBUG  
  printer = &print; 
#endif
}


// **************** Setup the ADS1115 sensor *********************
void OXS_ADS1115::setup() {
#ifdef DEBUG  
  printer->print(F("ADS1115 sensor I2C Addr="));
  printer->println(ads_Addr,HEX);
#endif
  I2c.begin() ;
  I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop 
#ifdef DEBUGADS1115SCAN
  I2c.scan() ; // scan all I2C address
  printer->print(F("I2C scan adr: "));
  printer->println( I2c.scanAdr , HEX  );
#endif
  ads_Counter[0] = ads_MaxCount[0] ;
  ads_Counter[1] = ads_MaxCount[1] ;
  ads_Counter[2] = ads_MaxCount[2] ;
  ads_Counter[3] = ads_MaxCount[3] ;
  
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
  adsCurrentData.milliAmps.available = false ;
  adsCurrentData.consumedMilliAmps.available = false ;
#endif            
  
  ads_requestNextConv() ; // Write next config and ask for conversion
#ifdef DEBUG  
  printer->print(F("Set up Ads1115 done. I2C Error code= "));
  printer->println(I2CErrorCodeAds1115);
  printer->print(F(" milli="));  
  printer->println(millis());
#endif
}  //end of setup


/****************************************************************************/
/* readSensor - Read ADS115                                                 */
/*********************    *******************************************************/
boolean OXS_ADS1115::readSensor() {  // return true when there is a new average data to calculate
  if ( ( millis() - ads_MilliAskConv ) >  (uint32_t) ( (  0x88 >> ads_Rate[ads_CurrentIdx]) + 1) )   { // when delay of conversion expires (NB delay is 137 msec when ads_rate = 0, and goes down up to 3ms then is divided by 2 for each increase ) 
#ifdef DEBUG_FORCE_ADS_VOLT_1_4_WITHOUT_ADS1115
    ads_SumOfConv[ads_CurrentIdx] += ads_CurrentIdx  + 1 ; // so at the end e.g. the ADS_VOLT_3 should be equal to 3 
    ads_Counter[ads_CurrentIdx]-- ;
    if ( ads_Counter[ads_CurrentIdx] == 0 ) {
      ads_Conv[ads_CurrentIdx].value = round( ((float) ads_SumOfConv[ads_CurrentIdx] / (float) ads_MaxCount[ads_CurrentIdx] / 32768 ) * ads_Scale[ads_CurrentIdx] ) + ads_Offset[ads_CurrentIdx];
      ads_Conv[ads_CurrentIdx].available = true ;
      ads_SumOfConv[ads_CurrentIdx] = 0 ;            // reset the sum 
      ads_Counter[ads_CurrentIdx] = ads_MaxCount[ads_CurrentIdx] ;   // reset the counter to the number of count before averaging
      ads_Last_Conv_Idx = ads_CurrentIdx ;
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
            if ( ads_CurrentIdx == ( ADS_CURRENT_BASED_ON - ADS_VOLT_1 ) ) ads_calculateCurrent() ;
#endif            

#ifdef DEBUG  
      printer->print(F("At ")); printer->print(millis());
      printer->print(F(" idx=")); printer->print(ads_CurrentIdx);
      printer->print(F(" wait=")); printer->print( ( (uint8_t) 0x88 >> ads_Rate[ads_CurrentIdx] ) + 1 );
      printer->print(F(" val=")); printer->print(ads_Conv[ads_CurrentIdx].value);
      printer->println(" ");
#endif
             ads_requestNextConv() ;
             return true ; 
            }  
#else  // normal code = when DEBUG_FORCE_ADS_VOLT_1_4_WITHOUT_ADS1115 is not activated
    if( I2CErrorCodeAds1115 == 0 ) { // if there is no error on previous I2C request
      I2CErrorCodeAds1115 = I2c.write(ads_Addr , 0X0 ) ; // send the Address, 0 = conversion register (in order to be able to read the conversion register)
      if( I2CErrorCodeAds1115 == 0 ) { // if there is no error on previous I2C request
        I2CErrorCodeAds1115 = I2c.read( ads_Addr , 2 ) ; // read 2 bytes , they are in a buffer and must be retrieved with a receive function.
        if( I2CErrorCodeAds1115 == 0 ) { // if there is no error on previous I2C request
            uint16_t valueAdc ;
            valueAdc = I2c.receive() << 8 | I2c.receive() ;
#if defined(ADS_AIRSPEED_BASED_ON) and (ADS_AIRSPEED_BASED_ON >= ADS_VOLT1) and (ADS_AIRSPEED_BASED_ON <= ADS_VOLT_4)
          if (ads_CurrentIdx == ( ADS_AIRSPEED_BASED_ON - ADS_VOLT_1) ) {
            ads_calculate_airspeed( ( int16_t)valueAdc ) ;
          }
#endif                     
          ads_SumOfConv[ads_CurrentIdx] +=   (int16_t) valueAdc ;

#ifdef DEBUGADS1115EACHREAD  
      printer->print(F("At ")); printer->print(millis()) ;
      printer->print(F(" idx=")); printer->print(ads_CurrentIdx) ;
      printer->print(F(" Val=")); printer->print( (int16_t) valueAdc ) ;
      printer->print(F(" count=")); printer->print(ads_Counter[ads_CurrentIdx]) ;
      printer->println(" ");
#endif
          ads_Counter[ads_CurrentIdx]-- ;
          if ( ads_Counter[ads_CurrentIdx] == 0 ) {
            float adcToMvoltScaling ;
            if (ads_Gain[ads_CurrentIdx]) {
              adcToMvoltScaling = (0X2000 >>  ads_Gain[ads_CurrentIdx] ) / 32768.0 ;  // When ads_Gain[] = 1, it means that 32768 = 4096 mvolt; 4096 = 0x1000) , when 2, it is 2048 mvolt, ...
            } else {
              adcToMvoltScaling =  6144  / 32768.0 ;  // When ads_Gain[] = 0, it means that 32768 = 6144 mvolt
            }
            ads_Conv[ads_CurrentIdx].value = round( ((float) ads_SumOfConv[ads_CurrentIdx] / (float) ads_MaxCount[ads_CurrentIdx] * adcToMvoltScaling ) * ads_Scale[ads_CurrentIdx] ) + ads_Offset[ads_CurrentIdx];
            ads_Conv[ads_CurrentIdx].available = true ;
#ifdef DEBUGADS1115MVOLT  
      printer->print(F("At ")); printer->print(millis()) ;
      printer->print(F(" idx=")); printer->print(ads_CurrentIdx) ;
      printer->print(F(" Val=")); printer->print( ads_Conv[ads_CurrentIdx].value ) ;
      printer->println(" ");
#endif

            
            ads_SumOfConv[ads_CurrentIdx] = 0 ;            // reset the sum 
            ads_Counter[ads_CurrentIdx] = ads_MaxCount[ads_CurrentIdx] ;   // reset the counter to the number of count before averaging
            ads_Last_Conv_Idx = ads_CurrentIdx ;
#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON)
            if ( ads_CurrentIdx == ( ADS_CURRENT_BASED_ON - ADS_VOLT_1 ) ) ads_calculateCurrent() ;
#endif            
          }
          ads_requestNextConv() ;
          return true ;
        }
      }
    }
//    resetI2c() ;
#endif // end of debug with forced values
    ads_requestNextConv() ; // new request for conversion in case of error 
    
  }
  return false ;
} // end of readSensor


void OXS_ADS1115::ads_requestNextConv(void) {
    do {
      ads_CurrentIdx++ ;
      if( ads_CurrentIdx > 3 ) ads_CurrentIdx = 0 ;
    } while (  ads_Measure[ads_CurrentIdx] == ADS_OFF ) ;
// perhaps this line has to be splitted in 2 in order to let multiplexer, gain and rate to set up before asking for a conversion.

    uint8_t dataToWrite[2] ;
    dataToWrite[0] = (( 1 << 7 | ads_Measure[ads_CurrentIdx] << 4 | ads_Gain[ads_CurrentIdx] << 1 | 1  ) ) ;
    dataToWrite[1] = ( ads_Rate[ads_CurrentIdx] << 5 | 0B11 );
        // bit 15 says that a conversion is requested, bit 8 says on shot mode, bits 0 and 1 = 11 says comparator is disabled.
    I2CErrorCodeAds1115 = I2c.write((uint8_t) ads_Addr , (uint8_t) 0X01 , (uint8_t) 2 , &dataToWrite[0] ) ; // send the Address, 1 = config register , 2 bytes , pointer to the data to write
//    if ( I2CErrorCodeAds1115 ) I2CErrorCodeAds1115 = I2c.write( (uint8_t) ads_Addr , (uint8_t) 0X01 , (uint8_t) 2 , &dataToWrite[0] ) ; // retry once if there is an error (probably we should add a clear of I2C bus in between)
#ifdef DEBUGADS1115REQUESTCONV  
      printer->print(F("At ")); printer->print(millis()) ;
      printer->print(F(" cmd=")); printer->print(dataToWrite[0], HEX) ;
      printer->print(F(" ")); printer->print( dataToWrite[1] , HEX ) ;
      printer->println(" ");
#endif

    
    
    ads_MilliAskConv = millis() ;
} // end of Ads_requestNextConv


#if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON) // this part is compiled only when the config ask for current
void OXS_ADS1115::ads_calculateCurrent(void) {
    
    static int32_t sumCurrent = 0 ;
    static uint16_t cnt ;
    static uint32_t milliTmp ;
    static uint32_t lastCurrentMillis ;
    sumCurrent +=  ads_Conv[ads_CurrentIdx].value ;
    cnt++ ;
    milliTmp = millis() ;
  if  (lastCurrentMillis == 0)  {
    lastCurrentMillis = milliTmp ;
  }
  else if (  (milliTmp - lastCurrentMillis ) > 200 )  {   // calculate average only once per 200 millisec
      adsCurrentData.milliAmps.value = ((sumCurrent / cnt) - MVOLT_AT_ZERO_AMP ) * 1000 / MVOLT_PER_AMP ;
//      if (currentData.milliAmps.value < 0) currentData.milliAmps.value = 0 ;
      adsCurrentData.milliAmps.available = true ;
      floatConsumedMilliAmps += ((float) adsCurrentData.milliAmps.value) * (milliTmp - lastCurrentMillis ) / 3600.0 /1000.0 ;   
      adsCurrentData.consumedMilliAmps.value = (int32_t) floatConsumedMilliAmps ;
      adsCurrentData.consumedMilliAmps.available = true ;
      lastCurrentMillis =  milliTmp ;
#ifdef DEBUGCURRENT
      printer->print("At time  = ");
      printer->print(milliTmp);
      printer->print(" Cnt = ");
      printer->print(cnt);
      printer->print(" average current =  ");
      printer->print(adsCurrentData.milliAmps.value);
      printer->print(" consumed milliAmph =  ");
      printer->println(adsCurrentData.consumedMilliAmps.value);
#endif
      sumCurrent = 0;
      cnt = 0;
  } 
}
#endif


#if defined(ADS_AIRSPEED_BASED_ON) and (ADS_AIRSPEED_BASED_ON >= ADS_VOLT1) and (ADS_AIRSPEED_BASED_ON <= ADS_VOLT_4) // this part is compiled only when required
float ads_sumDifPressureAdc_0 ;
uint8_t ads_cntDifPressureAdc_0 ;

void OXS_ADS1115::ads_calculate_airspeed( int16_t ads_difPressureAdc ) {
  // convert ads_volt to pressure.
  static int32_t ads_pressure;
  static int32_t difPressureSum ;
  static float offset7002 ;
  static int16_t calibrateCount7002 ;
  static boolean calibrated7002 = false ;
  static float ads_difPressureAdc_0 ;
  static float ads_abs_deltaDifPressureAdc ;
  static float ads_smoothDifPressureAdc ;
  static float expoSmooth7002_adc_auto ;
  static int32_t ads_smoothAirSpeed ; 
  uint32_t ads_airSpeedMillis ;
  static uint32_t ads_nextAirSpeedMillis ;
//#define DEBUG_AIRSPEED_WITH_DUMMY_ADS_DATA
#ifdef DEBUG_AIRSPEED_WITH_DUMMY_ADS_DATA
  static int16_t dummy_ads_value ; 
  ads_difPressureAdc = ((millis() / 1000 ) % (100) ) * 300 ; 
#endif 
  if ( calibrated7002 == false) {
       calibrateCount7002++ ;
       if (calibrateCount7002 == 256 ) { // after 256 reading , we can calculate the offset 
         offset7002 =  (  (float) difPressureSum / 128.0 ) ; //there has been 128 reading (256-128)                     
         calibrated7002 = true ;
       } else if  (calibrateCount7002 >= 128  ){ // after 128 reading, we can start cummulate the ADC values in order to calculate the offset 
          difPressureSum += ads_difPressureAdc ;
       } // end calibration
  }  else { // sensor is calibrated
                    ads_difPressureAdc_0 = ( ads_difPressureAdc - offset7002 )  ;
                    ads_sumDifPressureAdc_0 += ads_difPressureAdc_0 ;
                    ads_cntDifPressureAdc_0++ ;
#define FILTERING7002_ADC_MIN        0.001 // 
#define FILTERING7002_ADC_MAX        0.01  // 
#define FILTERING7002_ADC_MIN_AT       10  // when abs(delta between ADC and current value) is less than MIN_AT , apply MIN  
#define FILTERING7002_ADC_MAX_AT       100 // when abs(delta between ADC and current value) is more than MAX_AT , apply MAX (interpolation in between)
                    ads_abs_deltaDifPressureAdc =  abs(ads_difPressureAdc_0 - ads_smoothDifPressureAdc) ;
                    if (ads_abs_deltaDifPressureAdc <= FILTERING7002_ADC_MIN_AT) {
                       expoSmooth7002_adc_auto = FILTERING7002_ADC_MIN ;  
                    } else if (ads_abs_deltaDifPressureAdc >= FILTERING7002_ADC_MAX_AT)  {
                       expoSmooth7002_adc_auto = FILTERING7002_ADC_MAX ; 
                    } else {
                       expoSmooth7002_adc_auto = FILTERING7002_ADC_MIN + ( FILTERING7002_ADC_MAX - FILTERING7002_ADC_MIN) * (ads_abs_deltaDifPressureAdc - FILTERING7002_ADC_MIN_AT) / (FILTERING7002_ADC_MAX_AT - FILTERING7002_ADC_MIN_AT) ;
                    }
                    if ( ( ads_smoothDifPressureAdc <=2 ) && ( ads_smoothDifPressureAdc >= -2 ) ) expoSmooth7002_adc_auto *= 0.2 ; 
                    ads_smoothDifPressureAdc += expoSmooth7002_adc_auto * ( ads_difPressureAdc_0 - ads_smoothDifPressureAdc ) ; // 

               // calculate airspeed based on pressure, altitude and temperature
               // airspeed (m/sec) = sqr(2 * differential_pressure_in_Pa / air_mass_kg_per_m3) 
               // air_mass_kg_per_m3 = pressure_in_pa / (287.05 * (Temp celcius + 273.15))
               // and differantial_pressure_Pa =  ((smoothDifPressureAdc  ) * 2048 / 32768) ;  // with 7002, 1 mvolt = 1 pa and ads1115 ADC gives 32768 when volt = 2048 mvolt; so 1 step ADC = 2048 / 32768
                //2048 is used because we supposed that ads gain is set on 2048 ; MPXV7002 provides 1 mvolt per pa wit 5 volt Vcc; it is ratiometric so there should be some Vcc correction
               // so airspeed m/sec =sqr( 2 * 287.05 *  2048 / 32768 * smoothDifPressureAdc * (temperature Celsius + 273.15) / pressure_in_pa )
               // rawAirSpeed cm/sec =  5,99 * 100 * sqrt( (float) abs(smoothDifPressureAdc) * temperature4525  /  actualPressure) ); // in cm/sec ; actual pressure must be in Pa (so 101325 about at sea level
               //                    =  32.32 * sqrt( (float) abs(smoothDifPressureAdc) ); // in cm/sec ; if pressure is standard = 101325 and temp = 15 C°)
               //                    =  10256 * sqrt( (float) abs(smoothDifPressureAdc) /  actualPressure) ); // in cm/sec ; temp is supposed to be 20 C°, pressure is in Pa
#ifdef AIRSPEED_AT_SEA_LEVEL_AND_15C
               ads_smoothAirSpeed =  32.32 * sqrt( (float) ( abs(ads_smoothDifPressureAdc) ) ); // indicated airspeed is calculated at 15 Celsius and 101325 pascal
#else               
               ads_smoothAirSpeed =  10256.0 * sqrt( (float) ( abs(ads_smoothDifPressureAdc)  /  (float) actualPressure) ); // in cm/sec ; actual pressure must be in pa (so 101325 about at sea level)
#endif              
             if ( ads_smoothDifPressureAdc < 0 ) ads_smoothAirSpeed = - ads_smoothAirSpeed ; // apply the sign
      
  }  // end of test on calibration
  ads_airSpeedMillis = millis() ;
  if ( ads_airSpeedMillis  > ads_nextAirSpeedMillis){ // publish airspeed only once every xx ms
              ads_nextAirSpeedMillis = ads_airSpeedMillis + 200 ;
//              if ( ads_smoothAirSpeed >  0) {  // normally send only if positive and greater than 300 cm/sec , otherwise send 0 but for test we keep all values to check for drift  
#ifdef AIRSPEED_IN_KMH  // uncomment this line if AIR speed has to be in knot instead of km/h
                  adsAirSpeedData.airSpeed.value = ads_smoothAirSpeed * 0.36 ; // from cm/sec to 1/10 km/h
#else
                  adsAirSpeedData.airSpeed.value = ads_smoothAirSpeed * 0.1943844492 ; // from cm/sec to 1/10 knot/h
#endif
//              } else {
//                  adsAirSpeedData.airSpeed.value = 0 ;
//              }    
              adsAirSpeedData.airSpeed.available = true ; 

// check if offset must be reset
              if (adsAirSpeedData.airspeedReset) { // adjust the offset if a reset command is received from Tx
                    offset7002 =  offset7002  + ads_smoothDifPressureAdc ;
                    ads_smoothDifPressureAdc = 0 ;
                    adsAirSpeedData.airspeedReset = false ; // avoid that offset is changed again and again if PPM do not send a command
              }
#ifdef DEBUGADSAIRSPEEDDATA
                  static bool firstRawData = true ;
                  if ( firstRawData ) {
                          printer->println(F("at,  difPressureAdc ,difPressADC_0 , ads_smoothDifPressureAdc , actualPressure , adsAirSpeedData.airSpeed, offset, reset  ")) ;
                        firstRawData = false ;
                  } else {
                        printer->print( ads_airSpeedMillis ); printer->print(F(" , "));
                        printer->print( ads_difPressureAdc ); printer->print(F(" , "));
                        printer->print( ads_difPressureAdc_0); printer->print(F(" , "));
                        printer->print( ads_smoothDifPressureAdc * 1000); printer->print(F(" , ")); 
                        printer->print( actualPressure ); printer->print(F(" , ")); 
                        printer->print( ads_smoothAirSpeed * 3.6 / 100 ); printer->print(F(" , "));
                        printer->print( offset7002 * 1000); printer->print(F(" , ")); 
                        if ( ads_smoothDifPressureAdc == 0) printer->print(F(" reset")) ;
                        
                        printer->println(" ") ; 
                  }       
#endif


 
  } // end test on millis()
  
}
#endif // end of conditional compiling for calculate airspeed.

#endif // end of #if defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined( ADS_MEASURE) 


