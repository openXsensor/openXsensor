from oxsConfig import ttk , W , E , Spinbox 
from oxsConfig import fPpm , fPpmType , ppmTypeVar , ppmTypeChanged , ppmPinVar ,  ppmForMin100Var , ppmForPlus100Var

#define PIN_PPM           2     // Uncomment this line in order to use a Rx channel to control oXs; default is 2 but my own device use 3
#define PPM_MIN_100       988     // default 1500 - 512 ; // pulse width (usec) when TX sends a channel = -100
#define PPM_PLUS_100      2012    // default 1500 + 512 ; // pulse width (usec) when TX sends a channel = +100

#define PPM_VIA_SPORT             // uncomment this line to get ppm data over SPORT protocol instead of from a PWM channel (it requires a Tx with openTx running LUA script)



#fPpmType = ttk.Frame(fPpm)
fPpmType.grid(column= 1 , row=8) #by default make it visible

ttk.Label(fPpm, text="Ppm type" ).grid(column= 0, row=1, padx= 5 , pady = 10 , sticky = W)
ppmTypeBox = ttk.Combobox(fPpm, textvariable=ppmTypeVar , 
                values=('Rx chanel', "Sport"), state="readonly", width=12)
ppmTypeBox.bind('<<ComboboxSelected>>', ppmTypeChanged)
ppmTypeBox.grid(column=1, row=1 , padx=10)

ttk.Label(fPpmType, text="Pin" ).grid(column= 0, row=0, padx= 5 , pady=10)

ppmPinBox = ttk.Combobox(fPpmType, textvariable=ppmPinVar, values=('2', '3') , state="readonly" ,width=2)
ppmPinBox.grid(column=1, row=0, padx=10 , pady=10)

ttk.Label(fPpmType, text="nbr of micro seconds for ppm = -100" ).grid(column= 0, row=1, padx= 5 , pady=10)
ppmForMin100Box = Spinbox(fPpmType, from_=800 , to=1200, textvariable=ppmForMin100Var , 
                        increment='1', width=5)
ppmForMin100Box.grid(column=1, row=1 , padx=10 , pady=10)

ttk.Label(fPpmType, text="nbr of micro seconds for ppm = +100" ).grid(column= 0, row=1, padx= 5 , pady=10)
ppmForPlus100Box = Spinbox(fPpmType, from_=1800 , to=2200 , textvariable=ppmForPlus100Var , 
                        increment='1', width=5)
ppmForPlus100Box.grid(column=1, row=2 , padx=10 , pady=10)
