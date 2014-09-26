/* ============================================================
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================*/

// Based loosely on:
// Atmel AVR304: Half Duplex Interrupt Driven Software UART

// Author: Mike Blandford

#include "Aserial.h"

#ifdef DEBUG
//#define DEBUGSETNEWDATA
#endif
//#define DEBUGASERIAL

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
//#ifdef FRSKY_SPORT
struct t_sportData *FirstData = 0 ;
struct t_sportData *ThisSportData = 0 ;
//#else // Hub protocol
//struct t_hubData *FirstData = 0 ;
struct t_hubData *ThisHubData = 0 ;
//#endif



static volatile uint8_t state ;                  //!< Holds the state of the UART.
static volatile unsigned char SwUartTXData ;     //!< Data to be transmitted.
static volatile unsigned char SwUartTXBitCount ; //!< TX bit counter.
static volatile uint8_t SwUartRXData ;           //!< Storage for received bits.
static volatile unsigned char SwUartRXBitCount ; //!< RX bit counter.

volatile uint8_t TxCount ;

// only for Hub
//volatile uint8_t TxCount ;
volatile uint8_t TxHubData[maxSizeBuffer] ;
volatile uint8_t TxMax ;

// initially for Sport
uint8_t LastRx ;
//uint8_t TxCount ;
uint8_t TxSportData[7] ;
uint16_t Crc ;
uint8_t DataSent ;
uint8_t SportSync ;

// Here the code for both protocols +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! \brief  Timer1 interrupt service routine. *************** interrupt between 2 bits (handled by timer1)
//
//  Timer1 will ensure that bits are written and
//  read at the correct instants in time.
//  The state variable will ensure context
//  switching between transmit and recieve.
//  If state should be something else, the
//  variable is set to IDLE. IDLE is regarded
//  as a safe state/mode.

uint8_t ByteStuffByte = 0 ;

ISR(TIMER1_COMPA_vect)
{
  if ( sportAvailable ) {    // ++++++++ here only for SPORT protocol ++++++++++++++++++++++++++++++++++
  switch (state)
	{
  // Transmit Byte.
	  case TRANSMIT :		// Output the TX buffer.************ on envoie des bits de data
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
        	else		//Send stop bit.
    			{
        	  CLEAR_TX_PIN();                         // Output a logic 1.
        	  state = TRANSMIT_STOP_BIT;
                //ENABLE_TIMER0_INT() ;	                  // Allow this in now.
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
		//ENABLE_TIMER0_INT() ;									// Allow this in now.
    	        OCR1A += TICKS2WAITONESPORT ;  // Count one period into the future.
                CLEAR_TIMER_INTERRUPT( ) ;
*/
#if DEBUGASERIAL
	      PORTC &= ~1 ;
#endif
  	      break ;

  // Go to idle after stop bit was sent.
          case TRANSMIT_STOP_BIT: //************************************* We send a stop bit
		if ( ByteStuffByte || (++TxCount < 8 ) )		// Have we sent 8 bytes?
		{
			if ( ByteStuffByte )
			{
				SwUartTXData = ByteStuffByte ;
				ByteStuffByte = 0 ;
			}
			else
                        {
		            if ( TxCount < 7 )		// Data (or crc)?
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
		  //DISABLE_TIMER0_INT() ;	    // For the byte duration
		}
	      else  // 8 bytes have been send
		{
		  state = WAITING ;
                  sendStatus = SEND ;
		  OCR1A += ((uint16_t)3500 * 16 ) ;		// 3.5mS gap before listening
		  TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN is input, tri-stated.
		  TRXPORT &= ~( 1 << PIN_SERIALTX ) ;           // PIN is input, tri-stated.
       		  
                  struct t_sportData *pdata = ThisSportData ;
		  FORCE_INDIRECT( pdata ) ;

		  pdata->serialSent = 1 ;
		  DataSent = 1 ;
		  SportSync = 1 ;
		  pdata = pdata->next ;
		  if ( pdata == 0 )		               // Wrap at end
		    {
			pdata = FirstData ;
		    }
		  ThisSportData = pdata ;
		}

               break ;
               
//#ifdef FRSKY_SPORT
          case RECEIVE :  // Start bit has been received and we will read bits of data      
            OCR1A += TICKS2WAITONESPORT ;                    // Count one period after the falling edge is trigged.
            //Receiving, LSB first.
	    {
		uint8_t data ;				// Use a temporary local storage
	 	data = SwUartRXBitCount ;
    	        if( data < 8 )                          // If 8 bits are not yet read
		{
    	            SwUartRXBitCount = data + 1 ;
		    data = SwUartRXData ;
		    data >>= 1 ;		        // Shift due to receiving LSB first.
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
    	        else	//Done receiving =  8 bits are in SwUartRXData
		{
#if DEBUGASERIAL
	            PORTC &= ~1 ;
#endif
		    if ( LastRx == 0x7E )
		    {
			if ( SwUartRXData == SENSOR_ID )
			{
		          // This is us
			  struct t_sportData *pdata = ThisSportData ;
			  FORCE_INDIRECT( pdata ) ;
			  //if ( pdata )	             // We have something to send
			  if   ( sendStatus == LOADED ){     
			    if ( pdata->dataLock == 0 ) {
/*			    This part can help debug the transmission of a value,      Mike it could be set in a #ifdef
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
 				TxSportData[0] = pdata->data[0] ;
 			        TxSportData[1] = pdata->data[1] ;
 				TxSportData[2] = pdata->data[2] ;
 				TxSportData[3] = pdata->data[3] ;
 				TxSportData[4] = pdata->data[4] ;
 				TxSportData[5] = pdata->data[5] ;
 				TxSportData[6] = pdata->data[6] ;
                                
			    }
			    else
			    {	// Discard frame to be sent
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
                            OCR1A += ( 400*16 - TICKS2WAITONESPORT) ;		// 400 uS gap before sending
			  }
			  else
			  {
			    // Wait for idle time
			    state = WAITING ;
			    OCR1A += ((uint16_t)3500 * 16 ) ;		// 3.5mS gap before listening
			  }
		        }
			else // it is not the expected device ID
			{
			    state = WAITING ;
			    OCR1A += ((uint16_t)3500 * 16 ) ;		// 3.5mS gap before listening
			    SportSync = 1 ;
			}
		  }
		  else // Previous code is not equal to x7E 
	          {
		    	DISABLE_TIMER_INTERRUPT() ;      		// Stop the timer interrupts.
	  	        state = IDLE ;                                  // Go back to idle.
			PCIFR = ( 1<<PCIF2 ) ;				// clear pending interrupt
			PCICR |= ( 1<<PCIE2 ) ;				// pin change interrupt enabled
//PORTC |= 2 ;
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
	    //DISABLE_TIMER0_INT() ;		    // For the byte duration
#if DEBUGASERIAL
	    PORTC &= ~1 ;
#endif
  	    break ;
//#endif // end of Frsky_Port

	case WAITING :
//#ifdef FRSKY_SPORT
    	       DISABLE_TIMER_INTERRUPT() ;		// Stop the timer interrupts.
	       state = IDLE ;                           // Go back to idle.
	       PCIFR = ( 1<<PCIF2 ) ;			// clear pending interrupt
	       PCICR |= ( 1<<PCIE2 ) ;			// pin change interrupt enabled
                break ;

  // Unknown state.
	  default:        
  	      state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
 } // end sportAvailable == true
 else { //  ++++++++ here only for Hub protocol ++++++++++++++++++++++++++++++++++
  switch (state)
	{
  // Transmit Byte.
	  case TRANSMIT :		// Output the TX buffer.************ on envoie des bits de data
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
        	else		//Send stop bit.
    			{
        	  CLEAR_TX_PIN();                         // Output a logic 1.
        	  state = TRANSMIT_STOP_BIT;
                //ENABLE_TIMER0_INT() ;	                  // Allow this in now.
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
		//ENABLE_TIMER0_INT() ;									// Allow this in now.
    	        OCR1A += TICKS2WAITONEHUB ;  // Count one period into the future.
                CLEAR_TIMER_INTERRUPT( ) ;
*/
#if DEBUGASERIAL
	      PORTC &= ~1 ;
#endif
  	      break ;

  // Go to idle after stop bit was sent.
          case TRANSMIT_STOP_BIT: //************************************* We send a stop bit
		if ( ++TxCount < TxMax) 		// Have we sent all bytes?
		{
                  SwUartTXData = TxHubData[TxCount] ;			        
                  SET_TX_PIN() ;                        // Send a logic 0 on the TX_PIN.
	          OCR1A = TCNT1 + TICKS2WAITONEHUB ;       // Count one period into the future.
		  SwUartTXBitCount = 0 ;
		  state = TRANSMIT ;
		  //DISABLE_TIMER0_INT() ;		// For the byte duration
		}
	        else  // all bytes have been send
		{
		  TxCount = 0 ;
                  TxMax = 0 ;
                  state = WAITING ;
                  //sendStatus = SEND ;
                  OCR1A += ((uint16_t)100 * 16 ) ;	// 100uS gap
		  //OCR1A += ((uint16_t)3500 * 16 ) ;	// 3.5mS gap before listening
		  //TRXDDR &= ~( 1 << PIN_SERIALTX );   // PIN is input, tri-stated.
		  //TRXPORT &= ~( 1 << PIN_SERIALTX );  // PIN is input, tri-stated.
       		  
                  struct t_hubData *pdata = ThisHubData ;
		  FORCE_INDIRECT( pdata ) ;
                  

		  //pdata->serialSent = 1 ;
		  //DataSent = 1 ;
		  //SportSync = 1 ;
		  //pdata = pdata->next ;
		  //if ( pdata == 0 )		        // Wrap at end
		  //  {
		  //	pdata = FirstData ;
		  //  }
		  //ThisData = pdata ;
		}

// here original code
/*
		if ( TxNotEmpty )
		{
                        SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
	  	        OCR1A = TCNT1 + TICKS2WAITONE ;   // Count one period into the future.
			SwUartTXData = ByteToSend ;	  // Grab byte
			TxNotEmpty = 0 ;		  // Mark 'sent'
 			state = TRANSMIT ;
		        SwUartTXBitCount = 0 ;
		}
		else
		{
			OCR1A += ((uint16_t)100*16) ;	   // 100uS gap 
	                state = WAITING ;                  // Go back to idle.
		}
*/
               break ;
               

	case WAITING :
  	
    	       DISABLE_TIMER_INTERRUPT() ;		// Stop the timer interrupts.
	       state = IDLE ;                           // Go back to idle.
//	       PCIFR = (1<<PCIF2) ;			// clear pending interrupt
//	       PCICR |= (1<<PCIE2) ;			// pin change interrupt enabled

// here original code from Mike
/*
		if ( TxNotEmpty )
		{
                        SET_TX_PIN() ;                    // Send a logic 0 on the TX_PIN.
	  	        OCR1A = TCNT1 + TICKS2WAITONE ;   // Count one period into the future.
			SwUartTXData = ByteToSend ;	  // Grab byte
			TxNotEmpty = 0 ;		  // Mark 'sent'
 			state = TRANSMIT ;
		        SwUartTXBitCount = 0 ;
		}
		else
		{
    	                DISABLE_TIMER_INTERRUPT() ;	   // Stop the timer interrupts.
	                state = IDLE ;                     // Go back to idle.
		}
*/

		
                break ;

  // Unknown state.
	  default:        
  	      state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE

 } // end "else sport" = end Hub  
} // End of ISR



// End of the code for both protocols +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//____________________Here the code for SPORT interface only ++++++++++++++++++++++++++++++++++++++++++

//brief  Function to initialize the UART for Sport protocol
//  This function will set up pins to transmit and receive on. Control of Timer0 and External interrupt 0.
void initSportUart( struct t_sportData *pdata )           //*************** initialise UART pour SPORT
{
    FirstData = ThisSportData = pdata ;
    
    //PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input, tri-stated.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is input, tri-stated.

  // External interrupt
  
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;			// IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif

    PCIFR = (1<<PCIF2) ;	// clear pending interrupt
    PCICR |= (1<<PCIE2) ;	// pin change interrupt enabled

    // Internal State Variable
    state = IDLE ;

#if DEBUGASERIAL
  DDRC = 0x03 ;		// PC0,1 as o/p debug
  PORTC = 0 ;
#endif
}


void setSportNewData( struct t_sportData *pdata, uint16_t id, uint32_t value )
{
	pdata->dataLock = 1 ;
	pdata->data[0] = 0x10 ;
	pdata->data[1] = id ; // low byte
	pdata->data[2] = id >> 8 ; // hight byte
	pdata->data[3] = value ;
	pdata->data[4] = value >> 8 ;
	pdata->data[5] = value >> 16 ;
	pdata->data[6] = value >> 24 ;
	pdata->dataLock = 0 ;
#ifdef DEBUGSETNEWDATA                                        // Mike this code was for debug only, can be removed or set with #ifdef 
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
	if ( TRXPIN & ( 1 << PIN_SERIALTX ) )			// Pin is high = start bit (inverted)
	{
		PCICR &= ~( 1<<PCIE2 ) ;			// pin change interrupt disabled
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
}






//____________________Here the code for HUB interface only ++++++++++++++++++++++++++++++++++++++++++
//brief  Function to initialize the UART for Sport protocol
//  This function will set up pins to transmit and receive on. Control of Timer0 and External interrupt 0.
void initHubUart( struct t_hubData *pdata )
{
  ThisHubData = pdata ;
  TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is low
  TRXDDR |= ( 1 << PIN_SERIALTX ) ;        // PIN is output.
	
  //Internal State Variable
  state = IDLE ;
  TxMax = 0 ;
  TxCount = 0 ;

#if DEBUGASERIAL
  DDRC = 0x03 ;		// PC0,1 as o/p debug
  PORTC = 0 ;
#endif
}

void setHubNewData( struct t_hubData *pdata )
{
  if ( (TxCount == 0) && (pdata->maxData > 0) ) {
    for (int cntNewData = 0 ; cntNewData < pdata->maxData ; cntNewData++) {
          TxHubData[cntNewData] = pdata->data[cntNewData] ;
      }
      TxMax = pdata->maxData  ;
#ifdef DEBUGSETNEWDATA
          Serial.print("set new data at ");
          Serial.print( millis() );
          Serial.print(" ");  
          for (int cntNewData = 0 ; cntNewData < pdata->maxData ; cntNewData++) {
            Serial.print( pdata->data[cntNewData] , HEX );
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
	SwUartTXData = TxHubData[0] ;
	//TxNotEmpty = 0 ;
        state = TRANSMIT ;
 	ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#if DEBUGASERIAL
	PORTC &= ~1 ;
#endif

}

// end of function that are hub specific


