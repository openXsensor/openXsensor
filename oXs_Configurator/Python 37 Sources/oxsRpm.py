from oxsConfig import ttk , W , E , Spinbox ,fRpm , pulsesPerRotationVar 

#define PULSES_PER_ROTATION 2

ttk.Label(fRpm, text="Number of pulses per rotation").grid(row=1, padx= 5 , pady=(20,2))


pulsesPerRotationBox = Spinbox(fRpm, from_=1, to=1000, width= 5 , textvariable=pulsesPerRotationVar)
pulsesPerRotationBox.grid(column=1, row=1 , padx=5, pady=(20,2))

