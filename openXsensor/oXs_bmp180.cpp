#include "oXs_bmp180.h"

#if defined(SENSOR_IS_BMP180) 

#ifdef DEBUG
//#define DEBUGI2CMS5611
//#define DEBUGDATA
//#define DEBUGVARIOI2C
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

static bmp085_calib_data _bmp085_coeffs;   // Last read accelerometer data will be available here
static uint8_t           _bmp085Mode;


//long result ;


#ifdef DEBUG  
OXS_BMP180::OXS_BMP180( HardwareSerial &print)
#else
OXS_BMP180::OXS_BMP180(void)
#endif
{
  // constructor
//  _addr=addr;
  varioData.SensorState = 0 ;
#ifdef DEBUG  
  printer = &print; //operate on the address of print
//  printer->begin(115200);
//  printer->print("Vario Sensor:MS5611 I2C Addr=");
//  printer->println(addr,HEX);
#endif
}


// **************** Setup the BMP180 sensor *********************
void OXS_BMP180::setup() {
  unsigned int _calibrationData[12]; // The factory calibration data of the BMP180
  varioData.absoluteAlt.available = false ;
  varioData.relativeAlt.available = false ; 
  varioData.climbRate.available = false ;
  varioData.sensitivity.available = false ;
//  varioData.vSpeed10SecAvailable = false ;
  sensitivityMin = SENSITIVITY_MIN ; // set the min smoothing to the default value
  varioData.delaySmooth = 20000 ; // delay between 2 altitude calculation = 20msec = 20000 usec
  nextAltMillis  =  5000 ;  // in msec; save when Altitude has to be calculated; altitude is available only after some delay in order to get a stable value (less temperature drift)
//  nextAverageAltMillis =  nextAltMillis ;  // in msec ; save when AverageAltitude has to be calculated
//  nextAverageAltMillis =  nextAltMillis ; 

#ifdef ALT_TEMP_COMPENSATION
  alt_temp_compensation = ALT_TEMP_COMPENSATION ;
#endif

  
#ifdef DEBUG
  printer->print(F("Vario Sensor:BMP180 "));
  printer->println(" ");
  printer->print(F(" milli="));  
  printer->println(millis());

#endif
  
  I2c.begin() ;
  I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop
#ifdef DEBUGI2CBMP180
  I2c.scan() ;
  printer->print(F("last I2C scan adr: "));
  printer->println( I2c.scanAdr , HEX  );
#endif  
    errorCalibration = false ;
    for (byte i = 1; i <=11; i++) {
       errorI2C =  I2c.read( BMP180_ADR, 0xA8 + i*2, 2 ) ; //read 2 bytes from the device after sending the register to be read (first register = 0xAA (=register AC1)
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

    _bmp085_coeffs.ac1 = _calibrationData[1];
    _bmp085_coeffs.ac2 = _calibrationData[2];
    _bmp085_coeffs.ac3 = _calibrationData[3];
    _bmp085_coeffs.ac4 = _calibrationData[4];
    _bmp085_coeffs.ac5 = _calibrationData[5];
    _bmp085_coeffs.ac6 = _calibrationData[6];
    _bmp085_coeffs.b1  = _calibrationData[7];
    _bmp085_coeffs.b2  = _calibrationData[8];
    _bmp085_coeffs.mb  = _calibrationData[9];
    _bmp085_coeffs.mc  = _calibrationData[10];
    _bmp085_coeffs.md  = _calibrationData[11];
    _bmp085Mode        = 1; // perform an average of 2 pressure reads



#ifdef DEBUG  
  printer->println(F("setup vario done."));  
#endif
  
}  //end of setup


//********************************************************************************************
//***                            read the sensor                                           ***
//********************************************************************************************
bool OXS_BMP180::readSensor() {
long result = 0 ;
#ifdef  DEBUGVARIOI2C
    printer->print(F("sensorState= "));
    printer->println(varioData.SensorState);
#endif
  bool newVSpeedCalculated  = false ; 
  if (varioData.SensorState==0) { // ========================= Read the pressure
    if (  micros()   >   varioData.lastCommandMicros + 9000){ // wait 9 msec at least before asking for reading the pressure
//        long result = 0;
	      if(  ! I2c.read( BMP180_ADR, 0xF6, 3 )) { ; //read 3 bytes from the device starting from register F6; keep previous value in case of error 
        	result = I2c.receive() ;
         	result <<= 8 ;
         	result |= I2c.receive() ;
         	result <<= 8 ;
         	result |= I2c.receive() ;
                D1=result >> 7; // divide by 2* (8- the parameter for number of averages)
        } else {
              D1 = 0 ; // D1 value are not processed to calculate Alt.
        }      
        I2c.write( BMP180_ADR ,0xF4 , 0x2E ) ; // ask a conversion of Temperature sending 2E in register F4
        varioData.lastCommandMicros = micros(); 
        varioData.SensorState = 1;
    } // end of delay of 9 ms  
  } // end of SensorState == 1 
  else if (varioData.SensorState==1){ // =========================  
    if ( ( micros() - varioData.lastCommandMicros ) > 9000 ) { // wait 9000 usec to get Temp with high precision
          if ( ! I2c.read( BMP180_ADR , 0xF6, 2 )) { ; //read 2 bytes from the device in register F6 ; keep previous value in case of error
                result = I2c.receive() ;
                result <<= 8 ;
                result |= I2c.receive() ;
                D2=result;
          }      
          I2c.write( BMP180_ADR , 0xF4 , 0x74) ;// ask a conversion of Pressure sending 74 in register F4; 74 means an average of 2 reads and so a normal wait time of 7.5 msec
          varioData.lastCommandMicros = micros() ; 
          varioData.SensorState=2; // 
    }  // End of process if temperature can be read 
  } // End of process if SensorState was 1  
  else if (varioData.SensorState==2) {    // ========================== new Pressure and (new or old) Temp are known so Request Pressure immediately and calculate altitude
    varioData.SensorState=0;
    if ((D1 > 0) & (millis() > 1000) ) { // If D1 has been read in a previous loop and if sensor is started since more than 1 sec, then calculate pressure etc...
              calculateVario() ;
              newVSpeedCalculated = true ;
    }
  } //   end ( varioData.SensorState == 2 )     
  return newVSpeedCalculated ;                             // return true if a new Vspeed is available  
}  // end read sensor     


void OXS_BMP180::calculateVario() {       
      if (D2Prev == 0) D2Prev = D2 ;
      D2Apply = (D2 + D2Prev ) / 2 ;
      D2Prev = D2 ; 
#if BMP085_USE_DATASHEET_VALS
    _bmp085_coeffs.ac1 = 408;
    _bmp085_coeffs.ac2 = -72;
    _bmp085_coeffs.ac3 = -14383;
    _bmp085_coeffs.ac4 = 32741;
    _bmp085_coeffs.ac5 = 32757;
    _bmp085_coeffs.ac6 = 23153;
    _bmp085_coeffs.b1  = 6190;
    _bmp085_coeffs.b2  = 4;
    _bmp085_coeffs.mb  = -32768;
    _bmp085_coeffs.mc  = -8711;
    _bmp085_coeffs.md  = 2868;
    _bmp085Mode        = 0;
    D2Apply = 27898 ;
    D1 = 23843 ;
#endif      
      int32_t X1 = (D2Apply - (int32_t)_bmp085_coeffs.ac6) * ((int32_t)_bmp085_coeffs.ac5) >> 15;
      int32_t X2 = ((int32_t)_bmp085_coeffs.mc << 11) / (X1+(int32_t)_bmp085_coeffs.md);
      int32_t b5 = X1 + X2 ;
      varioData.temperature = ( b5 + 8 ) >> 4 ; // = Temperature

      // calcul of pressure.
      int32_t p ;
      int32_t b6 = b5 - 4000;
      int32_t x1 = (_bmp085_coeffs.b2 * ((b6 * b6) >> 12)) >> 11;
      int32_t x2 = (_bmp085_coeffs.ac2 * b6) >> 11;
      int32_t x3 = x1 + x2;
      int32_t b3 = (((((int32_t) _bmp085_coeffs.ac1) * 4 + x3) << _bmp085Mode) + 2) >> 2;
      x1 = (_bmp085_coeffs.ac3 * b6) >> 13;
      x2 = (_bmp085_coeffs.b1 * ((b6 * b6) >> 12)) >> 16;
      x3 = ((x1 + x2) + 2) >> 2;
      uint32_t b4 = (_bmp085_coeffs.ac4 * (uint32_t) (x3 + 32768)) >> 15;
      uint32_t b7 = ((uint32_t) (D1 - b3) * (50000 >> _bmp085Mode));
      if (b7 < 0x80000000) 
      {
        p = (b7 << 1) / b4;
      }
      else
      {
        p = (b7 / b4) << 1;
      }
      x1 = (p >> 8) * (p >> 8);
      x1 = (x1 * 3038) >> 16;
      x2 = (-7357 * p) >> 16;
      varioData.rawPressure = (p + ((x1 + x2 + 3791) >> 4)) * 10000;

//      varioData.temperature= TEMP;
//      OFF  = (((int64_t)_calibrationData[2]) << 16) + ( (_calibrationData[4] * dT) >> 7);
//      OFF  = (((int64_t)_calibrationData[2]) << 16) + ( ( (_calibrationData[4] - alt_temp_compensation ) * dT) >> 7);
//      SENS = (((int64_t)_calibrationData[1]) << 15) + ((_calibrationData[3] * dT) >> 8);
//      varioData.rawPressure= (((((((int64_t) D1) * (int64_t) SENS) >> 21) - OFF) * 10000 ) >> 15) ; // 1013.25 mb gives 1013250000 is a factor to keep higher precision (=1/100 cm).
      
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
//        pressureMicrosPrev2 = pressureMicrosPrev1 - 20000 ; 
      }
      altitude += 0.04 * (varioData.rawAltitude - altitude) ;
//      varioData.altitudeAt20MsecAvailable = true ; // inform openxsens.ino that calculation of dTE can be performed

      altitudeLowPass += 0.085 * ( varioData.rawAltitude - altitudeLowPass) ;
      altitudeHighPass += 0.1 * ( varioData.rawAltitude - altitudeHighPass) ;
//      if (pressureMicrosPrev1 > pressureMicrosPrev2 ) varioData.delaySmooth += 0.1 * ( pressureMicrosPrev1 -  pressureMicrosPrev2  - varioData.delaySmooth ) ; //delay between 2 measures  only if there is no overflow of pressureMicos
      climbRate2AltFloat = ((altitudeHighPass - altitudeLowPass )  * 5666.685 ) / 20000; 

      abs_deltaClimbRate =  abs(climbRate2AltFloat - varioData.climbRateFloat) ;
      if ( varioData.sensitivityPpm  > 0) sensitivityMin =   varioData.sensitivityPpm ; 
      if ( (abs_deltaClimbRate <= SENSITIVITY_MIN_AT) || (sensitivityMin >= SENSITIVITY_MAX) ) {
         varioData.sensitivity.value = sensitivityMin ;  
      } else if (abs_deltaClimbRate >= SENSITIVITY_MAX_AT)  {
         varioData.sensitivity.value = SENSITIVITY_MAX ; 
      } else {
         varioData.sensitivity.value = sensitivityMin + ( SENSITIVITY_MAX - sensitivityMin ) * (abs_deltaClimbRate - SENSITIVITY_MIN_AT) / (SENSITIVITY_MAX_AT - SENSITIVITY_MIN_AT) ;
      }
      varioData.climbRateFloat += varioData.sensitivity.value * (climbRate2AltFloat - varioData.climbRateFloat)  * 0.001 ; // sensitivity is an integer and must be divided by 1000
      
      if ( abs((int32_t)  varioData.climbRateFloat - varioData.climbRate.value) > VARIOHYSTERESIS ) {
          varioData.climbRate.value = (int32_t)  varioData.climbRateFloat  ;
      }    
      varioData.climbRate.available=true; // allows SPORT protocol to transmit the value
//      varioData.switchClimbRateAvailable = true ; // inform readsensors() that a switchable vspeed is available
//      varioData.averageClimbRateAvailable = true ; // inform readsensors() that a vspeed is available to calculate the average
      // AltitudeAvailable is set to true only once every 100 msec in order to give priority to climb rate on SPORT
      altMillis = millis() ;
      if (altMillis > nextAltMillis){
        nextAltMillis = altMillis + 100 ;
        varioData.absoluteAlt.value = altitude / 100 ; // altitude is in m *10000 and AbsoluteAlt must be in m * 100
        varioData.absoluteAlt.available=true ;  // Altitude is considered as available only after several loop in order to reduce number of transmission on Sport.
        varioData.sensitivity.available = true ;
        if (varioData.altOffset == 0) varioData.altOffset = varioData.absoluteAlt.value ;
        varioData.relativeAlt.value = varioData.absoluteAlt.value - varioData.altOffset ;
        varioData.relativeAlt.available = true ;
        if ( varioData.relativeAlt.value > varioData.relativeAltMax ) varioData.relativeAltMax = varioData.relativeAlt.value ;
        varioData.relativeAltMaxAvailable = true ;
//        if ( altMillis > nextAverageAltMillis ){ // calculation of the difference of altitude (in m) between the 10 last sec
//            nextAverageAltMillis = altMillis + 500 ; // calculate only once every 500 msec
//            varioData.vSpeed10Sec = (varioData.absoluteAlt.value - varioData.prevAlt[varioData.idxPrevAlt]) /100 ;
//            varioData.prevAlt[varioData.idxPrevAlt] = varioData.absoluteAlt.value ;
//            varioData.idxPrevAlt++ ;
//            if ( varioData.idxPrevAlt >= 20 ) varioData.idxPrevAlt = 0 ;
//            if ( altMillis > 15000) {  // make the data avalaible only after 15 sec)
//                varioData.vSpeed10SecAvailable = true ;
//            }  
//        }  

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
 //           printer->print( smoothAltitude ) ;            printer->print(" ,"); 
 //           printer->print( rawRateVSpeed ) ;            printer->print(" ,"); 
 //           printer->print( smoothRateVSpeed ) ;            printer->print(" ,"); 
 //           printer->print( expoSmooth5611_alt_auto * 1000 ) ;            printer->print(" ,"); 
 //           printer->print( expoSmooth5611_vSpeed_auto * 1000 ) ;            printer->print(" ,"); 
            printer->println( ) ;
            
#endif        

      pressureMicrosPrev2 = pressureMicrosPrev1 ;
} // End of calculate Vario

#endif // end of #if defined(SENSOR_IS_BMP180) 




