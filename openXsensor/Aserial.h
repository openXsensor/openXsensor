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

// Author: Mike Blandford
#ifndef ASERIAL_h
#define ASERIAL_h

#include "oXs_config.h"
#include <Arduino.h>
#include "oXs_out_frsky.h"          // one variable is shared between both files
#include "oXs_out_multiplex.h"          // one variable is shared between both files
#include "oXs_general.h"


//#ifdef FRSKY_SPORT
struct t_sportData
{
	struct t_sportData *next ;
	uint8_t data[7] ;
	uint8_t dataLock ;
	uint8_t serialSent ;
} ;

void setSportNewData( struct t_sportData *pdata, uint16_t id, uint32_t value ) ;
void setMultiplexNewData( struct t_sportData *pdata, uint16_t id, uint32_t value ) ;
void initSportUart( struct t_sportData *pdata ) ;
//void initMultiplexUart( struct t_sportData *pdata ) ;
void initMultiplexUart( struct t_mbAllData *pdata ) ;

extern uint8_t DataSent ;
extern uint8_t SportSync ;


extern volatile uint8_t sendStatus ;

extern volatile uint8_t debug01 ;
extern volatile uint8_t debug02 ;
extern volatile uint8_t debug03 ;
extern volatile uint8_t debug04 ;


//#else // Hub protocol ==========================================================

#define maxSizeBuffer 70  // max size of the buffer used to store the data to be sent in the hub protocol

struct t_hubData
{
	uint8_t data[maxSizeBuffer] ; 
	uint8_t currentData ; //index of current data
	uint8_t maxData ;   // max number of data prepared to be send
} ;

void initHubUart( struct t_hubData *pdata ) ;
//void sendHubByte( uint8_t byte ) ;
void setHubNewData( struct t_hubData *pdata ) ;
void startHubTransmit( void ) ;

//#endif // FRSKY_SPORT + HUB

 
extern volatile bool RpmSet ;
extern volatile uint16_t RpmValue ;
extern volatile bool sportAvailable ;

uint32_t micros( void ) ;
uint32_t millis( void ) ;

// UART's state.
#define   IDLE               0        // Idle state, both transmit and receive possible.
#define   TRANSMIT           1        // Transmitting byte.
#define   TRANSMIT_STOP_BIT  2        // Transmitting stop bit.
#define   RECEIVE            3        // Receiving byte.
#define	  TxPENDING          4
#define	  WAITING            5

// 57600 = Desired baudrate for Sport protocol = 17 micro sec per bit.
// 9600   =  Desired baudrate for Hub protocol
//This section chooses the correct timer values for the Sport protocol = 57600 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         278  // Ticks between two bits.
//#define TICKS2WAITONE       278  // Wait one bit period.
//#define TICKS2WAITONE_HALF  416	 // Wait one and a half bit period.
  #if F_CPU == 20000000L   // 20MHz clock 
    // Sinan: Not tested                                                     
    #define TICKS2COUNTSPORT         348  // Ticks between two bits.
    #define TICKS2WAITONESPORT       348  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  520    // Wait one and a half bit period.
  #elif F_CPU == 16000000L  // 16MHz clock                                                  
    #define TICKS2COUNTSPORT         278  // Ticks between two bits.
    #define TICKS2WAITONESPORT       278  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  416    // Wait one and a half bit period.
  #elif F_CPU == 8000000L   // 8MHz clock
    // Assumes a 8MHz clock                                                   
    #define TICKS2COUNTSPORT         139  // Ticks between two bits.
    #define TICKS2WAITONESPORT       139  // Wait one bit period.
    #define TICKS2WAITONE_HALFSPORT  208    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif

//This section chooses the correct timer values for Hub protocol = 9600 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         (278*6)  // Ticks between two bits.
//#define TICKS2WAITONE       (278*6)  // Wait one bit period.
//#define TICKS2WAITONE_HALF  (416*6)	 // Wait one and a half bit period.
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    // Sinan: Not tested
    #define TICKS2COUNTHUB         (348*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (348*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (520*6)    // Wait one and a half bit period.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define TICKS2COUNTHUB         (278*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (278*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (416*6)    // Wait one and a half bit period.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define TICKS2COUNTHUB         (139*6)  // Ticks between two bits.
    #define TICKS2WAITONEHUB       (139*6)  // Wait one bit period.
    #define TICKS2WAITONE_HALFHUB  (208*6)    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif

//This section chooses the correct timer values for Multiplex protocol = 38400 baud.
// Assumes a 16MHz clock
//#define TICKS2COUNT         (278*6)  // Ticks between two bits.
//#define TICKS2WAITONE       (278*6)  // Wait one bit period.
//#define TICKS2WAITONE_HALF  (416*6)	 // Wait one and a half bit period.
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    // Sinan: Not tested
    #define TICKS2COUNTMULTIPLEX         (521)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (521)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (781)    // Wait one and a half bit period.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define TICKS2COUNTMULTIPLEX         (417)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (417)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (625)    // Wait one and a half bit period.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define TICKS2COUNTMULTIPLEX         (208)  // Ticks between two bits.
    #define TICKS2WAITONEMULTIPLEX       (208)  // Wait one bit period.
    #define TICKS2WAITONE_HALFMULTIPLEX  (313)    // Wait one and a half bit period.
  #else
    #error Unsupported clock speed
  #endif



//#define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
//#define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
// INTERRUPT_EARLY_BIAS is to bias the sample point a bit early in case
// the Timer 0 interrupt (5.5uS) delays the start bit detection
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define INTERRUPT_EXEC_CYCL   112       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  40       // Cycles to allow of other interrupts.
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define INTERRUPT_EXEC_CYCL   90       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  32       // Cycles to allow of other interrupts.
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define INTERRUPT_EXEC_CYCL   45       // Cycles to execute interrupt routines from interrupt.
    #define INTERRUPT_EARLY_BIAS  16       // Cycles to allow of other interrupts.
  #else
    #error Unsupported clock speed
  #endif

// this section define some delays used in Aserial; values can be used by any protocol
  #if F_CPU == 20000000L     // 20MHz clock                                                  
    #define DELAY_4000  ((uint16_t)4000.0 * 20.0 /16.0 )
    #define DELAY_3500  ((uint16_t)3500.0 * 20.0 /16.0 )    
    #define DELAY_2000  ((uint16_t)2000.0 * 20.0 /16.0 )
    #define DELAY_1600  ((uint16_t)1600.0 * 20.0 /16.0 )    
    #define DELAY_400  ((uint16_t)400.0 * 20.0 /16.0 )
    #define DELAY_100  ((uint16_t)100.0 * 20.0 /16.0 )
    
  #elif F_CPU == 16000000L   // 16MHz clock                                                  
    #define DELAY_4000 ((uint16_t) (1000L * 16) )     
    #define DELAY_3500 ((uint16_t) (1000L * 16) )         
    #define DELAY_2000 ((uint16_t) (1000L * 16) )     
    #define DELAY_1600 ((uint16_t) (1000L * 16) )     
    #define DELAY_400 ((uint16_t) (400 * 16) )     
    #define DELAY_100 ((uint16_t) (100 * 16) )     
  #elif F_CPU == 8000000L    // 8MHz clock                                                   
    #define  DELAY_4000 ((uint16_t)4000L * 8 )
    #define  DELAY_3500 ((uint16_t)3500L * 8 )    
    #define  DELAY_2000 ((uint16_t)2000 * 8 )
    #define  DELAY_1600 ((uint16_t)1600 * 8 )    
    #define  DELAY_400 ((uint16_t)400 * 8 )
    #define  DELAY_100 ((uint16_t)100 * 8 )    
  #else
    #error Unsupported clock speed
  #endif



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


//used in multiplex protocol only

#define SET_TX_PIN_MB( ) ( TRXDDR &= ~( 1 << PIN_SERIALTX ) ) // in fact we put the port in high imp because the rx has a pull up resistance
#define CLEAR_TX_PIN_MB( ) ( TRXDDR |= ( 1 << PIN_SERIALTX ) ) // in fact we put the port in output mode. PORT is already set to 0 during initialisation

#define MB_MAX_ADRESS  15

#define NOT_ACTIVE     0
#define NOT_AVAILABLE  1
#define LOCKED         2
#define AVAILABLE      3

//#ifdef MULTIPLTEX
uint8_t returnState() ;
uint8_t returnDebugUartRx() ;
//#endif




#endif // end #define ASERIAL_h
