/*
*************************************************************************************
*                                  Important Note:
*  Do not remove or modify this file unless you want to add your own comments.
*  The purpose of this file is only to provide some explanations.
*  The set up has to be done in the oXs_config.h file.
*************************************************************************************

See OpenXsensor https://github.com/openXsensor/
started by Rainer Schlosshan

************************************************************************************************************************
************************************* General explanation of the different options *************************************
************************************************************************************************************************
*  The file oXs_config.h allows the user to set up different options. Here is a summary of the main options.
*
*  1 - Telemetry protocol 
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
*  12 - IMU 6050 (accelerometer/gyro sensor) (optionnal) 
*  xx - Reserved for developer
*
* Note : Active parameters are normally on a line beginning by "#define", followed by the name of the parameter and most of the time a value.
*        To deactivate a parameter, in general, you can add "//" just before "#define"; this line will then be considered as a comment and discarded.
************************************************************************************************************************


**** 1 - telemetry Protocol **********************************************************************
* Currently oXs supports 4 telemetry protocols : Multiplex , Frsky, Jeti and Hott (=Graupner)
* One (and only one) has to be selected; e.g. if you activate the Multiplex, the Frsky, Jeti and Hott are disactivated (and the opposite) 
* FrSky uses 2 different protocols:
*       - SPORT is used for X series receivers (like X8R or X6R)
*       - HUB is used for D series receivers (like D4R-II)
*      oXs can in principe support both Frsky protocols simultanously (using option FRSKY_SPORT_HUB). It can detect automatically the type of receiver it is connected to.
*      This way there is no need to reload the program with another protocol if you want to use the same OXS hardware on another type of Frsky receiver.
*      Still, the autodetection seems not 100% reliable and requires more memory..    
*      In case autodetection is not reliable or if you need more memory (because many sensors are connected to oXs), you can force a specific FRSKY protocol.
*      
*      For the HUB protocol, only one OXS may be connected to the receiver (other sensors are not allowed). 
*      In SPORT protocol, there may be several sensors connected on the same bus (e.g. a GPS) but each sensor must have a different SPORT_SENSOR_ID.
*           For SPORT protocol, oXs uses up to 6 SPORT_SENSOR_ID. The 6 SENSOR_ID used by oXs are :    
*               DATA_ID_VARIO  0x00  // = sensor 0
*               DATA_ID_FLVSS  0xA1  //          1
*               DATA_ID_FAS    0x22  //          2
*               DATA_ID_GPS    0x83  //          3
*               DATA_ID_RPM    0xE4  //          4
*               DATA_ID_ACC    0x1B  //          ?
*  
*  To select the protocol to be used by oXs, fill the line #define PROTOCOL with one of the allowed values: FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT; JETI
************************************************************************************************************************
#define PROTOCOL FRSKY_SPORT    // select between FRSKY_SPORT , FRSKY_HUB , FRSKY_SPORT_HUB , MULTIPLEX , HOTT, JETI

**** 2 - Serial data pin ***********************************************************************************************
*   OXS has to be connected to the receiver in order to transmit his data.
*   Therefore, one DIGITAL Arduino pin has to be connected to the receiver.
*   You must specify the Arduino pin being used.
*   Default: 4 ; allowed values are 2 or 4 but take care not using the same pin for 2 features.
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
*     Still it is possible to use a board with a BMP085 or BMP180 or a BMP280 sensor (which are cheaper but less accurate). Please note that only the first sensor can be a BMP085, BMP180 or BMP280.
*     It is possible to connect 1 or 2  baro sensors; the first sensor can be a BMP085 or BMP180 or a MS5611; the second one must be a MS5611 (because BMPxxx have only one I2C address)
*         Note : when using an Arduino with 5 Volt Vcc, select always a baro sensor board which have a 3.3 voltage regulator and a I2C level shifter.
*     Each sensor can calculate the absolute/relative altitude (in meter with 1 decimal) and the vertical speed (in meter/sec with 2 decimals).
*     A second sensor can be useful because if it is associated with a TEK probe and if option PPM is used, it is possible to select from the TX the sensor that will generate the vario tones.
*     This allows to switch e.g. between a pneumatic compensated and an uncompensated vario.
*     Using 2 baro sensors can also be an option to consider if you want e.g. a reduced reaction time of the vario for the same noise level.
*        Note: to get a faster reaction time, sensitivity has to be increased (e.g. using 80 instead of 50 as SENSITIVITY_MIN)
*     When 2 baro sensors are used, oXs can transmit as vertical speed the average from both sensors.
*     Uncomment the line "#define VARIO" to enable first sensor.
*     Uncomment the line "#define SENSOR_IS_BMP180" if a BMP085 or BMP180 is used as first sensor instead of a MS5611.
*     Uncomment the line "#define SENSOR_IS_BMP280" if a BMP280 is used as first sensor instead of a MS5611.  
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
*     
*     If you want very short reaction time of the vario, the best solution is to use a MPU6050 sensor (accelerometer/gyro) on top of a baro sensor (see below)
*     Note: when a vario is used (#define VARIO being uncommented) , you have to specify (in section 9) how the "main vertical speed" has to be calculated
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

* 4.5 - Switching between different vertical speeds calculations (optional) **************************************************************
*     When you use two baro sensors or one baro sensor + one airspeed sensor (4525D0 - see section 5) or one baro sensor and one IMU, OXS can calculate several vertical speeds (or dTE).
*     When the PPM option is implemented, OXS allows to select from TX which value (Vspeed from first or second baro sensor or compensated by airspeed, ...) has to be sent as vertical speed and so will control the vario tone. 
*     This allows switching between e.g. compensated and uncompensated vario.
*     Even if OXS can calculate up to 5 vertical speeds ( FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU ), it is only possible to switch between 2 predefined vertical speeds.
*     To enable this feature, additional parameters are required:
*       1) Specify what are respectively the primary and the secondary vertical speeds using the lines:
*                  #define VARIO_PRIMARY       XXXXXXX  
*                  #define VARIO_SECONDARY     YYYYYYY
*                 where XXXXXX and YYYYYY are each one values selected between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU 
*       2) Specify a range of PPM value that OXS has to check in order to send or the primary or the secondary vertical speed using the lines:
*                 #define SWITCH_VARIO_MIN_AT_PPM 10 
*                 #define SWITCH_VARIO_MAX_AT_PPM 90 
*             When the ABSOLUTE value of PPM is between SWITCH_VARIO_MIN_AT_PPM (typical value = 10) and SWITCH_VARIO_MAX_AT_PPM (typical value = 90),
*              - OXS will switch to the primary vertical speed IF  PPM is POSITIVE
*              - OXS will switch to the secondary vertical speed IF  PPM is NEGATIVE
*            Note: when the absolute value of PPM is outside this range, OXS will not switch from vertical speed and keep the current one.
*                  This principle allows to use a switch on TX simultaneously with a pot in order to control sensitivity or compensation.
*                  Switching from positive to negative can be achieved on openTx with a mixer using MULTIPLY by -100%.
*                  Sending a PPM value outside this range allows to instruct OXS to apply another order (e.g. reset the airspeed offset) without switching the vertical speed.
*       3) Specify in section 9 (data to transmit) that the source for vertical speed is "PPM_SELECTION"             
****************************************************************************************************************************
#define VARIO_PRIMARY       AIRSPEED_COMPENSATED  // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
#define VARIO_SECONDARY     FIRST_BARO            // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU
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
*     Glider ratio requires an airspeed sensor and a vario; 
*     Averaged sink/climb rate can be calculated even without an airspeed (but in this case tolerance on airspeed is discarded)
*     In order to activate the calculations, you must uncomment the 4 lines of parameters    
*     In order to deactivate the calculation, it is enough to put the line GLIDER_RATIO_CALCULATED_AFTER_X_SEC as comment
*************************************************************************************************************************
#define GLIDER_RATIO_CALCULATED_AFTER_X_SEC       10        // minimum elapsed time (in sec) to transmit calculations
#define SPEED_TOLERANCE              5                      // in % of speed
#define VSPEED_MIN_TOLERANCE -200                           // out of tolerance when Vspeed is lower than this value (cm/sec)
#define VSPEED_MAX_TOLERANCE  -10                           // out of tolerance when Vspeed is upper than this value (cm/sec)


************************************************************************************************************************
* Note : it is not required to comment the sensitivity, hysteresis, OutputClimbRateMin/Max, ... parameters when a vario,
*        is not used (those parameters are just discarded)
*        The vario setup parameters (sensitivity, hysteresis, PPM, ...) are shared by both baro sensors
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
* oXs can calculate 2 types of airspeed:
*  - the first one is an airspeed based on air density at 15 Celcius degree and 1013 hPa (normalised sea level). It is a normalised airspeed (indicated airspeed) as normally used on aircraft.
*    So e.g. the stall speed does not change with altitude
*  - the second one take into account the pressure (provided by a baro sensor) and the initial airspeed sensor temperature in order to calculate a "true" airspeed (to be compared with a GPS speed when wind is null)  
* The normalised airspeed is calculated when line #define AIRSPEED_AT_SEA_LEVEL_AND_15C is uncommented. To get the "true" airspeed, put this line as comment
* 
* oXs can send the airspeed in (1/10) of knot/h or in (1/10) km/h. For openTx, you normally must use the knot/h option (only some old 2.1.x versions requires km/h option).
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
#define AIRSPEED_AT_SEA_LEVEL_AND_15C // if this line is commented, airspeed is calculated using baro pressure and temperature (so being "true" airspeed instead of normalised airspeed)      
//#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h ( except some old versions, openTx expects knot/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 120


**** 6 - Voltage measurements and current sensor settings (optional) ***************************************************
*
* 6.1 - Voltage Reference to measure voltages and current **************************************************************
*     Current and Voltages can be measured in (1023) steps from or VCC , from an 1.1 internal reference or from an external reference.
*     If VCC is very stable, it is probably more accurate and easier to measure current and voltages based on VCC (so in 1023 steps from VCC).
*     Still this requires that the voltage applied on Arduino "RAW" pin is regulated or at least always more than 5.5 volt (in order to let the Arduino board voltage regulates it at 5 volt).
*     If voltage on "Raw" pin is less than 5.5 volt and changes (e.g. due to servo consumption or low battery) then current and voltage measurements will become wrong.
*     If VCC can't be very stable (e.g. Arduino is powered via RX by a 4.8 NiMh) and you need only the voltages (no need for the current measurement), then it is possible to measure based on the Arduino 1.1 internal voltage reference.
*     If you need current measurement too, using internal voltage is not "the" solution because current sensor needs stable voltage too.
*     Take care that voltage dividers (see below) must be setup in order that Arduino analog pin voltage do not exceed VCC or 1.1 volt or the external voltage depending on the option you choose.
*     - Uncomment the "#define USE_INTERNAL_REFERENCE" to activate the 1.1 internal voltage reference (otherwise, measurements will be based on VCC).
*     - Uncomment the "#define USE_EXTERNAL_REFERENCE" to activate the external voltage reference (otherwise, measurements will be based on VCC).
*        Note: in order to use an external reference, you have to use add a voltage reference device to your Arduino. This is quite difficult on Arduino pro mimi because the AREF pin is not available on the pin headers
*     - In order to get best measurements, you can setup the voltage reference being used with line #define REFERENCE_VOLTAGE.
*       Value must be defined in millivolt (e.g. 5100 for 5.1 Volt) and may not contains decimals;
*       If ommitted, oXs will automatically apply 1100 (when USE_INTERNAL_REFERENCE is defined) and 5000 (othewise)
*       When external reference is used, REFERENCE_VOLTAGE must be specified
************************************************************************************************************************
//#define USE_INTERNAL_REFERENCE
//#define USE_EXTERNAL_REFERENCE  // uncomment this line if you use an external reference instead of Vcc
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
*        Note: a capacitor (e.g. 100nf) could be added too between ground and Arduino Analog pin in order to get more stable values.
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
*     oXs can use some of them to calculate the voltage of some lipo cells.
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
*            If OXS is connected to a lipo having less cells, OXS will automatically reduce the number of cells which still let the TX calculate the total voltage and the lowest cell voltage 
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

***** 6.5 - Ads1115 parameters  *************************************************************************************
*    It is possible to connect an external ADC of the type ads1115. 
*    This device is very cheap (about 2 $) and can provide more accuracy than internal Arduino ADC
*    It has 16 bits resolution, accurate internal voltage reference, a programmable gain amplifier and the ability to measure directly the voltage between 2 pins 
*    See datatsheet of ads1115 for more details
*    oXs can be connected to one of those device via I2C bus. It can then provide up to 4 voltage measurements named ADS_VOLT_1 ...ADS_VOLT_4
*    oXs allows to convert one ads voltage measurement into current and consumption (when ads1115 is connected to a current sensor) 
*    oXs also allows to convert one ads voltage measurement into airspeed and compensated vario (when ads1115 is connected to a differential baro sensor like the MPXV7002) 
*          MPXV7002 sensor is a cheapier and easier to find alternative to the MS4525 sensor (but is less accurate)
*    In order to use an ads1115 ADC, you must uncomment ACD_MEASURE and specify several sets of 4 parameters. 
*    Attention : let line ADC_MEASURE as comment if ads1115 is not used (in order to avoid I2C errors and slow down oXs)
*        In ACD_MEASURE, you specify the ads115 pins being used for voltage measurement
*              Fill always 4 values. Still, if you do not need all 4 measurements, use the value ADS_OFF for the unrequired measurement(s)
*              Note : the more measurements you ask for, the more time it require to get each of them because ads can only perfom one at a time
*              Select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
*        In ADC_FULL_SCALE_VOLT, you specify the ads1115 gain parameter for each of the 4 measurements.
*              Fill always 4 values even if you do not need all 4 measurements
*              This allows to amplify a low voltage applied on input pins before it is converted by the ADC. So accuracy of conversion is optimum. 
*              Select between MV6144 MV4096 MV2048 MV1024 MV512 MV256 where the digits give the max mvolt being applied on the pin (e.g. for A0_TO_GND) or between 2 pins (e.g. for A0_TO_A1)
*        In ADS_SCALE, you specify a scaling factor to be applied on each measurement in order to get a value that conforms the expected measurement.
*              Fill always 4 values even if you do not need all 4 measurements
*              When scaling parameter = 1, oXs returns a value = 1 when the voltage applied on ads1115 pin(s) is the max voltage set by ADC_FULL_SCALE_VOLT. 
*              So, if ADC_FULL_SCALE_VOLT is set on MV512, when input voltage will be 512mv (or more)), oXs will return 1 if ADS_SCALE = 1.
*              If you do not have a voltage divider on ads1115, you probably expect that oXs returns 512; then set ADS_SCALE to 512.
*              If you have a voltage divider, the voltage that you want to measure is greater than the voltage applied on ads1115 pin and you have to increase ADS_SCALE.
*              e.g. if your voltage divider divides your voltage by 10, you must set ADS_SCALE to 5120 (= 512 * 10)
*              Note: ADS_SCALE can have decimals (e.g. 100.5) . It can be positive or negative; It can't be 0
*        In ADS_OFFSET, you specify an offset to be applied. 
*              Fill always 4 values even if you do not need all 4 measurements
*              When no offset has to be applied, set the value on 0
*              The specified offset is added to the value calculated after scaling.  
*              each value must be an integer (positive or negative); it can be 0
*        In ADS_RATE, you specify the number of milli sec that ads1115 takes to convert a voltage.      
*              Fill always 4 values even if you do not need all 4 measurements
*              Using a high value reduces power consumption still, it reduces the number of measurements that can be performed/transmitted per second    
*              Select values between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2;  the digits correspond to the number of milli sec (e.g. MS18 means 18 msec)
*              Note : oXs will wait at least the specified delay but it can be that the delay is higher due to other tasks to be performed by oXs
*        In ADS_AVERAGING_ON, you specify the number of voltages to be collected in order to calculate an average. 
*              Fill always 4 values even if you do not need all 4 measurements. If you do not want averaging, set the value on 1
*              It must must be an integer, positive and different from 0.
*              Note : Increasing the value is a way to reduce noise. Still it increase the delay between 2 transmissions 
*        In ADS_CURRENT_BASED_ON, you specify which voltage measurement (if any) is used for current (and current consumption)
*              Uncomment this line only when a conversion is requested 
*              Keep this line as comment if there is no current sensor or if the wurrent sensor is connected to an Arduino pin as explained in section 6.4
*              Fill only one value; select a value between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : when oXs calculate a curent based on a voltage, it used also 2 parameters from section 6.4 : MVOLT_AT_ZERO_AMP and MVOLT_PER_AMP
*        In ADS_AIRSPEED_BASED_ON, you specify which voltage measurement (if any) is used for airspeed
*              Uncomment this line only when an analog airspeed sensor is connected to ads1115 
*              Keep this line as comment if there is no airspeed sensor connected to ads1115
*              Fill only one value; select a value between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*              Note : A typical airspeed sensor is the MPXV7002DP which is easaly available on ebay or aliexpress.com.
*                     When this sensor is used, you have to configure ads1115 in following way:
*                     - ask only for one ads1115 measurement and it must be a differential voltage : so use this setup : ADS_MEASURE A0_to_A1, ADS_OFF, ADS_OFF, ADS_OFF
*                     - connect one resistor of about 10kohm between ads1115 Vcc and ads1115 A1 and another one of the same value between ads1115 Ground and ads1115 A1; so A1 pin get Vcc/2 volt
*                     - set the ads1115 gain in order to get 2048 mvolt at full scale:  so use : ADC_FULL_SCALE_VOLT MV2048,MV2048,MV2048,MV2048  
*                     - set the ads rate in order to make ADC conversion as fast as possible; so use : ADS_RATE MS2, MS2, MS2, MS2
*                     - the other parameters are not critical (not used for airspeed)
************************************************************************************************************************
#define ADS_MEASURE A1_TO_GND ,   ADS_OFF , ADS_OFF , ADS_OFF // uncomment when ADS1115 is used; select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV4096, MV4096, MV4096, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 2, 10, 1, 1 // can be a float
#define ADS_RATE  MS137 , MS5, MS3 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 1 , 10, 50, 50 // number of values used for averaging (must be between 1 and 254) 
#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4

**** 7 - RPM (rotations per minute) sensor settings (optional) ***************************************************************************
*      It is possible to measure RPM using a sensor connected to pin ICP (=PB0, = pin 8) of OXS.
*      This sensor must provide a level change (0 - Vcc) on this pin each time a blade passes in front of it.
*      The number of blades is an important parameter to set up but this is done on Tx side.
*      To activate this function, uncomment the #define MEASURE_RPM line below.
*      Note: The digital pin 8 (PB0/ICP) is the only one to be used to measure RPM.
*            The value calculated by oXs is in Hertz (and not in roration per minute)
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
*   Depending on the parameters in the config.h file (and the sensors connected to oXs), oXs calculates several measurements 
*   In Multiplex protocol, you have to specify whch measurements are sent (and on which line on the display) (see below) 
*   In SPORT, HUB , JETI and HOTT protocol, main measurements are automatically transmitted in their expected fields and some set up allows to transmit some extra measurements reusing available fields .
*   One of the calculated measurement is named "main vertical speed". 
*   Depending on content of line #define VSPEED_SOURCE, it is calculated based on 
*      - first baro sensor
*      - second baro sensor
*      - average of both baro sensors
*      - first baro with compensation from airspeed (= dtE)
*      - first baro + IMU (accelerometer+ gyro)
*      - primary or secondary vertical speed selected by PPM 
*    ex:  
*      #define VSPEED_SOURCE  PPM_SELECTION       // select between FIRST_BARO, SECOND_BARO , AVERAGE_FIRST_SECOND, AIRSPEED_COMPENSATED , BARO_AND_IMU or PPM_SELECTION
**** 9.1 FrSKY protocol (SPORT and HUB ) ************************************************************************************ 
*      Measurements being automatically transmitted are : 
*         - Relative altitude(cm), main vertical Speed(cm/sec) (when at least first baro sensor is present)
*         - Cell voltages (when NUMBER_OF_CELLS > 0) 
*         - Current (when Current sensor is present)
*         - GPS (long, lat, speed, altitude , course) (when GPS sensor is present)
*         - RPM (hz) (when RPM sensor is present)
*      On top of this, you can also specify how to fill telemetry fields Vfas, Fuel, A3, A4, T1, T2, AccX, AccY, AccZ (see available options below)
*      Put those line(s) as comment when no measurement has to be transmitted in those telemetry field(s)
*    ex :  
*      #define VFAS_SOURCE VOLT_4                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define FUEL_SOURCE VOLT_4                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define A3_SOURCE VOLT_4                   // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define A4_SOURCE VOLT_4                   // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
*      #define ACCX_SOURCE TEST_1                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define ACCY_SOURCE TEST_2                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define ACCZ_SOURCE TEST_3                 //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, PITCH, ROLL, YAW, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define T1_SOURCE GLIDER_RATIO             //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
*      #define T2_SOURCE SENSITIVITY              //  select between TEST_1, TEST_2, TEST_3, GLIDER_RATIO , SECONDS_SINCE_T0 ,AVERAGE_VSPEED_SINCE_TO , SENSITIVITY , PPM, VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4 
**** 9.2 Hott protocol ************************************************************************************ 
*      Measurements being automatically transmitted are : 
*         - Relative altitude(cm), main vertical Speed(cm/sec) (when at least first baro sensor is present)
*         - Cell voltages, lowest cell voltage (when NUMBER_OF_CELLS > 0) 
*         - up to 3 battery voltages (set up has to specify which voltage measurements are transmitted - E.g. VOLT_1,VOLT_2, ...)  
*         - Current (when Current sensor is present)
*         - GPS (long, lat, speed, altitude , course, distance from home, direction from home, number of sat, type of fix) (when GPS sensor is present)
*         - RPM (when RPM sensor is present)
*      When you measure cell Voltages, you can get an alarm on Tx when lowest cell voltage is lower than a specified level.   
*      To activate this alarm, uncomment next line and specify the voltage (in milliVolt ) that trigger the alarm
*         //#define CELL_UNDERVOLTAGE_WARNING 3300            // Warning threshold in mV;
*         
*      On top of this, you can also specify how to fill telemetry fields temperature1 and temperature2 
*      note : those 2 fields can only report a value from -20 up to 235; for PPM, a value of -100 will be displayed as 0 and +100 will be displayed as 200)      
*      Put those line(s) as comment when no measurement has to be transmitted in those telemetry field(s)      
*     ex:  
*       #define BATTERY_1_SOURCE          VOLT_4                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define BATTERY_2_SOURCE          VOLT_2                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define MAIN_BATTERY_SOURCE       VOLT_5                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
*       #define TEMPERATURE_1_SOURCE      TEST_1          //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
*       #define TEMPERATURE_2_SOURCE      SENSITIVITY          //  select between TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
**** 9.3 Multiplex  ************************************************************************************ 
*      For Multiplex protocol, you have to define which OXS measurements are sent to Tx and on which lines they appear on the Tx telemetry panels.
*      You have also to specify if some scaling have to be applied by OXS and you can specify a range of value in order to set alarms on/off.
*      So, you MUST specify ONE ROW for EACH OXS measurement to transmit to Tx. Each row must contains:
*        -  1 : the line number where the measurement has to be appear on the display. Valid number are from 2 to 15, do not use twice the same line number
*        -  2 : a comma
*        -  3 : the OXS measurement name to transmit on this line (e.g. "VOLT_1")  (see note (2))
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
* Note (2) : A OXS measurement name (e.g. VOLT_1) may not appear on several rows.
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
*     - on line 3, the relative altitude measurement (without alarms)
*     - on line 6, the main vertical speed measurement (with alarms if it exceed 50m/s)
*            #define SETUP_MULTIPLEX_DATA_TO_SEND    \
*                       3 , REL_ALTIMETER , 1 , 1 , 0 , -16384 , 16383,\
*                       6 , VERTICAL_SPEED , 1 , 1 , -500 , 500
*  IMPORTANT : keep always the line "#define SETUP_DATA_TO_SEND    \"  ; do not insert any comment lines after or between the rows used for the set up.
*   
* Here the list of oXs measurements that can be sent
*    Code to be used                       Meaning 
*    ALTIMETER           m      Absolute altitude (from first baro sensor)
*    REL_ALTIMETER       m      Relative altitude (from first baro sensor)  
*    ALTIMETER_MAX       m      Max relative altitude
*    VERTICAL_SPEED     cm/s    Main vertical speed (as set up in VSPEED_SOURCE)
*    SENSITIVITY        none    Vario sensitivity 
*    VOLT_1            0.1V(5)     Value read on first PIN_VOLTAGE     
*    VOLT_2            0.1V(5)     Value read on 2d PIN_VOLTAGE        
*    VOLT_3            0.1V(5)     Value read on 3d PIN_VOLTAGE        
*    VOLT_4            0.1V(5)     Value read on 4th PIN_VOLTAGE       
*    VOLT_5            0.1V(5)     Value read on 5th PIN_VOLTAGE       
*    VOLT_6            0.1V(5)     Value read on 6th PIN_VOLTAGE       
*    CURRENTMA         0.1A        MilliAmp measured by current sensor  
*    MILLIAH           mAh        Consumed milli Amp heure             
*    RPM              Hz         Rotation per min                     
*    AIR_SPEED        0.1 km/h   Air speed   
*    CELL_1             0.1V(5)    Value based on first PIN_VOLTAGE         
*    CELL_2             0.1V(5)    Value based on first and 2d PIN_VOLTAGE  
*    CELL_3             0.1V(5)    Value based on 2d and 3d PIN_VOLTAGE     
*    CELL_4             0.1V(5)    Value based on 3d and 4th PIN_VOLTAGE    
*    CELL_5             0.1V(5)    Value based on 4th and 5th PIN_VOLTAGE   
*    CELL_6             0.1V(5)    Value based on 5th and 6th PIN_VOLTAGE   
*    CELL_MIN           0.1V(5)    Value based on CELL_1 ... CELL_6         
*    CELL_TOT           0.1V(5)    Value based on VOLT1...VOLT6             
*    PPM                         Value set by Tx to control some functions (sensitivity, ...) (range is normally -100 / +100)
*    GPS_COURSE        0.1 deg   Orientation of plane       
*    GPS_SPEED         0.1 km/h  Ground speed (2D or 3D)       
*    GPS_ALTITUDE      m         Absolute Altitude       
*    GPS_DISTANCE      m         Distance from home
*    GPS_BEARING       0.1 deg   Direction from home  
*    TEST_1                      Value used for test
*    TEST_2                      Value used for test
*    TEST_3                      Value used for test
*   (5) Unit depends also on the calibration parameter that are used (e.g. when a voltage is provided by a temperature sensor, unit can be degree)
*       When used in order to measure Cell(s), calibration must ensure that internal unit = milliVolt (and oXS will convert mVolt to 0.1Volt when transmitting over Multiplex protocol.
***** 9.4 - Jeti data ************************************************************************************************************************
*      Measurements being automatically transmitted are : 
*         - Relative altitude(cm), main vertical Speed(cm/sec) (when at least first baro sensor is present) and maximum altitude
*         - Cell voltages including min and total (when NUMBER_OF_CELLS > 0) 
*         - Current and consumption (when Current sensor is present)
*         - GPS (long, lat, speed, altitude , course, distance and direction from home) (when GPS sensor is present)
*      On top of this, you can also specify to send one of the 6 voltage (if voltage is measured)

//#define VOLTAGE_SOURCE   VOLT_1                 // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6

**********************************************************************************************************************************************       
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
*          This resistor is added (at least for an Arduino 5 volt) in order to protect the GPS pin. 
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

**********  12 - IMU based on mpu6050 (accelerometer/gyro sensor) (optionnal) ********************************************************
*  It is possible to connect an IMU sensor (=accelerometer/gyro) to arduino; this is optionnal.
*  It allows :
*      - to reduce the reaction time of the vario by about 0.5 sec (note: a baro sensor has to be connected too because oXs merges the data from both sensors)
*      - to transmit data about accelerations and/or orientation (pitch/roll); in this case it is important that oXs device is mounted in a fix position and is aligned with the plane axis. 
*  Only one IMU sensor is supported : the mpu6050. 
*  This sensor is easily available on ebay, ... under different modules. The best module to use is probably the GY-86 because it has also a voltage regulator (3.3volt), I2C level converters, and a baro sensor (MS5611)     
*  5 pins from the mpu6050 have to be connected to Arduino:
*       - MP6050 ground  <--->  Arduino ground
*       - MP6050 Vcc     <--->  Arduino Vcc
*       - MP6050 SDA     <--->  Arduino SDA = Arduino A4
*       - MP6050 SCL     <--->  Arduino SCL = Arduino A5
*       - MP6050 INT     <--->  Arduino INT0 = Arduino 2 OR Arduino INT1 = Arduino 3(do not use the same pin for another purpose like PPM!)
* In order to activate the IMU, uncomment the line #define USE_6050       
* When IMU is activated, this version of oXs calculates a vertical speed in a different way merging the altitude from baro sensor with vertical acceleration (in Earth reference).
* This other type of vertical speed is available in the field "VERTICAL_SPEED_I". In Frsky protocol, it is possible to transmit it e.g. as Vspd.  
* It is also possible to assign it in  "VARIO_PRIMARY" or "VARIO_SECONDARY"  and so to switch between 2 vario sources from the Tx (using a ppm channel) 
* In order to get best results from IMU, it is required to calibrate the accelerometer offsets. To do so, please :
*    - upload a version of oXs firmware whith the line #define DISPLAY_ACC_OFFSET uncommented
*    - let oXs runs while connected to the PC (via USB serial interface = FTDI)
*    - open Arduino IDE terminal (press CTRL + SHIFT + M simultaniously)
*    - take care to set the baud rate to 115200 (or 38400 if GPS is activated too)
*    - after startup, terminal should, every 2 or 3 sec, display Acc followed by 3 numbers being respectively AccX, AccY and AccZ. Please note that those numbers change when mpu6050 moves.
*    - ensure that the mpu6050 (GY86) is perfectly horizontal and does not move (e.g. put on a table) 
*    - notice the 2 first numbers ( = AccX and AccY ) ; Don't take care of the 3rd number because when the sensor is in this position, it will reflect the gravity and will be around 16384. 
*    - rotate mpu6050 in order to get X or Y axis perfectly vertical and do not move. Now, the 3rd number would become much lower (because it does not measure gravity anymore)
*    - notice the 3rd number ( = Accz )
*    - update oXs_config.h file filling the 3 numbers in lines #define ACC_OFFSET_X , #define ACC_OFFSET_Y and #define ACC_OFFSET_Z
*    - set line #define DISPLAY_ACC_OFFSET as comment (adding "//" in front)
*    - upload again oXs firmware in arduino
************************************************************************************************************************ 
 //#define USE_6050
#define PIN_INT_6050 3    // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM) 
#define ACC_OFFSET_X 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)
#define DISPLAY_ACC_OFFSET

 
**** xx - Reserved for developer. **************************************************************************************
* DEBUG must be activated here when you want to debug one or several functions in some other files.
* Enabling DEBUG will allow to use Arduino Serial Monitor at 115200 baud (or 38600 when GPS is activated) to see init data and some live sensor values
* You can then select the parts that you want to debug by uncommenting the specifics DEBUG parameters you want in each file
* Note: OXS allows to transmit 3 fields named TEST1, TEST2, TEST3. You can fill those fields with whatever you want where you want if you want to transmit additional data to the Tx.
* Just fill in test1.value (or 2, 3) with an int32_t and test1.available (or 2, 3) with true and add those OXS measurements in the data to be sent section. 
************************************************************************************************************************
//#define DEBUG


