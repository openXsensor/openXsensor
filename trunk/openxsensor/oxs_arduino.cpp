#include "Arduino.h"
#include "oxs_arduino.h"
//#include "HardwareSerial.h"
//#include "oxs_config.h" // already in .h file
 
//#define DEBUGNEWVALUE
//#define DEBUGDELAY
//#define DEBUGCELLCALCULATION

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;


#ifdef DEBUG
OXS_ARDUINO::OXS_ARDUINO(HardwareSerial &print) 
#else
OXS_ARDUINO::OXS_ARDUINO(uint8_t x) 
#endif
{
#ifdef DEBUG  
  printer = &print; //operate on the address of print   
#endif
}

// **************** Setup the Current sensor *********************
void OXS_ARDUINO::setupDivider( void ) {
#ifdef USE_INTERNAL_REFERENCE   
  analogReference(INTERNAL) ;
#endif  
  arduinoData.mVoltPin[0] = PIN_Voltage1 ;
  arduinoData.offset[0] = offset_1 ;
  arduinoData.mVoltPerStep[0] = mVoltPerStep_1 ;

  arduinoData.mVoltPin[1] = PIN_Voltage2 ;
  arduinoData.offset[1] = offset_2 ;
  arduinoData.mVoltPerStep[1] = mVoltPerStep_2 ;
  
  arduinoData.mVoltPin[2] = PIN_Voltage3 ;
  arduinoData.offset[2] = offset_3 ;
  arduinoData.mVoltPerStep[2] = mVoltPerStep_3 ;

  arduinoData.mVoltPin[3] = PIN_Voltage4 ;
  arduinoData.offset[3] = offset_4 ;
  arduinoData.mVoltPerStep[3] = mVoltPerStep_4 ;

  arduinoData.mVoltPin[4] = PIN_Voltage5 ;
  arduinoData.offset[4] = offset_5 ;
  arduinoData.mVoltPerStep[4] = mVoltPerStep_5 ;

  arduinoData.mVoltPin[5] = PIN_Voltage6 ;
  arduinoData.offset[5] = offset_6 ;
  arduinoData.mVoltPerStep[5] = mVoltPerStep_6 ;

  for (int cntInit = 0 ; cntInit < 6 ; cntInit++) {
    pinMode(arduinoData.mVoltPin[cntInit],INPUT);
    arduinoData.sumVoltage[cntInit] = 0 ;
    arduinoData.mVoltAvailable[cntInit] = false ; 
  }
  arduinoData.atLeastOneVoltage = ( arduinoData.mVoltPin[0] < 7 || arduinoData.mVoltPin[1] < 7 || arduinoData.mVoltPin[2] < 7 ||arduinoData.mVoltPin[3] < 7 ||arduinoData.mVoltPin[4] < 7 || arduinoData.mVoltPin[5] < 7 ) ;
}

// Maximum voltage that is allowed (theoretical) on the voltage divider is 5/R2/(R1+R2);

  static byte voltageNr = 0;

void OXS_ARDUINO::readSensor() {
// here we should test if Current sensor is used too; otherwise, it is not necessary to use the 2 next instructions

#ifdef DEBUGDELAY
        long milliVoltBegin = millis() ;
#endif

    while ( arduinoData.mVoltPin[voltageNr] > 7) { // Skip nr if voltageNr have not a pin defined between 0 and 7
        voltageNrIncrease();
    }  
    arduinoData.sumVoltage[voltageNr] += readVoltage(voltageNr) ;
#ifdef DEBUGDELAY
        printer->print("readVoltage voltageNr =  ");
        printer->print(voltageNr);
        printer->print(" begin at =  ");
        printer->print(milliVoltBegin);
        printer->print(" end at =  ");
        printer->println(millis());
#endif

    voltageNrIncrease();

}      



// Select next voltage to read ; if more than X voltage have been read, calculates the averages for each voltage 
void OXS_ARDUINO::voltageNrIncrease() {
  static int cnt = 0;
  static unsigned long lastVoltMillis = millis() ;
  static int32_t secondMVolt ;
  static int32_t previousMVolt ;  
  
  voltageNr++;
  if(voltageNr == 6) { 
      voltageNr = 0 ;
      cnt++;
      if(millis() > ( lastVoltMillis + 200) ){   // calculate average only once every 500 msec 
        for (int cntVolt = 0 ; cntVolt < 6 ; cntVolt++) {      
          if ( arduinoData.mVoltPin[cntVolt] < 8) {
            arduinoData.mVolt[cntVolt] = (arduinoData.sumVoltage[cntVolt] / cnt  * arduinoData.mVoltPerStep[cntVolt] ) + arduinoData.offset[cntVolt];
//            arduinoData.mVolt[cntVolt] = (1 + cntVolt) * 3000 + cntVolt * (millis() & 0xFF)  ; // this is just to test the cell calculation ; !!!!!!!!!!!to be removed
            arduinoData.mVoltAvailable[cntVolt] = true ;
            arduinoData.sumVoltage[cntVolt] = 0 ;
#ifdef DEBUGNEWVALUE
            printer->print("At ");
            printer->print(millis());
            printer->print(" Cnt = ");
            printer->print(cnt);
            printer->print(" mVolt ");
            printer->print(cntVolt);
            printer->print(" = ");
            printer->println( arduinoData.mVolt[cntVolt] );
#endif
          } // if
        } // End For   
#if (NUMBEROFCELLS > 0)
        if (NUMBEROFCELLS == 1) {
          secondMVolt = 0 ; 
        }
        else { 
          secondMVolt = arduinoData.mVolt[1]; 
        }
        arduinoData.mVoltCell_1_2 = calculateCell(0, arduinoData.mVolt[0] , secondMVolt , 0) ;
        arduinoData.mVoltCell_1_2_Available = true ;   
        if (NUMBEROFCELLS > 2) {
            if (NUMBEROFCELLS == 3) {
              secondMVolt = 0 ; 
            }
            else { 
              secondMVolt = arduinoData.mVolt[3] ;
            }  
            arduinoData.mVoltCell_3_4 = calculateCell(arduinoData.mVolt[1] , arduinoData.mVolt[2] , secondMVolt , 2) ;
            arduinoData.mVoltCell_3_4_Available = true ;
        }
        if (NUMBEROFCELLS > 4) {
            if (NUMBEROFCELLS == 5) {
              secondMVolt = 0 ; 
            }
            else { 
              secondMVolt = arduinoData.mVolt[5] ;
            }  
            arduinoData.mVoltCell_5_6 = calculateCell(arduinoData.mVolt[3] , arduinoData.mVolt[4] , secondMVolt , 4) ;
            arduinoData.mVoltCell_5_6_Available = true ;
        }
#endif
        cnt=0;
        lastVoltMillis = millis() ;
        
      }   // End if VOLT_BUFFER_LENGTH
   }     // End if == 6
}



void OXS_ARDUINO::resetValues() {
  // not used currently
}    


int OXS_ARDUINO::readVoltage( int value ) { // value is the index in an aray giving the pin to read
  //******** First discharge the capacitor of ADCMux to ground in order to avoid that measurement from another pin has an impact on this measurement  
#ifdef USE_INTERNAL_REFERENCE
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 11 00 1111 (11 = use VRef as max, 1111 = measure ground level)
#else
  ADMUX =  _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0); // binary = 01 00 1111 (01 = use Vcc as max, 1111 = measure ground level)
#endif
    delayMicroseconds(200); // Wait for Vref to settle 
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // wait that conversion is done
  
  analogRead( arduinoData.mVoltPin[value]); // read the value from the sensor 
  // discard the first measurement
  delayMicroseconds(100); // Wait for ADMux to settle 
  return analogRead(arduinoData.mVoltPin[value]); // use the second measurement
}

// calculate 2 cell voltages, make some checks and format in Frsky format.
uint32_t OXS_ARDUINO::calculateCell(int32_t V0 , int32_t V1 , int32_t V2 , int cellId) {
  int32_t cell_1 ;
  int32_t cell_2 ;
  if (V0 < 500) V0 = 0 ;
  if (V1 < 500) V1 = 0 ;
  if (V2 < 500) V2 = 0 ;
  cell_1= V1 - V0 ;
  cell_2= V2 - V1 ;
  if (cell_1 < 500) cell_1 = 0 ;
  if (cell_2 < 500) cell_2 = 0 ;
  cell_1 = (cell_1 >> 1) & 0xFFF ;
  cell_2 = (cell_2 >> 1) & 0xFFF ; 
#ifdef DEBUGCELLCALCULATION
            printer->print("Cell calculation for cellId ");
            printer->print(cellId) ;
            printer->print(" Frist Cell = ");
            printer->print(cell_1) ;
            printer->print(" Second Cell = ");
            printer->print(cell_2) ;
            printer->print(" Frsky value = ");
            printer->println( (cell_2 << 20) | (cell_1 << 8) | ( ( (int32_t) NUMBEROFCELLS)<<4 ) | (int32_t) cellId  , HEX );
#endif
  return (cell_2 << 20) | (cell_1 << 8) | ( ( (int32_t) NUMBEROFCELLS)<<4 ) | (int32_t) cellId ;
}
