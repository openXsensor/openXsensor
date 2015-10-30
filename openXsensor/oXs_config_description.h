/*
*************************************************************************************
*                                  Important Note:
*  Do not remove or modify this file unless you want to add your own comments.
*  The purpose of this file is only to provide some explanations.
*  The set up has to be done in the oXs_config.h file.
*************************************************************************************

See OpenXsensor https://github.com/openXsensor/
started by Rainer Schloßhan

************************************************************************************************************************
************************************* General explanation of the different options *************************************
************************************************************************************************************************
*  The file oXs_config.h allows the user to set up different options. Here is a summary of the main options.
*
*  1 - General protocol & Frsky sensor_ID to be used 
*  2 - Serial data pin
*  3 - PPM settings (optional)
*  4 - Vario settings (optional)
*   4.1 - Connecting 1 or 2 barometric sensor(s) 
*   4.2 - Sensitivity predefined by program
*   4.3 - Sensitivity adjusted from the TX
*   4.4 - Hysteresis parameter & Alt compensation based on sensor temp 
*   4.5 - Different vertical speeds calculations (optional)
*   4.6 - Analog vertical speed (optional)
*   4.7 - Calculating glider ratio, average sink/climb rate, average altitude gain/loose
*  5 - Airspeed sensor settings (optional)
*  6 - Voltage measurements and current sensor settings (optional)
*   6.1 - Voltage Reference to measure voltages and current
*   6.2 - Voltages parameters
*   6.3 - Max number of lipo cells to measure (and transmit to Tx)
*   6.4 - Current sensor parameters
*  7 - RPM (rotations per minute) sensor settings (optional)
*  8 - Persistent memory settings (optional)
*  9 - Data to transmit - This part specifies list of codes to be used and how to combine them
*  10 - Sequencer  (ON/OFF) for some digital outputs (E.g. for a light controller)
*  11 - GPS (optional)
*  xx - Reserved for developer
*
* Note : Active parameters are normally on a line beginning by "#define", followed by the name of the parameter and most of the time a value.
*        To deactivate a parameter, in general, you can add "//" just before "#define"; this line will then be considered as a comment and discarded.
************************************************************************************************************************


**** 1 - Protocol & Frsky sensor_ID to be used **********************************************************************
* Currently oXs supports only 2 telemetry protocols : Multiplex & Frsky
* Both are not supported simultanously; if you activate the Multiplex, the Frsky is disactivated (and the opposite) 
* 1.1 Multiplex protocol
*       in order to activate the Multiplex protocol, simply uncomment the line //#define MULTIPLEX
* 1.2 Frsky protocol and device ID (required when Sport protocol is used)
*     FrSky uses 2 different protocols:
*       - SPORT is used for X series receivers (like X8R or X6R)
*       - HUB is used for D series receivers (like D4R-II)
*     oXs supports now both protocols and will detect automatically the type of receiver it is connected to.
*     This way there is no need any more to specify the protocol to upload in the Arduino and
*     to reload the program with another protocol if you want to use the same OXS hardware on another type of Frsky receiver.
*     In case if autodetection does not work reliably for any reason, you can force the use of a specific protocol by uncommenting one of the following lines:
*     #define FRSKY_TYPE_SPORT     //To force SPORT protocol (X series receiver)
*     #define FRSKY_TYPE_HUB       //To force HUB protocol (D series receiver)
*       Note: if both lines are uncommented, SPORT will get the priority over HUB.
*             Take care that some telemetry fields exist only for the SPORT protocol (see section 8).
*     For the HUB protocol, only one OXS may be connected to the receiver (other sensors are not allowed). 
*     In SPORT protocol, there may be several sensors connected on the same bus (e.g. a GPS) but each sensor must have a different SPORT_SENSOR_ID.
*     So, you have to set up the SENSOR_ID for your OXS that is different from sensor Id of other sensors.
*     You define the SPORT_SENSOR_ID with this line : #define SPORT_SENSOR_ID    28 
*     Value must be between 1 and 28 (included)
*     Here the default sensor_IDs used by FrSky for their own sensors, so it's better not to use those ones if you use oXs and Frsky sensor simultanously.
*       VARIO    1
*       FLVSS    2
*       FAS      3
*       GPS      4
*       RPM      5
*       SP2UH    6
*       SP2UR    7
************************************************************************************************************************
//#define MULTIPLEX
#define SPORT_SENSOR_ID    28   


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
*   It requires then some mixing on Tx side and some set up in the oXs-config.h file (see below).
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
* 4.1 - Connecting 1 or 2 barometric sensor(s) is an optional feature: *********************************************
*     oXs uses normally MS5611 barometric sensor. Such a sensor is usually mount on a board (like the GY-63) which have a 3.3 voltage regulator and a I2C level shifter.
*     Still it is possible to use a board with a BMP085 or BMP180 sensor (which are cheaper but less accurate). Please note that only the first sensor can be a BMP085 or BMP180.
*     It is possible to connect 1 or 2  baro sensors; the first sensor can be a BMP085 or BMP180 or a MS5611; the second one must be a MS5611 (because BMPxxx have only one I2C address)
*     Each sensor can calculate the absolute/relative altitude (in meter with 1 decimal) and the vertical speed (in meter/sec with 2 decimals).
*     A second sensor can be useful because if it is associated with a TEK probe and if option PPM is used, it is possible to select from the TX the sensor that will generate the vario tones.
*     This allows to switch between a pneumatic compensated and an uncompensated vario.
*     Using 2 baro sensors can also be an option to consider if you want e.g. a reduced reaction time of the vario for the same noise level.
*        Note: to get a faster reaction time, sensitivity has to be increased (e.g. using 80 instead of 80 as SENSITIVITY_MIN)
*     When 2 baro sensors are used, oXs can transmit as vertical speed the average from both sensors (see VERTICALSPEED_A in section 9) 
*     Uncomment the line "#define VARIO" to enable first sensor.
*     Uncomment the line "#define SENSOR_IS_BMP180" if a BMP085 or BMP180 is used as first sensor instead of a MS5611. 
*     Uncomment the line "#define VARIO2" to enable second sensor.
*     Both sensors are connected in parallel (using same Arduino pins A4 and A5).
*     First sensor ( MS5611 or BMPxxx) reacts on an I2C address 0x77.
*     When used, second sensor (a MS5611) must have the MS5611 CE (chip enable) pin connected to VDD. So it gets automatically an I2C address 0x76. (note: default MS5611 has CE connected to GND and so an I2C address = 0x77)
*     Note : When only one sensor is used, it must be connected as first sensor.
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
*     Sensitivity will be changed only when PPM signal is between SENSITIVITY_MIN_AT_PPM - 5 and  SENSITIVITY_MAX_AT_PPM + 5.
*     Important note : Sensitivity is changed based on the absolute value of PPM; so PPM = -25 has the same effect as PPM = 25.
*     This is important in order to combine change in sensitivity with a switch on TX to select the sensor (1 or 2) sending the vertical speed (allowing switching between compensated and uncompensated vario).
************************************************************************************************************************
#define SENSITIVITY_MIN_AT_PPM 10
#define SENSITIVITY_MAX_AT_PPM 40
#define SENSITIVITY_PPM_MIN  20
#define SENSITIVITY_PPM_MAX 100

* 4.4 - Hysteresis parameter & Alt compensation based on sensor temp ***************************************************
*     OXS can also apply some hysteresis on vertical speed changes.
*     It allows to avoid that transmitted Vspeed changes too often (which can result in bad sound alerts). So it is another way to reduce sensitivity
*     VARIOHYSTERESIS means that transmitted Vspeed will change only if measured VSpeed (after filtering) differs from previous transmitted value by more than this parameter.
*     Typical value can be 5  (= 5 cm/s); 0 means no hysteresis.
*
*    oXs also allows to add an extra compensation to the MS5611 sensor (not to the BMPxxx) in order to reduce the drift of altitude when the internal sensor temperature increase after power on.
*    Internal temperature of MS5611 increases by several degree when it is read by the Arduino.
*    In principe MS5611 sensor contains calibration parameters in order to compensate temperature changes.
*    Still, it seems that those parameters are not always optimal.
*    So oXs allows to apply an extra compensation for the temperature. Therefore you must use fill the parameter ALT_TEMP_COMPENSATION.
*    In order to define the optimal value for your sensor, the easiest way is probably to try several values.
*    Following info can be useful:
*        - It is better to make the tests when the atmospheric pressure is stable (calm day, no wind)
*        - Ensure that altitude is displayed on the Tx
*        - Ensure that MS5611 is protected from light (because the sensor is light sensitive)
*        - First with ALT_TEMP_COMPENSATION set to zero (or the line being as comment), start Tx and Rx. Do not move oXs (so the altitude on Tx should ideally remains 0).
*        - After about 5 minutes, notice the altitude drift.
*        - Power Tx and Rx off for at least 5 min in order to let drop the internal vario temperature.
*        - Repeat this test one or 2 times
*        - If the aftitude drift is more or less the same for each test and exceed 1 meter, it will probably be possible to reduce it filling some value in the parameter ALT_TEMP_COMPENSATION
*        - If the drift is positive, then increase ALT_TEMP_COMPENSATION otherwise decrease it; ALT_TEMP_COMPENSATION can be negative (it should be the case if the initial drift is negative).
*        - It is difficult to calculate the value to be used. On my sensor I noticed that using a value = 1000 could compensate a drift of about 3 meter but it can varies per sensor.
*        - Having filled a value in ALT_TEMP_COMPENSATION, repeat the previous tests (powering Tx and Rx, waiting 5 min, looking at the drift) and increase or decrease further more the value if required.
*    Important note: filling ALT_TEMP_COMPENSATION will not eliminate totally the drift of altitude because:
*         - it can also be the result of small variation of the atmospheric pressure (that can't be compensated)
*         - it can be generated by other factors 
*         - temperature compensation is probably not linear over the whole range of temperature and pressure
************************************************************************************************************************
#define VARIOHYSTERESIS 5

* 4.5 - Different vertical speeds calculations (optional) **************************************************************
*     When you use two baro sensors or one baro sensor + one airspeed sensor (4525D0 - see section 5), OXS can calculate several vertical speeds (or dTE).
*     When the PPM option is implemented, OXS allows to select from TX which value (Vspeed from first or second baro sensor or compensated by airspeed) has to be sent as vertical speed and so will control the vario tone. 
*     This allows switching between e.g. compensated and uncompensated vario.
*     Even if OXS can calculate up to 4 vertical speeds (VERTICAL_SPEED, VERTICAL_SPEED_2, PRANDTL_DTE, VERTICAL_SPEED_A), it is only possible currently to switch between 2 predefined vertical speeds.
*     To enable this feature, additional parameters are required:
*       1) Specify what are respectively the primary and the secondary vertical speeds using the lines:
*                  #define VARIO_PRIMARY       2  
*                  #define VARIO_SECONDARY     1
*                 where 0 means first baro sensor, 1 means second baro sensor  , 2 means Prandtl Dte (=vario based on vario 1 + compensation from airspeed sensor), 3 means average of first and second baro sensors.
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
#define VARIO_PRIMARY       2  // 0 means first baro sensor, 1 means second baro sensor , 2 means vario based on vario 1 + compensation from airspeed , 3 means average of first and second baro sensors
#define VARIO_SECONDARY     1  // 0 means first baro sensor, 1 means second baro sensor , 2 means vario based on vario 1 + compensation from airspeed, 3 means average of first and second baro sensors
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

* 4.7  - Calculating glider ratio, average sink/climb rate ***************************************************************
*     oXs can calculate and transmit glider ratio and average sink/climb rate when airspeed and vspeed stay within some tolerances for more than X seconds (e.g 5 or 10 sec)
*     The calculated values are :
*        - enlapsed time used to calculate glider ratio and average sink/climb rate
*        - averaged sink/climb rate  ( = difference of altitude / enlapsed time )
*        - glider ratio (= distance / difference of altitude) (in fact = airspeed * enlapsed time / difference of altitude )
*     Those values required an airspeed sensor and a vario.
*     Glider ratio is a parameter that can be use e.g. to fine tune the setup of the glider. It makes only sense if the speeds are quite regular.
*     So oXs calculates only when:      
*         - the airspeed does not change by more than a defined % (compared to the beginning of the enlapsed time). This % can be defined by the user in SPEED_TOLERANCE.
*         - the vertical speed stays withing a range of value defined by the user in VSPEED_MIN_TOLERANCE  and VSPEED_MAX_TOLERANCE
*     Every 0.5 sec, oXs checks if the current measurements are within the tolerance. 
*     If oXs is out of tolerance, it resets all calculations using the last (=current) measurements as new references. Glider ratio and average sink/climb are set to 0 
*     If oXs is within tolerance since more than a user defined enlapsed time (defined by GLIDER_RATIO_CALCULATED_AFTER_X_SEC), it performs the calculations
*     Note: in this version of oXs, if you want to sent the calculated field you have to fill the data to transmit section using following code
*         - TEST1 for enlapsed time (in 1/10 of sec) 
*         - TEST2 for averageVspeed (in cm/sec like Vspeed)
*         - TEST3 for gliderRatio (in 1/10 of units)
*     In order to activate the calculations, you must have a vario, an airspeed and uncomment the 4 lines of parameters    
*     In order to deactivate the calculation, it is enough to put the line GLIDER_RATIO_CALCULATED_AFTER_X_SEC as comment
*************************************************************************************************************************
#define GLIDER_RATIO_CALCULATED_AFTER_X_SEC       10        // minimum elapsed time (in sec) to transmit calculations
#define SPEED_TOLERANCE              5                      // in % of speed
#define VSPEED_MIN_TOLERANCE -200                           // out of tolerance when Vspeed is lower than this value (cm/sec)
#define VSPEED_MAX_TOLERANCE  -10                           // out of tolerance when Vspeed is upper than this value (cm/sec)


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
* oXs can send the airspeed in (1/10) of knot/h or in (1/10) km/h. For openTx 2.1.x, you must use the km/h option, for previous version the knot/h option.
* To activate the km/h option, activate this line #define AIRSPEED_IN_KMH
* Put this line as comment to activate the knot/h option
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
#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h (openTx 2.0 expect knot/h while openTx 2.1 expect km/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 120


**** 6 - Voltage measurements and current sensor settings (optional) ***************************************************
*
* 6.1 - Voltage Reference to measure voltages and current **************************************************************
*     Current and Voltages can be measured in (1023) steps from or VCC or an 1.1 internal reference.
*     If VCC is very stable, it is probably more accurate and easier to measure current and voltages based on VCC (so in 1023 steps from VCC).
*     Still this requires that the voltage applied on Arduino "RAW" pin is regulated or at least always more than 5.5 volt (in order to let the Arduino board voltage regulates it at 5 volt).
*     If voltage on "Raw" pin is less than 5.5 volt and changes (e.g. due to servo consumption or low battery) then current and voltage measurements will become wrong.
*     If VCC can't be very stable (e.g. Arduino is powered via RX by a 4.8 NiMh) and you need only the voltages (no need for the current measurement), then it is possible to measure based on the Arduino 1.1 internal voltage reference.
*     If you need current measurement too, using internal voltage is not "the" solution because current sensor needs stable voltage too.
*     Take care that voltage dividers (see below) must be setup in order that Arduino analog pin voltage do not exceed VCC or 1.1 volt depending on the option you choose.
*     - Uncomment the "#define USE_INTERNAL_REFERENCE" to activate the 1.1 internal voltage reference (otherwise, measurements will be based on VCC).
*     - In order to get best measurements, you can setup the voltage reference being used with line #define REFERENCE_VOLTAGE.
*       Value must be defined in millivolt (e.g. 5100 for 5.1 Volt) and may not contains decimals;
*       If ommitted, oXs will automatically apply 1100 (when USE_INTERNAL_REFERENCE is defined) and 5000 (othewise)
 *
************************************************************************************************************************
//#define USE_INTERNAL_REFERENCE
#define REFERENCE_VOLTAGE 5000

* 6.2 - Voltages parameters *******************************************************************************************
*     oXs can measure up to 6 input voltages (please note that depending on manufacturer, some Arduino pro mini have less analog pins available)
*     In order to measure voltages, you :
*       - must specify the Analog pins (A0 to A7) connected to a battery (e.g. a multicell lipo) or to a sensor (e.g. a temperature sensor convert the temperature in a voltage that can be measured)
*       - must specify the values of resistors being used for the voltage dividers (see below)
*       - can specify offset and/or scaling to apply
* 
*     Note : one analog pin can also be used to measure a current using a current sensor; the set up for a current sensor is described in section 6.4 (see below);
*          Do not use the same analog pin to measure a voltage and a current.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
*     
*    The pins being used to measure some voltages are defined in line #define PIN_VOLTAGE. It is better to put this line as comment (having "//" in front) if no one voltage has to be measured.  
*     When used, this line must contains 6 values (separated by comma); the first value is used to measure VOLT1, the second VOLT2, ...up to VOLT6 
*     Each value must be a number from 0 up to 7 (0 means A0 = analog 0, 1 means A1, ...7 means A7) or the value "8" (when a voltage does not have to be measured).
*     Note: the same alalog pin values can be used in several voltages (e.g. for VOLT1 and VOLT6).
*     
*     Note: If lipo voltage to be measured, it MUST start from VOLT1 for 1s, VOLT2 for 2s....., other voltages may be measured but need to be after number of cells to be measured
*         and the cel number MUST be specified in "numberofcells" (see next section)
*         The Ax pin number can be at random depending on which pin is connected for each cell.
*         
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use voltage divider in order to reduce the voltage applied on Arduino pin compared to the voltage you want to measure.
*     For each voltage to scale down, proceed as follow:
*      - make a divider with 2 resistors 
*
*                 ------>  End point  = connect to the device to measure (battery, Current sensor, ...) 
*                 |
*               __|__   
*              |     |
*              |     |   R2 (Resistor_To_Voltage)          
*              |     |              
*              |_____|          
*                 |
*                 |------> mid point = connect to Arduino pin A0,A1,A2,A3, A6 or A7
*               __|__   
*              |     |
*              |     |   R1 (Resistor_To_Ground)          
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
*    The resistors used in voltage dividers have to be specified in lines #define RESISTOR_TO_GROUND and #define RESISTOR_TO_VOLTAGE.
*    Eeach line, must contains 6 values (one for each voltage); each value represent the resistance in Ohm or kOhm but use the same unit for both lines. 
*    If no divider is used for some voltage, set both resistors to 0 (zero)
*    If no divider at all is used, lines can be commented (adding "//" in front)
*     
*     In order to get best voltage measurements, oXs can be calibrated. This optional process allows to compensate tolerances on resistors and on Arduino ADC (analog to digital converter).
*     To calibrate each voltage measurement proceed as follow in order to find the best values to be set up in lines #define OFFSET_VOLTAGE  and #define SCALE_VOLTAGE
*      - set parameters in oXs_config.h  
*            - letting  OFFSET_VOLTAGE = 0 and SCALE_VOLTAGE = 1 (so adding no compensation)
*            - selecting a field to transmit the desired voltage (e.g. T1 for VOLT3) and fill the line  "#define SETUP_DATA_TO_SEND" accordingly 
*      - load the program in Arduino 
*      - apply several different voltages on End point (not exceeding the max voltage allowed based on R1 and R2)
*      - for each applied voltage, measure the applied voltage with a voltmeter and read the value received on telemetry panel on Tx  
*      - set the values in excel (or on a graphic) and calculate the best values OFFSET_VOLTAGE and SCALE_VOLTAGE (using a linear regression)
*     If this seems too complex, just use 2 voltages as different as possible (but in the range of the normal values you want to measure)
*     and apply calculation based on following example:        .
*      I expect voltage to be normally between 4 volt and 6 volt, so I apply 2 voltages close to those values to End point
*        - for first voltage, voltmeter gives 3510 millivolt and telemetry gives 3622
*        - for second voltage, voltmeter gives 5900 millivolt and telemetry gives 6013
*      Then SCALE_VOLTAGE = (5900-3510) / (6013-3622) = 0.99958
*      and OFFSET_VOLTAGE = -3510 + (3622 * 0.99958 ) = 110
*  Note: You can apply the same principle if you are measuring something else than a voltage.
*         E.g. You can calibrate a temperature sensor and set the calibration parameter in order to get degree on the TX display.
*     
*     Line #define OFFSET_VOLTAGE  and #define SCALE_VOLTAGE are optional (can be put as comment adding "//" in front)
*     If defined, each line must contains 6 values. Use 0 (for offset) and 1 (for scale) if no calibration is done for some voltage.
*     
************************************************************************************************************************
                           VOLT1  VOLT2  VOLT3  VOLT4  VOLT5  VOLT6 
#define PIN_VOLTAGE         2    , 0    , 2   ,  3 ,     8 ,    8             
#define RESISTOR_TO_GROUND 12   , 20 ,   30 ,   40 ,    50 ,   60           // set value to 0 when no divider is used for one of this voltage
#define RESISTOR_TO_VOLTAGE 50, 100.1 , 200,   300 ,   500 ,  600           // set value to 0 when no divider is used for one of this voltage
#define OFFSET_VOLTAGE      0 ,    0 ,    0 ,    0 ,     0 ,    0           // can be negative, must be integer
#define SCALE_VOLTAGE       1 ,    1 ,    1 ,    1 ,     1 ,    1           // can be negative, can have decimals


* 6.3 - Max number of Lipo cell to measure  (and transmit to Tx) ***********************************************************
*     The different voltages measured under 6.3 are all related to the ground.
*     OXS can use some of them to calculate the voltage of some lipo cells.
*     Define here the max number of cell voltages of a lipo you want to transmit; value can be 0 (no cells),1,2,3,4,5,6.
*     If a value greater than 1 is defined, then the OXS will calculate the voltage of each cell based on the difference between 2 successive voltages starting from Voltage1.
*     The total voltage of all cells will be calculated on TX side summing all cell voltages again.
*     TX will display the total voltage in a telemetry field named "Cells".
*     TX will also identify the cell with the lowest voltage and display it in a field named "Cell".
*     TX has also a special screen where all voltages will be displayed (see Taranis manual).
*     E.g. if number of cells = 3, 
*           voltage on cell 1 will be voltage measured on the first pin defined in PIN_Voltage
*           voltage on cell 2 will be the difference between voltages measure on second pin and first pin (so VOLT2 - VOLT1)
*           voltage on cell 3 will be the difference between voltages measure on third pin and second pin (so VOLT3 - VOLT2)
*           etc.
*     When transmitting cell voltages, you may NOT FORGET to configure the PIN_VOLTAGE, RESISTOR_TO_GROUND, RESISTOR_TO_VOLTAGE (and optionaly the calibration parameters too) .
*     The pins MUST start and sequenced from VOLT1,2,3,4.. for 1s, 2s,....
*     Pins voltage in excess may be used in order to transmit other voltages (e.g. from a temperature sensor)
*     E.g. if NUMBEROFCELLS = 3, First pin (in the list of 6) must be connected to cell 1 (via a voltage divider calculated for about 4.5 volt
*                                Second pin must be connected to cell 2 (via a voltage divider calculated for about 9 volt
*                                Third pin  must be connected to cell 3 (via a voltage divider calculated for about 13 volt
*                                Other pins may still be used for other data (temperature, current, ...)
*     Notes: You must use voltage dividers to scale down the voltages on each pin of the lipo balance plug
*            If you use the 1.1 internal reference, you can set all R1 = 10 kOhm. Then R2 could best be
*                   33 kOhm for Voltage1, 68 kOhm for Voltage2, 120 kOhm for Voltage3 and 150 kOhm for voltage4
*            Please note that the more cells you have the more inaccurate the measurements become specially if you do not calibrate the voltages.
*            Probably, it make no sense to measure more that 3 or 4 cells individually
*            If you don't want to transmit cell voltage, set value to 0 (zero) or comment the line.
*            This parameter defines the max number of cells you expect to transmit. 
*            If OXS is connected to a lipo having less cells, OXS will send 0 volt for the "missing" cells which still let the TX calculate the total voltage and the lowest cell voltagen 
************************************************************************************************************************
#define NUMBEROFCELLS    3 


* 6.4 - Current sensor  parameters   **************************************************************************************
*     It is possible to measure a current (and current consumption) if a current sensor is connected.
*     Connecting a current sensor is an optional feature.
*     It requires some additional hardware. It can be an IC like ACS712 (for 5, 20, 30 amp) or ACS758 (for 50, 100, 150, 200 amp).
*     Most sensors can read bidirectional currents but ACS758 has "U" types that read only unidirectional current (providing then an higher sensitivity).
*     Those current sensors are quite cheap (see e.g. ebay) and return a voltage that depends on the current. This voltage is measured by OXS via an analog pin.
*     The Pin value to enter in the oXs_config.h is a number from 0 up to 7 (0 means A0, 1 means A1, ...7 means A7).
*     If a current sensor is used, do not to use a pin that is already used by a voltage.
*  !! Take care that the voltage applied to Arduino pin may not exceed Vcc (normally 5 volt) or 1.1 volt (if internal reference voltage is used).
*     It can be that you have to use a voltage divider in order to reduce the voltage applied on Arduino pin.
*     See explanation above (6.2) about voltage divider.
*     Take care : do NOT use pins A4 and A5 if you use a vario or an airspeed (those pins are reserved for the barometric and pressure sensors).
* Note: The current sensor is normally powered by the 5 volt VCC from OXS (same as the vario sensor).
*       There are bidirectional sensor and unidirectional sensor.
*       For bidirectional, output is normally equal to VCC/2 when current = 0 Amp and, for unidirectional, output is normally 0,6 volt at 0 Amp.
*       If OXS is connected to a battery giving less than 5.2 volt, the supply voltage for the current sensor will vary with the OXS supply voltage.
*       Therefore VCC/2 ( = O amp) will varies with VCC.
*       This is an issue if the Arduino ADC is configured to use the 1.1 volt internal reference.
*       So, in this case it is better to configure the ADC in order to use VCC as reference for conversion.
*       In order to use a current sensor, you have to uncomment the line //#define PIN_CURRENTSENSOR and specify the Arduino pin connected to the current sensor. 
*       You must also define 2 parameters depending of the type of sensor being use; those parameters are given in the datasheet of the sensor).
*         - MVOLT_AT_ZERO_AMP  =  milliVolt generated by the sensor when current is 0 Amp: normal value is :
*                                       - for a bidirectional sensor  : Vcc from current sensor / 2 (so = 2500 if sensor is connected to Arduino Vcc and Arduino Vcc is 5 Volt).
*                                       - 600 for unidirectional sensor 
*         - MVOLT_PER_AMP       =  milliVolt per Amp. The value depend on the sensitivity of the sensor (e.g. an ACS712ELCTR-30A-T has a sensitivity of 66 mvolt/A, a ACS758LCB-050U has a sensitivity of 60 mv/Amp)
*        
*        If you use the 1.1 internal reference to measure voltagse and current, you must also use a voltage divider in order to scale down the voltage produced by the current sensor.
*        See the section 6.2 above about voltage divider. The principle are just the same but the names of the 2 paraameters are:
*          - RESISTOR_TO_GROUND_FOR_CURRENT
*          - RESISTOR_TO_CURRENT_SENSOR 
*  Note: those parameters are automatically discarded when PIN-CURRENTSENSOR is not defined (= set as comment).
*  Note: When current sensor is used, oXs can also calculate and transmit current consumption (milliAh) and Fuel (in % going down from 100% to 0%).
*        If you want the last one, then use a setup like "Fuel , MILLIAH , -100 , 4000 ,0" in "data to transmit section" (and replace 4000 by the capacity - in milliAmph - of your battery) (see below).
*        Still, with Tx using openTx or Ersky9x software, it is better to let the Tx calculates those values by it self based on the current. 
*               This ensure that values are consistent; it allows to reset the values on Tx side; it allows to change the value of the battery capacity on Tx side (so without having to reload another set up in Arduino oXs).    
*               E.g on Ersky9x, in Telemetry menu set up "current source"  set "FAS"; in "mAh Alarm", set the mah you want for alarm to sound and select warning sound/voice, 
*               ie 70% of 2200 mAh lipo, use 1540. then the FUEL percentage will start from 100% count down to 0% when 1540 is consumed.
************************************************************************************************************************
//#define PIN_CURRENTSENSOR      2
#define MVOLT_AT_ZERO_AMP        600
#define MVOLT_PER_AMP            60
#define RESISTOR_TO_GROUND_FOR_CURRENT  10
#define RESISTOR_TO_CURRENT_SENSOR      40

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
*      Furthermore for Multiplex protocol you can also specify a range value in order to set alarms on/off.
*
*      Each measurement that OXS can transmit has a name (= OXS measurement name) in order to identify it (see list below).
*      Most (but not all) of the oXs measurements can be transmitted via both protocolss but there are a few exception 
*
*      In Multiplex Protocol, for each oXs measurement to transmit, you only have to specify the line number (2...15) where the measurement has to appear on the display.
*            On Tx side there is no specific calculation on the transmitted data. 
*      In Frsky Protocol, each transmitted measurement is identified by some Frsky code while transmitted.
*            Depending on the Frsky code, Tx knows which calculations have to be done, which conversion, where/how to display/record the value.
*            The setup up in oXs is based on the telemetry names used in the telemetry screens on Tx side.

*
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
*        oXs             oXs     Description              Combination with DEFAULTFIELD             When DEFAULTFIELD is used,    Mpx unit
*   Measurement names   unit                                  Mandatory, Optional, Not allowed          value in on TX in field named 
*    ALTIMETER           cm      Altitude (1)                              Optional (X series)                 Alt                1m
*                                                                          Mandatory (D series)                Alt
*    ALTIMETER_2         cm      Altitude (2)                              Optional (X series)                 Alt                1m
*                                                                          Mandatory (D series)  
*    VERTICAL_SPEED     cm/s     Vertical speed (1)                                  Optional                 VSpd                0.1m/s
*    VERTICAL_SPEED_2   cm/s     Vertical speed (2)                                  Optional                 VSpd
*    VERTICAL_SPEED_A   cm/s     Vertical speed (7)                                  Optional                 VSpd
*    PRANDTL_DTE        cm/s     Compensated vertical speed (based on airspeed)(3)   Optional                 VSpd
*    PPM_VSPEED         cm/s     Vertical speed selected by PPM (4)       Optional  (X series)                VSpd
*                                                                         Not implemented (D series)  
*    SENSITIVITY        none     Vario sensitivity (1)                             NOT allowed
*    SENSITIVITY_2      none     Vario sensitivity (2)                             NOT allowed 
*    ALT_OVER_10_SEC     m       Difference of Altitude over 10 last sec (1)       NOT allowed
*    ALT_OVER_10_SEC_2   m       Difference of Altitude over 10 last sec (2)       NOT allowed
*    VOLT1             mV(5)     Value read on first PIN_VOLTAGE                   NOT allowed
*    VOLT2             mV(5)     Value read on 2d PIN_VOLTAGE                      NOT allowed
*    VOLT3             mV(5)     Value read on 3d PIN_VOLTAGE                      NOT allowed
*    VOLT4             mV(5)     Value read on 4th PIN_VOLTAGE                     NOT allowed
*    VOLT5             mV(5)     Value read on 5th PIN_VOLTAGE                     NOT allowed
*    VOLT6             mV(5)     Value read on 6th PIN_VOLTAGE                     NOT allowed
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
*    REL_ALTIMETER       cm      Relative altitude (1)                       Not implemented in FRSKY protocol                 1m
*    REL_ALTIMETER_2     cm      Relative altitude (2)                       Not implemented in FRSKY protocol                 1m
*    ALTIMETER_MAX       cm      Max relative altitude (1)                   Not implemented in FRSKY protocol                 1m
*    CELL_1             mV(5)    Value based on first PIN_VOLTAGE            Not implemented in FRSKY protocol                 0.1V
*    CELL_2             mV(5)    Value based on first and 2d PIN_VOLTAGE     Not implemented in FRSKY protocol                 0.1V
*    CELL_3             mV(5)    Value based on 2d and 3d PIN_VOLTAGE        Not implemented in FRSKY protocol                 0.1V
*    CELL_4             mV(5)    Value based on 3d and 4th PIN_VOLTAGE       Not implemented in FRSKY protocol                 0.1V
*    CELL_5             mV(5)    Value based on 4th and 5th PIN_VOLTAGE      Not implemented in FRSKY protocol                 0.1V
*    CELL_6             mV(5)    Value based on 5th and 6th PIN_VOLTAGE      Not implemented in FRSKY protocol                 0.1V
*    CELL_MIN           mV(5)    Value based on CELL_1 ... CELL_6            Not implemented in FRSKY protocol                 0.1V
*    CELL_TOT           mV(5)    Value based on VOLT1...VOLT6                Not implemented in FRSKY protocol                 0.1V


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
*   (7) Measurement is the average of first and second baro (MS5611) sensor 
* 
* Note: when DEFAULTFIELD is Optional or NOT allowed, you can normally select yourself a telemetry field name (e.g. SENSITIVITY can be sent in T1 or T2 ,...) 
*
*
***********************************************   Telemetry fields in Frsky protocol (= names on Tx) *******************
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
*   Cell & Cells         CELLS_1_2 & CELLS_3_4 & CELLS_5_6 (cell is the lowest cell and cells the total voltage)
*
* Note: in Ersky9: 
*    Alt is in custom display in ERSKY9x but there is also a Alt displays on the screenvaving the v1-v6 which is from GPS altitude 
*    
***** General set up to define which measurements are transmitted and how ***********************************************
*     You MUST specify ONE ROW for EACH OXS measurement to transmit to Tx.
*     The content of the line is different for Multiplex and for Frsky sprotocol
* 9.1 Multiplex protocol : each row must contains:
*        -  1 : the line number where the measurement has to be appear on the display. Valid number are from 2 to 15, do not use twice the same line number
*        -  2 : a comma
*        -  3 : the OXS measurement name to transmit in this field (e.g. "VOLT1")  (see note (2))
*        -  4 : a comma
*        -  5 : a multiplier factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to multiply by 10 , ...) (see note (3))
*        -  6 : a comma
*        -  7 : a divider factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to divide by 10 , ...)  (see note (3))
*        -  8 : a comma
*        -  9 : an offset factor to apply after having applied the multiplier and divider ((set "0" to keep the original measurement, "100" to add 100, ...)
*        - 10 : a comma
*        - 11 : an alarm LOW value (see note (4))
*        - 12 : a comma
*        - 13 : an alarm HIGH value (see note (4))
*        - 14 : a comma and "\" if there is least one next more (so don't fill on the last row);
*                  TAKE CARE that "\" MUST be the LAST character on the row (even no space after)
*                  TAKE CARE that no comment lines ("*...") may exist between rows
* 9.2 Frsky protocol : each row must contains:
*        -  1 : (telemetry) field name in openTx (e.g. "Alt" ) or DEFAULTFIELD (when allowed) (!! see note (1) below)
*        -  2 : a comma
*        -  3 : the OXS measurement name to transmit in this field (e.g. "VOLT1")  (see note (2))
*        -  4 : a comma
*        -  5 : a multiplier factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to multiply by 10 , ...) (see note (3))
*        -  6 : a comma
*        -  7 : a divider factor to apply on the value to transmitted (set "1" to keep the original measurement, 10 to divide by 10 , ...)  (see note (3))
*        -  8 : a comma
*        -  9 : an offset factor to apply after having applied the multiplier and divider ((set "0" to keep the original measurement, "100" to add 100, ...)
*        - 10 : a comma and "\" if there is least one next more (don't fill on the last row);
*                  TAKE CARE that "\" MUST be the LAST character on the row (even no space after)
*                  TAKE CARE that no comment lines ("*...") may exist between rows
  
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
* Note (4) : alarm limits must be integer (no decimal); they can be negative (e.g. "-100"). 
*                In order to set the alarms foresseen in the Multiplex protocol, oXs will procceed as follow:
*                 - adapt the number of decimals in the oXs measurement (most oxs measurements have more decimals than foreseen in Multiplex protocol)
*                 - apply the Multiplier/divider/offset parameters
*                 - compare the result with the alarm LOW & HIGH values
*               The limits have to be fixed taking into account all the digits that apears on the display.
*                  E.g. Voltages are displayed in 0.1 volt. If you want to get an alarm when voltage is equal or lower than 11.2 volt, set the alarm low value to 112.
*              In order to avoid alarms, set the LOW value to -16384 and/or the HIGH value to 16383 (limits results from the 15 bits in Multiplex protocol)
*   
* Here an example of set up in order to transmit on Multiplex protocol
*     - on line 3, the relative altitude measurement
*     - on line 6, the vertical speed measurement (with alarms if it exceed 50m/s)
*            #define SETUP_MULTIPLEX_DATA_TO_SEND    \
*                       3 , ALTIMETER , 1 , 1 , 0 , -16384 , 16383,\
*                       6 , VERTICAL_SPEED , 1 , 1 , -500 , 500
*
* Here an example of set up in order to transmit on Frsky protocol:
*    - as Altitude : the altitude measurement,
*    - as Vertical speed : the vertical speed measurement
*    - as Current : the current measurement
*    - as Fuel : the current consumption in % for an accu of 4000mAmph starting at 100% (still best to avoid sending Fuel when Tx can calculate it - see current section) 
*    - as Temperature T1 : the VOLT6 measurement divided by 100
*               #define SETUP_FRSKY_DATA_TO_SEND    \
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

**** 10 - Sequencer (ON/OFF) for several digital outputs **************************************************************************************
* oXs allows you to control (HIGH/LOW) up to 6 digitals Arduino outputs in different sequences.
* Each sequence is composed of one or several steps; each step defines for how long (= a duration) which outputs are HIGH and which outputs are LOW.
* oXs determines normally the sequence to be played based on the signal received on a PPM channel (see section 3 in order to set up a PPM signal). 
* Still there are 2 exceptions:
*    At power on or when no PPM channel is configured/received, oXs will generate as default the sequence defined in the line #define SEQUENCE_m100 (see below)
*    When a low voltage alarm is configured (see below) and if the voltage becomes low, oXs will generate the sequence defined in the line #define SEQUENCE_LOW (see below) as long as the voltage remains low   
* When a sequence has been played, oXs can or repeat it or just wait for a new sequence. The difference is made in the set up of the sequence.
* Each time a new (= different) valid PPM signal is received, oXs start immediately the corresponding sequence (even if the current sequence is not completely played)
* - In order to use the sequencer functionality, you first have to define which Arduino digital pin have to be controlled by the sequencer.
*     The arduino pins that can be controlled are the pin 13, 12, 11, 10 , 9 and 8.
*     This set up is achived by a line like : #define SEQUENCE_OUTPUTS 0b100100
*     Each bit (1 or 0 after the "b") represent an output; the least significant bit correspond to pin 8, the bit on the left pin 9 etc... up to pin 13  
*     Put 1 when the pin has to be controlled by the sequencer, 0 otherwise; In this example, it means that only pins 13 and 10 would be controlled.
*     Note: if the line #define SEQUENCE_OUTPUTS xxxx is omitted or put as comment, then the sequencer is not active at all.
*           Take care not to use the same pin for the sequencer and for another oXs funtionallity (e.g. as Tx pin, for push button, for PPM, for RPM, ...)
*           If a pin is set up with 0 (= not controlled by oXs), it will never be forced to HIGH or LOW by the sequencer even if a 1 or 0 is set up in a sequence.
*           When sequencer is activated ( SEQUENCE_OUTPUTS is defined) PPM signal is automatically used ONLY to control the sequence (so PPM can't control any more vario sensitivity, ...)
*           Current passing through the digital pins of Arduino should not exceed 40mA per pin (and 200 mA for all pins).
*               In case you want to have a higher current (which is the case for most high-power LEDs and LED strips), you need to add a transistor. Connection diagram could easily be found in Google.
* - Then you can (optionnal) specify the units used for defining the durations
*       By default, the durations are expressed in 10 milli second. A parameter allow you to increase the unit in multiple of 10 milli second;
*       E.g with a line like #define SEQUENCE_UNIT 50, the durations will become multiple of 500 milli second (= 50 * 10).
*       Note; this parameter should be an integer between 1 and 1000. So, take care that it is not possible to get a duration less than 10 msec.
*             If this line is ommitted (or as comment), the default value (1 = 10 msec) will be applied.
* - Then you have to define the sequences being used for each value of the PPM channel
*       You can define up to 9 different sequences.
*       A sequence is defined by a line like : #define SEQUENCE_m50    200 , 0b100000 , 300 , 0b000000 , 100 , 0b100100
*       Each sequence is identified by the value of the ppm signal that will activate it; suffix m100 (m = minus) means that it should be activated when ppm is about -100, m75 is for ppm = -75, 75 is for ppm = +75, etc...
*       Sequence suffix are in multiple of 25; so valid sequence suffix are only : m100, m75, m50, m25, 0, 25, 50, 75 and 100
*       Each sequence is composed of several steps (from 1 up to 126 steps or even more) (separated by ",")
*          Each step is composed of 2 parameters (also separated by ",") : a duration and a combination (LOW/HIGH) of outputs
*             - A duration can be any value between 0 and 255.
*               The value fix the minimum duration that a combination of outputs has to be applied. Duration (in msec) = value * SEQUENCE_UNIT * 10
*               So a value = 2 means a duration of 1 sec (if SEQUENCE_UNIT = 50).
*               Value = 0 has a special meaning. When oXs reachs a duration = 0, it applies the corresponding combination of outputs and keeps it for an unlimitted time.
*               This allows to force the outputs to stay with a specific combination after having played the sequence.
*               If duration = 0 is used, it should be in the last step of the sequence (because oXs will never apply the following steps).
*               If duration is set to 0 in the first step, oXs will apply directly the specific combination of outputs and keep it. 
*               If duration = 0 is not used in a sequence, oXs will automatically repeat the whole sequence after reaching the last step.
*               Note: if you need a duration that is longer than the max duration (= 255 * SEQUENCE_UNIT * 10 msec), you can put several steps with the same combination of outputs.   
*             - A combination (LOW/HIGH) of outputs defines which pins have to be set to LOW and which one to HIGH
*               A combination can be defined in binary format so setting six 1 (HIGH) and/or 0 (LOW) just after "0b" (e.g. 0b100100)
*               The least significant bit correspond to pin 8, the bit on the left pin 9 etc... up to pin 13.
*               So if SEQUENCE_OUTPUTS = 0b110111, then 0b100100 means that:
*                   - pins 13 and 10 have to be HIGH,
*                   - pins 9 and 8  have to be LOW
*                   - the others (pin 12 and 11) are not controlled by sequence because of the value assigned to SEQUENCE_OUTPUTS = 0b100111 
*      So #define SEQUENCE_m50    2 , 0b100000 , 3 , 0b000000 , 1 , 0b100100 means (assuming that SEQUENCE_OUTPUTS = 0b100100 and SEQUENCE_UNIT = 50, ):
*            - set pin 13 to HIGH and  pin 10 to 0 (= 0b100000) when PPM signal becomes -50
*            - then wait at least for 2 * 50 * 10 = 1000 ms = 1 sec before changing the outputs
*            - after 1 sec, set pin 13 ( and pin 10) to LOW (=0b000000) for a duration of 1.5 sec (3 * 50 * 10)
*            - after 1.5 sec, set pin 13 and 10 to HIGH for a duration of 0.5 sec (1 * 50 * 10)
*            - after 0.5 sec repeat first step (pin 13 HIGH for 1 sec)
*            - continue with next steps
*      Note: when a sequence name is not defined, oXs handles it like it would be defined with 0 , 0b000000 (so no repeat, all outputs LOW)
* - Finally you can (but it is not mandatory) set up the condition(s) for a low voltage detection. When a voltage becomes too low, oXs starts automatically SEQUENCE_LOW (and discard PPM channel)
*     A low voltage condition can be set up based on 1 or 2 voltage(s):
*         - the voltage on the Arduino pin defined by the 6th parameter PIN_VOLTAGE; this set up is achived by a line like : #define SEQUENCE_MIN_VOLT_6 6000 where 6000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget assign a pin number to the 6th parameter in #define PIN_VOLTAGE and to fill (if requested) the 6th parameter of other voltage parameters.        
*                 The pin defined in the 6th parameter of PIN_VOLTAGE can be the same as another parameter in PIN_VOLTAGE; this can be useful if you want to set up low voltage parameters too.
*         - the lowest lipo cell voltage; this set up is achived by a line like : #define SEQUENCE_MIN_CELL 3000 where 3000 is the "low" voltage in mVolt.
*           Note: if you use this option, do not forget to define the other voltage parameters PIN_VOLTAGE , etc ... and NUMBEROFCELLS        
*     Note:  when no one low voltage parameter is defined, oXs will not automatically start SEQUENCE_LOW.
*            when both voltage parameters are defined, oXs will automatically start SEQUENCE_LOW as soon as one of the 2 voltages becomes low.
*            If you want that oXs notifies a low voltage detection do not forget to also define SEQUENCE_LOW (otherwise, oXs will just set all output pins to LOW)
*            If you have telemetry, you can also make a set up on Tx side in order to detect a low voltage and then send a specific value on the ppm channel.
*               In this case you do not have to define the set up in oXs and the same device can be used on several model.
************************************************************************************************************************

**** 11 - GPS (optionnal)  *********************************************************************************************
* It is possible to connect a GPS module to Arduino. In this case, oXs will transmit to the Tx some data generated by the GPS module.
* Note: data are sent to the receiver only when the GPS has a fix. 
*     So, it can take several minutes before oXs start sending GPS data.
*     If GPS lost his fix, oXs will stop sending GPS data until GPS got a new fix.
* When Frsky (SPORT or HUB) receiver is used, oXs will always send longitude, latitude, altitude, ground speed and course. 
*   This does not require any additional lines in the section "Data to transmit" (see 9 here above) 
* When Multiplex receiver is used, the user has well to specify under the line "#define SETUP_MULTIPLEX_DATA_TO_SEND" which data are sent and the line number where the data have to appear on the display.
* Hardware points of attention.
*   Supported GPS modules: oXs supports modules based on UBLOX GPS (easily available on ebay or aliexpress) like Neo6M, Neo7M and Neo8M. 
*   Most modules have a build in voltage regulator in order to drop the voltage down to 3.3 volt which is about the max allowed by the GPS
*   They have 4 pins available that have to be connected to Arduino
*     - GPS gound is connected to Arduino ground
*     - GPS Vcc is normally connected to Arduino Raw pin (which is normally connected to Vcc from the receiver). 
*          Still take care that the voltage regulator on the GPS module is, most of the time, foreseen for a maximum voltage of 6 Volt while arduino Raw pin accept more. 
*          So, if you Raw pin get more that 6 volt, it is safe to add another voltage regulator to drop the GPS Vcc down.  
*    - GPS Tx pin is connected to Arduino Rx pin      
*    - GPS Rx pin is connected to a resistor (e.g. 10k) and the other pin of the resistor is connected to Arduino pin 6 (digital pin 6). 
*          This resistor is added (at least for a Arduino 5 volt) in order to protect the GPS pin. 
*          This is requested because Arduino will generate a signal with a high level equal to arduino Vcc (so normally 5 volt) while the GPS module should normally not accept more than 3.3 Volt.
*          To be safier, you could even add a second resistor between GPS Rx pin and Ground (value= 22k) but, in my case, it worked without this second resistor. 
*          Note: it would be possible to use another pin than Arduino pin 6 but then it requires to change some parameters in file oXs_gps.cpp (see "Setup the GPS sensor").
*   !! IMPORTANT NOTE : 
*      Arduino Rx pin is used for 2 purposed:  getting the data from the GPS and uploading the program into the Arduino (normally only to be done once) using a USB to serial adapter.
*              You must avoid having both GPS and USB connected at the same time because while uploading a program in Arduino, there will be conflicts between the signals sent by the 2 devices and programming will fail.
*              There is another risk: if your USB to serial adapter generates 5 volts signal, it could damage the GPS module. 
*              So, when you connect the usb to serial adapter to the Arduino, you should disconnect at least the GPS TX pin from the Arduino Rx pin.  
*              Personnaly I use a connector between Arduino and GPS module and so I can disconnect totally the GPS module.
* Software points of attention
*    UBLOX GPS module are normally delivered with a default configuration (generating automatically e.g some NMEA messages at 9600 bauds at a refresh rate of 1 hz).
*    oXs assumes that, at start up, GPS is working at 9600 bauds. oXs sent then some commands in order to
*       - disable all NMEA messages
*       - activates some UBX messages
*       - increase frequency of calculation (to 5 Hz instead of 1hz)
*       - set up the baud rate to 38400 instead of 9600.
*    Those parameters are not saved in the GPS (because some GPS modules does not allow it). So, oXs will send those commands at each power on.   
*    If you oXs does not send GPS data, please check that your GPS module has still the default configuration (most important is that it is configured to receive UBX command messages at 9600 bauds). 
*       An easy way to check the GPS configuration is to connect the GPS module to a 3.3 volt FTDI ( or USB to serial adapter) and to use a free software named "u-center". 
*       This software is available on the official web site of UBLOX. More info is easily available on the web. 
*  OXs allows to modify some parameters in the config.h file:
*     -   #define GPS_INSTALLED      : uncomment this line if a GPS is connected and has to transmit his data
*     -   #define GPS_SPEED_IN_KMH   : uncomment this line if GPS speed has to be sent in km/h instead of knot/h 
*     -   #define GPS_SPEED_3D       : uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test)
*     
************************************************************************************************************************ 



**** xx - Reserved for developer. **************************************************************************************
* DEBUG must be activated here when you want to debug one or several functions in some other files.
* You can then select the parts that you want to debug by uncommenting the specifics DEBUG parameters you want in each file
* Note: OXS allows to transmit 3 fields named TEST1, TEST2, TEST3. You can fill those fields with whatever you want where you want if you want to transmit additional data to the Tx.
* Just fill in test1Value (or 2, 3) with an int32_t and test1ValueAvailable (or 2, 3) with true and add those OXS measurements in the data to be sent section. 
************************************************************************************************************************
//#define DEBUG


