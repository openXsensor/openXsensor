// Author: Mike Blandford

#include "oxs_general.h"

#ifdef DEBUG
//#define xxxxxxxxxx
#endif

uint16_t MillisPrecount ;
uint16_t lastTimerValue ;
uint32_t TotalMicros ;
uint32_t TotalMillis ;

uint32_t lastEventTime ;
uint8_t RpmCounter ;


// RPM code
#ifdef MEASURE_RPM
ISR( TIMER1_CAPT_vect, ISR_NOBLOCK )
{
	uint16_t elapsed ;
	uint32_t eventTime ;
	uint32_t difference ;

	if ( ++RpmCounter > 3 )
	{
		cli() ;
		uint16_t time = ICR1 ;	// Read timer 1
		sei() ;
		elapsed = time - lastTimerValue ;
  #if F_CPU == 20000000L   // 20MHz clock 
   #error Unsupported clock speed
  #elif F_CPU == 16000000L  // 16MHz clock                                                  
    		eventTime = TotalMicros + ( elapsed >> 4 ) ;
  #elif F_CPU == 8000000L   // 8MHz clock
  		eventTime = TotalMicros + ( elapsed >> 3 ) ;
    #else
    #error Unsupported clock speed
  #endif

		//eventTime = TotalMicros + ( elapsed >> 4 ) ;
		RpmCounter = 0 ;
		difference = eventTime - lastEventTime ;
		lastEventTime = eventTime ;
		if ( difference > 200 )
		{
			RpmValue = 4000000 / difference ;
		}
		else
		{
			RpmValue = 0 ;
		}
		RpmSet = true ;
	}
}

#endif // MEASURE_RPM

uint32_t micros()
{
	uint16_t elapsed ;
	uint8_t millisToAdd ;
	uint8_t oldSREG = SREG ;
	cli() ;
	uint16_t time = TCNT1 ;	// Read timer 1
	SREG = oldSREG ;

	elapsed = time - lastTimerValue ;
	
 #if F_CPU == 20000000L   // 20MHz clock 
   #error Unsupported clock speed
  #elif F_CPU == 16000000L  // 16MHz clock                                                  
        elapsed >>= 4 ;
  #elif F_CPU == 8000000L   // 8MHz clock
        elapsed >>= 3 ;
    #else
    #error Unsupported clock speed
  #endif

        //elapsed >>= 4 ;
	
	uint32_t ltime = TotalMicros ;
	ltime += elapsed ;
	cli() ;
	TotalMicros = ltime ;	// Done this way for RPM to work correctly
	lastTimerValue = time ;
	SREG = oldSREG ;	// Still valid from above
	
	elapsed += MillisPrecount;
	millisToAdd = 0 ;
	if ( elapsed  > 3999 )
	{
		millisToAdd = 4 ;
		elapsed -= 4000 ;
	}
	else if ( elapsed  > 2999 )
	{
		millisToAdd = 3 ;		
		elapsed -= 3000 ;
	}
	else if ( elapsed  > 1999 )
	{
		millisToAdd = 2 ;
		elapsed -= 2000 ;
	}
	else if ( elapsed  > 999 )
	{
		millisToAdd = 1 ;
		elapsed -= 1000 ;
	}
	TotalMillis += millisToAdd ;
	MillisPrecount = elapsed ;
	return TotalMicros ;
}

// retrun the number of milli second
uint32_t millis()
{
	micros() ;
	return TotalMillis ;
}


void init()
{
  // Timer1
  TIMSK1 &= ~( 1<< OCIE1A ) ; // Disable interupt on timer 1 for compA
  TCCR1A = 0x00 ;    //Init.
  TCCR1B = 0xC1 ;    // I/p noise cancel, rising edge, Clock/1

  //  initialise le ADC converter
#if defined(ADCSRA) 
	// set a2d prescale factor to 128
	// 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
	// XXX: this will not work properly for other clock speeds, and
	// this code should use F_CPU to determine the prescale factor.
	//sbi(ADCSRA, ADPS2);
	//sbi(ADCSRA, ADPS1);
	//sbi(ADCSRA, ADPS0);
  #if ( F_CPU == 20000000L ) || ( F_CPU == 16000000L )   // 20 MHz or 16 MHz clock
        // set a2d prescale factor to 128
        // 20 MHz / 128 = 156.25 KHz
        // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
        // XXX: this will not work properly for other clock speeds, and          
        // this code should use F_CPU to determine the prescale factor.
        sbi(ADCSRA, ADPS2);
        sbi(ADCSRA, ADPS1);
        sbi(ADCSRA, ADPS0);
  #elif F_CPU == 8000000L    // 8MHz clock        
        // set a2d prescale factor to 64
        // 8 MHz / 64 = 125 KHz, inside the desired 50-200 KHz range.
        sbi(ADCSRA, ADPS2);
        sbi(ADCSRA, ADPS1);
        cbi(ADCSRA, ADPS0);
  #else
    #error Unsupported clock speed
  #endif
  



	// enable a2d conversions
	sbi(ADCSRA, ADEN);
#endif

	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
#if defined(UCSRB)
	UCSRB = 0;
#elif defined(UCSR0B)
	UCSR0B = 0;
#endif

#ifdef MEASURE_RPM
	DDRB &= ~0x01 ;	// Pin is input
	PORTB |= 1 ; 		// With pullup
	sbi( TIMSK1, ICIE1 ) ; // allow change interrupt 
#endif // MEASURE_RPM
	sei();  //allow interrupt in general

// timer2 (used by analoWrite to generate PWM
	// set timer 2 prescale factor to 64
#if defined(TCCR2) && defined(CS22)
	sbi(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
	sbi(TCCR2B, CS22);
#else
	#warning Timer 2 not finished (may not be present on this CPU)
#endif

	// configure timer 2 for phase correct pwm (8-bit)
#if defined(TCCR2) && defined(WGM20)
	sbi(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
	sbi(TCCR2A, WGM20);
#else
	#warning Timer 2 not finished (may not be present on this CPU)
#endif
 

}

void delay(unsigned long ms)
{
	uint16_t start = (uint16_t)micros();
	uint16_t lms = ms ;

	while (lms > 0) {
		if (((uint16_t)micros() - start) >= 1000) {
			lms--;
			start += 1000;
		}
	}
}
 
// Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock. 
void delayMicroseconds(unsigned int us)
{
//	 calling avrlib's delay_us() function with low values (e.g. 1 or
//	 2 microseconds) gives delays longer than desired.
//	delay_us(us);
#if F_CPU >= 20000000L
//	 for the 20 MHz clock on rare Arduino boards

//	 for a one-microsecond delay, simply wait 2 cycle and return. The overhead
//	 of the function call yields a delay of exactly a one microsecond.
	__asm__ __volatile__ (
		"nop" "\n\t"
		"nop"); //just waiting 2 cycle
	if (--us == 0)
		return;

//	 the following loop takes a 1/5 of a microsecond (4 cycles)
//	 per iteration, so execute it five times for each microsecond of
//	 delay requested.
	us = (us<<2) + us; // x5 us

//	 account for the time taken in the preceeding commands.
	us -= 2;

#elif F_CPU >= 16000000L
//	 for the 16 MHz clock on most Arduino boards

//	 for a one-microsecond delay, simply return.  the overhead
//	 of the function call yields a delay of approximately 1 1/8 us.
	if (--us == 0)
		return;

//	 the following loop takes a quarter of a microsecond (4 cycles)
//	 per iteration, so execute it four times for each microsecond of
//	 delay requested.
	us <<= 2;

//	 account for the time taken in the preceeding commands.
	us -= 2;
#else
//	 for the 8 MHz internal clock on the ATmega168

//	 for a one- or two-microsecond delay, simply return.  the overhead of
//	 the function calls takes more than two microseconds.  can't just
//	 subtract two, since us is unsigned; we'd overflow.
	if (--us == 0)
		return;
	if (--us == 0)
		return;

//	 the following loop takes half of a microsecond (4 cycles)
//	 per iteration, so execute it twice for each microsecond of
//	 delay requested.
	us <<= 1;
    
//	 partially compensate for the time taken by the preceeding commands.
//	 we can't subtract any more than this or we'd overflow w/ small delays.
	us--;
#endif

//	 busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}


//#define delay_us(x) {unsigned char _dcnt; \
//                      _dcnt = (x)/(24000000UL/FOSC)|1; \
//                      while(--_dcnt !=0) continue; \
//                    }



