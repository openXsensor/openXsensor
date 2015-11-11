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

SoftwareSerial mySerial(255 , 10); // RX, TX , optional inverted logic // so here use only pin 10 as output to send the bytes
  
void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("Serial setup");

  // set the data rate for the SoftwareSerial port used is read mode

}

void loop() // run over and over
{
  mySerial.begin(19200);    // set the data rate for the SoftwareSerial port used is read mode
  mySerial.write(0x80);     // byte to says it is a binary format
  mySerial.write(0x8D);     // byte to say it is a GAM (general air module)
  mySerial.end() ;          // set pin in high impedance (I hope)
  delay(200) ;              // wait 200 msec to let the sensor answer.
  
  mySerial.begin(19200);    // set the data rate for the SoftwareSerial port used is read mode
  mySerial.write(0x80);     // byte to says it is a binary format
  mySerial.write(0x8A);     // byte to say it is a GPS 
  mySerial.end() ;          // set pin in high impedance (I hope)
  delay(200) ;              // wait 200 msec to let the sensor answer.
  
}

