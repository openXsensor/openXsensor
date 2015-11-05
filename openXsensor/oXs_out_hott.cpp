// File for Hott
#include "oXs_out_hott.h"
#ifdef HOTT

#ifdef DEBUG
// ************************* Several parameters to help debugging
  #define DEBUGHOTT

#endif


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

//volatile uint8_t sendStatus ;   
volatile uint8_t flagUpdateHottBuffer ; // When a polling for oXs is received, it says that main loop must update the buffer (within the 4mes)  
volatile uint8_t debugStatus ; 

// Transmit buffer
volatile static union {              // union is a easy way to access the data in several ways (name of field + index of byte)
    volatile HOTT_GAM_MSG gamMsg ;   // structured general air module
    volatile uint8_t txBuffer[TXHOTTDATA_BUFFERSIZE] ;
}  TxHottData;



volatile uint8_t state ;                  //!< Holds the state of the UART.
static volatile unsigned char SwUartTXData ;     //!< Data to be transmitted.
static volatile unsigned char SwUartTXBitCount ; //!< TX bit counter.
static volatile uint8_t SwUartRXData ;           //!< Storage for received bits.
static volatile uint8_t SwUartRXBitCount ;       //!< RX bit counter.
static volatile uint8_t TxCount ;
static volatile uint8_t LastRx ;                 // last byte received (allows to check on the second byte received for polling)
volatile uint8_t debugUartRx ;
static uint8_t delayTxPendingCount ;             // Used to register the number of extra delay(each 1msec) to wait in TxPending (so before replying)
volatile uint8_t ppmInterrupted ; // This flag is activated at the end of handling interrupt on Timer 1 Compare A if during this interrupt handling an interrupt on pin change (INT0 or INT1) occurs
                         // in this case, ppm will be wrong and has to be discarded       



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
    flagUpdateHottBuffer = false ; // does not allow to fill directly the buffer with data to transmit; in fact, filling occurs only after a polling
    // fill the buffer with 0 
    for ( uint8_t i = 0 ; i <= TXHOTTDATA_BUFFERSIZE ; i++ ) {       // first fill the buffer with 0 
       TxHottData.txBuffer[i] = 0 ;
    }
    // fill fixed values in the buffer
    TxHottData.gamMsg.start_byte    = 0x7c ;
    TxHottData.gamMsg.gam_sensor_id = 0x8d ; //GENRAL AIR MODULE = HOTT_TELEMETRY_GAM_SENSOR_ID
    TxHottData.gamMsg.sensor_id     = 0xd0 ;
    TxHottData.gamMsg.stop_byte     = 0x7D ;

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
    initHottUart( ) ;                   // initialise UART 
	
#ifdef DEBUGHOTT
      printer->print(F("Hott Output Module: TX Pin="));
      printer->println(_pinTx);
#endif
}


void OXS_OUT::sendData() {
#ifdef DEBUGHOTT
      printer->print(F("F= ")); printer->print(flagUpdateHottBuffer);
      printer->print(F(" S=")); printer->print(state);
      printer->print(F(" LR=")); printer->print(LastRx);
      printer->print(F(" Tc=")); printer->println(TxCount);
#endif
    if ( flagUpdateHottBuffer ) {        // this flag is set to true when UART get a polling of the device. Then measurement must be filled in the buffer
// in general air module data to fill are:
#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 1) 
          TxHottData.gamMsg.cell[0] =  voltageData->mVoltCell[0] /20 ; // Volt Cell 1 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 2) 
          TxHottData.gamMsg.cell[1] =  voltageData->mVoltCell[1] /20 ; // Volt Cell 2 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 3) 
          TxHottData.gamMsg.cell[2] =  voltageData->mVoltCell[2] /20 ; // Volt Cell 3 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 4) 
          TxHottData.gamMsg.cell[3] =  voltageData->mVoltCell[3] /20 ; // Volt Cell 4 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 5) 
          TxHottData.gamMsg.cell[4] =  voltageData->mVoltCell[4] /20 ; // Volt Cell 5 (in 2 mV increments, 210 == 4.20 V)
#endif
#if  defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 6) 
          TxHottData.gamMsg.cell[5] =  voltageData->mVoltCell[5] /20 ; // Volt Cell 6 (in 2 mV increments, 210 == 4.20 V)
#endif
#if defined(USE_VOLT_X_AS_BATTERY1) && (USE_VOLT_X_AS_BATTERY1 <7) && (USE_VOLT_X_AS_BATTERY1 > 0) && defined(PIN_VOLTAGE)
          TxHottData.gamMsg.Battery1 = voltageData->mVolt[USE_VOLT_X_AS_BATTERY1] / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif
#if defined(USE_VOLT_X_AS_BATTERY2) && (USE_VOLT_X_AS_BATTERY2 <7) && (USE_VOLT_X_AS_BATTERY2 > 0) && defined(PIN_VOLTAGE)
          TxHottData.gamMsg.Battery2 = voltageData->mVolt[USE_VOLT_X_AS_BATTERY2] / 100;    //battery 1 voltage  0.1V steps. 55 = 5.5V only pos. voltages
#endif
          TxHottData.gamMsg.rpm++ ;
          if ( TxHottData.gamMsg.rpm > 1000) TxHottData.gamMsg.rpm = 1 ; 
#ifdef MEASURE_RPM 
          TxHottData.gamMsg.rpm  = RpmValue /10 ;                      //#22 RPM in 10 RPM steps. 300 = 3000rpm
#endif
#ifdef VARIO       
        TxHottData.gamMsg.altitude =  ((varioData->relativeAlt ) / 100 ) + 500 ;  //altitude in meters. offset of 500, 500 = 0m
        TxHottData.gamMsg.climbrate_L = varioData->climbRate + 30000 ;          //climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
#endif
#if defined(PIN_CURRENTSENSOR)
        TxHottData.gamMsg.current =  currentData->milliAmps /100;               //current in 0.1A steps 100 == 10,0A
#endif
#if defined(USE_VOLT_X_AS_MAIN_BATTERY) && (USE_VOLT_X_AS_MAIN_BATTERY <7) && (USE_VOLT_X_AS_MAIN_BATTERY > 0) && defined(PIN_VOLTAGE)
          TxHottData.gamMsg.main_voltage = voltageData->mVolt[USE_VOLT_X_AS_MAIN_BATTERY] / 100;          //Main power voltage using 0.1V steps 100 == 10,0V] / 100
#endif
#if defined(PIN_CURRENTSENSOR)
        TxHottData.gamMsg.batt_cap =  currentData->consumedMilliAmps / 10 ;   // used battery capacity in 10mAh steps
#endif
#ifdef AIRSPEED       
       TxHottData.gamMsg.speed =  airSpeedData->airSpeed  ;                  //  Km/h 
#endif
#if defined(NUMBEROFCELLS) && (NUMBEROFCELLS >= 0) 
          TxHottData.gamMsg.min_cell_volt =  voltageData->mVoltCellMin /20 ; // minimum cell voltage in 2mV steps. 124 = 2,48V
#endif


// field of msg not implemented
//  byte climbrate3s;                     //#28 climb rate in m/3sec. Value of 120 = 0m/3sec
//  byte min_cell_volt_num;               //#38 number of the cell with the lowest voltage
//  uint16_t rpm2;                        //#39 LSB 2nd RPM in 10 RPM steps. 100 == 1000rpm
//  byte general_error_number;            //#41 General Error Number (Voice Error == 12) TODO: more documentation
//  byte pressure;                        //#42 High pressure up to 16bar. 0,1bar scale. 20 == 2.0bar


            // calculate the check sum on first bytes
            TxHottData.txBuffer[TXHOTTDATA_BUFFERSIZE-1] = 0 ;
            for(uint8_t i = 0; i < TXHOTTDATA_BUFFERSIZE-1; i++){  // one byte less because the last byte is the checksum
              TxHottData.txBuffer[TXHOTTDATA_BUFFERSIZE-1] += TxHottData.txBuffer[i];
#ifdef DEBUGHOTT
      printer->print(TxHottData.txBuffer[i], HEX); printer->print(F(" "));
#endif
            }  // end for
#ifdef DEBUGHOTT
      printer->println(F(" "));
#endif

    flagUpdateHottBuffer = false ;       // reset the flag to say that all data have been updated and that UART can transmit the buffer
    }   // end ( flagUpdateHottBuffer )
}




//---------------------------------- Here the code to handle the UART  
#ifdef DEBUG
//#define DEBUGSETNEWDATA
//#define DEBUGASERIAL
#endif

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))


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
        if (  ++TxCount < TXHOTTDATA_BUFFERSIZE  ) {   
              SwUartTXData = TxHottData.txBuffer[TxCount] ;
              CLEAR_TX_PIN_MB();                     // Send a logic 0 on the TX_PIN as start bit  
//              OCR1A = TCNT1 + TICKS2WAITONEHOTT ;   // Count one period into the future.
              OCR1A += DELAY_2000 ;                   // wait 2msec before sending the next byte (this is requested by Hott protocol)
              SwUartTXBitCount = 0 ;
              state = TRANSMIT ;
        } else {                                        // all bytes have already been sent
              state = WAITING ;
              OCR1A += DELAY_2000;                      // 2mS gap before listening
              TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input
              TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is tri-stated.
        }
        break ;

        case RECEIVE :                                        // Start bit has been received and we will read bits of data receiving, LSB first.     
              OCR1A += TICKS2WAITONEHOTT ;                    // Count one period after the falling edge is trigged.
              uint8_t data ;                                  // Use a temporary local storage (it save some bytes (and perhaps time)
              data = SwUartRXBitCount ;
              if( data < 8 ) {                                //If 8 bits are not yet read
                  SwUartRXBitCount = data + 1 ;
                  data = SwUartRXData ;
                  data >>= 1 ;                                // Shift due to receiving LSB first.
                  if( !(GET_RX_PIN( ) == 0 )) data |= 0x80 ;  // If a logical 1 is read, let the data mirror this.
                  SwUartRXData = data ;
               } else {                                       //Done receiving =  8 bits are in SwUartRXData
                  if ( LastRx == HOTT_BINARY_MODE_REQUEST_ID ) {     // if the previous byte identifies a polling for a reply in binary format
                           if ( SwUartRXData != HOTT_TELEMETRY_GAM_SENSOR_ID ) {
                                state = WAITING ;
                                OCR1A += ( DELAY_4000 - TICKS2WAITONEHOTT) ;                // 4mS gap before listening (take care that 4096 is the max we can wait because timer 1 is 16 bits and prescaler = 1)
                           } else  {                                 // the sensor has to reply (if it has data; here we assume it has always data and the data will be in the Hott buffer)
                               flagUpdateHottBuffer = true ;         // flag to say to send function that the buffer must be filled. It is expected that send function is called fast enough (so main loop may not be blocked) 
                               state = TxPENDING ;
                               OCR1A += ( DELAY_4000 - TICKS2WAITONEHOTT) ;                   // 4ms gap before sending; normally Hott protocols says to wait 5 msec but this is too much for timer1
                               delayTxPendingCount  = 1 ;            //  ask for 1 more delay of 1ms in order to reach the total of 5msec                 
                            }      // end last byte was a polling code

                  } else {                                // Previous code is not equal to HOTT_BINARY_MODE_REQUEST_ID , enter to iddle mode (so we will accept to read another byte)                                 
                      DISABLE_TIMER_INTERRUPT() ;         // Stop the timer interrupts.
                      state = IDLE ;                      // Go back to idle.
                      PCIFR = ( 1<<PCIF2 ) ;              // clear pending interrupt
                      PCICR |= ( 1<<PCIE2 ) ;             // pin change interrupt enabled
                  }
                  LastRx = SwUartRXData ;                 // save the byte being received; so next time a byte is received, comparison is possible.
               } // End receiving  1 byte (8 bits) (and 1 bit)
        break ;
  
      case TxPENDING :                                         //End of delay before sending data has occurs
            if ( delayTxPendingCount ) {                       // if additional delay is requested, perform it      
                delayTxPendingCount--;
                OCR1A += DELAY_1000 ; 
            } else {
                if ( flagUpdateHottBuffer ) {                     // it is expected that the main loop will update the buffer and reset this flag within the delay
                    OCR1A += DELAY_1000 ;                          // if it is not yet done, go back to wait
                    state = WAITING ;
                } else {
                    CLEAR_TX_PIN_MB() ;                                // Send a start bit (logic 0 on the TX_PIN).
                    OCR1A = TCNT1 + TICKS2WAITONEHOTT ;                // Count one period into the future.
                    SwUartTXBitCount = 0 ;
                    SwUartTXData = TxHottData.txBuffer[0] ;
                    TxCount = 0 ;
                    state = TRANSMIT ;
                }
            }    
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


// ***********************************  initialise UART for Hott protocol *********************************************
//  This function will set up pins to transmit and receive on. Control of Timer1 and pin change interrupt 0.
void initHottUart( )           //*************** initialise UART for Multiplex
{
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
//  Interrupt on Pin Change to detect change on level on Hott signal (= could be a start bit)
//
// The falling edge in the beginning of the start
//  bit will trig this interrupt. The state will
//  be changed to RECEIVE, and the timer interrupt
//  will be set to trig one and a half bit period
//  from the falling edge. At that instant the
//  code should sample the first data bit.
//
//  note  initHottUart( void ) must be called in advance.
//
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt
// on this port

ISR(PCINT2_vect)                                  // There is a start bit.
{
  if (!( TRXPIN & ( 1 << PIN_SERIALTX ) )) {      // Pin is low = start bit 
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled
            state = RECEIVE ;                 // Change state
            DISABLE_TIMER_INTERRUPT() ;       // Disable timer to change its registers.
            OCR1A = TCNT1 + TICKS2WAITONE_HALFHOTT - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.
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


// -------------------------End of HOTT protocol--------------------------------------------------------------------------------------

#endif // END of HOTT

