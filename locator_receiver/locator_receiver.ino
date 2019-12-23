#include "lora.h"

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C




extern uint8_t oXsGpsPdop ;    // gps precision sent by oxs
extern uint8_t oXsLastGpsDelay ; // delay since last gps fix at oxs side
extern uint8_t oXsPacketRssi ;   // RSSI of last packet received by oXs

extern uint32_t loraLastPacketReceivedMillis ;
uint32_t prevLoraLastPacketReceivedMillis ;
extern uint32_t loraLastGpsPacketReceivedMillis ;
extern int loraRxPacketRssi ;
extern float loraRxPacketSnr ;
extern int32_t lastGpsLon ; 
extern int32_t lastGpsLat ;    

uint8_t lonDegree ;
uint8_t lonMinute ;
uint8_t lonSeconde ;
uint8_t lonSecDec ;
uint8_t latDegree ;
uint8_t latMinute ;
uint8_t latSeconde ;
uint8_t latSecDec ;

SSD1306AsciiAvrI2c oled;


char *oXsLastGpsDelayText[8] = { "No" , "<1 s" , "<10 s" , "<1 m" ,"<10 m" , "<1 h" , "<10 h" , ">10 h" } ;


void print2Pos(uint8_t n) { // print on oled adding a space before if n is less than 10 
  if ( n < 10 ) oled.print(" "); 
  oled.print(n);
}


void convertLonLat( int32_t GPS_LatLon, uint8_t &_degree , uint8_t & _minute , uint8_t & _seconde, uint8_t & _secDec  ) {
  uint32_t GPS_LatLonAbs ;
  uint32_t minute7decimals ;
  uint32_t seconde7decimals ;
  uint32_t secondeDec7decimals ;
  //Serial.print("LonLat= ") ; Serial.println( GPS_LatLon ) ;
  GPS_LatLonAbs = ( GPS_LatLon < 0 ? - GPS_LatLon : GPS_LatLon)  ; 
  _degree = ( GPS_LatLonAbs / 10000000 )  ;                              // extract the degre without  decimal
  minute7decimals = (GPS_LatLonAbs % 10000000 ) * 60;
  _minute =  minute7decimals / 10000000 ;
  seconde7decimals = (minute7decimals % 10000000 ) * 60 ;
  _seconde = seconde7decimals / 10000000 ;
  secondeDec7decimals =  ( seconde7decimals % 10000000 ) * 10 ;
  _secDec = secondeDec7decimals / 10 ;
  //Serial.print("degree= ") ; Serial.println( (int32_t)_degree ) ;
}
void setup() {
  Serial.begin(115200) ;
  Serial.println("start lora") ;
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!");

}

uint8_t loraDebugCode ;
uint8_t countTxToDebug = 0 ; // just to debug
uint8_t countRxToDebug = 0 ; // just to debug


void loop() {
  loraDebugCode = loraHandle();
  if ( loraDebugCode ) fillOled() ; // this for debugging
  if ( loraDebugCode == 1 ) countTxToDebug++ ;
  if ( loraDebugCode == 2 ) countRxToDebug++ ; 
  
  if ( loraLastPacketReceivedMillis != prevLoraLastPacketReceivedMillis ) { // if we receive a new packet, then print
    prevLoraLastPacketReceivedMillis = loraLastPacketReceivedMillis ;
    
    convertLonLat( lastGpsLon , lonDegree , lonMinute , lonSeconde , lonSecDec ) ; 
    //Serial.print("lonDegree back= ") ; Serial.println(lonDegree) ; 
    convertLonLat( lastGpsLat , latDegree , latMinute , latSeconde , latSecDec ) ;
    fillOled() ;
    //Serial.print( "At=") ;  Serial.print( loraLastPacketReceivedMillis ) ; Serial.print( " lastGpsAt=") ;  Serial.print( loraLastGpsPacketReceivedMillis ) ;
    //Serial.print( " pdop=") ;  Serial.print( oXsGpsPdop ) ; Serial.print( " GpsDelay=") ;  Serial.print( oXsLastGpsDelay ) ;
    //Serial.print( " oXsRSSI=") ;  Serial.print( oXsPacketRssi ) ;   
    //Serial.print( " RxRSSI=") ;  Serial.print( loraRxPacketRssi ) ; Serial.print( " RxSNR=") ;  Serial.print( loraRxPacketSnr ) ; 
    //Serial.print( " Long=") ; Serial.print( lonDegree ) ; Serial.print( "° ") ; Serial.print( lonMinute ) ; Serial.print( "min ") ; Serial.print( lonSeconde ) ; Serial.print( "sec ") ; Serial.print( lonSecDec ) ;
    //Serial.print( " Lat=") ; Serial.print( latDegree ) ; Serial.print( "° ") ; Serial.print( latMinute ) ; Serial.print( "min ") ; Serial.print( latSeconde ) ; Serial.print( "sec ") ; Serial.print( latSecDec ) ;
    //Serial.println( " ") ;
  }
  
}

void fillOled() {
  static uint8_t countToDebug = 0 ; // just to debug
  //oled.clear() ;
  oled.setCursor(0,0) ;
//  oled.print("Tx= ");
//  oled.print(countTxToDebug) ;
//  oled.setCursor(0,2) ;
//  oled.print("Rx= ");
//  oled.print(countRxToDebug) ;
  
  
  if (lastGpsLon >= 0) {
    oled.print("E ");
  } else {
    oled.print("O ");
  }
  print2Pos(lonDegree) ; oled.print("  ");
  print2Pos(lonMinute) ; oled.print("' ");
  print2Pos(lonSeconde) ; oled.print(".");
  oled.print(lonSecDec); oled.print("\""); oled.clearToEOL() ;
  
  oled.setCursor(0,1) ;
  if (lastGpsLat >= 0) {
    oled.print("N ");
  } else {
    oled.print("S ");
  }
  print2Pos(latDegree) ; oled.print("  ");
  print2Pos(latMinute) ; oled.print("' ");
  print2Pos(latSeconde) ; oled.print(".");
  oled.print(latSecDec); oled.print("\"");oled.clearToEOL() ;
  
  oled.setCursor(0, 2) ; oled.print("dop "); oled.print(oXsGpsPdop); oled.print( " ") ;
  oled.setCursor(75, 2) ; oled.print("GPS "); oled.print(oXsLastGpsDelayText[oXsLastGpsDelay]); oled.clearToEOL() ;
  oled.setCursor(0, 3) ; oled.print("oXs Rssi = "); oled.print(oXsPacketRssi);  oled.clearToEOL() ;
  //oled.setCursor(0, 4) ; oled.print("Last gps rec.  "); oled.print( ( millis() - loraLastPacketReceivedMillis ) /1000); oled.print( " s") ; oled.clearToEOL() ;
  
  oled.setCursor(0, 5) ; oled.print("Last pack rec. ");
  uint32_t delayLastPacketReceived ;
  delayLastPacketReceived = millis() - loraLastPacketReceivedMillis ;
  if ( delayLastPacketReceived < 60000 )  {
    oled.print( delayLastPacketReceived /1000); oled.print( " s   ") ;
  } else {
    oled.print( delayLastPacketReceived /60000); oled.print( " m   ") ;
  }
  oled.setCursor(0, 7) ; oled.print("RSSI = "); oled.print(loraRxPacketRssi); oled.print( "   ") ;
  oled.setCursor(75, 7) ;
  oled.print("SNR = "); oled.print(loraRxPacketSnr); oled.clearToEOL() ;

}

