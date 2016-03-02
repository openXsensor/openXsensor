#ifndef OXS_GENERAL_h
#define OXS_GENERAL_h

#include "oXs_config.h"
#include <Arduino.h>
#include "oXs_out_frsky.h"          // one variable is shared between both files
#include "oXs_out_multiplex.h"          // one variable is shared between both files

uint32_t micros( void ) ;
uint32_t millis( void ) ;
void   blinkLed(uint8_t blinkType) ;

// ****************************** arduino ressources being used *********************************************
//  INT0 interrupt used by mpu to say that data are available; it activate a flag read in the main loop
//  INT0 or INT1 interrupt used for ppm; timer1 is read and difference between rising and failing gives ppm
//  I2C is used but without interrupt
//  Timer1 is used for delay() and millis(), for UART with Rx (use COMPA and interrupt on COMPA), for RPM Capture interrupt is used (pin change on ICP1 = pin 8); COMPB is free
//  Timer 0 and Timer 2 are free
//  PCINT2 (pin change interrupt) is used for UART with RX too ; it allows to detect the start bit sent by Rx
//  GPS: oXs sent characters by his own serial software (without interrupt)
//  GPS: oXs read characters using the hardware UART and the Arduino librairy. This librairy uses interrupt. !! It blocks other interrupts and so there is a risk that software serial becomes out of timing because handling of COMPA could be out of delay 
// *************************************************************************************************************

//Some IO, timer and interrupt specific defines.
#define ENABLE_PIN_CHANGE_INTERRUPT( )       ( PCICR |= (1<<PCIE2) )
#define DISABLE_PIN_CHANGE_INTERRUPT( )      ( PCICR &= ~( 1<<PCIE2 ) )
#define CLEAR_PIN_CHANGE_INTERRUPT( )         ( PCIFR = (1<<PCIF2) )

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

extern volatile bool RpmSet  ;
extern volatile uint16_t RpmValue ;


#endif // end #define OXS_GENERAl_h


