/*
*************************************************************************************
*                                  Important Note:
*  Do not remove or modify this file unless you want to add your own comments.
*  The purpose of this file is only to provide some explanations.
*  The set up has to be done in the oxs_config.h file.
*************************************************************************************

See OpenXsensor https://code.google.com/p/openxsensor/
started by Rainer Schloßhan

************************************************************************************************************************
************************************* General explanation of the different options *************************************
************************************************************************************************************************
*  The file oxs_config.h allows the user to set up different options. Here is a summary of the main options.
*  Note: the oxs_config.h file present on this Google site is not always meaningful.
*       It could be that the combination of active/non active parameters is not consistent.
*       This is just the result of many updates and tests in this document.
*       So take always care to set up the oxs_config.h file according to your needs and check carefully all options.
*       You can also use the OXS configurator in order to generate automatically a valid file.
*
*  1 - Sensor_ID to be used 
*  2 - Serial data pin
*  3 - PPM settings (optional)
*  4 - Vario settings (optional)
*   4.1 - Connecting 1 or 2 MS5611 barometric sensor
*   4.2 - Sensitivity predefined by program
*   4.3 - Sensitivity adjusted from the TX
*   4.4 - Hysteresis parameter
*   4.5 - Different vertical speeds calculations (optional)
*   4.6 - Analog vertical speed (optional)
*  5 - Airspeed sensor settings (optional)
*  6 - Voltage measurements and current sensor settings (optional)
*   6.1 - Select the reference (VCC or 1.1 internal voltage reference)
*   6.2 - Voltages analog pins
*   6.3 - Voltage measurements calibration parameters
*   6.4 - Number of lipo cells to measure (and transmit to Tx)
*   6.5 - Current sensor analog pin
*   6.6 - Current sensor calibration parameters
*  7 - RPM (rotations per minute) sensor settings (optional)
*  8 - Persistent memory settings (optional)
*  9 - Data to transmit - This part specifies list of codes to be used and how to combine them
*  10 - Reserved for developer
*
* Note : Active parameters are normally on a line beginning by "#define", followed by the name of the parameter and most of the time a value.
*        To deactivate a parameter, in general, you can add "//" just before "#define"; this line will then be considered as a comment and discarded.
************************************************************************************************************************


**** 1 - Sensor_ID to be used ******************************************************************************************
* FrSky uses 2 different protocols:
*   - SPORT is used for X series receivers (like X8R or X6R)
*   - HUB is used for D series receivers (like D4R-II)
* OXS supports now both protocols and detects automatically the type of receiver it is connected to.
* So there is no need any more to specify the protocol to upload in the Arduino and
* to reload the program with another protocol if you want to use the same OXS hardware on another type of receiver.
* Still you have to take care that some telemetry fields exist only for the SPORT protocol (see section 8).
*
* For the HUB protocol, only one OXS may be connected to the receiver (other sensors are not allowed). 
* In SPORT protocol, there may be several sensors connected on the same bus (e.g. a GPS) but each sensor must have a different SENSOR_ID.
* So, you have to define a SENSOR_ID for your OXS that is different from sensor Id of other sensors.
* Valid values are 0x1B, 0xBA, ... (there are 28 values)     
* Here the default sensor_IDs used by FrSky for their own sensors (Physical IDs + CRC), so it's better not to use those ones.
*    #define DATA_ID_VARIO  0x00  0
*    #define DATA_ID_FLVSS  0xA1  1
*    #define DATA_ID_FAS    0x22  2
*    #define DATA_ID_GPS    0x83  3
*    #define DATA_ID_RPM    0xE4  4
*    #define DATA_ID_SP2UH  0x45  5
*    #define DATA_ID_SP2UR  0xC6  6
************************************************************************************************************************
#define SENSOR_ID    0x1B   


**** 2 - Serial data pin ***********************************************************************************************
*   OXS has to be connected to the receiver in order to transmit his data.
*   Therefore, one DIGITAL Arduino pin that has to be connected to "Rx" pin from receiver (for D series RX) or to S.Port signal pin (for X series RX).
*   You must specify the Arduino pin being used.
*   Default: 4 ; allowed values are 0 up to 7 but take care not using the same pin for 2 features.
************************************************************************************************************************
#define PIN_SERIALTX  4
                                

**** 3 - PPM settings **************************************************************************************************
*   OXS can (optionally) read a PPM Signal coming from Tx.
*   This allows, from the TX:
*      - to change the vario sensitivity using a pot or a switch on TX.
*      - to switch between uncompensated and compensated vario (when using 2 baro sensors one being connected to a TEK probe)
*      - to reset the airspeed sensor (when the value drift with temperature)
*      - to change the vario compensation factor (when compensated vario uses the airspeed) using a pot or a switch on TX
*   It requires then some mixing on Tx side and some set up in the oxs-config.h file (see below).
*
*   If you want to use this feature, you must:
*      - uncomment the 3 folowing lines
*      - specify in PIN_PPM the Arduino DIGITAL pin being connected to a Rx channel                                It is a DIGITAL Arduino pin that has to be connected to a signal channel from receiver (like for a servo).
*            Default: 2 ; you can also use pin 3 to read the PPM signal.
*            Take care, only pin 2 or 3 may be used for PPM signal.
*            Remove or keep this line as comment if you want to disable the remote control functionality.
*      - specify in PPM_MIN_100 the width of the pulse (in micro seconds) when the channel send a "logical" value = -100.
*              Default value is 988.
*      - specify in PPM_PLUS_100 the width of the pulse (in micro seconds) when the channel send a "logical" value = +100.
*              Default value is 2012.
*   Those 2 last parameters are used in order to calibrate OXS with TX when PPM is used.
*   Probably that if you use the default values, it will work.
*   Still it is safe to calibrate them (but only if you use PPM).
*   To do so, ensure that you transmit the OXS measurement field "PPM" to the TX (e.g. as T1).
*   Note the values on TX (e.g. in field T1) when the TX send respectively -100 and +100 on the PPM channel (use a switch or a pot).
*   Fill in those values in those parameters and reload your program in the Arduino.
************************************************************************************************************************
#define PIN_PPM             2 
#define PPM_MIN_100       988   
#define PPM_PLUS_100      2012   

**** 4 - Vario settings ************************************************************************************************
*
* 4.1 - Connecting 1 or 2 MS5611 barometric sensor is an optional feature: *********************************************
*     It is possible to connect 1 or 2  baro sensors; each sensor must be a MS5611 - like on GY63 board available on ebay
*     Each sensor can calculate the relative altitude (in meter with 1 decimal) and the vertical speed (in meter/sec with 2 decimals).
*     A second sensor can be useful because if it is associated with a TEK probe and if option PPM is used, it is possible to select from the TX the sensor that will generate the vario tones.
*     This allows to switch between a pneumatic compensated and an uncompensated vario.
*     Uncomment the line "#define VARIO" to enable first MS5611.
*     Uncomment the line "#define VARIO2" to enable second MS5611.
*     Both MS5611 are connected in parallel (using same Arduino pins A4 and A5).
*     First MS5611 is the one having the MS5611 CE (chip enable) pin connected to ground. This is the normal set up for e.g. a GY63 module. It reacts on an I2C address 0x77.
*     Second MS5611 must have the MS5611 CE (chip enable) pin connected to VDD. So it gets automatically an I2C address 0x76.
*     Note : When only one MS5611 is used, it must be connected as first MS5611 (CE connected to ground).
*
*     When a vario is used, OXS can take care of some parameters to adjust sensitivity and hysteresis.
*     Normal sensitivity can be set between:
*         - 20 (conservative setting, reaction time = several seconds)                     
*         - 150 (fast but lot of errors, reaction time = much less than a second)       
*        40 is a normal setting when measuring small vertical speed (search for lift with a glider); it gives a reaction time of around 1 sec.
*     Sensitivity can be predefined by program and/or adjusted from TX.
************************************************************************************************************************
#define VARIO  //set as comment if there is no vario
#define VARIO2  //set as comment if there is no second vario

* 4.2 - Sensitivity predefined by program: *****************************************************************************
*     Set up uses 4 parameters:
*        SENSIVITY_MIN = This is the sensitivity being used in normal condition (most of the time).
*                        So this is the main parameter which controls the vario sensitivity.
*                        This value is used by OXS when the difference between the current calculated Vspeed (every 20 ms) and the previous smoothed VSpeed is LESS than the value set in SENSITIVITY_MIN_AT ; typical value is 40.
*        SENSIVITY_MAX = This is the maximum sensitivity being used when Vspeed changes very fast.
*                        This value is use by OXS when the difference between the current calculated Vspeed and the previous smoothed VSpeed is GREATER than the value set in SENSITIVITY_MAX_AT ; typical value is 300.
*        SENSITIVITY_MIN_AT = defines up to which value SENSITIVITY_MIN applies (see above) (cm/s); typical value is 100 (cm/s).
*        SENSITIVITY_MAX_AT = defines from which value SENSITIVITY_MAX applies (see above) (cm/s); typical value is 1000 (cm/s).
*           Sensitivity is automatically interpolated between SENSITIVITY_MIN and SENSITIVITY_MAX.
*     Those parameters allows OXS to automatically have a faster reaction time when Vspeed changes very fast and still keeping a quiet vario in calm condition.
*     Note: SENSITIVITY_MAX can be set equal to SENSITIVITY_MIN if no automatic adaptation is desired in case of fast change.
************************************************************************************************************************
#define SENSITIVITY_MIN 50
#define SENSITIVITY_MAX 300
#define SENSITIVITY_MIN_AT 100
#define SENSITIVITY_MAX_AT 1000

* 4.3 - Sensitivity adjusted from the TX *******************************************************************************
*     Sensitivity can be adjusted from the Tx using a switch and/or a potentiometer on TX and some mixer on a servo channel.    
*     To use this feature, a servo pin from Rx has to be connected to OXS (see PPM in section 3).
*     When OXS detects a signal from TX, the SENSIVITY_MIN defined in section 4.2 is automatically discarded and replaced by a value calculated based on parameters in section 4.3 and on PPM signal.
*
*     Set up uses 4 parameters:
*         SENSITIVITY_MIN_AT_PPM  = When Tx sends this value on the PPM channel, SENSITIVITY_MIN (see section 4.2) will be replaced by SENSITIVITY_PPM_MIN;  typical value could be 10.
*         SENSITIVITY_MAX_AT_PPM  = When Tx sends this value on the PPM channel, SENSITIVITY_MIN (see section 4.2) will be replaced by SENSITIVITY_PPM_MAX;  typical value could be 40.
*              The SENSIVITY_PPM_MIN+MAX parameters define the range in which you will be able to adjust the sensitivity:
*         SENSITIVITY_PPM_MIN     =  the minimum value for SENSITIVITY_MIN that can be assigned using PPM; typical value could be 20.
*         SENSITIVITY_PPM_MAX     =  the maximum value for SENSITIVITY_MIN that can be assigned using PPM; typical value could be 100.
*             SENSITIVITY_MIN is automatically interpolated between SENSITIVITY_PPM_MIN and SENSITIVITY_PPM_MAX depending on the PPM signal.
*     Sensitivity will be changed only when PPM signal is between SENSITIVITY_PPM_MIN - 5 and  SENSITIVITY_PPM_MAX + 5.
*     Important note : Sensitivity is changed based on the absolute value of PPM; so PPM = -25 has the same effect as PPM = 25.
*     This is important in order to combine change in sensitivity with a switch on TX to select the sensor (1 or 2) sending the vertical speed (allowing switching between compensated and uncompensated vario).
************************************************************************************************************************
#define SENSITIVITY_MIN_AT_PPM 10
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20
#define SENSITIVITY_PPM_MAX 100

* 4.4 - Hysteresis parameter ******************************************************************************************
*     OXS can also apply some hysteresis on vertical speed changes.
*     it allows to avoid that transmitted Vspeed changes too often (which can result in bad sound alerts).
*     VARIOHYSTERESIS means that transmitted Vspeed will change only if measured VSpeed (after filtering) differs from previous transmitted value by more than this parameter.
*     Typical value can be 5  (= 5 cm/s); 0 means no hysteresis.
************************************************************************************************************************
#define VARIOHYSTERESIS 5

* 4.5 - Different vertical speeds calculations (optional) **************************************************************
*     When you use two MS5611 sensors or one MS5611 + one 4525D0 (see section 5), OXS can calculate several vertical speeds (or dTE).
*     When the PPM option is implemented, OXS allows to select from TX which value (Vspeed from first or second baro sensor or compensated by airspeed) has to be sent as vertical speed and so will control the vario tone. 
*     This allows switching between e.g. compensated and uncompensated vario.
*     Even if OXS can calculate up to 3 vertical speeds (VERTICAL_SPEED, VERTICAL_SPEED_2, PRANDTL_DTE), it is only possible currently to switch between 2 predefined vertical speeds.
*     To enable this feature, additional parameters are required:
*       1) Specify what are respectively the primary and the secondary vertical speeds using the lines:
*                  #define VARIO_PRIMARY       2  
*                  #define VARIO_SECONDARY     1
*                 where 0 means first ms5611, 1 means second ms5611 , 2 means Prandtl Dte (=vario based on vario 1 + compensation from airspeed sensor).
*       2) Specify a range of PPM value that OXS has to check in order to send or the primary or the secondary vertical speed using the lines:
*                 #define SWITCH_VARIO_MIN_AT_PPM 10 
*                 #define SWITCH_VARIO_MAX_AT_PPM 90 
*             When the ABSOLUTE value of PPM is between SWITCH_VARIO_MIN_AT_PPM (typical value = 10) and SWITCH_VARIO_MAX_AT_PPM (typical value = 90),
*              - OXS will switch to the primary vertical speed IF  PPM is POSITIVE
*              - OXS will switch to the secondary vertical speed IF  PPM is NEGATIVE
*     Note: when the absolute value of PPM is outside this range, OXS will not switch from vertical speed and keep the current one.
*         This principle allows to use a switch on TX simultaneously with a pot in order to control sensitivity or compensation.
*         Switching from positive to negative can be achieved on openTx with a mixer using MULTIPLY by -100%.
*         Sending a PPM value outside this range allows to instruct OXS to apply another order (e.g. reset the airspeed offset) without switching the vertical speed.
****************************************************************************************************************************
#define VARIO_PRIMARY       2  // 0 means first ms5611, 1 means second ms5611 , 2 means vario based on vario 1 + compensation from airspeed
#define VARIO_SECONDARY     1  // 0 means first ms5611, 1 means second ms5611 , 2 means vario based on vario 1 + compensation from airspeed
#define SWITCH_VARIO_MIN_AT_PPM 10
#define SWITCH_VARIO_MAX_AT_PPM 90

* 4.6 - Analog vertical speed (optional) ***********************************************************************************
*     OXS can also deliver the vertical speed (from sensor 1 only) as an analog signal that has to be connected to A1 or A2 on FrSky receiver:
*     This can be useful if you have a receiver that has no digital communication pin (or if it is already used by another sensor)
*     Additional Hardware (1 resistor + 1 capacitor) is required! Read the WiKi if you want to use this.
*     To activate this:
*     - uncomment the 3 following parameters
*     - define in PIN_ANALOG_VSPEED the Arduino DIGITAL pin being used (see below)
*         Default 3; The pin can only be 3 or 11 because it has to use timer 2.
*     - set the min and max limits for the vertical speed (in meter/sec)
*        - ANALOG_VSPEED_MIN value or lower will apply 0 Volt to the receiver
*        - ANALOG_VSPEED_MAX  value or higher will apply 3.2 Volt to the receiver
*************************************************************************************************************************
#define PIN_ANALOG_VSPEED   3
#define ANALOG_VSPEED_MIN -3
#define ANALOG_VSPEED_MAX  3

************************************************************************************************************************
* Note : it is not required to comment the sensitivity, hysteresis, OutputClimbRateMin/Max, ... parameters when a vario,
*        is not used (those parameters are just discarded)
*        The vario setup parameters (sensitivity, hysteresis, PPM, ...) are shared by both MS5611
************************************************************************************************************************


**** 5 - Airspeed sensor settings (optional) ***************************************************************************
* OXS can measure the difference between the static and the dynamic pressures provided by a Prandtl probe.
* It requires a specific sensor 4525DO-DS5AI001DP.
* Please note that this version works with a Vcc = 5 Volt and can measure an airspeed up to 360 km/h.
* There are other versions of 4525DO sensor with Vcc = 3.3 Volt or allowing higher pressure and so higher airspeed.
* The sensor 4525DO is connected to the Arduino in parallel with the MS5611 (so using A4 and A5 on top of Ground and Vcc).
* It is recommended adding a decoupling capacitor (100nf) as close as possible from 4524DO between ground and Vcc.
* Note: at low airspeed, the difference of pressure to measure is so low that the measurement is not reliable.
*       OXS returns automatically a value = zero when airspeed is lower that around 10 km/h.
* Uncomment the line "#define AIRSPEED" to activate the airspeed measurement.
*
* The pressure sensor 4525DO need some calibration in order to return "0" when airspeed is null.
* OXS calibrates automatically the sensor when powered on (considering that the current measurement correspond to a "Zero" airspeed.
* To do so, it is important that there is no wind on the Prandtl probe when OXS start up.
* Anyway, some drift may occurs when the temperature of the sensor changes.
* OXS allows to recalibrate the sensor from the Tx if the PPM option is implemented (see section 3).
* The parameter AIRSPEED_RESET_AT_PPM specifies the value that TX has to send on the PPM channel to force a recalibration.
* Default value is 100.
* The easiest is to configure a TX mixer that will generate a pulse of e.g. 1 sec with this value (e.g. 100).
* Please note that the request for recalibration should be send only when the airspeed (on the probe) is really zero otherwise it would generate wrong measurements.
*
* OXS can also use the airspeed measurements in order to calculate a compensated vario named PRANDTL_DTE (=delta total energy).
* See the web for more informations about dTE (= electronically compensated vario).
* The principle is to try to detect only true air movement by neutralising the up and down resulting from elevator changes.
* Normally, dTE has to be transmitted as Vspeed (vertical speed = defaultfield) because OpenTX does not yet support a specific field for it.
*
* When the option PPM is implemented, it is also possible to adjust the compensation factor between 2 values.
* This requires to uncomment 4 parameters:
*     COMPENSATION_MIN_AT_PPM  specify the PPM value in order to set the compensation to the value specified by COMPENSATION_PPM_MIN; default = 60.
*     COMPENSATION_MAX_AT_PPM  specify the PPM value in order to set the compensation to the value specified by COMPENSATION_PPM_MAX; default = 90.
*     COMPENSATION_PPM_MIN     minimum compensation; in % ; default 80 
*     COMPENSATION_PPM_MAX     maximum compensation; in % ; default 120
************************************************************************************************************************
#define AIRSPEED  MS4525

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 120


**** 6 - Voltage measurements and current sensor settings (optional) ***************************************************
*
* 6.1 - Voltage Reference selection (VCC or 1.1V internal)**************************************************************
*     Current and Voltages can be measured in (1023) steps from or VCC or an 1.1 internal reference.
*     If VCC is very stable, it is probably more accurate and easier to measure current and voltages based on VCC (so in 1023 steps from VCC).
*     Still this requires that the voltage applied on Arduino "RAW" pin is regulated or at at least always more than 5.5 volt (in order to let the Arduino board voltage regulates it at 5 volt).
*     If voltage on "Raw" pin is less than 5.5 volt and changes (e.g. due to servo consumption or low battery) then current and voltage measurements will become wrong.
*     If VCC can't be very stable (e.g. Arduino is powered via RX by a 4.8 NiMh) and you need only the voltages (no need for the current measurement), then it is possible to measure based on the Arduino 1.1 internal voltage reference.
*     If you need current measurement too, using internal voltage is not "the" solution because current sensor needs stable voltage too.
*     Take care that voltage dividers (see below) must be calculated in order that Arduino analog pin voltage do not exceed VCC or 1.1 volt depending on the option you choose.
*     Uncomment the "#define USE_INTERNAL_REFERENCE" to activate the 1.1 internal voltage reference (otherwise, measurements will be based on VCC).
************************************************************************************************************************
//#define USE_INTERNAL_REFERENCE

* 6.2 - Voltages analog Pins *******************************************************************************************
*     Analog pins (A0 to A7) can be used to measure up to 6 input voltages (please note that depending on manufacturer, some Arduino pro mini have less analog pins available) 
*     A voltage can be provided by a battery (e.g. a multicell lipo) or a sensor (e.g. a temperature sensor convert the temperature in a voltage that can be measured) 
*     Note : one analog pin can also be used to measure a current using a current sensor; the set up for a current sensor is described in section 6.5 (see below);
*          Do not use the same analog pin to measure a voltage and a current.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
*     The Pin value to enter in the oxs_config.h is a number from 0 up to 7 (0 means A0 = analog 0, 1 means A1, ...7 means A7).
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use voltage divider in order to reduce the voltage applied on Arduino pin compared to the voltage you want to measure.
*     See explanation below about voltage divider and about using VCC or 1.1 internal voltage divider.
*     Note : all voltages are measured to ground; so, for a multicell lipo, it will be max 4.2 volt for the first cell, 8.4 for the second, 12.6 for the third,... 
*   
*     If there is no need to measure some voltage, set the line as comment or remove the line.
*  Note: even if it does not make much sense, the same pin value can be used for several PIN_VOLTAGE (the voltage on this pin will then be measured for each PIN_VOLTAGE setup)                                            
************************************************************************************************************************
#define PIN_VOLTAGE_1 0      Pin for measuring Voltage 1 ( analog In Pin! )
#define PIN_VOLTAGE_2 1      Pin for measuring Voltage 2 ( analog In Pin! )
#define PIN_VOLTAGE_3 2      Pin for measuring Voltage 3 ( analog In Pin! )
#define PIN_VOLTAGE_4 3      Pin for measuring Voltage 4 ( analog In Pin! )
#define PIN_VOLTAGE_5 6      Pin for measuring Voltage 5 ( analog In Pin! )
#define PIN_VOLTAGE_6 7      Pin for measuring Voltage 6 ( analog In Pin! )

* 6.3 - Voltage measurements calibration parameters ********************************************************************
*     For each of the 6 voltages, you can set up an offset and a ratio mVolt per ADC step.
*     The set up is specific for each Arduino and depends mainly on the resistors used as divider (and so on the reference being used).
*
*     Arduino can not convert a voltage that exceed the reference (Vcc or 1.1 volt) being used.
*     If the voltage to measure exceed this value, it has to be divided (scaled down) using a voltage divider.
*     For each voltage to scale down, proceed as follow:
*      - make a divider with 2 resistors 
*
*                 ------>  End point  = connect to the device to measure (battery, Current sensor, ...) 
*                 |
*               __|__   
*              |     |
*              |     |   R2           
*              |     |              
*              |_____|          
*                 |
*                 |------> mid point = connect to Arduino pin A0,A1,A2,A3, A6 or A7
*               __|__   
*              |     |
*              |     |   R1          
*              |     |              
*              |_____|          
*                 |
*                 ------>  connect to Ground
*
*      - R1 and R2 are chosen to make sure that voltage apply to Arduino is quiet close to ( but never exceed) VCC or 1.1 volt depending on your choice regarding the current & voltage measurement (see here above)
*      - Volt on Arduino pin = VCC (or 1.1 volt) = "max voltage to measure from this sensor" * R1 / (R1 + R2)
*      - R1 could be 10 kOhm; so R2 = R1 * ( ( "max voltage to measure from this sensor"  / VCC [or 1.1 depending on the reference] ) - 1 )
*         e.g. using 1.1 internal voltage reference and in order to measure max 6 volt with R1 = 10000, then R2 = 10000 * (( 6 / 1.1 ) - 1) = 45545 Ohm; best rounded up to high available value e.g 47000 ohm
*
*     Due to errors on resistors, on Vcc or 1.1 volt reference and on ADC it is required, for best result, to calibrate each voltage measurement as follow:
*      - set parameters in oxs_config.h  :
*            - choose to measure based on VCC or 1.1 internal voltage (and (un)comment line "#define USE_INTERNAL_REFERENCE"
*            - set first OFFSET = 0 (default value)
*            - set first MVOLT_PER_STEP = 1 (default value)
*            - select a field to transmit the desired voltage (e.g. T1) and fill the line  "#define SETUP_DATA_TO_SEND" accordingly 
*      - load the program in Arduino 
*      - apply several different voltages on End point (not exceeding the max voltage allowed based on R1 and R2)
*      - for each applied voltage, measure the applied voltage with a voltmeter and read the value received on telemetry panel on Tx  
*      - set the values in excel (or on a graphic) and calculate the best values for MVOLT_PER_STEP and OFFSET (using a linear regression)
*     If this seems too complex, just use 2 voltages as different as possible (but in the range of the normal values you want to measure)
*     and apply calculation based on following example:        .
*      I expect voltage to be normally between 4 volt and 6 volt, so I apply 2 voltages close to those values to End point
*        - for first voltage, voltmeter gives 3510 millivolt and telemetry gives 533
*        - for second voltage, voltmeter gives 5900 millivolt and telemetry gives 904
*      So MVOLT_PER_STEP = (5900-3510) / (904-533) = 6.4420
*      So OFFSET = 3510 - (533 * 6.4420 ) = 76
*  Note: You can apply the same principle if you are measuring something else than a voltage.
*         E.g. You can calibrate a temperature sensor and set the calibration parameter in order to get degree on the TX display.
*  Note: when some pins are not used for voltage measurement, you can set them as comment or remove them. Anyway, if they exist, they will be discarded.
*
*  Note : if you can't calibrate (e.g. you do not have a voltmeter and want that OXS gives the battery voltage, you can normally calculate the theoretical values based on this formula:
*         OFFSET = 0
*         MVOLT_PER_STEP = V / 1023 * (R1 + R2) / R1  where 
*                     - V (in milliVolt) is Vcc (e.g. 5000) or internal 1.1 ref. (e.g. 1100) depending on the reference you use
*                     - R1 and R2 are the resistors used for the divider or 1 if no divider is used
*
*     #define OFFSET_1             0         Offset to apply for first voltage (= voltage 1) (default value = 0)
*     #define MVOLT_PER_STEP_1       1       millivolt per step from Analog to Digital Converter for first voltage (initial calibration value = 1)
*     #define OFFSET_2             0         Offset to apply for second voltage 
*     #define MVOLT_PER_STEP_2       1       millivolt per step from Analog to Digital Converter for second voltage
*     etc. up to 6
************************************************************************************************************************
#define OFFSET_1             0   
#define MVOLT_PER_STEP_1       4.89  // = 5000 / 1023 (if Vcc = 5 volt)     
#define OFFSET_2             0    
#define MVOLT_PER_STEP_2       1    
#define OFFSET_3             0       
#define MVOLT_PER_STEP_3       1
#define OFFSET_4             0        
#define MVOLT_PER_STEP_4       1        
#define OFFSET_5             0        
#define MVOLT_PER_STEP_5       1        
#define OFFSET_6             0        
#define MVOLT_PER_STEP_6       1        

* 6.4 - Number of Lipo cell to measure  (and transmit to Tx) ***********************************************************
*     The different voltages measured under 6.3 are all related to the ground.
*     OXS can use some of them to calculate the voltage of some lipo cells.
*     Define here the max number of cell voltages of a lipo you want to transmit; value can be 0 (no cells),1,2,3,4,5,6.
*     If a value greater than 1 is defined, then the OXS will calculate the voltage of each cell based on the difference between 2 successive voltages starting from Voltage1.
*     The total voltage of all cells will be calculated on TX side summing all cell voltages again.
*     TX will display the total voltage in a telemetry field named "Cells".
*     TX will also identify the cell with the lowest voltage and display it in a field named "Cell".
*     TX has also a special screen where all voltages will be displayed (see Taranis manual).
*     E.g. if number of cells = 3, 
*           voltage on cell 1 will be voltage measured on PIN_Voltage1
*           voltage on cell 2 will be the difference between voltages measure on PIN_VOLTAGE_2 and PIN_VOLTAGE_1
*           voltage on cell 3 will be the difference between voltages measure on PIN_VOLTAGE_3 and PIN_VOLTAGE_2
*           etc.
*     When transmitting cell voltages, you may NOT FORGET to configure the PIN_VOLTAGE_x and to configure the calibration parameters too .
*     Pins voltage in excess may be used in order to transmit other voltages (e.g. from a temperature sensor)
*     E.g. if NUMBEROFCELLS = 3, PIN_VOLTAGE_1 must be connected to cell 1 (via a voltage divider calculated for about 4.5 volt
*                                PIN_VOLTAGE_2 must be connected to cell 2 (via a voltage divider calculated for about 9 volt
*                                PIN_VOLTAGE_3 must be connected to cell 3 (via a voltage divider calculated for about 13 volt
*                                PIN_VOLTAGE_4, 5 and/or 6 may still be used for other data (temperature, current, ...)
*     Notes: You must use voltage dividers to scale down the voltages on each pin of the lipo balance plug
*            If you use the 1.1 internal reference, you can set all R1 = 10 kOhm. Then R2 could best be
*                   33 kOhm for Voltage1, 68 kOhm for Voltage2, 120 kOhm for Voltage3 and 150 kOhm for voltage4
*            Please note that the more cells you have the more inaccurate the measurements become.
*            Probably, it make no sense to measure more that 3 or 4 cells individually
*            If you don't want to transmit cell voltage, set value to 0 (zero) or comment the line.
*            This parameter defines the max number of cells you expect to transmit. If OXS is connected to a lipo having less cells, OXS will send 0 volt for the "missing" cells
************************************************************************************************************************
#define NUMBEROFCELLS    3 

* 6.5 - Current sensor analog pin **************************************************************************************
*     It is possible to measure a current (and current consumption) if a current sensor is connected.
*     Connecting a current sensor is an optional feature.
*     It requires some additional hardware. It can be an IC like ACS712 (for 5, 20, 30 amp) or ACS758 (for 50, 100, 150, 200 amp).
*     Most sensors can read bidirectional currents but ACS758 has "U" types that read only unidirectional current (providing then an higher sensitivity).
*     Those current sensors are quite cheap (see e.g. ebay) and return a voltage that depends on the current. This voltage is measured by OXS via an analog pin.
*     The Pin value to enter in the oxs_config.h is a number from 0 up to 7 (0 means A0, 1 means A1, ...7 means A7).
*     If a current sensor is used, do not to use a pin that is already used by a voltage.
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use a voltage divider in order to reduce the voltage applied on Arduino pin.
*     See explanation above (6.3) about voltage divider.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
* Note: The current sensor is normally powered by the 5 volt VCC from OXS (same as the vario sensor).
*       There are bidirectional sensor and unidirectional sensor.
*       For bidirectional, output is normally equal to VCC/2 when current = 0 Amp, for unidirectional, output is normally 0,6 volt at 0 Amp.
*       If OXS is connected to a battery giving less than 5.2 volt, the supply voltage for the current sensor will vary with the OXS supply voltage.
*       Therefore VCC/2 ( = O amp) will varies with VCC.
*       This is an issue if the Arduino ADC is configured to use the 1.1 volt internal reference.
*       So, in this case it is better to configure the ADC in order to use VCC as reference for conversion.
*       In order to use a current sensor, you have to uncomment the line //#define PIN_CURRENTSENSOR and to specify the calibrations
************************************************************************************************************************
//#define PIN_CURRENTSENSOR   2

* 6.6 - Current sensor calibration parameters **************************************************************************
*     Current sensor has normally to be calibrated based on 2 parameters :
*        OFFSET_CURRENT_STEPS  =  Offset to apply for current; normal value is 1024/2 for a bidirectional sensor because 0 Amp gives VCC/2 (or 1.1 V/2 when using a divider).
*                                 Still for unidirectional sensor, voltage at 0 amp is 0.6 volt for 5 volt Vcc; so offset should then normally be 1024 * 0.6 /5 = 123.
*        MAMP_PER_STEP         =  milliAmp per step from Analog to Digital Converter; the value depend on the sensitivity of the sensor (and on an eventual voltage divider).
*                                 If no divider is used, the value of MAMP_PER_STEP is normally equal to V (in mvolt) / (sensitivity in mV/Amp * 1.023) where:
*                                     - V is Vcc (e.g. 5000) or internal 1.1 ref (e.g. 1100) depending on the reference you use
*                                     - Sensitivity is normally given in the datasheet from your sensor.
*                                         E.g. For a ACS758LCB-050U, sensitivity is 60 mv/Amp
*                                              So if using 5 volt Vcc => 5000 / (60 * 1.023) = 81.5.
*        Note: those parameters are automatically discarded when PIN-CURRENTSENSOR is not defined (= set as comment).
************************************************************************************************************************
#define OFFSET_CURRENT_STEPS         0         66mv offset (set to zero for now)
#define MAMP_PER_STEP                0.9775    INA282 with 0.1 ohm shunt gives 5000mv/A


**** 7 - RPM (rotations per minute) sensor settings (optional) ***************************************************************************
*      It is possible to measure RPM using a sensor connected to pin ICP (=PB0, = pin 8) of OXS.
*      This sensor must provide a level change (0 - Vcc) on this pin each time a blade passes in front of it.
*      The number of blades is an important parameter to set up but this is done on Tx side.
*      To activate this function, uncomment the #define MEASURE_RPM line below.
* Note: The digital pin 8 (PB0/ICP) is the only one to be used to measure RPM.
************************************************************************************************************************
//#define MEASURE_RPM


**** 8 - Persistent memory settings ************************************************************************************
*      Optional Feature.
*      If persistent memory is activated, current consumption will be stored in EEProm every 10 seconds.
*      This value will be restored every power-up.
*      So, you will get ongoing consumption even if the you turn off the model between flights.
*      It is possible to reset the consumption to zero pressing on a push button connected to OXS.
*      Therefore a DIGITAL Arduino pin has to be connected to a push button, the other pin of the push button being connected to Gnd (ground).
*      In order to use this feature, you must:
*          - uncomment the line "#define SAVE_TO_EEPROM"
*          - specify the DIGITAL Arduino pin being used.
*               Default: 10 ; other digital pin can be used; Do not use a pin that is already used for another purpose.
************************************************************************************************************************
#define SAVE_TO_EEPROM
#define PIN_PUSHBUTTON    10   


**** 9 - Data to transmit **********************************************************************************************
*      In this section, you will define which OXS measurements are sent to Tx and in which telemetry field they appear on the Tx telemetry panels.
*      You have also to specify if some scaling have to be applied by OXS.
*
*      Each measurement that OXS can transmit has a name (= OXS measurement name) in order to identify it (see list below).
*
*      The telemetry name are those used in the telemetry screens on Tx.

* Note: some telemetry name can't be sent for D series receiver because it has not been foreseen by FrSky (see below, e.g. A3 and A4).
*      In some cases, it is possible to let OXS automatically select the right telemetry name using "DEFAULTFIELD" as telemetry name (see list below).
*      Still not all OXS measurements may be combined with DEFAULTFIELD e.g. because FrSky and OpenTx have not foreseen this kind of measurement (e.g. for Vario sensitivity).
*      In those cases, you can still get the OXS measurement on Tx display choosing yourself a telemetry name (e.g. sending OXS sensitivity as T1).  
*      On the opposite, some OXS measurements requires such a special formatting, that only DEFAULTFIELD may be used; OXS will then automatically select the right codes.
*      In order to know for which measurement DEFAULTFIELD value is Mandatory, Optional (may be used but other can also be used) or is not allowed, see the list below.
* Note: for D series receiver, DEFAULTFIELD option does not work 100% the same way as the equivalent field_ID for some measurement. That the case for:
*        - ALTIMETER (Hub protocol requires sending different fields for Meters and Centimeters)
*        - VERTICAL_SPEED (In Hub protocol that are some conversion of value 0.10 and -0.10 into 0.09 and -0.09 because it seems that some version had an issue)
*        - CURRENTMA (in Hub protocol, with DEFAULTFIELD , OXS takes the absolute value of current in milliAmp), divide it automatically by 100 (so no additional multiplier/divider is needed)
*
* Note: FrSky requires that some telemetry fields get a special formatting (e.g. 2 cell voltages have to be combined in one field for X series protocol).
*       OXS takes automatically care of this when DEFAULTFIELD is used.
*       OpenTx applies also some reformatting on the received data. E.g. OXS send vertical speed in cm/s but it is OpenTx that display it in m/s with 2 decimals
*       It is also openTX that converts some data from metric to imperial or the opposite.
*       So when you decide to send a OXS measurement into another telemetry field than the normal one, it can be useful to apply some scaling.
*       In order to allow this, you have to specify in OXS a multiplier (default 1), a divider (default 1), an offset (default 0). Default values = no scaling.
*
* Note: OpenTx does not reformat T1, T2, AccX, AccY, AccZ  when Tx is set in metric. So those are fields that can easily be used to get the original OXS measurement (with scaling applied).
*
********************************************  Available OXS Measurements (when all options implemented)  ***************************************************
* OXS Measurement names  unit     Description              Combination with DEFAULTFIELD             When DEFAULTFIELD is used, 
*                                                         Mandatory, Optional, Not allowed          value in on TX in field named 
*    ALTIMETER           cm      Altitude (1)                              Optional (X series)                 Alt
*                                                                          Mandatory (D series)                Alt
*    ALTIMETER_2         cm      Altitude (2)                              Optional (X series)                 Alt
*                                                                          Mandatory (D series)  
*    VERTICAL_SPEED     cm/s     Vertical speed (1)                                  Optional                 VSpd
*    VERTICAL_SPEED_2   cm/s     Vertical speed (2)                                  Optional                 VSpd
*    PRANDTL_DTE        cm/s     Compensated vertical speed (based on airspeed)(3)   Optional                 VSpd
*    PPM_VSPEED         cm/s     Vertical speed selected by PPM (4)       Optional  (X series)                VSpd
*                                                                         Not implemented (D series)  
*    SENSITIVITY        none     Vario sensitivity (1)                             NOT allowed
*    SENSITIVITY_2      none     Vario sensitivity (2)                             NOT allowed 
*    ALT_OVER_10_SEC     m       Difference of Altitude over 10 last sec (1)       NOT allowed
*    ALT_OVER_10_SEC_2   m       Difference of Altitude over 10 last sec (2)       NOT allowed
*    VOLT1             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_1       NOT allowed
*    VOLT2             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_2       NOT allowed
*    VOLT3             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_3       NOT allowed
*    VOLT4             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_4       NOT allowed
*    VOLT5             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_5       NOT allowed
*    VOLT6             mV(5)     Value based on (milli)Volt on PIN_VOLTAGE_6       NOT allowed
*    CELLS_1_2        special    Volt of cells 1 & 2                                 Mandatory             Cell & Cells
*    CELLS_3_4        special    Volt of cells 3 & 4                                 Mandatory             Cell & Cells
*    CELLS_5_6        special    Volt of cells 5 & 6                                 Mandatory             Cell & Cells
*    CURRENTMA         mA        MilliAmp measured by current sensor                 Optional                 Curr
*    MILLIAH          mAh        Consumed milli Amp heure                            Optional                 Fuel
*    RPM              t/min(?)   Rotation per min                                    Mandatory                Rpm
*    AIR_SPEED        knot/10    Air speed                                 Optional (X series)                ASpd
*                                                                          Optional (D series)                Spd (6)      
*    PRANDTL_COMPENSATION cm/s   Compensation for compensated vario                NOT allowed
*    PPM                none     Value from PPM signal (range = -100 /+100)        NOT allowed
*
*   (1) Measurement from first baro (MS5611) sensor
*   (2) Measurement from second baro (MS5611) sensor (can e.g. be connected to a TEK probe to get a pneumatic compensated vario)
*   (3) PRANDTL_DTE is a compensated vertical speed (= delta total energy).
*       It is based on the vertical speed from the first baro but this value is corrected based on airspeed changed measured by a airspeed sensor.
*       So, it requires as well one baro sensor and one airspeed sensor with a Prandtl probe (see section 3 and 4)
*   (4) When 2 vertical speeds are calculated (using or 2 baro sensors or one baro + one airspeed sensors), OXS allows to select from the TX between 2 vertical speeds which one will be sent.
*       This requires that PPM is used to and that the option SWITCH_VARIO_WITH_PPM is activated. See section 3.
*   (5) Unit depends on the calibration parameter that are used (e.g. when a voltage is provided by a temperature sensor, unit can be degree)
*       When used in order to measure Cell(s), calibration must ensure that unit = milliVolt 
*   (6) For D series Rx, the hub protocol does not allow to transmit the airspeed as airspeed. OXS sent then the airspeed in the Gps speed

* 
* Note: when DEFAULTFIELD is Optional or NOT allowed, you can normally select yourself a telemetry field name (e.g. SENSITIVITY can be sent in T1 or T2 ,...) 
*
*
***********************************************   Telemetry fields   ***************************************************
*-- Field in OpenTx ---- Normal associated OXS Measurement names ------------------------
*  code   unit
*        (metric)
*   Alt     m            ALTIMETER
*   VSpd    m/s          VERTICAL_SPEED or VERTICAL_SPEED_2 or PRANDTL_DTE or PPM_VSPEED
*   Curr    A            CURRENTMA
*   Vfas    V            One of VOLT1, VOLT2,...VOLT6
*   T1                   Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   T2                   Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   Rpm                  RPM
*   Fuel                 MILLIAH
*   AccX                 Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   AccY                 Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   AccZ                 Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   A3                   Not available for D series Rx, Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   A4                   Not available for D series Rx, Can be used for one of VOLT1, VOLT2,...VOLT6 or sensitivity, PPM, ...
*   ASpd 1/10 of knot    AIR_SPEED , Not available for D series Rx
*   Cell & Cells         CELLS_1_2 & CELLS_3_4 & CELLS_5_6
*
**** General set up to define which measurements are transmitted and how ***********************************************
* You MUST specify here under ONE ROW for EACH OXS measurement to transmit to Tx.
* Each row must contains:
*   - 1 : the (telemetry) field name in openTx (e.g. "Alt" ) or DEFAULTFIELD (when allowed) (!! see note (1) below)
*   - 2 : a comma
*   - 3 : the OXS measurement name to transmit in this field (e.g. "VOLT1")  (see note (2))
*   - 4 : a comma
*   - 5 : a multiplier factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to multiply by 10 , ...) (see note (3))
*   - 6 : a comma
*   - 7 : a divider factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to divide by 10 , ...)  (see note (3))
*   - 8 : a comma
*   - 9 : an offset factor to apply after having applied the multiplier and divider ((set "0" to keep the original measurement, "100" to add 100, ...)
*   - 10 : a comma and "\" if there is least one next more (don't fill on the last row);
*                  TAKE CARE that "\" MUST be the LAST character on the row (even no space after)
*                  TAKE CARE that no comment lines ("*...") may exist between rows
*
* Note (1) : In many cases (see the list), you can/must specify the value "DEFAULTFIELD". It means that OXS will automatically transmit the data in the most normal foreseen field.
*            Still, in some cases, DEFAULTFIELD is not possible because the OXS measurement has no normal equivalent in openTX. You must then specify a valid (telemetry) field name in openTX.
*            Please note that some (telemetry) field names in openTx (A3, A4, ASpd) are not allowed when using a D series receiver because the hub protocol has no codeId to transmit them.
*            Except DEFAULTFIELD, a telemetry field name (e.g. "T1" ) may not appear on several rows.
*            Sequence of rows does not matter.
* Note (2) : A OXS measurement name (e.g. VOLT1) may not appear on several rows.
* Note (3) : Multiplier, divider and offset must be integer (no decimal); they can be negative (e.g. "-100").
*            Multiplier and divider can be useful e.g.
*              - to convert in other measurement system (meter <> foot)
*              - to convert in percentage (e.g. multiply by 100 and divide by 4000 in order to get the consumption of current in % for a 4000 mAmph accu)
*              - to adjust the number of digits displayed on Tx telemetry screen.
*              - to have e.g. a fuel value starting at 100 (in percent) and going down to 0 when consumption increase (then you must use a negative multiplier and an offset of 100%).
*            Multiplier/divider/offset must be set up but they are not always taken into account by OXS; it is the case when:
*              - CELLS_1_2, CELLS_3_4, CELLS_5_6 is transmitted (because those fields have a special formatting required by Tx
* Here an example of set up in order to transmit on SPORT protocol:
*    - as Altitude : the altitude measurement,
*    - as Vertical speed : the vertical speed measurement
*    - as Current : the current measurement
*    - as Fuel : the current consumption in % for an accu of 4000mAmph starting at 100% 
*    - as Temperature T1 : the VOLT6 measurement divided by 100
*               #define SETUP_DATA_TO_SEND    \
*                        DEFAULTFIELD , ALTIMETER , 1 , 1 , 0 ,\
*                        DEFAULTFIELD , VERTICAL_SPEED , 1 , 1 ,0 ,\
*                        DEFAULTFIELD , CURRENTMA , 1 , 1 , 0,\
*                        Fuel , MILLIAH , -100 , 4000 ,0, \
*                        T1 , VOLT6, 1 , 100, 0
* When the Cell voltages have to be transmitted, the voltages are transmitted by group of 2 over SPORT protocol.
*    For uniformity, the cell voltages are also calculated/saved by group of 2 for the Hub protocol even if they are all transmitted in one frame.
*    So in both cases, the number of row to complete is the number of cells you have divided by 2 and rounded up to the higher integer value.
*    E.g. for a lipo with 3 cells, you must specify 3 / 2 = 1.5 => 2 rows;
*         In each of them, use the telemetry field name DEFAULTFIELD but on the first row OXS measurement name must be CELLS_1_2 and on the second CELLS_3_4
*         There is no need filling a third row with CELLS_5_6 because there is no cell 5 or 6
* **********************************************************************************************************************
*  IMPORTANT : keep always the line "#define SETUP_DATA_TO_SEND    \"  ; do not insert any comment lines after or between the rows used for the set up.


**** 10 - Reserved for developer. **************************************************************************************
* DEBUG must be activated here when you want to debug one or several functions in some other files.
* You can then select the parts that you want to debug by uncommenting the specifics DEBUG parameters you want in each file
* Note: OXS allows to transmit 3 fields named TEST1, TEST2, TEST3. You can fill those fields with whatever you want where you want if you want to transmit additional data to the Tx.
* Just fill in test1Value (or 2, 3) with an int32_t and test1ValueAvailable (or 2, 3) with true and add those OXS measurements in the data to be sent section. 
************************************************************************************************************************
//#define DEBUG




