// File for Hott
#include "oXs_out_hott.h"
#ifdef HOTT

#ifdef DEBUG
// ************************* Several parameters to help debugging
//#define DEBUGSETNEWDATA
//#define DEBUGFORMATONEVALUE
#endif


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
    PCMSK2 |= 0x10 ;			              // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif
    PCIFR = (1<<PCIF2) ;               	// clear pending interrupt
    initHottUart( &hottData ) ;         
	
#ifdef DEBUG
      printer->print(F("Hott Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F(" milli="));  
      printer->println(millis());
      printer->println(F("Hott Output Module: Setup!"));
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

  switch ( fieldContainsData[currentFieldToSend][1] ) {
#ifdef VARIO       
      case  ALTIMETER :
        if ( ! varioData->absoluteAltAvailable ) return 0 ;
        valueTemp = (varioData->absoluteAlt ) / 100 ;
        varioData->absoluteAltAvailable = false ;
#ifdef DEBUGFORMATONEVALUE
        Serial.print("At ");
        Serial.print( millis());
        Serial.print("id=");
        Serial.print( currentFieldToSend );
        Serial.println("Alt available ");
#endif
        break ;
      case VERTICAL_SPEED : 
         if ( ! varioData->climbRateAvailable ) return 0; 
         valueTemp = varioData->climbRate / 10 ;
         varioData->climbRateAvailable = false ;
         break ;
      case SENSITIVITY :
             if ( !varioData->sensitivityAvailable ) return 0;
             valueTemp = varioData->sensitivity ;
             varioData->sensitivityAvailable = false ;
             break ;
      case ALT_OVER_10_SEC :
             if ( ! varioData->vSpeed10SecAvailable ) return 0;
             valueTemp = varioData->vSpeed10Sec  / 100 ; // todo : adjust decimals
             varioData->vSpeed10SecAvailable = false ;
             break ;
      case  REL_ALTIMETER :
        if ( ! varioData->relativeAltAvailable ) return 0 ;
        valueTemp = (varioData->relativeAlt ) / 100 ;
        varioData->relativeAltAvailable = false ;
            break ;
      case  ALTIMETER_MAX :
        if ( ! varioData->relativeAltMaxAvailable ) return 0 ;
        valueTemp = (varioData->relativeAltMax ) / 100 ;
        varioData->relativeAltMaxAvailable = false ;
            break ;
      
#endif  // End vario    

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

#if defined (VARIO )  &&  defined (VARIO2)
      case VERTICAL_SPEED_A : 
        if ( ! averageVSpeedAvailable  ) return 0;
        valueTemp = averageVSpeed / 10 ;
        averageVSpeedAvailable = false ; 
         break ; 
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY )  && defined (PIN_PPM)
      case PPM_VSPEED : 
        if ( ! switchVSpeedAvailable  ) return 0;
        valueTemp = switchVSpeed / 10 ;
        switchVSpeedAvailable = false ; 
         break ; 
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED :
        if ( ! airSpeedData->airSpeedAvailable  ) return 0;
        valueTemp = airSpeedData->airSpeed  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        airSpeedData->airSpeedAvailable = false ;
        break ;
      case  PRANDTL_COMPENSATION :
        if ( ! airSpeedData->compensationAvailable  ) return 0;
        valueTemp = airSpeedData->compensation / 10  ;
        airSpeedData->compensationAvailable = false ;
        break ;
#endif  // End airpseed    

#if defined (VARIO) && defined ( AIRSPEED)
      case PRANDTL_DTE :
        if ( ! compensatedClimbRateAvailable  ) return 0;
        valueTemp =  compensatedClimbRate / 10 ; 
        compensatedClimbRateAvailable = false ;
        break ;
#endif  // End defined (VARIO) && defined ( AIRSPEED)

#ifdef PIN_VOLTAGE
      case VOLT1 :  
         if (! voltageData->mVoltAvailable[0]  ) return 0;
         valueTemp = voltageData->mVolt[0] / 100;
         voltageData->mVoltAvailable[0] = false ;
         break ;
      case VOLT2 :  
         if ( ! voltageData->mVoltAvailable[1]  ) return 0;
         valueTemp = voltageData->mVolt[1] / 100 ;
         voltageData->mVoltAvailable[1] = false ;
          break ;
      case VOLT3 :  
         if ( ! voltageData->mVoltAvailable[2]  ) return 0;
         valueTemp = voltageData->mVolt[2] / 100 ;
         voltageData->mVoltAvailable[2] = false ;
          break ;
      case VOLT4 :  
         if ( ! voltageData->mVoltAvailable[3]  ) return 0;
         valueTemp = voltageData->mVolt[3] / 100 ;
         voltageData->mVoltAvailable[3] = false ;
          break ;
      case VOLT5 :  
         if ( ! voltageData->mVoltAvailable[4]  ) return 0;
         valueTemp = voltageData->mVolt[4] / 100 ;
         voltageData->mVoltAvailable[4] = false ;
          break ;
      case VOLT6 :  
         if ( ! voltageData->mVoltAvailable[5]  ) return 0;
         valueTemp = voltageData->mVolt[5] / 100 ;
         voltageData->mVoltAvailable[5] = false ;
          break ;
#endif

#if defined (PIN_CURRENTSENSOR)
      case CURRENTMA :
         if ( ! currentData->milliAmpsAvailable  ) return 0;
         valueTemp = currentData->milliAmps /100;
         currentData->milliAmpsAvailable = false ;
         break ;
      case MILLIAH :
         if ( ! currentData->consumedMilliAmpsAvailable  ) return 0;
         valueTemp = currentData->consumedMilliAmps ;
         currentData->consumedMilliAmpsAvailable = false ;
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
          if ( ! ppmAvailable  ) return 0;
          valueTemp = ppm ;
          ppmAvailable  = false ; 
          break ;
#endif


#ifdef MEASURE_RPM 
      case  RPM :
          if ( ! RpmAvailable  ) return 0;
          valueTemp = RpmValue ;  // to adjust probably
          RpmAvailable  = false ;
          break ;   
#endif
      case  TEST1 :
          if ( ! test1ValueAvailable  ) return 0;
          valueTemp = test1Value ;
          test1ValueAvailable  = false ; 
          break ;
      case  TEST2 :
          if ( ! test2ValueAvailable  ) return 0;
          valueTemp = test2Value ;
          test2ValueAvailable  = false ; 
          break ;
      case  TEST3 :
          if ( ! test3ValueAvailable  ) return 0;
          valueTemp = test3Value ;
          test3ValueAvailable  = false ; 
          break ;




      }  // end Switch
//      if ( (fieldContainsData[currentFieldToSend][0] != 0)  ) { ;  // to be adapted in a condition that protect modification of data array while data are transfered to TX buffer array 
//      }
      valueTemp = (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4]  ;
      alarm =  (( valueTemp <= fieldContainsData[currentFieldToSend][5] ) || ( valueTemp >= fieldContainsData[currentFieldToSend][6]) ) ? 1 : 0;
      setMultiplexNewData(  fieldContainsData[currentFieldToSend][0]  ,  valueTemp , alarm  ) ; 
      return fieldContainsData[currentFieldToSend][1] ;
}  // End function


void OXS_OUT::setHottNewData(  uint16_t id, int32_t value , uint8_t alarm)
{
#ifdef DEBUGSETNEWDATA
        int32_t valueOrig = value ;
#endif        
        hottData.mbData[id].active = LOCKED ;
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
        Serial.print( varioData->absoluteAlt );
        Serial.print(" V=");
        Serial.print( varioData->climbRate );
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

static volatile uint8_t TxHottData[47] ; // array containing the char to transmit (is filled only when a polling address is received 
//struct t_mbAllData  * volatile ThisMultiplexData = 0 ;

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
            SET_TX_PIN_MB() ;                             // Output a logic 1. (in high impedance)
            state = TRANSMIT_STOP_BIT;
          }
          OCR1A += TICKS2WAITONEHOTT ;  // Count one period into the future.
#if DEBUGASERIAL
        PORTC &= ~1 ;
#endif
          break ;

    case TRANSMIT_STOP_BIT:                    //************************************* handling after the stop bit has been sent
        if (  ++TxCount < 3  ) {   // !!!!!!!! to change with number of byte to transmit
              SwUartTXData = TxHottData[TxCount] ;
              CLEAR_TX_PIN_MB();                     // Send a logic 0 on the TX_PIN as start bit  
              OCR1A = TCNT1 + TICKS2WAITONEHOTT ;   // Count one period into the future.
              SwUartTXBitCount = 0 ;
              state = TRANSMIT ;
        } else {                                        // all bytes have already been sent
              state = WAITING ;
              OCR1A += DELAY_2000;                      // 2mS gap before listening
              TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
              TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
        }
        break ;

        case RECEIVE :  // Start bit has been received and we will read bits of data receiving, LSB first.     
              OCR1A += TICKS2WAITONEHOTT ;                    // Count one period after the falling edge is trigged.
              uint8_t data ;                     // Use a temporary local storage (it save some bytes (and perhaps time)
              data = SwUartRXBitCount ;
              if( data < 8 ) {                                     //If 8 bits are not yet read
                  SwUartRXBitCount = data + 1 ;
                  data = SwUartRXData ;
                  data >>= 1 ;                         // Shift due to receiving LSB first.
                  if( !(GET_RX_PIN( ) == 0 )) data |= 0x80 ;          // If a logical 1 is read, let the data mirror this.
                  SwUartRXData = data ;
               } else {                                      //Done receiving =  8 bits are in SwUartRXData
                  if ( LastRx == HOTT_BINARY_MODE_REQUEST_ID ) {                    // if the previous byte identifies a polling for a reply in binary format
                           if ( SwUartRXData <> HOTT_TELEMETRY_GAM_SENSOR_ID ) {
                                state = WAITING ;
                                OCR1A += DELAY_4000 ;                // 4mS gap before listening (take care that 4096 is the max we can wait because timer 1 is 16 bits and prescaler = 1)
                           } else  {                                 // the sensor has to reply (if it has data; here we assume it has always data and the data will be in the Hott buffer)
                               updateHottBuffer = true ;
                               state = TxPENDING ;
                               OCR1A += ( DELAY_4000 - TICKS2WAITONEHOTT) ;                   // 4ms gap before sending
       !!!!!!!!!!!!!!!!!!!                         
                            } // end last byte was a polling code

                  } else {                                // Previous code is not equal to HOTT_BINARY_MODE_REQUEST_ID , enter to iddle mode                                  
                      DISABLE_TIMER_INTERRUPT() ;         // Stop the timer interrupts.
                      state = IDLE ;                      // Go back to idle.
                      PCIFR = ( 1<<PCIF2 ) ;              // clear pending interrupt
                      PCICR |= ( 1<<PCIE2 ) ;             // pin change interrupt enabled
                  }
                  LastRx = SwUartRXData ;
               } // End receiving  1 bit or 1 byte (8 bits)



                   
              } // End receiving  1 bit or 1 byte (8 bits)
        break ;
  
      case TxPENDING :                                         //End of delay before sending data has occurs
            CLEAR_TX_PIN_MB() ;                                // Send a start bit (logic 0 on the TX_PIN).
            OCR1A = TCNT1 + TICKS2WAITONEHOTT ;                // Count one period into the future.
            SwUartTXBitCount = 0 ;
            SwUartTXData = TxHottData[0] ;
            TxCount = 0 ;
            state = TRANSMIT ;
            break ;

    case WAITING :                                  // At the end of wait time, stop timer interrupt but allow pin change interrupt in order to allow to detect incoming data
           DISABLE_TIMER_INTERRUPT() ;              // Stop the timer interrupts.
           state = IDLE ;                           // Go back to idle.
           CLEAR_PIN_CHANGE_INTERRUPT() ;           // clear pending pin change interrupt
           ENABLE_PIN_CHANGE_INTERRUPT() ;          // pin change interrupt enabled
            break ;

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
    PCMSK2 |= 0x10 ;                    // IO4 (PD4) on Arduini mini
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

// here code based on web site
// Initialisation message GAM (General Air Module)
void GMessage::init_gam_msg(){
  //met tous à Zero, puis on modifie les constantes
  memset(hott_gam_msg, 0, sizeof(struct HOTT_GAM_MSG));   
  hott_gam_msg->start_byte = 0x7c;
  hott_gam_msg->gam_sensor_id = HOTT_TELEMETRY_GAM_SENSOR_ID;
  hott_gam_msg->sensor_id = 0xd0;
  hott_gam_msg->stop_byte = 0x7d;
}

// Emission de la trame
void GMessage::send(int lenght){ 
  uint8_t sum = 0;
  delay(5);
  for(int i = 0; i < lenght-1; i++){
    sum = sum + _hott_serial_buffer[i];
    Serial.write (_hott_serial_buffer[i]);
    delayMicroseconds(HOTTV4_TX_DELAY);

    Serial.read(); // A cause du rebouclage TX <--> RX Lecture de l'octet émis 
  }  
  //Emision du checksum
  Serial.write (sum);
  delayMicroseconds(HOTTV4_TX_DELAY);
  Serial.read(); // A cause du rebouclage TX <--> RX Lecture de l'octet émis 
}

void GMessage::main(){ 
  // 
  static byte page_settings = 1;//numéro de la page de settings à afficher 
    
  if (Serial.available() >= 2) {
    uint8_t octet1 = Serial.read();
    switch (octet1) {
    case HOTT_BINARY_MODE_REQUEST_ID:
      { 
        uint8_t  octet2 = Serial.read();        
        // Demande RX Module =  $80 $XX
        switch (octet2) {
        case HOTT_TELEMETRY_GAM_SENSOR_ID: //0x8D
        {
            // init structure
            init_gam_msg();        
            
            // ******************************************************
            LEDPIN_ON    

            // Mesure des tensions
            lipo1 = tension.Tension_Lipo1();  //etc...
    
            // Calcul de la valeur de chaque elements
            switch (nb_Lipo){
             case 2 :                         // etc... other cases removed
             Lipo_total = lipo2;
             lipo2 = lipo2 - lipo1;
             break;  

            default : ;
          }     
         
         // par défaut 1 et 2      
          setLipo (lipo1,1);
          setLipo (lipo2,2);

          setVoltage1(Lipo_total);    // Affichage sur Batt 1
          setMainVoltage(Lipo_total); // Affichage sur principal
            
          if (Lipo_total < Lipo_Total_Mini)  {
            Lipo_Total_Mini = Lipo_total; // mémorise la valeur mini
            setVoltage2(Lipo_Total_Mini); 
          }
       
         // 
         // Recherche element le plus faible
         // pour affichage jauge
         // et gestion alarme radio
         lipo_mini = tension.Lipo_Mini(nb_Lipo,lipo1,lipo2,lipo3,lipo4,lipo5,lipo6);
       
         if ((lipo_mini >= 3.6)&&(lipo_mini <= 4.2))
            Jauge = 100 - (4.2 - lipo_mini) / 0.0066667;           
         else if ((lipo_mini < 3.6) && (lipo_mini >= 3.0)) 
            Jauge = (lipo_mini - 3.0) / 0.06 ; 
         else if (lipo_mini < 3.0)
            Jauge = 0;               
       
         // Alarme
         if (lipo_mini < (float(seuil_tension_alarme)/100.0))        
           Alarme = true;
         else
           Alarme = false;
  
         if (Alarme_On == true) {
           alarme (son_alarme); // Bip alarme
           temps_alarme = millis ();             // temps courant
           Alarme_On = false;                    // Flag faux
         }
         else {
           alarme (ALARME_OFF);}          // fin d'alarme
        
        // Affichage jauge carburant
        setFuelPercent (Jauge);

        LEDPIN_OFF
  
        if (Alarme == true) {
          temps = millis ();
          if (temps > temps_alarme + (repetition * 1000)) { // repetition  entre alarmes
            Alarme_On = true;
          }  
      }            
                        
      send(sizeof(struct HOTT_GAM_MSG));
            break;
       } //end case GAM*/
       } //end case octet 2
        break;
    }

    } 
  }
}

//////////////////////////////////////////////////
// Tension principale (resolution 0.1V)
void GMessage::setMainVoltage(float tension){
  hott_gam_msg->main_voltage = (uint16_t) (tension * 10);
}

// Batterie 1 (resolution 0.1V)
void GMessage::setVoltage1(float volt){
  hott_gam_msg->Battery1 =  (uint16_t) (volt * 10); 
}

// Batterie 2 (resolution 0.1V)
void GMessage::setVoltage2(float volt){
  hott_gam_msg->Battery2 = (uint16_t) (volt * 10); 
}

// Element Lipo n (resolution 0.02V)
void GMessage::setLipo(float volt, int lipo){

  if (lipo >= 1 and lipo <= 6)
  {
    lipo--;
    hott_gam_msg->cell[lipo] =  (uint16_t) 100 * volt / 2 ; 
    
    if (volt/2 <= hott_gam_msg->min_cell_volt || hott_gam_msg->min_cell_volt ==0)
    {
      hott_gam_msg->min_cell_volt = (uint16_t) 100 * volt/2 ;
      hott_gam_msg->min_cell_volt_num = lipo;
    }
  }
}

// Altitude relative en metres : -500 / +9999 [m]
void GMessage::setAltitudeInMeters(uint16_t alti){
  hott_gam_msg->altitude = alti + 500;
}

// Temperature : -20 a +235° C 
// 
void GMessage::setTemp(int temp, int capteur){
  if(temp < -20)
    temp = -20;
  else if(temp > 234)
    temp = 235;
  if (capteur < 1)
    capteur = 1;
  if (capteur > 2)
    capteur = 2; 
  if (capteur == 1)
    hott_gam_msg->temperature1 = temp + 20;
  else if (capteur == 2)
    hott_gam_msg->temperature2 = temp + 20;
}

// Niveau reservoir : 0%, 25%, 50%, 75%, 100%  
// 
void GMessage::setFuelPercent(uint8_t pourcent){
  if(pourcent > 100)
    pourcent = 100;
  else if(pourcent < 0)
    pourcent = 0;
  hott_gam_msg->fuel_procent = pourcent ;
}

// niveau reservoir en ml : 0 a 65535 ml
// 
void GMessage::setFuelMilliliters(uint16_t ml){
  hott_gam_msg->fuel_ml = ml ;
}

// Rotation : 0 a 655350 tr/min
// 

void GMessage::setRPM(uint16_t rpm){
  hott_gam_msg->rpm = rpm ;
} 

// Set climbrates
void GMessage::setClimbrate(uint16_t climb_L){
  hott_gam_msg->climbrate_L =  climb_L+30000;
}

void GMessage::setClimbrate_M3(int climb3s){
  hott_gam_msg->climbrate3s =  climb3s+120;
}

// Set speed
void GMessage::setSpeed(float speed){
  hott_gam_msg->climbrate3s =  (int) speed;
}


//
// Alarme sonore sur radio
//
void GMessage::alarme (uint8_t son){
  hott_gam_msg->warning_beeps = son;
}

void GMessage::_hott_send_text_msg() {
  for(byte *_hott_msg_ptr = hott_txt_msg->text[0]; _hott_msg_ptr < &hott_txt_msg->stop_byte ; _hott_msg_ptr++){
    if (*_hott_msg_ptr == 0)
      *_hott_msg_ptr = 0x20;
  }  
  //_hott_send_msg(_hott_serial_buffer, sizeof(struct HOTT_TEXTMODE_MSG));
  send(sizeof(struct HOTT_TEXTMODE_MSG));
}

char * GMessage::_hott_invert_all_chars(char *str) {
  return _hott_invert_chars(str, 0);
}

char * GMessage::_hott_invert_chars(char *str, int cnt) {
  if(str == 0) return str;
  int len = strlen(str);
  if((len < cnt)  && cnt > 0) len = cnt;
  for(int i=0; i< len; i++) {
    str[i] = (byte)(0x80 + (byte)str[i]);
  }
  return str;
}

void GMessage::_hott_invert_ligne(int ligne) {
  if (ligne>= 0 && ligne<= 7)
    for(int i=0; i< 21; i++) {
      if (hott_txt_msg->text[ligne][i] == 0)   //inversion du caratère null (fin de string)
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + 0x20);
      else
        hott_txt_msg->text[ligne][i] = (byte)(0x80 + (byte)hott_txt_msg->text[ligne][i]);
    }
}




// -------------------------End of HOTT protocol--------------------------------------------------------------------------------------

#endif // END of HOTT

