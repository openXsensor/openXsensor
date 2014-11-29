#include "oXs_ms5611.h"

#ifdef DEBUG
//#define DEBUGI2CMS5611
//#define DEBUGDATA
//#define DEBUGVARIOI2C
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

long result ;


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
//  printer->begin(115200);
//  printer->print("Vario Sensor:MS5611 I2C Addr=");
//  printer->println(addr,HEX);
#endif
}


// **************** Setup the MS5611 sensor *********************
void OXS_MS5611::setup() {
  varioData.absoluteAltAvailable=false ;
  varioData.climbRateAvailable=false ;
  varioData.sensitivityAvailable=false ;
  varioData.vSpeed10SecAvailable=false ;
//  varioData.newClimbRateAvailable = false; 
//  varioData.sensitivityPpm = 0 ;
//  varioData.idxPrevAlt = 0 ;
//  D1 = 0;
//  D2 = 0;
//  D2Prev = 0;
//  D2Apply = 0 ;
//  dT = 0 ;
//  TEMP = 0 ;
//  rawAltitude = 0 ;

//  altitude = 0 ;
//  altitudeLowPass = 0 ;
//  altitudeHighPass = 0 ;
  sensitivityMin = SENSITIVITY_MIN ; // set the min smoothing to the default value
  varioData.delaySmooth = 20000 ; // delay between 2 altitude calculation = 20msec = 20000 usec
  nextAltMillis  =  5000 ;  // in msec; save when Altitude has to be calculated; altitude is available only after some delay in order to get a stable value (less temperature drift)
  nextAverageAltMillis =  nextAltMillis ;  // in msec ; save when AverageAltitude has to be calculated
  nextAverageAltMillis =  nextAltMillis ; 
 //   static long lastResultPressure = 0 ; // used to replace a new pressure by an oldone if collected pressure is wrong (at some time we got wring values)
 //   static long lastResultTemp = 0 ; // idem for temperature
    
//  climbRate2AltFloat = 0 ;
//  climbRateFloat = 0 ;
  
#ifdef DEBUG
  printer->print(F("Vario Sensor:MS5611 I2C Addr="));
  printer->println(_addr,HEX);
  printer->print(F(" milli="));  
  printer->println(millis());

#endif
  
  I2c.begin() ;
  I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop
#ifdef DEBUGI2CMS5611
  I2c.scan() ;
  printer->print(F("last I2C scan adr: "));
  printer->println( I2c.scanAdr , HEX  );
#endif  
  errorI2C = I2c.write( _addr,0x1e) ;
    errorCalibration = false ;
  if (errorI2C > 0 ) {
#ifdef DEBUG
    printer->print(F("error code in setup I2CWrite: "));
    printer->println( errorI2C );
#endif
    errorCalibration = true ;
  } else {
    delay(100);
    for (byte i = 1; i <=6; i++) {
       errorI2C =  I2c.read( _addr, 0xa0 + i*2, 2 ) ; //read 2 bytes from the device after sending the command A0 + xx depending on the register to be read
       if ( errorI2C > 0 ) {
#ifdef DEBUG
            printer->print(F("error code in setup I2CRead: "));
            printer->println( errorI2C );
#endif
            errorCalibration = true ;
        } else {
            high = I2c.receive() ;
            low = I2c.receive() ;
            _calibrationData[i] = high<<8 | low;
        }  
#ifdef DEBUG
        printer->print(F("calibration data #"));
        printer->print(i);
        printer->print(F(" = "));
        printer->print( _calibrationData[i] );
        printer->print(F(" error= "));
        printer->println( errorI2C );
#endif
    } // End for 
  } // End if else  

#ifdef DEBUG  
  printer->println(F("setup vario done."));
#endif
  
//  resetValues(); // not used anymore (min, max, ...)
}  //end of setup


//********************************************************************************************
//***                            read the sensor                                           ***
//********************************************************************************************
void OXS_MS5611::readSensor() {
#ifdef  DEBUGVARIOI2C
    printer->print(F("sensorState= "));
    printer->println(varioData.SensorState);
#endif
  if (varioData.SensorState==1) { // ========================= Read the pressure
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros  > varioData.lastCommand2Micros + 4500){ // wait 9 msec at least before asking for reading the pressure
        long result = 0;
	if(  ! I2c.read( _addr, 0, 3 )) { ; //read 3 bytes from the device after sending a command "00"; keep previous value in case of error 
        	result = I2c.receive() ;
         	result <<= 8 ;
         	result |= I2c.receive() ;
         	result <<= 8 ;
         	result |= I2c.receive() ;
                D1=result;
        } else {
              D1 = 0 ; // D1 value are not processed to calculate Alt.
        }      
        I2c.write( _addr,0x58) ; // ask a conversion of Temperature
        varioData.lastCommand2Micros = (micros() >>1 ); 
        varioData.SensorState = 2;
    } // end of delay of 9 ms  
  } // end of SensorState == 1 
  else if (varioData.SensorState==2){ // =========================  
    extended2Micros = micros() >> 1 ;
    if (extended2Micros < varioData.lastCommand2Micros) extended2Micros = extended2Micros | 0x80000000 ;
    if ( extended2Micros > varioData.lastCommand2Micros + 4500) { // wait 9000 usec to get Temp with high precision
          if ( ! I2c.read( _addr, 0, 3 )) { ; //read 3 bytes from the device; keep previous value in case of error
                result = I2c.receive() ;
                result <<= 8 ;
                result |= I2c.receive() ;
                result <<= 8 ;
                result |= I2c.receive() ;
                D2=result;
          }      
          varioData.SensorState=0; // 
    }  // End of process if temperature can be read 
  
  } // End of process if SensorState was 1 or 2 
  if (varioData.SensorState==0) {    // ========================== new Pressure and (new or old) Temp are known so Request Pressure immediately and calculate altitude
    I2c.write( _addr,0x48) ;// ask a conversion of Pressure
    pressureMicrosPrev1 = pressureMicros ;
    pressureMicros = micros(); // pressureMicros is the timestamp to calculate climbrate between 2 pressures
    varioData.lastCommand2Micros = pressureMicros >> 1 ;
    varioData.SensorState=1;
    if ((D1 > 0) & (millis() > 1000) ) { // If D1 has been read in a previous loop and if sensor is started since more than 1 sec, then calculate pressure etc...
      if (D2Prev == 0) D2Prev = D2 ;
      D2Apply = (D2 + D2Prev ) / 2 ;
      D2Prev = D2 ; 
      dT = D2Apply - ((long)_calibrationData[5] << 8);
      TEMP = (2000 + (((int64_t)dT * (int64_t)_calibrationData[6]) >> 23)) / (float) 1.0 ;
      varioData.temperature= TEMP;
      OFF  = (((int64_t)_calibrationData[2]) << 16) + ((_calibrationData[4] * dT) >> 7);
      SENS = (((int64_t)_calibrationData[1]) << 15) + ((_calibrationData[3] * dT) >> 8);
      varioData.rawPressure= (((((((int64_t) D1) * (int64_t) SENS) >> 21) - OFF) * 10000 ) >> 15) ; // 1013.25 mb gives 1013250000 is a factor to keep higher precision (=1/100 cm).
      
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

      if ( varioData.rawPressure > 954610000) {
        varioData.rawAltitude = ( 1013250000 - varioData.rawPressure ) * 0.08526603 ; // = 500 / (101325 - 95461)  // returned value 1234567 means 123,4567 m (temp is fixed to 15 degree celcius)
      } else if ( varioData.rawPressure > 898760000) {
        varioData.rawAltitude = 5000000 + ( 954610000 - varioData.rawPressure ) * 0.089525515  ; 
      } else if ( varioData.rawPressure > 845980000) {
        varioData.rawAltitude = 10000000 + ( 898760000 - varioData.rawPressure ) * 0.094732853  ; 
      } else if ( varioData.rawPressure > 794980000) {
        varioData.rawAltitude = 15000000 + ( 845980000 - varioData.rawPressure ) *  0.098039216 ; 
      } else if ( varioData.rawPressure > 746860000) {
        varioData.rawAltitude = 20000000 + ( 794980000 - varioData.rawPressure ) *  0.103906899 ; 
      } else if ( varioData.rawPressure > 701120000) {
        varioData.rawAltitude = 25000000 + ( 746860000 - varioData.rawPressure ) *  0.109313511 ; 
      } else if ( varioData.rawPressure > 657680000) {
        varioData.rawAltitude = 30000000 + ( 701120000 - varioData.rawPressure ) *  0.115101289 ; 
      } else if ( varioData.rawPressure > 616450000) {
        varioData.rawAltitude = 35000000 + ( 657680000 - varioData.rawPressure ) *  0.121270919 ; 
      } else if ( varioData.rawPressure > 577330000) {
        varioData.rawAltitude = 40000000 + ( 616450000 - varioData.rawPressure ) *  0.127811861 ;
      } else {    varioData.rawAltitude = 45000000 + ( 577330000 - varioData.rawPressure ) *  0.134843581 ;
      }
     
// here the classical way to calculate Vspeed with high and low pass filter      
      if (altitude == 0) {
        altitudeLowPass = altitudeHighPass = altitude = varioData.rawAltitude ;
        pressureMicrosPrev2 = pressureMicrosPrev1 - 20000 ; 
      }
      altitude += 0.04 * (varioData.rawAltitude - altitude) ;
      varioData.altitudeAt20MsecAvailable = true ; // inform openxsens.ino that calculation of dTE can be performed

      altitudeLowPass += 0.085 * ( varioData.rawAltitude - altitudeLowPass) ;
      altitudeHighPass += 0.1 * ( varioData.rawAltitude - altitudeHighPass) ;
      if (pressureMicrosPrev1 > pressureMicrosPrev2 ) varioData.delaySmooth += 0.1 * ( pressureMicrosPrev1 -  pressureMicrosPrev2  - varioData.delaySmooth ) ; //delay between 2 measures  only if there is no overflow of pressureMicos
      climbRate2AltFloat = ((altitudeHighPass - altitudeLowPass )  * 5666.685 ) / varioData.delaySmooth; 

      abs_deltaClimbRate =  abs(climbRate2AltFloat - climbRateFloat) ;
      if ( varioData.sensitivityPpm  > 0) sensitivityMin =   varioData.sensitivityPpm ; 
      if ( (abs_deltaClimbRate <= SENSITIVITY_MIN_AT) || (sensitivityMin >= SENSITIVITY_MAX) ) {
         varioData.sensitivity = sensitivityMin ;  
      } else if (abs_deltaClimbRate >= SENSITIVITY_MAX_AT)  {
         varioData.sensitivity = SENSITIVITY_MAX ; 
      } else {
         varioData.sensitivity = sensitivityMin + ( SENSITIVITY_MAX - sensitivityMin ) * (abs_deltaClimbRate - SENSITIVITY_MIN_AT) / (SENSITIVITY_MAX_AT - SENSITIVITY_MIN_AT) ;
      }
      climbRateFloat += varioData.sensitivity * (climbRate2AltFloat - climbRateFloat)  * 0.001 ; // sensitivity is an integer and must be divided by 1000
      
      if ( abs((int32_t)  climbRateFloat - varioData.climbRate) > VARIOHYSTERESIS ) {
          varioData.climbRate = (int32_t)  climbRateFloat  ;
      }    
      varioData.climbRateAvailable=true; // allows SPORT protocol to transmit the value
      varioData.switchClimbRateAvailable = true ; // inform readsensors() that a switchable vspeed is available
      
      // AltitudeAvailable is set to true only once every 100 msec in order to give priority to climb rate on SPORT
      altMillis = millis() ;
      if (altMillis > nextAltMillis){
        nextAltMillis = altMillis + 100 ;
        varioData.absoluteAlt = altitude / 100; // altitude is in m *10000 and AbsoluteAlt must be in m * 100
        varioData.absoluteAltAvailable=true;  // Altitude is considered as available only after several loop in order to reduce number of transmission on Sport.
        varioData.sensitivityAvailable = true ;
        if ( altMillis > nextAverageAltMillis ){ // calculation of the difference of altitude (in m) between the 10 last sec
            nextAverageAltMillis = altMillis + 500 ; // calculate only once every 500 msec
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
//          printer->println(F( "T,Ra,Sm,A,NC,DS,AHP,ALP,CR2, Temp" )) ;
          printer->println(F( "T,Ra,Alt,vpsd, Alt2, rawVspd, vspd2 , smoothAlt, smoothVspd" )) ;
      }    
            printer->print(  pressureMicrosPrev1 ) ; printer->print(",");
            printer->print(  (float) varioData.rawAltitude  ) ; printer->print(","); // alt is displayed in CM with 2 decimal
 //           printer->print(  expoSmooth ) ;             printer->print(" ,");
            printer->print( (float) altitude  ) ;             printer->print(" ,");
            printer->print( varioData.climbRate ) ;            printer->print(" ,"); 
 //           printer->print( delaySmooth ) ;            printer->print(" ,"); 
 //           printer->print( altitudeHighPass ) ;             printer->print(" ,"); 
 //           printer->print( altitudeLowPass ) ;            printer->print(" ,"); 
 //           printer->print( climbRate2AltFloat ) ;             printer->print(" ,"); 
 //           printer->print( varioData.temperature ) ;
            printer->print( smoothAltitude ) ;            printer->print(" ,"); 
            printer->print( rawRateVSpeed ) ;            printer->print(" ,"); 
            printer->print( smoothRateVSpeed ) ;            printer->print(" ,"); 
            printer->print( expoSmooth5611_alt_auto * 1000 ) ;            printer->print(" ,"); 
            printer->print( expoSmooth5611_vSpeed_auto * 1000 ) ;            printer->print(" ,"); 
            printer->println( ) ;
            
#endif        


      pressureMicrosPrev2 = pressureMicrosPrev1 ;
      }  // end of D1 > 0 & time > 3 sec
  } // End of process if SensorState was 0    
} // End of readSensor






