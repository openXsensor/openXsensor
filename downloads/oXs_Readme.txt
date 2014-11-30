Requirements:

You need to have the Arduino IDE installed:

  Go to http://arduino.cc/en/Main/Software

--------------------------------------------------------------------------------------------------

Installation:

Put the folder "openXsensor" in your arduino project directory.

--------------------------------------------------------------------------------------------------

Change log:

v2.5
- Renaming of different files

v2.4
- Vfas value x10 (Sport).
- Data sent scaling (Hub).
- Descriptions updated.

v2.3
- Bug fix in micros().
- Update config.h.

v2.2
- Allow to transmit Airspeed in Gps speed for hub protocol.
- Sent GPS long, lat with Speed in Hub protocol.
- Support PPM on 8mzh arduino.
- Update config description.

v2.1
- Bug fix (when only 1 MS5611 is available)

v2.0
- This version supports the 2 Frsky (SPORT and HUB) protocols simultanously.
  So you can use the same OXS for both receivers without having to reload another firmware.
  Please note still that some telemetry fields does not exist in the hub protocol and will then be discarded.

- OXS can now support a second baro sensor MS5611. One can measure, as usual, the altitude and the uncompensated vertical speed, the other can be
  a (pneumatically) compensated vario if connected to a TEK probe.

- OXS can now provide the airspeed (min 10 km/h, max = 360 km/h) when a sensor 4525DO-DS5AI001DP and a Prandtl probe (often wrongly named a Pitot
  probe) are added. The sensor 4525DO connects in parallel with the MS5611 sensor(s) using 4 wires.  Note: it is not possible to get accurate
  measurement at low airspeed; therefore OXS will send a value equal 0 when the airspeed is less than about 10 km/h.

- When both 4525DO and MS5611 sensors are used, OXS can also calculate "Prandtl_dTE" (= delta Total Energy based on Prandtl probe) providing an
  (electronically) compensated vario without a TEK probe.

- There are new parameters about PPM in order to control OXS from the Tx.
  It is now possible to calibrate ppm, to adjust the vario sensitivity and the compensation factor for the electronically compensated vario, to reset
  the airspeed, to select between 2 measurements which one is sent as vertical speed (useful because openTx does not yet allow to select it on Tx level).

- In this oxs_config.h, all parameters related to an option can be omitted (or commented using "//) when not used.
  E.g. it is not required any more to set the pin to a dummy value "8" to declare that a voltage does not have to be measured.

- The names of all other parameters have been put in upper case (for standardisation)

- The names used to specify in which telemetry field an OXS measurement has to be sent, are now the names used by OpenTx in the telemetry panel
  (e.g.  for the altitude, the name is "Alt"). Therefore there is no need any more to change the name depending of the type of receiver being used.

- The code has been clean up (e.g. general functions have been splitted from Frsky protocols functions; so, it will be easier to implement other protocols.

- This version have 3 predefined fields (TEST1, TES2, TEST3) that can be transmitted to Tx with whatever you want. This can be useful to develop you
  own version (e.g. sending the MS5611 temperature).

- All the explanations contained in the oxs_config.h file have moved to a new file named oxs_config_description.h