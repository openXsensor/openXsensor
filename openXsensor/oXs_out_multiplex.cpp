// File for Multiplex
#include "oXs_out_multiplex.h"
#ifdef MULTIPLEX

#ifdef DEBUG
// ************************* Several parameters to help debugging
#define DEBUGSETNEWDATA
//#define DEBUGFORMATONEVALUE
#endif


extern unsigned long micros( void ) ;
extern unsigned long millis( void ) ;
extern void delay(unsigned long ms) ;

struct t_mbAllData multiplexData ;

//used only by Sport protocol
volatile uint8_t sendStatus ;   // !!! this is used in Aserial too
//struct t_sportData sportData ; 


//Used by both protocols

int fieldContainsData[][7]  = {  SETUP_MULTIPLEX_DATA_TO_SEND } ; // contains the set up of field to be transmitted, the first field contains the Multiplex adress to be used for this measurement
int numberOfFields = sizeof(fieldContainsData) / sizeof(fieldContainsData[0]) ;
uint8_t multiplexUnit[] = { MULTIPLEX_UNITS } ; // contains the unit code (4 bits) for each OXS measurement
//static uint8_t convertToMultiplexId[15] = { MULTIPLEX_ID } ; //// this array is used to convert an index inside fieldContainsData[][0] into the Multiplex field Id (or defaultfield)
//static uint8_t currentFieldToSend = 0 ; 



#ifdef DEBUG  
OXS_OUT_FRSKY::OXS_OUT_FRSKY(uint8_t pinTx,HardwareSerial &print)
#else
OXS_OUT_FRSKY::OXS_OUT_FRSKY(uint8_t pinTx)
#endif
{
  _pinTx = pinTx ;    
#ifdef DEBUG 
  printer = &print; //operate on the address of print
#endif
} // end constructor


// **************** Setup the FRSky OutputLib *********************
void OXS_OUT_FRSKY::setup() {

    //initilalise PORT
    TRXDDR &= ~( 1 << PIN_SERIALTX ) ;       // PIN is input, tri-stated.
    TRXPORT &= ~( 1 << PIN_SERIALTX ) ;      // PIN is input, tri-stated.

  // Activate pin change interupt on Tx pin
#if PIN_SERIALTX == 4
    PCMSK2 |= 0x10 ;			// IO4 (PD4) on Arduini mini
#elif PIN_SERIALTX == 2
    PCMSK2 |= 0x04 ;                    // IO2 (PD2) on Arduini mini
#else
    #error "This PIN is not supported"
#endif

    PCIFR = (1<<PCIF2) ;	// clear pending interrupt

    //initMultiplexUart( &sportData ) ;
    initMultiplexUart( &multiplexData ) ;
	
#ifdef DEBUG
      printer->print(F("Multiplex Output Module: TX Pin="));
      printer->println(_pinTx);
      printer->print(F(" milli="));  
      printer->println(millis());
      printer->println(F("Multiplex Output Module: Setup!"));
      printer->print(F("Number of fields to send = "));
      printer->println(numberOfFields);
      for (int rowNr = 0 ; rowNr < numberOfFields ; rowNr++) {
          printer->print(fieldContainsData[rowNr][0],HEX); printer->print(F(" , ")); 
          printer->print(fieldContainsData[rowNr][1]);  printer->print(F(" , "));
          printer->print(fieldContainsData[rowNr][2]);  printer->print(F(" , "));
          printer->print(fieldContainsData[rowNr][3]);  printer->print(F(" , "));
          printer->println(fieldContainsData[rowNr][4]);
      }    
#endif
     for ( int mbIndex = 0 ; mbIndex <= MB_MAX_ADRESS ;   mbIndex++ ) {
       multiplexData.mbData[mbIndex].response[0] = mbIndex ;
       multiplexData.mbData[mbIndex].active = NOT_ACTIVE ;
     }  
     for ( int mbIndex = 0 ; mbIndex < numberOfFields ;   mbIndex++ ) {
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[0] = ( fieldContainsData[mbIndex][0] << 4) | ( multiplexUnit[ fieldContainsData[mbIndex][1] ] );
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[1] = MB_NOVALUE ; // low byte 
       multiplexData.mbData[fieldContainsData[mbIndex][0]].response[2] = MB_NOVALUE >> 8 ; // hight byte
       multiplexData.mbData[fieldContainsData[mbIndex][0]].active = AVAILABLE ;
     }  

// for defug purpose only
multiplexData.mbData[15].response[0] = 0xF0 ;
multiplexData.mbData[15].response[1] = 0x0F ;
multiplexData.mbData[15].response[2] = 0xAA ;
#ifdef DEBUG
     for ( int mbIndex = 0 ; mbIndex <= MB_MAX_ADRESS ;   mbIndex++ ) {
        Serial.print("id=");
        Serial.print( mbIndex);
        Serial.print(" Act=");
        Serial.print( multiplexData.mbData[mbIndex].active, HEX );
        Serial.print(" r0=");
        Serial.print( multiplexData.mbData[mbIndex].response[0], HEX);
        Serial.print(" r1=");
        Serial.print( multiplexData.mbData[mbIndex].response[1], HEX);
        Serial.print(" r2=");
        Serial.println( multiplexData.mbData[mbIndex].response[2], HEX);
     }   
#endif


}



void OXS_OUT_FRSKY::sendData()
{
  	formatAllMultiplexData( ) ;
  
}


void OXS_OUT_FRSKY::formatAllMultiplexData()
{  
    for ( uint8_t mbIndex = 0 ; mbIndex < numberOfFields ;   mbIndex++ ) {
            formatOneValue( mbIndex ) ;
    }
}

//*************************************
// Format the sensor data into an array containing the 3 bytes to sent
// process occurs only if measurement has been flagged as available by the sensor
// return 0 if data has been processed; return the internal code of the measurement otherwise
//************************************
uint8_t OXS_OUT_FRSKY::formatOneValue( uint8_t currentFieldToSend) {
  static int32_t valueTemp ;
  static uint8_t alarm ;
  int fieldID ;
  fieldID = 0 ;
  valueTemp = 0 ; 

  switch ( fieldContainsData[currentFieldToSend][1] ) {
#ifdef VARIO       
      case  ALTIMETER :
        if ( ! varioData->absoluteAltAvailable ) return 0 ;
        valueTemp = (varioData->absoluteAlt ) / 100 ;
        varioData->absoluteAltAvailable = false ;
#ifdef DEBUGFORMATONEVALUE
        Serial.print("At ");
        Serial.print( millis());
        Serial.print("id=");
        Serial.print( currentFieldToSend );
        Serial.println("Alt available ");
#endif
        break ;
      case VERTICAL_SPEED : 
         if ( ! varioData->climbRateAvailable ) return 0; 
         valueTemp = varioData->climbRate / 10 ;
         varioData->climbRateAvailable = false ;
         break ;
      case SENSITIVITY :
             if ( !varioData->sensitivityAvailable ) return 0;
             valueTemp = varioData->sensitivity ;
             varioData->sensitivityAvailable = false ;
             break ;
      case ALT_OVER_10_SEC :
             if ( ! varioData->vSpeed10SecAvailable ) return 0;
             valueTemp = varioData->vSpeed10Sec  / 100 ; // todo : adjust decimals
             varioData->vSpeed10SecAvailable = false ;
             break ;
      case  REL_ALTIMETER :
        if ( ! varioData->relativeAltAvailable ) return 0 ;
        valueTemp = (varioData->relativeAlt ) / 100 ;
        varioData->relativeAltAvailable = false ;
            break ;
      case  ALTIMETER_MAX :
        if ( ! varioData->relativeAltMaxAvailable ) return 0 ;
        valueTemp = (varioData->relativeAltMax ) / 100 ;
        varioData->relativeAltMaxAvailable = false ;
            break ;
      
#endif  // End vario    

#ifdef VARIO2       
      case  ALTIMETER_2 :
        if ( ! varioData_2->absoluteAltAvailable  ) return 0;
        valueTemp = varioData_2->absoluteAlt  / 100 ;
        varioData_2->absoluteAltAvailable = false ;
        break ;
      case VERTICAL_SPEED_2 : 
         if ( ! varioData_2->climbRateAvailable  ) return 0;
         valueTemp = varioData_2->climbRate / 10 ;
         varioData_2->climbRateAvailable = false ;
         break ;
      case SENSITIVITY_2 :
             if ( ! varioData_2->sensitivityAvailable  ) return 0;
             valueTemp = varioData_2->sensitivity ;
             varioData_2->sensitivityAvailable = false ;
             break ;
     case ALT_OVER_10_SEC_2 :
             if ( ! varioData_2->vSpeed10SecAvailable  ) return 0;
             valueTemp = varioData_2->vSpeed10Sec / 100 ; // todo : adjust decimals
             varioData_2->vSpeed10SecAvailable = false ;
             break ; 
      case  REL_ALTIMETER_2 :
        if ( ! varioData_2->relativeAltAvailable ) return 0 ;
        valueTemp = (varioData_2->relativeAlt ) / 100 ;
        varioData_2->relativeAltAvailable = false ;           
        break ;     
#endif  // End vario2    

#if defined (VARIO )  &&  defined (VARIO2)
      case VERTICAL_SPEED_A : 
        if ( ! averageVSpeedAvailable  ) return 0;
        valueTemp = averageVSpeed / 10 ;
        averageVSpeedAvailable = false ; 
         break ; 
#endif

#if defined (VARIO )  && ( defined (VARIO2) || defined( AIRSPEED) ) && defined (VARIO_PRIMARY ) && defined (VARIO_SECONDARY )  && defined (PIN_PPM)
      case PPM_VSPEED : 
        if ( ! switchVSpeedAvailable  ) return 0;
        valueTemp = switchVSpeed / 10 ;
        switchVSpeedAvailable = false ; 
         break ; 
#endif

#ifdef AIRSPEED       
      case  AIR_SPEED :
        if ( ! airSpeedData->airSpeedAvailable  ) return 0;
        valueTemp = airSpeedData->airSpeed  * 1.852;   //  convert from 1/10 of knots to  1/10 of Km/h 
        airSpeedData->airSpeedAvailable = false ;
        break ;
      case  PRANDTL_COMPENSATION :
        if ( ! airSpeedData->compensationAvailable  ) return 0;
        valueTemp = airSpeedData->compensation / 10  ;
        airSpeedData->compensationAvailable = false ;
        break ;
#endif  // End airpseed    

#if defined (VARIO) && defined ( AIRSPEED)
      case PRANDTL_DTE :
        if ( ! compensatedClimbRateAvailable  ) return 0;
        valueTemp =  compensatedClimbRate / 10 ; 
        compensatedClimbRateAvailable = false ;
        break ;
#endif  // End defined (VARIO) && defined ( AIRSPEED)

#ifdef PIN_VOLTAGE_1
      case VOLT1 :  
         if (! voltageData->mVoltAvailable[0]  ) return 0;
         valueTemp = voltageData->mVolt[0] / 100;
         voltageData->mVoltAvailable[0] = false ;
         break ;
#endif         
#ifdef PIN_VOLTAGE_2
      case VOLT2 :  
         if ( ! voltageData->mVoltAvailable[1]  ) return 0;
         valueTemp = voltageData->mVolt[1] / 100 ;
         voltageData->mVoltAvailable[1] = false ;
          break ;
#endif         
#ifdef PIN_VOLTAGE_3
      case VOLT3 :  
         if ( ! voltageData->mVoltAvailable[2]  ) return 0;
         valueTemp = voltageData->mVolt[2] / 100 ;
         voltageData->mVoltAvailable[2] = false ;
          break ;
#endif         
#ifdef PIN_VOLTAGE_4
      case VOLT4 :  
         if ( ! voltageData->mVoltAvailable[3]  ) return 0;
         valueTemp = voltageData->mVolt[3] / 100 ;
         voltageData->mVoltAvailable[3] = false ;
          break ;
#endif         
#ifdef PIN_VOLTAGE_5
      case VOLT5 :  
         if ( ! voltageData->mVoltAvailable[4]  ) return 0;
         valueTemp = voltageData->mVolt[4] / 100 ;
         voltageData->mVoltAvailable[4] = false ;
          break ;
#endif         
#ifdef PIN_VOLTAGE_6
      case VOLT6 :  
         if ( ! voltageData->mVoltAvailable[5]  ) return 0;
         valueTemp = voltageData->mVolt[5] / 100 ;
         voltageData->mVoltAvailable[5] = false ;
          break ;
#endif

#if defined (PIN_CURRENTSENSOR)
      case CURRENTMA :
         if ( ! currentData->milliAmpsAvailable  ) return 0;
         valueTemp = currentData->milliAmps /100;
         currentData->milliAmpsAvailable = false ;
         break ;
      case MILLIAH :
         if ( ! currentData->consumedMilliAmpsAvailable  ) return 0;
         valueTemp = currentData->consumedMilliAmps ;
         currentData->consumedMilliAmpsAvailable = false ;
         break ;
#endif

#if (NUMBEROFCELLS > 0)     //  This part has still to be adapted for Multiplex !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      case  CELL_1 :
          if ( ! voltageData->mVoltCell_Available[0]  ) return 0;
          valueTemp =  voltageData->mVoltCell[0] /100 ; 
          voltageData->mVoltCell_Available[0]  = false ;
          break ;
      case  CELL_2 :
          if ( ! voltageData->mVoltCell_Available[1]  ) return 0;
          valueTemp =  voltageData->mVoltCell[1]  / 100 ; 
          voltageData->mVoltCell_Available[1]  = false ;
          break ;
      case  CELL_3 :
          if ( ! voltageData->mVoltCell_Available[2]  ) return 0;
          valueTemp =  voltageData->mVoltCell[2]  / 100 ; 
          voltageData->mVoltCell_Available[2]  = false ;
          break ;
      case  CELL_4 :
          if ( ! voltageData->mVoltCell_Available[3]  ) return 0;
          valueTemp =  voltageData->mVoltCell[3] / 100 ; 
          voltageData->mVoltCell_Available[3]  = false ;
          break ;
      case  CELL_5 :
          if ( ! voltageData->mVoltCell_Available[4]  ) return 0;
          valueTemp =  voltageData->mVoltCell[4] / 100 ; 
          voltageData->mVoltCell_Available[4]  = false ;
          break ;
      case  CELL_6 :
          if ( ! voltageData->mVoltCell_Available[5]  ) return 0;
          valueTemp =  voltageData->mVoltCell[5] / 100 ; 
          voltageData->mVoltCell_Available[5]  = false ;
          break ;
      case  CELL_MIN :
          if ( ! voltageData->mVoltCellMin_Available  ) return 0;
          valueTemp =  voltageData->mVoltCellMin / 100 ; 
          voltageData->mVoltCellMin_Available  = false ;
          break ;
      case  CELL_TOT :
          if ( ! voltageData->mVoltCellTot_Available ) return 0;
          valueTemp =  voltageData->mVoltCellTot  / 100 ; 
          voltageData->mVoltCellTot_Available  = false ;
          break ;

#endif  // NUMBEROFCELLS > 0 

#ifdef PIN_PPM 
      case  PPM :
          if ( ! ppmAvailable  ) return 0;
          valueTemp = ppm ;
          ppmAvailable  = false ; 
          break ;
#endif


#ifdef MEASURE_RPM 
      case  RPM :
          if ( ! RpmAvailable  ) return 0;
          valueTemp = RpmValue ;  // to adjust probably
          RpmAvailable  = false ;
          break ;   
#endif
      case  TEST1 :
          if ( ! test1ValueAvailable  ) return 0;
          valueTemp = test1Value ;
          test1ValueAvailable  = false ; 
          break ;
      case  TEST2 :
          if ( ! test2ValueAvailable  ) return 0;
          valueTemp = test2Value ;
          test2ValueAvailable  = false ; 
          break ;
      case  TEST3 :
          if ( ! test3ValueAvailable  ) return 0;
          valueTemp = test3Value ;
          test3ValueAvailable  = false ; 
          break ;




      }  // end Switch
//      if ( (fieldContainsData[currentFieldToSend][0] != 0)  ) { ;  // to be adapted in a condition that protect modification of data array while data are transfered to TX buffer array 
//      }
      valueTemp = (valueTemp * fieldContainsData[currentFieldToSend][2] / fieldContainsData[currentFieldToSend][3])  + fieldContainsData[currentFieldToSend][4]  ;
      alarm =  (( valueTemp <= fieldContainsData[currentFieldToSend][5] ) || ( valueTemp >= fieldContainsData[currentFieldToSend][6]) ) ? 1 : 0;
      setMultiplexNewData(  fieldContainsData[currentFieldToSend][0]  ,  valueTemp , alarm  ) ; 
      return fieldContainsData[currentFieldToSend][1] ;
}  // End function


void OXS_OUT_FRSKY::setMultiplexNewData(  uint16_t id, int32_t value , uint8_t alarm)
{
int32_t valueOrig = value ;
        multiplexData.mbData[id].active = LOCKED ;
        value = value << 1 ;
        value += alarm ;
        multiplexData.mbData[id].response[1] = value ;	
        multiplexData.mbData[id].response[2] = value >> 8 ; 
        multiplexData.mbData[id].active = AVAILABLE ;
#ifdef DEBUGSETNEWDATA                                        
        Serial.print("set new data at ");
        Serial.print( millis());
        Serial.print(" for id=");
        Serial.print( id ); 
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[0] , HEX );
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[1] , HEX );
        Serial.print(" ");
        Serial.print( multiplexData.mbData[id].response[2] , HEX );
        Serial.print(" A=");
        Serial.print( varioData->absoluteAlt );
        Serial.print(" V=");
        Serial.print( varioData->climbRate );
        Serial.print(" Vo=");
        Serial.print( valueOrig );

        Serial.println(" ");
#endif
              
}


// -------------------------End of Multiplex protocol--------------------------------------------------------------------------------------

#endif // END of MULTIPLEX

