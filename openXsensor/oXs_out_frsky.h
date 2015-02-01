// For FRSKY
#ifndef OXS_OUT_FRSKY_h
#define OXS_OUT_FRSKY_h

#include "oXs_config.h"
#include "oXs_ms5611.h" // we need the variodata struct
#include "oXs_4525.h" // we need the airspeeddata struct
#include "oXs_curr.h" // we need the currentdata struct
#include "oXs_voltage.h" // we need the arduinodata struct
#include "Aserial.h"

#ifndef MULTIPLEX

#define INTERVAL_FRAME1 200
//#define INTERVAL_FRAME2 1000

// FrSky new DATA IDs (2 bytes) (copied from openTX telemetry/frsky_sport.cpp on 11 jul 2014)
#define ALT_FIRST_ID            0x0100
#define ALT_LAST_ID             0x010f
#define VARIO_FIRST_ID          0x0110
#define VARIO_LAST_ID           0x011f
#define CURR_FIRST_ID           0x0200
#define CURR_LAST_ID            0x020f
#define VFAS_FIRST_ID           0x0210
#define VFAS_LAST_ID            0x021f
#define CELLS_FIRST_ID          0x0300
#define CELLS_SECOND_ID         0x0301
#define CELLS_THIRD_ID          0x0302
#define CELLS_LAST_ID           0x030f
#define T1_FIRST_ID             0x0400
#define T1_LAST_ID              0x040f
#define T2_FIRST_ID             0x0410
#define T2_LAST_ID              0x041f
#define RPM_FIRST_ID            0x0500
#define RPM_LAST_ID             0x050f
#define FUEL_FIRST_ID           0x0600
#define FUEL_LAST_ID            0x060f
#define ACCX_FIRST_ID           0x0700
#define ACCX_LAST_ID            0x070f
#define ACCY_FIRST_ID           0x0710
#define ACCY_LAST_ID            0x071f
#define ACCZ_FIRST_ID           0x0720
#define ACCZ_LAST_ID            0x072f
#define GPS_LONG_LATI_FIRST_ID  0x0800
#define GPS_LONG_LATI_LAST_ID   0x080f
#define GPS_ALT_FIRST_ID        0x0820
#define GPS_ALT_LAST_ID         0x082f
#define GPS_SPEED_FIRST_ID      0x0830
#define GPS_SPEED_LAST_ID       0x083f
#define GPS_COURS_FIRST_ID      0x0840
#define GPS_COURS_LAST_ID       0x084f
#define GPS_TIME_DATE_FIRST_ID  0x0850
#define GPS_TIME_DATE_LAST_ID   0x085f
#define A3_FIRST_ID             0x0900
#define A3_LAST_ID              0x090f
#define A4_FIRST_ID             0x0910
#define A4_LAST_ID              0x091f
#define AIR_SPEED_FIRST_ID      0x0a00
#define AIR_SPEED_LAST_ID       0x0a0f
#define RSSI_ID                 0xf101  // please do not use this code because it is already used by the receiver
#define ADC1_ID                 0xf102  // please do not use this code because it is already used by the receiver
#define ADC2_ID                 0xf103
#define BATT_ID                 0xf104
#define SWR_ID                  0xf105  // please do not use this code because it is already used by the receiver
// End of list of all telemetry fields supported by SPORT  (defined by Frsky) 

//list of all telemetry fields supported by Hub protocol (defined by Frsky) 
#define FRSKY_USERDATA_GPS_ALT_B    0x01
#define FRSKY_USERDATA_TEMP1        0x02
#define FRSKY_USERDATA_RPM          0x03
#define FRSKY_USERDATA_FUEL         0x04
#define FRSKY_USERDATA_TEMP2        0x05
#define FRSKY_USERDATA_CELL_VOLT    0x06
#define FRSKY_USERDATA_GPS_ALT_A    0x09
#define FRSKY_USERDATA_BARO_ALT_B   0x10
#define FRSKY_USERDATA_GPS_SPEED_B  0x11
#define FRSKY_USERDATA_GPS_LONG_B   0x12
#define FRSKY_USERDATA_GPS_LAT_B    0x13
#define FRSKY_USERDATA_GPS_CURSE_B  0x14
#define FRSKY_USERDATA_GPS_DM       0x15
#define FRSKY_USERDATA_GPS_YEAR     0x16
#define FRSKY_USERDATA_GPS_HM       0x17
#define FRSKY_USERDATA_GPS_SEC      0x18
#define FRSKY_USERDATA_GPS_SPEED_A  0x19
#define FRSKY_USERDATA_GPS_LONG_A   0x1A
#define FRSKY_USERDATA_GPS_LAT_A    0x1B
#define FRSKY_USERDATA_GPS_CURSE_A  0x1C
#define FRSKY_USERDATA_BARO_ALT_A   0x21
#define FRSKY_USERDATA_GPS_LONG_EW  0x22
#define FRSKY_USERDATA_GPS_LAT_EW   0x23
#define FRSKY_USERDATA_ACC_X        0x24
#define FRSKY_USERDATA_ACC_Y        0x25
#define FRSKY_USERDATA_ACC_Z        0x26
#define FRSKY_USERDATA_CURRENT      0x28
#define FRSKY_USERDATA_VERT_SPEED   0x30 // open9x Vario Mode Only
#define FRSKY_USERDATA_ALT_MIN      0x31 // open9x Vario Mode Only
#define FRSKY_USERDATA_ALT_MAX      0x32 // open9x Vario Mode Only
#define FRSKY_USERDATA_RPM_MAX      0x33 // open9x Vario Mode Only
#define FRSKY_USERDATA_T1_MAX       0x34 // open9x Vario Mode Only
#define FRSKY_USERDATA_T2_MAX       0x35 // open9x Vario Mode Only
#define FRSKY_USERDATA_GPS_SPEED_MAX  0x36 // open9x Vario Mode Only
#define FRSKY_USERDATA_GPS_DIS_MAX  0x37 // open9x Vario Mode Only
#define FRSKY_USERDATA_VFAS_NEW     0x39 // Use this field in order to display the value on VFAS on Tx, Take care that for a voltage, the value must be in 1/10 of Volt and not in mVolt
#define FRSKY_USERDATA_VOLTAGE_B    0x3A // do not use this code to transmit a voltage. It requires a special formatting that is not implemented. Use VFAS_NEW instead
#define FRSKY_USERDATA_VOLTAGE_A    0x3B // do not use this code to transmit a voltage. It requires a special formatting that is not implemented. Use VFAS_NEW instead
#define FRSKY_USERDATA_GPS_DIST     0x3C
#define FRSKY_USERDATA_FUELPERCENT  0x3D
// Endof list of all telemetry fields supported by Hub protocol (defined by Frsky) 


  
// mapping of fields to send
#define DEFAULTFIELD  0x00  // value to let OXS select automatically the best telemetry field.
#define   Alt      1       
#define   VSpd     2
#define   Curr     3
#define   Vfas     4
#define   T1       5
#define   T2       6
#define   Rpm      7
#define   Fuel     8
#define   AccX     9
#define   AccY     10
#define   AccZ     11
#define   A3       12
#define   A4       13
#define   ASpd     14

#define FRSKY_SPORT_ID DEFAULTFIELD , ALT_FIRST_ID ,  VARIO_FIRST_ID  , CURR_FIRST_ID , VFAS_FIRST_ID , T1_FIRST_ID , T2_FIRST_ID , RPM_FIRST_ID , \
                       FUEL_FIRST_ID , ACCX_FIRST_ID , ACCY_FIRST_ID , ACCZ_FIRST_ID , A3_FIRST_ID , A4_FIRST_ID, AIR_SPEED_FIRST_ID

#define FRSKY_HUB_ID  DEFAULTFIELD , DEFAULTFIELD , FRSKY_USERDATA_VERT_SPEED , FRSKY_USERDATA_CURRENT , FRSKY_USERDATA_VFAS_NEW , FRSKY_USERDATA_TEMP1 , FRSKY_USERDATA_TEMP2 , FRSKY_USERDATA_RPM , \
                      FRSKY_USERDATA_FUEL ,  FRSKY_USERDATA_ACC_X , FRSKY_USERDATA_ACC_Y , FRSKY_USERDATA_ACC_Z , 0xFF , 0xFF , 0xFF

  

//  This is the list of codes for each available measurements
#define ALTIMETER       1        // DEFAULTFIELD can be used in SPORT protocol (is then the same as ALT_FIRST_ID);  it MUST be used in Hub protocol because meters and centimeters are send in different fileds
#define VERTICAL_SPEED  2        // DEFAULTFIELD can be used
#define SENSITIVITY     3        // DEFAULTFIELD can NOT be used
#define ALT_OVER_10_SEC 4        // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
                                 // there is no telemetry field for this; it is possible to use e.g. T1 or T2; then you can use a custom function "play value" on Tx side
#define VOLT1           5        // DEFAULTFIELD can NOT be used
#define VOLT2           6        // DEFAULTFIELD can NOT be used
#define VOLT3           7        // DEFAULTFIELD can NOT be used
#define VOLT4           8        // DEFAULTFIELD can NOT be used
#define VOLT5           9        // DEFAULTFIELD can NOT be used
#define VOLT6           10        // DEFAULTFIELD can NOT be used
#define CURRENTMA       11        // DEFAULTFIELD can be used
#define MILLIAH         12        // if value must be sent as percentage, then uncomment the line "#define SEND_mAhPercentageAsFuel 4000" (see below)
#define CELLS_1_2       13        // Only DEFAULTFIELD can be used
#define CELLS_3_4       14        // Only DEFAULTFIELD can be used
#define CELLS_5_6       15        // Only DEFAULTFIELD can be used
#define RPM             16        // Only DEFAULTFIELD can be used
#define ALTIMETER_2        17      // DEFAULTFIELD can be used in SPORT protocol (is then the same as ALT_FIRST_ID);  it MUST be used in Hub protocol because meters and centimeters are send in different fileds
#define VERTICAL_SPEED_2   18      // DEFAULTFIELD can be used
#define SENSITIVITY_2      19      // DEFAULTFIELD can NOT be used
#define ALT_OVER_10_SEC_2  20      // DEFAULTFIELD can NOT be used ; this is the difference of altitude over the last 10 sec (kind of averaging vertical speed)
#define AIR_SPEED          21      // DEFAULTFIELD can be used in SPORT only
#define PRANDTL_COMPENSATION 22      // DEFAULTFIELD can NOT be used;use e.g. Temperature 1 or 2
#define PPM_VSPEED         23       // DEFAULTFIELD can be used; Vpseed from first or second MS5611 or compensatedVspeed will be sent; to be used only when VARIO_SECONDARY is defined and PIN_PPM is defined
#define PPM                24       // DEFAULTFIELD can NOT be used ;
#define PRANDTL_DTE        25       // DEFAULTFIELD can be used
#define TEST1              26       // reserved : only for debugging
#define TEST2              27       // reserved : only for debugging
#define TEST3              28       // reserved : only for debugging
#define VERTICAL_SPEED_A   29       // DEFAULTFIELD can be used
// to do : add alt min, alt max ,  rpm max? , current max (not sure that it is neaded because it can be calculated on TX side
// End of list of type of available measurements

  
 
#define UNKNOWN false
#define KNOWN true

/***************************************************************************************/
/* Transmission status                                                                 */ 
/***************************************************************************************/
#define TO_LOAD     0
#define LOADED      1
#define SENDING     2
#define SEND        3


class OXS_OUT_FRSKY {
  public:
#ifdef DEBUG  
    OXS_OUT_FRSKY(uint8_t pinTx,HardwareSerial &print);
#else
    OXS_OUT_FRSKY(uint8_t pinTx);
#endif
    VARIODATA* varioData ;
    VARIODATA* varioData_2 ;
    AIRSPEEDDATA* airSpeedData ;
    CURRENTDATA* currentData ;
    VOLTAGEDATA* voltageData ;
 //   uint8_t currentValueType ; //e.g. = ALTIMETER, VERTICAL_SPEED, = field_Id to transmit  
    void setup();
    void sendData();
    
  private:
// used by both protocols  
    uint8_t _pinTx;
#ifdef DEBUG  
    HardwareSerial* printer;
#endif
 
  
// used by SPORT protocol
    void sendSportData() ;
    uint8_t readStatusValue( uint8_t currentValueType) ;
    void loadSportValueToSend(  uint8_t ValueTypeToLoad) ;
    uint8_t nextFieldToSend(  uint8_t indexField) ;

// used by Hub protocol
    void sendHubData() ;
    void loadHubValueToSend(  uint8_t ValueTypeToLoad) ;
    void SendFrame1();
    void SendValue(uint8_t ID, uint16_t Value);
    void SendCellVoltage(uint32_t voltage);
    void SendGPSDist(uint16_t dist);
    void SendTemperature1(int16_t tempc);
    void SendTemperature2(int16_t tempc);
    void SendAlt(long altcm);
    void SendGPSAlt(long altcm);
    void SendFuel(uint16_t fuel);
    void SendCurrentMilliAmps(int32_t milliamps);
    void SendGPSSpeed(long speedknots) ;
    void sendHubByte( uint8_t byte ) ;
    void SendVoltX( uint8_t VoltToSend ,  uint8_t indexFieldToSend ) ;

};

extern int ppm ; 
extern bool ppmAvailable ;
extern int32_t compensatedClimbRate ;
extern bool compensatedClimbRateAvailable ;
extern int32_t switchVSpeed ; // used to transmit the selected Vspeed
extern bool switchVSpeedAvailable ;
extern int32_t averageVSpeed ; // used to transmit the average Vspeed
extern bool averageVSpeedAvailable ;
extern int32_t test1Value ;// used in order to test the transmission of any value
extern bool test1ValueAvailable ;
extern int32_t test2Value ;// used in order to test the transmission of any value
extern bool test2ValueAvailable ;
extern int32_t test3Value ;// used in order to test the transmission of any value
extern bool test3ValueAvailable ;
extern uint8_t selectedVario ;

#ifdef MEASURE_RPM
extern volatile uint16_t RpmValue ;
extern bool RpmAvailable ;
#endif // MEASURE_RPM


#endif // Enf of ndef MULTIPLEX
#endif // OXS_OUT_FRSKY_h






