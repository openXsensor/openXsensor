// file for FRSKY telemetry (SPORT and HUB)

#include "oXs_out_frsky.h"
#define FRSKY  ( defined(MULTIPLEX) || defined ( HOTT) )
#if (FRSKY == false) && defined(FRSKY2) //if Frsky protocol is used


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



extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

//used only by Sport protocol
extern uint8_t  volatile  sportData[7] ;
extern uint8_t volatile sportDataLock ;
extern uint8_t volatile sendStatus ;


//used only by Hub protocol
static int fieldToSend ;
static bool fieldOk ;
extern uint8_t volatile hubData[MAXSIZEBUFFER] ; 
//extern uint8_t volatile hubCurrentData ; //index of current data
extern uint8_t volatile hubMaxData ;   // max number of data prepared to be send


//Used by both protocols
volatile bool sportAvailable = false ;
int fieldContainsData[][5]  = {  SETUP_FRSKY_DATA_TO_SEND } ; // contains the set up of field to be transmitted
int numberOfFields = sizeof(fieldContainsData) / sizeof(fieldContainsData[0]) ;
static uint16_t convertToSportId[15] = { FRSKY_SPORT_ID } ; // this array is used to convert an index inside fieldContainsData[][0] into the SPORT field Id (or defaultfield) 
static uint8_t convertToHubId[15] = { FRSKY_HUB_ID } ; //// this array is used to convert an index inside fieldContainsData[][0] into the Hub field Id (or defaultfield) 
static uint8_t currentFieldToSend = 0 ; 
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

#ifdef FRSKY_TYPE_SPORT
    sportAvailable = true ;     // force the SPORT protocol
#elif defined (FRSKY_TYPE_HUB)
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
    }
    else {
        sportAvailable = true ;
    }
#endif // end test on FRSKY_TYPE
    if ( sportAvailable) {
        initSportUart(  ) ;
    } else {
      	initHubUart( ) ;
    }  
	
#ifdef DEBUG
      printer->print(F("FRSky Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F("Sport protocol= "));
      printer->println(sportAvailable);
      printer->print(F(" milli="));  
      printer->println(millis());
      printer->println(F("FRSky Output Module: Setup!"));
      printer->print(F("Number of fields to send = "));
      printer->println(numberOfFields);
      for (int rowNr = 0 ; rowNr < numberOfFields ; rowNr++) {
          printer->print(fieldContainsData[rowNr][0],HEX); printer->print(F(" , ")); 
          printer->print(fieldContainsData[rowNr][1]);  printer->print(F(" , "));
          printer->print(fieldContainsData[rowNr][2]);  printer->print(F(" , "));
          printer->print(fieldContainsData[rowNr][3]);  printer->print(F(" , "));
          printer->println(fieldContainsData[rowNr][4]);
      }    
#endif
}  // end of setup


void OXS_OUT::sendData()
{
  if (sportAvailable) {
	sendSportData( ) ;
#ifdef GPS_INSTALLED
  FrSkySportSensorGpsSend() ;
#endif  
  } else {
	sendHubData( ) ;
  }
}

//For SPORT protocol
//****************************************************** Look which value can be transmitted and load it in a set of fields used by interrupt routine
    /* We look if a value to send is available ; if available the value is loaded
       We have to take care that all types of value (when available) have to be transmitted after each other
       Note : it should be possible to send more values if OXS would reply to more than one device ID because the polling occurs more often
         Still here we will react only to one device ID (see oXs_config.h)
       We look at the values of each sensor in sequence
       Each value has a extra field "available":
         Available = true = KNOWN when that value is calculated. It becomes false = UNKNOWN when the value is loaded for transmission.; 
       There is one general status related to the transmission. This "sendStatus" is shared by all values
         sendStatus can be  : TO_LOAD, LOADED, SENDING, SEND ; 
             For one value, when the value is available and if sendStatus = TO_LOAD or LOADED, we can load the value (again); 
             It is not allowed to load the value is the sendStatus = SENDING
             sendStatus goes from TO_LOAD to LOADED (in the main loop) when the value is loaded in 'setSportNewData' (in Aserial); The indicator available of this value become false = UNKNOWN in order to avoid to load it again
             sendStatus goes from LOADED to SENDING (in the interrupt) when the start bit is received for the right device ID 
             sendStatus  goes from SENDING to SEND (in the interrupt) when all bytes (including Checksum) have been transmitted
       
       When sendStatus is :
        - "TO_LOAD:
              - If a value is not availvable (UNKNOWN), we try to skip to next available value (in order to send as many data as possible)
              - If a value is availvable (KNOWN), the value is loaded and flagged as UNKNOWN. sendStatus become LOADED
        - "LOADED" :
               - If a value is availvable (KNOWN), the value is loaded and flagged as UNKNOWN. sendStatus remains LOADED
               - If a value is not availvable (UNKNOWN), we keep the already loaded value and sendStatus remains LOADED
        - "SENDING", We just wait that the already value is sent (even if a new value is already available)
         - "SEND" : sendStatus becomes "TO_LOAD" and we skip to the next value to send.
  
   */

void OXS_OUT::sendSportData()
{  
#ifdef DEBUGSENDDATASTATUS
    printer->print("Begining sendData at = ");
    printer->print(millis());
    printer->print(" , sendStatus = ");
    printer->print(sendStatus);
    printer->print("  currentFieldToSend = ");
    printer->print(currentFieldToSend);
    printer->print("  status of currentFieldToSend  = ");
    printer->println(readStatusValue( currentFieldToSend ));
#endif

      
#ifdef DEBUGSENDDATA
   uint8_t oldSendStatus ;
   uint8_t oldSendStatus2 ;
   uint8_t oldCurrentFieldToSend ;
   unsigned long oldMs = millis();
   oldSendStatus = sendStatus ;
   oldCurrentFieldToSend = currentFieldToSend ;
#endif

#ifdef DEBUGTRANSMITDELAY
   static unsigned long lastTransmit = 0 ;
   static unsigned long newTransmit = 0 ;
#endif

#ifdef DEBUGSENDDATADELAY
   static unsigned long milliSendDataBegin ;
   milliSendDataBegin = millis() ;
#endif

#ifdef DEBUGSENDSENSITIVITY
    printer->print(F("Vario1 sensitivityppm = "));
    printer->print(varioData->sensitivityPpm) ;
    printer->print(F(" Vario1 sensitivity = "));
    printer->print(varioData->sensitivity) ;
    printer->print(F(" Vario2 sensitivityppm = "));
    printer->print(varioData_2->sensitivityPpm) ;
    printer->print(F(" Vario2 sensitivity = "));
    printer->println(varioData_2->sensitivity) ;
    

#endif


     switch ( sendStatus )    {
      case SEND :
#ifdef DEBUGTRANSMITDELAY
          if (currentValueType ==4) {
            newTransmit = micros() ;
            printer->print("  End of transmission of data type = ");
            printer->print(currentValueType);
            printer->print("  after a delay of  ");
            printer->println(newTransmit - lastTransmit);
            printer->println(" ") ;
            lastTransmit = newTransmit ;
          }
#endif      
          currentFieldToSend = nextFieldToSend(currentFieldToSend)  ;
          if ( readStatusValue( currentFieldToSend ) == KNOWN ) {
              loadSportValueToSend( currentFieldToSend ) ;
              sendStatus = LOADED ; 
          }
          else {
              sendStatus = TO_LOAD ;
          }	 
          break ;
      case TO_LOAD :       
	  if ( readStatusValue( currentFieldToSend) == KNOWN ) {
              loadSportValueToSend( currentFieldToSend ) ;
              sendStatus = LOADED ; 
          }
          else  {
              currentFieldToSend = nextFieldToSend( currentFieldToSend ) ;
              if ( readStatusValue( currentFieldToSend) == KNOWN ) {
                  loadSportValueToSend( currentFieldToSend ) ;
                  sendStatus = LOADED ; 
              }
              break ;
          }
          break;
     
      case LOADED :
          if ( readStatusValue( currentFieldToSend ) == KNOWN)  {
              loadSportValueToSend( currentFieldToSend ) ;
          }	  
          break;
      
      case SENDING :
      break;
            
      default :
          sendStatus = TO_LOAD ;
#ifdef DEBUGSENDDATA
      printer->println("le sendStatus a une valeur anormale");
#endif  
    } // End of Switch

    
#ifdef DEBUGSENDDATA
    oldSendStatus2 = sendStatus ;
    if (oldSendStatus != sendStatus) { 
      printer->print("End sendData with new sendStatus ;  milli at begin = ");
      printer->print(oldMs);
      printer->print(",  old SendStatus = ");
      printer->print(oldSendStatus);
      printer->print( ", New sendStatus = ");
      printer->print(oldSendStatus2);
      printer->print(" , Old FieldToSend = ");
      printer->print(oldCurrentFieldToSend);
      printer->print(" , New FieldToSend = ");
      printer->print(currentFieldToSend);
      printer->print(" , millis on end = ");
      printer->println(millis());
    } 
#endif

#ifdef DEBUGSENDDATADELAY
      printer->print("sendData, begin at = ");
      printer->print(milliSendDataBegin);
      printer->print(",  end at = ");
      printer->println(millis());
#endif
}


//**************************************************************
// Check if a value is available (return = KNOWN or UNKNOWN) 
// fieldToSend = index of a field 
//**************************************************************
uint8_t OXS_OUT::readStatusValue( uint8_t fieldToSend) {
  switch ( fieldContainsData[fieldToSend][1] )
    {
#ifdef PIN_VOLTAGE
     case  VOLT1 :
          return voltageData->mVoltAvailable[0] ;    
     case  VOLT2 :
          return voltageData->mVoltAvailable[1] ;
     case  VOLT3 :
          return voltageData->mVoltAvailable[2] ;
     case  VOLT4 :
          return voltageData->mVoltAvailable[3] ;
     case  VOLT5 :
          return voltageData->mVoltAvailable[4] ;
     case  VOLT6 :
          return voltageData->mVoltAvailable[5] ;     
#endif          

#ifdef VARIO
      case  ALTIMETER :
          return varioData->absoluteAltAvailable ;
      case  VERTICAL_SPEED :
          return varioData->climbRateAvailable ;
      case  SENSITIVITY :
          return varioData->sensitivityAvailable ;
      case ALT_OVER_10_SEC :
          return varioData->vSpeed10SecAvailable ;
#endif        

#ifdef VARIO2
      case  ALTIMETER_2 :
          return varioData_2->absoluteAltAvailable ;
      case  VERTICAL_SPEED_2 :
          return varioData_2->climbRateAvailable ;
      case  SENSITIVITY_2 :
          return varioData_2->sensitivityAvailable ;
      case ALT_OVER_10_SEC_2 :
          return varioData_2->vSpeed10SecAvailable ;
#endif

#if defined (VARIO)  &&  defined (VARIO2) 
      case  VERTICAL_SPEED_A :
          return averageVSpeedAvailable ; 
#endif

#if defined (VARIO)  &&  defined (USE_6050) 
      case  VERTICAL_SPEED_I :
          return vSpeedImuAvailable ; 
#endif


#if defined (VARIO)  && ( defined (VARIO2)  || defined (AIRSPEED)  || defined(USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY ) && defined (PIN_PPM)
      case  PPM_VSPEED :
          return switchVSpeedAvailable ; 
#endif

#ifdef AIRSPEED
      case AIR_SPEED : 
          return airSpeedData->airSpeedAvailable ;
      case PRANDTL_COMPENSATION : 
          return airSpeedData->compensationAvailable ;    
#endif

#if defined (VARIO) && defined ( AIRSPEED)
      case PRANDTL_DTE :
           return compensatedClimbRateAvailable ;
#endif
          
#ifdef PIN_CURRENTSENSOR
      case CURRENTMA  :
          return  currentData->milliAmpsAvailable ; 
      case MILLIAH  :
          return  currentData->consumedMilliAmpsAvailable ; 
#endif

#if (NUMBEROFCELLS > 0)
      case  CELLS_1_2 :
          return  voltageData->mVoltCell_1_2_Available ; 
      case  CELLS_3_4 :
          return  voltageData->mVoltCell_3_4_Available ; 
      case  CELLS_5_6 :
          return  voltageData->mVoltCell_5_6_Available ; 
#endif

#ifdef PIN_PPM
      case PPM :
        return ppmAvailable ;
#endif


#ifdef MEASURE_RPM
      case RPM :
        return RpmAvailable ;
#endif

      case TEST1 :
        return test1ValueAvailable ;
      case TEST2 :
        return test2ValueAvailable ;
      case TEST3 :
        return test3ValueAvailable ;

      default:
          return  UNKNOWN ;
     }
  return  UNKNOWN ;
}    // End readStatusValue()



//**************************************************************************************
// Search the next (index of) field to transmit 
// 
//**************************************************************************************
uint8_t OXS_OUT::nextFieldToSend(  uint8_t indexField) {
// First we look for a value that is available (KNOWN); search occurs in sequence starting from the first next value
#ifdef DEBUGNEXTVALUETYPE
      printer->print("nextFieldToSend - Original index = ");
      printer->println(indexField);
#endif

  for (int countValueType=numberOfFields ; countValueType>0; countValueType--) {  
    indexField++;
    if ( indexField >= numberOfFields ) { 
        indexField = 0 ;
    }  
#ifdef DEBUGNEXTVALUETYPE
      printer->print("first loop for; next field =");
      printer->println(indexField);
#endif

#ifdef VARIO       
      if ( (fieldContainsData[indexField][1] == ALTIMETER) && ( varioData->absoluteAltAvailable == KNOWN ))  { return indexField ; }        
      else if ( (fieldContainsData[indexField][1] == VERTICAL_SPEED)  && ( varioData->climbRateAvailable == KNOWN ) )  { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == SENSITIVITY)  && ( varioData->sensitivityAvailable == KNOWN ) )  { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == ALT_OVER_10_SEC)  && ( varioData->vSpeed10SecAvailable == KNOWN ) )  { return indexField ; } 
#endif

#ifdef VARIO2       
      if ( (fieldContainsData[indexField][1] == ALTIMETER_2) && ( varioData_2->absoluteAltAvailable == KNOWN ))  { return indexField ; }        
      else if ( (fieldContainsData[indexField][1] == VERTICAL_SPEED_2)  && ( varioData_2->climbRateAvailable == KNOWN ) )  { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == SENSITIVITY_2)  && ( varioData_2->sensitivityAvailable == KNOWN ) )  { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == ALT_OVER_10_SEC_2)  && ( varioData_2->vSpeed10SecAvailable == KNOWN ) )  { return indexField ; } 
#endif

#if defined (VARIO)  &&  defined (VARIO2)
      if ( (fieldContainsData[indexField][1] == VERTICAL_SPEED_A) && ( averageVSpeedAvailable == KNOWN ))  { return indexField ; }        
#endif

#if defined (VARIO)  &&  defined (USE_6050)
      if ( (fieldContainsData[indexField][1] == VERTICAL_SPEED_I) && ( vSpeedImuAvailable == KNOWN ))  { return indexField ; }        
#endif

#if defined (VARIO) && ( defined (VARIO2) || defined (AIRSPEED) || defined(USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY )  && defined (PIN_PPM)
      if  (( fieldContainsData[indexField][1] == PPM_VSPEED)  && (switchVSpeedAvailable == KNOWN ) )  { return indexField ; }    
#endif


#ifdef AIRSPEED       
      if ( (fieldContainsData[indexField][1] == AIR_SPEED) && ( airSpeedData->airSpeedAvailable == KNOWN ))  { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == PRANDTL_COMPENSATION) && ( airSpeedData->compensationAvailable == KNOWN ))  { return indexField ; } 
#endif

#if defined (VARIO) && defined ( AIRSPEED)
      if ( (fieldContainsData[indexField][1] == PRANDTL_DTE) && ( compensatedClimbRateAvailable == KNOWN ))  { return indexField ; } 
#endif

         
#ifdef PIN_VOLTAGE
      if ( (fieldContainsData[indexField][1] == VOLT1) && ( voltageData->mVoltAvailable[0] == KNOWN ) && ( voltageData->mVoltPin[0] < 8 ) ) { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == VOLT2) && ( voltageData->mVoltAvailable[1] == KNOWN ) && ( voltageData->mVoltPin[1] < 8 ) ) { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == VOLT3) && ( voltageData->mVoltAvailable[2] == KNOWN ) && ( voltageData->mVoltPin[2] < 8 ) ) { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == VOLT4) && ( voltageData->mVoltAvailable[3] == KNOWN ) && ( voltageData->mVoltPin[3] < 8 ) ) { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == VOLT5) && ( voltageData->mVoltAvailable[4] == KNOWN ) && ( voltageData->mVoltPin[4] < 8 ) ) { return indexField ; } 
      if ( (fieldContainsData[indexField][1] == VOLT6) && ( voltageData->mVoltAvailable[5] == KNOWN ) && ( voltageData->mVoltPin[5] < 8 ) ) { return indexField ; } 
#endif         

#if defined (PIN_CURRENTSENSOR)
    if ( (fieldContainsData[indexField][1] == CURRENTMA) && ( currentData->milliAmpsAvailable == KNOWN ) ){ return indexField ; }  
    else if ( (fieldContainsData[indexField][1] == MILLIAH ) && ( currentData->consumedMilliAmpsAvailable == KNOWN ) ){ return indexField ; }  
#endif

#if (NUMBEROFCELLS > 0)
      if ( (fieldContainsData[indexField][1] == CELLS_1_2) && ( voltageData->mVoltCell_1_2_Available == KNOWN ) ){ return indexField ; }  
      else if ( (fieldContainsData[indexField][1] == CELLS_3_4) && ( voltageData->mVoltCell_3_4_Available == KNOWN ) ){ return indexField ; }  
      else if ( (fieldContainsData[indexField][1] == CELLS_5_6) && ( voltageData->mVoltCell_5_6_Available == KNOWN ) ){ return indexField ; }  
#endif          

#ifdef PIN_PPM
      if ( (fieldContainsData[indexField][1] == PPM) && ( ppmAvailable == KNOWN ) ){ return indexField ; } 
#endif

#ifdef MEASURE_RPM
      if ( (fieldContainsData[indexField][1] == RPM) && ( RpmAvailable == KNOWN ) ){ return indexField ; } 
#endif
      if ( (fieldContainsData[indexField][1] == TEST1) && ( test1ValueAvailable == KNOWN ) ){ return indexField ; } 
      if ( (fieldContainsData[indexField][1] == TEST2) && ( test2ValueAvailable == KNOWN ) ){ return indexField ; } 
      if ( (fieldContainsData[indexField][1] == TEST3) && ( test3ValueAvailable == KNOWN ) ){ return indexField ; } 
   } // end FOR

//If no one value is available, we select next value type (even if the value is not available)
     indexField++ ;  
    if ( indexField >= numberOfFields ) { 
        indexField = 0 ;
    }
#ifdef DEBUGNEXTVALUETYPE
      printer->print("After FOR; index of next field =");
      printer->println(indexField);
#endif
     
      return indexField ;   
}



//*************************************
// Load the value to transmit in a temp field that will be transmitted when start bit will be recieved
// flag this value as "UNKNOWN" (Unknown)
//************************************
void OXS_OUT::loadSportValueToSend( uint8_t currentFieldToSend) {
  static int32_t valueTemp ;
  int fieldID ;
  fieldID = 0 ;
  valueTemp = 0 ; 
  switch ( fieldContainsData[currentFieldToSend][1] ) {
#ifdef VARIO       
      case  ALTIMETER :
        valueTemp = varioData->absoluteAlt  ;
        fieldID = ALT_FIRST_ID ;
        varioData->absoluteAltAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDALTIMETER
        static unsigned long StartAltimeter2=micros();
        printer->print("++ LoadValueToSend - altimeter is loaded = ");
        printer->print( millis()  );
        printer->print(" value = ");
        printer->print( varioData->absoluteAlt );
        printer->print(" , delay = ");
        printer->println( (micros() - StartAltimeter2 )/1000 );
        StartAltimeter2 = micros() ;
#endif  
        break ;
      case VERTICAL_SPEED : 
         valueTemp = varioData->climbRate ;
         fieldID = VARIO_FIRST_ID ;         
         varioData->climbRateAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDVERTICALSPEED
          static unsigned long StartClimbRate2=micros();
          printer->print("** LoadValueToSend - vertical speed is loaded = ");
          printer->print( millis() );
          printer->print(" value = ");
          printer->print( millis() /10 );
          printer->print(" , delay = ");
          printer->println( (micros() - StartClimbRate2 )/1000 );
          StartClimbRate2 = micros() ;
#endif        
           break ;
         case SENSITIVITY :
             valueTemp = varioData->sensitivity ;
             varioData->sensitivityAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDSENSITIVITY
            static unsigned long StartSensitivity2=micros();
            printer->print("LoadValueToSend - Sensitivity is loaded = ");
            printer->print( millis()  );
            printer->print(" value = ");
            printer->print( varioData->sensitivity );
            printer->print(" , delay = ");
            printer->println( (micros() - StartSensitivity2 )/1000 );
            StartVRef2 = micros() ;
#endif
             break ;
         case ALT_OVER_10_SEC :
             valueTemp = varioData->vSpeed10Sec ;
             varioData->vSpeed10SecAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDALT_OVER_10_SEC
            static unsigned long StartVSpeed10Sec2=micros();
            printer->print("LoadValueToSend - vSpeed10Sec is loaded = ");
            printer->print( millis()  );
            printer->print(" value = ");
            printer->print( varioData->vSpeed10Sec );
            printer->print(" , delay = ");
            printer->println( (micros() - StartVSpeed10Sec2 )/1000 );
            StartVRef2 = micros() ;
#endif
             break ;
             
             
#endif  // End vario    

#ifdef VARIO2       
      case  ALTIMETER_2 :
        valueTemp = varioData_2->absoluteAlt  ;
        fieldID = ALT_FIRST_ID ;
        varioData_2->absoluteAltAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDALTIMETER_2
        static unsigned long StartAltimeter2_2=micros();
        printer->print("++ LoadValueToSend - altimeter is loaded = ");
        printer->print( millis()  );
        printer->print(" value = ");
        printer->print( varioData_2->absoluteAlt );
        printer->print(" , delay = ");
        printer->println( (micros() - StartAltimeter2_2 )/1000 );
        StartAltimeter2 = micros() ;
#endif  
        break ;
      case VERTICAL_SPEED_2 : 
         valueTemp = varioData_2->climbRate ;
         fieldID = VARIO_FIRST_ID ;         
         varioData_2->climbRateAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDVERTICALSPEED_2
          static unsigned long StartClimbRate2_2=micros();
          printer->print("** LoadValueToSend - vertical speed is loaded = ");
          printer->print( millis() );
          printer->print(" value = ");
          printer->print( millis() /10 );
          printer->print(" , delay = ");
          printer->println( (micros() - StartClimbRate2_2 )/1000 );
          StartClimbRate2 = micros() ;
#endif        
           break ;
         case SENSITIVITY_2 :
             valueTemp = varioData_2->sensitivity ;
             varioData_2->sensitivityAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDSENSITIVITY_2
            static unsigned long StartSensitivity2_2=micros();
            printer->print("LoadValueToSend - Sensitivity is loaded = ");
            printer->print( millis()  );
            printer->print(" value = ");
            printer->print( varioData_2->sensitivity );
            printer->print(" , delay = ");
            printer->println( (micros() - StartSensitivity2_2 )/1000 );
            StartVRef2 = micros() ;
#endif
             break ;
         case ALT_OVER_10_SEC_2 :
             valueTemp = varioData_2->vSpeed10Sec ;
             varioData_2->vSpeed10SecAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDALT_OVER_10_SEC_2
            static unsigned long StartVSpeed10Sec2_2=micros();
            printer->print("LoadValueToSend - vSpeed10Sec is loaded = ");
            printer->print( millis()  );
            printer->print(" value = ");
            printer->print( varioData_2->vSpeed10Sec );
            printer->print(" , delay = ");
            printer->println( (micros() - StartVSpeed10Sec2_2 )/1000 );
            StartVRef2 = micros() ;
#endif
             break ;       
#endif  // End vario2    

#if defined (VARIO )  &&  defined (VARIO2)
      case VERTICAL_SPEED_A : 
        valueTemp = averageVSpeed ;
        averageVSpeedAvailable = false ; 
         fieldID = VARIO_FIRST_ID ;         
         break ; 
#endif

#if defined (VARIO )  &&  defined (USE_6050)
      case VERTICAL_SPEED_I : 
        valueTemp = vSpeedImu ;
        vSpeedImuAvailable = false ; 
         fieldID = VARIO_FIRST_ID ;         
         break ; 
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) || defined(USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY )  && defined (PIN_PPM)
      case PPM_VSPEED : 
        valueTemp = switchVSpeed ;
        switchVSpeedAvailable = false ; 
         fieldID = VARIO_FIRST_ID ;         
         break ; 
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED :
        valueTemp = airSpeedData->airSpeed  ;
        fieldID = AIR_SPEED_FIRST_ID ;
        airSpeedData->airSpeedAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDAIRSPEED
        static unsigned long StartAirSpeed=micros();
        printer->print("++ LoadValueToSend - airspeed is loaded = ");
        printer->print( millis()  );
        printer->print(" value = ");
        printer->print( airSpeedData->airSpeed );
        printer->print(" , delay = ");
        printer->println( (micros() - StartAirSpeed )/1000 );
        StartAltimeter2 = micros() ;
#endif  
        break ;
      case  PRANDTL_COMPENSATION :
        valueTemp = airSpeedData->compensation  ;
        fieldID = T1_FIRST_ID ;
        airSpeedData->compensationAvailable = false ;
        break ;
        
#endif  // End airpseed    

#if defined (VARIO) && defined ( AIRSPEED)
      case PRANDTL_DTE :
        valueTemp =  compensatedClimbRate ; 
        fieldID = VARIO_FIRST_ID ; 
        compensatedClimbRateAvailable = false ;
        break ;
#endif  // End defined (VARIO) && defined ( AIRSPEED)




#ifdef PIN_VOLTAGE
      case VOLT1 :  
         valueTemp = voltageData->mVolt[0] ;
         voltageData->mVoltAvailable[0] = false ;
#ifdef DEBUGLOADVALUETOSENDVOLT1
          static unsigned long StartVolt1=micros();
          printer->print("LoadValueToSend - mVolt1 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( voltageData->mVolt[0] );
          printer->print(" , delay = ");
          printer->println( (micros() - StartVolt1 )/1000 );
          StartVolt1 = micros() ;
#endif 
          break ;
      case VOLT2 :  
         valueTemp = voltageData->mVolt[1] ;
         voltageData->mVoltAvailable[1] = false ;
#ifdef DEBUGLOADVALUETOSENDVOLT2
          static unsigned long StartVolt2=micros();
          printer->print("LoadValueToSend - mVolt2 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( voltageData->mVolt[1] );
          printer->print(" , delay = ");
          printer->println( (micros() - StartVolt2 )/1000 );
          StartVolt2 = micros() ;
#endif          
          break ;
      case VOLT3 :  
         valueTemp = voltageData->mVolt[2] ;
         voltageData->mVoltAvailable[2] = false ;
          break ;
      case VOLT4 :  
         valueTemp = voltageData->mVolt[3] ;
         voltageData->mVoltAvailable[3] = false ;
          break ;
      case VOLT5 :  
         valueTemp = voltageData->mVolt[4] ;
         voltageData->mVoltAvailable[4] = false ;
          break ;
      case VOLT6 :  
         valueTemp = voltageData->mVolt[5] ;
         voltageData->mVoltAvailable[5] = false ;
          break ;
#endif

#if defined (PIN_CURRENTSENSOR)
      case CURRENTMA :
         valueTemp = currentData->milliAmps ;
         fieldID = CURR_FIRST_ID ;         
         currentData->milliAmpsAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDCURRENTMA
          static unsigned long StartCurrentMa=micros();
          printer->print("LoadValueToSend CurrentMa at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( currentData->milliAmps );
          printer->print(" , delay = ");
          printer->println( (micros() - StartCurrentMa )/1000 );
          StartCurrentMa = micros() ;
#endif          
         break ;
      case MILLIAH :
         valueTemp = currentData->consumedMilliAmps ;
         currentData->consumedMilliAmpsAvailable = false ;
#ifdef DEBUGLOADVALUETOSENDMILLIAH
          static unsigned long StartMilliAh=micros();
          printer->print("LoadValueToSend CurrentMa at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( currentData->consumedMilliAmps  );
          printer->print(" , delay = ");
          printer->println( (micros() - StartMilliAh )/1000 );
          StartMilliAh = micros() ;
#endif          
         
         break ;
#endif

#if (NUMBEROFCELLS > 0)
      case  CELLS_1_2 :
          valueTemp =  voltageData->mVoltCell_1_2  ; 
          fieldID = CELLS_FIRST_ID ; 
          voltageData->mVoltCell_1_2_Available  = false ;
#ifdef DEBUGLOADVALUETOSENDCELL_1_2
          static unsigned long StartCell_1_2=micros();
          printer->print("LoadValueToSend Cell_1_2 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (voltageData->mVoltCell_1_2 & 0xFFF00000) >> 20  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_1_2 & 0x000FFF00) >> 8  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_1_2 & 0x000000FF) , HEX );
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_1_2 )/1000 );
          StartCell_1_2 = micros() ;
#endif                    
          break ;
          
      case  CELLS_3_4 :
          valueTemp = voltageData->mVoltCell_3_4 ; 
          fieldID = CELLS_FIRST_ID ; 
          voltageData->mVoltCell_3_4_Available  = false ;     
#ifdef DEBUGLOADVALUETOSENDCELL_3_4
          static unsigned long StartCell_3_4=micros();
          printer->print("LoadValueToSend Cell_3_4 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (voltageData->mVoltCell_3_4 & 0xFFF00000) >> 20  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_3_4 & 0x000FFF00) >> 8  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_3_4 & 0x000000FF) , HEX );
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_3_4 )/1000 );
          StartCell_3_4 = micros() ;
#endif                    
          break ;
          
      case  CELLS_5_6 :
          valueTemp = voltageData->mVoltCell_5_6 ;
          fieldID = CELLS_FIRST_ID ;  
          voltageData->mVoltCell_5_6_Available  = false ; 
#ifdef DEBUGLOADVALUETOSENDCELL_5_6
          static unsigned long StartCell_5_6=micros();
          printer->print("LoadValueToSend Cell_5_6 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (voltageData->mVoltCell_5_6 & 0xFFF00000 >> 20)  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_5_6 & 0x000FFF00 >> 8)  );
          printer->print(" ");
          printer->print( (voltageData->mVoltCell_5_6 & 0x000000FF) , HEX );
          
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_5_6 )/1000 );
          StartCell_5_6 = micros() ;
#endif                    
          break ;

#endif  // NUMBEROFCELLS > 0 

#ifdef PIN_PPM 
      case  PPM :
          valueTemp = ppm ;
          fieldID = T1_FIRST_ID ;  
          ppmAvailable  = false ; 
          break ;
#endif


#ifdef MEASURE_RPM 
      case  RPM :
          valueTemp = RpmValue ;
          fieldID = RPM_FIRST_ID ;  
          RpmAvailable  = false ;
          break ;   
#endif
      case  TEST1 :
          valueTemp = test1Value ;
          test1ValueAvailable  = false ; 
          break ;
      case  TEST2 :
          valueTemp = test2Value ;
          test2ValueAvailable  = false ; 
          break ;
      case  TEST3 :
          valueTemp = test3Value ;
          test3ValueAvailable  = false ; 
          break ;



      }  // end Switch
      if ( (fieldContainsData[currentFieldToSend][0] != DEFAULTFIELD)  ) fieldID = convertToSportId[ fieldContainsData[currentFieldToSend][0]] ;
      if ( (fieldID >= VFAS_FIRST_ID) && (fieldID <= VFAS_LAST_ID) ) valueTemp = valueTemp / 10 ;
#ifdef DEBUGWITHFIXVALUE
      static int delta = 1 ;
      static int32_t prevValue = 0 ;
      static int32_t maxValue = 255;
      static int32_t minValue = -255;
      if (prevValue <  minValue || prevValue >  maxValue) delta = delta * -1 ;
      prevValue  = prevValue + delta ;
      valueTemp = prevValue ; 
#endif //End DEBUGWITHFIXVALUE
//      setSportNewData( &sportData, fieldID  ,  (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4] ) ; 
      setSportNewData( fieldID  ,  (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4] ) ; 
#ifdef DEBUGLOADVALUETOSEND
          printer->print("Loaded at = ");
          printer->print( millis() );
          printer->print(" for field index = ");
          printer->print( currentFieldToSend  );
          printer->print(" Device ID (hex) = ");
          printer->print( fieldID  , HEX );
          printer->print(" , value= ");
          printer->print( (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) + fieldContainsData[currentFieldToSend][4] );
          printer->print(" , Hex value= ");
          printer->println( (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) + fieldContainsData[currentFieldToSend][4] , HEX );

#endif
}  // End function



#ifdef GPS_INSTALLED
 //!! shared with Aserial
extern uint8_t volatile gpsSendStatus ; 
extern uint8_t volatile gpsSportDataLock ;
extern uint8_t volatile gpsSportData[7] ;
#define GPS_DATA_COUNT 5

void OXS_OUT::FrSkySportSensorGpsSend(void)
{
  // gpsSendStatus can be TO_LOAD, LOADED, SENDING, SEND ; it is managed here and in Aserial
  // new data is uploaded only when gpsSendStatus == SEND or TO_LOAD
  // each GPS data is loaded in sequence but only if available (otherwise this data is skipped)
  static uint8_t gpsDataIdx ;
  static uint16_t gpsSportId ;
  static uint32_t gpsSportValue ;
  static uint32_t gpsLastLoadedMillis ;
#ifdef DEBUGSIMULATEGPS
  static uint8_t gpsSimulateCount ;
#endif  
//   Serial.println(F("S gdps"));

  if  ((gpsSendStatus == SEND || gpsSendStatus == TO_LOAD) && (millis() - gpsLastLoadedMillis > 200 ) ){  // send only one data per 200 msec (to test if it help locking found on the Tx log)
            gpsDataIdx++;  // handle next GPS data; if not available, this field will be skipped.
            if(gpsDataIdx >= GPS_DATA_COUNT) {
              gpsDataIdx = 0;
            }
            switch(gpsDataIdx)
            {
              case 0: //longitude
                if (!GPS_lonAvailable) return ;
                GPS_lonAvailable = false ;
                gpsSportId = GPS_LONG_LATI_FIRST_ID ;
#ifdef DEBUGSIMULATEGPS
                gpsSportValue = ((( ((((uint32_t)( GPS_lon < 0 ? -GPS_lon : GPS_lon)) / 10 ) * 6 ) / 10 ) + gpsSimulateCount++ )& 0x3FFFFFFF) | 0x80000000;
#else                
                gpsSportValue = (( ((((uint32_t)( GPS_lon < 0 ? -GPS_lon : GPS_lon)) /10 ) * 6 ) / 10 ) & 0x3FFFFFFF)  | 0x80000000;
#endif                
                if(GPS_lon < 0) gpsSportValue |= 0x40000000;
                break;
              case 1: //latitude
                if (!GPS_latAvailable) return ;
                GPS_latAvailable = false ;
                gpsSportId = GPS_LONG_LATI_FIRST_ID ;
                gpsSportValue = ((  ((((uint32_t)( GPS_lat < 0 ? -GPS_lat : GPS_lat)) / 10 ) * 6 )/ 10 ) & 0x3FFFFFFF ) ;
                if(GPS_lat < 0) gpsSportValue |= 0x40000000;
                break;
              case 2: // GPS_altitude
                if (!GPS_altitudeAvailable) return ;
                GPS_altitudeAvailable = false ;
                gpsSportId = GPS_ALT_FIRST_ID ;
#ifdef DEBUGSIMULATEGPS
                gpsSportValue = (GPS_altitude / 10) + gpsSimulateCount++; // convert mm in cm 
#else                
                gpsSportValue = GPS_altitude / 10; // convert mm in cm 
#endif                
                break;
              case 3: // GPS_speed_3d  // could be 2D
#ifdef GPS_SPEED_3D
                if (!GPS_speed_3dAvailable) return ; 
                GPS_speed_3dAvailable = false ;
                gpsSportId = GPS_SPEED_FIRST_ID ;
#ifdef GPS_SPEED_IN_KMH
                gpsSportValue = ( ((uint32_t) GPS_speed_3d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
#else                                
                gpsSportValue = ( ((uint32_t) GPS_speed_3d) * 700 ) / 36  ; // convert cm/s in 1/100 of knots (factor = 19.44)
#endif // end of GPS_SPEED_IN_KMH
                break;
#else                   // use gps_Speed_2d
                if (!GPS_speed_2dAvailable) return ; 
                GPS_speed_2dAvailable = false ;
                gpsSportId = GPS_SPEED_FIRST_ID ;
#ifdef GPS_SPEED_IN_KMH
                gpsSportValue = ( ((uint32_t) GPS_speed_2d) * 36 )  ; // convert cm/s in 1/100 of km/h (factor = 3.6)
#else                                
                gpsSportValue = ( ((uint32_t) GPS_speed_2d) * 700 ) / 36 ; // convert cm/s in 1/1000 of knots (factor = 19.44)
                Serial.print(F("2d Knot:"));Serial.println(gpsSportValue); 
#endif // end of GPS_SPEED_IN_KMH
                break;
#endif //  enf of GPS_SPEED_3D             
              case 4: //GPS_ground_courseAvailable
                if (!GPS_ground_courseAvailable) return ;
                GPS_ground_courseAvailable = false ;
                gpsSportId = GPS_COURS_FIRST_ID ;
                gpsSportValue = GPS_ground_course / 1000; // convert from degree * 100000 to degree * 100 
                break;
              default:
                return ;
            } // end case    
            gpsSportDataLock = 1 ;
            gpsSportData[0] = 0x10 ;
            gpsSportData[1] = gpsSportId ; // low byte
            gpsSportData[2] = gpsSportId >> 8 ; // hight byte
            gpsSportData[3] = gpsSportValue ;
            gpsSportData[4] = gpsSportValue >> 8 ;
            gpsSportData[5] = gpsSportValue >> 16 ;
            gpsSportData[6] = gpsSportValue >> 24 ;
            gpsSportDataLock = 0 ;
#ifdef DEBUGSENDGPS
  Serial.print(F("ID: "));
  Serial.println(gpsSportId , HEX);
#endif

            gpsSendStatus = LOADED ; // from here data can be sent by the interrupt in Aserial
  } // end test on gpsSendStatus == SEND or TOLOAD          
} // end function

#endif // of of GPS_INSTALLED


// -------------------------End of SPORT protocol--------------------------------------------------------------------------------------

//========================= Hub protocol ==========================================
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
//    static byte SwitchFrameVariant=0;
    lastMsFrame1=temp;
    SendFrame1();
//    if (SwitchFrameVariant==0)SendFrame1A();
//    if (SwitchFrameVariant==1)SendFrame1B();
//    SwitchFrameVariant++;
//    if(SwitchFrameVariant==2)SwitchFrameVariant=0 ;
  }
//  second frame was never used; if activated again, then we have to take care that orginal data are already sent before filling the buffer
#ifdef GPS_INSTALLED
  if ( (state == IDLE ) && (temp-lastMsFrame2) > INTERVAL_FRAME2  ) {
    lastMsFrame2=temp;
    SendFrame2();
  }
#endif
}  // end sendData Hub protocol

//======================================================================================================Send Frame 1A via serial
void OXS_OUT::SendFrame1(){
#ifdef DEBUGHUBPROTOCOL
  printer->print("FRSky output module: SendFrame1A:");
#endif
    hubMaxData = 0 ; // reset of number of data to send
#ifdef SEND_FixValue
   SendValue(FRSKY_USERDATA_TEMP1,(int16_t)1234); // Fix value in T1 ; only for test purpose
#endif
  for (int rowNr = 0 ; rowNr < numberOfFields ; rowNr++) {
    if ( hubMaxData < (MAXSIZEBUFFER - 13) ){
        loadHubValueToSend( rowNr ) ;    
    }    
  }    
  if( hubMaxData > 0 ) {
    sendHubByte(0x5E) ; // End of Frame 1!
    setHubNewData(  ) ;
  }  
#ifdef DEBUGHUBPROTOCOL
      printer->print("Data to send = ");
      for (int cntPrint = 0 ; cntPrint < hubData.maxData ; cntPrint++) {
        printer->print(" ");
        printer->print(hubData.data[cntPrint] , HEX);
      }
     printer->println(" "); 
#endif
  
}


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
#ifdef DEBUGHUBPROTOCOL
  printer->print(F("FRSky output module: SendFrame2:"));
#endif
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
  SendValue(FRSKY_USERDATA_GPS_LAT_A , (uint16_t) (( minWith7Decimals % 10000000L) / 1000 ) ) ; // Latitude (.MMMM)
  SendValue(FRSKY_USERDATA_GPS_LAT_EW , (uint16_t)(GPS_lat < 0 ? 'S' : 'N')) ;
  absLongLat = abs(GPS_lon) ;
  decimalPartOfDegree = (absLongLat % 10000000 );
  minWith7Decimals = decimalPartOfDegree * 60 ;
  SendValue(FRSKY_USERDATA_GPS_LONG_B , (uint16_t) (((absLongLat / 10000000L) * 100 ) +  (minWith7Decimals / 10000000L )) ) ; // Longitude (DDMM)
  SendValue(FRSKY_USERDATA_GPS_LONG_A , (uint16_t) (( minWith7Decimals % 10000000L) / 1000 ) ) ;   // Longitude (.MMMM)
  SendValue(FRSKY_USERDATA_GPS_LONG_EW , (uint16_t)(GPS_lon < 0 ? 'W' : 'E')) ;
  SendValue(FRSKY_USERDATA_GPS_ALT_B ,  (int16_t) GPS_altitude / 1000 ); // Altitude m
  SendValue(FRSKY_USERDATA_GPS_ALT_A , (uint16_t) ( (abs(GPS_altitude) % 1000 ) / 10 ) ) ; // Altitude in centimeter
  uint32_t GPSSpeedKnot = GPS_speed_3d * 1944 ; // speed in knots with 5 décimals (1 cm/sec = 0,0194384 knot)
  SendValue(FRSKY_USERDATA_GPS_SPEED_B , (uint16_t) ( GPSSpeedKnot / 100000) ) ;  // Speed knots
  SendValue(FRSKY_USERDATA_GPS_SPEED_A , (uint16_t) ( (GPSSpeedKnot % 100000 ) /1000) ) ; // Speed 2 decimals of knots
  SendValue(FRSKY_USERDATA_GPS_CURSE_B , (uint16_t) ( GPS_ground_course / 100000 ) ) ;  // Course degrees
  SendValue(FRSKY_USERDATA_GPS_CURSE_A , (uint16_t) ( (GPS_ground_course % 100000) / 1000 ) ) ;   // // Course 2 decimals of degrees
  if( hubMaxData > 0 ) {
    sendHubByte(0x5E) ; // End of Frame 2!
    setHubNewData(  ) ;
  }  
#ifdef DEBUGHUBPROTOCOL
      printer->print("Data to send = ");
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


//***************************************************
// Search the value, format it and put it in a buffer
//***************************************************
void OXS_OUT::loadHubValueToSend( uint8_t currentFieldToSend ) {
//  static int16_t valueTemp ;
//  static int fieldToSend ;
//  static bool fieldOk ;
//  valueTemp = 0 ; 
  fieldToSend = (int) convertToHubId[ fieldContainsData[currentFieldToSend][0] ]  ;
  if ( (fieldToSend >= FRSKY_USERDATA_GPS_ALT_B ) && (fieldToSend <= FRSKY_USERDATA_FUELPERCENT ) )  fieldOk = true ;
  else fieldOk = false ;
  switch ( fieldContainsData[currentFieldToSend][1] ) {

#ifdef VARIO       
      case  ALTIMETER :    
//          if ( (SwitchFrameVariant == 0) && (varioData->absoluteAltAvailable) ) { //========================================================================== Vario Data
              if (fieldToSend == DEFAULTFIELD) {
                  SendAlt( ( (varioData->absoluteAlt * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );           
//                varioData->absoluteAltAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData->absoluteAlt * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                 varioData->absoluteAltAvailable = false ;
              }   
//          }
          break ;
      case VERTICAL_SPEED : 
//          if ( (SwitchFrameVariant == 0) && (varioData->climbRateAvailable) ){
              if (fieldToSend == DEFAULTFIELD) {
                 // Attempt to work around the annoying 10cm double beep
                //SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)varioData->climbRate); // ClimbRate in open9x Vario mode
                if (varioData->climbRate==10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)9); // ClimbRate in open9x Vario mode
                else if (varioData->climbRate==-10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)-9);
                else SendValue(FRSKY_USERDATA_VERT_SPEED,( ( (int16_t) varioData->climbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] ); // ClimbRate in open9x Vario mode
//                varioData->climbRateAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ((int16_t)varioData->climbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
                 varioData->climbRateAvailable = false ;
              }  
//          }   
          break ;
       case SENSITIVITY :
//          if ( (SwitchFrameVariant == 0) && (varioData->sensitivityAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend , ( ((int16_t) varioData->sensitivity * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
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
                SendAlt( ( (varioData_2->absoluteAlt * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4]);           
//                varioData_2->absoluteAltAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData_2->absoluteAlt * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
//                 varioData_2->absoluteAltAvailable = false ;
              }   
//          }
          break ;
      case VERTICAL_SPEED_2 : 
//          if ( (SwitchFrameVariant == 0) && (varioData_2->climbRateAvailable) ){
              if (fieldToSend == DEFAULTFIELD) {
                 // Attempt to work around the annoying 10cm double beep
                //SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)varioData->climbRate); // ClimbRate in open9x Vario mode
                if (varioData_2->climbRate==10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)9); // ClimbRate in open9x Vario mode
                else if (varioData_2->climbRate==-10) SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)-9);
                else SendValue(FRSKY_USERDATA_VERT_SPEED, ( ( (int16_t) varioData_2->climbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] ); // ClimbRate in open9x Vario mode
//                varioData_2->climbRateAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) varioData_2->climbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//                 varioData_2->climbRateAvailable = false ;
              }  
//          }   
          break ;
       case SENSITIVITY_2 :
//          if ( (SwitchFrameVariant == 0) && (varioData_2->sensitivityAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend , ( ( (int16_t) varioData_2->sensitivity * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
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
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) averageVSpeed * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }   
          break ;   
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) || defined(USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY ) && defined (PIN_PPM)
      case PPM_VSPEED :
              if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) switchVSpeed * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
              }   
          break ;   
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED : 
              if (fieldToSend == DEFAULTFIELD) {
                 SendGPSSpeed(  (( (int16_t) airSpeedData->airSpeed * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4]) ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) airSpeedData->airSpeed * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
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
                 SendValue((int8_t) fieldToSend , ( ((int16_t) compensatedClimbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
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
                 SendCurrentMilliAmps(( ( (int16_t) currentData->milliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4]);
//                 currentData->milliAmpsAvailable = false ;
             }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend , ( ( (int16_t) currentData->milliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
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
                 SendCellVoltage( voltageData->mVoltCell_1_2 ) ;
//                 voltageData->mVoltCell_1_2_Available  = false ;
             }
//          }    
          break ;
          
      case  CELLS_3_4 :
//          if ( (SwitchFrameVariant == 0) && ( voltageData->mVoltCell_3_4_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( voltageData->mVoltCell_3_4 ) ;
//                 voltageData->mVoltCell_3_4_Available  = false ;
             }
//          }   
          break ;
          
      case  CELLS_5_6 :
//          if ( (SwitchFrameVariant == 0) && ( voltageData->mVoltCell_5_6_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( voltageData->mVoltCell_5_6 ) ;
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
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test1Value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;
      case  TEST2 :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test2Value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;
      case  TEST3 :
            if(  fieldOk == true ) {
                   SendValue((int8_t) fieldToSend ,(int16_t) ( (test3Value * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])) + fieldContainsData[currentFieldToSend][4] );
             }
          break ;


      }  // end Switch
}  // End function  loadValueToSend (Frame 1)


#ifdef PIN_VOLTAGE
// ********************************************************** //
//  SendVoltX => send a voltage                              //
// ********************************************************* //
void OXS_OUT::SendVoltX( uint8_t VoltToSend , uint8_t indexFieldToSend) {
//        if ( (SwitchFrameVariant == 0) && (  voltageData->mVoltAvailable[VoltToSend] == true )) {
           if ( fieldOk == true ) {
             SendValue((int8_t) fieldToSend ,(int16_t) ( ( voltageData->mVolt[VoltToSend] * fieldContainsData[indexFieldToSend][2] / fieldContainsData[indexFieldToSend][3])) + fieldContainsData[indexFieldToSend][4] );
//             voltageData->mVoltAvailable[VoltToSend] = false ;
           }
//         }
}

#if defined (NUMBEROFCELLS)  && (NUMBEROFCELLS > 0)
// ********************************************************** //
// SendCellVoltage => send a cell voltage                     //
// ********************************************************** //
void OXS_OUT::SendCellVoltage( uint32_t voltage) {
  static byte cellID ;
    static uint16_t cellVolt;
  // For SPORT, cell voltage is formatted as (hex) 12 34 56 78 where 123 = volt of cell n+1 (divided by 2), 456 idem for cell n, 7 = max number of cell and 8 = n (number of cell)
  // target format for Hub (hex) is having 2 data sent in format : 84 56 and 91 23 (where 9 = content of 8 incresed by 1)
    cellID = (voltage & 0x0000000f);
    cellVolt = ((voltage >> 8) & 0x0fff) ;
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
#endif // end cell
#endif // enf of 6 voltage

// ********************************** //
//  SendGPSDist => send 0..32768      //
// ********************************** //
void OXS_OUT::SendGPSDist(uint16_t dist) {// ==> Field "Dist" in open9x
  SendValue(0x3C,uint16_t(dist)); //>> DIST
}

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


// ********************************** Here the code to handle the UART communication with the receiver
#ifdef DEBUG
//#define DEBUGSETNEWDATA
//#define DEBUGASERIAL
#endif

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
#define GPS_SENSOR_ID 0x83  // Id normally used by Frsky for Gps.

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
//  Timer1 will ensure that bits are written and
//  read at the correct instants in time.
//  The state variable will ensure context
//  switching between transmit and recieve.
//  If state should be something else, the
//  variable is set to IDLE. IDLE is regarded
//  as a safe state/mode.

//For Frsky only
uint8_t ByteStuffByte = 0 ;

// initially only for Hub
//volatile uint8_t TxHubData[] ;
//volatile uint8_t TxMax ;
//struct t_hubData * volatile ThisHubData = 0 ;
uint8_t volatile hubData[MAXSIZEBUFFER] ; 
uint8_t volatile hubMaxData ;   // max number of data prepared to be send

// initially only for Sport
uint8_t LastRx ;
uint8_t TxSportData[7] ;
uint16_t Crc ;
//uint8_t DataSent ;
//uint8_t SportSync ;
//struct t_sportData * volatile FirstData = 0 ;
//struct t_sportData * volatile ThisSportData = 0 ;
uint8_t  volatile  sportData[7] ;
uint8_t volatile sportDataLock ;
uint8_t volatile sendStatus ;
uint8_t volatile gpsSendStatus ; 
uint8_t volatile gpsSportDataLock ;
uint8_t volatile gpsSportData[7] ;
uint8_t currentSensorId ; // save the sensor id being received and on which oXs will reply (can be the main sensor id or GPS sensor id) 


ISR(TIMER1_COMPA_vect)
{
  if ( sportAvailable ) {    // ++++++++ here only for SPORT protocol ++++++++++++++++++++++++++++++++++
  switch (state)
  {
  // Transmit Byte.
    case TRANSMIT :   // Output the TX buffer.************ on envoie des bits de data
#if DEBUGASERIAL
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
  
  /*              
              // Here another code in order to avoid going out of interrupt during transmission
                
                while ( SwUartTXBitCount < 8) {
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
                  OCR1A += TICKS2WAITONESPORT ;  // Count one period into the future.
                  //digitalWrite(PIN_LED, HIGH );
                  do { }
                    while ( !(TIFR1 & (1 << OCF1A) ) ) ; 
                  CLEAR_TIMER_INTERRUPT( ) ; 
                  //digitalWrite(PIN_LED, LOW );
                  
                  
              } // end while 8 bits have been sent
                              
                CLEAR_TX_PIN();                         // Output a logic 1.
              state = TRANSMIT_STOP_BIT;
    //ENABLE_TIMER0_INT() ;                 // Allow this in now.
              OCR1A += TICKS2WAITONESPORT ;  // Count one period into the future.
                CLEAR_TIMER_INTERRUPT( ) ;
*/
#if DEBUGASERIAL
          PORTC &= ~1 ;
#endif
          break ;

  // Go to idle after stop bit was sent.
          case TRANSMIT_STOP_BIT: //************************************* We send a stop bit
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
                  SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
                  OCR1A = TCNT1 + TICKS2WAITONESPORT ;   // Count one period into the future.
                  SwUartTXBitCount = 0 ;
                  state = TRANSMIT ;
                  //DISABLE_TIMER0_INT() ;      // For the byte duration
                }
                else  // 8 bytes have been send
                {
#ifdef GPS_INSTALLED  // fill status depending on sensorId being handled
                  if ( currentSensorId == GPS_SENSOR_ID ) {
                    gpsSendStatus = SEND ;
                  } else {
                    sendStatus = SEND ;
                  }
#else
                  sendStatus = SEND ;
#endif
                  state = WAITING ;
                  OCR1A += DELAY_3500 ;   // 3.5mS gap before listening
                  TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
                  TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
                        
//                  struct t_sportData * volatile pdata = ThisSportData ;
//                  FORCE_INDIRECT( pdata ) ;
            
//                  pdata->serialSent = 1 ;
//                  DataSent = 1 ;
//                  SportSync = 1 ;
//                  pdata = pdata->next ;
//                  if ( pdata == 0 )                  // Wrap at end
//                    {
//                  pdata = FirstData ;
//                    }
//                  ThisSportData = pdata ;
                }

               break ;
               
//#ifdef FRSKY_SPORT
     case RECEIVE :  // Start bit has been received and we will read bits of data      
            OCR1A += TICKS2WAITONESPORT ;                    // Count one period after the falling edge is trigged.
            //Receiving, LSB first.
            {
                    uint8_t data ;        // Use a temporary local storage
                    data = SwUartRXBitCount ;
                    if( data < 8 )                          // If 8 bits are not yet read
                    {
                        SwUartRXBitCount = data + 1 ;
                        data = SwUartRXData ;
                        data >>= 1 ;            // Shift due to receiving LSB first.
#if DEBUGASERIAL
                        PORTC &= ~1 ;
#endif
                        if( GET_RX_PIN( ) == 0 )
                        {
                            data |= 0x80 ;                    // If a logical 1 is read, let the data mirror this.
                        }
#if DEBUGASERIAL
                        PORTC |= 1 ;
#endif
                        SwUartRXData = data ;
                    }
                    else  //Done receiving =  8 bits are in SwUartRXData
                    {
#if DEBUGASERIAL
                    PORTC &= ~1 ;
#endif
                    if ( LastRx == 0x7E )
                    {
                        if ( SwUartRXData == sensorId )
                        {
//                          struct t_sportData * volatile pdata = ThisSportData ;
//                          FORCE_INDIRECT( pdata ) ;
                          //if ( pdata )               // We have something to send
                          if   ( sendStatus == LOADED ){     
                            if ( sportDataLock == 0 ) {
/*          This part can help debug the transmission of a value,      Mike it could be set in a #ifdef
                                switch (idToSend) { 
                                     case VARIO_ID :
                                       idToSend = DIVIDER_VOLTAGE_ID ;
                                       break ;
                                     case DIVIDER_VOLTAGE_ID :
                                       idToSend = VCC_ID ;
                                       break ;
                                      case VCC_ID :
                                       idToSend =  ALT_ID ;
                                       break ;
                                      default :
                                       idToSend = VARIO_ID ;
                                 } 
                                
                                idToSend = DIVIDER_VOLTAGE_ID ;

                                TxData[0] = 0x10 ;
              TxData[1] = idToSend ; // partie basse de l ID pour une valeur
        TxData[2] = idToSend >> 8 ;
        TxData[3] = TxData[3] + 1 ;
        TxData[4] = 0 ;
        TxData[5] = 0 ;
        TxData[6] = 0 ;

 */                               
                                TxSportData[0] = sportData[0] ;
                                TxSportData[1] = sportData[1] ;
                                TxSportData[2] = sportData[2] ;
                                TxSportData[3] = sportData[3] ;
                                TxSportData[4] = sportData[4] ;
                                TxSportData[5] = sportData[5] ;
                                TxSportData[6] = sportData[6] ;
                                                        
                            }
                            else // locked
                            { // Discard frame to be sent if data is locked
                                TxSportData[0] = 0 ;
                                TxSportData[1] = 0 ;
                                TxSportData[2] = 0 ;
                                TxSportData[3] = 0 ;
                                TxSportData[4] = 0 ;
                                TxSportData[5] = 0 ;
                                TxSportData[6] = 0 ;
                            }
                            state = TxPENDING ;
                            sendStatus = SENDING ;
                            currentSensorId = SwUartRXData ; // save the sensorId being handled (can be the main one or the code for GPS).
                            OCR1A += ( DELAY_400 - TICKS2WAITONESPORT) ;    // 400 uS gap before sending
                         } // end LOADED
                        else // No data are loaded (so there is no data yet available)
                        {
                          // Wait for idle time
                          state = WAITING ;
                          OCR1A += DELAY_3500 ;   // 3.5mS gap before listening
                        }
                      } 
                      else if ( SwUartRXData == GPS_SENSOR_ID ) {
                          if   ( gpsSendStatus == LOADED ){     
                            if ( gpsSportDataLock == 0 ) {
                                TxSportData[0] = gpsSportData[0] ;
                                TxSportData[1] = gpsSportData[1] ;
                                TxSportData[2] = gpsSportData[2] ;
                                TxSportData[3] = gpsSportData[3] ;
                                TxSportData[4] = gpsSportData[4] ;
                                TxSportData[5] = gpsSportData[5] ;
                                TxSportData[6] = gpsSportData[6] ;
                             }
                            else // locked
                            { // Discard frame to be sent if data is locked
                                TxSportData[0] = 0 ;
                                TxSportData[1] = 0 ;
                                TxSportData[2] = 0 ;
                                TxSportData[3] = 0 ;
                                TxSportData[4] = 0 ;
                                TxSportData[5] = 0 ;
                                TxSportData[6] = 0 ;
                            }
                            state = TxPENDING ;
                            gpsSendStatus = SENDING ;
                            currentSensorId = SwUartRXData ; // save the sensorId being handled (can be the main one or the code for GPS).
                            OCR1A += ( DELAY_400 - TICKS2WAITONESPORT) ;    // 400 uS gap before sending
                          } // end LOADED 
                      } // end GPS_SENSOR_ID
                      else // polling on a sensorId that does not request a reply
                      {
                          state = WAITING ;
                          OCR1A += DELAY_3500 ;   // 3.5mS gap before listening
                      }
                  }    // received 1 byte and was equal to 0x7E
                  else // Previous code is not equal to x7E 
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
  
  case TxPENDING :
#if DEBUGASERIAL
      PORTC |= 1 ;
#endif
      TRXDDR |= ( 1 << PIN_SERIALTX ) ;       // PIN is output
            SET_TX_PIN() ;                          // Send a logic 0 on the TX_PIN.
      OCR1A = TCNT1 + TICKS2WAITONESPORT ;         // Count one period into the future.
      SwUartTXBitCount = 0 ;
      Crc = SwUartTXData = TxSportData[0] ;
      TxCount = 0 ;
      state = TRANSMIT ;
      //DISABLE_TIMER0_INT() ;        // For the byte duration
#if DEBUGASERIAL
      PORTC &= ~1 ;
#endif
        break ;
//#endif // end of Frsky_Port

  case WAITING :
//#ifdef FRSKY_SPORT
             DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupts.
         state = IDLE ;                           // Go back to idle.
         PCIFR = ( 1<<PCIF2 ) ;     // clear pending interrupt
         PCICR |= ( 1<<PCIE2 ) ;      // pin change interrupt enabled
                break ;

  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
 } // end sportAvailable == true
 else { //  ++++++++ here only for Hub protocol ++++++++++++++++++++++++++++++++++
 //  ***********************************************  Hub protocol *****************************
  switch (state)
  {
  // Transmit Byte.
    case TRANSMIT :   // Output the TX buffer.************ on envoie des bits de data
#if DEBUGASERIAL
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
          OCR1A += TICKS2WAITONEHUB ;  // Count one period into the future.
  
  /*              
              // Here another code in order to avoid going out of interrupt during transmission
                
                while ( SwUartTXBitCount < 8) {
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
                  OCR1A += TICKS2WAITONEHUB ;  // Count one period into the future.
                  //digitalWrite(PIN_LED, HIGH );
                  do { }
                    while ( !(TIFR1 & (1 << OCF1A) ) ) ; 
                  CLEAR_TIMER_INTERRUPT( ) ; 
                  //digitalWrite(PIN_LED, LOW );
                  
                  
              } // end while 8 bits have been sent
                              
                CLEAR_TX_PIN();                         // Output a logic 1.
              state = TRANSMIT_STOP_BIT;
    //ENABLE_TIMER0_INT() ;                 // Allow this in now.
              OCR1A += TICKS2WAITONEHUB ;  // Count one period into the future.
                CLEAR_TIMER_INTERRUPT( ) ;
*/
#if DEBUGASERIAL
        PORTC &= ~1 ;
#endif
          break ;

  // Go to idle after stop bit was sent.
          case TRANSMIT_STOP_BIT: //************************************* We send a stop bit
    if ( ++TxCount < hubMaxData)    // Have we sent all bytes?
    {
      SwUartTXData = hubData[TxCount] ;             
      SET_TX_PIN() ;                        // Send a logic 0 on the TX_PIN.
      OCR1A = TCNT1 + TICKS2WAITONEHUB ;       // Count one period into the future.
      SwUartTXBitCount = 0 ;
      state = TRANSMIT ;
      //DISABLE_TIMER0_INT() ;    // For the byte duration
    }
          else  // all bytes have been send
    {
      TxCount = 0 ;
//      TxMax = 0 ;
      state = WAITING ;
      //sendStatus = SEND ;
      OCR1A += DELAY_100 ;  // 100uS gap
      //OCR1A += ((uint16_t)3500 * 16 ) ; // 3.5mS gap before listening
      //TRXDDR &= ~( 1 << PIN_SERIALTX );   // PIN is input, tri-stated.
      //TRXPORT &= ~( 1 << PIN_SERIALTX );  // PIN is input, tri-stated.
            
//      struct t_hubData *pdata = ThisHubData ;
//      FORCE_INDIRECT( pdata ) ;
                  

      //pdata->serialSent = 1 ;
      //DataSent = 1 ;
      //SportSync = 1 ;
      //pdata = pdata->next ;
      //if ( pdata == 0 )           // Wrap at end
      //  {
      //  pdata = FirstData ;
      //  }
      //ThisData = pdata ;
    }

// here original code
/*
    if ( TxNotEmpty )
    {
                        SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
              OCR1A = TCNT1 + TICKS2WAITONE ;   // Count one period into the future.
      SwUartTXData = ByteToSend ;   // Grab byte
      TxNotEmpty = 0 ;      // Mark 'sent'
      state = TRANSMIT ;
            SwUartTXBitCount = 0 ;
    }
    else
    {
      OCR1A += ((uint16_t)100*16) ;    // 100uS gap 
                  state = WAITING ;                  // Go back to idle.
    }
*/
               break ;
               

  case WAITING :
    
         DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupts.
         state = IDLE ;                           // Go back to idle.
//         PCIFR = (1<<PCIF2) ;     // clear pending interrupt
//         PCICR |= (1<<PCIE2) ;      // pin change interrupt enabled

// here original code from Mike
/*
    if ( TxNotEmpty )
    {
                        SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
              OCR1A = TCNT1 + TICKS2WAITONE ;   // Count one period into the future.
      SwUartTXData = ByteToSend ;   // Grab byte
      TxNotEmpty = 0 ;      // Mark 'sent'
      state = TRANSMIT ;
            SwUartTXBitCount = 0 ;
    }
    else
    {
                      DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupts.
                  state = IDLE ;                     // Go back to idle.
    }
*/

    
                break ;

  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE

 } // end "else sport" = end Hub
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif
} // End of ISR


// End of the code for both Frsky protocols +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//____________________Here the code for SPORT interface only ++++++++++++++++++++++++++++++++++++++++++

//brief  Function to initialize the UART for Sport protocol
//  This function will set up pins to transmit and receive on. Control of Timer0 and External interrupt 0.
void initSportUart(  )           //*************** initialise UART pour SPORT
{
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

#if DEBUGASERIAL
  DDRC = 0x03 ;   // PC0,1 as o/p debug
  PORTC = 0 ;
#endif
}


void setSportNewData( uint16_t id, uint32_t value )
{
  sportDataLock = 1 ;
  sportData[0] = 0x10 ;
  sportData[1] = id ; // low byte
  sportData[2] = id >> 8 ; // hight byte
  sportData[3] = value ;
  sportData[4] = value >> 8 ;
  sportData[5] = value >> 16 ;
  sportData[6] = value >> 24 ;
  sportDataLock = 0 ;
  
//  pdata->dataLock = 1 ;
//  pdata->data[0] = 0x10 ;
//  pdata->data[1] = id ; // low byte
//  pdata->data[2] = id >> 8 ; // hight byte
//  pdata->data[3] = value ;
//  pdata->data[4] = value >> 8 ;
//  pdata->data[5] = value >> 16 ;
//  pdata->data[6] = value >> 24 ;
//  pdata->dataLock = 0 ;

#ifdef DEBUGSETNEWDATA                                         
        Serial.print("set new data at ");
        Serial.print( millis());
        Serial.print(" ");
        Serial.print( pdata->data[0] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[1] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[2] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[3] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[4] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[5] , HEX );
        Serial.print(" ");
        Serial.print( pdata->data[6] , HEX );
        Serial.println(" ");
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
#if DEBUGASERIAL
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






//____________________Here the code for HUB interface only ++++++++++++++++++++++++++++++++++++++++++
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

#if DEBUGASERIAL
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
  SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
  cli() ;
  OCR1A = TCNT1 + TICKS2WAITONEHUB ;   // Count one period into the future.
  CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
  sei() ;
  SwUartTXBitCount = 0 ;
  SwUartTXData = hubData[0] ;
  //TxNotEmpty = 0 ;
  state = TRANSMIT ;
  ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#if DEBUGASERIAL
  PORTC &= ~1 ;
#endif

}
// end of function that are hub specific



//********************************** End of code to handle the UART communication with the receiver
#endif   //End of not MULTIPLEX

