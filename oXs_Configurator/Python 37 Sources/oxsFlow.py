from oxsConfig import ttk , W , E , fFlow, flowPulsesmlVar, flowTankCapVar, flowPPMResetVar
from oxsConfig import flowL1ValueVar, flowL1CorrVar, flowL2ValueVar, flowL2CorrVar, flowL3ValueVar, flowL3CorrVar, flowL4ValueVar, flowL4CorrVar


#define PULSES_PER_ML                    10.0                 // number of pulses per milli liter (depends on sensor); can have decimals
#define TANK_CAPACITY                    1000                 // tank capacity in ml
#define INIT_FLOW_PARAM  30 , 100 , 500 , 700 , 0 , 0, 0, 0   // define at 4 levels of flow (in mliter/min) (e.g. 30, 100, 500, 700) 4 correction parameters (in %; e.g. 20, 10, -5, 15); flow levels have to be sorted from low to high
#define FLOW_SENSOR_RESET_AT_PPM         95                   // when absolute value of ppm is greater than this, flow counter is reset.

ttk.Label(fFlow, text="Pulses per ml" ).grid(column= 0, row=1, padx= (15,5), pady=25 , sticky=(E))
ttk.Label(fFlow, text="Tank capacity (ml)" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Level 1" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Correction 1" ).grid(column= 2, row=3, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Level 2" ).grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Correction 2" ).grid(column= 2, row=4, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Level 3" ).grid(column= 0, row=5, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Correction 3" ).grid(column= 2, row=5, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Level 4" ).grid(column= 0, row=6, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="Correction 4" ).grid(column= 2, row=6, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fFlow, text="PPM Reset level" ).grid(column= 0, row=7, padx= (15,5), pady=5 , sticky=(E))

ttk.Entry(fFlow, textvariable=flowPulsesmlVar , width='5', justify='right').grid(column=1 , row=1)
ttk.Entry(fFlow, textvariable=flowTankCapVar , width='5', justify='right').grid(column=1 , row=2)
ttk.Entry(fFlow, textvariable=flowL1ValueVar , width='5', justify='right').grid(column=1 , row=3)
ttk.Entry(fFlow, textvariable=flowL1CorrVar , width='5', justify='right').grid(column=3 , row=3)
ttk.Entry(fFlow, textvariable=flowL2ValueVar , width='5', justify='right').grid(column=1 , row=4)
ttk.Entry(fFlow, textvariable=flowL2CorrVar , width='5', justify='right').grid(column=3 , row=4)
ttk.Entry(fFlow, textvariable=flowL3ValueVar , width='5', justify='right').grid(column=1 , row=5)
ttk.Entry(fFlow, textvariable=flowL3CorrVar , width='5', justify='right').grid(column=3 , row=5)
ttk.Entry(fFlow, textvariable=flowL4ValueVar , width='5', justify='right').grid(column=1 , row=6)
ttk.Entry(fFlow, textvariable=flowL4CorrVar , width='5', justify='right').grid(column=3 , row=6)
ttk.Entry(fFlow, textvariable=flowPPMResetVar , width='5', justify='right').grid(column=1 , row=7)