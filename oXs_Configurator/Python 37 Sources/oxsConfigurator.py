#to do : add the parameters to fill TEST1...3 (in oxsAddFields.py and Ini, upload , save, write)
#        add parameters for Acc (in .py, ini, upload , save, write)
#        add parameters for ADC (in .py, ini, upload , save, write)
#        add parameters for Airsped (in .py, ini, upload , save, write)
#        add parameters for Flow (in .py, ini, upload , save, write)
#        add parameters for Locator (in .py, ini, upload , save, write)
#        add parameters for Magnet (in .py, ini, upload , save, write)
#        add parameters for Rpm (in .py, ini, upload , save, write)
#        add parameters for Sequence (in .py, ini, upload , save, write)
#        in Multiplex protocol, add input parameters (at least min and max)
#        Fill the config_basic.h with data for Multiplex, Hott and Jeti protocols


from oxsVario import varioTypeBox 
from oxsVoltage import volt1PinBox
from oxsCurrent import currentPinBox
from oxsGps import gpsRateBox
from oxsFrsky import frskyVfasBox
from oxsMultiplex import MultiplexLine1Box
from oxsHott import hottCellUndervoltageBox
from oxsJeti import jetiVoltageSourceBox



from oxsConfig import ttk , W , E , nb , HIDDEN , root , \
	fMain, fAdc, fAirspeed , fAnalogVario , fCurrent , fFlow, fFrsky ,fGps , fHott , fImu , fJeti ,\
	fMagnet , fMultiplex , fRpm , fLocator , fSequence, fVario , fVoltage , \
	varioChanged , varioExist, voltageChanged , voltageExist , currentChanged , currentExist ,\
	gpsChanged, gpsExist, airspeedChanged, airspeedExist, rpmChanged, rpmExist, imuChanged, imuExist,magnetChanged, magnetExist , \
	flowChanged, flowExist, adcChanged , adcExist , locatorExist , locatorChanged , sequenceChanged , sequenceExist ,	\
	generateOxsConfig , saveConfig , uploadConfig 

ttk.Label(fMain,  text="Components").grid( row=0 , pady=(10,2)) #column=0 , sticky=(N,W,E,S)

varioCheck = ttk.Checkbutton(fMain, text='Vario', 
	    command=varioChanged, variable=varioExist,
	    onvalue='On', offvalue='Off').grid(row=1, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Voltages',  command=voltageChanged, variable=voltageExist,
	    onvalue='On', offvalue='Off').grid(row=2, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Current',  command=currentChanged, variable=currentExist,
	    onvalue='On', offvalue='Off').grid(row=3, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='GPS',  command=gpsChanged, variable=gpsExist,
	    onvalue='On', offvalue='Off').grid(row=4, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Airspeed',  command=airspeedChanged, variable=airspeedExist,
	    onvalue='On', offvalue='Off').grid(row=5, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Rpm',  command=rpmChanged, variable=rpmExist,
	    onvalue='On', offvalue='Off').grid(row=6, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Acc/Gyro (IMU6050)',  command=imuChanged, variable=imuExist,
	    onvalue='On', offvalue='Off').grid(row=7, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Magn. (HMC5883)',  command=magnetChanged, variable=magnetExist,
	    onvalue='On', offvalue='Off').grid(row=8, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Flow sensor',  command=flowChanged, variable=flowExist,
	    onvalue='On', offvalue='Off').grid(row=9, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='External ADC (ADS1115)',  command=adcChanged, variable=adcExist,
	    onvalue='On', offvalue='Off').grid(row=10, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Locator',  command=locatorChanged, variable=locatorExist,
	    onvalue='On', offvalue='Off').grid(row=11, sticky=(W,E), padx=20 , pady=5)

ttk.Checkbutton(fMain, text='Sequencer',  command=sequenceChanged, variable=sequenceExist,
	    onvalue='On', offvalue='Off').grid(row=12, sticky=(W,E), padx=20 , pady=5)

ttk.Label(fMain, text="Protocol of Rx").grid(column=0, row=13, pady=(20,2))

nb.add(fMain, text='Main')
nb.add(fVario, text='Vario', state=HIDDEN)
nb.add(fVoltage, text='Voltages', state=HIDDEN)
nb.add(fCurrent, text='Current', state=HIDDEN)
nb.add(fGps, text='GPS', state=HIDDEN)
nb.add(fAirspeed, text='Airspeed', state=HIDDEN)
nb.add(fRpm, text='Rpm', state=HIDDEN)
nb.add(fImu, text='Acc/Gyro (IMU6050)', state=HIDDEN)
nb.add(fMagnet, text='Magnet (HMC5883)', state=HIDDEN)
nb.add(fFlow, text='Flow', state=HIDDEN)
nb.add(fAdc, text='ADC', state=HIDDEN)
nb.add(fLocator , text='Locator' , state=HIDDEN)
nb.add(fSequence, text='Sequencer', state=HIDDEN)
nb.add(fFrsky, text='FRSKY', state='normal')
nb.add(fJeti, text='JETI', state=HIDDEN)
nb.add(fHott, text='HOTT', state=HIDDEN)
nb.add(fMultiplex, text='MULTIPLEX', state=HIDDEN)
nb.pack()

fMainRight = ttk.Frame(fMain)
fMainRight.grid(column=3 , row=0, rowspan=10)
ttk.Label(fMainRight, text="Select\ncomponents,\nthe protocol\nand\nfill tabs\nor upload a\nConfig").grid(column=0, row=0, pady=(20,2))
ttk.Button(fMainRight, text='Upload Config', command=uploadConfig).grid(column=0, row=1, pady=10)
ttk.Button(fMainRight, text='Save Config', command=saveConfig).grid(column=0, row=2, pady=10)
ttk.Button(fMainRight, text='Generate oXs Config', command=generateOxsConfig).grid(column=0, row=4, pady=10)



root.mainloop()
