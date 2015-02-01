/*
  Software serial multple serial test
 
 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.
 
 The circuit: 
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)
 
 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts, 
 so only the following can be used for RX: 
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
 
 Not all pins on the Leonardo support change interrupts, 
 so only the following can be used for RX: 
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 
 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example
 
 This example code is in the public domain.
 
 */
#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10, 11); // RX, TX
int ledPin = 13;                 // LED connected to digital pin 13
int ad = 0 ;
  
void initUart() ;
void sendChar( uint8_t toSend) ;
  
void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("Serial setup");

  // set the data rate for the SoftwareSerial port used is read mode
//  mySerial.begin(38400);
//  mySerial.println("Hello, world?");

  pinMode(ledPin, OUTPUT);      // sets the digital pin as output 
  
  initUart() ;
}

void loop() // run over and over
{
  cli() ;
  sendChar(ad);
  sei() ; 
  ad++ ;
  if ( ad > 15 ) ad = 0 ;
  delay(6) ;
/*  
  if (mySerial.available()) {
    do {  
      mySerial.read() ;
    } while (mySerial.available()) ;   
    digitalWrite(ledPin, HIGH);   // sets the LED on

  } else {
        digitalWrite(ledPin, LOW);    // sets the LED off

  }
*/  
}

// specific software is use to simule multiplex RX
// pin 10 is used here (could be anay other
// pin 10 is normally configured as input with pull up enabled, so level is normally high
// When a byte must be sent, a change is require to force output mode with 0 e.g for the startbit 

void initUart() {
    //arduino pin 10 is on avr pin 14 = PB2    
    //PORT
    DDRB &= ~( 1 << DDB2 ) ;       // PIN is input.
    PORTB &= ~( 1 << PORTB2 ) ;      // PIN is tri-stated.
}

void sendChar( uint8_t toSend) {
  // baud rate is 38 400 => 1000000 / 38400 = 26 usec
   DDRB |= ( 1 << DDB2 ) ;       // PIN is output.
// PORTB stay low  
   delayMicroseconds(26); 
   for (uint8_t i= 0 ; i<8 ; i++) {
       if( toSend & 0x01 ) {           // If the LSB of the TX buffer is 1:
      	      DDRB &= ~( 1 << DDB2 ) ;       // PIN is input = level is high
      	} else {                                // Otherwise:
      	    DDRB |= ( 1 << DDB2 ) ;       // PIN is output = level is low
      	}
      	toSend = toSend >> 1 ;    // Bitshift the TX buffer and
        delayMicroseconds(26); 
  } 
  DDRB &= ~( 1 << DDB2 ) ;       // PIN is input = level is high
  delayMicroseconds(26);
}
