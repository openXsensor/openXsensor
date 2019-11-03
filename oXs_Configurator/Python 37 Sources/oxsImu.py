#from tkinter import StringVar
from oxsConfig import ttk , W , E , fImu


#define PIN_INT_6050 2   // Interrupt from 6050 has to be connected to Arduino pin 2 or pin 3 (do not use here the same pin as PPM)

#//#define DISPLAY_ACC_OFFSET // used ONLY in order to display the acceleration offset on pc terminal; KEEP AS COMMENT once offsets have been setup 

#define ACC_OFFSET_X 0 // fill here the first value reported when DISPLAY_ACC_OFFSET is activated (eg. -160)
#define ACC_OFFSET_Y 0 // fill here the second value reported when DISPLAY_ACC_OFFSET is activated (eg. -150)
#define ACC_OFFSET_Z 0 // fill here the third value reported when DISPLAY_ACC_OFFSET is activated (eg. -1100)

#dummyImuVar = StringVar(value='-')
ttk.Label(fImu, text="Not yet implemented (so you have to edit manually the config files)" ).grid(column= 0, row=4, padx= 1 , pady= 20)
#dummyBox = ttk.Combobox(fImu, textvariable=dummyImuVar, 
#                values=( "-"  ),
#                state="readonly")