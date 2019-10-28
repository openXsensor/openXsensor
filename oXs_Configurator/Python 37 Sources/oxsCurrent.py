from oxsConfig import ttk , W , E , fCurrent , currentPinVar , currentExist , currentMvoltAt0Var , currentMvoltPerAmpVar ,\
     currentRcVar , currentRgVar , voltReferenceVar , voltRefValueVar

#here start the definition of the current tab
#define PIN_CURRENTSENSOR      2
#define MVOLT_AT_ZERO_AMP        600
#define MVOLT_PER_AMP            60
#define RESISTOR_TO_GROUND_FOR_CURRENT  10
#define RESISTOR_TO_CURRENT_SENSOR      40
ttk.Label(fCurrent, text="Analog pin" ).grid(column= 0, row=1, padx= (15,5), pady=25 , sticky=(E))
ttk.Label(fCurrent, text="mVolt at 0 Amp" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fCurrent, text="mVolt per Amp" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fCurrent, text="R to ground" ).grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(E))
ttk.Label(fCurrent, text="R to current sensor" ).grid(column= 0, row=5, padx= (15,5), pady=5 , sticky=(E))

currentPinBox = ttk.Combobox(fCurrent, textvariable=currentPinVar ,
                values=('0', "1" , "2" , "3" , "4",  "5" , "6", "7"),
                state="readonly", width=1)
currentPinBox.grid(column=1 , row=1)

currentMvoltAt0Box = ttk.Entry(fCurrent, textvariable=currentMvoltAt0Var , width='5')
currentMvoltAt0Box.grid(column=1 , row=2)

currentMvoltPerAmpBox = ttk.Entry(fCurrent, textvariable=currentMvoltPerAmpVar , width='5')
currentMvoltPerAmpBox.grid(column=1 , row=3)

currentRgBox = ttk.Entry(fCurrent, textvariable=currentRgVar , width='5')
currentRgBox.grid(column=1 , row=4)

currentRcBox = ttk.Entry(fCurrent, textvariable=currentRcVar , width='5')
currentRcBox.grid(column=1 , row=5)

def voltReferenceChanged():
    if voltReferenceVar.get()=='Vcc':
        voltRefValueBox['state']='normal'
        voltRefValueLabel['state']='normal'
    else:
        voltRefValueBox['state']='disabled'
        voltRefValueLabel['state']='disabled'


ttk.Label(fCurrent, text="Reference for voltage measurements"
    ).grid(column= 0, row=10, padx= 5, pady=(30,2), columnspan= '5' )
ttk.Radiobutton(fCurrent,  variable=voltReferenceVar, text='Internal' , value='Internal',
    command=voltReferenceChanged).grid(column=0, row=11, sticky=(W,E), padx=20 , pady=2 , columnspan= 5)
ttk.Radiobutton(fCurrent,  variable=voltReferenceVar, text='External' , value='External',
     command=voltReferenceChanged   ).grid(column=0, row=12, sticky=(W,E), padx=20, pady=2 , columnspan= 5)
ttk.Radiobutton(fCurrent,  variable=voltReferenceVar, text='Vcc' , value='Vcc',
	command=voltReferenceChanged    ).grid(column=0, row=13, sticky=(W,E), padx=20, pady=2 , columnspan= 5)

voltRefValueLabel = ttk.Label(fCurrent, text="Vcc voltage", state='disabled'    )
voltRefValueLabel.grid(column= 1, row=13, sticky=(E) ,padx= (5,0), pady=2, columnspan= '2' )
voltRefValueBox = ttk.Entry(fCurrent, textvariable=voltRefValueVar , width='5' , state='disabled')
voltRefValueBox.grid( column=3, row=13, sticky=(W,E), padx=20, pady=2 )

