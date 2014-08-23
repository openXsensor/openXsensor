#ifndef OXS_CONFIG_h
#define OXS_CONFIG_h
// openxvario http://code.google.com/p/openxvario/
// started by Rainer Schloßhan 


//*************************************************************************************
// General explanation of the different options                                                          
//*************************************************************************************
//  This file allows the user to set up different options. Here a summary of the main options
//  1 - Protocol to be used to transmit data to Tx: it must be FRSKY_SPORT when using X serie Rx (X8R, X6R, ...) and NOT FRSKY_SPORT for D serie Rx
//  1.1 - Sensor_ID to be used for X serie receiver 
//  2 - Hardware setting: in this section, you define which sensors are connected and on which arduino pin
//  2.1 - Digital pins used for Serial communication with TX, for reading a PPM signal from TX in order to adjust vario sensitivity, for checking a push button (reset some data) , for generating an analog signal based on vertical speed
//  2.2 - Analog pins used to measure voltages; it requires normally some voltage dividers (= set of 2 resistors)
//  2.3 - Analog pins used to measure current and consumption (it requires a cuurent sensor)
//  3 - General set up to define wich measurements are transmitted and how
//      This part specifies list of codes to be used and how to combine them
//  4 - Set up for vario (optional)
//  5 - Set up for current and voltage measurements (optional)
//  5.1 - Select the reference (VCC or 1.1 internal voltage reference)
//  5.2 - Calibration parameters for current sensor
//  5.3 - Calibration parameters for voltage measurements
//  5.4 - Number of lipo cells to measure (and transmit to Tx)
//  6 - Set up for RPM sensor
//  7 - Set up of Persistent memory 
//
//  Note : active parameters are normally on a line begining by "#define" followed by the name of the parameter and sometime a value
//         To deactivate a parameter, in general, you can add "//" just before "#define"; this line will then be considered as a comment and discarded 
//         Take care that in special cases (see below - e.g. for Pin_Voltage ) the parameter must always be present but can be deactivated using a special value   
//  Note: it could be that the combination of active/non active parameters as currently defined in this document is not consistent (e.g. it make no sense to activate PPM is there is no Vario sensor)
//        This is just the result of many updates in this document.


//**** 1 FrSky protocol to be used  *********************************************************************************
// There are 2 different protocols:
//   - SPORT is used for X serie receivers (like X8R or X6R)
//   - HUB is used for D serie receivers (like D4R-II)
//  Currently, XOS does not automatically detect the type of receiver it is connected to
//  So you have to specify the protocol to upload in the arduino and you have to reload the program with another protocol if you want to use the same hardware OXS on another type of receiver
//
//  In SPORT protocol, there may be several sensors connected on the same bus but each sensor must have a different SENSOR_ID
//  The SENSOR_ID has no impact for the HUB protocol
//*************************************************************************************
#define FRSKY_SPORT	   // put this line as comment if OXS is connected to a D serie receiver ( = Hub protocol); do not comment for X serie receiver.
#define SENSOR_ID    0x1B  // this parameter identifies a device connected on SPORT. It is not allowed having 2 devices with the same ID connected at the same time
                           // valid values are 0x1B, 0xBA, ... (there are 28 values)     


						   //**** 2 Hardware settings ************************************************************
// Here you can specify which pins will be used for which connection 
// Arduino has:
//    - digital pin (named 0, 1, ...13)
//    - analog pin (named A0, A1, ... A8); the number of pins can vary depending on the arduino board
//**** 2.1 Digital pins *********************************************************************************
#define PIN_SerialTX        4  // The pin to transmit the serial data to the frsky telemetry enabled receiver
                               // It is a DIGITAL arduino pin that has to be connected to "Rx" pin from receiver (for D serie RX) or to Signal pin (for X serie RX)
                               // mandatory ; default: 4 ; allowed values are 0 up to 7 but take care not using the same pin as PPM (if PPM is used) 

//#define PIN_PPM           2  // Arduino can read a PPM Signal coming from Tx. This allows to change the vario sensitivity using a pot or a switch on TX.
                               // It is a DIGITAL arduino pin that has to be connected to a signal channel from receiver (like for a servo)
                               // optional; default: 2 ; you can also use pin 3 to read the PPM signal
                               // take care, only pin 2 or 3 may be used for PPM signal    
                               // Put this line as comment if you want to disable the remote control functionality


//#define PIN_PushButton    10  // Arduino can check if a push button has been pushed in order to reset some values (consumption, max altititude, max current, ...)
                               // normally most of those parameters shoud best be kept on Tx side but I think it is not possible for current consumption
                               // It is a DIGITAL arduino pin that has to be connected to a push button, the other pin of the push button being connected to Gnd (ground)
                               // optional; default: 10 ; Do not use a pin that is already used for another purpose
                               // Put this line as comment to completly disable button code
                               
//#define PIN_AnalogClimbRate 3  // 3 the pin used to optionally write the vertical speed to the Rx a1 or a2 pin (can be 3 or 11 because it has to use timer 2)

// Note : The digital pin 8 (PB0/ICP) is the only one to be used to measure RPM 

                            
//**** 2.2 Analog Pins for voltages *********************************************************************************
//   Analog pins can be used to measure up to 6 input voltages (please note that, depending on manufacter, some Arduino pro mini have less Analog pin available) 
//   A voltage can be provided by a battery (e.g. a multicell lipo) or a sensor (e.g. a temperature sensor convert the temperature in a voltage that can be measured) 
//   Note : one Analog pin can also be used to measure a current using a current sensor; the set up for a current sensor is described in section 2.3 (see below);
//          in this case, only 5 voltages can be measured because Arduino has no enough Analog pin
//   The Pin value (on arduino) is a number from 0 up to 7 (0 means A0 = Analog 0, 1 means A1, ...7 means A7)
// !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference volatge is used).
//   It can be that you have to use voltage divider in order to reduce the voltage applied on arduino pin compare to the voltage you want to measure
//    See explanation below about voltage divider and about using VCC or 1.1 internal voltage divider.
//   Note : all voltages are measured to ground; so, for a multicell lipo, it will be max 4.2 volt for the first cell, 8.4 for the second, 12.6 for the third,... 
//   
//   If there is no need to measure 6 voltage, DO NOT SET the line AS COMMENT BUT SET THE VALUE = 8                                         
//   Note :even if it does not make much sense ,the same pin value can be used for several PIN_Voltages (the voltage on this pin will then be measured for each PIN_Voltage setup)                                            
//   Take care : do NOT use pins 4 and 5 if you use a vario                             
//               (those pins are reserved for the barometric sensor)                   
//*************************************************************************************
#define PIN_Voltage1 8    //  Pin for measuring Voltage 1 ( Analog In Pin! )
#define PIN_Voltage2 8    //  Pin for measuring Voltage 2 ( Analog In Pin! )
#define PIN_Voltage3 8    //  Pin for measuring Voltage 3 ( Analog In Pin! )
#define PIN_Voltage4 8    //  Pin for measuring Voltage 4 ( Analog In Pin! )
#define PIN_Voltage5 8    //  Pin for measuring Voltage 5 ( Analog In Pin! )
#define PIN_Voltage6 8    //  Pin for measuring Voltage 6 ( Analog In Pin! )


//**** 2.3 Analog pin used for curent sensor *********************************************************************************
//   It is possible to measure a current (and current comsumption) if a current sensor is connected.
//   This current sensor returns a voltage that depends on the current. This voltage is measured by the arduino via an Analog pin
//   The Pin value (on arduino) is a number from 0 up to 7 (0 means A0, 1 means A1, ...7 means A7)
//   If a current sensor is used, better not to use a pin that is already used by a voltage.
// !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference volatge is used).
//   It can be that you have to use voltage divider in order to reduce the voltage applied on arduino pin
//    see explanation below about voltage divider
//   Take care : do NOT use pins 4 and 5 if you use a vario                             
//               (those pins are reserved for the barometric sensor)                   
// note : The current sensor is normally powered by the 5 volt VCC from Arduino (same as the vario sensor)
//        There are bidirectional sensor and unidirectional sensor.
//        For bidirectional, output is normally equal to VCC/2 when current = 0 Amp, for unidirectional, output is normally 0,6 volt at 0 Amp.
//        If OXS is connected to a battery giving less than 5.2 volt, the supply voltage for the current sensor will vary with the OXS supply voltage.
//        Therefore VCC/2 ( = O amp) will varies with VCC
//        This is an issue if the arduino ADC is configured to use the 1.1 volt internal reference.
//        So, in this case it is better to configure the ADC in order to use VCC as reference for conversion.
//*************************************************************************************
#define PIN_CurrentSensor   2  // The Analog pin the optional current Sensor is connected to (2 = pin A2)



//**** 3 Fields to transmit **************************************************************************************
// Each field send to TX has an field ID; this field_ID let TX know which kind of value it receives and how to use it. Codes are normally different for SPORT and for HUB protocols
// In some cases, it is possible to let Arduino automatically select the right code using "DEFAULTFIELD" (see below).
// Each data that Arduino can transmit has also a code in order to identify the data
// In this section, you must specify which telemetry field_ID has to be used to transmit which data measured/calculated by Arduino
//****3.1 Telemetry fields for SPORT **************************************************************
// This is the list of all (main) telemetry fields supported by SPORT  (X serie Rx) (NB: the codes are defined by Frsky)     
// Note : some of those values may not (yet) be displayed on Taranis
// You can use code from this list when you in which telemetry field a measurent has to be transmitted
//     See below for more explanation
//     Do not modify this list 
//*************************************************************************************
//#define RSSI_ID            0xf101  // please do not use this code because it is already used by the receiver
//#define ADC1_ID            0xf102  // please do not use this code because it is already used by the receiver
#define ADC2_ID            0xf103  
#define BATT_ID            0xf104
//#define SWR_ID             0xf105   // please do not use this code because it is already used by the receiver
#define T1_FIRST_ID        0x0400
#define T1_LAST_ID         0x040f
#define T2_FIRST_ID        0x0410
#define T2_LAST_ID         0x041f
#define RPM_FIRST_ID       0x0500
#define RPM_LAST_ID        0x050f
#define FUEL_FIRST_ID      0x0600
#define FUEL_LAST_ID       0x060f
#define ALT_FIRST_ID       0x0100
#define ALT_LAST_ID        0x010f
#define VARIO_FIRST_ID     0x0110
#define VARIO_LAST_ID      0x011f
#define ACCX_FIRST_ID      0x0700
#define ACCX_LAST_ID       0x070f
#define ACCY_FIRST_ID      0x0710
#define ACCY_LAST_ID       0x071f
#define ACCZ_FIRST_ID      0x0720
#define ACCZ_LAST_ID       0x072f
#define CURR_FIRST_ID      0x0200
#define CURR_LAST_ID       0x020f
#define VFAS_FIRST_ID      0x0210
#define VFAS_LAST_ID       0x021f
#define GPS_SPEED_FIRST_ID 0x0830
#define GPS_SPEED_LAST_ID  0x083f
#define CELLS_FIRST_ID     0x0300
#define CELLS_SECOND_ID    0x0301
#define CELLS_THIRD_ID     0x0302
#define CELLS_LAST_ID      0x030f
// End of list of all telemetry fields supported by SPORT  (defined by Frsky) 


//**** 3.2 Telemetry fields for Hub *************************************************************
// This is the list of all (main) telemetry fields supported by Hub protocol ( = D serie receivers)  (code are defined by Frsky)     
// You can use code from this list when you define in which telemetry field a measurent has to be transmitted
//     See below for more explanation
//     Do not modify this list 
//*************************************************************************************
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
#define FRSKY_USERDATA_VFAS_NEW     0x39 // open9x Vario Mode Only
#define FRSKY_USERDATA_VOLTAGE_B    0x3A
#define FRSKY_USERDATA_VOLTAGE_A    0x3B
#define FRSKY_USERDATA_GPS_DIST     0x3C
#define FRSKY_USERDATA_FUELPERCENT  0x3D
// End of list of all telemetry fields supported by Hub protocol (defined by Frsky) 


//**** 3.3 Telemetry field both for SPORT and HUB *************************************************
// The value DEFAULTFIELD can be used to say that the measurement has to be transmitted using the normal field for this type of measurement.
// This value is valid for both protocols ( = types of receiver = D and X series); Arduino will automatically select the right field_ID taking care of the selected protocol and of the measurement)
// Still not all measurements have a DEFAULTFIELD and on the opposite, some measurement are structure in such a special way, that only DEFULTFIELD makes sense.
// In order to know for wich measurement DEFAULTFIELD value is allowed or must be used, look at the comments in section 3.4 below.
// Note: in the Hub protocol (D serie receiver), DEFAULTFIELD option does not work 100% the same way as the equivalent field_ID for some measurement. That the case for  
//       ALTIMETER (Hub protocol requires sending different fields for Meters and Centimeters)
//       VERTICAL_SPEED (In Hub protocol that are some conversion of value 0.10 and -0.10 into 0.09 and -0.09 because it seems that some version had an issue)
//       CURRENTMA (in Hub protocol, with DEFAULTFIELD , Arduino takes the absolute value of current in milliAmp), divide it automatically by 100 (so no additional multiplier/divider is needed)
// Note: in the hub protocol, when DEFAULTFIELD is used, Aduinino do NOT apply the multiplier/divider/offset (because some fields have already a special formatting inside Arduino (see here above)

#define DEFAULTFIELD                0x00 
                                                  


//**** 3.4  Data that can be transmitted to Tx *************************************************
//  This is the list of codes for each available measurements
//  Do not change those values
//  Use those codes when you define which data has to be transmitted (see below) 
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
// to do : add alt min, alt max ,  rpm max? , current max (not sure that it is neaded because it can be calculated on TX side
// End of list of type of available measurements


//**** 3.5 General set up to define wich measurements are transmitted and how **********************************************************************
// You MUST specify here under ONE ROW for EACH FIELD to tranmit to Tx.
// Each row must contains:
//   - 1 : the code of a field to transmit (e.g. "T1_FIRST_ID" ) (!! see note (1) below)
//   - 2 : a comma 
//   - 3 : the code of a measurement to transmit in this field (e.g. "VOLT1")  (see note (2))
//   - 4 : a comma 
//   - 5 : a multiplier factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to multiply by 10 , ...) (see note (3)) 
//   - 6 : a comma 
//   - 7 : a divider factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to divide by 10 , ...)  (see note (3))
//   - 8 : a comma
//   - 9 : an offset factor to apply after having applied the multiplier and divider ((set "0" to keep the original measurement, "100" to add 100, ...)
//   - 10 : a comma and "\" if there is least one next more (don't fill on the last row);
//                  TAKE CARE that "\" MUST be the LAST character on the row (even no space after)
//                  TAKE CARE that no comment lines ("//...") may exist between rows
//
// Note (1) : The code of a field to transmit is normally to select in the list of fields for SPORT or for HUB protocol depending on the protocol (= serie Rx) you are using
//            Do not use code foreseen for SPORT when you use a D serie receiver.
//            The opposite will perhaps (?) works but it is safier to avoid it. 
//            In some cases (see  "Data that can be transmitted to Tx" here above), you can/must specify the value "DEFAULTFIELD". It means that Arduino will automatically transmit the data in the most normal foreseen field.
//            A code of a field to transmit (e.g. "T1_FIRST_ID" ) may not appear on several rows
//            Sequence of rows does not matter.
// Note (2) : A code of a measurement (e.g. VOLT1) may not appear on several rows
// Note (3) : Multiplier, divider and offset must be integer (no decimal); they can be negative (e.g. "-100")
//            Multiplier and divider can be useful e.g.
//                     - to convert in other measurement system (meter <> foot)
//                     - to convert in percentage (e.g. multiply by 100 and divide by 4000 in order to get the consumption of current in % for a 4000 mAmph accu)
//                     - to adjust the number of digits displayed on Tx telemetry screen.
//                     - to have e.g. a fuel value starting at 100 (in percent) and going down to 0 when consumption increase (then you must use a negative multiplier and an offset of 100%).
//            Multiplier/divider/offset must be set up but they are not always taken into account by XOS; it is the case when:
//                  - CELLS_1_2, CELLS_3_4, CELLS_5_6 is transmitted (because those fields have a special formatting required by Tx
//                  - Field_ID is defined using the option DEFAULTFIELD combined with a Hub protocol (D serie Rx) because special formatting are applied (see 3.3 above)
// Here an example of set up in order to transmit on SPORT protocol
//    - as Altitude : the altitude measurement,
//    - as Vertical speed : the vertical speed measurement
//    - as Current : the current measurement
//    - as Fuel : the current consumption in % for an accu of 4000mAmph starting at 100% 
//    - as Temperature T1 : the VOLT6 measurement divided by 100
//               #define SETUP_DATA_TO_SEND    \
//                        DEFAULTFIELD , ALTIMETER , 1 , 1 , 0 ,\
//                        DEFAULTFIELD , VERTICAL_SPEED , 1 , 1 ,0 ,\
//                        DEFAULTFIELD , CURRENTMA , 1 , 1 , 0,\
//                        FUEL_FIRST_ID , MILLIAH , -100 , 4000 ,0, \
//                        T1_FIRST_ID , VOLT6, 1 , 100, 0
// When the Cell voltages have to be transmitted, the voltages are transmitted by group of 2 over SPORT protocol.
//    For uniformity, the cell voltages are also calculated/saved by group of 2 for the Hub protocol even if they are all transmitted in one frame.
//    So in both cases, the number of row to complete is the number of cells you have divided by 2 and rounded up to the higher integer value.
//    E.g. for a lipo with 3 cells, you must specify 3 / 2 = 1.5 => 2 rows, selecting field CELLS_1_2 in first row  and CELLS_3_4 in second row.
//             There is no need filling a third row because there is no cell 5 or 6
//    You can say that both must be sent as CELLS_FIRST_ID  like this
//                          CELLS_FIRST_ID , CELLS_1_2 , 1, 1 ,\
//                          CELLS_FIRST_ID , CELLS_3_4 , 1, 1
// *********************************************************************
//  IMPORTANT : keep always the line "#define SETUP_DATA_TO_SEND    \"  ; do not insert any comment lines after or between the rows used for the set up.
/*
*/
#define SETUP_DATA_TO_SEND    \
                        DEFAULTFIELD , ALTIMETER , 1 , 1 , 0 ,\
                        DEFAULTFIELD , VERTICAL_SPEED , 1 , 1 ,0 , \
                        T1_FIRST_ID , ALT_OVER_10_SEC , 1 , 1,  0
 //                       DEFAULTFIELD , CURRENTMA , 10 , 1 , 0, \
 //                       FUEL_FIRST_ID , MILLIAH, -100 , 390 , 100, \
 //                       CELLS_FIRST_ID , CELLS_1_2 , 1, 1 , 0


/* Here some typical values for Hub protocol
#define SETUP_DATA_TO_SEND    \
                        DEFAULTFIELD , ALTIMETER , 1 , 1 , 0,\
                        DEFAULTFIELD , VERTICAL_SPEED , 1 , 1 , 0, \
                        DEFAULTFIELD , CURRENTMA , 1 , 1 , 0, \
                        DEFAULTFIELD , CELLS_1_2 , 1 , 1 , 0, \
                        DEFAULTFIELD , CELLS_3_4 , 1 , 1 , 0, \
                        DEFAULTFIELD , RPM , 1 , 1 ,  0,\
                        FRSKY_USERDATA_TEMP1 , VOLT1, 1 , 1 , 0, \
                        FRSKY_USERDATA_TEMP2 , VOLT2, 1 , 1 , 0, \
                        FRSKY_USERDATA_FUEL , MILLIAH , 1, 1 , 0
*/

/* Here some typical values for SPORT protocol

#define SETUP_DATA_TO_SEND    \
                        ALT_FIRST_ID , ALTIMETER , 1 , 1 , 0,\
                        VARIO_FIRST_ID , VERTICAL_SPEED , 1 , 1 , 0,\
                        CELLS_FIRST_ID , CELLS_1_2 , 1, 1 , 0,\
                        CELLS_SECOND_ID , CELLS_3_4 , 1, 1,  0,\   
                        T1_FIRST_ID , VOLT4 , 1 , 1 , 0,\
                        VFAS_FIRST_ID , VOLT2 , 10 , 1 , 0,\
                        CURR_FIRST_ID , VOLT3 , 1, 1 ,  0,\
                        T1_FIRST_ID , ALT_OVER_10_SEC , 1 , 1,  0
                       
*/
 

//**** 4 - Set up for vario ****************************************************************************************
// 4.1 Connecting a MS5611 barometric sensor is an optional feature
//     It can calculate the relative altitude (in meter) and the vertical speed (in meter/sec with 2 decimals)
//     Uncomment the line "#define VARIO" to enable this feature.
//     When a vario is used, Arduino can take care of some parameters to adjust sensitivity and hysteresis                                       
//     Sensitivity can be set between                                                                 
//           20 (conservative setting, reaction time = several seconds)                     
//           150 (fast but lot of errors, reaction time = much less than a second)       
//        40 is a normal setting when measuring small vertical speed (search for lift with a glider)
//     Sensitivity can be predefined by program and/or adjusted from TX
// 4.2 Sensitivity predefined by program  
//     Set up uses 4 parameters:                                                      
//        SENSIVITY_MIN = sensivity being used when vertical speed is lower than VSPEED_MIN ; typical value is 40                        
//        SENSIVITY_MAX = sensivity being used when vertical speed is higher than VSPEED_MAX ; typical value is 100
//                     SENSITIVITY_MAX can be set equal to MIN when higher sensitivity for high Vspd is unwanted  
//        VSPEED_MIN = Sensivity min applies when vertical speed is lower than this value (cm/s); typical value is 20 (cm/sec) 
//        VSPEED_MAX = Sensivity max applies when vertical speed is higher than this value (cm/s); typical value is 100 (cm/sec) 
//           Sensivity is automatically interpolated between VSPEED_MIN ans VSPEED_MAX
// 4.3 Sensitivity adjusted from the Tx using a switch and/or a potentiometer and a servo channel.
//     In this case, a servo pin from Rx has to be connected to Arduino (see PPM in hardware section)
//     Set up uses 4 parameters
//         PPM_RANGE_MIN    = the total range min of the ppm input (Pulse legth in microseconds ); typical value is 981
//         PPM_RANGE_MAX 1999  // the total range max of the ppm input (Pulse legth in microseconds ) ; typical value is 1999
//                The SENSIVITY_PPM_MIN+MAX Parameters define the range in wehich you will be able to adjust 
//         SENSITIVITY_PPM_MIN     //  the min value for sensitivity; typical value could be 20 
//         SENSITIVITY_PPM_MAX     //  the max value for sensitivity; typical value could be 100
//     Note : When Arduino detect a signal from TX, the parameters for the predefined sensitivity are automatically discarded
// 4.4 Arduino can also apply some hysteresis on vertical speed changes
//      The parameter allows to avoid that transmitted Vspd changes to often (wich can result in bad sound alerts)
//      This parameter VARIOHYSTERESIS means that transmitted Vspd will change only if measure VSpd (after filtering) differs from previous transmitted value by more than this parameter
//     Typical value can be 5  (= 5 cm/s); 0 means no hysteresis
// 4.5 Arduino can also deliver the vertical speed as an analog signal that has to be connected to A1 or A2 on receiver
//     This can be useful if you have a receiver that has no digital communication pin (or if it is already used by another sensor)
//     Additional Hardware is required! read the WiKi if you want to use this
//     To activate this:
//     - define the digital pin being used (see hardware section)
//     - set the min and max limits for the vertical speed (in meter/sec)
//        - OutputClimbRateMin value or lower will apply 0 Volt to the receiver
//        - OutputClimbRateMax  value or higher will apply 3.2 Volt to the receiver
// Note : it is not required to comment the sensitivity, hysteresis and OutputClimbRateMin/Max parameters when a vario is not used (those parameters are just discarded)
//*******************************************************************************************************************
#define VARIO // set as comment if there is no vario 

#define SENSITIVITY_MIN 50 
#define SENSITIVITY_MAX 50 
#define VSPEED_MIN 20
#define VSPEED_MAX 100

#define PPM_RANGE_MIN 981
#define PPM_RANGE_MAX 1999
#define SENSITIVITY_PPM_MIN  20
#define SENSITIVITY_PPM_MAX 100

#define VARIOHYSTERESIS 5

#define OutputClimbRateMin -3 
#define OutputClimbRateMax  3 


//**** 5 Set up for current sensor & 6 voltages **************************************************************************************
//**** 5.1 - Select the reference (VCC or 1.1 internal voltage reference) ************************************************************
// Curent and Voltages can be measured in (1023) steps from or VCC or an 1.1 internal reference.
// If VCC is very stable, it is probably more accurate and easier to measure current and voltages based on VCC (so in 1023 steps from VCC).
//     Still this requires that the voltage applied on arduino "RAW" pin is or regulated or at at least always more than 5.5 volt (in order to let the arduino board voltage regulate it at 5 volt).
//     If voltage on "Raw" pin is less than 5.5 volt and changes (e.g. due to servo consumption or low battery) then current and voltage measurements will become wrong.    
// If VCC can't be very stable (e.g. arduino is powered via RX by a 4.8 NiMh) and you need only the voltages (no need for the current measurement), then it is possible to measure based on an arduino 1.1 internal voltage reference.
//     If you need current measurement too, using internal voltage is not "the" solution because current sensor need stable voltage too.
// Take care that voltage dividers (see below) must be calculated in order that arduino analog pin voltage do not exceed or VCC or 1.1 volt depending on the option you choice.   
// Uncomment the "#define USE_INTERNAL_REFERENCE" to activate the 1.1 internal voltage reference (otherwise, measurements will be based on VCC)
//***************************************************************************************
//#define USE_INTERNAL_REFERENCE


//**** 5.2 Calibration parameters for current sensor ************************************************************
// Connecting a current sensor is on optional feature
// It requires some additionnal hardware. It can be an IC like ACS712 (for 5, 20, 30 amp) or ACS758 (for 50, 100, 150, 200 amp).
// Most sensors can read bidirectional currents but ACS758 has "U" types that read only unidirectional current (providing then an higher sensitivity)
// Current sensor has normally to be calibrated based on 2 parameters :
//    offsetCurrentSteps  =  Offset to apply for current; normal value is 1024/2 for a bidirectional sensor because 0 Amp gives VCC/2 (or 1.1 V/2 when using a divider)
//                           Still for unidirectional sensor, voltage at 0 amp is 0.6 volt for 5 volt Vcc; so offset should then normally be 1024 * 0.6 /5 = 123 
//    mAmpPerStep         =  milliAmp per step from Analoge to Digital Converter; the value depend on the sensitivity of the sensor (and on an eventual voltage divider)
//                           The value is normally = V (in mvolt) / (sensitivity in mv/Amp * 1.023) where:
//                                 V is or Vcc (e.g. 5000) or interna 1.1 ref (e.g. 1100) depending on the reference you use
//                                 sensitivity is normally given in the datasheet from your sensor.
//                           E.g. for a ACS758LCB-050U, sensitivity is 60 mv/Amp
//                              So if using 5 volt Vcc => 5000 / (60 * 1.023) = 81.5 
// Uncomment the #define PIN_CurrentSensor (see Hardware section) to enable the current sensor
// note : those parameters are automatically discarded when PIN-CurrentSensor is not defined (= set as comment)
//***************************************************************************************
#define offsetCurrentSteps         0    // 66mv offset (set to zero for now)
#define mAmpPerStep                0.9775   // INA282 with 0.1 ohm shunt gives 5000mv/A     


//***** 5.3 - Calibration parameters for voltage measurements *****************************************************
// For each of the 6 voltages, you can set up an offset and a ratio mVolt per ADC step
// The set up is specific for each Arduino and depends mainly on the resistors used as divider (and so on the reference being used)
// 
// Arduino can not convert a voltage that exceed the reference (Vcc or 1.1 volt) being used.
// If the voltage to measure exceed this value, it has to be divided (scaled down) using a voltage divider.
// For each voltage to scale down, proceed as follow:
// - make a divider with 2 resistors 

//                 ------>  End point  = connect to the device to measure (battery, Currentsensor, ...) 
//                 |
//               __|__   
//              |     |
//              |     |   R2           
//              |     |              
//              |_____|          
//                 |
//                 |------> mid point = connect to Arduino pin A0,A1,A2,A3, A6 or A7
//               __|__   
//              |     |
//              |     |   R1          
//              |     |              
//              |_____|          
//                 |
//                 ------>  connect to Ground
//
// - R1 and R2 are choosen to make sure that voltage apply to Arduino is quiet close to ( but never exceed) VCC or 1.1 volt depending on your choice regarding the current & voltage measurement (see here above)
// - Volt on Arduino pin = VCC (or 1.1 volt) = "max voltage to measure from this sensor" * R1 / (R1 + R2)
// - R1 could be 10 kOhm; so R2 = R1 * ( ( "max voltage to measure from this sensor"  / VCC [or 1.1 depending on the reference] ) - 1 )
//    e.g. using 1.1 internal voltage reference and in order to measure max 6 volt with R1 = 10000, then R2 = 10000 * (( 6 / 1.1 ) - 1) = 45545 Ohm; best rounded up to high available value e.g 47000 ohm
//
// Due to errors on resistor, on Vcc or 1.1 volt reference and on ADC it is required, for best result, to calibrate each voltage measurement as follow:
// - set parameters in oxs_config.h  :
//            - choice to measure based on VCC or 1.1 internal voltage (and (un)comment line "#define USE_INTERNAL_REFERENCE"
//            - set first offset = 0 (default values)
//            - set first mVoltPerStep = 1 (default values)
//            - select a field to transmit the desired voltage and fill the line  "#define SETUP_DATA_TO_SEND" accordingly 
// - load the program in Arduino 
// - apply several different voltages on End point (not exceeding the max voltage allowed based on R1 and R2)
// - for each applied voltage, measure the applied voltage with a voltmeter and read the value received on telemetry panel on Tx
// - set the values in excel (or on a graphic) and calculate the best values for mVoltPerStep and offset (using a linear regression)
//     If this seems to complex, just use 2 voltages as different as possible (but in the range of the normal values you want to measure)
//     and apply calculation based on following example:        .
//     I expect voltage to be normally between 4 volt and 6 volt, so I apply 2 voltages close to those values to End point
//       - for first voltage, voltmeter gives 3510 millivolt and telemetry gives 533
//       - for secong voltage, voltmeter gives 5900 millivolt and telemetry gives 904
//    So mVoltPerStep = (5900-3510) / (904-533) = 6.4420
//    So offset = 3510 - (533 * 6.4420 ) = 76
//  Note : when some pins are not used for voltage measurement, apply default values    
//#define offset_1             0        // Offset to apply for first voltage (= voltage 1) (default value = 0)
//#define mVoltPerStep_1       1        // millivolt per step from Analoge to Digital Converter for first voltage (initial calibration value = 1)
//#define offset_2             0        // Offset to apply for second voltage 
//#define mVoltPerStep_2       1        // millivolt per step from Analoge to Digital Converter for second voltage
// etc ... up to 6
//******************************************************************************************
#define offset_1             0   
#define mVoltPerStep_1       4.89 // = 5000 / 1023 (if Vcc =5 volt)     
#define offset_2             0    
#define mVoltPerStep_2       1    
#define offset_3             0       
#define mVoltPerStep_3       1
#define offset_4             0        
#define mVoltPerStep_4       1        
#define offset_5             0        
#define mVoltPerStep_5       1        
#define offset_6             0        
#define mVoltPerStep_6       1        

//**** 5.4 Number of Lipo cell to measure  (and transmit to Tx) *********************************************************************
//     The different voltages measured under 5.3 are all related to the ground.
//     Arduino can use some of them to calculate the voltage of some lipo cell
//     Define here the number of cell voltages of a lipo you want to transmit; value can be 0 (no cells),1,2,3,4,5,6
//     If a value greater than 1 is defined, then the Arduino will calculate the voltage of each cell based on the difference between 2 succesive voltages starting from Voltage1.
//     The total voltage of all cells wil be calculated on TX side summing all cell voltages again.
//     TX will display the total voltage in a telemetry field named "Cells"
//     TX will also identify the cell with the lowest voltage and display it in a field named "Cell"
//     TX has also a special screen where all voltages will be displayed (see Taranis manual)
//     E.g. if number of cells = 3, 
//           voltage on cell 1 will be voltage measured on PIN_Voltage1
//           voltage on cell 2 will be the difference between voltages measure on PIN_Voltage2 and PIN_Voltage1
//           voltage on cell 3 will be the difference between voltages measure on PIN_Voltage3 and PIN_Voltage2
//           etc...
//    When transmitting cell voltages, you may NOT FORGET TO CONFIGURE the PIN_Voltage and to configure the calibration parameters too .
//    Pins voltage in excess may be used in order to transmit other voltages (e.g. from a temperature sensor)
//    E.g. if NUMBEROFCELLS = 3, PIN_Voltage 1 must be connected to cell 1 (via a voltage divider calculated for about 4.5 volt
//                               PIN_Voltage 2 must be connected to cell 2 (via a voltage divider calculated for about 9 volt
//                               PIN_Voltage 3 must be connected to cell 3 (via a voltage divider calculated for about 13 volt
//                               PIN_Voltage 4, 5 and/or 6 may still be used for other data (temperature, current, ...)
//    Note : You must use voltage dividers to scale down the voltages on each pin of the lipo balance plug
//           If you use the 1.1 internal reference, you can set all R1 = 10 kOhm. Then R2 could best be
//                  33 kOhm for Voltage1, 68 kOhm for Voltage2, 120 kOhm for Voltage3 and 150 kOhm for voltage4
//           Please note that the more cells you have the more unaccurate the measurements become.
//           Probably, it make no sense to measure more that 3 or 4 cells individually
//*****************************************************************************
#define NUMBEROFCELLS 0  // keep this line but set value to 0 (zero) if you do not want to transmit cell voltage.


//**** 6 - Set up for RPM (rotation per minute) ********************************************************************************
//    Optional Feature.
//    It is possible to measure RPM using a sensor connected to pin ICP (=PB0) of arduino.
//    This sensor must provide a level change (0 - 5 volt) on this pin each time a blade goes before the sensor.
//    The number of blades is an important paramater to set up but this is done on Tx side
//    To activate this function, uncomment the #define MEASURE_RPM line here under.
//*************************************************************************************
//#define MEASURE_RPM	1



//**** 7 - Set up of Persistent memory *********************************************************************************
// Optional Feature                                                
// If defined, some telemetry values will be stored in EEProm every 10 seconds.        
// These values will be restored every powerup. (e.g. mAh counted...)
// By doing this we will get ongoing data counts even if the you turn off the modell   
// between flights            
// Uncomment the line "#define SAVE_TO_EEPROM" to activate the persistent storage 
//*************************************************************************************
//#define SAVE_TO_EEPROM      



//**** 9 Reserve for developer. *********************************************************************************
// DEBUG must be activated here when we want to debug one or several functions in some other files.

//#define DEBUG

#ifdef DEBUG
#include "HardwareSerial.h"
#endif

//*************************************************************************************
// do not modify those lines
#ifdef PIN_PPM
 #if PIN_PPM == 2
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp

	#define PPM_INT_MASK			3
	#define PPM_INT_EDGE			1
	#define PPM_PIN_HEX				0x02
	#define PPM_INT_BIT				0x01
 #endif

 #if PIN_PPM == 3
	#define PPM_INTERRUPT			ON // define to use interrupt code in Aserial.cpp
	#define PPM_INT_MASK			0x0C
	#define PPM_INT_EDGE			0x04
	#define PPM_PIN_HEX				0x04
	#define PPM_INT_BIT				0x02
 #endif
#endif


//*************** There is normally no reason changing the 2 next parameters
#define I2CAdd           0x77 // 0x77 The I2C Address of the MS5611 breakout board 
                               // (normally 0x76 or 0x77 configured on the MS5611 module 
                               // via a solder pin or fixed)
                               
#define PIN_LED            13  // The Signal LED (default=13=onboard LED)

//*************************************************************************************
// Other Configuration Options  => various different values to take influence on       
//                    various stuff. Normaly you do not have to change any of these    
//*************************************************************************************
//#define FORCE_ABSOLUTE_ALT // If defined, the height offset in open9x will be resetted upon startup, which results 
// in an absolute height display in open9x . (You can still change to a relative display 
// by pressing [MENU] in the telem.screens
// If not defined, open9x will use the first transmitted altitude as an internal offset, 
// which results in an initial height of 0m
// !!!!!!!!! Not sure that this works in this version !!!!!!!!!!!!!!


#endif// End define OXS_CONFIG_h
