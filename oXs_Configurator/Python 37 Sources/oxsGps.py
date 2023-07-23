from oxsConfig import ttk , W , E , fGps , gpsM10, gps3dExist , gpsTimeExist , gpsRateVar

#here start the definition of the GPS tab
#define GPS_SPEED_3D      // uncomment this line if GPS speed has to be the 3d speed instead of the 2d speed (note: 3d is probably less accurate - to test) 
#define GPS_REFRESH_RATE  5       // rate at which GPS sent new data; select between 1, 5 or 10 (Hz). Default = 5 Hz; Ublox NEO6 does not support 10 hz  
ttk.Label(fGps, text="M9/M10" ).grid(column= 0, row=1, padx= (15,5), pady=5 , sticky=(E))
ttk.Checkbutton(fGps,  variable=gpsM10,
	    onvalue='On', offvalue='Off').grid(column=1, row=1, sticky=('W','E') )
ttk.Label(fGps, text="3D speed" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(E))
ttk.Checkbutton(fGps,  variable=gps3dExist,
	    onvalue='On', offvalue='Off').grid(column=1, row=2, sticky=('W','E') )
ttk.Label(fGps, text="GET UTCTIME (S-Port only)" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(E))
ttk.Checkbutton(fGps,  variable=gpsTimeExist,
	    onvalue='On', offvalue='Off').grid(column=1, row=3, sticky=('W','E') )        
ttk.Label(fGps, text="Refresh rate (Hz)").grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(E))
gpsRateBox = ttk.Combobox(fGps, textvariable=gpsRateVar, 
                values=('1', "5" , "10"),
                state="readonly", width=2 )
gpsRateBox.grid(column=1, row=4, sticky=(W) )
