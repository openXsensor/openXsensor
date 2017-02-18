// File for Multiplex
#include "oXs_out_multiplex.h"
#if defined(PROTOCOL) &&  (PROTOCOL == MULTIPLEX) 

#ifdef DEBUG
// ************************* Several parameters to help debugging
//#define DEBUGSETNEWDATA
//#define DEBUGFORMATONEVALUE
#endif
//#define DEBUG_MULTIPLEX_WITHOUT_RX  // this allows to let oXs generates dummy data when there is no Rx to give the polling on the bus. In this case, oXs does not look at the pooling to send data
//#define DEBUG_FORCE_VSPEED_TO 123   // this is used to force oXs to send a fixed dummy value for Vspeed in order to test if an issue result of bmp180 or from Multiplex protocol.

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

struct t_mbAllData multiplexData ;
volatile uint8_t sendStatus ;   
int fieldContainsData[][7]  = {  SETUP_MULTIPLEX_DATA_TO_SEND } ; // contains the set up of field to be transmitted, the first field contains the Multiplex adress to be used for this measurement
int numberOfFields = sizeof(fieldContainsData) / sizeof(fieldContainsData[0]) ;
uint8_t multiplexUnit[] = { MULTIPLEX_UNITS } ; // contains the unit code (4 bits) for each OXS measurement

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


// **************** Setup the OutputLib *********************
void OXS_OUT::setup() {

    //initilalise PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input, tri-stated.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is input, tri-stated.

  // Activate pin change interupt on Tx pin
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;			// IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif

    PCIFR = (1<<PCIF2) ;	// clear pending interrupt

    //initMultiplexUart( &sportData ) ;
    initMultiplexUart( &multiplexData ) ;
	
#ifdef DEBUG
      printer->print(F("Multiplex Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F(" milli="));  
      printer->println(millis());
      printer->println(F("Multiplex Output Module: Setup!"));
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
     for ( int mbIndex = 0 ; mbIndex <= MB_MAX_ADRESS ;   mbIndex++ ) {
       multiplexData.mbData[mbIndex].response[0] = mbIndex ;
       multiplexData.mbData[mbIndex].active = NOT_ACTIVE ;
     }  
     for ( int mbIndex = 0 ; mbIndex < numberOfFields ;   mbIndex++ ) {
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[0] = ( fieldContainsData[mbIndex][0] << 4) | ( multiplexUnit[ fieldContainsData[mbIndex][1] ] );
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[1] = (uint8_t) MB_NOVALUE ; // low byte 
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[2] = MB_NOVALUE >> 8 ; // hight byte
       multiplexData.mbData[fieldContainsData[mbIndex][0]].active = AVAILABLE ;
     }  

// for defug purpose only
multiplexData.mbData[15].response[0] = 0xF0 ;
multiplexData.mbData[15].response[1] = 0x0F ;
multiplexData.mbData[15].response[2] = 0xAA ;
#ifdef DEBUG
     for ( int mbIndex = 0 ; mbIndex <= MB_MAX_ADRESS ;   mbIndex++ ) {
        Serial.print("id=");
        Serial.print( mbIndex);
        Serial.print(" Act=");
        Serial.print( multiplexData.mbData[mbIndex].active, HEX );
        Serial.print(" r0=");
        Serial.print( multiplexData.mbData[mbIndex].response[0], HEX);
        Serial.print(" r1=");
        Serial.print( multiplexData.mbData[mbIndex].response[1], HEX);
        Serial.print(" r2=");
        Serial.println( multiplexData.mbData[mbIndex].response[2], HEX);
     }   
#endif


}



void OXS_OUT::sendData()
{
    for ( uint8_t mbIndex = 0 ; mbIndex < numberOfFields ;   mbIndex++ ) {
            formatOneValue( mbIndex ) ;
    }
}

//*************************************
// Format the sensor data into an array containing the 3 bytes to sent
// process occurs only if measurement has been flagged as available by the sensor
// return 0 if data has been processed; return the internal code of the measurement otherwise
//************************************
uint8_t OXS_OUT::formatOneValue( uint8_t currentFieldToSend) {
  static int32_t valueTemp ;
  static uint8_t alarm ;
//  int fieldID ;
//  fieldID = 0 ;
  valueTemp = 0 ;
  
#ifdef GPS_INSTALLED            
  uint8_t GPS_no_fix = ( (GPS_fix_type != 3 ) && (GPS_fix_type != 4 ) ) ; // this flag is true when there is no fix
#endif          
      
  switch ( fieldContainsData[currentFieldToSend][1] ) {
#ifdef VARIO       
      case  ALTIMETER :
        if ( ! varioData->absoluteAlt.available ) return 0 ;
        valueTemp = (varioData->absoluteAlt.value ) / 100 ;
        varioData->absoluteAlt.available = false ;
#ifdef DEBUGFORMATONEVALUE
        Serial.print("At ");
        Serial.print( millis());
        Serial.print("id=");
        Serial.print( currentFieldToSend );
        Serial.println("Alt available ");
#endif
        break ;
      case VERTICAL_SPEED : 
         if ( ! mainVspeed.available ) return 0; 
         valueTemp = mainVspeed.value / 10 ;
         mainVspeed.available = false ;
#ifdef DEBUG_FORCE_VSPEED_TO
         valueTemp = DEBUG_FORCE_VSPEED_TO ;
#endif         
         break ;
      case SENSITIVITY :
             if ( !varioData->sensitivity.available ) return 0;
             valueTemp = varioData->sensitivity.value ;
             varioData->sensitivity.available = false ;
             break ;
//      case ALT_OVER_10_SEC :
//             if ( ! varioData->vSpeed10SecAvailable ) return 0;
//             valueTemp = varioData->vSpeed10Sec  / 100 ; // todo : adjust decimals
//             varioData->vSpeed10SecAvailable = false ;
//             break ;
      case  REL_ALTIMETER :
        if ( ! varioData->relativeAlt.available ) return 0 ;
        valueTemp = (varioData->relativeAlt.value ) / 100 ;
        varioData->relativeAlt.available = false ;
            break ;
      case  ALTIMETER_MAX :
        if ( ! varioData->relativeAltMaxAvailable ) return 0 ;
        valueTemp = (varioData->relativeAltMax ) / 100 ;
        varioData->relativeAltMaxAvailable = false ;
            break ;
      
#endif  // End vario    

/*
#ifdef VARIO2       
      case  ALTIMETER_2 :
        if ( ! varioData_2->absoluteAltAvailable  ) return 0;
        valueTemp = varioData_2->absoluteAlt  / 100 ;
        varioData_2->absoluteAltAvailable = false ;
        break ;
      case VERTICAL_SPEED_2 : 
         if ( ! varioData_2->climbRateAvailable  ) return 0;
         valueTemp = varioData_2->climbRate / 10 ;
         varioData_2->climbRateAvailable = false ;
         break ;
      case SENSITIVITY_2 :
             if ( ! varioData_2->sensitivityAvailable  ) return 0;
             valueTemp = varioData_2->sensitivity ;
             varioData_2->sensitivityAvailable = false ;
             break ;
     case ALT_OVER_10_SEC_2 :
             if ( ! varioData_2->vSpeed10SecAvailable  ) return 0;
             valueTemp = varioData_2->vSpeed10Sec / 100 ; // todo : adjust decimals
             varioData_2->vSpeed10SecAvailable = false ;
             break ; 
      case  REL_ALTIMETER_2 :
        if ( ! varioData_2->relativeAltAvailable ) return 0 ;
        valueTemp = (varioData_2->relativeAlt ) / 100 ;
        varioData_2->relativeAltAvailable = false ;           
        break ;     
#endif  // End vario2    
*/
/*
#if defined (VARIO )  &&  defined (VARIO2)
      case VERTICAL_SPEED_A : 
        if ( ! averageVSpeedAvailable  ) return 0;
        valueTemp = averageVSpeed / 10 ;
        averageVSpeedAvailable = false ; 
         break ; 
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) || defined( USE_6050) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY )  && defined (PIN_PPM)
      case PPM_VSPEED : 
        if ( ! switchVSpeedAvailable  ) return 0;
        valueTemp = switchVSpeed / 10 ;
        switchVSpeedAvailable = false ; 
         break ; 
#endif
*/
#ifdef AIRSPEED       
      case  AIR_SPEED :
        if ( ! airSpeedData->airSpeed.available  ) return 0;
        valueTemp = airSpeedData->airSpeed.value  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        airSpeedData->airSpeed.available = false ;
        break ;
//      case  PRANDTL_COMPENSATION :
//        if ( ! airSpeedData->compensationAvailable  ) return 0;
//        valueTemp = airSpeedData->compensation / 10  ;
//        airSpeedData->compensationAvailable = false ;
//        break ;
#endif  // End airpseed    

//#if defined (VARIO) && defined ( AIRSPEED)
//      case PRANDTL_DTE :
//        if ( ! compensatedClimbRateAvailable  ) return 0;
//        valueTemp =  compensatedClimbRate / 10 ; 
//        compensatedClimbRateAvailable = false ;
//        break ;
//#endif  // End defined (VARIO) && defined ( AIRSPEED)

#ifdef PIN_VOLTAGE
      case VOLT_1 :  
         if (! voltageData->mVolt[0].available  ) return 0;
         valueTemp = voltageData->mVolt[0].value / 100;
         voltageData->mVolt[0].available = false ;
         break ;
      case VOLT_2 :  
         if ( ! voltageData->mVolt[1].available  ) return 0;
         valueTemp = voltageData->mVolt[1].value / 100 ;
         voltageData->mVolt[1].available = false ;
          break ;
      case VOLT_3 :  
         if ( ! voltageData->mVolt[2].available  ) return 0;
         valueTemp = voltageData->mVolt[2].value / 100 ;
         voltageData->mVolt[2].available = false ;
          break ;
      case VOLT_4 :  
         if ( ! voltageData->mVolt[3].available  ) return 0;
         valueTemp = voltageData->mVolt[3].value / 100 ;
         voltageData->mVolt[3].available = false ;
          break ;
      case VOLT_5 :  
         if ( ! voltageData->mVolt[4].available  ) return 0;
         valueTemp = voltageData->mVolt[4].value / 100 ;
         voltageData->mVolt[4].available = false ;
          break ;
      case VOLT_6 :  
         if ( ! voltageData->mVolt[5].available  ) return 0;
         valueTemp = voltageData->mVolt[5].value / 100 ;
         voltageData->mVolt[5].available = false ;
          break ;
#endif

#if defined (PIN_CURRENTSENSOR)
      case CURRENTMA :
         if ( ! currentData->milliAmps.available  ) return 0;
         valueTemp = currentData->milliAmps.value /100;
         currentData->milliAmps.available = false ;
         break ;
      case MILLIAH :
         if ( ! currentData->consumedMilliAmps.available  ) return 0;
         valueTemp = currentData->consumedMilliAmps.value ;
         currentData->consumedMilliAmps.available = false ;
         break ;
#endif

#if (NUMBEROFCELLS > 0)     //  This part has still to be adapted for Multiplex !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      case  CELL_1 :
          if ( ! voltageData->mVoltCell_Available[0]  ) return 0;
          valueTemp =  voltageData->mVoltCell[0] /100 ; 
          voltageData->mVoltCell_Available[0]  = false ;
          break ;
      case  CELL_2 :
          if ( ! voltageData->mVoltCell_Available[1]  ) return 0;
          valueTemp =  voltageData->mVoltCell[1]  / 100 ; 
          voltageData->mVoltCell_Available[1]  = false ;
          break ;
      case  CELL_3 :
          if ( ! voltageData->mVoltCell_Available[2]  ) return 0;
          valueTemp =  voltageData->mVoltCell[2]  / 100 ; 
          voltageData->mVoltCell_Available[2]  = false ;
          break ;
      case  CELL_4 :
          if ( ! voltageData->mVoltCell_Available[3]  ) return 0;
          valueTemp =  voltageData->mVoltCell[3] / 100 ; 
          voltageData->mVoltCell_Available[3]  = false ;
          break ;
      case  CELL_5 :
          if ( ! voltageData->mVoltCell_Available[4]  ) return 0;
          valueTemp =  voltageData->mVoltCell[4] / 100 ; 
          voltageData->mVoltCell_Available[4]  = false ;
          break ;
      case  CELL_6 :
          if ( ! voltageData->mVoltCell_Available[5]  ) return 0;
          valueTemp =  voltageData->mVoltCell[5] / 100 ; 
          voltageData->mVoltCell_Available[5]  = false ;
          break ;
      case  CELL_MIN :
          if ( ! voltageData->mVoltCellMin_Available  ) return 0;
          valueTemp =  voltageData->mVoltCellMin / 100 ; 
          voltageData->mVoltCellMin_Available  = false ;
          break ;
      case  CELL_TOT :
          if ( ! voltageData->mVoltCellTot_Available ) return 0;
          valueTemp =  voltageData->mVoltCellTot  / 100 ; 
          voltageData->mVoltCellTot_Available  = false ;
          break ;

#endif  // NUMBEROFCELLS > 0 

#ifdef PIN_PPM 
      case  PPM :
          if ( ! ppm.available  ) return 0;
          valueTemp = ppm.value ;
          ppm.available  = false ; 
          break ;
#endif


#ifdef MEASURE_RPM 
      case  RPM :
          if ( ! sport_rpm.available  ) return 0;
          valueTemp = sport_rpm.value ;  // to adjust probably
          sport_rpm.available  = false ;
          break ;   
#endif
      case  TEST_1 :
          if ( ! test1.available  ) return 0;
          valueTemp = test1.value ;
          test1.available  = false ; 
          break ;
      case  TEST_2 :
          if ( ! test2.available  ) return 0;
          valueTemp = test2.value ;
          test2.available  = false ; 
          break ;
      case  TEST_3 :
          if ( ! test3.available  ) return 0;
          valueTemp = test3.value ;
          test3.available  = false ; 
          break ;

#ifdef GPS_INSTALLED            
      case GPS_COURSE :
        if (GPS_no_fix ) return 0 ;
        valueTemp = GPS_ground_course / 10000 ; // convert from degre * 100000 to 1/10 of degree
        break ;
      case GPS_SPEED :
        if (GPS_no_fix ) return 0 ;
#ifdef GPS_SPEED_3D
        valueTemp = ((uint32_t) GPS_speed_3d) * 36 /100 ;       // convert from cm/sec to 1/10 of km/h
#else
        valueTemp = ((uint32_t) GPS_speed_2d) * 36 /100 ;       // convert from cm/sec to 1/10 of km/h
#endif
        break ;
      case GPS_ALTITUDE : 
        if (GPS_no_fix ) return 0 ;
        valueTemp = GPS_altitude / 1000 ;                        // convert from mm to m 
        break ;
      case GPS_DISTANCE :
        if (GPS_no_fix ) return 0 ;
        valueTemp = GPS_distance ;                             // keep in m
        break ;
      case GPS_BEARING :
        if (GPS_no_fix ) return 0 ;
        valueTemp = GPS_bearing * 10 ;                         // convert from degree to 1/10 of degree
        break ;
#endif                           // end GPS_INSTALLED
      }  // end Switch
//      if ( (fieldContainsData[currentFieldToSend][0] != 0)  ) { ;  // to be adapted in a condition that protect modification of data array while data are transfered to TX buffer array 
//      }
      valueTemp = (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4]  ;
      alarm =  (( valueTemp <= fieldContainsData[currentFieldToSend][5] ) || ( valueTemp >= fieldContainsData[currentFieldToSend][6]) ) ? 1 : 0;
      setMultiplexNewData(  fieldContainsData[currentFieldToSend][0]  ,  valueTemp , alarm  ) ; 
      return fieldContainsData[currentFieldToSend][1] ;
}  // End function


void OXS_OUT::setMultiplexNewData(  uint16_t id, int32_t value , uint8_t alarm)
{
#ifdef DEBUGSETNEWDATA
        int32_t valueOrig = value ;
#endif        
        multiplexData.mbData[id].active = LOCKED ;
        value = value << 1 ;
        value += alarm ;
        multiplexData.mbData[id].response[1] = value ;	
        multiplexData.mbData[id].response[2] = value >> 8 ; 
        multiplexData.mbData[id].active = AVAILABLE ;
#ifdef DEBUGSETNEWDATA                                        
        Serial.print("set new data at ");
        Serial.print( millis());
        Serial.print(" for id=");
        Serial.print( id ); 
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[0] , HEX );
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[1] , HEX );
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[2] , HEX );
        Serial.print(" A=");
        Serial.print( varioData->absoluteAlt.value );
        Serial.print(" V=");
        Serial.print( mainVspeed.value );
        Serial.print(" Vo=");
        Serial.print( valueOrig );

        Serial.println(" ");
#endif
              
}

//---------------------------------- Here the code to handle the UART  
#ifdef DEBUG
//#define DEBUGSETNEWDATA
//#define DEBUGASERIAL
#endif

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
//uint8_t sensorId ;

static volatile uint8_t TxMultiplexData[3] ; // array containing the char to transmit (is filled only when a polling address is received 
//static uint8_t firstMultiplexData ;
struct t_mbAllData  * volatile ThisMultiplexData = 0 ;

ISR(TIMER1_COMPA_vect)
{
  switch (state)
  {
      case TRANSMIT :   // startbit has been sent, it is time to output now 8 bits and 1 stop bit
#if DEBUGASERIAL
          PORTC |= 1 ;
#endif
          if( SwUartTXBitCount < 8 ) {            // If not 8 bits have been sent
            if( SwUartTXData & 0x01 ) {           // If the LSB of the TX buffer is 1:
              SET_TX_PIN_MB() ;
            } else {                                // Otherwise:
              CLEAR_TX_PIN_MB() ;                     // Send a logic 0 on the TX_PIN
            }
            SwUartTXData = SwUartTXData >> 1 ;    // Bitshift the TX buffer and
            SwUartTXBitCount += 1 ;               // increment TX bit counter.
          } else {    //Send stop bit.
            SET_TX_PIN_MB() ;                             // Output a logic 1.
            state = TRANSMIT_STOP_BIT;
          }
          OCR1A += TICKS2WAITONEMULTIPLEX ;  // Count one period into the future.
#if DEBUGASERIAL
        PORTC &= ~1 ;
#endif
          break ;

    case TRANSMIT_STOP_BIT:                    //************************************* handling after the stop bit has been sent
        if (  ++TxCount < 3  ) {   //  there are 3 bytes to send in the Multiplex interface ?
              SwUartTXData = TxMultiplexData[TxCount] ;
//                  SwUartTXData = TxCount ;
              CLEAR_TX_PIN_MB();                     // Send a logic 0 on the TX_PIN as start bit  
              OCR1A = TCNT1 + TICKS2WAITONEMULTIPLEX  - INTERRUPT_BETWEEN_TRANSMIT;   // Count one period into the future.
              SwUartTXBitCount = 0 ;
              state = TRANSMIT ;
        } else {                     // 3 bytes have already been sent
              state = WAITING ;
 //                 sendStatus = SEND ;
              OCR1A += DELAY_2000;    // 2mS gap before listening
              TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
              TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
        }
        break ;

        case RECEIVE :  // Start bit has been received and we will read bits of data receiving, LSB first.     
              OCR1A += TICKS2WAITONEMULTIPLEX ;                    // Count one period after the falling edge is trigged.
              uint8_t data ;                     // Use a temporary local storage (it save some bytes (and perhaps time)
              data = SwUartRXBitCount ;
              if( data < 8 ) {                                     //If 8 bits are not yet read
                  SwUartRXBitCount = data + 1 ;
                  data = SwUartRXData ;
                  data >>= 1 ;                         // Shift due to receiving LSB first.
                  if( !(GET_RX_PIN( ) == 0 )) data |= 0x80 ;          // If a logical 1 is read, let the data mirror this.
                  SwUartRXData = data ;
               } else {                                      //Done receiving =  8 bits are in SwUartRXData
                   struct t_mbAllData * volatile pdata = ThisMultiplexData ;
                   FORCE_INDIRECT( pdata ) ;
                   if ( SwUartRXData > MB_MAX_ADRESS ) {
//                        mb_commandReceived ( SwUartRXData ) ; to implement if wanted in a second phase
                        state = WAITING ;
                        OCR1A += DELAY_4000 ;                // 4mS gap before listening (take care that 4096 is the max we can wait because timer 1 is 16 bits and prescaler = 1)
                   } else  {
                        if  ( pdata->mbData[ SwUartRXData ] . active == AVAILABLE )  { 
                          TxMultiplexData[0] = pdata->mbData[SwUartRXData].response[0] ;
                          TxMultiplexData[1] = pdata->mbData[SwUartRXData].response[1] ;
                          TxMultiplexData[2] = pdata->mbData[SwUartRXData].response[2] ;
                          if ( (TxMultiplexData[2] != (MB_NOVALUE>>8)) || (TxMultiplexData[1] != (MB_NOVALUE & 0xff)) ) pdata->mbData [ SwUartRXData ] . active = NOT_AVAILABLE ;      // this line could be set in comment if we want to send same data and not only when a new calculation is done
                              state = TxPENDING ;
                              OCR1A += ( DELAY_1600 - TICKS2WAITONEMULTIPLEX) ;      // 1.6ms gap before sending
                          } else {                                                           // Status was not AVAILABLE, so there are no data ready to send
                              state = WAITING ;
                              OCR1A += DELAY_4000 ;   // 4mS gap before listening (take care that 4096 is the max we can wait because timer 1 is 16 bits and prescaler = 1)
                          }
                   } // end receiving 1 byte
           } // End receiving  1 bit or 1 byte (8 bits)
        break ;
  
      case TxPENDING :                                         //End of delay before sending data has occurs
            CLEAR_TX_PIN_MB() ;                          // Send a start bit (logic 0 on the TX_PIN).
            OCR1A = TCNT1 + TICKS2WAITONEMULTIPLEX - INTERRUPT_ENTRY_TRANSMIT;         // Count one period into the future.
            SwUartTXBitCount = 0 ;
            SwUartTXData = TxMultiplexData[0] ;
            TxCount = 0 ;
            state = TRANSMIT ;
            break ;

    case WAITING :                                  // At the end of wait time, stop timer interrupt but allow pin change interrupt in order to allow to detect incoming data
           state = IDLE ;                           // Go back to idle.
           DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupts.
           CLEAR_PIN_CHANGE_INTERRUPT() ;   // clear pending pin change interrupt
           ENABLE_PIN_CHANGE_INTERRUPT() ;    // pin change interrupt enabled
#ifdef DEBUG_MULTIPLEX_WITHOUT_RX                 // When no Rx is connected, oXs has to use only timer interrupts.
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
            CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
            ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#endif
            break ;

#ifdef DEBUG_MULTIPLEX_WITHOUT_RX
    case IDLE :                                  // When there is no RX, there is no pin change interrupt. State is IDLE by default or after sending one set of data
                                                 // here we will load some dummy data to transmit, wait for 4msec and we will go to TxPending to transmit the data.
            static uint8_t lineWhenDebugging = 0 ;
            struct t_mbAllData * volatile pdataDebug;
            pdataDebug = ThisMultiplexData ;
            FORCE_INDIRECT( pdataDebug ) ;
            lineWhenDebugging++ ;
            if ( lineWhenDebugging > MB_MAX_ADRESS ) lineWhenDebugging = 0 ;
            if  ( pdataDebug->mbData[ lineWhenDebugging ] . active == AVAILABLE )  { 
                          TxMultiplexData[0] = pdataDebug->mbData[lineWhenDebugging].response[0] ;
                          TxMultiplexData[1] = pdataDebug->mbData[lineWhenDebugging].response[1] ;
                          TxMultiplexData[2] = pdataDebug->mbData[lineWhenDebugging].response[2] ;
                          pdataDebug->mbData [ lineWhenDebugging ] . active = NOT_AVAILABLE ;  
                  //  TxMultiplexData[0] = 0x55 ; TxMultiplexData[1] = 0xAA ; TxMultiplexData[2] = 0x55 ; // can be use to measure the timing between bits
                          state = TxPENDING ;
                          OCR1A += DELAY_2000 ;                                                   
             } else {                                                           // Status was not AVAILABLE, so there are no data ready to send
                   OCR1A += DELAY_100 ;   // 100usec gap before trying again that another data becomes available 
             }
             break ;
#endif

  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif

} // End of ISR



//brief  Function to initialize the UART for multiplex protocol
//  This function will set up pins to transmit and receive on. Control of Timer1 and pin change interrupt 0.
void initMultiplexUart( struct t_mbAllData * volatile pdata )           //*************** initialise UART for Multiplex
{
    ThisMultiplexData = pdata ;              // this allows the ISR routine to get access to the data in OXS_Out_Multiplex
    
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
    CLEAR_PIN_CHANGE_INTERRUPT() ;
    ENABLE_PIN_CHANGE_INTERRUPT() ; 
    state = IDLE ;     // Internal State Variable

#if DEBUGASERIAL
    DDRC = 0x01 ;   // PC0 as o/p debug = pin A0 !!!!
    PORTC = 0 ; 
#endif

#ifdef DEBUG_MULTIPLEX_WITHOUT_RX    // in this debug case, we have to enable timer interrupt without waiting for a pin change interrupt.
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
            CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
            ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#endif
}


// ! \brief  External interrupt service routine.  ********************
//  Interrupt on Pin Change to detect change on level on Multiplex signal (= could be a start bit)
//
// The falling edge in the beginning of the start
//  bit will trig this interrupt. The state will
//  be changed to RECEIVE, and the timer interrupt
//  will be set to trig one and a half bit period
//  from the falling edge. At that instant the
//  code should sample the first data bit.
//
//  note  initMultiplexUart( void ) must be called in advance.
//
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt
// on this port

ISR(PCINT2_vect)
{
  if (!( TRXPIN & ( 1 << PIN_SERIALTX ) )) {      // Pin is low = start bit 
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
            state = RECEIVE ;                 // Change state
            DISABLE_TIMER_INTERRUPT() ;       // Disable timer to change its registers.
            OCR1A = TCNT1 + TICKS2WAITONE_HALFMULTIPLEX - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.
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

}  // end ISR pin change




// -------------------------End of Multiplex protocol--------------------------------------------------------------------------------------

#endif // END of MULTIPLEX

