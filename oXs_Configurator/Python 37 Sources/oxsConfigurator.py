#to do : 
#        add parameters for Acc (in .py, ini, upload , save, write)
#        add parameters for ADC (in .py, ini, upload , save, write)
#        add parameters for Airspeed (in .py, ini, upload , save, write)
#        add parameters for Flow (in .py, ini, upload , save, write)
#        add parameters for Locator (in .py, ini, upload , save, write)
#        add parameters for Magnet (in .py, ini, upload , save, write)
#        add parameters for Sequence (in .py, ini, upload , save, write)

import tkinter as tk
from tkinter import X, Y, BOTTOM, RIGHT, LEFT, HORIZONTAL , END , DISABLED

#import Pmw

from oxsPpm import ppmTypeBox
from oxsVario import varioTypeBox 
from oxsVoltage import volt1PinBox
from oxsCurrent import currentPinBox
from oxsRpm import pulsesPerRotationBox
from oxsGps import gpsRateBox
from oxsAirspeed import airspeedTypeBox
from oxsImu import *
from oxsMagnet import *
from oxsFlow import *
from oxsAdc import *
from oxsLocator import *
from oxsSequence import *
from oxsAddFields import expAltTimeBox
from oxsFrsky import frskyVfasBox
from oxsMultiplex import MultiplexLine1Box
from oxsHott import hottCellUndervoltageBox
from oxsJeti import jetiVoltageSourceBox


from oxsConfig import  ttk , W , E , BOTH ,nb , HIDDEN , StringVar ,  root , \
    fMain, fPpm , fAdc, fAirspeed , fAnalogVario , fCurrent , fFlow, fFrsky ,fGps , fHott , fImu , fJeti ,\
    fMagnet , fMultiplex , fRpm , fLocator , fSequence, fVario , fVoltage , fAddFields ,\
    ppmChanged , ppmExist , varioChanged , varioExist, voltageChanged , voltageExist , currentChanged , currentExist ,\
    gpsChanged, gpsExist, airspeedChanged, airspeedExist, rpmChanged, rpmExist, imuChanged, imuExist,magnetChanged, magnetExist , \
    flowChanged, flowExist, adcChanged , adcExist , locatorExist , locatorChanged , sequenceChanged , sequenceExist ,	\
    addFieldsExist , addFieldsChanged,  protocolVar , protocolChanged , pinToRxVar , generateOxsConfig , saveConfig , uploadConfig 

def viewOxsDoc():
    global win1
    try:
        if win1.state() == "normal": win1.focus()
    except:
        win1 = tk.Toplevel()
        win1.geometry("300x300+500+200")
        win1["bg"] = "navy"
        #st = Pmw.ScrolledText( win1, borderframe=1, labelpos=E, label_text= "", usehullsize=1,
        #        hull_width=400, hull_height=300, text_padx=10,  text_pady=10, text_wrap='none', text_state = 'disabled')
        #st.pack(fill=BOTH, expand=1, padx=5, pady=5)
        sy = tk.Scrollbar(win1)
        sx = tk.Scrollbar(win1 ,  orient=HORIZONTAL)
        oxsDoc = tk.Text(win1 , height=500, width=300, wrap='none')
        sx.pack(side=BOTTOM, fill=X)
        sy.pack(side=RIGHT, fill=Y)
        oxsDoc.pack(side=LEFT, fill=Y)
        sy.config(command=oxsDoc.yview)
        sx.config(command=oxsDoc.xview)
       #self.pack()
#       
        
        fileName = r"..\..\openXsensor\oXs_config_description.h"
        try:
            f = open(fileName, "r")
            oxsDoc.insert(END, f.read() )
        #    st.settext( text = f.read() )
        except:
            oxsDoc.insert(END, "File with docs not found")
        #    st.settext(text = "File with docs not found")
        oxsDoc.config(state=DISABLED)


ttk.Label(fMain,  text="Components (details to be filled in tabs)").grid( row=0 , pady=(10,2)) #column=0 , sticky=(N,W,E,S)
ttk.Checkbutton(fMain, text='Ppm',  command=ppmChanged, variable=ppmExist,
        onvalue='On', offvalue='Off').grid(row=1, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Vario', command=varioChanged, variable=varioExist,
        onvalue='On', offvalue='Off').grid(row=2, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Voltages',  command=voltageChanged, variable=voltageExist,
        onvalue='On', offvalue='Off').grid(row=3, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Current',  command=currentChanged, variable=currentExist,
        onvalue='On', offvalue='Off').grid(row=4, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='GPS',  command=gpsChanged, variable=gpsExist,
        onvalue='On', offvalue='Off').grid(row=5, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Airspeed',  command=airspeedChanged, variable=airspeedExist,
        onvalue='On', offvalue='Off').grid(row=6, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Rpm',  command=rpmChanged, variable=rpmExist,
        onvalue='On', offvalue='Off').grid(row=7, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Acc/Gyro (IMU6050)',  command=imuChanged, variable=imuExist,
        onvalue='On', offvalue='Off').grid(row=8, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Magn. (HMC5883)',  command=magnetChanged, variable=magnetExist,
        onvalue='On', offvalue='Off').grid(row=9, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Flow sensor',  command=flowChanged, variable=flowExist,
        onvalue='On', offvalue='Off').grid(row=10, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='External ADC (ADS1115)',  command=adcChanged, variable=adcExist,
        onvalue='On', offvalue='Off').grid(row=11, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Locator',  command=locatorChanged, variable=locatorExist,
        onvalue='On', offvalue='Off').grid(row=12, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Sequencer',  command=sequenceChanged, variable=sequenceExist,
        onvalue='On', offvalue='Off').grid(row=13, sticky=(W,E), padx=20 , pady=5)
ttk.Checkbutton(fMain, text='Extra fields (TEST 1,2,3)',  command=addFieldsChanged, variable=addFieldsExist,
        onvalue='On', offvalue='Off').grid(row=14, sticky=(W,E), padx=20 , pady=5)

ttk.Label(fMain, text="Protocol of Rx").grid(column=0, row=15, pady=(20,2))
protocolBox = ttk.Combobox(fMain, textvariable=protocolVar, state = 'readonly' , 
                values=('FRSKY_SPORT', 'JETI', 'HOTT', 'MULTIPLEX', 'FRSKY_HUB' , 'FRSKY_SPORT_HUB'))
protocolBox.bind('<<ComboboxSelected>>', protocolChanged)
protocolBox.grid(column=1, row=15 , pady=(20,2) )

ttk.Label(fMain, text="Pin used for telemetry").grid(column=0, row=16, pady=(5,2))
pinToRxBox = ttk.Combobox(fMain, textvariable=pinToRxVar, 
                values=('4', '2'), state='readonly',  width=2)
pinToRxBox.grid(column=1, row=16, pady=(5,2))

nb.add(fMain, text='Main')
nb.add(fPpm, text='Ppm', state=HIDDEN)
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
nb.add(fAddFields , text= 'Extra Fields' , state=HIDDEN)
nb.add(fFrsky, text='FRSKY', state='normal')
nb.add(fJeti, text='JETI', state=HIDDEN)
nb.add(fHott, text='HOTT', state=HIDDEN)
nb.add(fMultiplex, text='MULTIPLEX', state=HIDDEN)
nb.pack()

fMainRight = ttk.Frame(fMain)
fMainRight.grid(column=3 , row=0, rowspan=10)
ttk.Label(fMainRight, text="-Select components\n-Select protocol\n-Fill tab(s)\n   or\n-Upload a Config\n\n-Generate oXs config").grid(column=0, row=0, pady=(20,2))
ttk.Button(fMainRight, text='Upload Config', command=uploadConfig).grid(column=0, row=1, pady=10)
ttk.Button(fMainRight, text='Save Config', command=saveConfig).grid(column=0, row=2, pady=10)
ttk.Button(fMainRight, text='Generate oXs Config', command=generateOxsConfig).grid(column=0, row=4, pady=30)
ttk.Button(fMainRight, text='View oXs doc', command=viewOxsDoc).grid(column=0, row=5, pady=30)



root.mainloop()

