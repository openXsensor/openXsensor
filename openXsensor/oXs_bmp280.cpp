#include "oXs_bmp280.h"

#ifdef DEBUG
//#define DEBUGI2CMS5611
//#define DEBUGDATA
//#define DEBUGVARIOI2C
#endif

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

static BMP280_CALIB_DATA _bmp280_coeffs;   // Last read calibration data will be available here
//static uint8_t           _bmp085Mode;


#ifdef DEBUG  
OXS_BMP280::OXS_BMP280( HardwareSerial &print)
#else
OXS_BMP280::OXS_BMP280(void)
#endif
{
  // constructor
  //_addr=addr;
#define I2C_BMP280_ADR 0x77 ; 
  _addr = I2C_BMP280_ADR ;
  varioData.SensorState = 0 ;
#ifdef DEBUG  
  printer = &print; //operate on the address of print
  printer->begin(115200);
  printer->print("Vario Sensor:BMP280 I2C Addr=");
  printer->println(_addr,HEX);
#endif
}


// **************** Setup the BMP280 sensor *********************
void OXS_BMP280::setup() {
  unsigned int _calibrationData[13]; // The factory calibration data of the BMP280
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

//#ifdef ALT_TEMP_COMPENSATION
//  alt_temp_compensation = ALT_TEMP_COMPENSATION ;
//#endif

  
#ifdef DEBUG
  printer->print(F("Vario Sensor:BMP280 "));
  printer->println(" ");
  printer->print(F(" milli="));  
  printer->println(millis());

#endif
  
  I2c.begin() ;
  I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop
#ifdef DEBUGI2CBMP280
  I2c.scan() ;
  printer->print(F("last I2C scan adr: "));
  printer->println( I2c.scanAdr , HEX  );
#endif  

// write in register 0xF4 value 0x33 (it means oversampling temperature 1 X , oversampling pressure 8 X and normal mode = continue )
    errorI2C = I2c.write( _addr , (uint8_t) 0xF4 , (uint8_t) 0x33 ) ;
// write in register 0xF5 value 0x00 (it means 0.5msec between sampling, no filter, I2C protocol )
    errorI2C = I2c.write( _addr , (uint8_t) 0xF5 , (uint8_t) 0x00 ) ;

    errorCalibration = false ;
    for (byte i = 1; i <=12; i++) {
       errorI2C =  I2c.read( _addr , 0x86 + i*2, 2 ) ; //read 2 bytes from the device after sending the register to be read (first register = 0x86 (=register AC1)
       if ( errorI2C > 0 ) {
#ifdef DEBUG
            printer->print(F("error code in setup I2CRead: "));
            printer->println( errorI2C );
#endif
            errorCalibration = true ;
        } else {
            low = I2c.receive() ;
            high = I2c.receive() ;
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

    _bmp280_coeffs.dig_T1 = _calibrationData[1];
    _bmp280_coeffs.dig_T2 = _calibrationData[2];
    _bmp280_coeffs.dig_T3 = _calibrationData[3];
    _bmp280_coeffs.dig_P1 = _calibrationData[4];
    _bmp280_coeffs.dig_P2 = _calibrationData[5];
    _bmp280_coeffs.dig_P3 = _calibrationData[6];
    _bmp280_coeffs.dig_P4  = _calibrationData[7];
    _bmp280_coeffs.dig_P5  = _calibrationData[8];
    _bmp280_coeffs.dig_P6  = _calibrationData[9];
    _bmp280_coeffs.dig_P7  = _calibrationData[10];
    _bmp280_coeffs.dig_P8  = _calibrationData[11];
    _bmp280_coeffs.dig_P9  = _calibrationData[12];
    //_bmp280Mode        = 1; // perform an average of 2 pressure reads


    

#ifdef DEBUG  
  printer->println(F("setup vario done."));  
#endif
  
}  //end of setup


//********************************************************************************************
//***                            read the sensor                                           ***
//********************************************************************************************
bool OXS_BMP280::readSensor() {
    static uint32_t lastBMP280ReadMicro ;
    bool newVSpeedCalculated  = false ; 
    if ( micros() > (  lastBMP280ReadMicro + 20000) ) { 
          int32_t adc_T = 0 ;
          int32_t adc_P = 0 ;
          int32_t t_fine; // t_fine carries fine temperature as global value
          int32_t var1 ; 
          int32_t var2 ;
          int32_t T; // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
          uint32_t p; // pressure in pascal
          
          errorI2C = I2c.read( _addr , (uint8_t) 0xF7 , (uint8_t) 6) ; // read 6 bytes starting from register F7
          adc_P = I2c.receive() ;
          adc_P <<= 8 ;
          adc_P |= I2c.receive() ;
          adc_P <<= 8 ;
          adc_P |= I2c.receive() ;
          adc_P = adc_P >> 4 ;  
          adc_T = I2c.receive() ;
          adc_T <<= 8 ;
          adc_T |= I2c.receive() ;
          adc_T <<= 8 ;
          adc_T |= I2c.receive()  ;
          adc_T = adc_T >> 4 ;
          lastBMP280ReadMicro = micros() ;
          
          var1 = ((((adc_T>>3) - ((int32_t)_bmp280_coeffs.dig_T1<<1))) * ((int32_t)_bmp280_coeffs.dig_T2)) >> 11;
          var2 = (((((adc_T>>4) - ((int32_t)_bmp280_coeffs.dig_T1)) * ((adc_T>>4) - ((int32_t)_bmp280_coeffs.dig_T1))) >> 12) * ((int32_t)_bmp280_coeffs.dig_T3)) >> 14;
          t_fine = var1 + var2;
          varioData.temperature = (t_fine * 5 + 128) >> 8; 
          
          var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
          var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)_bmp280_coeffs.dig_P6);
          var2 = var2 + ((var1*((int32_t)_bmp280_coeffs.dig_P5))<<1);
          var2 = (var2>>2)+(((int32_t)_bmp280_coeffs.dig_P4)<<16);
          var1 = (((_bmp280_coeffs.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)_bmp280_coeffs.dig_P2) * var1)>>1))>>18;
          var1 =((((32768+var1))*((int32_t)_bmp280_coeffs.dig_P1))>>15);
          if (var1 == 0) { 
            varioData.rawPressure = 0 ; 
          } else {
            p = (((uint32_t)(((int32_t)1048576) - adc_P)-(var2>>12)))*3125;
            if (p < 0x80000000)   {
              p = (p << 1) / ((uint32_t)var1);
            }   else   {
              p = (p / (uint32_t)var1) * 2;
            }
            var1 = (((int32_t)_bmp280_coeffs.dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
            var2 = (((int32_t)(p>>2)) * ((int32_t)_bmp280_coeffs.dig_P8))>>13;
            varioData.rawPressure = (uint32_t)((int32_t)p + ((var1 + var2 + _bmp280_coeffs.dig_P7) >> 4))*  10000 ;  
          }  
          if ( (errorI2C == 0 ) & (millis() > 1000) ) { // If no I2c error  and if sensor is started since more than 1 sec, then calculate pressure etc...
              calculateVario() ;
              newVSpeedCalculated = true ;
          }
  } //   end check on 20 msec     
  return newVSpeedCalculated ;                             // return true if a new Vspeed is available  
}  // end read sensor     



void OXS_BMP280::calculateVario() {       
      
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






