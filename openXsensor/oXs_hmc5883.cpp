/* =============================================================================
This part is based on Nav6 project (source code is placed under the MIT license)

Copyright (c) 2013 Kauai Labs 

Portions of this work are based upon the I2C Dev Library by Jeff Rowberg
(www.i2cdevlib.com) which is open-source licensed under the MIT
License.  

This work also incorporates modifications on the official Invensense Motion
Driver v. 5.1 library in order to reduce the size of flash memory.
=============================================================================
*/





#include "oXs_hmc5883.h"

#ifdef USE_HMC5883

#ifdef DEBUG 
  #ifndef GENERATE_MAG_CALIBRATION_DATA
    #define DEBUG_HMC5883
  #endif
#endif

    float magHeading ;
    boolean newMagHeading ;
    const int16_t XMagOffset = XMAG_OFFSET ;
    const int16_t YMagOffset = YMAG_OFFSET ;
    const int16_t ZMagOffset = ZMAG_OFFSET ;
    const float XX  = XXMAG_CORRECTION ;
    const float XY  = XYMAG_CORRECTION ;
    const float XZ  = XZMAG_CORRECTION  ;
    const float YX  = YXMAG_CORRECTION  ;
    const float YY  = YYMAG_CORRECTION ;
    const float YZ  = YZMAG_CORRECTION ;
    const float ZX  = ZXMAG_CORRECTION ;
    const float ZY  = ZYMAG_CORRECTION ;
    const float ZZ  = ZZMAG_CORRECTION ;


extern struct ONE_MEASUREMENT roll ;
extern struct ONE_MEASUREMENT pitch ;

uint8_t hmc5883WriteByte (uint8_t reg ,  uint8_t character ) {
  return I2c.write ( HMC5883L_ADDRESS , reg , character ) ;  
}

boolean setup_hmc5883() {  // set up magnetometer
            // note: when HMC5883 is on a GY86, I2C has to be accessed through the IMU6050; it requires some special config in the IMU register; this is already done in the IMU initialisation
#ifdef DEBUG_HMC5883
    Serial.println(F("Initializing HMC5883..."));
#endif
    I2c.begin() ;
    I2c.timeOut( 80); //initialise the time out in order to avoid infinite loop 

#ifdef DEBUG_HMC5883
    uint8_t error ;
    error = I2c.read( HMC5883L_ADDRESS , HMC5883L_REG_IDENT_A , 3 ) ;
    Serial.print(F("error= "));
    Serial.println(error) ;
    Serial.print(F("Id= "));
    Serial.print(I2c.receive()) ;
    Serial.print(F(" , "));
    Serial.print(I2c.receive()) ;
    Serial.print(F(" , "));
    Serial.println(I2c.receive()) ;  
#endif
    hmc5883WriteByte(HMC5883L_REG_CONFIG_B,  HMC5883L_RANGE_0_88GA << 5); // set the range on 0.88 GA(max sensitivity)
    hmc5883WriteByte(HMC5883L_REG_MODE, HMC5883L_CONTINOUS); // set in continuous mode
    hmc5883WriteByte(HMC5883L_REG_CONFIG_A , (HMC5883L_SAMPLES_8 << 5) | ( HMC5883L_DATARATE_75HZ << 2)) ; // Takes 8 samples to average, datarate = 75hz , normal measurement (no external magnetic field) )
    
#ifdef DEBUG_HMC5883
    Serial.println(F("HMC5883 init done"));
#endif
  return true  ;
}

boolean read_hmc5883() {   // return a flag that is true when a new heading is available
  static uint32_t previousMillisHmc ;
  uint32_t currentMillisHmc = millis() ;
  if( currentMillisHmc > ( previousMillisHmc + 200 ) ) { // read the sensor every 200 msec 
      I2c.read( HMC5883L_ADDRESS , HMC5883L_REG_OUT_X_M , 6 ) ; // read 6 byte of data
      
      int16_t X = (I2c.receive() << 8 ) |  I2c.receive() ;
      int16_t Z = (I2c.receive() << 8 ) |  I2c.receive() ;
      int16_t Y = (I2c.receive() << 8 ) |  I2c.receive() ;



#ifdef  GENERATE_MAG_CALIBRATION_DATA
    Serial.print( (int) X ) ; Serial.print( " ") ;
    Serial.print( (int) Y ) ; Serial.print( " ") ;
    Serial.println( (int) Z ) ;
#endif

    int16_t XWithOff = X - XMAG_OFFSET ;
    int16_t YWithOff = Y - YMAG_OFFSET ;
    int16_t ZWithOff = Z - ZMAG_OFFSET ;
    float XC = XXMAG_CORRECTION * XWithOff + XYMAG_CORRECTION * YWithOff + XZMAG_CORRECTION * ZWithOff ;
    float YC = YXMAG_CORRECTION * XWithOff + YYMAG_CORRECTION * YWithOff + YZMAG_CORRECTION * ZWithOff ;
    float ZC = ZXMAG_CORRECTION * XWithOff + ZYMAG_CORRECTION * YWithOff + ZZMAG_CORRECTION * ZWithOff ;
    
    static float X1 ;
    static float Y1 ;
    static float Z1 ;
    static float rollRad1  ;
    static float pitchRad1 ;
    static float cosRoll ;
    static float sinRoll ;  
    static float cosPitch;
    static float sinPitch;

    static float Xh ;
    static float Yh ;

  float rollRad = ((float) roll.value) * 0.0017453293  ; // from 1/10 of degree to rad
  float pitchRad = ((float )pitch.value) * 0.0017453293 ;


#ifdef DEBUG_HMC5883
      static boolean firstPass = true ;
      if ( firstPass ) {
        firstPass = false ;
        Serial.println(F("X , Y , Z , uncomoensated tilted heading, roll , pitch ==> compensated tilted headings"));
      }
      Serial.print( (int) X ) ; Serial.print( " , ") ;
      Serial.print( (int) Y ) ; Serial.print( "  , ") ;
      Serial.print( (int) Z ) ; Serial.print( " - ") ;
      Serial.print( (int) ( atan2( YC , XC) * 57.29577951 ) ); Serial.print( "     ") ;   
      Serial.print( (int) roll.value / 10) ; Serial.print( " ") ; // in deg
      Serial.print( (int) pitch.value  / 10 ) ; Serial.print( " ==> ") ;  // in deg
#endif
 
  
  for ( uint8_t i = 10 ; i <= 10 ; i++) {         // this loop allows to test all combinations of reversing sign (to test them all, replace by i = 0 ; i<= 15
    X1 = XC ;
    Y1 = YC ;
    Z1 = ZC ;
    rollRad1 = rollRad ;
    pitchRad1 = pitchRad ;
    if (i & 0x01)  Y1 = - YC ; 
    if (i & 0x02)  Z1 = - ZC ; 
    if (i & 0x04)  rollRad1 = - rollRad ; 
    if (i & 0x8)  pitchRad1 = - pitchRad ;
    //Serial.println( " ") ;  
    cosRoll = cos(rollRad1);
    sinRoll = sin(rollRad1);  
    cosPitch = cos(pitchRad1);
    sinPitch = sin(pitchRad1);

    Xh = X1 * cosPitch + Y1 * sinRoll * sinPitch - Z1* cosRoll * sinPitch ;
    Yh = Y1 * cosRoll + Z1 * sinRoll ;

     
    if ( Xh == 0 ) { 
      if (Yh <= 0 ) { 
        magHeading  = 90 ;
      } else { 
        magHeading  = 270 ;
      }
    } else {
      magHeading  = atan2( Yh , Xh) * 57.29577951 ; //convert in degree  
    }
    newMagHeading = true ;
#ifdef DEBUG_HMC5883
     Serial.print( (int) magHeading  ) ;Serial.print( "   ") ;
#endif
  
  } // end for
#ifdef DEBUG_HMC5883
      Serial.println( " ") ;
#endif
  
      previousMillisHmc = currentMillisHmc ; 
      return true ;
      
  } else {
    return true ;
  }  
}   // end of read_hmc5883



#endif // end USE_HMC5883
