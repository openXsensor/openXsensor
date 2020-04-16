#include "oXs_sdp3x.h"

#ifdef DEBUG
    #define DEBUG_SDP3X_SCAN
    #define DEBUG_SDP3X_ERRORCODE
    #define DEBUG_SDP3X_RAWDATA
    //#define DEBUG_SDP3X_SIMULATION
#endif



extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#ifdef DEBUG  
OXS_SDP3X::OXS_SDP3X(uint8_t addr, HardwareSerial &print)
#else
OXS_SDP3X::OXS_SDP3X(uint8_t addr)
#endif
{  // constructor
  _addr=addr;
#ifdef DEBUG  
  printer = &print; 
#endif
}


// **************** Setup the 4525DO sensor *********************
void OXS_SDP3X::setup() {
  airSpeedData.airSpeed.available = false ;     
#ifdef DEBUG  
  printer->print(F("AirSpeed Sensor:SDP3X I2C Addr="));
  printer->println(_addr,HEX);
#endif
  I2c.begin() ;
  I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop 
#ifdef DEBUG_SDP3X_SCAN
  I2c.scan() ; // scan all I2C address
  printer->print(F("I2C scan adr: "));
  printer->println( I2c.scanAdr , HEX  );
#endif
#ifdef DEBUG_SDP3X_SIMULATION // use dummy setup in case of simulation
  nextPressureReadMillis = millis() + 2;    //
  nextAirSpeedMillis  = nextPressureReadMillis + 200 ; 
  airSpeedData.temperature4525 = 23.4  ; 
  temperatureKelvinSdp3x = 273.15 + airSpeedData.temperature4525 ; // in Kelvin
  dpScaleSdp3x =  966.0 / 1013.0 / 60 ; //60=SDP31 , 240=SDP32
#else // not a simulation
  // set the sensor in continous mode with averaging (send a command 0X3615)
  I2c.write(_addr, (uint8_t) 0X36 , (uint8_t) 0X15);
  delay(20); // wait 20msec in order to get the first data (datasheet says 8 msec)
  // read the sensor to get the initial temperature and the scaling factor
  I2CErrorCodeSdp3x = I2c.read( _addr,  9 ) ; //read 9 bytes from the device;
  nextPressureReadMillis = millis() + 2;    //
  nextAirSpeedMillis  = nextPressureReadMillis + 200 ; 
  if (I2CErrorCodeSdp3x == 0) {
      data[0] = I2c.receive() ; // discard diffPressure 
      data[1] = I2c.receive() ; // discard diffPressure
      //int16_t dp_raw   = (int16_t)data[0] << 8 | data[1];
      data[0] = I2c.receive() ; // discard this byte = CRC
      data[0] = I2c.receive() ;
      data[1] = I2c.receive() ;
      airSpeedData.temperature4525 = ((int16_t)data[0] << 8 | data[1]) / 200.0  ; // sdp3x use a scale factor of 200 for temp
      temperatureKelvinSdp3x = 273.15 + airSpeedData.temperature4525 ; // in Kelvin
      data[0] = I2c.receive() ; // discard this byte = CRC
      data[0] = I2c.receive() ;
      data[1] = I2c.receive() ;
      dpScaleSdp3x =  966.0 / 1013.0 / ((int16_t)data[0] << 8 | data[1]);
      // datasheet says that we have to apply a correction of 966/actual pressure in mbar; it is estimated with 1013
  }
#ifdef DEBUG  
    printer->print(F("Set up SDP3X done. I2C Error code= "));
    printer->println(I2CErrorCodeSdp3x);
    printer->print(F(" milli="));  
    printer->println(millis());
#endif
#endif // endif for simulation
}  //end of setup


/****************************************************************************/
/* readSensor - Read differential pressure + temperature from the 4525DO    */
/****************************************************************************/
void OXS_SDP3X::readSensor() {
    unsigned long airSpeedMillis = millis() ;
    if ( airSpeedMillis >= nextPressureReadMillis){  // do not read the sensor if there is less than 0.5 msec since previous read 
        nextPressureReadMillis = airSpeedMillis + 2 ;
#ifdef DEBUG_SDP3X_SIMULATION // use dummy read in case of simulation
        if (true){ // added to keep the same if level as when no simulation
            airSpeedData.difPressureAdc_zero =  0x7FFF * dpScaleSdp3x     ;  // diffPressure in pa
#else // not in simulation
        I2CErrorCodeSdp3x = I2c.read( _addr,  2 ) ; //read 2 bytes from the device;
#ifdef DEBUG_SDP3X_ERRORCODE  
        if ( I2CErrorCodeSdp3x) {
            printer->print(F("Read SDP3X done. I2C Error code= "));
            printer->println(I2CErrorCodeSdp3x);
        }
#endif
        if( I2CErrorCodeSdp3x == 0) { // when no read error, we calculate
      	    data[0] = I2c.receive() ;
       	    data[1] = I2c.receive() ;
            airSpeedData.difPressureAdc_zero =  ((int16_t) (data[0] << 8) + data[1] ) * dpScaleSdp3x     ;  // diffPressure in pa
#endif // end of simulation               
#define FILTERING_SDP3X_MIN        0.01   // 
#define FILTERING_SDP3X_MAX        0.1 // 
#define FILTERING_SDP3X_MIN_AT       10 // when abs is less than MIN_AT , apply MIN  
#define FILTERING_SDP3X_MAX_AT       100 // when abs is more than MAX_AT , apply MAX (interpolation in between)
            float abs_deltaDifPressure =  abs(airSpeedData.difPressureAdc_zero - smoothDifPressure) ;
            if (abs_deltaDifPressure <= FILTERING_SDP3X_MIN_AT) {
                expoSmooth_auto = FILTERING_SDP3X_MIN ;  
            } else if (abs_deltaDifPressure >= FILTERING_SDP3X_MAX_AT)  {
                expoSmooth_auto = FILTERING_SDP3X_MAX ; 
            } else {
                expoSmooth_auto = FILTERING_SDP3X_MIN + ( FILTERING_SDP3X_MAX - FILTERING_SDP3X_MIN) * (abs_deltaDifPressure - FILTERING_SDP3X_MIN_AT) / (FILTERING_SDP3X_MAX_AT - FILTERING_SDP3X_MIN_AT) ;
            }
            smoothDifPressure += expoSmooth_auto * ( airSpeedData.difPressureAdc_zero - smoothDifPressure ) ; // 
          
               // calculate airspeed based on pressure, altitude and temperature
               // airspeed (m/sec) = sqr(2 * differential_pressure_in_Pa / air_mass_kg_per_m3) 
               // air_mass_kg_per_m3 = pressure_in_pa / (287.05 * (Temp celcius + 273.15))
               // so airspeed m/sec =sqr( 2 * 287.05 * differential_pressure_pa * (temperature Celsius + 273.15) / pressure_in_pa )
               // so at 15° and 1013hpa in cm/sec = 127.79 (=sqr(2*287.05*288.15/101300))
               // or rawAirSpeed cm/sec =  2396 * sqrt( (float) abs(smoothDifPressureAdc) * temperatureKelvin  /  actualPressure) ); // in cm/sec ; actual pressure must be in pa (so 101325 about at sea level)
#ifdef AIRSPEED_AT_SEA_LEVEL_AND_15C
            airSpeedData.smoothAirSpeed =  127.79 * sqrt( (float) ( abs(smoothDifPressure) ) ); // indicated airspeed is calculated at 15 Celsius and 101325 pascal
#else               
            airSpeedData.smoothAirSpeed =  2396.0 * sqrt( (float) ( abs(smoothDifPressure) * temperatureKelvinSdp3x  /  actualPressure) ); // in cm/sec ; actual pressure must be in pa (so 101325 about at sea level)
#endif              
            if ( smoothDifPressure < 0 ) airSpeedData.smoothAirSpeed = - airSpeedData.smoothAirSpeed ; // apply the sign
              
#ifdef DEBUG_SDP3X_RAWDATA  
            static bool firstRawData = true ;
            if ( firstRawData ) {
                printer->println(F("at,  difPressure , expoSmooth_auto ,  smoothDifPressure, smoothAirSpeed, ")) ;
                firstRawData = false ;
            } else {
                printer->print( airSpeedMillis ); printer->print(F(" , "));
                printer->print(  airSpeedData.difPressureAdc_zero); printer->print(F(" , "));
                printer->print( expoSmooth_auto ); printer->print(F(" , "));
                printer->print( smoothDifPressure ); printer->print(F(" , ")); 

                printer->print( airSpeedData.smoothAirSpeed * 3.6 / 100); printer->print(F(" , ")); 
                printer->println(" ") ; 
            }       
#endif
        } // end no error on I2C    
        
        if (airSpeedMillis > nextAirSpeedMillis) { // publish airspeed only once every xx ms
              nextAirSpeedMillis = airSpeedMillis + 200 ;
              if ( airSpeedData.smoothAirSpeed >  0) {  // normally send only if positive and greater than 300 cm/sec , otherwise send 0 but for test we keep all values to check for drift  
#ifdef AIRSPEED_IN_KMH  // uncomment this line if AIR speed has to be in knot instead of km/h
                  airSpeedData.airSpeed.value = airSpeedData.smoothAirSpeed * 0.36 ; // from cm/sec to 1/10 km/h
#else
                  airSpeedData.airSpeed.value = airSpeedData.smoothAirSpeed * 0.1943844492 ; // from cm/sec to 1/10 knot/h
#endif
              } else {
                  airSpeedData.airSpeed.value = 0 ;
              }    
              airSpeedData.airSpeed.available = true ;  
        }  // end of process every xx millis
    } // end of process every 2 millis
} // End of readSensor


