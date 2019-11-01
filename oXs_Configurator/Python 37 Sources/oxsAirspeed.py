from oxsConfig import ttk , W , E , Spinbox , fAirspeed , airspeedTypeVar , airspeedAtSea15CVar , airspeedInKmhVar , \
    airspeedResetAtPpmVar , airspeedCompMinAtPpmVar , airspeedCompMaxAtPpmVar , airspeedCompMinVar , airspeedCompMaxVar

#define AIRSPEED_SENSOR_USE       NO_AIRSPEED       // select between NO_AIRSPEED , MS4525 , MPXV7002

#define AIRSPEED_AT_SEA_LEVEL_AND_15C // if this line is commented, airspeed is calculated using baro pressure and temperature (so being "true" airspeed instead of normalised airspeed)      
#define AIRSPEED_IN_KMH  // uncomment this line if airspeed has to be in km/h instead of knot/h ( except some old versions, openTx expects knot/h) 

#define AIRSPEED_RESET_AT_PPM   100

#define COMPENSATION_MIN_AT_PPM 60
#define COMPENSATION_MAX_AT_PPM 90
#define COMPENSATION_PPM_MIN  80
#define COMPENSATION_PPM_MAX 140

ttk.Label(fAirspeed, text="Type of sensor").grid(row=1, sticky=E , pady=(20,2))
airspeedTypeBox = ttk.Combobox(fAirspeed, textvariable=airspeedTypeVar, 
                values=( "MS4525" , "MPXV7002" ),
                state="readonly")
airspeedTypeBox.grid(column=1 , sticky=W , row=1, padx=5)
ttk.Checkbutton(fAirspeed, text='Airspeed at sea level and 15 degree Celsius',  variable=airspeedAtSea15CVar,
	    onvalue='On', offvalue='Off').grid(row=2, sticky=(W), padx=10 , pady=(20,2))
ttk.Checkbutton(fAirspeed, text='Airspeed in km/h (otherwise in knot/h as expected by openTx)',  variable=airspeedInKmhVar,
	    onvalue='On', offvalue='Off').grid(row=3, sticky=(W,E), padx=10 , pady=(20,2))

ttk.Label(fAirspeed, text="Reset airspeed when ppm is (%)").grid(row=4, sticky=(E), pady=(20,2))
airspeedResetAtPpmBox = Spinbox(fAirspeed , from_=0, to=100, width = 5 , textvariable=airspeedResetAtPpmVar ) 
airspeedResetAtPpmBox.grid(column=1, row=4 , sticky=W,  padx=5, pady=(20,2))

ttk.Label(fAirspeed, text="Min Compensation when ppm is (%)").grid(row=5, sticky=(E), pady=(20,2))
airspeedCompMinAtPpmBox = Spinbox(fAirspeed , from_=0, to=100, width = 5 , textvariable=airspeedCompMinAtPpmVar ) 
airspeedCompMinAtPpmBox.grid(column=1, row=5 , sticky=W,  padx=5, pady=(20,2))

ttk.Label(fAirspeed, text="Max Compensation when ppm is (%)").grid(row=6, sticky=(E), pady=(20,2))
airspeedCompMaxAtPpmBox = Spinbox(fAirspeed , from_=0, to=100, width = 5 , textvariable=airspeedCompMaxAtPpmVar ) 
airspeedCompMaxAtPpmBox.grid(column=1, row=6 , sticky=W,  padx=5, pady=(20,2))

ttk.Label(fAirspeed, text="Min Compensation is (%)").grid(row=7, sticky=(E), pady=(20,2))
airspeedCompMinBox = Spinbox(fAirspeed , from_=50, to=100, width = 5 , textvariable=airspeedCompMinVar ) 
airspeedCompMinBox.grid(column=1, row=7 , sticky=W,  padx=5, pady=(20,2))

ttk.Label(fAirspeed, text="Max Compensation is (%)").grid(row=8, sticky=(E), pady=(20,2))
airspeedCompMaxBox = Spinbox(fAirspeed , from_=100, to=200, width = 5 , textvariable=airspeedCompMaxVar ) 
airspeedCompMaxBox.grid(column=1, row=8 , sticky=W,  padx=5, pady=(20,2))
