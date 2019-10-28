from tkinter import StringVar , Tk , W ,E , S , HIDDEN, DoubleVar , Spinbox , IntVar , filedialog
from tkinter import ttk 
from tkinter import messagebox


#from impExp import *
import configparser

configUploadFileName = None

def generateOxsConfig():
    f=open("oXs_config_basic.h" , 'w')
    fa=open("oXs_config_advanced.h" , 'w')
    f.write("#ifndef OXS_CONFIG_BASIC_h\n")
    f.write("#define OXS_CONFIG_BASIC_h\n")
    fa.write("#ifndef OXS_CONFIG_ADVANCED_h\n")
    fa.write("#define OXS_CONFIG_ADVANCED_h\n")
    fa.write(f"#define PIN_SERIALTX {pinToRxVar.get()}\n")
    f.write(f"\n#define PROTOCOL {protocolVar.get()}\n\n")
    if (protocolVar.get() == 'FRSKY_SPORT' or
         protocolVar.get() == 'FRSKY_SPORT_HUB' or
         protocolVar.get() == 'FRSKY_HUB') :
        if frskyVfasVar.get() != 'Not sent':
            f.write(f"#define VFAS_SOURCE {frskyVfasVar.get()}\n")
        if frskyFuelVar.get() != 'Not sent':
            f.write(f"#define FUEL_SOURCE {frskyFuelVar.get()}\n")
        if frskyA3Var.get() != 'Not sent':
            f.write(f"#define A3_SOURCE {frskyA3Var.get()}\n")
        if frskyA4Var.get() != 'Not sent':
            f.write(f"#define A4_SOURCE {frskyA4Var.get()}\n")
        if frskyAccxVar.get() != 'Not sent':
            f.write(f"#define ACCX_SOURCE {frskyAccxVar.get()}\n")
        if frskyAccyVar.get() != 'Not sent':
            f.write(f"#define ACCY_SOURCE {frskyAccyVar.get()}\n")
        if frskyAcczVar.get() != 'Not sent':
            f.write(f"#define ACCZ_SOURCE {frskyAcczVar.get()}\n")
        if frskyT1Var.get() != 'Not sent':
            f.write(f"#define T1_SOURCE {frskyT1Var.get()}\n")
        if frskyT2Var.get() != 'Not sent':
            f.write(f"#define T2_SOURCE {frskyT2Var.get()}\n")          

    if protocolVar.get() == 'FRSKY_SPORT' or protocolVar.get() == 'FRSKY_SPORT_HUB':
        fa.write("\n#define         DATA_ID_VARIO  0x00\n")
        fa.write("#define         DATA_ID_FLVSS  0xA1\n")
        fa.write("#define         DATA_ID_FAS    0x22\n")
        fa.write("#define         DATA_ID_GPS    0x83\n")
        fa.write("#define         DATA_ID_RPM    0xE4\n")
        fa.write("#define         DATA_ID_ACC    0x67\n")
        fa.write("#define         DATA_ID_TX     0x0D\n")



    
    if varioExist.get() == "On":
        f.write(f"#define FIRST_BARO_SENSOR_USE {varioTypeVar.get()}\n")
        f.write(f"#define VSPEED_SOURCE {varioSourceVar.get()}\n")
        if secondBaroExist.get() == 'On':
            fa.write("\n#define SECOND_BARO_SENSOR_USE    NO_BARO\n")
        else:
            fa.write("\n#define SECOND_BARO_SENSOR_USE    MS5611\n")
        fa.write("\n#define SENSITIVITY_MIN 80\n")
        fa.write("#define SENSITIVITY_MAX 300\n")
        fa.write("#define SENSITIVITY_MIN_AT 100\n")
        fa.write("#define SENSITIVITY_MAX_AT 1000\n")
        fa.write(f"\n#define VARIOHYSTERESIS {str(varioHysteresisVar.get())}\n")
        if analogVarioExist.get() == "On":
            fa.write(f"\n#define PIN_ANALOG_VSPEED {varioAnalogPinVar.get()}\n")
            fa.write(f"#define ANALOG_VSPEED_MIN  {varioAnalogMinVar.get()}\n")
            fa.write(f"#define ANALOG_VSPEED_MAX  {varioAnalogMaxVar.get()}\n")
    else:
        f.write("#define FIRST_BARO_SENSOR_USE NO_BARO\n")
    if airspeedExist.get() == "On":
        f.write(f"\n#define AIRSPEED_SENSOR_USE {airspeedTypeVar.get()}\n")
    else:
        f.write("\n#define AIRSPEED_SENSOR_USE NO_AIRSPEED\n")
    if voltageExist.get() == "On":
        f.write("\n#define ARDUINO_MEASURES_VOLTAGES   YES\n")
        f.write(f"#define NUMBEROFCELLS {voltNumberOfCellsVar.get()}\n")
        if voltReferenceVar.get() == "Internal":
            fa.write("\n#define USE_INTERNAL_REFERENCE\n")
        elif voltReferenceVar.get() == "Internal":
            fa.write("\n#define USE_EXTERNAL_REFERENCE\n")
        else:
            fa.write(f"\n#define REFERENCE_VOLTAGE {str(voltRefValueVar.get())}\n")
        fa.write(f"\n#define PIN_VOLTAGE ")
        if volt1Exist.get() == 'On':
            fa.write(f" {volt1PinVar.get()} , ")
        else:
            fa.write("8 , ")
        if volt2Exist.get() == 'On':
            fa.write(f" {volt2PinVar.get()} , ")
        else:
            fa.write("8 , ")
        if volt3Exist.get() == 'On':
            fa.write(f" {volt3PinVar.get()} , ")
        else:
            fa.write("8 , ")
        if volt4Exist.get() == 'On':
            fa.write(f" {volt4PinVar.get()} , ")
        else:
            fa.write("8 , ")
        if volt5Exist.get() == 'On':
            fa.write(f" {volt5PinVar.get()} , ")
        else:
            fa.write("8 , ")
        if volt6Exist.get() == 'On':
            fa.write(f" {volt6PinVar.get()} \n")
        else:
            fa.write("8\n")
        fa.write((f"#define RESISTOR_TO_GROUND {volt1RgVar.get()} , " +
                 f"{volt2RgVar.get()} , {volt3RgVar.get()} , {volt4RgVar.get()} , " +
                 f"{volt5RgVar.get()} , {volt6RgVar.get()}\n"))
        fa.write((f"#define RESISTOR_TO_VOLTAGE {volt1RvVar.get()} , " +
                 f"{volt2RvVar.get()} , {volt3RvVar.get()} , {volt4RvVar.get()} , " +
                 f"{volt5RvVar.get()} , {volt6RvVar.get()}\n"))
        fa.write((f"#define OFFSET_VOLTAGE {volt1OffsetVar.get()} , " +
                 f"{volt2OffsetVar.get()} , {volt3OffsetVar.get()} , {volt4OffsetVar.get()} , " +
                 f"{volt5OffsetVar.get()} , {volt6OffsetVar.get()}\n"))
        fa.write((f"#define SCALE_VOLTAGE {volt1ScaleVar.get()} , " +
                 f"{volt2ScaleVar.get()} , {volt3ScaleVar.get()} , {volt4ScaleVar.get()} , " +
                 f"{volt5ScaleVar.get()} , {volt6ScaleVar.get()}\n"))
    else:
        f.write("\n#define ARDUINO_MEASURES_VOLTAGES   NO\n")
    
    if currentExist.get() == "On":
        f.write("\n#define ARDUINO_MEASURES_A_CURRENT YES\n")
        fa.write(f"\n#define PIN_CURRENTSENSOR {currentPinVar.get()}\n")
        fa.write(f"#define MVOLT_AT_ZERO_AMP {str(currentMvoltAt0Var.get())}\n")
        fa.write(f"#define MVOLT_PER_AMP     {str(currentMvoltPerAmpVar.get())}\n")
        fa.write(f"#define RESISTOR_TO_GROUND_FOR_CURRENT {str(currentRgVar.get())}\n")
        fa.write(f"#define RESISTOR_TO_CURRENT_SENSOR     {str(currentRcVar.get())}\n")
    else:
        f.write("\n#define ARDUINO_MEASURES_A_CURRENT NO\n")
    
    if adcExist.get() == "On":
        f.write("\n#define AN_ADS1115_IS_CONNECTED YES\n")
    else:
        f.write("\n#define AN_ADS1115_IS_CONNECTED NO\n")

    if rpmExist.get() == "On":
        f.write("\n#define CALCULATE_RPM YES\n")
    else:
        f.write("\n#define CALCULATE_RPM NO\n")

    f.write("\n#define SAVE_TO_EEPROM NO\n") 

    if gpsExist.get() == "On":
        f.write("\n#define A_GPS_IS_CONNECTED YES\n")
        if gps3dExist.get() == 'On':
            fa.write("\n#define GPS_SPEED_3D\n")
            fa.write(f"#define GPS_REFRESH_RATE {gpsRateVar.get()}\n")  
    else:
        f.write("\n#define A_GPS_IS_CONNECTED NO\n")

    if imuExist.get() == "On":
        f.write("\n#define A_MPU6050_IS_CONNECTED YES\n")
    else:
        f.write("\n#define A_MPU6050_IS_CONNECTED NO\n")

    if magnetExist.get() == "On":
        f.write("\n#define CALCULATE_YAW_WITH_HMC5883 YES\n")
    else:
        f.write("\n#define CALCULATE_YAW_WITH_HMC5883 NO\n")

    if flowExist.get() == "On":
        f.write("\n#define A_FLOW_SENSOR_IS_CONNECTED   YES\n")
    else:
        f.write("\n#define A_FLOW_SENSOR_IS_CONNECTED   NO\n")
    
    if locatorExist.get() == "On":
        f.write("\n#define A_LOCATOR_IS_CONNECTED   YES\n")
    else:
        f.write("\n#define A_LOCATOR_IS_CONNECTED   NO\n")
    



    f.write("\n#endif\n")
    fa.write("\n#endif\n") 
    f.close()
    fa.close()
    messagebox.showinfo("oXs configurator", "Config files advanced and basic have been created")

config =  configparser.ConfigParser()
config.add_section("Main")
config.add_section("Vario")
config.add_section("Voltage")
config.add_section("Current")
config.add_section("Gps")

def uploadConfig():   
    print("Read config")
    #global configUploadFileName
    #configUploadFileName = filedialog.askopenfilename(initialdir="C:/Data/",
    configUploadFileName = filedialog.askopenfilename(\
                           filetypes =(("Ini files", "*.ini"),("All files","*.*")),
                           title = "Choose a file."
                           )
    #config.read('config.ini')
    config.read(configUploadFileName)
    varioExist.set(value= config.get('Main', 'varioExist'))
    varioChanged()
    voltageExist.set(value = config.get("Main", "voltageExist"))
    voltageChanged()
    currentExist.set(value = config.get("Main", "currentExist"))
    currentChanged()
    gpsExist.set(value = config.get("Main", "gpsExist"))
    gpsChanged()
    airspeedExist.set(value= config.get("Main", "airspeedExist"))
    airspeedChanged()
    rpmExist.set(value= config.get("Main", "rpmExist"))
    rpmChanged()
    imuExist.set(value= config.get("Main", "imuExist"))
    imuChanged()
    magnetExist.set(value= config.get("Main", "magnetExist"))
    magnetChanged()
    flowExist.set(value= config.get("Main", "flowExist"))
    flowChanged()
    adcExist.set(value= config.get("Main", "adcExist"))
    adcChanged()
    sequenceExist.set(value= config.get("Main", "sequenceExist"))
    sequenceChanged()
    protocolVar.set(value= config.get("Main", "protocolVar"))
    protocolChanged()
    varioTypeVar.set(value= config.get("Vario", "varioTypeVar"))
    secondBaroExist.set(value= config.get("Vario", "secondBaroExist"))
    varioSourceVar.set(value= config.get("Vario", "varioSourceVar"))
    varioHysteresisVar.set(value= config.getint("Vario", "varioHysteresisVar"))
    analogVarioExist.set(value= config.get("Vario", "analogVarioExist"))
    analogVarioChanged()
    varioAnalogPinVar.set(value= config.get("Vario", "varioAnalogPinVar"))
    varioAnalogMinVar.set(value= config.getfloat("Vario", "varioAnalogMinVar"))
    varioAnalogMaxVar.set(value= config.getfloat("Vario", "varioAnalogMaxVar"))
    
    volt1Exist.set(value= config.get("Voltage", "volt1Exist"))
    volt2Exist.set(value= config.get("Voltage", "volt2Exist"))
    volt3Exist.set(value= config.get("Voltage", "volt3Exist"))
    volt4Exist.set(value= config.get("Voltage", "volt4Exist"))
    volt5Exist.set(value= config.get("Voltage", "volt5Exist"))
    volt6Exist.set(value= config.get("Voltage", "volt6Exist"))
    volt1PinVar.set(value= config.get("Voltage", "volt1PinVar"))
    volt2PinVar.set(value= config.get("Voltage", "volt2PinVar"))
    volt3PinVar.set(value= config.get("Voltage", "volt3PinVar"))
    volt4PinVar.set(value= config.get("Voltage", "volt4PinVar"))
    volt5PinVar.set(value= config.get("Voltage", "volt5PinVar"))
    volt6PinVar.set(value= config.get("Voltage", "volt6PinVar"))
    volt1RgVar.set(value= config.getfloat("Voltage", "volt1RgVar"))
    volt2RgVar.set(value= config.getfloat("Voltage", "volt2RgVar"))
    volt3RgVar.set(value= config.getfloat("Voltage", "volt3RgVar"))
    volt4RgVar.set(value= config.getfloat("Voltage", "volt4RgVar"))
    volt5RgVar.set(value= config.getfloat("Voltage", "volt5RgVar"))
    volt6RgVar.set(value= config.getfloat("Voltage", "volt6RgVar"))
    volt1RvVar.set(value= config.getfloat("Voltage", "volt1RvVar"))
    volt2RvVar.set(value= config.getfloat("Voltage", "volt2RvVar"))
    volt3RvVar.set(value= config.getfloat("Voltage", "volt3RvVar"))
    volt4RvVar.set(value= config.getfloat("Voltage", "volt4RvVar"))
    volt5RvVar.set(value= config.getfloat("Voltage", "volt5RvVar"))
    volt6RvVar.set(value= config.getfloat("Voltage", "volt6RvVar"))
    volt1ScaleVar.set(value= config.getfloat("Voltage", "volt1ScaleVar"))
    volt2ScaleVar.set(value= config.getfloat("Voltage", "volt2ScaleVar"))
    volt3ScaleVar.set(value= config.getfloat("Voltage", "volt3ScaleVar"))
    volt4ScaleVar.set(value= config.getfloat("Voltage", "volt4ScaleVar"))
    volt5ScaleVar.set(value= config.getfloat("Voltage", "volt5ScaleVar"))
    volt5ScaleVar.set(value= config.getfloat("Voltage", "volt6ScaleVar"))
    volt1OffsetVar.set(value= config.getfloat("Voltage", "volt1OffsetVar"))
    volt2OffsetVar.set(value= config.getfloat("Voltage", "volt2OffsetVar"))
    volt3OffsetVar.set(value= config.getfloat("Voltage", "volt3OffsetVar"))
    volt4OffsetVar.set(value= config.getfloat("Voltage", "volt4OffsetVar"))
    volt5OffsetVar.set(value= config.getfloat("Voltage", "volt5OffsetVar"))
    volt6OffsetVar.set(value= config.getfloat("Voltage", "volt6OffsetVar"))
        
    currentPinVar.set(value= config.get("Current", "currentPinVar"))
    currentMvoltAt0Var.set(value= config.getfloat("Current", "currentMvoltAt0Var"))
    currentMvoltPerAmpVar.set(value= config.getfloat("Current", "currentMvoltPerAmpVar"))
    currentRgVar.set(value= config.getfloat("Current", "currentRgVar"))
    currentRcVar.set(value= config.getfloat("Current", "currentRcVar"))
    
    gps3dExist.set(value= config.get("Gps", "gps3dexist"))
    gpsRateVar.set(value= config.getint("Gps", "gpsratevar"))

def saveConfig():   
    config.set("Main", "varioExist", varioExist.get())
    config.set("Main", "voltageExist", voltageExist.get())
    config.set("Main", "currentExist", currentExist.get())
    config.set("Main", "gpsExist", gpsExist.get())
    config.set("Main", "airspeedExist", airspeedExist.get())
    config.set("Main", "rpmExist", rpmExist.get())
    config.set("Main", "imuExist", imuExist.get())
    config.set("Main", "magnetExist", magnetExist.get())
    config.set("Main", "flowExist", flowExist.get())
    config.set("Main", "adcExist", adcExist.get())
    config.set("Main", "sequenceExist", sequenceExist.get())
    config.set("Main", "protocolVar", protocolVar.get())
    config.set("Vario", "varioTypeVar", varioTypeVar.get())
    config.set("Vario", "secondBaroExist", secondBaroExist.get())
    config.set("Vario", "varioSourceVar", varioSourceVar.get())
    config.set("Vario", "varioHysteresisVar", str(varioHysteresisVar.get()))
    config.set("Vario", "analogVarioExist", analogVarioExist.get())
    config.set("Vario", "varioAnalogPinVar", varioAnalogPinVar.get())
    config.set("Vario", "varioAnalogMinVar", str(varioAnalogMinVar.get()))
    config.set("Vario", "varioAnalogMaxVar", str(varioAnalogMaxVar.get()))
    config.set("Voltage", "volt1Exist", volt1Exist.get())
    config.set("Voltage", "volt2Exist", volt2Exist.get())
    config.set("Voltage", "volt3Exist", volt3Exist.get())
    config.set("Voltage", "volt4Exist", volt4Exist.get())
    config.set("Voltage", "volt5Exist", volt5Exist.get())
    config.set("Voltage", "volt6Exist", volt6Exist.get())
    config.set("Voltage", "volt1PinVar", volt1PinVar.get())
    config.set("Voltage", "volt2PinVar", volt2PinVar.get())
    config.set("Voltage", "volt3PinVar", volt3PinVar.get())
    config.set("Voltage", "volt4PinVar", volt4PinVar.get())
    config.set("Voltage", "volt5PinVar", volt5PinVar.get())
    config.set("Voltage", "volt6PinVar", volt6PinVar.get())
    config.set("Voltage", "volt1RgVar", str(volt1RgVar.get()))
    config.set("Voltage", "volt2RgVar", str(volt2RgVar.get()))
    config.set("Voltage", "volt3RgVar", str(volt3RgVar.get()))
    config.set("Voltage", "volt4RgVar", str(volt4RgVar.get()))
    config.set("Voltage", "volt5RgVar", str(volt5RgVar.get()))
    config.set("Voltage", "volt6RgVar", str(volt6RgVar.get()))
    config.set("Voltage", "volt1RvVar", str(volt1RvVar.get()))
    config.set("Voltage", "volt2RvVar", str(volt2RvVar.get()))
    config.set("Voltage", "volt3RvVar", str(volt3RvVar.get()))
    config.set("Voltage", "volt4RvVar", str(volt4RvVar.get()))
    config.set("Voltage", "volt5RvVar", str(volt5RvVar.get()))
    config.set("Voltage", "volt6RvVar", str(volt6RvVar.get()))
    config.set("Voltage", "volt1ScaleVar", str(volt1ScaleVar.get()))
    config.set("Voltage", "volt2ScaleVar", str(volt2ScaleVar.get()))
    config.set("Voltage", "volt3ScaleVar", str(volt3ScaleVar.get()))
    config.set("Voltage", "volt4ScaleVar", str(volt4ScaleVar.get()))
    config.set("Voltage", "volt5ScaleVar", str(volt5ScaleVar.get()))
    config.set("Voltage", "volt6ScaleVar", str(volt6ScaleVar.get()))
    config.set("Voltage", "volt1OffsetVar", str(volt1OffsetVar.get()))
    config.set("Voltage", "volt2OffsetVar", str(volt2OffsetVar.get()))
    config.set("Voltage", "volt3OffsetVar", str(volt3OffsetVar.get()))
    config.set("Voltage", "volt4OffsetVar", str(volt4OffsetVar.get()))
    config.set("Voltage", "volt5OffsetVar", str(volt5OffsetVar.get()))
    config.set("Voltage", "volt6OffsetVar", str(volt6OffsetVar.get()))
    config.set("Voltage", "voltReferenceVar", voltReferenceVar.get())
    config.set("Voltage", "voltRefValueVar", str(voltRefValueVar.get()))
    config.set("Current", "currentPinVar", currentPinVar.get())
    config.set("Current", "currentMvoltAt0Var", str(currentMvoltAt0Var.get()))
    config.set("Current", "currentMvoltPerAmpVar", str(currentMvoltPerAmpVar.get()))
    config.set("Current", "currentRgVar", str(currentRgVar.get()))
    config.set("Current", "currentRcVar", str(currentRcVar.get()))
    config.set("Gps", "gps3dExist", gps3dExist.get())
    config.set("Gps", "gpsRateVar", gpsRateVar.get())
    configSaveFileName = filedialog.asksaveasfilename(title="Save as...", defaultextension="*.ini",\
         filetypes=[("Ini files","*.ini"),("All files", "*")], initialfile=configUploadFileName)
    print(configSaveFileName)
    config.write(open(configSaveFileName ,'w'))
    #configUploadFileName = configSaveFileName
    #config.write(open('config.ini','w'))


def protocolChanged(*arg):
    print(protocolVar.get() )
    nb.tab(13, state="hidden")
    nb.tab(14, state="hidden")
    nb.tab(15, state="hidden")
    nb.tab(16, state="hidden")
    if protocolVar.get() == "FRSKY_SPORT" or protocolVar.get() == "FRSKY_HUB" or protocolVar.get() == "FRSKY_SPORT_HUB" :
        nb.tab(13, state="normal")
    elif protocolVar.get() == "JETI":
        nb.tab(14, state="normal")
    elif protocolVar.get() == "HOTT":
        nb.tab(15, state="normal")
    elif protocolVar.get() == "MULTIPLEX":
        nb.tab(16, state="normal")


def varioChanged():
    if varioExist.get() == 'On':
        nb.tab(1, state="normal")
    else:
        nb.tab(1, state="hidden")
def voltageChanged():
    if voltageExist.get() == 'On':
        nb.tab(2, state="normal")
    else:
        nb.tab(2, state="hidden")
def currentChanged():
    if currentExist.get() == 'On':
        nb.tab(3, state="normal")
    else:
        nb.tab(3, state="hidden")
def gpsChanged():
    if gpsExist.get() == 'On':
        nb.tab(4, state="normal")
    else:
        nb.tab(4, state="hidden")
def airspeedChanged():
    if airspeedExist.get() == 'On':
        nb.tab(5, state="normal")
    else:
        nb.tab(5, state="hidden")
def rpmChanged():
    if rpmExist.get() == 'On':
        nb.tab(6, state="normal")
    else:
        nb.tab(6, state="hidden")
def imuChanged():
    if imuExist.get() == 'On':
        nb.tab(7, state="normal")
    else:
        nb.tab(7, state="hidden")
def magnetChanged():
    if magnetExist.get() == 'On':
        nb.tab(8, state="normal")
    else:
        nb.tab(8, state="hidden")
def flowChanged():
    if flowExist.get() == 'On':
        nb.tab(9, state="normal")
    else:
        nb.tab(9, state="hidden")
def adcChanged():
    if adcExist.get() == 'On':
        nb.tab(10, state="normal")
    else:
        nb.tab(10, state="hidden")
def locatorChanged():
    if locatorExist.get() == 'On':
        nb.tab(11, state="normal")
    else:
        nb.tab(11, state="hidden")        
def sequenceChanged():
    if varioExist.get() == 'On':
        nb.tab(12, state="normal")
    else:
        nb.tab(12, state="hidden")

def analogVarioChanged():
    if analogVarioExist.get() == 'On':
        fAnalogVario.grid(row=8)
    else:    
        fAnalogVario.grid_remove()




root = Tk()
root.title("openXsensor")
nb = ttk.Notebook(root)
nb.enable_traversal()
fMain = ttk.Frame(nb)   # first page, which would get widgets gridded into it
#ttk.Label(fMain,  text="Components").grid( row=0 , pady=(10,2)) #column=0 , sticky=(N,W,E,S)


varioExist = StringVar(value='Off')
voltageExist = StringVar(value='Off')
currentExist = StringVar(value='Off')
gpsExist = StringVar(value='Off')
airspeedExist = StringVar(value='Off')
rpmExist = StringVar(value='Off')
imuExist = StringVar(value='Off')
magnetExist = StringVar(value='Off')
flowExist = StringVar(value='Off')
locatorExist = StringVar(value='Off')
adcExist = StringVar(value='Off')
sequenceExist = StringVar(value='Off')

protocolVar = StringVar(value='FRSKY_SPORT')
protocolBox = ttk.Combobox(fMain, textvariable=protocolVar, 
                values=('FRSKY_SPORT', 'JETI', 'HOTT', 'MULTIPLEX', 'FRSKY_HUB' , 'FRSKY_SPORT_HUB'))
protocolBox.bind('<<ComboboxSelected>>', protocolChanged)
protocolBox['state']='readonly'
protocolBox.grid(column=1, row=13 , pady=(20,2) )

ttk.Label(fMain, text="Pin used for telemetry").grid(column=0, row=14, pady=(5,2))
pinToRxVar = StringVar(value='4')
pinToRxBox = ttk.Combobox(fMain, textvariable=pinToRxVar, 
                values=('4', '2'), state='readonly',  width=2)
pinToRxBox.grid(column=1, row=14, pady=(5,2))




fVario = ttk.Frame(nb)   # page 1
fVoltage = ttk.Frame(nb) # page 2
fCurrent = ttk.Frame(nb) # page 3
fGps = ttk.Frame(nb) # page 4
fAirspeed = ttk.Frame(nb) # page 5
fRpm = ttk.Frame(nb) # page 6
fImu = ttk.Frame(nb) # page 7
fMagnet = ttk.Frame(nb) # page 8
fFlow = ttk.Frame(nb) # page 9
fAdc = ttk.Frame(nb) # page 10
fLocator = ttk.Frame(nb) # page 11
fSequence = ttk.Frame(nb) # page 12
fFrsky = ttk.Frame(nb) # page 13
fJeti = ttk.Frame(nb) # page 14
fHott = ttk.Frame(nb) # page 15
fMultiplex = ttk.Frame(nb) # page 16

varioTypeVar = StringVar(value='MS5611')

secondBaroExist = StringVar(value='Off')
varioSourceVar = StringVar(value='FIRST_BARO')
varioHysteresisVar = IntVar(value=5)


fAnalogVario = ttk.Frame(fVario)
analogVarioExist = StringVar(value='Off')
varioAnalogPinVar = StringVar(value='3')
varioAnalogMinVar = DoubleVar(value=-3)
varioAnalogMaxVar = DoubleVar(value=3)

volt1Exist = StringVar(value='Off')
volt2Exist = StringVar(value='Off')
volt3Exist = StringVar(value='Off')
volt4Exist = StringVar(value='Off')
volt5Exist = StringVar(value='Off')
volt6Exist = StringVar(value='Off')
volt1PinVar = StringVar(value='0')
volt2PinVar = StringVar(value='1')
volt3PinVar = StringVar(value='2')
volt4PinVar = StringVar(value='3')
volt5PinVar = StringVar(value='6')
volt6PinVar = StringVar(value='7')

volt1RgVar = DoubleVar(value='0.0')
volt2RgVar = DoubleVar(value='0.0')
volt3RgVar = DoubleVar(value='0.0')
volt4RgVar = DoubleVar(value='0.0')
volt5RgVar = DoubleVar(value='0.0')
volt6RgVar = DoubleVar(value='0.0')

volt1RvVar = DoubleVar(value='0.0')
volt2RvVar = DoubleVar(value='0.0')
volt3RvVar = DoubleVar(value='0.0')
volt4RvVar = DoubleVar(value='0.0')
volt5RvVar = DoubleVar(value='0.0')
volt6RvVar = DoubleVar(value='0.0')

volt1ScaleVar = DoubleVar(value='1.0')
volt2ScaleVar = DoubleVar(value='1.0')
volt3ScaleVar = DoubleVar(value='1.0')
volt4ScaleVar = DoubleVar(value='1.0')
volt5ScaleVar = DoubleVar(value='1.0')
volt6ScaleVar = DoubleVar(value='1.0')

volt1OffsetVar = DoubleVar(value='0.0')
volt2OffsetVar = DoubleVar(value='0.0')
volt3OffsetVar = DoubleVar(value='0.0')
volt4OffsetVar = DoubleVar(value='0.0')
volt5OffsetVar = DoubleVar(value='0.0')
volt6OffsetVar = DoubleVar(value='0.0')

voltNumberOfCellsVar = StringVar(value='0')
voltReferenceVar = StringVar(value='Internal')
voltRefValueVar = DoubleVar(value='5.0')

currentPinVar = StringVar(value='0')
currentMvoltAt0Var = DoubleVar(value='0.0')
currentMvoltPerAmpVar = DoubleVar(value='0.0')
currentRgVar = DoubleVar(value='0.0')
currentRcVar = DoubleVar(value='0.0')

airspeedTypeVar = StringVar()

gps3dExist = StringVar(value='Off')
gpsRateVar = StringVar(value='5')



frskyVfasVar= StringVar(value='Not sent')
frskyFuelVar= StringVar(value='Not sent')
frskyA3Var= StringVar(value='Not sent')
frskyA4Var= StringVar(value='Not sent')
frskyAccxVar= StringVar(value='Not sent')
frskyAccyVar= StringVar(value='Not sent')
frskyAcczVar= StringVar(value='Not sent')
frskyT1Var= StringVar(value='Not sent')
frskyT2Var= StringVar(value='Not sent')

multiplexLine1Var = StringVar(value='Not sent')
multiplexLine2Var = StringVar(value='Not sent')
multiplexLine3Var = StringVar(value='Not sent')
multiplexLine4Var = StringVar(value='Not sent')
multiplexLine5Var = StringVar(value='Not sent')
multiplexLine6Var = StringVar(value='Not sent')
multiplexLine7Var = StringVar(value='Not sent')
multiplexLine8Var = StringVar(value='Not sent')
multiplexLine9Var = StringVar(value='Not sent')

hottCellUndervoltageVar = DoubleVar(value=0)
hottBattery1SourceVar = StringVar(value='Not sent')
hottBattery2SourceVar = StringVar(value='Not sent')
hottMainBatterySourceVar = StringVar(value='Not sent')
hottT1SourceVar = StringVar(value='Not sent')
hottT2SourceVar = StringVar(value='Not sent')

jetiVoltageSourceVar = StringVar(value='Not sent')
jetiTemperatureSourceVar = StringVar(value='Not sent')
