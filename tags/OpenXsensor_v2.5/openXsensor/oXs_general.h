#ifndef OXS_GENERAL_h
#define OXS_GENERAL_h

#include "oXs_config.h"
#include <Arduino.h>
#include "oXs_out_frsky.h"          // one variable is shared between both files


uint32_t micros( void ) ;
uint32_t millis( void ) ;


//Some IO, timer and interrupt specific defines.

#define ENABLE_TIMER_INTERRUPT( )       ( TIMSK1 |= ( 1<< OCIE1A ) )
#define DISABLE_TIMER_INTERRUPT( )      ( TIMSK1 &= ~( 1<< OCIE1A ) )
#define CLEAR_TIMER_INTERRUPT( )        ( TIFR1 = (1 << OCF1A) )
                                                                          // this code for TIMER0 is probably not required anymore.
#define	DISABLE_TIMER0_INT()		( TIMSK0 &= ~( 1<<TOIE0 ) ) //(Timer 0 is used by arduino lib micros() and millis()   
#define	ENABLE_TIMER0_INT()		( TIMSK0 |= ( 1<<TOIE0 ) )

#define TCCR             TCCR1A             //!< Timer/Counter Control Register
#define TCCR_P           TCCR1B             //!< Timer/Counter Control (Prescaler) Register
#define OCR              OCR1A              //!< Output Compare Register
#define EXT_IFR          EIFR               //!< External Interrupt Flag Register
#define EXT_ICR          EICRA              //!< External Interrupt Control Register

#define TRXDDR  DDRD
#define TRXPORT PORTD
#define TRXPIN  PIND

#define SET_TX_PIN( )    ( TRXPORT |= ( 1 << PIN_SERIALTX ) )
#define CLEAR_TX_PIN( )  ( TRXPORT &= ~( 1 << PIN_SERIALTX ) )
#define GET_RX_PIN( )    ( TRXPIN & ( 1 << PIN_SERIALTX ) )

#endif // end #define OXS_GENERAl_h



//#define FOSC 16000000UL // internal clock of 16 mhz
#if F_CPU   == 20000000L  // for the 20 MHz clock on rare Arduino boards
  #define FOSC 20000000UL 
#elif F_CPU == 16000000L  // for the 16 MHz clock on most Arduino boards
  #define FOSC 16000000UL 
#elif F_CPU == 8000000L   // for the 8 MHz internal clock
  #define FOSC 8000000UL 
#else
  #error Unsupported clock speed
#endif


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

