from oxsConfig import ttk , W , E ,fVoltage  , volt1Exist , volt2Exist , volt3Exist ,volt4Exist , volt5Exist , volt6Exist ,\
    volt1PinVar , volt2PinVar , volt3PinVar , volt4PinVar , volt5PinVar , volt6PinVar , \
    volt1RgVar , volt2RgVar , volt3RgVar , volt4RgVar , volt5RgVar , volt6RgVar ,\
    volt1RvVar , volt2RvVar , volt3RvVar , volt4RvVar , volt5RvVar , volt6RvVar ,\
    volt1ScaleVar , volt2ScaleVar , volt3ScaleVar , volt4ScaleVar , volt5ScaleVar , volt6ScaleVar ,\
    volt1OffsetVar , volt2OffsetVar , volt3OffsetVar , volt4OffsetVar , volt5OffsetVar , volt6OffsetVar , \
    voltNumberOfCellsVar , voltReferenceVar , voltRefValueVar 

from oxsCurrent import voltRefValueCurrentBox , voltRefValueCurrentLabel 

# here start the definition of the Voltages tab
ttk.Label(fVoltage, text="Volt 1" ).grid(column= 1, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Volt 2" ).grid(column= 2, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Volt 3" ).grid(column= 3, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Volt 4" ).grid(column= 4, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Volt 5" ).grid(column= 5, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Volt 6" ).grid(column= 6, row=2, padx= 5, pady=5 )
ttk.Label(fVoltage, text="Actif" ).grid(column= 0, row=3, padx= 5 )
ttk.Label(fVoltage, text="Analog pin" ).grid(column= 0, row=4, padx= 5 )
ttk.Label(fVoltage, text="R to ground (0=No)" ).grid(column= 0, row=5, padx= 5 )
ttk.Label(fVoltage, text="R to voltage (0=No)" ).grid(column= 0, row=6, padx= 5 )
ttk.Label(fVoltage, text="Scale" ).grid(column= 0, row=7, padx= 5 )
ttk.Label(fVoltage, text="Offset" ).grid(column= 0, row=8, padx= 5 )

ttk.Checkbutton(fVoltage,  variable=volt1Exist,
	    onvalue='On', offvalue='Off').grid(column=1, row=3, sticky=(W,E), padx=20 )
ttk.Checkbutton(fVoltage,  variable=volt2Exist,
	    onvalue='On', offvalue='Off').grid(column=2, row=3, sticky=(W,E), padx=20 )
ttk.Checkbutton(fVoltage,  variable=volt3Exist,
	    onvalue='On', offvalue='Off').grid(column=3, row=3, sticky=(W,E), padx=20 )
ttk.Checkbutton(fVoltage,  variable=volt4Exist,
	    onvalue='On', offvalue='Off').grid(column=4, row=3, sticky=(W,E), padx=20 )
ttk.Checkbutton(fVoltage,  variable=volt5Exist,
	    onvalue='On', offvalue='Off').grid(column=5, row=3, sticky=(W,E), padx=20 )
ttk.Checkbutton(fVoltage,  variable=volt6Exist,
	    onvalue='On', offvalue='Off').grid(column=6, row=3, sticky=(W,E), padx=20 )
#command= volt1Changed,

volt1PinBox = ttk.Combobox(fVoltage, textvariable=volt1PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt1PinBox.grid(column=1, row=4 , padx=10)
volt2PinBox = ttk.Combobox(fVoltage, textvariable=volt2PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt2PinBox.grid(column=2, row=4 , padx=10)
volt3PinBox = ttk.Combobox(fVoltage, textvariable=volt3PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt3PinBox.grid(column=3, row=4 , padx=10)
volt4PinBox = ttk.Combobox(fVoltage, textvariable=volt4PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt4PinBox.grid(column=4, row=4 , padx=10)
volt5PinBox = ttk.Combobox(fVoltage, textvariable=volt5PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt5PinBox.grid(column=5, row=4 , padx=10)
volt6PinBox = ttk.Combobox(fVoltage, textvariable=volt6PinVar , 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"), state="readonly", width=1)
volt6PinBox.grid(column=6, row=4 , padx=10)

ttk.Entry(fVoltage, textvariable=volt1RgVar , width='5').grid(column=1, row=5 , padx=1)
ttk.Entry(fVoltage, textvariable=volt2RgVar , width='5').grid(column=2, row=5 , padx=1)
ttk.Entry(fVoltage, textvariable=volt3RgVar , width='5').grid(column=3, row=5 , padx=1)
ttk.Entry(fVoltage, textvariable=volt4RgVar , width='5').grid(column=4, row=5 , padx=1)
ttk.Entry(fVoltage, textvariable=volt5RgVar , width='5').grid(column=5, row=5 , padx=1)
ttk.Entry(fVoltage, textvariable=volt6RgVar , width='5').grid(column=6, row=5 , padx=1)

ttk.Entry(fVoltage, textvariable=volt1RvVar , width='5').grid(column=1, row=6 , padx=1)
ttk.Entry(fVoltage, textvariable=volt2RvVar , width='5').grid(column=2, row=6 , padx=1)
ttk.Entry(fVoltage, textvariable=volt3RvVar , width='5').grid(column=3, row=6 , padx=1)
ttk.Entry(fVoltage, textvariable=volt4RvVar , width='5').grid(column=4, row=6 , padx=1)
ttk.Entry(fVoltage, textvariable=volt5RvVar , width='5').grid(column=5, row=6 , padx=1)
ttk.Entry(fVoltage, textvariable=volt6RvVar , width='5').grid(column=6, row=6 , padx=1)

ttk.Entry(fVoltage, textvariable=volt1ScaleVar , width='5').grid(column=1, row=7 , padx=1)
ttk.Entry(fVoltage, textvariable=volt2ScaleVar , width='5').grid(column=2, row=7 , padx=1)
ttk.Entry(fVoltage, textvariable=volt3ScaleVar , width='5').grid(column=3, row=7 , padx=1)
ttk.Entry(fVoltage, textvariable=volt4ScaleVar , width='5').grid(column=4, row=7 , padx=1)
ttk.Entry(fVoltage, textvariable=volt5ScaleVar , width='5').grid(column=5, row=7 , padx=1)
ttk.Entry(fVoltage, textvariable=volt6ScaleVar , width='5').grid(column=6, row=7 , padx=1)

ttk.Entry(fVoltage, textvariable=volt1OffsetVar , width='5').grid(column=1, row=8 , padx=1)
ttk.Entry(fVoltage, textvariable=volt2OffsetVar , width='5').grid(column=2, row=8 , padx=1)
ttk.Entry(fVoltage, textvariable=volt3OffsetVar , width='5').grid(column=3, row=8 , padx=1)
ttk.Entry(fVoltage, textvariable=volt4OffsetVar , width='5').grid(column=4, row=8 , padx=1)
ttk.Entry(fVoltage, textvariable=volt5OffsetVar , width='5').grid(column=5, row=8 , padx=1)
ttk.Entry(fVoltage, textvariable=volt6OffsetVar , width='5').grid(column=6, row=8 , padx=1)

ttk.Label(fVoltage, text="Number of Lipo Cells").grid(column=0, row=9, pady=(20,20))
ttk.Combobox(fVoltage, textvariable=voltNumberOfCellsVar, 
                values=('0', "1" , "2" , "3" , "4",  "5" , "6"),
                state="readonly", width=1).grid(column=1, row=9, pady=(20,20))

def voltReferenceChanged():
    if voltReferenceVar.get()=='Vcc' or voltReferenceVar.get()=='External':
        voltRefValueVoltageBox['state']='normal'
        voltRefValueCurrentBox['state']='normal'
        voltRefValueVoltageLabel['state']='normal'
        voltRefValueCurrentLabel['state']='normal'
    else:
        voltRefValueVoltageBox['state']='disabled'
        voltRefValueCurrentBox['state']='disabled'
        voltRefValueVoltageLabel['state']='disabled'
        voltRefValueCurrentLabel['state']='disabled'

def voltReferenceChangedInCurrent():
    if voltReferenceVar.get()=='Vcc' or voltReferenceVar.get()=='External':
        voltRefValueVoltageBox['state']='normal'
        voltRefValueCurrentBox['state']='normal'
        voltRefValueVoltageLabel['state']='normal'
        voltRefValueCurrentLabel['state']='normal'
    else:
        voltRefValueVoltageBox['state']='disabled'
        voltRefValueCurrentBox['state']='disabled'
        voltRefValueVoltageLabel['state']='disabled'
        voltRefValueCurrentLabel['state']='disabled'        

ttk.Label(fVoltage, text="Reference for voltage measurements"
    ).grid(column= 0, row=10, padx= 5, pady=(30,2), columnspan= '5' )
ttk.Radiobutton(fVoltage,  variable=voltReferenceVar, text='Internal' , value='Internal',
    command=voltReferenceChanged).grid(column=0, row=11, sticky=(W,E), padx=20 , pady=2 , columnspan= 5)
ttk.Radiobutton(fVoltage,  variable=voltReferenceVar, text='External' , value='External',
     command=voltReferenceChanged   ).grid(column=0, row=12, sticky=(W,E), padx=20, pady=2 , columnspan= 5)
ttk.Radiobutton(fVoltage,  variable=voltReferenceVar, text='Vcc' , value='Vcc',
	command=voltReferenceChanged    ).grid(column=0, row=13, sticky=(W,E), padx=20, pady=2 , columnspan= 5)

voltRefValueVoltageLabel = ttk.Label(fVoltage, text="External or Vcc voltage (in mVolt)"  ,state='disabled'  )
voltRefValueVoltageLabel.grid(column= 1, row=13, sticky=(E) ,padx= (5,0), pady=2, columnspan= '2' )
voltRefValueVoltageBox = ttk.Entry(fVoltage, textvariable=voltRefValueVar , width='5' , state='disabled')
voltRefValueVoltageBox.grid( column=3, row=13, sticky=(W,E), padx=20, pady=2 )
