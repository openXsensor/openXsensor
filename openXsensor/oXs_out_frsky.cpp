// file for FRSKY telemetry (SPORT and HUB)

#include "oXs_out_frsky.h"
#if defined(PROTOCOL) && ( (PROTOCOL == FRSKY_SPORT) || ( PROTOCOL == FRSKY_HUB ) || (PROTOCOL == FRSKY_SPORT_HUB ) ) //if Frsky protocol is used

#ifdef DEBUG
// ************************* Several parameters to help debugging
//#define DEBUGSENDDATASTATUS
//#define DEBUGSENDDATA
//#define DEBUGSENDSENSITIVITY
//#define DEBUGNEXTVALUETYPE
//#define DEBUGSENDDATADELAY
//#define DEBUGTRANSMITDELAY
//#define DEBUGLOADVALUETOSEND
//#define DEBUGLOADVALUETOSENDALTIMETER
//#define DEBUGLOADVALUETOSENDVERTICALSPEED
//#define DEBUGLOADVALUETOSENDALTIMETER_2
//#define DEBUGLOADVALUETOSENDVERTICALSPEED_2

//#define DEBUGLOADVALUETOSENDVOLT1
//#define DEBUGLOADVALUETOSENDVOLT2
//#define DEBUGLOADVALUETOSENDSENSITIVITY
//#define DEBUGLOADVALUETOSENDALT_OVER_10_SEC
//#define DEBUGLOADVALUETOSENDSENSITIVITY_2
//#define DEBUGLOADVALUETOSENDALT_OVER_10_SEC_2

//#define DEBUGLOADVALUETOSENDCELL_1_2
//#define DEBUGLOADVALUETOSENDCELL_3_4
//#define DEBUGLOADVALUETOSENDCELL_5_6
//#define DEBUGLOADVALUETOSENDCURRENTMA
//#define DEBUGLOADVALUETOSENDMILLIAH
//#define DEBUGHUBPROTOCOL
//#define DEBUGWITHFIXVALUE     // for SPORT protocol only; send a value varying continuously (cycling) between min and max value (see code below)
#endif

extern OXS_MS5611 oXs_MS5611 ;
extern OXS_VOLTAGE oXs_Voltage ; 
extern OXS_CURRENT oXs_Current ;
extern OXS_4525 oXs_4525 ;

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

//used only by Sport protocol
extern uint8_t  volatile  sportData[7] ;
uint8_t volatile sportDataLock ;
extern uint8_t volatile sendStatus ;
#if defined(PIN_VOLTAGE) && defined(VFAS_SOURCE) 
  struct ONE_MEASUREMENT vfas ; 
#endif

#if defined(GPS_INSTALLED)
    struct ONE_MEASUREMENT sport_gps_lon ; 
    struct ONE_MEASUREMENT sport_gps_lat ;
    struct ONE_MEASUREMENT sport_gps_alt ;
    struct ONE_MEASUREMENT sport_gps_speed ; 
    struct ONE_MEASUREMENT sport_gps_course;
#endif

extern struct ONE_MEASUREMENT sport_rpm ;

//used only by Hub protocol
//static int fieldToSend ;
//static bool fieldOk ;
extern uint8_t volatile hubData[MAXSIZEBUFFER] ; 
//extern uint8_t volatile hubCurrentData ; //index of current data
extern uint8_t volatile hubMaxData ;   // max number of data prepared to be send


//Used by both protocols
volatile bool sportAvailable = false ;
//int fieldContainsData[][5]  = {  SETUP_FRSKY_DATA_TO_SEND } ; // contains the set up of field to be transmitted
//int numberOfFields = sizeof(fieldContainsData) / sizeof(fieldContainsData[0]) ;
//static uint16_t convertToSportId[15] = { FRSKY_SPORT_ID } ; // this array is used to convert an index inside fieldContainsData[][0] into the SPORT field Id (or defaultfield) 
//static uint8_t convertToHubId[15] = { FRSKY_HUB_ID } ; //// this array is used to convert an index inside fieldContainsData[][0] into the Hub field Id (or defaultfield) 
//static uint8_t currentFieldToSend = 0 ; 
extern volatile uint8_t state ;                  //!< Holds the state of the UART.


#ifdef DEBUG  
OXS_OUT::OXS_OUT(uint8_t pinTx,HardwareSerial &print)
#else
OXS_OUT::OXS_OUT(uint8_t pinTx)
#endif
{
  _pinTx = pinTx ;    
#ifdef DEBUG 
  printer = &print; //operate on the address of print
#endif
} // end constructor


// **************** Setup the FRSky OutputLib *********************
void OXS_OUT::setup() {
// here we look if sport is available or not; when available, sport protocol must be activated otherwise hub protocol
    //initilalise PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input, tri-stated.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is input, tri-stated.

#if defined( PROTOCOL ) && ( PROTOCOL == FRSKY_SPORT )
    initMeasurement() ;
    initSportUart(  ) ;
    sportAvailable = true ;     // force the SPORT protocol
#elif defined(PROTOCOL) && ( PROTOCOL == FRSKY_HUB )
    initHubUart( ) ;
    sportAvailable = false ;    // force the HUB protocol
#else                           // we will detect automatically if SPORT is available     
                                      // Activate pin change interupt on Tx pin
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;			            // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                  // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif // test on PIN_SERIALTX
    delay(1500) ; // this delay has been added because some users reported that SPORT is not recognised with a X6R ; perhaps is it related to the EU firmware (2015)
#ifdef DEBUG_SPORT_PIN 
    digitalWrite(DEBUG_SPORT_PIN, HIGH); // Set the pulse used during SPORT detection to HIGH because detecttion is starting
#endif

    PCIFR = (1<<PCIF2) ;	            // clear pending interrupt
    delay(20) ;                      // to see if SPORT is active, we have to wait at least 12 msec and check bit PCIF2 from PCIFR; if bit is set, it is SPORT
#ifdef DEBUG_SPORT_PIN
    digitalWrite(DEBUG_SPORT_PIN, LOW); // Set the pulse used during SPORT detection to LOW because detection is done
#endif    
    if ( ( PCIFR & (1<<PCIF2)) == 0 ) {
        sportAvailable = false ;
        initHubUart() ;
    }
    else {
        sportAvailable = true ;
        initMeasurement() ;
        initSportUart() ;
    }
#endif // end test on FRSKY
#ifdef DEBUG
      printer->print(F("FRSky Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F("Sport protocol= "));
      printer->println(sportAvailable);
#endif // end DEBUG
}  // end of setup


void OXS_OUT::sendData() {
#if defined( PROTOCOL ) && ( PROTOCOL == FRSKY_SPORT )
  sendSportData() ;
#elif defined(PROTOCOL) && ( PROTOCOL == FRSKY_HUB )
  sendHubData() ;
#else                           // we will detect automatically if SPORT is available     
  if (sportAvailable) {
	  sendSportData() ;
  } else {
	  sendHubData() ;
  }
#endif  
}

//For SPORT protocol
//****************************************************** Look which value can be transmitted and load it in a set of fields used by interrupt routine
// oXs reacts on 6 sensorId being send by the Rx
// In the main loop, we look periodically (calling function sendData ) if a new data has to be preloaded for each of the 6 sensors
// It is the ISR that send the data and set a flag (to 1) in a bit of "frskyStatus" to say that a new data has to be loaded (bit 0...5 are used).
//       The main loop set the bit to 0 when a data has been loaded.

#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )

volatile uint8_t frskyStatus = 0x3F  ;                                                   //Status of SPORT protocol saying if data is to load in work field (bit 0...5 are used for the 6 sensorId), initially all data are to be loaded
uint8_t currFieldIdx[6] = { 0 , 2, 5 , 8 , 13 , 17 } ;                          // per sensor, say which field has been loaded the last time (so next time, we have to search from the next one)
const uint8_t fieldMinIdx[7]  = { 0 , 2, 5 , 8 , 13 , 17 , 20 } ;                     // per sensor, say the first field index ; there is one entry more in the array to know the last index
const uint8_t fieldId[20] = { 0x10 , 0x11 , 0x30 , 0x30 , 0x30 , 0x21 , 0x20 , 0x60 , 0x80, 0x80 , 0x82 , 0x83 , 0x84 , 0x50 , 0x40 , 0x41 , 0xA0 , 0x70 , 0x71 , 0x72 } ; //fieldID to send to Tx (to shift 4 bits to left
struct ONE_MEASUREMENT * p_measurements[20] ;      // array of 20 pointers (each pointer point to a structure containing a byte saying if a value is available and to the value.
// There are 20 possible fields to transmit in SPORT
// They are grouped per sensor ID
// Sensor 0 start from index = 0 and contains Alt + Vspeed
// Sensor 1 start from index = 2 and contains Cell_1_2 , Cell_3_4 and Cell_5_6
// Sensor 2 start from index = 5 and contains vfas , current and fuel
// Sensor 3 start from index = 8 and contains gps_lon , gps_lat, gps_alt , gps_speed and gps_course
// Sensor 4 start from index = 13 and contains rpm, T1, T2, airspeed
// Sensor 5 start from index = 17 and contains accX , accY, accZ

int32_t dataValue[6] ;   // keep for each sensor id the next value to be sent
uint8_t dataId[6] ;      // keep for each sensor id the Id of next field to be sent
uint8_t sensorSeq  ;
uint8_t sensorIsr  ;

struct ONE_MEASUREMENT no_data = { 0, 0 } ; 

void initMeasurement() {
// pointer to Altitude
#if defined(VARIO) 
    p_measurements[0] = &oXs_MS5611.varioData.relativeAlt ; // we use always relative altitude in Frsky protocol
#else
   p_measurements[0] = &no_data ;
#endif

// pointer to VSpeed
#if defined(VARIO)
    p_measurements[1] = &mainVspeed ;  
#else
    p_measurements[1] = &no_data ;
#endif

// pointer to Cell_1_2
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 0)      
   p_measurements[2] = &oXs_Voltage.voltageData.mVoltCell_1_2 ; 
#else
    p_measurements[2] = &no_data ;
#endif

// pointer to Cell_3_4
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 2)      
   p_measurements[3] = &oXs_Voltage.voltageData.mVoltCell_3_4 ; 
#else
    p_measurements[3] = &no_data ;
#endif

// pointer to Cell_5_6
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 4)      
   p_measurements[4] = &oXs_Voltage.voltageData.mVoltCell_5_6 ; 
#else
    p_measurements[4] = &no_data ;
#endif
// pointer to vfas
#if defined(PIN_VOLTAGE) && defined(VFAS_SOURCE) && ( VFAS_SOURCE == VOLT_1 || VFAS_SOURCE == VOLT_2 || VFAS_SOURCE == VOLT_3 || VFAS_SOURCE == VOLT_4 || VFAS_SOURCE == VOLT_5 || VFAS_SOURCE == VOLT_6 )
    p_measurements[5] = &vfas ;
#else
    p_measurements[5] = &no_data ;
#endif
   
// pointer to current
#if defined(PIN_CURRENTSENSOR) 
    p_measurements[6] = &oXs_Current.currentData.milliAmps ;
#else
    p_measurements[6] = &no_data ;
#endif

// pointer to fuel                                    // not defined currently
   p_measurements[7] = &no_data ;

// pointer to GPS lon
#if defined(GPS_INSTALLED)
  p_measurements[8] = &sport_gps_lon ; 
#else
  p_measurements[8] = &no_data ; 
#endif

// pointer to GPS lat
#if defined(GPS_INSTALLED)
  p_measurements[9] = &sport_gps_lat ; 
#else
  p_measurements[9] = &no_data ; 
#endif

// pointer to GPS alt
#if defined(GPS_INSTALLED)
  p_measurements[10] = &sport_gps_alt ; 
#else
  p_measurements[10] = &no_data ; 
#endif

// pointer to GPS speed
#if defined(GPS_INSTALLED)
  p_measurements[11] = &sport_gps_speed ; 
#else
  p_measurements[11] = &no_data ; 
#endif

// pointer to GPS course
#if defined(GPS_INSTALLED)
  p_measurements[12] = &sport_gps_course ; 
#else
  p_measurements[12] = &no_data ; 
#endif

// pointer to RPM
#if defined(MEASURE_RPM) 
  p_measurements[13] = &sport_rpm ; 
#else
  p_measurements[13] = &no_data ; 
#endif

// pointer to T1
#if defined(T1_SOURCE) && ( T1_SOURCE == TEST_1)
   p_measurements[14] = &test1 ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == TEST_2)
   p_measurements[14] = &test2 ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == TEST_3)
   p_measurements[14] = &test3 ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == GLIDER_RATIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[14] = &gliderRatio ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[14] = &secFromT0 ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[14] = &averageVspeedSinceT0 ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == SENSITIVITY) && defined(VARIO)
   p_measurements[14] = &oXs_MS5611.varioData.sensitivity ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == PPM) && defined(PIN_PPM)
   p_measurements[14] = &ppm ; 
#elif defined(T1_SOURCE) && defined(PIN_VOLTAGE) && ( T1_SOURCE == VOLT_1 || T1_SOURCE == VOLT_2 || T1_SOURCE == VOLT_3 || T1_SOURCE == VOLT_4 || T1_SOURCE == VOLT_5 || T1_SOURCE == VOLT_6 )
   p_measurements[14] = &oXs_Voltage.voltageData.mVolt[T1_SOURCE - VOLT_1] ;
#else
   p_measurements[14] = &no_data ; // T1 
#endif

// pointer to T2   
#if defined(T2_SOURCE) && ( T2_SOURCE == TEST_1)
   p_measurements[15] = &test1 ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == TEST_2)
   p_measurements[15] = &test2 ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == TEST_3)
   p_measurements[15] = &test3 ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == GLIDER_RATIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[15] = &gliderRatio ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[15] = &secFromT0 ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[15] = &averageVspeedSinceT0 ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == SENSITIVITY) && defined(VARIO) 
   p_measurements[15] = &oXs_MS5611.varioData.sensitivity ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == PPM) && defined(PIN_PPM)
   p_measurements[15] = &ppm ; 
#elif defined(T2_SOURCE) && defined(PIN_VOLTAGE) && ( T2_SOURCE == VOLT_1 || T2_SOURCE == VOLT_2 || T2_SOURCE == VOLT_3 || T2_SOURCE == VOLT_4 || T2_SOURCE == VOLT_5 || T2_SOURCE == VOLT_6 )
   p_measurements[15] = &oXs_Voltage.voltageData.mVolt[T2_SOURCE - VOLT_1] ;
#else
   p_measurements[15] = &no_data ; // T2 
#endif


   
// pointer to airspeed
#if defined(AIRSPEED) 
  p_measurements[16] = &oXs_4525.airSpeedData.airSpeed ; 
#else
  p_measurements[16] = &no_data ; 
#endif

// pointer to accX
#if defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_1)
   p_measurements[17] = &test1 ; // accX
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_2)
   p_measurements[17] = &test2 ; // accX
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_3)
   p_measurements[17] = &test3 ; // accX
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == GLIDER_RATIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[17] = &gliderRatio ; 
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[17] = &secFromT0 ; 
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[17] = &averageVspeedSinceT0 ; 
#elif defined(ACCX_SOURCE) && defined(PIN_VOLTAGE) && ( ACCX_SOURCE == VOLT_1 || ACCX_SOURCE == VOLT_2 || ACCX_SOURCE == VOLT_3 || ACCX_SOURCE == VOLT_4 || ACCX_SOURCE == VOLT_5 || ACCX_SOURCE == VOLT_6 )
   p_measurements[17] = &oXs_Voltage.voltageData.mVolt[ACCX_SOURCE - VOLT_1] ;
#else
   p_measurements[17] = &no_data ; // accX
#endif

// pointer to accY
#if defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_1)
   p_measurements[18] = &test1 ; // accY
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_2)
   p_measurements[18] = &test2 ; // accY
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_3)
   p_measurements[18] = &test3 ; // accY
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == GLIDER_RATIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[18] = &gliderRatio ; 
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[18] = &secFromT0 ; 
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[18] = &averageVspeedSinceT0 ; 
#elif defined(ACCY_SOURCE) && defined(PIN_VOLTAGE) && ( ACCY_SOURCE == VOLT_1 || ACCY_SOURCE == VOLT_2 || ACCY_SOURCE == VOLT_3 || ACCY_SOURCE == VOLT_4 || ACCY_SOURCE == VOLT_5 || ACCY_SOURCE == VOLT_6 )
   p_measurements[18] = &oXs_Voltage.voltageData.mVolt[ACCY_SOURCE - VOLT_1] ;
#else
   p_measurements[18] = &no_data ; // accY
#endif

// pointer to accZ
#if defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_1)
   p_measurements[19] = &test1 ; // accZ
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_2)
   p_measurements[19] = &test2 ; // accZ
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_3)
   p_measurements[19] = &test3 ; // accZ
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == GLIDER_RATIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[19] = &gliderRatio ; 
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[19] = &secFromT0 ; 
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   p_measurements[19] = &averageVspeedSinceT0 ; 
#elif defined(ACCZ_SOURCE) && defined(PIN_VOLTAGE) && ( ACCZ_SOURCE == VOLT_1 || ACCZ_SOURCE == VOLT_2 || ACCZ_SOURCE == VOLT_3 || ACCZ_SOURCE == VOLT_4 || ACCZ_SOURCE == VOLT_5 || ACCZ_SOURCE == VOLT_6 )
   p_measurements[19] = &oXs_Voltage.voltageData.mVolt[ACCZ_SOURCE - VOLT_1] ;
#else
   p_measurements[19] = &no_data ; // accZ
#endif

}


void OXS_OUT::sendSportData()
{  
                                                                          // first we calculate fields that are used only by SPORT
#if defined(PIN_VOLTAGE) && defined(VFAS_SOURCE) 
  #if (VFAS_SOURCE == VOLT_1) || (VFAS_SOURCE == VOLT_2) || (VFAS_SOURCE == VOLT_3) || (VFAS_SOURCE == VOLT_4) || (VFAS_SOURCE == VOLT_5) || (VFAS_SOURCE == VOLT_6)
   if ( (!vfas.available) && ( oXs_Voltage.voltageData.mVolt[VFAS_SOURCE - VOLT_1].available) ){
      vfas.value = oXs_Voltage.voltageData.mVolt[VFAS_SOURCE - VOLT_1].value / 10 ;  // voltage in mv is divided by 10 because SPORT expect it (volt * 100) 
      vfas.available = true ; 
   }
  #else
  #error When defined, VFAS_SOURCE must be VOLT_1, VOLT_2, ... or VOLT_6
  #endif
#endif

#if defined(GPS_INSTALLED)
  if (GPS_lonAvailable) {
        sport_gps_lon.available = GPS_lonAvailable ;
        GPS_lonAvailable = false ; 
        sport_gps_lon.value = (( ((((uint32_t)( GPS_lon < 0 ? -GPS_lon : GPS_lon)) /10 ) * 6 ) / 10 ) & 0x3FFFFFFF)  | 0x80000000;  
        if(GPS_lon < 0) sport_gps_lon.value |= 0x40000000;
  }
  if (GPS_latAvailable) {
        sport_gps_lat.available = GPS_latAvailable ;
        GPS_latAvailable = false ;
        sport_gps_lat.value = ((  ((((uint32_t)( GPS_lat < 0 ? -GPS_lat : GPS_lat)) / 10 ) * 6 )/ 10 ) & 0x3FFFFFFF ) ;
        if(GPS_lat < 0) sport_gps_lat.value |= 0x40000000;
  }
  if (GPS_altitudeAvailable) {
        sport_gps_alt.available = GPS_altitudeAvailable ;
        GPS_altitudeAvailable = false ;
        sport_gps_alt.value = GPS_altitude / 10; // convert mm in cm 
  }
  #ifdef GPS_SPEED_3D
  if (GPS_speed_3dAvailable) { 
        sport_gps_speed.available = GPS_speed_3dAvailable ;
        GPS_speed_3dAvailable = false ;
    #ifdef GPS_SPEED_IN_KMH
         sport_gps_speed.value = ( ((uint32_t) GPS_speed_3d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
    #else                                
         sport_gps_speed.value = ( ((uint32_t) GPS_speed_3d) * 700 ) / 36  ; // convert cm/s in 1/100 of knots (factor = 19.44)
    #endif // end of GPS_SPEED_IN_KMH      
  }
  #else                   // use gps_Speed_2d
  if (GPS_speed_2dAvailable) { 
           sport_gps_speed.available = GPS_speed_2dAvailable ;
           GPS_speed_2dAvailable = false ;
    #ifdef GPS_SPEED_IN_KMH
           sport_gps_speed.value = ( ((uint32_t) GPS_speed_2d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
    #else                                
           sport_gps_speed.value = ( ((uint32_t) GPS_speed_2d) * 700 ) / 36 ; // convert cm/s in 1/1000 of knots (factor = 19.44)
    #endif // end of GPS_SPEED_IN_KMH
  }
  #endif //  enf of GPS_SPEED_3D  or 2D           
  if (GPS_ground_courseAvailable) {
            sport_gps_course.available = GPS_ground_courseAvailable ;
            GPS_ground_courseAvailable = false ;
            sport_gps_course.value = GPS_ground_course / 1000;               // convert from degree * 100000 to degree * 100                
  }     
#endif // end of GPS_INSTALLED

//    Serial.print("frskyStatus "); Serial.println(frskyStatus,HEX) ;
    if ( frskyStatus ) {                                                                                  // if at least one data has to be loaded
      for (uint8_t sensorSeq = 0 ; sensorSeq < 6 ; sensorSeq++ ) {                                        // for each sensor (currently 6)  
        if ( frskyStatus & (1 << sensorSeq ) )  {                          //if frskyStatus says that a data must be loaded for this sensor
            uint8_t currFieldIdx_ = currFieldIdx[sensorSeq] ;                // retrieve the last field being loaded
            for (uint8_t iCount = fieldMinIdx[sensorSeq] ; iCount < fieldMinIdx[sensorSeq+1] ; iCount++ ) {        // we will not seach more than the number of fields for the selected sensor 
                currFieldIdx_++ ;                                                                          // search with next field
                if ( currFieldIdx_ >= fieldMinIdx[sensorSeq+1] ) currFieldIdx_ = fieldMinIdx[sensorSeq] ;        // if overlap within sensor range, set idx to first idx for this sensorSeq
//                Serial.print("currFieldIdx_ "); Serial.print(currFieldIdx_) ; 
//                Serial.print(" p_m.av "); Serial.print( p_measurements[currFieldIdx_]->available) ;
//                Serial.print(" p_m.va "); Serial.println( p_measurements[currFieldIdx_]->value) ;
                if ( p_measurements[currFieldIdx_]->available  ){                                                // if data of current index of sensor is available
                  p_measurements[currFieldIdx_]->available = 0 ;                                                         // mark the data as not available
                  dataValue[sensorSeq] =  p_measurements[currFieldIdx_]->value ;                                         // store the value in a buffer
                  dataId[sensorSeq] = fieldId[currFieldIdx_] ;                                                   // mark the data from this sensor as available
                  cli() ;
                  frskyStatus &= ~(1<< sensorSeq) ;                                               // says that data is loaded by resetting one bit
                  sei();
                  Serial.print("Load "); Serial.print(dataId[sensorSeq],HEX) ; Serial.print(" ") ; Serial.println(dataValue[sensorSeq]);
                  break ;                                                                         // exit inner for
                }            
            }     
            currFieldIdx[sensorSeq] = currFieldIdx_   ;                                            // save currentFieldIdx for this 
        }
      } // End for one sensorSeq 
    }   // End of if (frskystatus)
}

#endif
// -------------------------End of SPORT protocol--------------------------------------------------------------------------------------

//========================= Hub protocol ==========================================
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_HUB ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )
void OXS_OUT::sendHubData()  // for Hub protocol
{
  static uint32_t lastMsFrame1=0;
  static uint32_t temp ;
#ifdef GPS_INSTALLED  
  static unsigned int lastMsFrame2=0;
#endif
  
  temp = millis() ;
  if (  (state == IDLE) && (temp-lastMsFrame1) >= INTERVAL_FRAME1  ) {
#ifdef DEBUGHUBPROTOCOL
     printer->print("Send Data at = ");
     printer->println( millis() );
#endif
    lastMsFrame1=temp;
    SendFrame1();
  }
//  second frame used for GPS
#ifdef GPS_INSTALLED
  if ( (state == IDLE ) && (temp-lastMsFrame2) > INTERVAL_FRAME2  ) {
    lastMsFrame2=temp;
    SendFrame2();
  }
#endif
}  // end sendData Hub protocol

//======================================================================================================Send Frame 1A via serial
void OXS_OUT::SendFrame1(){
  hubMaxData = 0 ; // reset of number of data to send

// pointer to Altitude
#if defined(VARIO) 
  uint16_t Centimeter =  uint16_t(abs(oXs_MS5611.varioData.relativeAlt.value)%100);
  int32_t Meter;
  if (oXs_MS5611.varioData.relativeAlt.value >0){
    Meter = (oXs_MS5611.varioData.relativeAlt.value - Centimeter);
  } else{
    Meter = -1*(abs(oXs_MS5611.varioData.relativeAlt.value) + Centimeter);
  }
  Meter=Meter/100;
  SendValue(FRSKY_USERDATA_BARO_ALT_B, (int16_t)Meter);
  SendValue(FRSKY_USERDATA_BARO_ALT_A, Centimeter);
#endif

// VSpeed
#if defined(VARIO) 
  SendValue( FRSKY_USERDATA_VERT_SPEED , (int16_t) mainVspeed.value); 
#endif

// Cell_1_2
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 0)      
  SendCellVoltage( oXs_Voltage.voltageData.mVoltCell_1_2.value ); 
#endif

// Cell_3_4
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 2)      
  SendCellVoltage( oXs_Voltage.voltageData.mVoltCell_3_4.value) ; 
#endif

// Cell_5_6
#if defined(PIN_VOLTAGE) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 4)      
   SendCellVoltage( oXs_Voltage.voltageData.mVoltCell_5_6.value) ; 
#endif

// vfas
#if defined(PIN_VOLTAGE) && defined(VFAS_SOURCE) &&  ( (VFAS_SOURCE == VOLT_1) || (VFAS_SOURCE == VOLT_2) || (VFAS_SOURCE == VOLT_3) || (VFAS_SOURCE == VOLT_4) || (VFAS_SOURCE == VOLT_5) || (VFAS_SOURCE == VOLT_6) )
    SendValue( FRSKY_USERDATA_VFAS_NEW ,  (int16_t) (voltageData->mVolt[VFAS_SOURCE - VOLT_1 ].value / 100) ) ; // convert mvolt in 1/10 of volt; in openTx 2.1.x, it is possible to get 1 more decimal using [VFAS_SOURCE - VOLT_1 ].value/10.)+2000);  
#endif
   
// current
#if defined(PIN_CURRENTSENSOR) 
    SendValue( FRSKY_USERDATA_CURRENT ,  (int16_t) oXs_Current.currentData.milliAmps.value ) ;
#endif

// fuel                                     // not defined currently
   
// RPM
#if defined(MEASURE_RPM) 
    SendValue( FRSKY_USERDATA_RPM , (int16) sport_rpm.value ) ; 
#endif

// T1
#if defined(T1_SOURCE) && ( T1_SOURCE == TEST_1)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) test1.value ) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == TEST_2)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) test2.value) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == TEST_3)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) test3.value) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == GLIDER_RATIO) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) gliderRatio.value) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) secFromT0.value) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) averageVspeedSinceT0.value) ;     
#elif defined(T1_SOURCE) && ( T1_SOURCE == SENSITIVITY) && defined(VARIO)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) oXs_MS5611.varioData.sensitivity.value) ; 
#elif defined(T1_SOURCE) && ( T1_SOURCE == PPM) && defined(PIN_PPM)
    SendValue( FRSKY_USERDATA_TEMP1 , (int16_t) ppm.value) ; 
#endif

// T2   
#if defined(T2_SOURCE) && ( T2_SOURCE == TEST_1)
   SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) test1.value ) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == TEST_2)
   SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) test2.value ) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == TEST_3)
   SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) test3.value ) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == GLIDER_RATIO) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) gliderRatio.value) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) secFromT0.value) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) averageVspeedSinceT0.value) ;     
#elif defined(T2_SOURCE) && ( T2_SOURCE == SENSITIVITY) && defined(VARIO)
   SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) oXs_MS5611.varioData.sensitivity.value) ; 
#elif defined(T2_SOURCE) && ( T2_SOURCE == PPM) && defined(PIN_PPM)
    SendValue( FRSKY_USERDATA_TEMP2 , (int16_t) ppm.value) ; 
#endif
   
// airspeed                                        // not implemented in Hub protocol; to add in T1 or T2
#if defined(AIRSPEED) 
  //oXs_4525.airSpeedData.airSpeed ; 
#endif

// accX
#if defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_1)
   SendValue( FRSKY_USERDATA_ACC_X , (int16_t) test1.value) ;
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_2)
   SendValue( FRSKY_USERDATA_ACC_X , (int16_t) test2.value) ;
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == TEST_3)
   SendValue( FRSKY_USERDATA_ACC_X , (int16_t) test3.value) ;
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == GLIDER_RATIO) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   SendValue( FRSKY_USERDATA_ACC_X , (int16_t) gliderRatio.value) ; 
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_X , (int16_t) secFromT0.value) ; 
#elif defined(ACCX_SOURCE) && ( ACCX_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_X , (int16_t) averageVspeedSinceT0.value) ;     
#endif

// accY
#if defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_1)
   SendValue( FRSKY_USERDATA_ACC_Y , (int16_t) test1.value) ;
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_2)
   SendValue( FRSKY_USERDATA_ACC_Y , (int16_t) test2.value) ;
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == TEST_3)
   SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) test3.value) ;
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == GLIDER_RATIO) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   SendValue( FRSKY_USERDATA_ACC_Y , (int16_t) gliderRatio.value) ; 
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_Y , (int16_t) secFromT0.value) ; 
#elif defined(ACCY_SOURCE) && ( ACCY_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_Y , (int16_t) averageVspeedSinceT0.value) ;     
#endif

// accZ
#if defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_1)
   SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) test1.value) ;
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_2)
   SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) test2.value) ;
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == TEST_3)
   SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) test3.value) ;
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == GLIDER_RATIO) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
   SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) gliderRatio.value) ; 
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == SECONDS_SINCE_T0 ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) secFromT0.value) ; 
#elif defined(ACCZ_SOURCE) && ( ACCZ_SOURCE == AVERAGE_VSPEED_SINCE_TO ) && defined(VARIO) && defined(GLIDER_RATIO_CALCULATED_AFTER_X_SEC)
    SendValue( FRSKY_USERDATA_ACC_Z , (int16_t) averageVspeedSinceT0.value) ;     
#endif

  if( hubMaxData > 0 ) {
    sendHubByte(0x5E) ; // End of Frame 1!
    setHubNewData(  ) ;
  }  
#ifdef DEBUGHUBPROTOCOL
      printer->print(F("Frame 1 to send: "));
      for (int cntPrint = 0 ; cntPrint < hubData.maxData ; cntPrint++) {
        printer->print(" ");
        printer->print(hubData.data[cntPrint] , HEX);
      }
     printer->println(" "); 
#endif  
}  // end send frame 1


#define FRSKY_USERDATA_GPS_ALT_B    0x01  // Altitude m
#define FRSKY_USERDATA_GPS_ALT_A    0x09  // Altitude in centimeter
#define FRSKY_USERDATA_GPS_SPEED_B  0x11  // Speed knots
#define FRSKY_USERDATA_GPS_LONG_B   0x12  // Longitude (DDMM)
#define FRSKY_USERDATA_GPS_LAT_B    0x13  // Latitude (DDMM)
#define FRSKY_USERDATA_GPS_CURSE_B  0x14  // Course degrees
#define FRSKY_USERDATA_GPS_SPEED_A  0x19  // Speed 2 decimals of knots
#define FRSKY_USERDATA_GPS_LONG_A   0x1A  // Longitude (.MMMM)
#define FRSKY_USERDATA_GPS_LAT_A    0x1B  // Latitude (.MMMM)
#define FRSKY_USERDATA_GPS_CURSE_A  0x1C  // // Course 2 decimals of degrees
#define FRSKY_USERDATA_GPS_LONG_EW  0x22  //(uint16_t)(flon < 0 ? 'W' : 'E')
#define FRSKY_USERDATA_GPS_LAT_EW   0x23  //(uint16_t)(lat < 0 ? 'S' : 'N')
#define FRSKY_USERDATA_GPS_DIST     0x3C


#ifdef GPS_INSTALLED
//======================================================================================================Send Frame 2 via serial used for GPS
void OXS_OUT::SendFrame2(){
  hubMaxData = 0 ; // reset of number of data to send
// here we fill the buffer with all GPS data
// GPS_lon             // longitude in degree with 7 decimals, (neg for S)
// GPS_lat             // latitude   in degree with 7 decimals, (neg for ?)
// GPS_altitude;       // altitude in mm
// GPS_speed_3d;       // speed in cm/s
// GPS_speed;          // speed in cm/s
// GPS_ground_course ; // degrees with 5 decimals
  uint32_t absLongLat = abs(GPS_lat) ;
  uint32_t decimalPartOfDegree = (absLongLat % 10000000 );
  uint32_t minWith7Decimals = decimalPartOfDegree * 60 ;
  SendValue(FRSKY_USERDATA_GPS_LAT_B , (uint16_t) (((absLongLat / 10000000L) * 100 ) +  (minWith7Decimals / 10000000L )) ) ; // Latitude (DDMM)
  SendValue(FRSKY_USERDATA_GPS_LAT_A , (uint16_t) (( minWith7Decimals % 10000000L) / 1000 ) ) ;                              // Latitude (.MMMM)
  SendValue(FRSKY_USERDATA_GPS_LAT_EW , (uint16_t)(GPS_lat < 0 ? 'S' : 'N')) ;
  absLongLat = abs(GPS_lon) ;
  decimalPartOfDegree = (absLongLat % 10000000 );
  minWith7Decimals = decimalPartOfDegree * 60 ;
  SendValue(FRSKY_USERDATA_GPS_LONG_B , (uint16_t) (((absLongLat / 10000000L) * 100 ) +  (minWith7Decimals / 10000000L )) ) ; // Longitude (DDMM)
  SendValue(FRSKY_USERDATA_GPS_LONG_A , (uint16_t) (( minWith7Decimals % 10000000L) / 1000 ) ) ;                              // Longitude (.MMMM)
  SendValue(FRSKY_USERDATA_GPS_LONG_EW , (uint16_t)(GPS_lon < 0 ? 'W' : 'E')) ;
  SendValue(FRSKY_USERDATA_GPS_ALT_B ,  (int16_t) GPS_altitude / 1000 );                                                      // Altitude m
  SendValue(FRSKY_USERDATA_GPS_ALT_A , (uint16_t) ( (abs(GPS_altitude) % 1000 ) / 10 ) ) ;                                    // Altitude centimeter
  uint32_t GPSSpeedKnot = GPS_speed_3d * 1944 ;                                                                               // speed in knots with 5 décimals (1 cm/sec = 0,0194384 knot)
  SendValue(FRSKY_USERDATA_GPS_SPEED_B , (uint16_t) ( GPSSpeedKnot / 100000) ) ;                                              // Speed knots
  SendValue(FRSKY_USERDATA_GPS_SPEED_A , (uint16_t) ( (GPSSpeedKnot % 100000 ) /1000) ) ;                                     // Speed 2 decimals of knots
  SendValue(FRSKY_USERDATA_GPS_CURSE_B , (uint16_t) ( GPS_ground_course / 100000 ) ) ;                                        // Course degrees
  SendValue(FRSKY_USERDATA_GPS_CURSE_A , (uint16_t) ( (GPS_ground_course % 100000) / 1000 ) ) ;                               // Course 2 decimals of degrees
  if( hubMaxData > 0 ) {
    sendHubByte(0x5E) ; // End of Frame 2!
    setHubNewData(  ) ;
  }  
#ifdef DEBUGHUBPROTOCOL
      printer->print("Frame2 to send = ");
      for (int cntPrint = 0 ; cntPrint < hubData.maxData ; cntPrint++) {
        printer->print(" ");
        printer->print(hubData.data[cntPrint] , HEX);
      }
     printer->println(" "); 
#endif
  
}
#endif // end of GPS_INSTALLED


// ******************************************************** //
// SendValue => send a value as frsky sensor hub data       //
// ******************************************************** //
void OXS_OUT::SendValue(uint8_t ID, uint16_t Value) {
  uint8_t tmp1 = Value & 0x00ff;
  uint8_t tmp2 = (Value & 0xff00)>>8;
  sendHubByte(0x5E) ;
  sendHubByte(ID);

  if ( (tmp1 == 0x5E) || (tmp1 == 0x5D) ){ 
	      tmp1 ^= 0x60 ;
        sendHubByte(0x5D);
  }
  sendHubByte(tmp1);  
  if ( (tmp2 == 0x5E) || (tmp2 == 0x5D) ){ 
	      tmp2 ^= 0x60 ;
        sendHubByte(0x5D);
  }
  sendHubByte(tmp2);
}

//***************************************************
// Put a byte in the buffer
//***************************************************
void OXS_OUT::sendHubByte( uint8_t byte )
{	
	hubData[hubMaxData] = byte ;
	hubMaxData ++ ;	
}

/*
// ***************************************************
// Search the value, format it and put it in a buffer
// ***************************************************
void OXS_OUT::loadHubValueToSend( uint8_t currentFieldToSend ) {
  fieldToSend = (int) convertToHubId[ fieldContainsData[currentFieldToSend][0] ]  ;
  if ( (fieldToSend >= FRSKY_USERDATA_GPS_ALT_B ) && (fieldToSend <= FRSKY_USERDATA_FUELPERCENT ) )  fieldOk = true ;
  else fieldOk = false ;
  switch ( fieldContainsData[currentFieldToSend][1] ) {

#ifdef VARIO       
      case  ALTIMETER :    
//          if ( (SwitchFrameVariant == 0) && (varioData->absoluteAltAvailable) ) { //========================================================================== Vario Data
              if (fieldToSend == DEFAULTFIELD) {
                  SendAlt( ( (varioData->relativeAlt.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );           
//                varioData->absoluteAlt.available = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData->relativeAlt.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                 varioData->absoluteAlt.available = false ;
              }   
//          }
          break ;
      case VERTICAL_SPEED : 
//          if ( (SwitchFrameVariant == 0) && (varioData->climbRateAvailable) ){
              if (fieldToSend == DEFAULTFIELD) {
                 // Attempt to work around the annoying 10cm double beep
                //SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)varioData->climbRate); // ClimbRate in open9x Vario mode
                if (varioData->climbRate.value==10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)9); // ClimbRate in open9x Vario mode
                else if (varioData->climbRate.value==-10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)-9);
                else SendValue(FRSKY_USERDATA_VERT_SPEED,( ( (int16_t) varioData->climbRate.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] ); // ClimbRate in open9x Vario mode
//                varioData->climbRateAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ((int16_t)varioData->climbRate.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
                 varioData->climbRate.available = false ;
              }  
//          }   
          break ;
       case SENSITIVITY :
//          if ( (SwitchFrameVariant == 0) && (varioData->sensitivity.available ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend , ( ((int16_t) varioData->sensitivity.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//               varioData->sensitivityAvailable = false ;
             }
//          }   
          break ;
       case ALT_OVER_10_SEC :
//          if ( (SwitchFrameVariant == 0) && (varioData->vSpeed10SecAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend , ( ( (int16_t) varioData->vSpeed10SecAvailable * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
//               varioData->vSpeed10SecAvailable = false ;
             }
//          }   
          break ;
          
          
#endif   // end vario   


#ifdef VARIO2       
      case  ALTIMETER_2 :    
 //         if ( (SwitchFrameVariant == 0) && (varioData_2->absoluteAltAvailable) ) { //========================================================================== Vario Data
              if (fieldToSend == DEFAULTFIELD) {
                SendAlt( ( (varioData_2->absoluteAlt.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4]);           
//                varioData_2->absoluteAltAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData_2->absoluteAlt.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                 varioData_2->absoluteAltAvailable = false ;
              }   
//          }
          break ;
      case VERTICAL_SPEED_2 : 
//          if ( (SwitchFrameVariant == 0) && (varioData_2->climbRateAvailable) ){
              if (fieldToSend == DEFAULTFIELD) {
                 // Attempt to work around the annoying 10cm double beep
                //SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)varioData->climbRate); // ClimbRate in open9x Vario mode
                if (varioData_2->climbRate.value == 10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)9); // ClimbRate in open9x Vario mode
                else if (varioData_2->climbRate.value == -10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)-9);
                else SendValue(FRSKY_USERDATA_VERT_SPEED, ( ( (int16_t) varioData_2->climbRate.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] ); // ClimbRate in open9x Vario mode
//                varioData_2->climbRateAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) varioData_2->climbRate.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//                 varioData_2->climbRateAvailable = false ;
              }  
//          }   
          break ;
       case SENSITIVITY_2 :
//          if ( (SwitchFrameVariant == 0) && (varioData_2->sensitivityAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend , ( ( (int16_t) varioData_2->sensitivity.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//               varioData_2->sensitivityAvailable = false ;
             }
//          }   
          break ;
       case ALT_OVER_10_SEC_2 :
//          if ( (SwitchFrameVariant == 0) && (varioData_2->vSpeed10SecAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData_2->vSpeed10SecAvailable * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
//               varioData_2->vSpeed10SecAvailable = false ;
             }
//          }   
          break ;
          
          
#endif   // end vario2 

#if defined (VARIO )  &&  defined (VARIO2)
      case VERTICAL_SPEED_A :
              if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) averageVSpeed.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }   
          break ;   
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) || defined(USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY ) && defined (PIN_PPM)
      case PPM_VSPEED :
              if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) switchVSpeed.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }   
          break ;   
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED : 
              if (fieldToSend == DEFAULTFIELD) {
                 SendGPSSpeed(  (( (int16_t) airSpeedData->airSpeed.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4]) ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) airSpeedData->airSpeed.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }  
          break ;
      case PRANDTL_COMPENSATION :    
//          if ( (SwitchFrameVariant == 0) && (airSpeedData->airSpeedAvailable) ) { //========================================================================== Vario Data
              if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) airSpeedData->compensation * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                 airSpeedData->airSpeedAvailable = false ;
              }   
//          }
          break ;
          
#endif   // end airspeed   

#if defined (VARIO) && defined ( AIRSPEED)
      case PRANDTL_DTE :    
              if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ((int16_t) compensatedClimbRate.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }   
          break ;                      
#endif  // End defined (VARIO) && defined ( AIRSPEED)


#ifdef PIN_VOLTAGE 
      case VOLT1 :  
         SendVoltX( 0 , currentFieldToSend) ;
         break ;
      case VOLT2 :  
         SendVoltX( 1 , currentFieldToSend) ;
          break ;
      case VOLT3 :  
         SendVoltX( 2 , currentFieldToSend) ;
          break ;
      case VOLT4 :  
         SendVoltX( 3 , currentFieldToSend) ;
          break ;
      case VOLT5 :  
         SendVoltX( 4 , currentFieldToSend) ;
         break ;
      case VOLT6 :  
         SendVoltX( 5 , currentFieldToSend) ;
         break ;
#endif

#if defined (PIN_CURRENTSENSOR)
      case CURRENTMA :
//          if ( (SwitchFrameVariant == 0) && (currentData->milliAmpsAvailable ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCurrentMilliAmps(( ( (int16_t) currentData->milliAmps.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4]);
//                 currentData->milliAmpsAvailable = false ;
             }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) currentData->milliAmps.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
//                 currentData->milliAmpsAvailable = false ;
              }  
//          }    
         break ;
      case MILLIAH :
//          if ( (SwitchFrameVariant == 0) && (currentData->consumedMilliAmpsAvailable) ){
                if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (currentData->consumedMilliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                   currentData->consumedMilliAmpsAvailable = false ;  
              }  
//           }
         break ;
//      case FRSKY_USERDATA_CURRENT_MAX :
////          if ( (SwitchFrameVariant == 0)  ) {
//             if ( fieldToSend == DEFAULTFIELD ) {
//                 SendValue(0x38,int16_t(currentData->maxMilliAmps));    //Cur+ OK!
//             }
//             else if(  fieldOk == true ) {
//                 SendValue((int8_t) fieldToSend ,(int16_t) ( (currentData->maxMilliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
//             }  
//          }    
//         break ;
#endif  // End PIN_CURRENTSENSOR

#if defined (NUMBEROFCELLS)  && (NUMBEROFCELLS > 0) && defined (PIN_VOLTAGE) 
      case  CELLS_1_2 :
//         if ( (SwitchFrameVariant == 0) && ( voltageData->mVoltCell_1_2_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( voltageData->mVoltCell_1_2.value ) ;
//                 voltageData->mVoltCell_1_2_Available  = false ;
             }
//          }    
          break ;
          
      case  CELLS_3_4 :
//          if ( (SwitchFrameVariant == 0) && ( voltageData->mVoltCell_3_4_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( voltageData->mVoltCell_3_4.value ) ;
//                 voltageData->mVoltCell_3_4_Available  = false ;
             }
//          }   
          break ;
          
      case  CELLS_5_6 :
//          if ( (SwitchFrameVariant == 0) && ( voltageData->mVoltCell_5_6_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( voltageData->mVoltCell_5_6.value ) ;
//                 voltageData->mVoltCell_5_6_Available  = false ;
//             }
          }   
          break ;

#endif  // NUMBEROFCELLS > 0         

#ifdef PIN_PPM
      case  PPM :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend , ( ( (int16_t) ppm * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;
#endif


#ifdef MEASURE_RPM
      case  RPM :
//          if ( (SwitchFrameVariant == 0) && ( RpmAvailable  ) ) {
             if ( fieldToSend == DEFAULTFIELD || fieldToSend == FRSKY_USERDATA_RPM ) {
                  SendValue(FRSKY_USERDATA_RPM, (( RpmValue * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4]);
             }
//          }
          break ;
#endif  //  MEASURE_RPM
      case  TEST1 :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test1.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;
      case  TEST2 :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test2.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;
      case  TEST3 :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test3.value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;


      }  // end Switch
}  // End function  loadValueToSend (Frame 1)

*/

#if defined(PIN_VOLTAGE) && defined (NUMBEROFCELLS)  && (NUMBEROFCELLS > 0)
// ********************************************************** //
// SendCellVoltage => send a cell voltage                     //
// ********************************************************** //
void OXS_OUT::SendCellVoltage( uint32_t voltage) {
  // For SPORT, cell voltage is formatted as (hex) 12 34 56 78 where 123 = volt of cell n+1 (divided by 2), 456 idem for cell n, 7 = max number of cell and 8 = n (number of cell)
  // target format for Hub (hex) is having 2 data sent in format : 84 56 and 91 23 (where 9 = content of 8 incresed by 1)
    byte cellID = (voltage & 0x0000000f);
    uint16_t cellVolt = ((voltage >> 8) & 0x0fff) ;
    uint8_t v1 = ( (cellID  )<<4 & 0xf0) | ((cellVolt & 0x0f00)>>8) ;
    uint8_t v2 = (cellVolt & 0x00ff);
    uint16_t Value = (v2<<8) | (v1 & 0x00ff) ;
    SendValue(FRSKY_USERDATA_CELL_VOLT, Value);
    cellID++;
    if (cellID < NUMBEROFCELLS) {
        cellVolt = (voltage & 0xfff00000) >> 20 ;
        uint8_t v1 = ( (cellID )<<4 & 0xf0) | ((cellVolt & 0x0f00)>>8) ;
        uint8_t v2 = (cellVolt & 0x00ff);
        uint16_t Value = (v2<<8) | (v1 & 0x00ff) ;
        SendValue(FRSKY_USERDATA_CELL_VOLT, Value);
    }  
}
#endif // enf of 6 voltage

// ********************************** //
//  SendGPSDist => send 0..32768      //
// ********************************** //
//void OXS_OUT::SendGPSDist(uint16_t dist) {// ==> Field "Dist" in open9x
//  SendValue(0x3C,uint16_t(dist)); //>> DIST
//}

// ************************************************************ //
//  SendTemperature1/2 =>  tempc in 1/100th of degree celsius   //
// Display Range in openTX: -3276,8..32768=-3276,8C..+3276,C    //
// ************************************************************ //
void OXS_OUT::SendTemperature1(int16_t tempc) {
  SendValue(FRSKY_USERDATA_TEMP1, tempc);
}
void OXS_OUT::SendTemperature2(int16_t tempc) {
  SendValue(FRSKY_USERDATA_TEMP2, tempc);
}
// ************************************* //
// SendRPM => Send Rounds Per Minute     //
// ************************************* //
//void OXS_OUT::SendRPM(uint16_t rpm) {
//  byte blades=2;
//  rpm = uint16_t((float)rpm/(60/blades));  
//  SendValue(FRSKY_USERDATA_RPM, rpmValue);
//}

// ************************************* //
// SendFuel => SendFuel Level            //
// ************************************* //
void OXS_OUT::SendFuel(uint16_t fuel) {
  SendValue(FRSKY_USERDATA_FUEL, fuel);
}

// ********************************** //
// SendAlt => Send ALtitude in cm     //
// *********************************  //
void OXS_OUT::SendAlt(long altcm)
{
  uint16_t Centimeter =  uint16_t(abs(altcm)%100);
  long Meter;

  if (altcm >0){
    Meter = (altcm-(long)Centimeter);
  }
  else{
    Meter = -1*(abs(altcm)+(long)Centimeter);
  }
  Meter=Meter/100;
  SendValue(FRSKY_USERDATA_BARO_ALT_B, (int16_t)Meter);
  SendValue(FRSKY_USERDATA_BARO_ALT_A, Centimeter);
}
// **************************************************************** //
// SendGPSAlt - send the a value to the GPS altitude field          //
// parameter: altitude in cm between -3276800 and 3276799           //
// cm will not be displayed                                         //
// **************************************************************** //
void OXS_OUT::SendGPSAlt(long altcm)
{
  uint16_t Centimeter =  uint16_t(abs(altcm)%100);
  long Meter;
  if (altcm >0){
    Meter = (altcm-(long)Centimeter);
  }
  else{
    Meter = -1*(abs(altcm)+(long)Centimeter);
  }
  Meter=Meter/100;

  // need to send a gps fix before openTX displays this field....
  SendValue(FRSKY_USERDATA_GPS_LONG_A, 1);
  SendValue(FRSKY_USERDATA_GPS_LONG_B, 1);
  SendValue(FRSKY_USERDATA_GPS_LAT_A, 1);
  SendValue(FRSKY_USERDATA_GPS_LAT_B, 1);
  // now send the data
  SendValue(FRSKY_USERDATA_GPS_ALT_B, Meter);
  SendValue(FRSKY_USERDATA_GPS_ALT_A, Centimeter);
}


// **************************************************************** //
// SendGPSSpeed - send the a value to the GPS speed                 //
// value is split in 2 fields                                       //
// knots and 1/10 of knots                                          //
// **************************************************************** //
void OXS_OUT::SendGPSSpeed(long speedknots)
{
  uint16_t knotDecimal =  uint16_t(abs(speedknots)%10);
  long knots;
  if (speedknots >0){
    knots = (speedknots-(long) knotDecimal);
  }
  else{
    knots = -1*(abs(speedknots)+(long)knotDecimal);
  }
  knots=knots/10;

  // need to send a gps fix before openTX displays this field....
  SendValue(FRSKY_USERDATA_GPS_LONG_A, 1);
  SendValue(FRSKY_USERDATA_GPS_LONG_B, 1);
  SendValue(FRSKY_USERDATA_GPS_LAT_A, 1);
  SendValue(FRSKY_USERDATA_GPS_LAT_B, 1);
  // now send the data
  SendValue(FRSKY_USERDATA_GPS_SPEED_B, knots);
  SendValue(FRSKY_USERDATA_GPS_SPEED_A, knotDecimal);
}

// *********************************************** //
// SendCurrentMilliAmps => Send Current            //
// current will be transmitted as 1/10th of A      //
// Range: -32768..32767 => disp -327,6..3276,7     //
// *********************************************** //
void OXS_OUT::SendCurrentMilliAmps(int32_t milliamps) 
{
#ifdef ForceAbsolutCurrent
  milliamps=abs(milliamps);
#endif 
  SendValue(FRSKY_USERDATA_CURRENT, (uint16_t)(milliamps/100));
}

//#endif // End of FRSKY_SPORT

#endif // end HUB protocol --------------------------------------------------------------------------

// ********************************** Here the code to handle the UART communication with the receiver
#ifdef DEBUG
//#define DEBUGSETNEWDATA
#endif

//#define DEBUGASERIAL              // Generate signal on A0 A1 in order to debug UART timing

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))

volatile uint8_t state ;                  //!< Holds the state of the UART.
static volatile unsigned char SwUartTXData ;     //!< Data to be transmitted.
static volatile unsigned char SwUartTXBitCount ; //!< TX bit counter.
static volatile uint8_t SwUartRXData ;           //!< Storage for received bits.
static volatile uint8_t SwUartRXBitCount ;       //!< RX bit counter.
static volatile uint8_t TxCount ;

volatile uint8_t debugUartRx ;

volatile uint8_t ppmInterrupted ; // This flag is activated at the end of handling interrupt on Timer 1 Compare A if during this interrupt handling an interrupt on pin change (INT0 or INT1) occurs
                         // in this case, ppm will be wrong and has to be discarded       
uint8_t sensorId ;


// Here the code for both Frsky protocols +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! \brief  Timer1 interrupt service routine. *************** interrupt between 2 bits (handled by timer1)
//
//  Timer1 will ensure that bits are written and read at the correct instants in time.
//  The state variable will ensure context switching between transmit and recieve.
//  If state should be something else, the variable is set to IDLE. IDLE is regarded as a safe state/mode.

//For Frsky only
uint8_t ByteStuffByte = 0 ;

// only for Hub
uint8_t volatile hubData[MAXSIZEBUFFER] ; 
uint8_t volatile hubMaxData ;   // max number of data prepared to be send

// only for Sport
uint8_t LastRx ;
uint8_t TxSportData[7] ;
uint16_t Crc ;
uint8_t  volatile  sportData[7] ;
uint8_t volatile sendStatus ;
uint8_t volatile gpsSendStatus ; 
uint8_t volatile gpsSportDataLock ;
uint8_t volatile gpsSportData[7] ;
uint8_t currentSensorId ; // save the sensor id being received and on which oXs will reply (can be the main sensor id or GPS sensor id) 


ISR(TIMER1_COMPA_vect)
{
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )  
  if ( sportAvailable ) {    // ++++++++ here only for SPORT protocol ++++++++++++++++++++++++++++++++++
  switch (state)
  {
  // Transmit Byte.
    case TRANSMIT :   // Output the TX buffer in SPORT ************ we are sending each bit of data
#ifdef DEBUGASERIAL
          PORTC |= 1 ;
#endif
          if( SwUartTXBitCount < 8 )
          {
            if( SwUartTXData & 0x01 )
            {           // If the LSB of the TX buffer is 1:
              CLEAR_TX_PIN() ;                    // Send a logic 1 on the TX_PIN.
            }
            else
            {                                // Otherwise:
              SET_TX_PIN() ;                      // Send a logic 0 on the TX_PIN.
            }
            SwUartTXData = SwUartTXData >> 1 ;    // Bitshift the TX buffer and
            SwUartTXBitCount += 1 ;               // increment TX bit counter.
          }
          else    //Send stop bit.
          {
            CLEAR_TX_PIN();                         // Output a logic 1.
            state = TRANSMIT_STOP_BIT;
                //ENABLE_TIMER0_INT() ;                   // Allow this in now.
          }
          OCR1A += TICKS2WAITONESPORT ;  // Count one period into the future.  
#ifdef DEBUGASERIAL
          PORTC &= ~1 ;
#endif
          break ;

  // Go to idle after stop bit was sent.
          case TRANSMIT_STOP_BIT: // SPORT ************************************* We have sent a stop bit, we look now if there is other byte to send
                if ( ByteStuffByte || (++TxCount < 8 ) )    // Have we sent 8 bytes?
                {
                  if ( ByteStuffByte )
                  {
                    SwUartTXData = ByteStuffByte ;
                    ByteStuffByte = 0 ;
                  }
                  else
                  {
                      if ( TxCount < 7 )    // Data (or crc)?
                      {
                          SwUartTXData = TxSportData[TxCount] ;
                          Crc += SwUartTXData ; //0-1FF
                          Crc += Crc >> 8 ; //0-100
                          Crc &= 0x00ff ;
                      }
                      else
                      {
                          SwUartTXData = 0xFF-Crc ; // prepare sending check digit
                      }
                      if ( ( SwUartTXData == 0x7E ) || ( SwUartTXData == 0x7D ) )
                      {
                        ByteStuffByte = SwUartTXData ^ 0x20 ;
                        SwUartTXData = 0x7D ;         
                      }
                  }
                  SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN. (= start bit)
                  OCR1A = TCNT1 + TICKS2WAITONESPORT - INTERRUPT_BETWEEN_TRANSMIT;   // Count one period into the future. Compensate the time for ISR
                  SwUartTXBitCount = 0 ;
                  state = TRANSMIT ;
                  //DISABLE_TIMER0_INT() ;      // For the byte duration
                }
                else  // 8 bytes have been send
                {
                  frskyStatus |=  1 << sensorIsr ;              // set the bit relative to sensorIsr to say that a new data has to be loaded for sensorIsr.
                  state = WAITING ;
                  OCR1A += DELAY_3500 ;   // 3.5mS gap before listening
                  TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
                  TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
                }
               break ;
               
     case RECEIVE :  // SPORT ****  Start bit has been received and we will read bits of data receiving, LSB first.    
           OCR1A += TICKS2WAITONESPORT ;                    // Count one period after the falling edge is trigged.
           {
                    uint8_t data ;        // Use a temporary local storage
                    data = SwUartRXBitCount ;
                    if( data < 8 ) {                         // If 8 bits are not yet read
                        SwUartRXBitCount = data + 1 ;
                        data = SwUartRXData ;
                        data >>= 1 ;                         // Shift due to receiving LSB first.
#ifdef DEBUGASERIAL
                        PORTC &= ~1 ;
#endif
                        if( GET_RX_PIN( ) == 0 ) {
                            data |= 0x80 ;                    // If a logical 1 is read, let the data mirror this.
                        }
#ifdef DEBUGASERIAL
                        PORTC |= 1 ;
#endif
                        SwUartRXData = data ;
                    }
                    else { //Done receiving =  8 bits are in SwUartRXData
#ifdef DEBUGASERIAL
                        PORTC &= ~1 ;
#endif
                        if ( LastRx == 0x7E ) {
                            switch (SwUartRXData ) {

#define  VARIO_ID        DATA_ID_VARIO       // replace those values by the right on
#define  CELL_ID         DATA_ID_FLVSS
#define  CURRENT_ID      DATA_ID_FAS
#define  GPS_ID          DATA_ID_GPS
#define  RPM_ID          DATA_ID_RPM
#define  ACC_ID          0x1B

                              case VARIO_ID :
                                sensorIsr = 0 ; break ;
                              case CELL_ID :
                                sensorIsr = 1 ; break ;
                              case CURRENT_ID :
                                sensorIsr = 2 ; break ;
                              case GPS_ID :
                                sensorIsr = 3 ; break ;
                              case RPM_ID :
                                sensorIsr = 4 ; break ;
                              case ACC_ID :
                                sensorIsr = 5 ; break ;
                              default : 
                                sensorIsr = 255 ;  
                            }
                            if ( ( sensorIsr < 6 ) && ( ( frskyStatus & ( 1 << sensorIsr )) == 0 ) ) {    // If this sensor ID is supported by oXs and oXs has prepared data to reply data in dataValue[] for this sensorSeq    
                                     // if ( sportDataLock == 0 ) {
                                          TxSportData[0] = 0x10 ;
                                          TxSportData[1] = dataId[sensorIsr] << 4  ;
                                          TxSportData[2] = dataId[sensorIsr] >> 4 ;
                                          TxSportData[3] = dataValue[sensorIsr] ;
                                          TxSportData[4] = dataValue[sensorIsr] >> 8 ;
                                          TxSportData[5] = dataValue[sensorIsr] >> 16 ;
                                          TxSportData[6] = dataValue[sensorIsr] >> 24 ;
                                          state = TxPENDING ;
                                          OCR1A += ( DELAY_400 - TICKS2WAITONESPORT) ;    // 400 uS gap before sending (remove 1 tick time because it was already added before
                                      //}
                            } else  { // No data are loaded (so there is no data yet available)
                                  state = WAITING ;       // Wait for idle time
                                  OCR1A += DELAY_3500 ;   // 3.5mS gap before listening
                            } 
                        }    // received 1 byte and was equal to 0x7E
                        else // So previous code is not equal to x7E 
                        {
                            DISABLE_TIMER_INTERRUPT() ;         // Stop the timer interrupts.
                            state = IDLE ;                                  // Go back to idle.
                            PCIFR = ( 1<<PCIF2 ) ;        // clear pending interrupt
                            PCICR |= ( 1<<PCIE2 ) ;       // pin change interrupt enabled
                        }
                        LastRx = SwUartRXData ;
                     } // End receiving  1 bit or 1 byte (8 bits)
           }
           break ;
  
  case TxPENDING :   // SPORT ****** we will here send a start bit before sending a byte
#ifdef DEBUGASERIAL
        PORTC |= 1 ;
#endif
        TRXDDR |= ( 1 << PIN_SERIALTX ) ;       // PIN is output
        SET_TX_PIN() ;                          // Send a logic 0 on the TX_PIN. = start bit
        OCR1A = TCNT1 +  ( TICKS2WAITONESPORT - INTERRUPT_ENTRY_TRANSMIT );    // Count one period into the future (less the time to execute ISR) .
        SwUartTXBitCount = 0 ;
        Crc = SwUartTXData = TxSportData[0] ;
        TxCount = 0 ;
        state = TRANSMIT ;
#ifdef DEBUGASERIAL
        PORTC &= ~1 ;
#endif
        break ;
//#endif // end of Frsky_Port

  case WAITING :       // SPORT ******** we where waiting for some time before listening for an start bit; we can now expect a start bit again
         DISABLE_TIMER_INTERRUPT() ;  // Stop the timer interrupts.
         state = IDLE ;               // Go back to idle.
         PCIFR = ( 1<<PCIF2 ) ;       // clear pending interrupt
         PCICR |= ( 1<<PCIE2 ) ;      // pin change interrupt enabled
         break ;

  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
 } // end sportAvailable == true

#endif                                                     // end of #if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) ) 
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_HUB ) || ( PROTOCOL == FRSKY_SPORT_HUB ) ) // ++++++++ here for Hub protocol ++++++++++++++++++++++++++++++++++

  if (!sportAvailable ) {   // we are handling the Hub protocol
    switch (state) {
  // Transmit Byte.
      case TRANSMIT :   // Hub **** Output the TX buffer. Start bit has already been sent ************ 
#ifdef DEBUGASERIAL
          PORTC |= 1 ;
#endif
          if( SwUartTXBitCount < 8 ) {              // if all bits have not been sent
              if( SwUartTXData & 0x01 ) {             // If the LSB of the TX buffer is 1:
                CLEAR_TX_PIN() ;                        // Send a logic 1 on the TX_PIN.
              } else {                                // Otherwise:
                SET_TX_PIN() ;                          // Send a logic 0 on the TX_PIN.
              }
              SwUartTXData = SwUartTXData >> 1 ;     // Bitshift the TX buffer and
              SwUartTXBitCount += 1 ;                // increment TX bit counter.
          } else {                                 //all 8 bits have been sent so now Send stop bit.
             CLEAR_TX_PIN();                         // Output a logic 1.
             state = TRANSMIT_STOP_BIT;
          }
          OCR1A += TICKS2WAITONEHUB ;  // Count one period into the future.  
#ifdef DEBUGASERIAL
          PORTC &= ~1 ;
#endif
          break ;

  // Go to idle after stop bit was sent.
      case TRANSMIT_STOP_BIT: //  HUB ********************* We have sent a stop bit, now sent eventually next byte from buffer
          if ( ++TxCount < hubMaxData) {          // Have we sent all bytes? if not,
            SwUartTXData = hubData[TxCount] ;        // load next byte     
            SET_TX_PIN() ;                           // Send a logic 0 on the TX_PIN. (= start bit)
            OCR1A = TCNT1 + TICKS2WAITONEHUB - INTERRUPT_BETWEEN_TRANSMIT ;       // Count one period into the future.
            SwUartTXBitCount = 0 ;
            state = TRANSMIT ;
          } else {                                // if all bytes have been send, wait 100usec
            TxCount = 0 ;
            state = WAITING ;
            OCR1A += DELAY_100 ;  // 100uS gap
          }
          break ;           

      case WAITING :    
         DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupts.
         state = IDLE ;                           // Go back to idle.  
         break ;

  // Unknown state.
      default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
      } // End CASE

 } // end  Hub protocol
#endif // end of code for HUB protocol 
} // End of ISR

// End of the code for both Frsky protocols +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//____________________Here the code for SPORT interface only ++++++++++++++++++++++++++++++++++++++++++
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_SPORT ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )  
//brief  Function to initialize the UART for Sport protocol
//  This function will set up pins to transmit and receive on. Control of Timer0 and External interrupt 0.
void initSportUart(  )           //*************** initialise UART pour SPORT
{
/*
#if defined ( SPORT_SENSOR_ID )  && SPORT_SENSOR_ID >= 1 && SPORT_SENSOR_ID <= 28  
    #define ID_MIN_1 (SPORT_SENSOR_ID - 1)
    #define BIT0   ( ID_MIN_1 & 0x01 )
    #define BIT1   (( ID_MIN_1 >> 1) & 0x01 )
    #define BIT2   (( ID_MIN_1 >> 2) & 0x01 )
    #define BIT3   (( ID_MIN_1 >> 3) & 0x01 )
    #define BIT4   (( ID_MIN_1 >> 4) & 0x01 )
    #define BIT5   (BIT0 xor BIT1 xor BIT2) 
    #define BIT6   (BIT2 xor BIT3 xor BIT4) 
    #define BIT7   (BIT0 xor BIT2 xor BIT4) 
    sensorId = ID_MIN_1 | (BIT5 << 5) | (BIT6 << 6) | (BIT7 << 7) ;
#else
  #error "SPORT_SENSOR_ID must be between 1 and 28 (included)"
#endif
*/     
    //PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is tri-stated.

  // External interrupt
  
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;      // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif

    PCIFR = (1<<PCIF2) ;  // clear pending interrupt
    PCICR |= (1<<PCIE2) ; // pin change interrupt enabled

    // Internal State Variable
    state = IDLE ;

#ifdef DEBUGASERIAL
  DDRC = 0x03 ;   // PC0,1 as o/p debug
  PORTC = 0 ;
#endif
}

// ! \brief  External interrupt service routine.  ********************
//  Interrupt on Pin Change to detect change on level on SPORT signal (= could be a start bit)
//
// The falling edge in the beginning of the start
//  bit will trig this interrupt. The state will
//  be changed to RECEIVE, and the timer interrupt
//  will be set to trig one and a half bit period
//  from the falling edge. At that instant the
//  code should sample the first data bit.
//
//  note  initSoftwareUart( void ) must be called in advance.
//
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt
// on this port
//#ifdef FRSKY_SPORT
ISR(PCINT2_vect)
{
  if ( TRXPIN & ( 1 << PIN_SERIALTX ) ) {     // Pin is high = start bit (inverted)
    DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
//PORTC &= ~2 ;
    state = RECEIVE ;                 // Change state
            DISABLE_TIMER_INTERRUPT() ;       // Disable timer to change its registers.
          OCR1A = TCNT1 + TICKS2WAITONE_HALFSPORT - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.
#ifdef DEBUGASERIAL
          PORTC |= 1 ;
#endif
            SwUartRXBitCount = 0 ;            // Clear received bit counter.
            CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
            ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
  }
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif

}

#endif // #end of code for SPORT protocol 




//____________________Here the code for HUB interface only ++++++++++++++++++++++++++++++++++++++++++
#if defined( PROTOCOL ) &&  ( ( PROTOCOL == FRSKY_HUB ) || ( PROTOCOL == FRSKY_SPORT_HUB ) )  
//brief  Function to initialize the UART for Sport protocol
//  This function will set up pins to transmit and receive on. Control of Timer0 and External interrupt 0.
void initHubUart( )
{
//  ThisHubData = pdata ;
  TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is low
  TRXDDR |= ( 1 << PIN_SERIALTX ) ;        // PIN is output.
  
  //Internal State Variable
  state = IDLE ;
//  TxMax = 0 ;
  TxCount = 0 ;

#ifdef DEBUGASERIAL
  DDRC = 0x03 ;   // PC0,1 as o/p debug
  PORTC = 0 ;
#endif
}

void setHubNewData(  )
{
  if ( (TxCount == 0) && (hubMaxData > 0) ) {
//    for (int cntNewData = 0 ; cntNewData < hubMaxData ; cntNewData++) {
//          TxHubData[cntNewData] = hubData[cntNewData] ;
//      }
//      TxMax = hubMaxData  ;
#ifdef DEBUGSETNEWDATA
          Serial.print("set new data at ");
          Serial.print( millis() );
          Serial.print(" ");  
          for (int cntNewData = 0 ; hubMaxData ; cntNewData++) {
            Serial.print( hubData[cntNewData] , HEX );
            Serial.print(" ");
          }
          Serial.println(" ");        
#endif          
      startHubTransmit() ;
  }    
}

void startHubTransmit()
{
  if ( state != IDLE )
  {
    return ;
  }
  SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN (=start bit).
  cli() ;
  OCR1A = TCNT1 + TICKS2WAITONEHUB  - INTERRUPT_ENTRY_TRANSMIT;   // Count one period into the future.
  CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
  sei() ;
  SwUartTXBitCount = 0 ;
  SwUartTXData = hubData[0] ;
  //TxNotEmpty = 0 ;
  state = TRANSMIT ;
  ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#ifdef DEBUGASERIAL
  PORTC &= ~1 ;
#endif

}
// end of function that are hub specific
#endif // end of code for Hub protocol


//********************************** End of code to handle the UART communication with the receiver
#endif   //End of FRSKY protocols

