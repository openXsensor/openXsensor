from oxsConfig import ttk , W , E , fMemory, pushButtPinVar

#define SAVE_TO_EEPROM    NO
#define PIN_PUSHBUTTON    10   


fMemory.grid(column= 1 , row=8) #by default make it visible

ttk.Label(fMemory, text="Pin PushButton" ).grid(column= 0, row=0, padx= (15,5) , pady=10 , sticky=(W))

ttk.Combobox(fMemory, textvariable=pushButtPinVar, values=('5', '7', '8', '9', '10', '11', '12', '13') , state="readonly" ,width=2).grid(column=1, row=0, sticky=(W), padx=10 , pady=10)
