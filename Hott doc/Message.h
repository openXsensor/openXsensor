//
// Message.h
// 

#include "Arduino.h"


// Baudrate UART
#define SERIAL_COM_SPEED    19200

// Radio keys
#define HOTT_KEY_RIGHT  14
#define HOTT_KEY_DOWN   11
#define HOTT_KEY_UP     13
#define HOTT_KEY_SET    9
#define HOTT_KEY_LEFT   7

// Hott protocol v4 delay
#define HOTTV4_TX_DELAY 1600 // Delai entre octets

//Graupner #33620 Electric Air Module (EAM)
#define HOTT_TELEMETRY_GEA_SENSOR_ID    0x8E // Electric Air Module ID
#define HOTT_BINARY_MODE_REQUEST_ID	    0x80
#define HOTT_TEXT_MODE_REQUEST_ID       0x7f
#define HOTT_TELEMETRY_GAM_SENSOR_ID    0x8d

//Graupner #33600 Gps module
#define HOTT_TELEMETRY_GPS_SENSOR_ID    0x8a
#define HOTT_GPS_SENSOR_TEXT_ID 0xA0 // GPS Module ID          

//Graupner #33601 Vario Module
#define HOTT_TELEMETRY_VARIO_SENSOR_ID  0x89

// String for text mode
#define HOTT_TEXTMODE_MSG_TEXT_LEN 168

// TEXT MODE STRUCTURE
struct HOTT_TEXTMODE_MSG {
  byte start_byte;		//#01 Starting constant value == 0x7b
  byte esc;				//#02 Escape (higher-ranking menu in text mode or Text mode leave)
						//0x00 to stay normal
						//0x01 to exit
						//I will send 2 times, so the ESCAPE works really well, so two data frames with 0x01 in byte 2
  byte warning_beeps;	//#03 1=A 2=B ...
  byte text[8][21];		//#04...#171 168 ASCII text to display to
						// Bit 7 = 1 -> Inverse character display
						// Display 21x8
  byte stop_byte;		//#172 constant value 0x7d
  byte parity;			//#173 Checksum / parity
};

// GENERAL AIR MODULE 
struct HOTT_GAM_MSG {
  byte start_byte;          //#01 start byte constant value 0x7c
  byte gam_sensor_id;       //#02 EAM sensort id. constat value 0x8d=GENRAL AIR MODULE
  byte warning_beeps;       //#03 1=A 2=B ... 0x1a=Z  0 = no alarm
					        /* VOICE OR BIP WARNINGS
  					        Alarme sonore A.. Z, octet correspondant 1 à 26
          					0x00  00  0  No alarm
          					0x01  01  A  
          					0x02  02  B  Negative Difference 2 B
          				        0x03  03  C  Negative Difference 1 C
          					0x04  04  D  
          					0x05  05  E  
          					0x06  06  F  Min. Sensor 1 temp. F
          					0x07  07  G  Min. Sensor 2 temp. G
          					0x08  08  H  Max. Sensor 1 temp. H
              				        0x09  09  I  Max. Sensor 2 temp. I
              					0xA   10  J  Max. Sens. 1 voltage J
        					0xB   11  K  Max. Sens. 2 voltage K
        		                        0xC   12  L  
        					0xD   13  M  Positive Difference 2 M
        					0xE   14  N  Positive Difference 1 N
        					0xF   15  O  Min. Altitude O
        					0x10  16  P  Min. Power Voltage P    // We use this one for Battery Warning
        					0x11  17  Q  Min. Cell voltage Q
        					0x12  18  R  Min. Sens. 1 voltage R
        					0x13  19  S  Min. Sens. 2 voltage S
        					0x14  20  T  Minimum RPM T
        					0x15  21  U  
        					0x16  22  V  Max. used capacity V
        					0x17  23  W  Max. Current W
        					0x18  24  X  Max. Power Voltage X
        					0x19  25  Y  Maximum RPM Y
        					0x1A  26  Z  Max. Altitude Z
        				        */
  byte sensor_id;             	        //#04 constant value 0xd0
  byte alarm_invers1; //#05 alarm bitmask. Value is displayed inverted
				      //Bit#  Alarm field
					  // 0    all cell voltage
					  // 1    Battery 1
					  // 2    Battery 2
					  // 3    Temperature 1
					  // 4    Temperature 2
					  // 5    Fuel
					  // 6    mAh
					  // 7    Altitude
  byte alarm_invers2;     //#06 alarm bitmask. Value is displayed inverted
    					  //Bit#  Alarm Field
    					  // 0    main power current
    					  // 1    main power voltage
    					  // 2    Altitude
    					  // 3    m/s                            
      					  // 4    m/3s
      					  // 5    unknown
    					  // 6    unknown
    					  // 7    "ON" sign/text msg active
  byte cell[6];				//#7 Volt Cell 1 (in 2 mV increments, 210 == 4.20 V)
				        //#8 Volt Cell 2 (in 2 mV increments, 210 == 4.20 V)
				        //#9 Volt Cell 3 (in 2 mV increments, 210 == 4.20 V)
				        //#10 Volt Cell 4 (in 2 mV increments, 210 == 4.20 V)
					//#11 Volt Cell 5 (in 2 mV increments, 210 == 4.20 V)
					//#12 Volt Cell 6 (in 2 mV increments, 210 == 4.20 V)
  uint16_t  Battery1;                   //#13 LSB battery 1 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
				        //#14 MSB 
  uint16_t  Battery2;                   //#15 LSB battery 2 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
					//#16 MSB
  byte temperature1;                    //#17 Temperature 1. Offset of 20. a value of 20 = 0°C
  byte temperature2;                    //#18 Temperature 2. Offset of 20. a value of 20 = 0°C
  byte fuel_procent;                    //#19 Fuel capacity in %. Values 0--100
					  //graphical display ranges: 0-100% with new firmwares of the radios MX12/MX20/...
  uint16_t fuel_ml;                     //#20 LSB Fuel in ml scale. Full = 65535!
					//#21 MSB
  uint16_t rpm;                         //#22 RPM in 10 RPM steps. 300 = 3000rpm
				        //#23 MSB
    uint16_t altitude;                  //#24 altitude in meters. offset of 500, 500 = 0m
				        //#25 MSB
  uint16_t climbrate_L;                 //#26 climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
				        //#27 MSB
  byte climbrate3s;                     //#28 climb rate in m/3sec. Value of 120 = 0m/3sec
  uint16_t current;                     //#29 current in 0.1A steps 100 == 10,0A
				        //#30 MSB current display only goes up to 99.9 A (continuous)
  uint16_t main_voltage;            	//#31 LSB Main power voltage using 0.1V steps 100 == 10,0V
					//#32 MSB (Appears in GAM display right as alternate display.)
  uint16_t batt_cap;                    //#33 LSB used battery capacity in 10mAh steps
					//#34 MSB 
  uint16_t speed;                       //#35 LSB (air?) speed in km/h(?) we are using ground speed here per default
					//#36 MSB speed
  byte min_cell_volt;                   //#37 minimum cell voltage in 2mV steps. 124 = 2,48V
  byte min_cell_volt_num;               //#38 number of the cell with the lowest voltage
  uint16_t rpm2;                        //#39 LSB 2nd RPM in 10 RPM steps. 100 == 1000rpm
					//#40 MSB
  byte general_error_number;      	//#41 General Error Number (Voice Error == 12) TODO: more documentation
  byte pressure;                        //#42 High pressure up to 16bar. 0,1bar scale. 20 == 2.0bar
  byte version;                         //#43 version number (Bytes 35 .43 new but not yet in the record in the display!)
  byte stop_byte;                       //#44 stop byte 0x7D
  byte parity;                          //#45 CHECKSUM CRC/Parity (calculated dynamicaly)
};


class GMessage {

private:

  // HOTT fonctions
  uint8_t checksum (uint8_t *data);
  void _hott_send_telemetry_data();
  void _hott_send_msg(byte *buffer, int len);
  void _hott_send_text_msg();
  char * _hott_invert_all_chars(char *str);
  char * _hott_invert_chars(char *str, int cnt);
  void init_gam_msg();
  void init_gps_msg();
  void init_vario_msg();
  void send(int lenght);
  void send2();
  void _hott_send_msg2(byte *buffer, int len) ;
  void _hott_send_text_msg2();
  void _hott_invert_ligne(int ligne) ;
  void setMainVoltage(float tension);
  void setVoltage1(float volt);
  void setVoltage2(float volt);
  void setLipo(float volt,int lipo);
  void setAmpere(uint16_t amp);
  void setMilliAmpere(uint16_t mA);
  void setAltitudeInMeters(uint16_t alti);
  void setClimbrate(uint16_t sr);
  void setClimbrate_M3(int srmmm);
  void setSpeed(float speed);
  void setTemp (int temp, int capteur);
  void setFuelPercent(uint8_t pourcent);
  void setFuelMilliliters(uint16_t ml);
  void setRPM(uint16_t rpm);
  void alarme(uint8_t son);
  uint16_t read_eprom(int address);
  void write_eprom(int address,uint16_t val) ;


public:
  GMessage();
  void init();
  void main();
  int get_delay(void);
  int get_fade_step(void);
  int pwm_size(void);
  void debug();
  
};





