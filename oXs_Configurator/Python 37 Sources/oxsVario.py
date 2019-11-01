from oxsConfig import ttk , W , E , Spinbox ,  fVario , varioTypeVar , secondBaroExist , varioSourceVar , varioHysteresisVar
from oxsConfig import  analogVarioChanged , fAnalogVario , analogVarioExist , varioAnalogPinVar , varioAnalogMaxVar , varioAnalogMinVar

#here start the definition of the vario tab
ttk.Label(fVario, text="Type of sensor").grid(row=1, pady=(20,2))
varioTypeBox = ttk.Combobox(fVario, textvariable=varioTypeVar, 
                values=('MS5611', "GY86" , "BMP085" , "BMP180" , "GY87",  "BMP280"),
                state="readonly")
#varioBox.bind('<<ComboboxSelected>>', varioChanged)
#protocolBox['state']='readonly'
varioTypeBox.grid(row=2, padx=10)
ttk.Checkbutton(fVario, text='a MS5611 is added as second sensor',  variable=secondBaroExist,
	    onvalue='On', offvalue='Off').grid(row=3, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Label(fVario, text="Source of Vspeed field").grid(row=4, pady=(20,2))
varioSourceBox = ttk.Combobox(fVario, textvariable=varioSourceVar, 
                values=('FIRST_BARO', 'BARO_AND_IMU', 'SECOND_BARO' , 'AVERAGE_FIRST_SECOND', 'AIRSPEED_COMPENSATED' , 'PPM_SELECTION'),
                state="readonly")
#varioBox.bind('<<ComboboxSelected>>', varioChanged)
#protocolBox['state']='readonly'
varioSourceBox.grid(row=5, padx=10)

ttk.Label(fVario, text="Hysteresis" ).grid(column= 0, row=6, padx= 5 , pady=(20,2))
varioHysteresisBox = Spinbox(fVario, from_=0.0, to=10.0, width = 5 , textvariable=varioHysteresisVar)
varioHysteresisBox.grid(column=1, row=6 , padx=10, pady=(20,2))



#def analogVarioChanged():
#    if analogVarioExist.get() == 'On':
#        fAnalogVario.grid(row=8)
#    else:    
#        fAnalogVario.grid_remove()
ttk.Checkbutton(fVario, text='Generate an analog Vspeed signal',  command= analogVarioChanged, variable=analogVarioExist,
	    onvalue='On', offvalue='Off').grid(row=7, sticky=(W,E), padx=20 , pady=(20,2))
ttk.Label(fAnalogVario, text="Pin" ).grid(column= 0, row=0, padx= 5 )

varioAnalogPinBox = ttk.Combobox(fAnalogVario, textvariable=varioAnalogPinVar, 
                values=('3', '11') , width=3)
varioAnalogPinBox.grid(column=1, row=0, padx=10)
ttk.Label(fAnalogVario, text="Min VSpeed" ).grid(column= 0, row=1, padx= 5 )
varioAnalogMinBox = Spinbox(fAnalogVario, from_=-10.0, to=0.0, textvariable=varioAnalogMinVar , 
                        increment='0.5', width=4)
varioAnalogMinBox.grid(column=1, row=1 , padx=10)
ttk.Label(fAnalogVario, text="Max VSpeed" ).grid(column= 0, row=2, padx= 5 )
varioAnalogMaxBox = Spinbox(fAnalogVario, from_=0.0, to=10.0, textvariable=varioAnalogMaxVar, 
                        increment = '0.5', width=4)
varioAnalogMaxBox.grid(column=1, row=2 , padx=10)

