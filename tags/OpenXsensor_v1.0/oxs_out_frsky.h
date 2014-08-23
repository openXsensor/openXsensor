#ifndef OXS_OUT_FRSKY_h
#define OXS_OUT_FRSKY_h

#include "Arduino.h"
#include "oxs_config.h"
#include "oxs_ms5611.h" // we need the variodata struct
#include "oxs_curr.h" // we need the currentdata struct
#include "oxs_arduino.h" // we need the arduinodata struct


#define INTERVAL_FRAME1 200
#define INTERVAL_FRAME2 1000

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
#define FRSKY_USERDATA_CURRENT_MAX  0x38
#define FRSKY_USERDATA_VFAS_NEW     0x39 // open9x Vario Mode Only

#define FRSKY_USERDATA_VOLTAGE_B    0x3A
#define FRSKY_USERDATA_VOLTAGE_A    0x3B
#define FRSKY_USERDATA_GPS_DIST     0x3C

  
 
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
    CURRENTDATA* currentData ;
    ARDUINODATA* arduinoData ;
    uint8_t currentValueType ; //e.g. = ALTIMETER, VERTICAL_SPEED, = field_Id to transmit  
    void setup();
    void sendData();
    
  private:
    uint8_t readStatusValue( uint8_t currentValueType) ;
    void loadValueToSend(  uint8_t ValueTypeToLoad) ;
    uint8_t nextFieldToSend(  uint8_t currentFieldToSend) ;
    uint8_t _pinTx;
#ifdef DEBUG  
    HardwareSerial* printer;
#endif
//    void SendFrame1A();
//    void SendFrame1B();
//    void SendFrame2();
    void SendFrame1();
    void SendValue(uint8_t ID, uint16_t Value);
    void SendCellVoltage(uint32_t voltage);
    void SendGPSDist(uint16_t dist);
    void SendTemperature1(int16_t tempc);
    void SendTemperature2(int16_t tempc);
    void SendRPM(uint16_t rpm)
			{ SendValue(FRSKY_USERDATA_RPM, rpm);	} ;
    void SendAlt(long altcm);
    void SendGPSAlt(long altcm);
    void SendFuel(uint16_t fuel);
    void SendCurrentMilliAmps(int32_t milliamps);
    
    void sendHubByte( uint8_t byte ) ;
    void SendVoltX( uint8_t VoltToSend  , uint8_t indexFieldToSend ) ;

};

extern bool RpmAvailable ;

#endif // OXS_OUT_FRSKY_h





