// File for Jeti
#include "oXs_out_jeti.h"
#include <avr/pgmspace.h>
#if defined(PROTOCOL) &&  (PROTOCOL == JETI) 

#ifdef DEBUG
// ************************* Several parameters to help debugging
//#define DEBUGSETNEWDATA
//#define DEBUGFORMATONEVALUE
//#define DEBUGJETIFRAME
//#define DEBUGADSCURRENT
//#define DEBUGSETNEWDATA
//#define DEBUGASERIAL
//#define DEBUGJETI
#endif
//#define DEBUG_FORCE_VARIODATA  // this is used to force oXs to send a fixed dummy value for Vspeed and Alt in order to test if an issue result of bmp180 or from Multiplex / Jeti protocol.
//#define DEBUG_SERIAL_RX          // this is used to generate pulses when oXs decodes a byte sent by the receiver
//#define DEBUGASERIAL             // this is used to generate pulses when oXs send a byte 

extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

//struct t_mbAllData jetiData ;
volatile uint8_t sendStatus ;
uint8_t listOfFields[16] ; // list of oXs Field Id to transmit
//struct ONE_MEASUREMENT * p_measurements[16] ;      // array of 16 pointers (each pointer point to a structure containing a byte saying if a value is available and to the value.
uint8_t listOfFieldsIdx ; // current fields being handled
uint8_t numberOfFields ; // number of fields to transmit (from 1 ... 15)
uint8_t volatile jetiData[63] = { 0x7E, 0x9F, 0x40 , 0x11 , 0xA4 ,  0xAD , 0x04, 0x00 };  // 64 = 29 bytes for data + 34 bytes for text ; buffer where Jeti frame is prepared , 7E = header, 0x?F = X frame(? = any 4 bits) , 40 = means data (TXT would be 00) + length in 6 bits - to be filled-, A400 0001 = device id ; 00  = fixed value
uint8_t volatile jetiMaxData ;   // max number of bytes prepared in the buffer to be sent
volatile uint8_t state ;                  //!< Holds the state of the software UART.
volatile uint8_t oneByteReceived ;           // Keep a flag that is true when a byte has been received
volatile uint8_t lastByteReceived ;           // Keep the last byte received
uint8_t prevByteReceived ;
uint8_t  countOfFieldsChecked  ; //   

uint32_t jetiLong ;
uint32_t jetiLat ;

static volatile unsigned char SwUartTXData ;     //!< Data to be transmitted.
static volatile uint8_t SwUartTXBit8 ; // the bit 8 has to be 0 for the fist byte in the tx buffer in the jeti protocol
static volatile uint8_t SwUartTXBitParity ; // keep the number of 1 in the data beeing sent because jeti protocol uses ODD parity 
static volatile unsigned char SwUartTXBitCount ; //!< TX bit counter.
static volatile uint8_t SwUartRXData ;           //!< Storage for received bits.
static volatile uint8_t SwUartRXBit9 ;           //!< Storage for received bit 9.
static volatile uint8_t  SwUartRXBitParity ;     //!< Storage for received parity bit.
static volatile uint8_t SwUartRXBitCount ;       //!< RX bit counter.
static volatile uint8_t TxCount ;

volatile uint8_t debugUartRx ;

volatile uint8_t ppmInterrupted ; // This flag is activated at the end of handling interrupt on Timer 1 Compare A if during this interrupt handling an interrupt on pin change (INT0 or INT1) occurs
                         // in this case, ppm will be wrong and has to be discarded       
uint8_t  nbWaitingDelay ;

uint8_t jetiMenu ;

char degreeChar[2] ;

//#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)  // we will use interrupt PCINT0 
  extern volatile uint16_t flowMeterCnt ;            // counter of pin change connected to the flow sensor (increased in the interrupt. reset every X sec when flow is processed)
  extern float currentFlow  ;                     // count the consumed ml/min during the last x sec
  extern float consumedML   ;                     // total of consumed ml since the last reset (or restart if reset is not activated); can be saved in EEPROM
  extern struct ONE_MEASUREMENT actualFlow ;             // in ml/min
  extern struct ONE_MEASUREMENT remainingFuelML ;         // in ml
  extern struct ONE_MEASUREMENT fuelPercent ;             // in % of tank capacity
  extern int16_t flowParam[8]  ;                      // table that contains the parameters to correct the ml/pulse depending on the flow ; can be loaded by SPORT in EEPROM; can also be defined in a parameter 
  extern uint8_t residuelFuel  ;                      // percentage of residual fuel
  extern uint16_t tankCapacity  ;     // capacity of fuel tank
  extern uint16_t consumedMLPrev   ;                 //Last value saved in eeprom
  extern uint16_t tankCapacityPrev ;                 //Last value saved in eeprom
  extern int16_t flowParamPrev[8]  ;                 //Last value saved in eeprom
  void processReceivedCmd( uint8_t cmd ) ;
  void upDown ( int8_t upOrDown ) ;
  void fillTxBuffer( void ) ;
  void itoc ( int16_t n , uint8_t posLastDigit ) ;

  extern void checkFlowParam() ;

#if defined (MEASURE_RPM)
  extern struct ONE_MEASUREMENT sport_rpm ;
#endif
  
//#endif
#define UP          0xD0
#define DOWN        0xB0
#define RIGHT       0xE0
#define LEFT        0x70
#define LEFT_RIGHT  0x60
#define JETI_MENU_MAX 10 // number of items in jeti menu starting with 0

#define JETI_DATA            0
#define JETI_TANK_RESET     1
#define JETI_TANK_CAPACITY  2
#define JETI_CAL_FLOW_1     3
#define JETI_CAL_FLOW_2     4
#define JETI_CAL_FLOW_3     5
#define JETI_CAL_FLOW_4     6
#define JETI_CORR_FLOW_1    7 
#define JETI_CORR_FLOW_2    8
#define JETI_CORR_FLOW_3    9 
#define JETI_CORR_FLOW_4    10 

//                                      "0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-0123456789ABCDE-
uint8_t const jetiText[161] PROGMEM = { "Flow      ml/minRemain        mlReset fuel      <=+=>          %Tank capacity   <= =>         mlCalib. Flow 1   <= =>     ml/minCalib. Corr 1   <= =>          %"}; 


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
    TRXPORT |= ( 1 << PIN_SERIALTX ) ;      //  Pull up activated.

  // Activate pin change interrupt on Tx pin  
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;                    // IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif

//    PCIFR = (1<<PCIF2) ;  // clear pending interrupt

    state = IDLE ;     // Internal State Variable

#ifdef DEBUGASERIAL
    DDRC = 0x01 ;   // PC0 as o/p debug = pin A0 !!!! is here put as output 
    PORTC = 0 ; 
#endif

#ifdef DEBUG_SERIAL_RX
    DDRC = 0x01 ;   // PC0 as o/p debug = pin A0 !!!! is here put as output 
    PORTC = 0 ; 
#endif

    degreeChar[0] = 0xB0 ; // for the symbol °, fill with 0xB0 followed by 0x00 as showed in jetiprotocol example and in datasheet (code A02 for european set of char))
    degreeChar[1] = 0x00 ;
    initJetiListOfFields() ;
  
#ifdef DEBUG
      printer->print(F("Jeti Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F(" milli="));  
      printer->println(millis());
      printer->println(F("Jeti Output Module: Setup!"));
      printer->print(F("Number of fields to send = "));
      printer->println(numberOfFields);
#endif
}


void OXS_OUT::initJetiListOfFields() {  // fill an array with the list of fields (field ID) that are defined -   Note : this list is not yet complete (e.g; GPS, ...)
  listOfFieldsIdx = 1 ; // 0 is not used for a value, because - for text- it contains the name of the sensor (e.g. openXsensor)
#if defined(VARIO) 
    listOfFields[listOfFieldsIdx++] = REL_ALTIMETER ; 
    listOfFields[listOfFieldsIdx++] = VERTICAL_SPEED ;
    listOfFields[listOfFieldsIdx++] = ALTIMETER_MAX ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 0)      
   listOfFields[listOfFieldsIdx++] = CELL_1 ;
   listOfFields[listOfFieldsIdx++] = CELL_MIN ;
   listOfFields[listOfFieldsIdx++] = CELL_TOT ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 1)      
   listOfFields[listOfFieldsIdx++] = CELL_2 ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 2)      
   listOfFields[listOfFieldsIdx++] = CELL_3 ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 3)      
   listOfFields[listOfFieldsIdx++] = CELL_4 ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 4)      
   listOfFields[listOfFieldsIdx++] = CELL_5 ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(NUMBEROFCELLS) && (NUMBEROFCELLS > 5)      
   listOfFields[listOfFieldsIdx++] = CELL_6 ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_1 || VOLTAGE_SOURCE == VOLT_2 || VOLTAGE_SOURCE == VOLT_3 || VOLTAGE_SOURCE == VOLT_4 || VOLTAGE_SOURCE == VOLT_5 || VOLTAGE_SOURCE == VOLT_6 )
    listOfFields[listOfFieldsIdx++] = VOLTAGE_SOURCE ;
#endif
#if ( defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) ) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON))
    listOfFields[listOfFieldsIdx++] =  CURRENTMA ;
    listOfFields[listOfFieldsIdx++] =  MILLIAH ;
#endif
#ifdef AIRSPEED    
    listOfFields[listOfFieldsIdx++] = AIR_SPEED ;
#elif defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_AIRSPEED_BASED_ON)    
    listOfFields[listOfFieldsIdx++] = AIR_SPEED ;
#elif defined(AIRSPEED_SDP3X)
    listOfFields[listOfFieldsIdx++] = AIR_SPEED ; 
#endif  // End airpseed    
#ifdef GPS_INSTALLED            
    listOfFields[listOfFieldsIdx++] = GPS_COURSE ;
    listOfFields[listOfFieldsIdx++] = GPS_SPEED ;
    listOfFields[listOfFieldsIdx++] = GPS_ALTITUDE ; 
    listOfFields[listOfFieldsIdx++] = GPS_DISTANCE ;
    listOfFields[listOfFieldsIdx++] = GPS_BEARING ;
    listOfFields[listOfFieldsIdx++] = GPS_LONG ;
    listOfFields[listOfFieldsIdx++] = GPS_LAT ;
#endif                           // end GPS_INSTALLED
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)
    listOfFields[listOfFieldsIdx++] = FLOW_ACTUAL ;
    listOfFields[listOfFieldsIdx++] = FLOW_REMAIN ;
    listOfFields[listOfFieldsIdx++] = FLOW_PERCENT ;
#endif                          // end FLOW_SENSOR_IS_CONNECTED
#if defined (TEMPERATURE_SOURCE) && ( defined (VARIO) && ( TEMPERATURE_SOURCE == MS5611 ) )  
    listOfFields[listOfFieldsIdx++] = TEMPERATURE ;
#endif
#if defined (MEASURE_RPM)
    listOfFields[listOfFieldsIdx++] = RPM ;
#endif
  numberOfFields = listOfFieldsIdx - 1 ;
  listOfFieldsIdx = 1 ; 
}

boolean OXS_OUT::retrieveFieldIfAvailable(uint8_t fieldId , int32_t * fieldValue , uint8_t * dataType) { // fill fieldValue and dataType for the fieldId when data is available, return true if data is available
#ifdef GPS_INSTALLED            
  uint8_t GPS_no_fix = ( (GPS_fix_type != 3 ) && (GPS_fix_type != 4 ) ) ; // this flag is true when there is no fix
#endif          

   switch (fieldId) {
#if defined(VARIO) 
    case REL_ALTIMETER :
      if ( ! varioData->relativeAlt.available ) return 0 ;
      * fieldValue = varioData->relativeAlt.value / 10 ; // Altitude converted from cm to dm
#ifdef DEBUG_FORCE_VARIODATA
      static int32_t relativeAltTest = 0 ;
      * fieldValue = relativeAltTest++ ;
      if ( relativeAltTest >= 300 ) relativeAltTest = 0 ;
#endif    
      * dataType = JETI22_1D ;
      varioData->relativeAlt.available = false ;
      break ;
    case VERTICAL_SPEED :
         if ( ! mainVspeed.available ) return 0; 
         * fieldValue = mainVspeed.value  ;
#ifdef DEBUG_FORCE_VARIODATA
        static int32_t VspeedTest = 0 ;
        * fieldValue = VspeedTest++ ;
        if ( VspeedTest >= 500 ) VspeedTest = 0 ; 
#endif    
         * dataType = JETI14_2D ;
         mainVspeed.available = false ;
        break ;
    case ALTIMETER_MAX :
        if ( ! varioData->relativeAltMaxAvailable ) return 0 ;
        * fieldValue = (varioData->relativeAltMax ) / 10 ; // Altitude converted from cm to dm
#ifdef DEBUG_FORCE_VARIODATA
        static int32_t relativeAltMaxTest = 0 ;
        * fieldValue =  relativeAltMaxTest-- ;
        if ( relativeAltMaxTest <= -300 ) relativeAltMaxTest = 0 ;
#endif    
        * dataType = JETI22_1D ;
        varioData->relativeAltMaxAvailable = false ;
        break ;
#endif

#if (NUMBEROFCELLS > 0)     //  This part has still to be adapted for Multiplex !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      case  CELL_1 :
          if ( ! voltageData->mVoltCell_Available[0]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[0] /10 ;  //Convert from mv to Volt with 2 decimal
          * dataType = JETI14_2D ;
          voltageData->mVoltCell_Available[0]  = false ;
          break ;
      case  CELL_2 :
          if ( ! voltageData->mVoltCell_Available[1]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[1]  / 10 ;
          * dataType = JETI14_2D ; 
          voltageData->mVoltCell_Available[1]  = false ;
          break ;
      case  CELL_3 :
          if ( ! voltageData->mVoltCell_Available[2]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[2]  / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCell_Available[2]  = false ;
          break ;
      case  CELL_4 :
          if ( ! voltageData->mVoltCell_Available[3]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[3] / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCell_Available[3]  = false ;
          break ;
      case  CELL_5 :
          if ( ! voltageData->mVoltCell_Available[4]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[4] / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCell_Available[4]  = false ;
          break ;
      case  CELL_6 :
          if ( ! voltageData->mVoltCell_Available[5]  ) return 0;
          * fieldValue =  voltageData->mVoltCell[5] / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCell_Available[5]  = false ;
          break ;
      case  CELL_MIN :
          if ( ! voltageData->mVoltCellMin_Available  ) return 0;
          * fieldValue =  voltageData->mVoltCellMin / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCellMin_Available  = false ;
          break ;
      case  CELL_TOT :
          if ( ! voltageData->mVoltCellTot_Available ) return 0;
          * fieldValue =  voltageData->mVoltCellTot  / 10 ; 
          * dataType = JETI14_2D ;
          voltageData->mVoltCellTot_Available  = false ;
          break ;

#endif  // NUMBEROFCELLS > 0 

#if defined ( TEMPERATURE_SOURCE ) && ( TEMPERATURE_SOURCE == NTC )  // if a voltage field is (mis)used to transmit a NTC temperature, then format is different (no decimal).
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_1 )
      case VOLT_1 :  
         if (! voltageData->mVolt[0].available  ) return 0;
         * fieldValue = voltageData->mVolt[0].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[0].available = false ;
         break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_2 )
      case VOLT_2 :  
         if ( ! voltageData->mVolt[1].available  ) return 0;
         * fieldValue = voltageData->mVolt[1].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[1].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_3 )
      case VOLT_3 :  
         if ( ! voltageData->mVolt[2].available  ) return 0;
         * fieldValue = voltageData->mVolt[2].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[2].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_4 )
      case VOLT_4 :  
         if ( ! voltageData->mVolt[3].available  ) return 0;
         * fieldValue = voltageData->mVolt[3].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[3].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_5 )
      case VOLT_5 :  
         if ( ! voltageData->mVolt[4].available  ) return 0;
         * fieldValue = voltageData->mVolt[4].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[4].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_6 )
      case VOLT_6 :  
         if ( ! voltageData->mVolt[5].available  ) return 0;
         * fieldValue = voltageData->mVolt[5].value ;
         * dataType = JETI14_0D ;
         voltageData->mVolt[5].available = false ;
          break ;
#endif
#else  // TEMPERATURE_SOURCE is not NTC and so VOLTAGE_SOURCE is a real voltage

#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_1 )
      case VOLT_1 :  
         if (! voltageData->mVolt[0].available  ) return 0;
         
         * fieldValue = voltageData->mVolt[0].value / 10;
         * dataType = JETI14_2D ;
         voltageData->mVolt[0].available = false ;
         break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_2 )
      case VOLT_2 :  
         if ( ! voltageData->mVolt[1].available  ) return 0;
         * fieldValue = voltageData->mVolt[1].value / 10 ;
         * dataType = JETI14_2D ;
         voltageData->mVolt[1].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_3 )
      case VOLT_3 :  
         if ( ! voltageData->mVolt[2].available  ) return 0;
         * fieldValue = voltageData->mVolt[2].value / 10 ;
         * dataType = JETI14_2D ;
         voltageData->mVolt[2].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_4 )
      case VOLT_4 :  
         if ( ! voltageData->mVolt[3].available  ) return 0;
         * fieldValue = voltageData->mVolt[3].value / 10 ;
         * dataType = JETI14_2D ;
         voltageData->mVolt[3].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_5 )
      case VOLT_5 :  
         if ( ! voltageData->mVolt[4].available  ) return 0;
         * fieldValue = voltageData->mVolt[4].value / 10 ;
         * dataType = JETI14_2D ;
         voltageData->mVolt[4].available = false ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_6 )
      case VOLT_6 :  
         if ( ! voltageData->mVolt[5].available  ) return 0;
         * fieldValue = voltageData->mVolt[5].value / 10 ;
         * dataType = JETI14_2D ;
         voltageData->mVolt[5].available = false ;
          break ;
#endif
#endif // end of defined ( TEMPERATURE_SOURCE ) && ( TEMPERATURE_SOURCE == NTC )

#if ( defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) ) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON))
#if defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) // when current is provide by arduino adc
      case CURRENTMA :
         if ( ! currentData->milliAmps.available  ) return 0;
         * fieldValue = currentData->milliAmps.value /10; // converted in A with 2 decimals
         * dataType = JETI14_2D ;
         currentData->milliAmps.available = false ;
         break ;
      case MILLIAH :
         if ( ! currentData->consumedMilliAmps.available  ) return 0;
         * fieldValue = currentData->consumedMilliAmps.value / 10 ; // converted in Ah with 2 decimals
         * dataType = JETI14_2D ;
         currentData->consumedMilliAmps.available = false ;
         break ;
#else  // when current is provided using an ads1115
      case CURRENTMA :
         if ( ! oXs_ads1115.adsCurrentData.milliAmps.available   ) return 0;
         * fieldValue = oXs_ads1115.adsCurrentData.milliAmps.value /10; // converted in A with 2 decimals
         * dataType = JETI14_2D ;
         oXs_ads1115.adsCurrentData.milliAmps.available = false ;
#ifdef DEBUGADSCURRENT
        printer->print(F("milliAmp="));
        printer->println(oXs_ads1115.adsCurrentData.milliAmps.value /10);
#endif                   
         break ;
      case MILLIAH :
         if ( ! oXs_ads1115.adsCurrentData.consumedMilliAmps.available  ) return 0;
         * fieldValue = oXs_ads1115.adsCurrentData.consumedMilliAmps.value  ; // in mAh without decimals
         * dataType = JETI22_0D ;                                             
         oXs_ads1115.adsCurrentData.consumedMilliAmps.available = false ;
#ifdef DEBUGADSCURRENT
         printer->print(F("consumed milliAmp="));
         printer->println(oXs_ads1115.adsCurrentData.consumedMilliAmps.value / 10);
#endif          
         break ;
#endif
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED :
        if ( ! airSpeedData->airSpeed.available  ) return 0;
        * fieldValue = airSpeedData->airSpeed.value  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        * dataType = JETI14_1D ;
        airSpeedData->airSpeed.available = false ;
        break ;
#elif defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_AIRSPEED_BASED_ON)
      case  AIR_SPEED :
        if ( ! oXs_ads1115.adsAirSpeedData.airSpeed.available  ) return 0;
        * fieldValue = oXs_ads1115.adsAirSpeedData.airSpeed.value  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        * dataType = JETI14_1D ;
        oXs_ads1115.adsAirSpeedData.airSpeed.available = false ;
        break ;
#elif defined(AIRSPEED_SDP3X)
      case  AIR_SPEED :
        if ( ! oXs_sdp3x.airSpeedData.airSpeed.available  ) return 0;
        * fieldValue = oXs_sdp3x.airSpeedData.airSpeed.value  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        * dataType = JETI14_1D ;
        oXs_sdp3x.airSpeedData.airSpeed.available = false ;
        break ;

#endif  // End airspseed    

#ifdef GPS_INSTALLED            
      case GPS_COURSE :
        if (GPS_no_fix ) return 0 ;
        * fieldValue = GPS_ground_course / 100000 ; // convert from degree * 100000 to degree
        * dataType = JETI14_0D ;
        break ;
      case GPS_SPEED :
        if (GPS_no_fix ) return 0 ;
#ifdef GPS_SPEED_3D
        * fieldValue = ((uint32_t) GPS_speed_3d) * 36 /100 ;       // convert from cm/sec to 1/10 of km/h
#else
        * fieldValue = ((uint32_t) GPS_speed_2d) * 36 /100 ;       // convert from cm/sec to 1/10 of km/h
#endif
        * dataType = JETI14_1D ;
        break ;
      case GPS_ALTITUDE : 
        if (GPS_no_fix ) return 0 ;
        * fieldValue  = GPS_altitude / 1000 ;                        // convert from mm to m 
        * dataType = JETI14_0D ;
        break ;
      case GPS_DISTANCE :
        if (GPS_no_fix ) return 0 ;
        * fieldValue  = GPS_distance ;                             // keep in m
        * dataType = JETI14_0D ;
        break ;
      case GPS_BEARING :
        if (GPS_no_fix ) return 0 ;
        * fieldValue  = GPS_bearing  ;                         // keep in degree 
        * dataType = JETI14_0D ;
        break ;
      case GPS_LONG :      
         if (GPS_no_fix ) return 0 ;
         jetiLong =  formatGpsLongLat (GPS_lon , true ) ;
         * fieldValue  = jetiLong  ;
         * dataType = JETI_GPS ;
         break ;                          
      case GPS_LAT :                                           // Still to be added
         if (GPS_no_fix ) return 0 ;
         jetiLat =  formatGpsLongLat (GPS_lat , false ) ;
         * fieldValue  = jetiLat  ;
         * dataType = JETI_GPS ;
         break ;                          
#endif                           // end GPS_INSTALLED
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)
      case FLOW_ACTUAL :
        if( ! actualFlow.available ) return 0 ;
          * fieldValue  = actualFlow.value  ;                    
          * dataType = JETI14_0D ;
          actualFlow.available = false ;  //reset the last bit to avoid sending twice the same value
          break ;
      case FLOW_REMAIN :
        if( ! remainingFuelML.available ) return 0 ;
          * fieldValue  = remainingFuelML.value ;                         
          * dataType = JETI14_0D ;
          remainingFuelML.available = false ;  
          break ;
      case FLOW_PERCENT :
        if( ! fuelPercent.available ) return 0 ;
          * fieldValue  = fuelPercent.value  ;                         
          * dataType = JETI14_0D ;
          fuelPercent.available = false ;  
          break ;      
#endif                           // A_FLOW_SENSOR_IS_CONNECTED
#if defined (TEMPERATURE_SOURCE) && ( defined (VARIO) && ( TEMPERATURE_SOURCE == MS5611 ) )    
    case TEMPERATURE :
        * fieldValue  = varioData->temperature / 100  ;                         
        * dataType = JETI14_0D ;
        break ;
#endif
#if defined (MEASURE_RPM)
    case RPM :
//        if ( ! sport_rpm.available  ) {
//          * fieldValue = 0 ;
//        } else {
#if defined (PULSES_PER_ROTATION)
        * fieldValue  =  sport_rpm.value * 60.0 / PULSES_PER_ROTATION ;
#else  // if PULSES_PER_ROTATION is not defined, we assume 1
        * fieldValue  =  sport_rpm.value * 60 ;
#endif        
//        }  
        * dataType = JETI22_0D ;
        break ;
#endif

   } // end of switch
   return 1 ;
}

uint32_t OXS_OUT::formatGpsLongLat (int32_t longLat, boolean isLong ) { // return the long or latitude in Jeti format
  union   {
    uint32_t valueInteger;
    char    valueBytes[4];
  } gps;
  uint32_t degree ;
  uint16_t minX1000 ;
  uint32_t longLatE7P ;
  longLatE7P = (longLat >= 0 ? longLat : longLat ) ;
  degree =  (uint16_t) (longLatE7P / 10000000) ; // remove 7 decimal from original value
  minX1000 =  (uint16_t ) ( (longLatE7P - (degree * 10000000)) * 6 / 1000 ) ; // keep 3 decimals to minutes
  gps.valueInteger = 0;
  gps.valueBytes[0]  = minX1000 & 0xFF;
  gps.valueBytes[1]  = ( minX1000 >> 8 ) & 0xFF;
  gps.valueBytes[2]  = degree & 0xFF;                      
  gps.valueBytes[3]  = ( degree >> 8 ) & 0x01;             
  gps.valueBytes[3] |= isLong  ? 0x20 : 0;
  gps.valueBytes[3] |= (longLat < 0) ? 0x40 : 0;
  return gps.valueInteger ;
}

boolean OXS_OUT::tryToAddFieldToJetiBuffer (void) { // return true if a field has been added in the jetiBuffer
      int32_t fieldValue ;
      uint8_t dataType ;
      uint8_t fieldAvailable = 0 ;
      
      while (   countOfFieldsChecked  ) {
 //       if ( listOfFieldsIdx == 0 ) {   // this if has been added to generate an empty transmission like in MAVLINK to JETI but I am not sure it is needed; furthermore it should be put outside the loop.
 //         listOfFieldsIdx++ ;
 //         return true; 
 //       }
        if ( retrieveFieldIfAvailable(listOfFields[listOfFieldsIdx] , &fieldValue , &dataType) ) {
          addFieldToJetiBuffer(fieldValue , dataType ) ;
          fieldAvailable = 1;
        } 
        listOfFieldsIdx++ ;
        if (listOfFieldsIdx > numberOfFields ) listOfFieldsIdx = 1;
        countOfFieldsChecked-- ;
        if ( fieldAvailable) return true; 
      } // End While
      return false ; 
}


void OXS_OUT::addFieldToJetiBuffer(int32_t fieldValue , uint8_t dataType ) {  // dataType has 1 high bits = 0 ,
                                                                     // then 2 bits to say precision (0= 0 digits, ... 2 = 2 digits) and 1 bit = 0
                                                                     // then 4 bits to say the type in Jeti codification     
    uint8_t codifJeti ;
    codifJeti = dataType & 0x0F ;
    switch (codifJeti) {
    case JETI_30 :
      jetiData[jetiMaxData++] = (listOfFieldsIdx << 4) | ( codifJeti ) ; // first 4 bits are the field identifiers ( 1...15) and the last 4 bits the type of data)
      jetiData[jetiMaxData++] = fieldValue & 0xFF ;
      jetiData[jetiMaxData++] = ( fieldValue >> 8 ) & 0xFF ;
      jetiData[jetiMaxData++] = ( fieldValue >> 16 ) & 0xFF ;
      jetiData[jetiMaxData++] = ( ( fieldValue >> 24 ) & 0x1F ) | ((fieldValue < 0) ? 0x80 :0x00 )  ; 
      break ;
    case JETI_22 :
      jetiData[jetiMaxData++] = (listOfFieldsIdx << 4) | (codifJeti) ; // first 4 bits are the field identifiers ( 1...15) and the last 4 bits the type of data)
      jetiData[jetiMaxData++] = fieldValue & 0xFF ;
      jetiData[jetiMaxData++] = ( fieldValue >> 8 ) & 0xFF ;
      jetiData[jetiMaxData++] = ( ( fieldValue >> 16 ) & 0x1F  ) | ((fieldValue < 0) ? 0x80 :0x00 )  ; 
      break ;
    case JETI_14 :
      jetiData[jetiMaxData++] = (listOfFieldsIdx << 4) | ( codifJeti) ; // first 4 bits are the field identifiers ( 1...15) and the last 4 bits the type of data)
      jetiData[jetiMaxData++] = fieldValue & 0xFF ;
      jetiData[jetiMaxData++] = ( ( fieldValue >> 8 ) & 0x1F ) | ((fieldValue < 0) ? 0x80 :0x00 )  ; 
      break ;
    case JETI_GPS :
      jetiData[jetiMaxData++] = (listOfFieldsIdx << 4) | ( codifJeti ) ; // first 4 bits are the field identifiers ( 1...15) and the last 4 bits the type of data)
      jetiData[jetiMaxData++] = fieldValue & 0xFF ;
      jetiData[jetiMaxData++] = ( fieldValue >> 8 ) & 0xFF ;
      jetiData[jetiMaxData++] = ( fieldValue >> 16 ) & 0xFF ;
      jetiData[jetiMaxData++] = ( ( fieldValue >> 24 ) & 0xFF )   ; 
      break ;

    
    }
    if ( ( codifJeti == 8 ) || ( codifJeti <= 4 ) ) { // when it is a number, then add precision 
         jetiData[jetiMaxData-1] |= dataType & 0x60 ; // add the number of decimals in position 6 & 7 of the last byte
    }
        
}

// Published in "JETI Telemetry Protocol EN V1.06"
//* Jeti EX Protocol: Calculate 8-bit CRC polynomial X^8 + X^2 + X + 1
uint8_t updateJetiCrc (uint8_t crc, uint8_t crc_seed)
{ 
  unsigned char crc_u;
  unsigned char i;
  crc_u = crc;
  crc_u ^= crc_seed;
  for (i=0; i<8; i++)
    crc_u = ( crc_u & 0x80 ) ? 7 ^ ( crc_u << 1 ) : ( crc_u << 1 );
  return (crc_u);
}


void startJetiTransmit()
{
  if ( state != IDLE )
  {
    return ;
  }
  SET_TX_PIN_TO_OUTPUT() ;
  CLEAR_TX_PIN() ;                    // Send a logic 0 on the TX_PIN (=start bit).
  uint8_t oReg = SREG ; // save status register
  cli() ;
  OCR1A = TCNT1 + TICKS2WAITONEJETI  - INTERRUPT_ENTRY_TRANSMIT;   // Count one period into the future.
  CLEAR_TIMER_INTERRUPT() ;         // Clear interrupt bits
  SREG = oReg ;  // restore status register
  SwUartTXBitCount = 0 ;
  SwUartTXData = jetiData[0] ;
  TxCount = 0 ;
  SwUartTXBit8 = 0 ; // says that bit 8 must be 0 because the first byte is a separator of message
  SwUartTXBitParity = 0 ; // Jeti protocol uses ODD parity
  state = TRANSMIT ;
  ENABLE_TIMER_INTERRUPT() ;        // Enable timer1 interrupt on again
#ifdef DEBUGASERIAL
  PORTC &= ~1 ;
#endif
}

void OXS_OUT::mergeLabelUnit( const uint8_t identifier , const char * label, const char * unit )
{
  uint8_t i = 0 ;
  uint8_t k = 0 ;
  jetiMaxData = 10 ;
  jetiData[8] = identifier ;
  while( label[i] != '\0'  )
    jetiData[ jetiMaxData++ ] = label[ i++ ];
  while( unit[k] != '\0'  )
    jetiData[ jetiMaxData++ ] = unit[ k++ ];
  jetiData[9] = (i << 3 ) | k ;
}


void OXS_OUT::fillJetiBufferWithText() {
  static uint8_t textIdx ;
  textIdx++ ;
  if (textIdx > numberOfFields ) textIdx = 0;
  switch (listOfFields[textIdx]) {
    case 0:
        mergeLabelUnit( textIdx, "oXs", " "  ) ; 
        break ;
#if defined(VARIO) 
    case REL_ALTIMETER :
      mergeLabelUnit( textIdx, "Rel. altit", "m"  ) ; 
        break ;
    case VERTICAL_SPEED :
      mergeLabelUnit( textIdx, "Vario", "m/s"  ) ; 
        break ;
    case ALTIMETER_MAX :
    mergeLabelUnit( textIdx, "Alt.max", "m"  ) ; 
        break ;
#endif

#if (NUMBEROFCELLS > 0)     
      case  CELL_1 :
          mergeLabelUnit( textIdx, "Cell 1", "V"  ) ;
          break ;
      case  CELL_2 :
          mergeLabelUnit( textIdx, "Cell 2", "V"  ) ;
          break ;
      case  CELL_3 :
          mergeLabelUnit( textIdx, "Cell 3", "V"  ) ;
          break ;
      case  CELL_4 :
          mergeLabelUnit( textIdx, "Cell 4", "V"  ) ;
          break ;
      case  CELL_5 :
          mergeLabelUnit( textIdx, "Cell 5", "V"  ) ;
          break ;
      case  CELL_6 :
          mergeLabelUnit( textIdx, "Cell 6", "V"  ) ;
          break ;
      case  CELL_MIN :
          mergeLabelUnit( textIdx, "LowestCell", "V"  ) ;
          break ;
      case  CELL_TOT :
          mergeLabelUnit( textIdx, "Accu. volt", "V"  ) ;
          break ;

#endif  // NUMBEROFCELLS > 0 

#if defined ( TEMPERATURE_SOURCE ) && ( TEMPERATURE_SOURCE == NTC )
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_1 )
      case VOLT_1 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
         break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_2 )
      case VOLT_2 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_3 )
      case VOLT_3 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_4 )
      case VOLT_4 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_5 )
      case VOLT_5 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_6 )
      case VOLT_6 :  
         mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif

#else
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_1 )
      case VOLT_1 :  
         mergeLabelUnit( textIdx, "Voltage 1", "V"  ) ;
         break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_2 )
      case VOLT_2 :  
         mergeLabelUnit( textIdx, "Voltage 2", "V"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_3 )
      case VOLT_3 :  
         mergeLabelUnit( textIdx, "Voltage 3", "V"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_4 )
      case VOLT_4 :  
         mergeLabelUnit( textIdx, "Voltage 4", "V"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_5 )
      case VOLT_5 :  
         mergeLabelUnit( textIdx, "Voltage 5", "V"  ) ;
          break ;
#endif
#if defined(ARDUINO_MEASURES_VOLTAGES) && (ARDUINO_MEASURES_VOLTAGES == YES) && defined(VOLTAGE_SOURCE) && ( VOLTAGE_SOURCE == VOLT_6 )
      case VOLT_6 :  
         mergeLabelUnit( textIdx, "Voltage 6", "V"  ) ;
          break ;
#endif
#endif // end defined ( TEMPERATURE_SOURCE ) && ( TEMPERATURE_SOURCE == NTC )

#if ( defined(ARDUINO_MEASURES_A_CURRENT) && (ARDUINO_MEASURES_A_CURRENT == YES) ) || ( defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_CURRENT_BASED_ON))
      case CURRENTMA :
         mergeLabelUnit( textIdx, "Current", "A"  ) ;
         break ;
      case MILLIAH :
         mergeLabelUnit( textIdx, "Capacity", "mAh"  ) ;
         break ;
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED :
        mergeLabelUnit( textIdx, "Airspeed", "Km/h"  ) ;
        break ;
#elif defined(AN_ADS1115_IS_CONNECTED) && (AN_ADS1115_IS_CONNECTED == YES ) && defined(ADS_MEASURE) && defined(ADS_AIRSPEED_BASED_ON)    
      case  AIR_SPEED :
        mergeLabelUnit( textIdx, "Airspeed", "Km/h"  ) ;
        break ; 
#elif defined(AIRSPEED_SDP3X)
      case  AIR_SPEED :
        mergeLabelUnit( textIdx, "Airspeed", "Km/h"  ) ;
        break ;                
#endif  // End airpseed    

#ifdef GPS_INSTALLED            
      case GPS_COURSE :
        mergeLabelUnit( textIdx, "Course", degreeChar  ) ;
        break ;
      case GPS_SPEED :
        mergeLabelUnit( textIdx, "Speed", "Km/h"  ) ;
        break ;
      case GPS_ALTITUDE : 
        mergeLabelUnit( textIdx, "Gps Alt", "m"  ) ;
        break ;
      case GPS_DISTANCE :
        mergeLabelUnit( textIdx, "Distance", "m"  ) ;
        break ;
      case GPS_BEARING :
        mergeLabelUnit( textIdx, "Gps Bearing", degreeChar  ) ;
        break ;
      case GPS_LONG :                                          // Still to be added 
        mergeLabelUnit( textIdx, "Gps Long", degreeChar  ) ;
        break ;
      case GPS_LAT :                                           // Still to be added
        mergeLabelUnit( textIdx, "Gps Lat", degreeChar  ) ;
        break ;

#endif                           // end GPS_INSTALLED
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)
      case FLOW_ACTUAL :
          mergeLabelUnit( textIdx, "Flow", "ml/min"  ) ;
          break ;
      case FLOW_REMAIN :
          mergeLabelUnit( textIdx, "Remain", "ml"  ) ;
          break ;
      case FLOW_PERCENT :
          mergeLabelUnit( textIdx, "Fuel", "%"  ) ;
          break ;      
#endif                           // A_FLOW_SENSOR_IS_CONNECTED 
#if defined (TEMPERATURE_SOURCE) && ( defined (VARIO) && ( TEMPERATURE_SOURCE == MS5611 ) )  
      case TEMPERATURE :
          mergeLabelUnit( textIdx, "Temp.", "\xB0\x43"  ) ;
          break ;
#endif
#if defined (MEASURE_RPM)
      case RPM :
          mergeLabelUnit( textIdx, "Revolution", "rpm"  ) ;
          break ;
#endif    
  } // end switch
    
        jetiData[2] =  ( jetiMaxData - 2 ) ; // update number of bytes that will be in buffer (including crc); keep flag in bit 6/7 to zero because it is text and not data
  
}

void OXS_OUT::sendData()    // this part could be modified in order to put more than 1 data in a frame.
                            // the buffer must contain a header (already filled with fixed data), then DATA or TEXT part , then a CRC, a separator (0xFE), 2 * 16 char of text and a trailer (0xFF)
                            // the TEXT part contains or the name of the device or the name and unit of each field
{
//  uint32_t temp ;
//  static uint32_t lastMsJetiDataFrame ;
//  static uint32_t lastMsJetiTextFrame ;
//  uint8_t jetiFrameReadyToSend;
//  jetiFrameReadyToSend = 0 ;
  static  uint8_t jetiSendDataFrameCount ;
#ifdef DEBUGJETI
         //printer->print("st: ") ; printer->println(state) ;
#endif  
  if (state == IDLE ) {
      if (jetiSendDataFrameCount  >= 5 ) { //Send a text frame instead of a data frame once every 5 data frames 
          jetiSendDataFrameCount = 0 ;
          //listOfFieldsIdx = 0 ; // set idx to 0 in order to force an empty frame after a TXT frame; this is not good because it could be that some fields with a high idx value are never sent if those with low value are always available
          fillJetiBufferWithText() ;                // fill the buffer (including number of bytes and type but not crc)
      }  else  {  
        jetiSendDataFrameCount++ ;                          // count the number of data frame sent (in order to send a text frame after 5 frames. 
        countOfFieldsChecked = numberOfFields ;             //countOfFieldsChecked is used in order to loop only once thru all data to be sent
        jetiMaxData = 8 ;                                   //  Jeti buffer contains 8 bytes in a header ; this header is already filled.
        if ( tryToAddFieldToJetiBuffer() ) {                    // try to add first field and if OK, continue adding another one.
          tryToAddFieldToJetiBuffer() ;                         // try to add second field 
        } 
        jetiData[2] = 0x40 | ( jetiMaxData - 2 ) ;      // set bit to say that it concerns data (and not text) and update number of bytes that will be in buffer (including crc) 
      } // end test on jetiSendDataFrameCount
//    if ( jetiFrameReadyToSend ) {
#ifdef DEBUGJETI
         printer->print("nf: ") ; printer->print(numberOfFields) ; printer->print(" nb: ") ; printer->print(jetiMaxData) ; printer->print(" > ") ;
        for (uint8_t i= 0 ; i < jetiMaxData ; i++){
          printer->print(jetiData[i], HEX) ; printer->print(" ");
        }
        printer->println("");
#endif
        uint8_t crc = 0 ;
        uint8_t c ;
        for(c=2; c<jetiMaxData ; c++) crc = updateJetiCrc (jetiData[c], crc);  // calculate crc starting from position 2 from the buffer
        jetiData[jetiMaxData++] = crc ;                                        // store crc in buffer
//after filling data or text, we fill the buffer with the 2 lines for the display box (this seems to be mandatory by the protocol)        
        jetiData[jetiMaxData++] = 0xFE ;                                       // fill the 2 lines for the display box (with 0xFE and 0xFF as delimiters)         uint8_t carToSend = 0x41 ;
#if defined ( A_FLOW_SENSOR_IS_CONNECTED ) && ( A_FLOW_SENSOR_IS_CONNECTED == YES)
        if (lastByteReceived != prevByteReceived ) { 
          prevByteReceived = lastByteReceived ;
          processReceivedCmd( lastByteReceived ) ;
        }
        fillTxBuffer() ;
#else                                                        // when a flow sensor is not connected, we send always a dummy text
        for ( uint8_t i_jetiBoxText = 0 ; i_jetiBoxText < 16 ; i_jetiBoxText++ ) {
            jetiData[jetiMaxData++] = 0x2E  ;                // here it is a dummy text; it could be changed later on.
            jetiData[jetiMaxData++] = 0x2D  ;
        }
#endif        
        jetiData[jetiMaxData++] = 0xFF ;
#ifdef DEBUGJETIFRAME                                                          // print the jeti buffer on 4 lines
        //printer->print(F("01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1E 1F 20 21 22 23 24 25 26 27 "));
        //printer->print(F("28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 40 "));
        //printer->println(F("41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 "));
        //for (int i = 0 ; i<jetiMaxData ; i++ )  // to display the full buffer
        /*
        for (int i = 8 ; i<= 13 ; i++ )           // to display only the first 2 bytes data
        {
           if ( jetiData[i]< 0x10 ) printer->print("0") ;
           printer->print(jetiData[i],HEX); printer->print(" ");
        }
        printer->println("");
        */
        
        for (int i = 0 ; i<jetiMaxData ; i++ )
        {
           if ( jetiData[i] >= 0x20 && jetiData[i] <= 0x7F) {
            printer->write(jetiData[i]);
            printer->print("  ");
           } else { 
              if ( jetiData[i]< 0x10 ) printer->print("0") ;
              printer->print(jetiData[i],HEX);printer->print(" ");
           }
        }
        printer->println(""); printer->println("");                 
        
#endif        
        startJetiTransmit() ;
//    }
  } // end state == IDLE   
}


void processReceivedCmd( uint8_t cmd ) {
  switch ( cmd ) {
    case DOWN :
      jetiMenu++ ;
      if (jetiMenu > JETI_MENU_MAX) jetiMenu = 0 ;
      break ;
    case UP :
      jetiMenu-- ;
      if (jetiMenu > JETI_MENU_MAX) jetiMenu = JETI_MENU_MAX ;
      break ;
    case LEFT_RIGHT :
      if ( jetiMenu == JETI_TANK_RESET ) consumedML = 0 ;
      break ;
    case LEFT :
      upDown(1) ;
      break ;
    case RIGHT :
      upDown(-1) ;
      break ;        
  }  // end switch
}  // end processReceivedCmd

void upDown ( int8_t upOrDown ) { 
  if ( jetiMenu == JETI_TANK_CAPACITY) {
    tankCapacity +=  (50 *  upOrDown );
  }
  else if ( ( jetiMenu >= JETI_CAL_FLOW_1) &&  (jetiMenu <= JETI_CAL_FLOW_4) ) {
    flowParam[jetiMenu - JETI_CAL_FLOW_1]+= (5 * upOrDown) ;
  }
  else if ( ( jetiMenu >= JETI_CORR_FLOW_1) &&  (jetiMenu <= JETI_CORR_FLOW_4) ) {
    flowParam[jetiMenu - JETI_CAL_FLOW_1 ] +=  upOrDown ;
  }
  checkFlowParam() ;  // check that new values are valid and consistent (is defined in openXsensor.ino)
} // end upDown

void memcpy_p (uint8_t posDes , uint8_t posSrc ) {
  for (uint8_t i = 0 ; i<32 ; i++) {
     jetiData[posDes++] = pgm_read_byte_near( jetiText + posSrc++ ) ;
  }
}
void fillTxBuffer( void ){
      if (jetiMenu < JETI_CAL_FLOW_1 ) {
        memcpy_p (  jetiMaxData , jetiMenu * 32 ) ;  // copy the text from a table (in flash memory) to the buffer
        switch ( jetiMenu ) {
        case JETI_DATA :
          itoc( (int16_t) actualFlow.value , jetiMaxData + 8 ) ;
          itoc( (int16_t) remainingFuelML.value , jetiMaxData + 16 + 12 ) ;
          break ;
        case JETI_TANK_RESET :
          itoc(  (int16_t)  fuelPercent.value , jetiMaxData + 16 + 13 ) ; 
          break ;
        case JETI_TANK_CAPACITY :
          itoc( tankCapacity , jetiMaxData + 16 + 12 ) ; 
          break ;
        } // end switch
      } else if ( jetiMenu < JETI_CORR_FLOW_1 ) {
          memcpy_p( jetiMaxData , 32 * JETI_CAL_FLOW_1 ) ;  // copy the text from a table (in flash memory) to the buffer
          itoc( jetiMenu - JETI_CAL_FLOW_1 + 1 , jetiMaxData + 12 ) ;
          itoc( flowParam[jetiMenu - JETI_CAL_FLOW_1 ] , jetiMaxData + 16 + 8 ) ;
      } else {
          memcpy_p( jetiMaxData  ,  32 * (JETI_CAL_FLOW_1 + 1) ) ;  // copy the text from a table (in flash memory) to the buffer
          itoc( jetiMenu - JETI_CORR_FLOW_1 + 1 , jetiMaxData + 12 ) ;
          itoc( flowParam[jetiMenu - JETI_CAL_FLOW_1 ] , jetiMaxData + 16 + 13 ) ;  
      }
      jetiMaxData += 32 ;
}  // end fillTxBuffer

void itoc ( int16_t n , uint8_t posLastDigit ) {
   uint8_t i ;
   uint8_t sign = 0 ;
  if (  n < 0 )  // record sign
  {
    n = -n;          // make n positive
    sign = 1 ; 
  }
  i = 0;
  do
  {       // generate digits in reverse order
    jetiData[posLastDigit--] = n % 10 + '0';   // get next digit
  } while ((n /= 10) > 0) ;     // delete it
  if (sign == 1 )
  {
    jetiData[posLastDigit] = '-';
  }
}  // end itoc

//---------------------------------- Here the code to handle the UART  

#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))



ISR(TIMER1_COMPA_vect)
{
  switch (state)
  {
      case TRANSMIT :   // startbit has been sent, it is time to output now 8 bits and 1 stop bit
#ifdef DEBUGASERIAL
          PORTC |= 1 ;
#endif
          if( SwUartTXBitCount < 8 ) {            // If not 8 bits have been sent
            if( SwUartTXData & 0x01 ) {           // If the LSB of the TX buffer is 1:
              SET_TX_PIN() ;
              SwUartTXBitParity++;                // count the number of 1 for ODD parity
            } else {                                // Otherwise:
              CLEAR_TX_PIN() ;                     // Send a logic 0 on the TX_PIN
            }
            SwUartTXData = SwUartTXData >> 1 ;    // Bitshift the TX buffer and
          } else if ( SwUartTXBitCount == 8) {         // send bit 9  that says if the byte is the header or not.
              if( SwUartTXBit8 ) {                   // If the bit 9 is set, then send a logic 1 on TX_PIN:
                SET_TX_PIN() ;
                SwUartTXBitParity++;                // count the number of 1 for ODD parity
              } else {                                 // Otherwise:
                CLEAR_TX_PIN() ;                     // Send a logic 0 on the TX_PIN (this is the case for a separator of message)
              }
          } else if ( SwUartTXBitCount == 9) {         // send parity bit (ODD parity in Jeti protocol)
                  if( SwUartTXBitParity & 0x01 ) {      // If the number of bits set to 1 is odd, then send a logic 0on TX_PIN:
                    CLEAR_TX_PIN() ;
                  } else {                                 // Otherwise:
                    SET_TX_PIN() ;                     // Send a logic 1 on the TX_PIN
                  }
          } else  {                   
                //Send stop bit after the parity bit.
                SET_TX_PIN() ;                             // Output a logic 1.
                OCR1A += TICKS2WAITONEJETI  + TICKS2WAITONEJETI ;  // Count already a first period into the future, the second is added just below (Jeti uses 2 stop bits); here we add a third stop bit because it does not seems to work with only 2
                state = TRANSMIT_STOP_BIT;
          }
          SwUartTXBitCount += 1 ;               // increment TX bit counter.
          OCR1A += TICKS2WAITONEJETI  ;  // Count 2 period into the future. (Jeti uses 2 stop bits)
#ifdef DEBUGASERIAL
        PORTC &= ~1 ;
#endif
          break ;

    case TRANSMIT_STOP_BIT:                    //************************************* handling after the stop bit has been sent
        if (  ++TxCount < jetiMaxData  ) {   
              SwUartTXData = jetiData[TxCount] ;
              CLEAR_TX_PIN();                     // Send a logic 0 on the TX_PIN as start bit  
              OCR1A = TCNT1 + TICKS2WAITONEJETI  - INTERRUPT_BETWEEN_TRANSMIT;   // Count one period into the future.
              SwUartTXBitCount = 0 ;
              if ( (TxCount == ( jetiMaxData - 1 ))  || (TxCount == ( jetiMaxData -34 )) ) 
              { 
                SwUartTXBit8 = 0 ; // force a bit 8 to 0 for the 2 text delimiters
              } else   {
                SwUartTXBit8 = 1 ; // force the bit 8 to 1 because only the first byte and the text delimiters in the buffer requests a bit 8 = 0
              }
              SwUartTXBitParity = 0 ; // reset the value for ODD parity to 0
              
              state = TRANSMIT ;
        } else {                     // all bytes have already been sent
              state = WAITING ;
              nbWaitingDelay = 15 ;   // use a counter to extend the delay for the WAITING State (Jeti asks for 20 msec between 2 frames); here we put 30 msec for safety 
              OCR1A += DELAY_2000;    // 2mS gap before listening (so before going to IDLE)
              TRXDDR &= ~( 1 << PIN_SERIALTX ) ;            // PIN become input during the WAIT time (it will become output when a new frame will be sent)
              TRXPORT |= ( 1 << PIN_SERIALTX ) ;           // PIN is pull up because it could be that the Rx send a byte (half duplex)
              CLEAR_PIN_CHANGE_INTERRUPT( ) ;              // clear pin change interrupt 2 (on port D) used by Rx and Tx (arduino pin 2 or pin 4)
              ENABLE_PIN_CHANGE_INTERRUPT( )  ;            // enable pin change interrupt 2 (because Jeti Rx could send a byte that we want to read)
        }
        break ;

    case WAITING :                                  // At the end of wait time, stop timer interrupt
           if (--nbWaitingDelay ) {
            OCR1A += DELAY_2000;  // wait one more time for 2 msec
           } else {
            state = IDLE ;                           // Go back to idle.
            DISABLE_TIMER_INTERRUPT() ;    // Stop the timer interrupt. We will wait that the main loop ask for sending a frame and reactivate the interrupt 
            DISABLE_TIMERB_INTERRUPT() ;         // Stop the timer COMPB interrupts because the RX can't send a keyboard key anymore
            DISABLE_PIN_CHANGE_INTERRUPT()  ;     // disable pin change interrupt 2 (on port d) because we do not have to wait anymore for a byte sent by Tx 
           }
           break ;


  // Unknown state.
    default:        
          state = IDLE;                               // Error, should not occur. Going to a safe state.
  } // End CASE
//#ifdef PPM_INTERRUPT
//  if ( EIFR & PPM_INT_BIT)  ppmInterrupted = 1 ;
//#endif

} // End of ISR


// ! \brief  External interrupt service routine.  ********************
//  Interrupt on Pin Change to detect change on level on Jeti signal (= could be a start bit)
//
// The falling edge in the beginning of the start
//  bit will trig this interrupt. The stateReceive will
//  be changed to RECEIVE, and the timer interrupt B
//  will be set to trig one and a half bit period
//  from the falling edge. At that instant the
//  code should sample the first data bit.
//
//  note  initSoftwareUart( void ) must be called in advance.
//
// This is the pin change interrupt for port D
// This assumes it is the only pin change interrupt on port D
ISR(PCINT2_vect)
{
  if ( ! (TRXPIN & ( 1 << PIN_SERIALTX ) )) {     // if Pin is low = start bit (inverted)
    DISABLE_PIN_CHANGE_INTERRUPT()  ;     // pin change interrupt disabled (it will be reactivated when a frame has been sent
//PORTC &= ~2 ;

    DISABLE_TIMERB_INTERRUPT() ;       // Disable timer to change its registers. (normally not needed because TimerB interrupt is not active)
    OCR1B = TCNT1 + TICKS2WAITONE_HALFJETI - INTERRUPT_EXEC_CYCL - INTERRUPT_EARLY_BIAS ; // Count one and a half period into the future.
#ifdef DEBUG_SERIAL_RX
    PORTC |= 1 ;
#endif
    SwUartRXBitCount = 0 ;            // Clear received bit counter.
    SwUartRXData = 0 ;                // Reset the byte that will contains the byte being read
    CLEAR_TIMERB_INTERRUPT() ;         // Clear interrupt bits
    ENABLE_TIMERB_INTERRUPT() ;        // Enable timerB interrupt on again
  }
}


ISR(TIMER1_COMPB_vect)       // interrupt on COMPB is used to receive data (keyboard jetibox) from Rx while COMPA is still used to wait some delay between 2 frames
{

          OCR1B += TICKS2WAITONEJETI ;                    // Count one period after the falling edge is trigged.
          uint8_t data ;        // Use a temporary local storage
          data = SwUartRXBitCount ;
          if( data < 8 ) {                         // If 8 bits are not yet read
                        SwUartRXBitCount = data + 1 ;
                        data = SwUartRXData ;
                        data >>= 1 ;                         // Shift due to receiving LSB first.
                        if( GET_RX_PIN( ) ) {
                            data |= 0x80 ;                    // If a logical 1 is read, let the data mirror this.
                        }
                        SwUartRXData = data ;
          } else if ( data == 8 ) {                           // we got 8 bits, so now we get the bit 9
                        SwUartRXBitCount = data + 1 ;
                        SwUartRXBit9 = GET_RX_PIN( ) ;
          } else {                                        //Done receiving =  8 bits are in SwUartRXData and 9th bit in SwUartRXBit9 is ready 
#ifdef DEBUG_SERIAL_RX
                        PORTC &= ~1 ;
#endif        

                        SwUartRXBitParity = GET_RX_PIN( ) ;
  //                      if ( ( ( SwUartRXData & 0X0F ) == 0 ) && ( SwUartRXBit9 == 0) ) {      // check that last 4 bits are 0 and that bit 9 = 0; in fact we could also check the parity bit
                            oneByteReceived = true ;                                       // Keep a flag that is true when a byte has been received
                            lastByteReceived  = SwUartRXData ;                             // Keep the last byte received
  //                      }
                        DISABLE_TIMERB_INTERRUPT() ;         // Stop the timer COMPB interrupts.
#ifdef DEBUG_SERIAL_RX
                        PORTC |= 1 ;
#endif

          }


} // end ISR TIMER1_COMPB_vect

// in ISR TIMER1_COMP_vect, when a frame has been totaly sent, interrupt on pin change must be cleared and enabled. TODO (to uncomment)
//                          when state become IDLE (end of 20 or 30 ms) after sending a frame, interrupt on COMPB must be disabled TODO (to uncomment)
// in pin change interrupt, pin change interrupt must be disabled and SwUartRxBitCount must be reset to 0 and COMPB must be cleared and enabled ;
// in SEND data, before filling the frame we should check if a byte has been received (oneByteReceived = true) and oneByteReceived should be set on false. TODO (to add)


// -------------------------End of Jeti protocol--------------------------------------------------------------------------------------

#endif // END of JETI


