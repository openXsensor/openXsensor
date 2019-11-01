from oxsConfig import ttk , W , E , fFlow


#define PULSES_PER_ML                    10.0                 // number of pulses per milli liter (depends on sensor); can have decimals
#define TANK_CAPACITY                    1000                 // tank capacity in ml
#define INIT_FLOW_PARAM  30 , 100 , 500 , 700 , 0 , 0, 0, 0   // define at 4 levels of flow (in mliter/min) (e.g. 30, 100, 500, 700) 4 correction parameters (in %; e.g. 20, 10, -5, 15); flow levels have to be sorted from low to high
#define FLOW_SENSOR_RESET_AT_PPM         95                   // when absolute value of ppm is greater than this, flow counter is reset.



ttk.Label(fFlow, text="Not yet implemented (so you have to edit manually the config files)" ).grid(column= 0, row=4, padx= 1 , pady= 20)
