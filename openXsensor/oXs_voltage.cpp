#include "oXs_voltage.h"
 
#ifdef DEBUG
//#define DEBUGNEWVALUE
//#define DEBUGDELAY
//#define DEBUGCELLCALCULATION
//#define DEBUGLOWVOLTAGE
//#define DEBUGNTC
#endif

#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES)

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;


#ifdef DEBUG
OXS_VOLTAGE::OXS_VOLTAGE(HardwareSerial &print) 
#else
OXS_VOLTAGE::OXS_VOLTAGE(uint8_t x) 
#endif
{
#ifdef DEBUG  
  printer = &print; //operate on the address of print   
#endif
}

// **************** Setup the Current sensor *********************
void OXS_VOLTAGE::setupVoltage( void ) {
  uint16_t tempRef ;
#ifdef DEBUG  
  printer->println("Enter setup voltage");
#endif

#ifdef USE_INTERNAL_REFERENCE   
  analogReference(INTERNAL) ;
#elif defined(USE_EXTERNAL_REFERENCE)
  analogReference(EXTERNAL) ;
#endif
#ifdef PIN_VOLTAGE  
  uint8_t tempPin[6 ] = { PIN_VOLTAGE };
#else
  uint8_t tempPin[6 ] = { 8 , 8 , 8 , 8 , 8 , 8 } ;
#endif  
#ifdef RESISTOR_TO_GROUND 
  float tempResistorToGround[6] = { RESISTOR_TO_GROUND } ;
#else
  float tempResistorToGround[6] = { 0 , 0 , 0 , 0 , 0 , 0 } ;
#endif
#ifdef RESISTOR_TO_VOLTAGE  
  float tempResistorToVoltage[6] = { RESISTOR_TO_VOLTAGE } ;
#else
  float tempResistorToVoltage[6] = { 0 , 0 , 0 , 0 , 0 , 0 } ;
#endif
#ifdef OFFSET_VOLTAGE  
  int tempOffsetVoltage[6] = { OFFSET_VOLTAGE} ;
#else 
  int tempOffsetVoltage[6] =  { 0 , 0 , 0 , 0 , 0 , 0 } ;
#endif
#ifdef SCALE_VOLTAGE   
  float tempScaleVoltage[6] = { SCALE_VOLTAGE }  ;
#else
  float tempScaleVoltage[6] =  { 1 , 1 , 1 , 1 , 1 , 1 } ;
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

#ifdef DEBUG  
  printer->print("Reference voltage:");
  printer->println(tempRef);
#endif
  voltageData.atLeastOneVolt = false ;
  for (int cntInit = 0 ; cntInit < 6 ; cntInit++) {
    if ( tempPin[ cntInit ] < 8 ) {
      voltageData.mVoltPin[cntInit] =  tempPin[ cntInit ] ;
      pinMode(voltageData.mVoltPin[cntInit],INPUT);
      voltageData.atLeastOneVolt = true ;
    } else {
      voltageData.mVoltPin[cntInit] = 8 ;
    }
    voltageData.offset[cntInit] = tempOffsetVoltage[ cntInit ] ;
    if ( tempResistorToGround[cntInit] > 0 && tempResistorToVoltage[cntInit] > 0 && tempScaleVoltage[cntInit] > 0 ) {
      voltageData.mVoltPerStep[cntInit] = tempRef / 1023.0 * ( tempResistorToGround[cntInit] + tempResistorToVoltage[cntInit] ) / tempResistorToGround[cntInit]  * tempScaleVoltage[cntInit];
    } else {
      voltageData.mVoltPerStep[cntInit] = tempRef / 1023.0  * tempScaleVoltage[cntInit];  
    }
    voltageData.sumVoltage[cntInit] = 0 ;
    voltageData.mVolt[cntInit].available = false ; 
#ifdef DEBUG  
    printer->print("Voltage:"); printer->print( cntInit + 1 );
    printer->print(" , pin="); printer->print( voltageData.mVoltPin[cntInit] );
    printer->print(" , offset="); printer->print( voltageData.offset[cntInit] );
    printer->print("  , mVoltPerStep="); printer->println( voltageData.mVoltPerStep[cntInit] );
#endif
    
  }
//  voltageData.atLeastOneVoltage = ( voltageData.mVoltPin[0] < 8 || voltageData.mVoltPin[1] < 8 || voltageData.mVoltPin[2] < 8 ||voltageData.mVoltPin[3] < 8 ||voltageData.mVoltPin[4] < 8 || voltageData.mVoltPin[5] < 8 ) ;
}



// Maximum voltage that is allowed (theoretical) on the voltage divider is 5/R2/(R1+R2);

  static byte voltageNr = 0;

void OXS_VOLTAGE::readSensor() {

    if (voltageData.atLeastOneVolt) { // no calculation if there is no voltage to calculate.
#ifdef DEBUGDELAY
        long milliVoltBegin = micros() ;
#endif

        while ( voltageData.mVoltPin[voltageNr] > 7) { // Skip nr if voltageNr have not a pin defined between 0 and 7
            voltageNrIncrease();                       // Find next voltage to be read; if overlap, calculate average for each voltage
        }  
        voltageData.sumVoltage[voltageNr] += readVoltage(voltageNr) ;   // read voltage 
#ifdef DEBUGDELAY
        milliVoltBegin = milliVoltBegin - micros() ;
        printer->print("VoltageNr ");
        printer->print(voltageNr);
        printer->print(" in ");
        printer->println(milliVoltBegin);
#endif

        voltageNrIncrease();                          // Find next voltage to be read; if overlap, calculate average for each voltage
    }
}      



// Select next voltage to read ; if all voltages have been read and 500 ms are enlapsed since previous average calculation, calculates the new averages for each voltage 
void OXS_VOLTAGE::voltageNrIncrease() {
  static int cnt = 0;
  static unsigned long lastVoltMillis = millis() ;
#if defined ( NUMBEROFCELLS ) && (NUMBEROFCELLS > 0)
    static int32_t secondMVolt ;
#endif  
  //static int32_t previousMVolt ;  
  
  voltageNr++;
  if(voltageNr == 6) { 
      voltageNr = 0 ;
      cnt++;
      if(millis() > ( lastVoltMillis + 500) ){   // calculate average only once every 500 msec 
        for (int cntVolt = 0 ; cntVolt < 6 ; cntVolt++) {      
          if ( voltageData.mVoltPin[cntVolt] < 8) {
            voltageData.mVolt[cntVolt].value = round( ((float) voltageData.sumVoltage[cntVolt]  * voltageData.mVoltPerStep[cntVolt] ) / (float) cnt  ) + voltageData.offset[cntVolt];
//            voltageData.mVolt[cntVolt].value = (voltageData.sumVoltage[cntVolt] / cnt  * voltageData.mVoltPerStep[cntVolt] ) + voltageData.offset[cntVolt];
//            voltageData.mVolt[cntVolt].value = (1 + cntVolt) * 3000 + cntVolt * (millis() & 0xFF)  ; // this is just to test the cell calculation ; !!!!!!!!!!!to be removed
            voltageData.mVolt[cntVolt].available = true ;
            voltageData.sumVoltage[cntVolt] = 0 ;
#ifdef DEBUGNEWVALUE
            printer->print("At ");
            printer->print(millis());
            printer->print(" Cnt = ");
            printer->print(cnt);
            printer->print(" mVolt ");
            printer->print(cntVolt);
            printer->print(" = ");
            printer->println( voltageData.mVolt[cntVolt].value );
#endif
          } // if
        } // End For 
        
#if defined ( NUMBEROFCELLS ) && (NUMBEROFCELLS > 0)
        voltageData.maxNumberOfCells = 0 ;
        int32_t mVoltOneCell ;
        uint8_t prevIndex ;
        for (uint8_t cellIndex = 0; cellIndex < NUMBEROFCELLS ; cellIndex++) {
          if (cellIndex == 0) {
            mVoltOneCell = voltageData.mVolt[cellIndex].value;
          } else { 
            mVoltOneCell = voltageData.mVolt[cellIndex].value - voltageData.mVolt[prevIndex].value ;
          }
          prevIndex = cellIndex ;        
          if (mVoltOneCell  < 500) {
            mVoltOneCell = 0 ;
          } else {
            voltageData.maxNumberOfCells = cellIndex + 1 ;
          }
          voltageData.mVoltCell[cellIndex]  = mVoltOneCell ;
          voltageData.mVoltCell_Available[cellIndex] = true ;
        }
        voltageData.mVoltCellMin = 0 ;
        voltageData.mVoltCellTot = 0 ;
        for (uint8_t cellIndex = 0; cellIndex < voltageData.maxNumberOfCells ; cellIndex++) {
          if (voltageData.mVoltCell[cellIndex] == 0 ) {
            break ;
          } else {
            if ( (voltageData.mVoltCellMin == 0) || ( voltageData.mVoltCellMin > voltageData.mVoltCell[cellIndex] ) ){
              voltageData.mVoltCellMin = voltageData.mVoltCell[cellIndex] ;
            }
              voltageData.mVoltCellTot = voltageData.mVolt[cellIndex].value ;            
          }  
        }
        if ( voltageData.mVoltCellMin > 0 ) {
          voltageData.mVoltCellMin_Available = true ;
        }
        voltageData.mVoltCellTot_Available = true ;
#if ( PROTOCOL  ==  FRSKY_SPORT ) || ( PROTOCOL  ==  FRSKY_HUB ) || ( PROTOCOL  ==  FRSKY_SPORT_HUB )  // For FRSKY protocols 
        if (voltageData.maxNumberOfCells > 0) {
            if (voltageData.maxNumberOfCells == 1) {
              secondMVolt = 0 ; 
            }
            else { 
              secondMVolt = voltageData.mVolt[1].value; 
            }
            voltageData.mVoltCell_1_2.value = calculateCell(0, voltageData.mVolt[0].value , secondMVolt , 0, voltageData.maxNumberOfCells ) ;
            voltageData.mVoltCell_1_2.available = true ;   
        }    
        if (voltageData.maxNumberOfCells > 2) {
            if (voltageData.maxNumberOfCells == 3) {
              secondMVolt = 0 ; 
            }
            else { 
              secondMVolt = voltageData.mVolt[3].value ;
            }  
            voltageData.mVoltCell_3_4.value = calculateCell(voltageData.mVolt[1].value , voltageData.mVolt[2].value , secondMVolt , 2 , voltageData.maxNumberOfCells) ;
            voltageData.mVoltCell_3_4.available = true ;
        }
        if (voltageData.maxNumberOfCells > 4) {
            if (voltageData.maxNumberOfCells == 5) {
              secondMVolt = 0 ; 
            }
            else { 
              secondMVolt = voltageData.mVolt[5].value ;
            }  
            voltageData.mVoltCell_5_6.value = calculateCell(voltageData.mVolt[3].value , voltageData.mVolt[4].value , secondMVolt , 4 , voltageData.maxNumberOfCells) ;
            voltageData.mVoltCell_5_6.available = true ;
        }
#endif // Enf of Frsky protocols
#endif // ( NUMBEROFCELLS ) && (NUMBEROFCELLS > 0)

#if defined  ( FIRST_NTC_ON_VOLT_NR ) && defined ( LAST_NTC_ON_VOLT_NR ) && ( FIRST_NTC_ON_VOLT_NR <= 6 ) && ( LAST_NTC_ON_VOLT_NR <= 6) &&  ( FIRST_NTC_ON_VOLT_NR >= 1 ) && ( LAST_NTC_ON_VOLT_NR >= 1 ) && ( FIRST_NTC_ON_VOLT_NR <= LAST_NTC_ON_VOLT_NR )
        uint8_t numberOfNtc = LAST_NTC_ON_VOLT_NR - FIRST_NTC_ON_VOLT_NR + 1;
        uint8_t minNtcIdx ;
        uint8_t maxNtcIdx ;
        int32_t minTemp = 99999999 ;
        int32_t maxTemp = 0 ;
        
       for ( uint8_t iNtc = FIRST_NTC_ON_VOLT_NR - 1 ; iNtc <= LAST_NTC_ON_VOLT_NR - 1 ; iNtc++ ) {
          convertNtcVoltToTemp( voltageData.mVolt[iNtc].value ) ;
          if (voltageData.mVolt[iNtc].value < minTemp ) {
            minTemp = voltageData.mVolt[iNtc].value ;
            minNtcIdx = iNtc + 1 ; 
          }
          if (voltageData.mVolt[iNtc].value > maxTemp ) {
            maxTemp = voltageData.mVolt[iNtc].value ;
            maxNtcIdx = iNtc + 1 ;
          }
        }  // end for   
        if ( numberOfNtc  > 2) {
          voltageData.mVolt[FIRST_NTC_ON_VOLT_NR - 1].value =  minTemp ;
          voltageData.mVolt[FIRST_NTC_ON_VOLT_NR ].value =  maxNtcIdx ;
          voltageData.mVolt[LAST_NTC_ON_VOLT_NR - 1].value =  maxTemp ; 
        }        
#endif  // end of NTC calculations

        cnt=0;
        lastVoltMillis = millis() ;
#ifdef SEQUENCE_OUTPUTS
        lowVoltage = false ;
#if defined( SEQUENCE_MIN_CELL)  
        if ( voltageData.mVoltCellMin < SEQUENCE_MIN_CELL) {        
          lowVoltage = true ;
        }
#endif //SEQUENCE_MIN_CELL
#if defined( SEQUENCE_MIN_VOLT_6) && defined (PIN_VOLTAGE_6 )
        if ( voltageData.mVolt[5].value < SEQUENCE_MIN_VOLT_6) {        
          lowVoltage = true ;
        }
#endif //( SEQUENCE_MIN_VOLT_6) && defined (PIN_VOLTAGE_6 )
#ifdef DEBUGLOWVOLTAGE
            printer->print("LowVoltage= ");
            printer->println(lowVoltage) ;
#endif
#endif // SEQUENCE_OUTPUTS
        
      }   // End if VOLT_BUFFER_LENGTH
   }     // End if == 6
}




int OXS_VOLTAGE::readVoltage( int value ) { // value is the index in an aray giving the pin to read
  //******** First discharge the capacitor of ADCMux to ground in order to avoid that measurement from another pin has an impact on this measurement  
#ifdef USE_INTERNAL_REFERENCE
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use internal VRef as max, 1111 = measure ground level)
#elif defined(USE_EXTERNAL_REFERENCE)
  ADMUX =  _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);                          // binary = 00 00 1111 (00 = use external VRef as max, 1111 = measure ground level)
#else
  ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0);             // binary = 01 00 1111 (01 = use Vcc as max, 1111 = measure ground level)
#endif
    delayMicroseconds(200); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done ; this takes 13 cycles of 125 khz (clock set in oXs_general.ccp so 104 usec
  
  analogRead( voltageData.mVoltPin[value]); // read the value from the sensor ; it requires about 120 usec 
  // discard the first measurement
  delayMicroseconds(100); // Wait for ADMux to settle 
  return analogRead(voltageData.mVoltPin[value]); // use the second measurement ; it requires about 120 usec
}


void OXS_VOLTAGE::resetValues() {
  // not used currently
}    



#if defined ( NUMBEROFCELLS ) && (NUMBEROFCELLS > 0 )
// calculate 2 cell voltages, make some checks and format in Frsky format.
uint32_t OXS_VOLTAGE::calculateCell(int32_t V0 , int32_t V1 , int32_t V2 , uint8_t cellId , uint8_t  maxNumberOfCells) {
  int32_t cell_1 ;
  int32_t cell_2 ;
  if (V0 < 500) V0 = 0 ;
  if (V1 < 500) V1 = 0 ;
  if (V2 < 500) V2 = 0 ;
  cell_1= V1 - V0 ;
  cell_2= V2 - V1 ;
  if (cell_1 < 500) cell_1 = 0 ;
  if (cell_2 < 500) cell_2 = 0 ;
  cell_1 = (cell_1 >> 1) & 0xFFF ; //Frsky expects that the value are divided by 2
  cell_2 = (cell_2 >> 1) & 0xFFF ; //Frsky expects that the value are divided by 2
#ifdef DEBUGCELLCALCULATION
            printer->print("Cell calculation for cellId ");
            printer->print(cellId) ;
            printer->print(" Frist Cell = ");
            printer->print(cell_1) ;
            printer->print(" Second Cell = ");
            printer->print(cell_2) ;
            printer->print(" Frsky value = ");
            printer->println( (cell_2 << 20) | (cell_1 << 8) | ( ( (int32_t) voltageData.maxNumberOfCells)<<4 ) | (int32_t) cellId  , HEX );
#endif
  return (cell_2 << 20) | (cell_1 << 8) | ( ( (int32_t) voltageData.maxNumberOfCells )<<4 ) | (int32_t) cellId ;
}
#endif // end calculateCell


#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined ( PIN_VOLTAGE ) && defined ( FIRST_NTC_ON_VOLT_NR )
void  OXS_VOLTAGE::convertNtcVoltToTemp (int32_t &voltage ) {     //Calculate temperature using the Beta Factor equation
        // Convert the thermal stress value to resistance
        // we reuse here the mVolt calculated by oXs. The config must be adapted in a such a way that this mVolt is equal to the raw value returned by the ADC * 1000 (for better accuracy)
        // therefore, the mVoltPerStep calculated must be equal to 1000 and so :
        // USE_INTERNAL_REFERENCE must be as comment (so with // in front off)
        // USE_EXTERNAL_REFERENCE must be as comment (so with // in front off)
        // REFERENCE_VOLTAGE must be as comment (so with // in front off)
        // PIN_VOLTAGE must be defined and the analog pin used for NTC must be specified in one of the 6 index        
        //  RESISTOR_TO_GROUND must be set on 0 (for the index being used)
        //  OFFSET_VOLTAGE must (normally) be set on 0 (for the index being used)
        //  SCALE_VOLTAGE  must be set on 204.6 (=1000 * 1023/5000) (for the index being used)
        // The calculated temperature is filled back in the voltage field
        // If there are more than 2 NTC, voltage from FIRST_NTC is filled with the lowest temp, voltage from LAST_NTC is filled with highest temp and voltage from FIRST_NTC+1 is filled with the index of the highest temp
#if defined( DEBUG) && defined (DEBUGNTC)
         Serial.print( "Voltage= " ) ; Serial.print( voltage );
#endif
        
        float media =  SERIE_RESISTOR /  ( (1023000.0 / voltage ) - 1 ) ;
/*        
        float temperatura = media / TERMISTOR_NOMINAL;     // (R/Ro)
        temperatura = log(temperatura); // ln(R/Ro)
        temperatura /= B_COEFFICIENT;                   // 1/B * ln(R/Ro)
        temperatura += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
        temperatura = 1.0 / temperatura ;                 // Invert the value
        temperatura -= 273.15 ;                         // Convert it to Celsius
        voltage = temperatura ;
*/                
        // T = 1/(A + B* ln(R) + C * ln(R) *ln(R) *ln(R)) 
        float steinhart = log(media) ;
        steinhart = 1 / ( STEINHART_A + STEINHART_B * steinhart + STEINHART_C * steinhart * steinhart * steinhart) ;
        steinhart -= 273.15 ;
        voltage = steinhart ;    

#if defined( DEBUG) && defined (DEBUGNTC)
         Serial.print( "Temp= " ) ; Serial.println( voltage );
#endif
              
    }  // end convertNtcVoltToTemp 
#endif



#endif // end of PIN_VOLTAGE
