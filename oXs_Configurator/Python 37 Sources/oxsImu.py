from oxsConfig import ttk , W , E , fImu, imuPinVar, imuDisplayOffset,imuOffsetXVar,imuOffsetYVar,imuOffsetZVar


#define PIN_INT_6050 2   // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM)

#//#define DISPLAY_ACC_OFFSET // used ONLY in order to display the acceleration offset on pc terminal; KEEP AS COMMENT once offsets have been setup 

#define ACC_OFFSET_X 0 // fill here the first value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the second value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the third value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)

fImu.grid(column= 1 , row=8) #by default make it visible

ttk.Label(fImu, text="Pin" ).grid(column= 0, row=0, padx= (15,5) , pady=10 , sticky=(W))
ttk.Label(fImu, text="Offset X" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fImu, text="Offset Y" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fImu, text="Offset Z" ).grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(W))
ttk.Combobox(fImu, textvariable=imuPinVar, values=('2', '3') , state="readonly" ,width=2).grid(column=1, row=0, sticky=(W), padx=10 , pady=10)

ttk.Checkbutton(fImu, text='Display the acceleration offset on pc terminal, turn off after IMU has been set',  variable=imuDisplayOffset,
	    onvalue='On', offvalue='Off').grid(column=0, row=1, columnspan = 8, sticky=(W,E), padx=15 , pady=(10,2))
ttk.Entry(fImu, textvariable=imuOffsetXVar , width='5').grid(column=1 , row=2)

ttk.Entry(fImu, textvariable=imuOffsetYVar , width='5').grid(column=1 , row=3)

ttk.Entry(fImu, textvariable=imuOffsetZVar , width='5').grid(column=1 , row=4)