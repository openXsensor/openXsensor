#include "Arduino.h"
//#include "oxs_config.h"  // already in .h file

//#define POLLINGI2C	1
//#ifdef POLLINGI2C
#include "I2C.h"
//#else
//#include <Wire.h>           // Mike, I presume this can be removed now, normally we should use only I2C
//#endif
#include "oxs_ms5611.h"

//#define DEBUGDATA

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#ifdef DEBUG  
OXS_MS5611::OXS_MS5611(uint8_t addr, HardwareSerial &print)
#else
OXS_MS5611::OXS_MS5611(uint8_t addr)
#endif
{
  // constructor
  _addr=addr;
  varioData.SensorState = 0 ;
#ifdef DEBUG  
  printer = &print; //operate on the address of print
  printer->begin(115200);
  printer->print("Vario Sensor:MS5611 I2C Addr=");
  printer->println(addr,HEX);
#endif
}


// **************** Setup the MS5611 sensor *********************
void OXS_MS5611::setup() {
  varioData.available=false;
  varioData.absoluteAltAvailable=false ;
  varioData.climbRateAvailable=false ;
  varioData.sensitivityAvailable=false ;
  varioData.vSpeed10SecAvailable=false ;
  varioData.sensitivityPpm = 0 ;
  varioData.idxPrevAlt = 0 ;
  
// #ifdef POLLINGI2C
  I2c.begin() ;
  I2c.write( _addr,0x1e) ;
//#else
//  Wire.begin();
//  SendCommand(0x1e);
//#endif
  delay(100);
  for (byte i = 1; i <=6; i++) {
    unsigned int low, high;
//#ifdef POLLINGI2C
    I2c.read( _addr, 0xa0 + i*2, 2 ) ; //read 2 bytes from the device
    high = I2c.receive() ;
    low = I2c.receive() ;
//#else		
//    SendCommand(0xa0 + i * 2);
//    Wire.beginTransmission(_addr);  
//    Wire.requestFrom((uint8_t)_addr, (uint8_t)2);
//    if(Wire.available()!=2) {
// #ifdef DEBUG
//      printer->println("Error: calibration data not available");
// #endif
//    }
//    high = Wire.read();
//    low = Wire.read();
//#endif
    _calibrationData[i] = high<<8 | low;
#ifdef DEBUG
    printer->print("calibration data #");
    printer->print(i);
    printer->print(" = ");
    printer->println( _calibrationData[i] ); 
#endif
  }

#ifdef DEBUG  
  printer->println("done.");
#endif
  
  resetValues();
      micros() ; // Mike, if this instruction is removed then the program do not run anymore with the options VARIO and DEBUG (it compiles but when running the monitor block after displaying calibration 3 
}  //end of setup

/****************************************************************/
/* SendCommand - Send a command to the I2C Bus               */
/****************************************************************/
/* Send a command to the MS5611 */
//void OXS_MS5611::SendCommand(byte command) {
//  Wire.beginTransmission(_addr);
//#ifdef DEBUG
//  printer->print("Vario Sensor:MS5611 BEFORE WRITE");
//#endif
//  if (!Wire.write(command)){
//#ifdef DEBUG
//    //printer->println("Error: write()");
//#endif 
//  }
//  if (Wire.endTransmission())   {
//#ifdef DEBUG
//    //printer->print("Error when sending command: ");
//    //printer->println(command, HEX);
//#endif
//#ifdef DEBUG
//  printer->print("Vario Sensor:MS5611 after WRITE");
//#endif
//
//  }
//} //end SendCommand

/****************************************************************/
/* readSensor - Read pressure + temperature from the MS5611    */
/****************************************************************/
//byte recalcCnt=0;

long result ;

void OXS_MS5611::readSensor() {
                                    // To do, some variable declarations could be used with  #ifdefDEBUGxxx to save space   
    static int32_t D1 = 0;
    static int32_t D2 = 0;
    static int32_t D2Prev = 0;
    static int32_t D2Apply = 0 ;
    static int64_t dT = 0 ;
    static int32_t TEMP = 0 ;
    static int64_t OFF, SENS;
    static int32_t rawAltitude = 0 ;

    static int32_t altitude = 0 ;
    static int32_t altitudeLowPass = 0 ;
    static int32_t altitudeHighPass = 0 ;
    static float expoSmooth ;
    static float delaySmooth ; 
    
    static unsigned long extended2Micros ;
    static unsigned long pressureMicros ; // save time when program send command asking the MS5611 to get the pressure
    static unsigned long pressureMicrosPrev1 ; // save the previous pressureMicros
    static unsigned long pressureMicrosPrev2 ; // save the previous of the previous pressureMicros
    static unsigned long altMillis ;
    static unsigned long nextAltMillis  =  3200 ;  // save when Altitude has to be calculated; altitude is available only after 3200 in order to get a stable value (less temperature drift)
    static unsigned long nextAverageAltMillis = 3200 ; // save when AverageAltitude has to be calculated
    
 //   static long lastResultPressure = 0 ; // used to replace a new pressure by an oldone if collected pressure is wrong (at some time we got wring values)
 //   static long lastResultTemp = 0 ; // idem for temperature
    
    static float climbRate2AltFloat = 0 ;
    static float climbRateFloat = 0 ;
 
 
  if (varioData.SensorState==1) { // ========================= 
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros  > varioData.lastCommand2Micros + 4500){ // wait 9 msec at least before asking for reading the pressure
        long result = 0;
//#ifdef POLLINGI2C               
	I2c.read( _addr, 0, 3 ) ; //read 3 bytes from the device
	result = I2c.receive() ;
 	result <<= 8 ;
 	result |= I2c.receive() ;
 	result <<= 8 ;
 	result |= I2c.receive() ;
//#else            // Mike this code could best be totally remove (if we use always I2C)  
//      SendCommand(0x00);
//      //Wire.beginTransmission(_addr);
//      Wire.requestFrom(_addr,(uint8_t) 3);
//      if(Wire.available()!= 3) {
//#ifdef DEBUG
//          printer->println("Error: pressure not available");
//#endif
//          result = lastResultPressure ;
//      }
//      else {
//          result = 0;
//          for (int i = 0; i <= 2; i++) result = (result<<8) | Wire.read();   
//          lastResultPressure = result ;
//      }
//#endif // POLLINGI2C      
      D1=result;
//#ifdef POLLINGI2C
      I2c.write( _addr,0x58) ;
//#else           // Mike this code could best be totally remove (if we use always I2C)  
//      SendCommand(0x58); // request Temperature with OSR =4096  (for low precision, command = 0x50 , for 1024 we need command 0x54 , for 4096 we need command 0x58 and have to wait 9 msec)
//#endif
      varioData.lastCommand2Micros = (micros() >>1 ); 
      varioData.SensorState = 2;
    } // end of delay of 9 ms  
  } // end of SensorState == 1 
  else if (varioData.SensorState==2){ // =========================  
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros > varioData.lastCommand2Micros + 4500) { // wait 9000 usec to get Temp with high precision
//#ifdef POLLINGI2C
          I2c.read( _addr, 0, 3 ) ; //read 3 bytes from the device
          result = I2c.receive() ;
          result <<= 8 ;
          result |= I2c.receive() ;
          result <<= 8 ;
          result |= I2c.receive() ;
//#else                 // Mike this code could best be totally remove (if we use always I2C)  
//          SendCommand(0x00);
//          //Wire.beginTransmission(_addr);  
//          Wire.requestFrom(_addr,(uint8_t) 3);
//          if(Wire.available()!= 3) {  
//#ifdef DEBUG
//              printer->println("Error: temperature not available");
//#endif
//              result = lastResultTemp ;
//          }
//          else {
//            result = 0;  
//            for (int i = 0; i <= 2; i++) result = (result<<8) | Wire.read();   
//            lastResultTemp = result ;
//          }
// #endif //POLLINGI2C
          D2=result;
          varioData.SensorState=0; // 
    }  // End of process if temperature can be read 
  
  } // End of process if SensorState was 4
  if (varioData.SensorState==0) {    // ========================== new Pressure and (new or old) Temp are known so Request Pressure immediately and calculate altitude
//#ifdef POLLINGI2C
    I2c.write( _addr,0x48) ;
//#else                  // Mike this code could best be totally remove (if we use always I2C)  
//    SendCommand(0x48);
//#endif

    pressureMicrosPrev1 = pressureMicros ;
    pressureMicros = micros(); // pressureMicros is the timestamp to calculate climbrate between 2 pressures
    varioData.lastCommand2Micros = pressureMicros >> 1 ;
    varioData.SensorState=1;
    if ((D1 > 0) & (millis() > 1000) ) { // If D1 has been read in a previous loop and if sensor is started since more than 1 sec, then calculate pressure etc...
      if (D2Prev == 0) D2Prev = D2 ;
      D2Apply = (D2 + D2Prev ) / 2 ;
      D2Prev = D2 ; 
      dT = D2Apply - ((long)_calibrationData[5] << 8);
      TEMP = (2000 + (((int64_t)dT * (int64_t)_calibrationData[6]) >> 23)) / (float)100;
//      varioData.temperature=(int)(TEMP*10);
      OFF  = (((int64_t)_calibrationData[2]) << 16) + ((_calibrationData[4] * dT) >> 7);
      SENS = (((int64_t)_calibrationData[1]) << 15) + ((_calibrationData[3] * dT) >> 8);
      rawPressure= (((((((int64_t) D1) * (int64_t) SENS) >> 21) - OFF) * 10000 ) >> 15) ; // 1013.25 mb gives 1013250000 is a factor to keep higher precision (=1/100 cm).
      
      // altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));
      // other alternative (faster) = 1013.25 = 0 m , 954.61 = 500m , etc...
      //      Pressure	Alt (m)	Ratio
      //      101325	0	0.08526603
      //      95461	500	0.089525515
      //      89876	1000	0.094732853
      //      84598	1500	0.098039216
      //      79498	2000	0.103906899
      //      74686	2500	0.109313511
      //      70112	3000	0.115101289
      //      65768	3500	0.121270919
      //      61645	4000	0.127811861
      //      57733	4500	0.134843581
      //      54025	5000	

      if ( rawPressure > 954610000) {
        rawAltitude = ( 1013250000 - rawPressure ) * 0.08526603 ; // = 500 / (101325 - 95461)  // returned value 1234567 means 123,4567 m (temp is fixed to 15 degree celcius)
      } else if ( rawPressure > 898760000) {
        rawAltitude = 5000000 + ( 954610000 - rawPressure ) * 0.089525515  ; 
      } else if ( rawPressure > 845980000) {
        rawAltitude = 10000000 + ( 898760000 - rawPressure ) * 0.094732853  ; 
      } else if ( rawPressure > 794980000) {
        rawAltitude = 15000000 + ( 845980000 - rawPressure ) *  0.098039216 ; 
      } else if ( rawPressure > 746860000) {
        rawAltitude = 20000000 + ( 794980000 - rawPressure ) *  0.103906899 ; 
      } else if ( rawPressure > 701120000) {
        rawAltitude = 25000000 + ( 746860000 - rawPressure ) *  0.109313511 ; 
      } else if ( rawPressure > 657680000) {
        rawAltitude = 30000000 + ( 701120000 - rawPressure ) *  0.115101289 ; 
      } else if ( rawPressure > 616450000) {
        rawAltitude = 35000000 + ( 657680000 - rawPressure ) *  0.121270919 ; 
      } else if ( rawPressure > 577330000) {
        rawAltitude = 40000000 + ( 616450000 - rawPressure ) *  0.127811861 ;
      } else {    rawAltitude = 45000000 + ( 577330000 - rawPressure ) *  0.134843581 ;
      }  
      if (altitude == 0) {
        altitudeLowPass = altitudeHighPass = altitude = rawAltitude ;
        pressureMicrosPrev2 = pressureMicrosPrev1 - 20000 ; 
        delaySmooth = 20000 ; // = 20msec = 20000 usec
        climbRateFloat = 0 ;
      }
 //     if( (rawAltitude < altitude + 10000) && (rawAltitude > altitude - 10000) ) { 
      altitudeLowPass += 0.085 * ( rawAltitude - altitudeLowPass) ;
      altitudeHighPass += 0.1 * ( rawAltitude - altitudeHighPass) ;
      altitude += 0.04 * (rawAltitude - altitude) ;
      if (varioData.sensitivityPpm == 0 ) { // sensivityPpm == 0 when a ppm signal has never been received; no PPM = automatic change of smoothing based on vertical speed
        expoSmooth = SENSITIVITY_MIN  + ( fabs(climbRateFloat) - VSPEED_MIN) / (VSPEED_MAX - VSPEED_MIN) ;
        if (expoSmooth < SENSITIVITY_MIN) {    
            expoSmooth = SENSITIVITY_MIN ;
        }
        else if(expoSmooth > SENSITIVITY_MAX) {
            expoSmooth = SENSITIVITY_MAX ;
        }
      } else { //when PPM is used, smoothing parameter is fixed to PPM value.
        expoSmooth = ((float) varioData.sensitivityPpm)  ;
      }
      varioData.sensitivity = expoSmooth  ;
      if (pressureMicrosPrev1 > pressureMicrosPrev2 ) delaySmooth += 0.1 * ( pressureMicrosPrev1 -  pressureMicrosPrev2  - delaySmooth ) ; //delay between 2 measures  only if there is no overflow of pressureMicos
      climbRate2AltFloat = ((altitudeHighPass - altitudeLowPass )  * 5666.685 ) / delaySmooth; 
      //climbRate2AltFloat =  ((float) ( ((float) altitudeExp - altitudeExpPrev ) * 10000.0 )   / (float)(  pressureMicrosPrev1 -  pressureMicrosPrev2 )) ;  // dcm / micro
      climbRateFloat += expoSmooth * (climbRate2AltFloat - climbRateFloat) / 1000 ;
      
      if ( abs((int32_t)  climbRateFloat - varioData.climbRate) > VARIOHYSTERESIS ) {
          varioData.climbRate = (int32_t)  climbRateFloat  ;
          if (varioData.maxClimbRate<varioData.climbRate)varioData.maxClimbRate=varioData.climbRate;
          if (varioData.minClimbRate>varioData.climbRate)varioData.minClimbRate=varioData.climbRate;
      }    
      varioData.climbRateAvailable=true;  
      
      // AltitudeAvailable is set to true only once every 100 msec in order to give priority to climb rate on SPORT
      altMillis = millis() ;
      if (altMillis > nextAltMillis){
        nextAltMillis = altMillis + 100 ;
        varioData.absoluteAlt = altitude / 100; // altitudeExp in m *10000 and AbsoluteAlt must be in m * 100
        varioData.absoluteAltAvailable=true;  // Altitude is considered as available only after several loop in order to reduce number of transmission on Sport.
        varioData.sensitivityAvailable = true ;
        if (varioData.altOffset == 0) varioData.altOffset = varioData.absoluteAlt ;
        varioData.relativeAlt=varioData.absoluteAlt- varioData.altOffset;
        if(varioData.maxAbsAlt<varioData.absoluteAlt)varioData.maxAbsAlt=varioData.absoluteAlt;
        if(varioData.minAbsAlt>varioData.absoluteAlt)varioData.minAbsAlt=varioData.absoluteAlt;
        if(varioData.maxRelAlt<varioData.relativeAlt)varioData.maxRelAlt=varioData.relativeAlt;
        if(varioData.minRelAlt>varioData.relativeAlt)varioData.minRelAlt=varioData.relativeAlt;
        varioData.available=true;
        if ( altMillis > nextAverageAltMillis ){ // calculation of the difference of altitude (in m) between the 10 last sec
            nextAverageAltMillis = altMillis + 500 ;
            varioData.vSpeed10Sec = (varioData.absoluteAlt - varioData.prevAlt[varioData.idxPrevAlt]) /100 ;
            varioData.prevAlt[varioData.idxPrevAlt] = varioData.absoluteAlt ;
            varioData.idxPrevAlt++ ;
            if ( varioData.idxPrevAlt >= 20 ) varioData.idxPrevAlt = 0 ;
            if ( altMillis > 15000) {  // make the data avalaible only after 15 sec)
                varioData.vSpeed10SecAvailable = true ;
            }  
        }  
      } // end If (altMillis > nextAltMillis)
#ifdef DEBUGDATA
      static bool firstPrintAlt = true ;
      if (firstPrintAlt == true) {
          firstPrintAlt = false ;
          printer->println( "T,Ra,Sm,A,NC,DS,AHP,ALP,CR2" ) ;
      }    
            printer->print(  pressureMicrosPrev1 ) ;
            printer->print(",");
            printer->print(  (float) rawAltitude  ) ; // alt is displayed in CM with 2 decimal
            printer->print(",");
            printer->print(  expoSmooth ) ;
            printer->print(" ,");
            printer->print( (float) altitude  ) ;
            printer->print(" ,");
            printer->print( varioData.climbRate ) ;
            printer->print(" ,"); 
            printer->print( delaySmooth ) ;
            printer->print(" ,"); 
            printer->print( altitudeHighPass ) ;
            printer->print(" ,"); 
            printer->print( altitudeLowPass ) ;
            printer->print(" ,"); 
            printer->print( climbRate2AltFloat ) ;

            printer->println( ) ;
            
#endif        

//      altitudeExpPrev = altitudeExp ;
      pressureMicrosPrev2 = pressureMicrosPrev1 ;
      }  // end of D1 > 0 & time > 3 sec
  } // End of process if SensorState was 0    
} // End of readSensor



void OXS_MS5611::resetValues(){
  varioData.altOffset=varioData.absoluteAlt;
  varioData.maxRelAlt=0;
  varioData.minRelAlt=0;
  varioData.maxAbsAlt=varioData.absoluteAlt;
  varioData.minAbsAlt=varioData.absoluteAlt;
  varioData.minClimbRate=0 ;   // it is better to reset to 0
  varioData.maxClimbRate=0 ;   // it is better to reset to 0
}




