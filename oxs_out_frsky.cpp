
// ************************* Several parameters to help debugging
//#define DEBUGSENDDATASTATUS
//#define DEBUGSENDDATA
//#define DEBUGNEXTVALUETYPE
//#define DEBUGSENDDATADELAY
//#define DEBUGTRANSMITDELAY
//#define DEBUGLOADVALUETOSEND
//#define DEBUGLOADVALUETOSENDALTIMETER
//#define DEBUGLOADVALUETOSENDVERTICALSPEED
//#define DEBUGLOADVALUETOSENDVOLT1
//#define DEBUGLOADVALUETOSENDVOLT2
//#define DEBUGLOADVALUETOSENDSENSITIVITY
//#define DEBUGLOADVALUETOSENDALT_OVER_10_SEC
//#define DEBUGLOADVALUETOSENDCELL_1_2
//#define DEBUGLOADVALUETOSENDCELL_3_4
//#define DEBUGLOADVALUETOSENDCELL_5_6
//#define DEBUGLOADVALUETOSENDCURRENTMA
//#define DEBUGLOADVALUETOSENDMILLIAH
//#define DEBUGHUBPROTOCOL
//#define DEBUGWITHFIXVALUE     // for SPORT protocol only; send a value varying continuously (cycling) between min and max value (see code below)


//#include "oxs_config.h"    // already in .h file
//#include "Arduino.h"       // already in .h file

#include "oxs_out_frsky.h"
#include "Aserial.h"

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

#if defined(FRSKY_SPORT)
volatile uint8_t sendStatus ;   // !!! this is used in Aserial too
struct t_sportData MyData[1] ;   // Mike, This could be adapted because there is no reason anymore using an array
#else  // Hub protocol
struct t_hubData MyData ;
#endif  // end FRSKY_SPORT

int fieldContainsData[][5]  = {  SETUP_DATA_TO_SEND } ;
int numberOfFields = sizeof(fieldContainsData) / sizeof(fieldContainsData[0]) ;
int currentFieldToSend = 0 ; 

#ifdef DEBUG  
OXS_OUT_FRSKY::OXS_OUT_FRSKY(uint8_t pinTx,HardwareSerial &print)
#else
OXS_OUT_FRSKY::OXS_OUT_FRSKY(uint8_t pinTx)
#endif
{
#ifdef DEBUG 
  _pinTx = pinTx ;               // Mike I imagine this instrcution is required because _pinTx is used here under; but why not print directly pinTX??? .h file to adapt accordingly
  printer = &print; //operate on the address of print
#endif
}


// **************** Setup the FRSky OutputLib *********************
void OXS_OUT_FRSKY::setup()
{
#if defined(FRSKY_SPORT)
  //	MyData[0].next = &MyData[1] ;  // I use only 1 occurence and there is a (quite complex) logic to define which field may be send in which sequence.
	initSportUart( &MyData[0] ) ;
#else
	initHubUart( &MyData ) ;
#endif
	
#ifdef DEBUG
      printer->begin(115200);                           // Mike , is this instruction required??? there is already a Serial.begin in the ino file
      printer->print("FRSky Output Module: TX Pin=");
      printer->println(_pinTx);
      printer->println("FRSky Output Module: Setup!");
      printer->print("Number of fields to send = ");
      printer->println(numberOfFields);
      for (int rowNr = 0 ; rowNr < numberOfFields ; rowNr++) {
          printer->print(fieldContainsData[rowNr][0],HEX); printer->print(" , "); 
          printer->print(fieldContainsData[rowNr][1]);  printer->print(" , ");
          printer->print(fieldContainsData[rowNr][2]);  printer->print(" , ");
          printer->println(fieldContainsData[rowNr][3]);
      }    
#endif
}

#ifdef MEASURE_RPM
extern volatile uint16_t Rpm ;
#endif // MEASURE_RPM


#if defined(FRSKY_SPORT)
//****************************************************** Look which value can be transmitted and load it in a set of fields used by interrupt routine
    /* We look if a value is available ; if available the value is loaded
       We have to take care that all types of value (when available) have to be transmitted
       Note : it should be possible to send more values if OXS would reply to more than one device ID because the polling occurs more often
         Still here we will react only to one device ID (see oxs_config.h)
       We look at the values off each sensor in sequence
       Each value has a extra field "available":
         Available = true = KNOWN when tha value is calculated. It becomes false = UNKNOWN when the value is loaded for transmission.; 
       There is 1 general status related to the transmission. This "sendStatus" is shared by all values
         sendStatus can be  : TO_LOAD, LOADED, SENDING, SEND ; 
             For one value, when the value is available and if sendStatus = TO_LOAD or LOADED, we can load the value (again); 
             It is not allowed to load the value is the sendStatus = SENDING
             sendStatus goes from TO_LOAD to LOADED when the value is loaded in 'setNewData'; The indicator available of this value become false = UNKNOWN in order to avoid to load it again
             sendStatus goes from LOADED to SENDING when the start bit is received for the right device ID
             sendStatus  goes from SENDING to SEND when all bytes (including Checksum) have been transmitted
       
       When sendStatus is :
        - "TO_LOAD:
              - If a value is not availvable (UNKNOWN), we try to skip to next available value (in order to send as many data as possible)
              - If a value is availvable (KNOWN), the value is loaded and flagged as UNKNOWN. sendStatus become LOADED
        - "LOADED" :
               - If a value is availvable (KNOWN), the value is loaded and flagged as UNKNOWN. sendStatus remains LOADED
               - If a value is not availvable (UNKNOWN), we keep the already loaded value and sendStatus remains LOADED
        - "SENDING", We just wait that the already value is send (even if a new value is already available)
         - "SEND" : sendStatus becomes "TO_LOAD" and we skip to the next value to send.
  
   */  
    
void OXS_OUT_FRSKY::sendData()
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
              loadValueToSend( currentFieldToSend ) ;
              sendStatus = LOADED ; 
          }
          else {
              sendStatus = TO_LOAD ;
          }	 
          break ;
      case TO_LOAD :       
	  if ( readStatusValue( currentFieldToSend) == KNOWN ) {
              loadValueToSend( currentFieldToSend ) ;
              sendStatus = LOADED ; 
          }
          else  {
              currentFieldToSend = nextFieldToSend( currentFieldToSend ) ;
              if ( readStatusValue( currentFieldToSend) == KNOWN ) {
                  loadValueToSend( currentFieldToSend ) ;
                  sendStatus = LOADED ; 
              }
              break ;
          }
          break;
     
      case LOADED :
          if ( readStatusValue( currentFieldToSend ) == KNOWN)  {
              loadValueToSend( currentFieldToSend ) ;
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
uint8_t OXS_OUT_FRSKY::readStatusValue( uint8_t fieldToSend) {
  switch ( fieldContainsData[fieldToSend][1] )
    {
#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 )       
     case  VOLT1 :
          return arduinoData->mVoltAvailable[0] ;    
     case  VOLT2 :
          return arduinoData->mVoltAvailable[1] ;
     case  VOLT3 :
          return arduinoData->mVoltAvailable[2] ;
     case  VOLT4 :
          return arduinoData->mVoltAvailable[3] ;
     case  VOLT5 :
          return arduinoData->mVoltAvailable[4] ;
     case  VOLT6 :
          return arduinoData->mVoltAvailable[5] ;     
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

#ifdef PIN_CurrentSensor
      case CURRENTMA  :
          return  currentData->milliAmpsAvailable ; 
      case MILLIAH  :
          return  currentData->consumedMilliAmpsAvailable ; 
#endif

#if (NUMBEROFCELLS > 0)
      case  CELLS_1_2 :
          return  arduinoData->mVoltCell_1_2_Available ; 
      case  CELLS_3_4 :
          return  arduinoData->mVoltCell_3_4_Available ; 
      case  CELLS_5_6 :
          return  arduinoData->mVoltCell_5_6_Available ; 
#endif

#ifdef MEASURE_RPM
      case RPM :
        return RpmAvailable ;
#endif

      default:
          return  UNKNOWN ;
     }
  return  UNKNOWN ;
}    // End readStatusValue()



//**************************************************************************************
// Search the next (index of) field to transmit 
// 
//**************************************************************************************
uint8_t OXS_OUT_FRSKY::nextFieldToSend(  uint8_t indexField) {
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

#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 ) 
      if     ( (fieldContainsData[indexField][1] == VOLT1) && ( arduinoData->mVoltAvailable[0] == KNOWN ) ) { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == VOLT2) && ( arduinoData->mVoltAvailable[1] == KNOWN ) ) { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == VOLT3) && ( arduinoData->mVoltAvailable[2] == KNOWN ) ) { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == VOLT4) && ( arduinoData->mVoltAvailable[3] == KNOWN ) ) { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == VOLT5) && ( arduinoData->mVoltAvailable[4] == KNOWN ) ) { return indexField ; } 
      else if ( (fieldContainsData[indexField][1] == VOLT6) && ( arduinoData->mVoltAvailable[5] == KNOWN ) ) { return indexField ; } 
#endif

#if defined (PIN_CurrentSensor)
    if ( (fieldContainsData[indexField][1] == CURRENTMA) && ( currentData->milliAmpsAvailable == KNOWN ) ){ return indexField ; }  
    else if ( (fieldContainsData[indexField][1] == MILLIAH ) && ( currentData->consumedMilliAmpsAvailable == KNOWN ) ){ return indexField ; }  
#endif

#if (NUMBEROFCELLS > 0)
      if ( (fieldContainsData[indexField][1] == CELLS_1_2) && ( arduinoData->mVoltCell_1_2_Available == KNOWN ) ){ return indexField ; }  
      else if ( (fieldContainsData[indexField][1] == CELLS_3_4) && ( arduinoData->mVoltCell_3_4_Available == KNOWN ) ){ return indexField ; }  
      else if ( (fieldContainsData[indexField][1] == CELLS_5_6) && ( arduinoData->mVoltCell_5_6_Available == KNOWN ) ){ return indexField ; }  
#endif          

#ifdef MEASURE_RPM
      if ( (fieldContainsData[indexField][1] == RPM) && ( RpmAvailable == KNOWN ) ){ return indexField ; } 
#endif

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
void OXS_OUT_FRSKY::loadValueToSend( uint8_t currentFieldToSend) {
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

#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 ) 
      case VOLT1 :  
         valueTemp = arduinoData->mVolt[0] ;
         arduinoData->mVoltAvailable[0] = false ;
#ifdef DEBUGLOADVALUETOSENDVOLT1
          static unsigned long StartVolt1=micros();
          printer->print("LoadValueToSend - mVolt1 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( arduinoData->mVolt[0] );
          printer->print(" , delay = ");
          printer->println( (micros() - StartVolt1 )/1000 );
          StartVolt1 = micros() ;
#endif 
          break ;
      case VOLT2 :  
         valueTemp = arduinoData->mVolt[1] ;
         arduinoData->mVoltAvailable[1] = false ;
#ifdef DEBUGLOADVALUETOSENDVOLT2
          static unsigned long StartVolt2=micros();
          printer->print("LoadValueToSend - mVolt2 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( arduinoData->mVolt[1] );
          printer->print(" , delay = ");
          printer->println( (micros() - StartVolt2 )/1000 );
          StartVolt2 = micros() ;
#endif          
          break ;
      case VOLT3 :  
         valueTemp = arduinoData->mVolt[2] ;
         arduinoData->mVoltAvailable[2] = false ;
          break ;
      case VOLT4 :  
         valueTemp = arduinoData->mVolt[3] ;
         arduinoData->mVoltAvailable[3] = false ;
          break ;
      case VOLT5 :  
         valueTemp = arduinoData->mVolt[4] ;
         arduinoData->mVoltAvailable[4] = false ;
          break ;
      case VOLT6 :  
         valueTemp = arduinoData->mVolt[5] ;
         arduinoData->mVoltAvailable[5] = false ;
          break ;
      
#endif

#if defined (PIN_CurrentSensor)
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
          valueTemp =  arduinoData->mVoltCell_1_2  ; 
          fieldID = CELLS_FIRST_ID ; 
          arduinoData->mVoltCell_1_2_Available  = false ;
#ifdef DEBUGLOADVALUETOSENDCELL_1_2
          static unsigned long StartCell_1_2=micros();
          printer->print("LoadValueToSend Cell_1_2 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (arduinoData->mVoltCell_1_2 & 0xFFF00000) >> 20  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_1_2 & 0x000FFF00) >> 8  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_1_2 & 0x000000FF) , HEX );
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_1_2 )/1000 );
          StartCell_1_2 = micros() ;
#endif                    
          break ;
          
      case  CELLS_3_4 :
          valueTemp = arduinoData->mVoltCell_3_4 ; 
          fieldID = CELLS_SECOND_ID ; 
          arduinoData->mVoltCell_3_4_Available  = false ;     
#ifdef DEBUGLOADVALUETOSENDCELL_3_4
          static unsigned long StartCell_3_4=micros();
          printer->print("LoadValueToSend Cell_3_4 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (arduinoData->mVoltCell_3_4 & 0xFFF00000) >> 20  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_3_4 & 0x000FFF00) >> 8  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_3_4 & 0x000000FF) , HEX );
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_3_4 )/1000 );
          StartCell_3_4 = micros() ;
#endif                    
          break ;
          
      case  CELLS_5_6 :
          valueTemp = arduinoData->mVoltCell_5_6 ;
          fieldID = CELLS_THIRD_ID ;  
          arduinoData->mVoltCell_5_6_Available  = false ; 
#ifdef DEBUGLOADVALUETOSENDCELL_5_6
          static unsigned long StartCell_5_6=micros();
          printer->print("LoadValueToSend Cell_5_6 at ");
          printer->print( millis()  );
          printer->print(" value = ");
          printer->print( (arduinoData->mVoltCell_5_6 & 0xFFF00000 >> 20)  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_5_6 & 0x000FFF00 >> 8)  );
          printer->print(" ");
          printer->print( (arduinoData->mVoltCell_5_6 & 0x000000FF) , HEX );
          
          printer->print(" , delay = ");
          printer->println( (micros() - StartCell_5_6 )/1000 );
          StartCell_5_6 = micros() ;
#endif                    
          break ;

#endif  // NUMBEROFCELLS > 0 

#ifdef MEASURE_RPM 
      case  RPM :
          valueTemp = Rpm ;
          fieldID = RPM_FIRST_ID ;  
          RpmAvailable  = false ; 
#endif

      }  // end Switch
      if ( (fieldContainsData[currentFieldToSend][0] != DEFAULTFIELD)  ) fieldID = fieldContainsData[currentFieldToSend][0] ;
#ifdef DEBUGWITHFIXVALUE
      static int delta = 1 ;
      static int32_t prevValue = 0 ;
      static int32_t maxValue = 255;
      static int32_t minValue = -255;
      if (prevValue <  minValue || prevValue >  maxValue) delta = delta * -1 ;
      prevValue  = prevValue + delta ;
      valueTemp = prevValue ; 
#endif //End DEBUGWITHFIXVALUE
      setNewData( &MyData[0], fieldID  ,  (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4] ) ; 

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

#else // not FRSKY_SPORT ==============================================Hub Protocol ================================================

  static int fieldToSend ;
  static bool fieldOk ;
  static byte SwitchFrameVariant=0;

void OXS_OUT_FRSKY::sendData()  // to do for Hub protocol
{
  static uint32_t lastMsFrame1=0;
//  static unsigned int lastMsFrame2=0;
  static uint32_t temp ;

  temp = millis() ;
  if ( (temp-lastMsFrame1) >= INTERVAL_FRAME1  ) {
#ifdef DEBUGHUBPROTOCOL
     printer->print("Send Data at = ");
     printer->println( millis() );
#endif
//    static byte SwitchFrameVariant=0;
    lastMsFrame1=temp;
    SendFrame1();
//    if (SwitchFrameVariant==0)SendFrame1A();
//    if (SwitchFrameVariant==1)SendFrame1B();

//    SwitchFrameVariant++;  // Test in order to send only 1 type of frame
//    if(SwitchFrameVariant==2)SwitchFrameVariant=0 ;
  }
//  second frame was never used; if activated again, then we have to take care that orginal data are already sent before filling the buffer
//  if ( (temp-lastMsFrame2) > INTERVAL_FRAME2  ) {
//    lastMsFrame2=temp;
//    SendFrame2();
//  }
}  // end sendData Hub protocol

//***********************************************************************************
//  SendFrame1 => generate the frame in a buffer and call Aserial (setNewData) it may be send     *
//***********************************************************************************
void OXS_OUT_FRSKY::SendFrame1(){
#ifdef DEBUGHUBPROTOCOL
  printer->print("FRSky output module: SendFrame1A:");
#endif
    MyData.maxData = 0 ; // reset of number of data to send
#ifdef SEND_FixValue
   SendValue(FRSKY_USERDATA_TEMP1,(int16_t)1234); // Fix value in T1 ; only for test purpose
#endif
  for (int rowNr = 0 ; rowNr < numberOfFields ; rowNr++) {
    if ( MyData.maxData < (maxSizeBuffer - 13) ){
        loadValueToSend( rowNr ) ;    
    }    
  }    
  if( MyData.maxData > 0 ) {
    sendHubByte(0x5E) ; // End of Frame 1!
    setNewData( &MyData ) ;
  }  
#ifdef DEBUGHUBPROTOCOL
      printer->print("Data to send = ");
      for (int cntPrint = 0 ; cntPrint < MyData.maxData ; cntPrint++) {
        printer->print(" ");
        printer->print(MyData.data[cntPrint] , HEX);
      }
     printer->println(" "); 
#endif
  
}

 

/**********************************************************/
/* SendValue => send a value as frsky sensor hub data     */
/**********************************************************/
void OXS_OUT_FRSKY::SendValue(uint8_t ID, uint16_t Value) {
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


void OXS_OUT_FRSKY::sendHubByte( uint8_t byte )
{	
	MyData.data[MyData.maxData] = byte ;
	MyData.maxData ++ ;	
}

//*************************************
// Put the value to transmit in a buffer
// Note : in hub protocol there is no check if a data is already (re)calculated or not.
//************************************
void OXS_OUT_FRSKY::loadValueToSend( uint8_t currentFieldToSend ) {
//  static int16_t valueTemp ;
//  static int fieldToSend ;
//  static bool fieldOk ;
//  valueTemp = 0 ; 
  fieldToSend = (int) fieldContainsData[currentFieldToSend][0]  ;
  if ( (fieldToSend >= FRSKY_USERDATA_GPS_ALT_B ) && (fieldToSend <= FRSKY_USERDATA_FUELPERCENT ) )  fieldOk = true ;
  else fieldOk = false ;
  switch ( fieldContainsData[currentFieldToSend][1] ) {

#ifdef VARIO       
      case  ALTIMETER :    
//          if ( (SwitchFrameVariant == 0) && (varioData->absoluteAltAvailable) ) { //========================================================================== Vario Data
              if (fieldToSend == DEFAULTFIELD) {
                SendAlt(varioData->absoluteAlt);           
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
                else SendValue(FRSKY_USERDATA_VERT_SPEED,(int16_t)varioData->climbRate); // ClimbRate in open9x Vario mode
//                varioData->climbRateAvailable = false ;
              }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData->climbRate * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//                 varioData->climbRateAvailable = false ;
              }  
//          }   
          break ;
       case SENSITIVITY :
//          if ( (SwitchFrameVariant == 0) && (varioData->sensitivityAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData->sensitivity * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]))+ fieldContainsData[currentFieldToSend][4] );
//               varioData->sensitivityAvailable = false ;
             }
//          }   
          break ;
       case ALT_OVER_10_SEC :
//          if ( (SwitchFrameVariant == 0) && (varioData->vSpeed10SecAvailable ) ){
             if ( fieldOk == true ) {
               SendValue((int8_t) fieldToSend ,(int16_t) ( (varioData->vSpeed10SecAvailable * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
//               varioData->vSpeed10SecAvailable = false ;
             }
//          }   
          break ;
          
          
#endif   // end vario   

#if defined( PIN_Voltage1 ) || defined( PIN_Voltage2 ) || defined( PIN_Voltage3 ) || defined( PIN_Voltage4 ) || defined( PIN_Voltage5 ) || defined( PIN_Voltage6 ) 
      case VOLT1 :  
         SendVoltX( 0 , currentFieldToSend ) ;
         break ;
      case VOLT2 :  
         SendVoltX( 1 , currentFieldToSend ) ;
          break ;
      case VOLT3 :  
         SendVoltX( 2 , currentFieldToSend ) ;
          break ;
      case VOLT4 :  
         SendVoltX( 3 , currentFieldToSend ) ;
          break ;
      case VOLT5 :  
         SendVoltX( 4 , currentFieldToSend) ;
         break ;
      case VOLT6 :  
         SendVoltX( 5 , currentFieldToSend) ;
         break ;
      
#endif

#if defined (PIN_CurrentSensor)
      case CURRENTMA :
//          if ( (SwitchFrameVariant == 0) && (currentData->milliAmpsAvailable ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCurrentMilliAmps(currentData->milliAmps);
//                 currentData->milliAmpsAvailable = false ;
             }
              else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (currentData->milliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
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
      case FRSKY_USERDATA_CURRENT_MAX :
//          if ( (SwitchFrameVariant == 0)  ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendValue(0x38,int16_t(currentData->maxMilliAmps));    //Cur+ OK!
             }
             else if(  fieldOk == true ) {
                 SendValue((int8_t) fieldToSend ,(int16_t) ( (currentData->maxMilliAmps * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3]) ) + fieldContainsData[currentFieldToSend][4] );
             }  
//          }    
         break ;
#endif  // End PIN_CurrentSensor


#if (NUMBEROFCELLS > 0)
      case  CELLS_1_2 :
//          if ( (SwitchFrameVariant == 0) && ( arduinoData->mVoltCell_1_2_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( arduinoData->mVoltCell_1_2 ) ;
//                 arduinoData->mVoltCell_1_2_Available  = false ;
             }
//          }    
          break ;
          
      case  CELLS_3_4 :
//          if ( (SwitchFrameVariant == 0) && ( arduinoData->mVoltCell_3_4_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( arduinoData->mVoltCell_3_4 ) ;
//                 arduinoData->mVoltCell_3_4_Available  = false ;
             }
//          }   
          break ;
          
      case  CELLS_5_6 :
//          if ( (SwitchFrameVariant == 0) && ( arduinoData->mVoltCell_5_6_Available ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                 SendCellVoltage( arduinoData->mVoltCell_5_6 ) ;
                 arduinoData->mVoltCell_5_6_Available  = false ;
             }
//          }   
          break ;

#endif  // NUMBEROFCELLS > 0         

#ifdef MEASURE_RPM
      case  RPM :
//          if ( (SwitchFrameVariant == 0) && ( RpmAvailable  ) ) {
             if ( fieldToSend == DEFAULTFIELD ) {
                  SendValue(FRSKY_USERDATA_RPM, Rpm);
             }
//          }
          break ;
#endif  //  MEASURE_RPM

      }  // end Switch
}  // End function  loadValueToSend (Frame 1)


void OXS_OUT_FRSKY::SendVoltX( uint8_t VoltToSend , uint8_t indexFieldToSend) {
//        if ( (SwitchFrameVariant == 0) && (  arduinoData->mVoltAvailable[VoltToSend] == true )) {
           if ( fieldOk == true ) {
             SendValue((int8_t) fieldToSend ,(int16_t) ( ( arduinoData->mVolt[VoltToSend] * fieldContainsData[indexFieldToSend][2] / fieldContainsData[indexFieldToSend][3])) + fieldContainsData[indexFieldToSend][4] );
//             arduinoData->mVoltAvailable[VoltToSend] = false ;
           }
//         }
}

/**********************************************************/
/* SendCellVoltage => send a cell voltage                 */
/**********************************************************/
/**********************************************************/
/* SendCellVoltage => send a cell voltage                 */
/**********************************************************/
void OXS_OUT_FRSKY::SendCellVoltage( uint32_t voltage) {
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

/**********************************/
/* SendGPSDist => send 0..32768   */
/**********************************/
void OXS_OUT_FRSKY::SendGPSDist(uint16_t dist) {// ==> Field "Dist" in open9x
  SendValue(0x3C,uint16_t(dist)); //>> DIST
}

/************************************************************/
/* SendTemperature1/2 =>  tempc in 1/100th of degree celsius */
/* Display Range in openTX: -3276,8..32768=-3276,8C..+3276,C */
/************************************************************/
void OXS_OUT_FRSKY::SendTemperature1(int16_t tempc) {
  SendValue(FRSKY_USERDATA_TEMP1, tempc);
}
void OXS_OUT_FRSKY::SendTemperature2(int16_t tempc) {
  SendValue(FRSKY_USERDATA_TEMP2, tempc);
}
/*************************************/
/* SendRPM => Send Rounds Per Minute */
/*************************************/
//void OXS_OUT_FRSKY::SendRPM(uint16_t rpm) {
//  byte blades=2;
//  rpm = uint16_t((float)rpm/(60/blades));  
//  SendValue(FRSKY_USERDATA_RPM, rpm);
//}

/*************************************/
/* SendFuel => SendFuel Level        */
/*************************************/
void OXS_OUT_FRSKY::SendFuel(uint16_t fuel) {
  SendValue(FRSKY_USERDATA_FUEL, fuel);
}

/**********************************/
/* SendAlt => Send ALtitude in cm */
/**********************************/
void OXS_OUT_FRSKY::SendAlt(long altcm)
{
  uint16_t Centimeter =  uint16_t(abs(altcm)%100);
  long Meter;
#if defined(FORCE_ABSOLUTE_ALT) && !defined(FRSKY_SPORT)
  altcm-=1;
#endif

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

/***********************************************/
/* SendCurrentMilliAmps => Send Current        */
/* current will be transmitted as 1/10th of A  */
/* Range: -32768..32767 => disp -327,6..3276,7 */
/***********************************************/
void OXS_OUT_FRSKY::SendCurrentMilliAmps(int32_t milliamps) 
{
#ifdef ForceAbsolutCurrent
  milliamps=abs(milliamps);
#endif 
  SendValue(FRSKY_USERDATA_CURRENT, (uint16_t)(milliamps/100));
}



#endif // End of FRSKY_SPORT

