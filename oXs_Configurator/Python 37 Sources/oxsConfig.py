from tkinter import StringVar , Tk , W ,E , S , BOTH, HIDDEN, DoubleVar , Spinbox , IntVar , filedialog
from tkinter import ttk 
from tkinter import messagebox

#from impExp import *
import configparser

#from oxsConfigurator import protocolChanged
#from oxsPpm import ppmTypeChanged

configUploadFileName = None

config =  configparser.ConfigParser()
config.add_section("Main")
config.add_section("Ppm")
config.add_section("Imu")
config.add_section("Magnet")
config.add_section("Vario")
config.add_section("Airspeed")
config.add_section("Voltage")
config.add_section("Current")
config.add_section("Ads")
config.add_section("Flow")
config.add_section("Rpm")
config.add_section("Gps")
config.add_section("Memory")
config.add_section("AddFields")
config.add_section("Frsky")
config.add_section("Multiplex")
config.add_section("Hott")
config.add_section("Jeti")

def generateOxsConfig():
    f=open("oXs_config_basic.h" , 'w')
    fa=open("oXs_config_advanced.h" , 'w')
    f.write("#ifndef OXS_CONFIG_BASIC_h\n")
    f.write("#define OXS_CONFIG_BASIC_h\n")
    f.write("\n")
    fa.write("#ifndef OXS_CONFIG_ADVANCED_h\n")
    fa.write("#define OXS_CONFIG_ADVANCED_h\n")
    fa.write("\n")
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

    if protocolVar.get() == 'JETI':
        if jetiVoltageSourceVar.get() != 'Not sent':
            f.write(f"#define VOLTAGE_SOURCE  {jetiVoltageSourceVar.get()}\n")
        if jetiTemperatureSourceVar.get() != 'Not sent':
            f.write(f"#define TEMPERATURE_SOURCE  {jetiTemperatureSourceVar.get()}\n")

    if protocolVar.get() == 'HOTT':
        if hottCellUndervoltageVar.get() > 0:
            f.write(f"#define CELL_UNDERVOLTAGE_WARNING  {hottCellUndervoltageVar.get()}\n")
        if  hottBattery1SourceVar.get() != 'Not sent':
            f.write(f"#define BATTERY_1_SOURCE   {hottBattery1SourceVar.get()}\n")
        if  hottBattery2SourceVar.get() != 'Not sent':
            f.write(f"#define BATTERY_2_SOURCE  {hottBattery2SourceVar.get()}\n")
        if  hottMainBatterySourceVar.get() != 'Not sent':
            f.write(f"#define MAIN_BATTERY_SOURCE  {hottMainBatterySourceVar.get()}\n")
        if  hottT1SourceVar.get() != 'Not sent':
            f.write(f"#define TEMPERATURE_1_SOURCE  {hottT1SourceVar.get()}\n")
        if  hottT2SourceVar.get() != 'Not sent':
            f.write(f"#define TEMPERATURE_2_SOURCE  {hottT2SourceVar.get()}\n")

    if protocolVar.get() == 'MULTIPLEX':
        if  (multiplexLine1Var.get() != 'Not sent') or (multiplexLine2Var.get() != 'Not sent') or (multiplexLine3Var.get() != 'Not sent') or \
            (multiplexLine4Var.get() != 'Not sent') or (multiplexLine5Var.get() != 'Not sent') or (multiplexLine6Var.get() != 'Not sent'):
            f.write("#define SETUP_MULTIPLEX_DATA_TO_SEND    ")
            firstLine = True
            if  multiplexLine1Var.get() != 'Not sent':
                firstLine = False
                f.write("\\")    
                f.write(f"\n                1, {multiplexLine1Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine2Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                2, {multiplexLine2Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine3Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                3, {multiplexLine3Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine4Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                4, {multiplexLine4Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine5Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                5, {multiplexLine5Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine6Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                6, {multiplexLine6Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine7Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                7, {multiplexLine7Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine8Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                    firstLine = False    
                f.write("\\")
                f.write(f"\n                8, {multiplexLine8Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            if  multiplexLine9Var.get() != 'Not sent':
                if firstLine == False:
                    f.write(" , ")
                else:
                     firstLine = False    
                f.write("\\")
                f.write(f"\n                9, {multiplexLine9Var.get()}  ,  1 , 1 , 0 , -16384 , 16383 ")
            f.write("\n")
        else:
            messagebox.showwarning("Warning","Generated files will be unvalid:\n for Multiplex protocol, you must specify the field(s) to be sent")   
    
    if addFieldsExist.get() == "On":
        if fillTest1VarioTemperatureVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_WITH_VARIO_TEMP\n") 
        if fillTest2VarioTemperatureVar.get() == "On":
            fa.write("\n#define FILL_TEST_2_WITH_VARIO_TEMP\n") 
        if fillTest3VarioTemperatureVar.get() == "On":
            fa.write("\n#define FILL_TEST_3_WITH_VARIO_TEMP\n") 
        if fillTest3ExpectedAltitudeVar.get() == "On":
            fa.write("\n#define FILL_TEST_3_WITH_EXPECTED_ALT\n") 
            fa.write(f"\n#define EXPECTED_ALT_AT_SEC {str(expAltTimeVar.get())}\n")
        if fillTest123LinearAccVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_2_3_WITH_LINEAR_ACC\n") 
        if fillTest12VspeedAltVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO\n") 
        if fillTest1DteVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_WITH_DTE\n") 
        if fillTest2PpmAirspeedCompVar.get() == "On":
            fa.write("\n#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION\n") 
        if fillTest1YawRateVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_WITH_YAWRATE\n") 
        if fillTest1HeadingVar.get() == "On":
            fa.write("\n#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER\n") 
        if fillTest123FlowVar.get() == "On":
            fa.write("\n#define FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION\n") 
        if fillTest1GpsNbrSatVar.get() == "On":
            fa.write("\n#define FILL_TEST1_WITH_GPS_NUMBER_OF_SAT\n")
        if fillTest2GpsHdopVar.get() == "On":
            fa.write("\n#define FILL_TEST2_WITH_GPS_HDOP\n")
    
    if ppmExist.get() == "On":
        if ppmTypeVar.get() == 'Rx chanel':
            fa.write(f"\n#define PIN_PPM {str(ppmPinVar.get())}\n")
            fa.write(f"#define PPM_MIN_100 {str(ppmForMin100Var.get())}\n")
            fa.write(f"#define PPM_PLUS_100 {str(ppmForPlus100Var.get())}\n")
        else:
            fa.write("\n#define PPM_VIA_SPORT \n")

    if persistentExist.get() == "On":
        f.write("\n#define SAVE_TO_EEPROM YES\n")
        f.write(f"\n#define PIN_PUSHBUTTON {str(pushButtPinVar.get())}\n")
    else:
        f.write("\n#define SAVE_TO_EEPROM NO\n") 

    if varioExist.get() == "On":
        f.write(f"#define FIRST_BARO_SENSOR_USE {varioTypeVar.get()}\n")
        if varioTypeVar.get() == 'BMP280':
            fa.write("#define BMP280_ADR 0x76\n")
        f.write(f"#define VSPEED_SOURCE {varioSourceVar.get()}\n")
        if secondBaroExist.get() == 'On':
            fa.write("\n#define SECOND_BARO_SENSOR_USE    MS5611\n")
        else:
            fa.write("\n#define SECOND_BARO_SENSOR_USE     NO_BARO\n")
        fa.write("\n#define SENSITIVITY_MIN 80\n")
        fa.write("#define SENSITIVITY_MAX 300\n")
        fa.write("#define SENSITIVITY_MIN_AT 100\n")
        fa.write("#define SENSITIVITY_MAX_AT 1000\n")
        if ppmExist.get() == "On":
            fa.write("\n#define SENSITIVITY_MIN_AT_PPM 10\n")
            fa.write("#define SENSITIVITY_MAX_AT_PPM 40\n")
            fa.write("#define SENSITIVITY_PPM_MIN  20\n")
            fa.write("#define SENSITIVITY_PPM_MAX 100\n")
        fa.write(f"\n#define VARIOHYSTERESIS {str(varioHysteresisVar.get())}\n")
        if analogVarioExist.get() == "On":
            fa.write(f"\n#define PIN_ANALOG_VSPEED {varioAnalogPinVar.get()}\n")
            fa.write(f"#define ANALOG_VSPEED_MIN  {varioAnalogMinVar.get()}\n")
            fa.write(f"#define ANALOG_VSPEED_MAX  {varioAnalogMaxVar.get()}\n")
    else:
        f.write("#define FIRST_BARO_SENSOR_USE NO_BARO\n")

    if airspeedExist.get() == "On":
        f.write(f"#define AIRSPEED_SENSOR_USE {airspeedTypeVar.get()}\n")
        if airspeedAtSea15CVar == "On":
            fa.write("\n#define AIRSPEED_AT_SEA_LEVEL_AND_15C\n")
        if airspeedInKmhVar == "On":
            fa.write("\n#define AIRSPEED_IN_KMH\n")
        fa.write(f"\n#define AIRSPEED_RESET_AT_PPM {airspeedResetAtPpmVar.get()}\n")
        fa.write(f"\n#define COMPENSATION_MIN_AT_PPM {airspeedCompMinAtPpmVar.get()}\n")
        fa.write(f"\n#define COMPENSATION_MAX_AT_PPM {airspeedCompMaxAtPpmVar.get()}\n")
        fa.write(f"\n#define COMPENSATION_PPM_MIN {airspeedCompMinVar.get()}\n")
        fa.write(f"\n#define COMPENSATION_PPM_MAX {airspeedCompMaxVar.get()}\n")
    else:
        f.write("\n#define AIRSPEED_SENSOR_USE NO_AIRSPEED\n")
    
    if voltageExist.get() == "On":
        f.write("\n#define ARDUINO_MEASURES_VOLTAGES   YES\n")
        f.write(f"#define NUMBEROFCELLS {voltNumberOfCellsVar.get()}\n")
        if voltReferenceVar.get() == "Internal":
            fa.write("\n#define USE_INTERNAL_REFERENCE\n")
        elif voltReferenceVar.get() == "External":
            fa.write("\n#define USE_EXTERNAL_REFERENCE\n")
            fa.write(f"\n#define REFERENCE_VOLTAGE {str(voltRefValueVar.get())}\n")
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
        if (Adc0TypeVar.get() != "ADS_OFF") or (Adc1TypeVar.get() != "ADS_OFF") or (Adc2TypeVar.get() != "ADS_OFF") or (Adc3TypeVar.get() != "ADS_OFF"):
            f.write("\n#define AN_ADS1115_IS_CONNECTED YES\n")
            fa.write(f"\n#define ADS_MEASURE {Adc0TypeVar.get()}, {Adc1TypeVar.get()}, {Adc2TypeVar.get()}, {Adc3TypeVar.get()}\n")
            fa.write(f"#define ADS_FULL_SCALE_VOLT {Adc0FsVar.get()}, {Adc1FsVar.get()}, {Adc2FsVar.get()}, {Adc3FsVar.get()}\n")
            fa.write(f"#define ADS_OFFSET {str(Adc0OffVar.get())}, {str(Adc1OffVar.get())}, {str(Adc2OffVar.get())}, {str(Adc3OffVar.get())}\n")
            fa.write(f"#define ADS_SCALE {str(Adc0ScVar.get())}, {str(Adc1ScVar.get())}, {str(Adc2ScVar.get())}, {str(Adc3ScVar.get())}\n")
            fa.write(f"#define ADS_RATE {Adc0CrVar.get()}, {Adc1CrVar.get()}, {Adc2CrVar.get()}, {Adc3CrVar.get()}\n")
            if Adc0AvgVar.get()<1:
                Adc0AvgVar.set(1)
            if Adc1AvgVar.get()<1:
                Adc1AvgVar.set(1)
            if Adc2AvgVar.get()<1:
                Adc2AvgVar.set(1)
            if Adc3AvgVar.get()<1:
                Adc3AvgVar.set(1)
            if Adc0AvgVar.get()>254:
                Adc0AvgVar.set(254)
            if Adc1AvgVar.get()>254:
                Adc1AvgVar.set(254)
            if Adc2AvgVar.get()>254:
                Adc2AvgVar.set(254)
            if Adc3AvgVar.get()>254:
                Adc3AvgVar.set(254)
            fa.write(f"#define ADS_AVERAGING_ON {str(Adc0AvgVar.get())}, {str(Adc1AvgVar.get())}, {str(Adc2AvgVar.get())}, {str(Adc3AvgVar.get())}\n")
            if AdcCurrVar.get() != "NO":
                fa.write(f"#define ADS_CURRENT_BASED_ON {AdcCurrVar.get()}\n")
            if AdcSpVar.get() != "NO":
                fa.write(f"#define ADS_AIRSPEED_BASED_ON {AdcSpVar.get()}\n")
        else:
            f.write("\n#define AN_ADS1115_IS_CONNECTED NO\n")
    else:
        f.write("\n#define AN_ADS1115_IS_CONNECTED NO\n")

    if rpmExist.get() == "On":
        f.write("\n#define CALCULATE_RPM YES\n")
        fa.write(f"\n#define PULSES_PER_ROTATION {pulsesPerRotationVar.get()}\n")
    else:
        f.write("\n#define CALCULATE_RPM NO\n")

    if gpsExist.get() == "On":
        f.write("\n#define A_GPS_IS_CONNECTED YES\n")
        if gpsTimeExist.get() == 'On':
            fa.write("\n#define GPS_TRANSMIT_TIME\n")
        if gps3dExist.get() == 'On':
            fa.write("\n#define GPS_SPEED_3D\n")
            fa.write(f"#define GPS_REFRESH_RATE {gpsRateVar.get()}\n")
        else:
            fa.write(f"\n#define GPS_REFRESH_RATE {gpsRateVar.get()}\n")  
    else:
        f.write("\n#define A_GPS_IS_CONNECTED NO\n")

    if imuExist.get() == "On":
        f.write("\n#define A_MPU6050_IS_CONNECTED YES\n")
        fa.write(f"\n#define PIN_INT_6050 {str(imuPinVar.get())}\n")
        if imuDisplayOffset.get() == "On":
            fa.write("\n#define DISPLAY_ACC_OFFSET\n")
            fa.write("#define ACC_OFFSET_X 0\n")
            fa.write("#define ACC_OFFSET_Y 0\n")
            fa.write("#define ACC_OFFSET_Z 0\n")
        else:
        
            fa.write(f"#define ACC_OFFSET_X {str(imuOffsetXVar.get())}\n")
            fa.write(f"#define ACC_OFFSET_Y {str(imuOffsetYVar.get())}\n")
            fa.write(f"#define ACC_OFFSET_Z {str(imuOffsetZVar.get())}\n")
    else:
        f.write("\n#define A_MPU6050_IS_CONNECTED NO\n")

    if magnetExist.get() == "On":
        if GeneratemagCalData.get() == "On":
            fa.write("\n#define GENERATE_MAG_CALIBRATION_DATA\n")
            fa.write("#define XMAG_OFFSET 0\n")
            fa.write("#define YMAG_OFFSET 0\n")
            fa.write("#define ZMAG_OFFSET 0\n")
            fa.write("#define XXMAG_CORRECTION 1\n")
            fa.write("#define XYMAG_CORRECTION 0\n")
            fa.write("#define XZMAG_CORRECTION 0\n")
            fa.write("#define YXMAG_CORRECTION 0\n")
            fa.write("#define YYMAG_CORRECTION 1\n")
            fa.write("#define YZMAG_CORRECTION 0\n")
            fa.write("#define ZXMAG_CORRECTION 0\n")
            fa.write("#define ZYMAG_CORRECTION 0\n")
            fa.write("#define ZZMAG_CORRECTION 1\n")
        else:
            fa.write(f"#define XMAG_OFFSET {str(xMagOffsetVar.get())}\n")
            fa.write(f"#define YMAG_OFFSET {str(yMagOffsetVar.get())}\n")
            fa.write(f"#define ZMAG_OFFSET {str(zMagOffsetVar.get())}\n")
            fa.write(f"#define XXMAG_CORRECTION {str(xxMagCorrectionVar.get())}\n")
            fa.write(f"#define XYMAG_CORRECTION {str(xyMagCorrectionVar.get())}\n")
            fa.write(f"#define XZMAG_CORRECTION {str(xzMagCorrectionVar.get())}\n")
            fa.write(f"#define YXMAG_CORRECTION {str(yxMagCorrectionVar.get())}\n")
            fa.write(f"#define YYMAG_CORRECTION {str(yyMagCorrectionVar.get())}\n")
            fa.write(f"#define YZMAG_CORRECTION {str(yzMagCorrectionVar.get())}\n")
            fa.write(f"#define ZXMAG_CORRECTION {str(zxMagCorrectionVar.get())}\n")
            fa.write(f"#define ZYMAG_CORRECTION {str(zyMagCorrectionVar.get())}\n")
            fa.write(f"#define ZZMAG_CORRECTION {str(zzMagCorrectionVar.get())}\n")
        f.write("\n#define CALCULATE_YAW_WITH_HMC5883 YES\n")

    else:
        f.write("\n#define CALCULATE_YAW_WITH_HMC5883 NO\n")

    if flowExist.get() == "On":
        f.write("\n#define A_FLOW_SENSOR_IS_CONNECTED   YES\n")
        fa.write(f"#define PULSES_PER_ML {str(flowPulsesmlVar.get())}\n")
        fa.write(f"#define TANK_CAPACITY {str(flowTankCapVar.get())}\n")
        fa.write(f"#define INIT_FLOW_PARAM {str(flowL1ValueVar.get())} , {str(flowL2ValueVar.get())} , {str(flowL3ValueVar.get())} , {str(flowL4ValueVar.get())} , {str(flowL1CorrVar.get())} , {str(flowL2CorrVar.get())} , {str(flowL3CorrVar.get())} , {str(flowL4CorrVar.get())}\n")
        fa.write(f"##define FLOW_SENSOR_RESET_AT_PPM {str(flowPPMResetVar.get())}\n")
    else:
        f.write("\n#define A_FLOW_SENSOR_IS_CONNECTED   NO\n")
    
    if sequenceExist.get() == 'On':
        if SeqOut1Var.get()=='1' or SeqOut2Var.get()=='1' or SeqOut3Var.get()=='1' or SeqOut4Var.get()=='1' or SeqOut5Var.get()=='1' or SeqOut6Var.get()=='1':
            fa.write(f"#define SEQUENCE_OUTPUTS 0b{SeqOut6Var.get()}{SeqOut5Var.get()}{SeqOut4Var.get()}{SeqOut3Var.get()}{SeqOut2Var.get()}{SeqOut1Var.get()}\n")
            fa.write(f"#define SEQUENCE_UNIT    {str(SeqUnitVar.get())}\n")
            if SeqTVar[0].get()!='0' and SeqTVar[0].get()!='':
                fa.write("#define SEQUENCE_m100 ")
                for y in range (0,25):
                    if SeqTVar[y*4].get()=='':
                        break
                    if y!=0:
                        fa.write(",")
                    fa.write(f"{SeqTVar[y*4].get()}, 0b{str(SeqB5Var[y*4].get())}{str(SeqB4Var[y*4].get())}{str(SeqB3Var[y*4].get())}{str(SeqB2Var[y*4].get())}{str(SeqB1Var[y*4].get())}{str(SeqB0Var[y*4].get())}")
                fa.write("\n")
            if SeqTVar[1].get()!='0' and SeqTVar[1].get()!='':
                fa.write("#define SEQUENCE_0 ")
                for y in range (0,25):
                    if SeqTVar[y*4+1].get()=='':
                        break
                    if y!=0:
                        fa.write(",")
                    fa.write(f"{SeqTVar[y*4+1].get()}, 0b{str(SeqB5Var[y*4+1].get())}{str(SeqB4Var[y*4+1].get())}{str(SeqB3Var[y*4+1].get())}{str(SeqB2Var[y*4+1].get())}{str(SeqB1Var[y*4+1].get())}{str(SeqB0Var[y*4+1].get())}")
                fa.write("\n")
            if SeqTVar[2].get()!='0' and SeqTVar[2].get()!='':
                fa.write("#define SEQUENCE_100 ")
                for y in range (0,25):
                    if SeqTVar[y*4+2].get()=='':
                        break
                    if y!=0:
                        fa.write(",")
                    fa.write(f"{SeqTVar[y*4+2].get()}, 0b{str(SeqB5Var[y*4+2].get())}{str(SeqB4Var[y*4+2].get())}{str(SeqB3Var[y*4+2].get())}{str(SeqB2Var[y*4+2].get())}{str(SeqB1Var[y*4+2].get())}{str(SeqB0Var[y*4+2].get())}")
                fa.write("\n")
            if voltageExist.get() == "On":
                if SeqTVar[3].get()!='0' and SeqTVar[3].get()!='' and (SeqMinVoltVar.get()!=0 or SeqMinCellVar.get()!=0):
                    fa.write("#define SEQUENCE_LOW ")
                    for y in range (0,25):
                        if SeqTVar[y*4+3].get()=='':
                            break
                        if y!=0:
                            fa.write(",")
                        fa.write(f"{SeqTVar[y*4+3].get()}, 0b{str(SeqB5Var[y*4+3].get())}{str(SeqB4Var[y*4+3].get())}{str(SeqB3Var[y*4+3].get())}{str(SeqB2Var[y*4+3].get())}{str(SeqB1Var[y*4+3].get())}{str(SeqB0Var[y*4+3].get())}")
                    fa.write("\n")
                    if (SeqMinVoltVar.get()!=0):
                        fa.write(f"#define SEQUENCE_MIN_VOLT_1 {str(SeqMinVoltVar.get())}")
                        messagebox.showinfo("oXs configurator", "Be carefull you may need to modify oXs_config_advanced.h to set the voltage source for sequence")
                    if (SeqMinCellVar.get()!=0):
                        fa.write(f"#define SEQUENCE_MIN_CELL {str(SeqMinCellVar.get())}")
    if locatorExist.get() == "On":
        f.write("\n#define A_LOCATOR_IS_CONNECTED   YES\n")
    else:
        f.write("\n#define A_LOCATOR_IS_CONNECTED   NO\n")
    
    

    f.write("\n#endif\n")
    fa.write("\n#endif\n") 
    f.close()
    fa.close()
    messagebox.showinfo("oXs configurator", "Config files advanced and basic have been created")

def uploadConfig():   
    from oxsVoltage import voltReferenceChanged
    print("Read config")
    #global configUploadFileName
    #configUploadFileName = filedialog.askopenfilename(initialdir="C:/Data/",
    configUploadFileName = filedialog.askopenfilename(\
                           filetypes =(("Ini files", "*.ini"),("All files","*.*")),
                           title = "Choose a file."
                           )
    #config.read('config.ini')
    config.read(configUploadFileName)
    ppmExist.set(value= config.get('Main', 'ppmExist'))
    ppmChanged()
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
    persistentExist.set(value= config.get("Main", "persistentExist"))
    persistentChanged()
    addFieldsExist.set(value= config.get("Main", "addFieldsExist"))
    addFieldsChanged()
    protocolVar.set(value= config.get("Main", "protocolVar"))
    protocolChanged()

    ppmTypeVar.set(value= config.get("Ppm", "ppmTypeVar"))
    ppmTypeChanged()
    ppmPinVar.set(value= config.getint("Ppm", "ppmPinVar"))
    ppmForMin100Var.set(value= config.getint("Ppm", "ppmForMin100Var"))
    ppmForPlus100Var.set(value= config.getint("Ppm", "ppmForPlus100Var"))

    imuPinVar.set(value= config.getint("Imu", "imuPinVar"))
    imuDisplayOffset.set(value= config.get("Imu", "imuDisplayOffset"))
    imuOffsetXVar.set(value= config.getint("Imu", "imuOffsetXVar"))
    imuOffsetYVar.set(value= config.getint("Imu", "imuOffsetYVar"))
    imuOffsetZVar.set(value= config.getint("Imu", "imuOffsetZVar"))

    GeneratemagCalData.set(value= config.get("Magnet", "GeneratemagCalData"))
    xMagOffsetVar.set(value= config.getint("Magnet", "xMagOffsetVar"))
    yMagOffsetVar.set(value= config.getint("Magnet", "yMagOffsetVar"))
    zMagOffsetVar.set(value= config.getint("Magnet", "zMagOffsetVar"))
    xxMagCorrectionVar.set(value= config.getfloat("Magnet", "xxMagCorrectionVar"))
    xyMagCorrectionVar.set(value= config.getfloat("Magnet", "xyMagCorrectionVar"))
    xzMagCorrectionVar.set(value= config.getfloat("Magnet", "xzMagCorrectionVar"))
    yxMagCorrectionVar.set(value= config.getfloat("Magnet", "yxMagCorrectionVar"))
    yyMagCorrectionVar.set(value= config.getfloat("Magnet", "yyMagCorrectionVar"))
    yzMagCorrectionVar.set(value= config.getfloat("Magnet", "yzMagCorrectionVar"))
    zxMagCorrectionVar.set(value= config.getfloat("Magnet", "zxMagCorrectionVar"))
    zyMagCorrectionVar.set(value= config.getfloat("Magnet", "zyMagCorrectionVar"))
    zzMagCorrectionVar.set(value= config.getfloat("Magnet", "zzMagCorrectionVar"))
    
    pushButtPinVar.set(value= config.getint("Memory", "pushButtPinVar"))

    varioTypeVar.set(value= config.get("Vario", "varioTypeVar"))
    secondBaroExist.set(value= config.get("Vario", "secondBaroExist"))
    varioSourceVar.set(value= config.get("Vario", "varioSourceVar"))
    varioHysteresisVar.set(value= config.getint("Vario", "varioHysteresisVar"))
    analogVarioExist.set(value= config.get("Vario", "analogVarioExist"))
    analogVarioChanged()
    varioAnalogPinVar.set(value= config.get("Vario", "varioAnalogPinVar"))
    varioAnalogMinVar.set(value= config.getfloat("Vario", "varioAnalogMinVar"))
    varioAnalogMaxVar.set(value= config.getfloat("Vario", "varioAnalogMaxVar"))
    
    airspeedTypeVar.set(value= config.get("Airspeed", "airspeedTypeVar"))
    airspeedAtSea15CVar.set(value= config.get("Airspeed", "airspeedAtSea15CVar"))
    airspeedInKmhVar.set(value= config.get("Airspeed", "airspeedInKmhVar"))
    airspeedResetAtPpmVar.set(value= config.getint("Airspeed", "airspeedResetAtPpmVar"))
    airspeedCompMinAtPpmVar.set(value= config.getint("Airspeed", "airspeedCompMinAtPpmVar"))
    airspeedCompMaxAtPpmVar.set(value= config.getint("Airspeed", "airspeedCompMaxAtPpmVar"))
    airspeedCompMinVar.set(value= config.getint("Airspeed", "airspeedCompMinVar"))
    airspeedCompMaxVar.set(value= config.getint("Airspeed", "airspeedCompMaxVar"))

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
    voltReferenceVar.set(value= config.get("Voltage", "voltReferenceVar"))
    voltRefValueVar.set(value= config.getfloat("Voltage", "voltRefValueVar"))
    voltReferenceChanged()


    currentPinVar.set(value= config.get("Current", "currentPinVar"))
    currentMvoltAt0Var.set(value= config.getfloat("Current", "currentMvoltAt0Var"))
    currentMvoltPerAmpVar.set(value= config.getfloat("Current", "currentMvoltPerAmpVar"))
    currentRgVar.set(value= config.getfloat("Current", "currentRgVar"))
    currentRcVar.set(value= config.getfloat("Current", "currentRcVar"))

    flowPulsesmlVar.set(value= config.getint("Flow", "flowPulsesmlVar"))
    flowTankCapVar.set(value= config.getint("Flow", "flowTankCapVar"))
    flowL1ValueVar.set(value= config.getint("Flow", "flowL1ValueVar"))
    flowL1CorrVar.set(value= config.getint("Flow", "flowL1CorrVar"))
    flowL2ValueVar.set(value= config.getint("Flow", "flowL2ValueVar"))
    flowL2CorrVar.set(value= config.getint("Flow", "flowL2CorrVar"))
    flowL3ValueVar.set(value= config.getint("Flow", "flowL3ValueVar"))
    flowL3CorrVar.set(value= config.getint("Flow", "flowL3CorrVar"))
    flowL4ValueVar.set(value= config.getint("Flow", "flowL4ValueVar"))
    flowL4CorrVar.set(value= config.getint("Flow", "flowL4CorrVar"))
    flowPPMResetVar.set(value= config.getint("Flow", "flowPPMResetVar"))

    pulsesPerRotationVar.set(value=config.get("Rpm","pulsesPerRotationVar"))

    Adc0TypeVar.set(value= config.get("Ads", "Adc0TypeVar"))
    Adc1TypeVar.set(value= config.get("Ads", "Adc1TypeVar"))
    Adc2TypeVar.set(value= config.get("Ads", "Adc2TypeVar"))
    Adc3TypeVar.set(value= config.get("Ads", "Adc3TypeVar"))
    Adc0FsVar.set(value= config.get("Ads", "Adc0FsVar"))
    Adc1FsVar.set(value= config.get("Ads", "Adc1FsVar"))
    Adc2FsVar.set(value= config.get("Ads", "Adc2FsVar"))
    Adc3FsVar.set(value= config.get("Ads", "Adc3FsVar"))
    Adc0OffVar.set(value= config.getint("Ads", "Adc0OffVar"))
    Adc1OffVar.set(value= config.getint("Ads", "Adc1OffVar"))
    Adc2OffVar.set(value= config.getint("Ads", "Adc2OffVar"))
    Adc3OffVar.set(value= config.getint("Ads", "Adc3OffVar"))
    Adc0ScVar.set(value= config.getfloat("Ads", "Adc0ScVar"))
    Adc1ScVar.set(value= config.getfloat("Ads", "Adc1ScVar"))
    Adc2ScVar.set(value= config.getfloat("Ads", "Adc2ScVar"))
    Adc3ScVar.set(value= config.getfloat("Ads", "Adc3ScVar"))
    Adc0CrVar.set(value= config.get("Ads", "Adc0CrVar"))
    Adc1CrVar.set(value= config.get("Ads", "Adc1CrVar"))
    Adc2CrVar.set(value= config.get("Ads", "Adc2CrVar"))
    Adc3CrVar.set(value= config.get("Ads", "Adc3CrVar"))
    Adc0AvgVar.set(value= config.getint("Ads", "Adc0AvgVar"))
    Adc1AvgVar.set(value= config.getint("Ads", "Adc1AvgVar"))
    Adc2AvgVar.set(value= config.getint("Ads", "Adc2AvgVar"))
    Adc3AvgVar.set(value= config.getint("Ads", "Adc3AvgVar"))
    AdcCurrVar.set(value= config.get("Ads", "AdcCurrVar"))
    AdcSpVar.set(value= config.get("Ads", "AdcSpVar"))

    gps3dExist.set(value= config.get("Gps", "gps3dexist"))
    gpsTimeExist.set(value= config.get("Gps", "gpstimeexist"))
    gpsRateVar.set(value= config.getint("Gps", "gpsratevar"))
    fillTest1VarioTemperatureVar.set(value= config.get("AddFields", "fillTest1VarioTemperatureVar"))
    fillTest2VarioTemperatureVar.set(value= config.get("AddFields", "fillTest2VarioTemperatureVar"))
    fillTest3VarioTemperatureVar.set(value= config.get("AddFields", "fillTest3VarioTemperatureVar"))
    fillTest3ExpectedAltitudeVar.set(value= config.get("AddFields", "fillTest3ExpectedAltitudeVar"))
    fillTest3ExpectedAltitudeChanged()
    expAltTimeVar.set(value= config.getfloat("AddFields", "expAltTimeVar"))
    fillTest123LinearAccVar.set(value= config.get("AddFields", "fillTest123LinearAccVar"))
    fillTest12VspeedAltVar.set(value= config.get("AddFields", "fillTest12VspeedAltVar"))
    fillTest1DteVar.set(value= config.get("AddFields", "fillTest1DteVar"))
    fillTest2PpmAirspeedCompVar.set(value= config.get("AddFields", "fillTest2PpmAirspeedCompVar"))
    fillTest1YawRateVar.set(value= config.get("AddFields", "fillTest1YawRateVar"))
    fillTest1HeadingVar.set(value= config.get("AddFields", "fillTest1HeadingVar"))
    fillTest123FlowVar.set(value= config.get("AddFields", "fillTest123FlowVar"))
    fillTest1GpsNbrSatVar.set(value= config.get("AddFields", "fillTest1GpsNbrSatVar"))
    fillTest2GpsHdopVar.set(value= config.get("AddFields", "fillTest2GpsHdopVar"))

    frskyVfasVar.set(value= config.get("Frsky", "frskyVfasVar"))
    frskyFuelVar.set(value= config.get("Frsky", "frskyFuelVar"))
    frskyA3Var.set(value= config.get("Frsky", "frskyA3Var"))
    frskyA4Var.set(value= config.get("Frsky", "frskyA4Var"))
    frskyAccxVar.set(value= config.get("Frsky", "frskyAccxVar"))
    frskyAccyVar.set(value= config.get("Frsky", "frskyAccyVar"))
    frskyAcczVar.set(value= config.get("Frsky", "frskyAcczVar"))
    frskyT1Var.set(value= config.get("Frsky", "frskyT1Var"))
    frskyT2Var.set(value= config.get("Frsky", "frskyT2Var"))

    multiplexLine1Var.set(value= config.get("Multiplex", "multiplexLine1Var"))
    multiplexLine2Var.set(value= config.get("Multiplex", "multiplexLine2Var"))
    multiplexLine3Var.set(value= config.get("Multiplex", "multiplexLine3Var"))
    multiplexLine4Var.set(value= config.get("Multiplex", "multiplexLine4Var"))
    multiplexLine5Var.set(value= config.get("Multiplex", "multiplexLine5Var"))
    multiplexLine6Var.set(value= config.get("Multiplex", "multiplexLine6Var"))
    multiplexLine7Var.set(value= config.get("Multiplex", "multiplexLine7Var"))
    multiplexLine8Var.set(value= config.get("Multiplex", "multiplexLine8Var"))
    multiplexLine9Var.set(value= config.get("Multiplex", "multiplexLine9Var"))

    hottBattery1SourceVar.set(value= config.get("Hott", "hottBattery1SourceVar"))
    hottBattery2SourceVar.set(value= config.get("Hott", "hottBattery2SourceVar"))
    hottCellUndervoltageVar.set(value= config.getint("Hott", "hottCellUndervoltageVar"))
    hottMainBatterySourceVar.set(value= config.get("Hott", "hottMainBatterySourceVar"))
    hottT1SourceVar.set(value= config.get("Hott", "hottT1SourceVar"))
    hottT2SourceVar.set(value= config.get("Hott", "hottT2SourceVar"))

    jetiVoltageSourceVar.set(value= config.get("Jeti", "jetiVoltageSourceVar"))
    jetiTemperatureSourceVar.set(value= config.get("Jeti", "jetiTemperatureSourceVar"))

def saveConfig():   
    config.set("Main", "ppmExist", ppmExist.get())
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
    config.set("Main", "persistentExist", persistentExist.get())
    
    config.set("Main", "addFieldsExist", addFieldsExist.get())
    config.set("Main", "protocolVar", protocolVar.get())
    
    config.set("Ppm", "ppmTypeVar", ppmTypeVar.get())
    config.set("Ppm", "ppmPinVar", str(ppmPinVar.get()))
    config.set("Ppm", "ppmForMin100Var", str(ppmForMin100Var.get()))
    config.set("Ppm", "ppmForPlus100Var", str(ppmForPlus100Var.get()))

    config.set("Imu", "imuPinVar", str(imuPinVar.get()))
    config.set("Imu", "imuDisplayOffset", imuDisplayOffset.get())
    config.set("Imu", "imuOffsetXVar", str(imuOffsetXVar.get()))
    config.set("Imu", "imuOffsetYVar", str(imuOffsetYVar.get()))
    config.set("Imu", "imuOffsetZVar", str(imuOffsetZVar.get()))

    config.set("Magnet", "GeneratemagCalData", GeneratemagCalData.get())
    config.set("Magnet", "xMagOffsetVar", str(xMagOffsetVar.get()))
    config.set("Magnet", "yMagOffsetVar", str(yMagOffsetVar.get()))
    config.set("Magnet", "zMagOffsetVar", str(zMagOffsetVar.get()))
    config.set("Magnet", "xxMagCorrectionVar", str(xxMagCorrectionVar.get()))
    config.set("Magnet", "xyMagCorrectionVar", str(xyMagCorrectionVar.get()))
    config.set("Magnet", "xzMagCorrectionVar", str(xzMagCorrectionVar.get()))
    config.set("Magnet", "yxMagCorrectionVar", str(xxMagCorrectionVar.get()))
    config.set("Magnet", "yyMagCorrectionVar", str(yyMagCorrectionVar.get()))
    config.set("Magnet", "yzMagCorrectionVar", str(yzMagCorrectionVar.get()))
    config.set("Magnet", "zxMagCorrectionVar", str(zxMagCorrectionVar.get()))
    config.set("Magnet", "zyMagCorrectionVar", str(zyMagCorrectionVar.get()))
    config.set("Magnet", "zzMagCorrectionVar", str(zzMagCorrectionVar.get()))
 
    config.set("Memory", "pushButtPinVar", str(pushButtPinVar.get()))
 
    config.set("Vario", "varioTypeVar", varioTypeVar.get())
    config.set("Vario", "secondBaroExist", secondBaroExist.get())
    config.set("Vario", "varioSourceVar", varioSourceVar.get())
    config.set("Vario", "varioHysteresisVar", str(varioHysteresisVar.get()))
    config.set("Vario", "analogVarioExist", analogVarioExist.get())
    config.set("Vario", "varioAnalogPinVar", varioAnalogPinVar.get())
    config.set("Vario", "varioAnalogMinVar", str(varioAnalogMinVar.get()))
    config.set("Vario", "varioAnalogMaxVar", str(varioAnalogMaxVar.get()))

    config.set("Airspeed", "airspeedTypeVar", airspeedTypeVar.get())
    config.set("Airspeed", "airspeedAtSea15CVar", airspeedAtSea15CVar.get())
    config.set("Airspeed", "airspeedInKmhVar", airspeedInKmhVar.get())
    config.set("Airspeed", "airspeedResetAtPpmVar", str(airspeedResetAtPpmVar.get()))
    config.set("Airspeed", "airspeedCompMinAtPpmVar", str(airspeedCompMinAtPpmVar.get()))
    config.set("Airspeed", "airspeedCompMaxAtPpmVar", str(airspeedCompMaxAtPpmVar.get()))
    config.set("Airspeed", "airspeedCompMinVar", str(airspeedCompMinVar.get()))
    config.set("Airspeed", "airspeedCompMaxVar", str(airspeedCompMaxVar.get()))
    
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

    config.set("Flow", "flowPulsesmlVar", str(flowPulsesmlVar.get()))
    config.set("Flow", "flowTankCapVar", str(flowTankCapVar.get()))
    config.set("Flow", "flowL1ValueVar", str(flowL1ValueVar.get()))
    config.set("Flow", "flowL1CorrVar", str(flowL1CorrVar.get()))
    config.set("Flow", "flowL2ValueVar", str(flowL2ValueVar.get()))
    config.set("Flow", "flowL2CorrVar", str(flowL2CorrVar.get()))
    config.set("Flow", "flowL3ValueVar", str(flowL3ValueVar.get()))
    config.set("Flow", "flowL3CorrVar", str(flowL3CorrVar.get()))
    config.set("Flow", "flowL4ValueVar", str(flowL4ValueVar.get()))
    config.set("Flow", "flowL4CorrVar", str(flowL4CorrVar.get()))
    config.set("Flow", "flowPPMResetVar", str(flowPPMResetVar.get()))

    config.set("Rpm", "pulsesPerRotationVar", str(pulsesPerRotationVar.get()))

    config.set("Ads", "Adc0TypeVar", Adc0TypeVar.get())
    config.set("Ads", "Adc1TypeVar", Adc1TypeVar.get())
    config.set("Ads", "Adc2TypeVar", Adc2TypeVar.get())
    config.set("Ads", "Adc3TypeVar", Adc3TypeVar.get())
    config.set("Ads", "Adc0FsVar", Adc0FsVar.get())
    config.set("Ads", "Adc1FsVar", Adc1FsVar.get())
    config.set("Ads", "Adc2FsVar", Adc2FsVar.get())
    config.set("Ads", "Adc3FsVar", Adc3FsVar.get())
    config.set("Ads", "Adc0OffVar", str(Adc0OffVar.get()))
    config.set("Ads", "Adc1OffVar", str(Adc1OffVar.get()))
    config.set("Ads", "Adc2OffVar", str(Adc2OffVar.get()))
    config.set("Ads", "Adc3OffVar", str(Adc3OffVar.get()))
    config.set("Ads", "Adc0ScVar", str(Adc0ScVar.get()))
    config.set("Ads", "Adc1ScVar", str(Adc1ScVar.get()))
    config.set("Ads", "Adc2ScVar", str(Adc2ScVar.get()))
    config.set("Ads", "Adc3ScVar", str(Adc3ScVar.get()))
    config.set("Ads", "Adc0CrVar", Adc0CrVar.get())
    config.set("Ads", "Adc1CrVar", Adc1CrVar.get())
    config.set("Ads", "Adc2CrVar", Adc2CrVar.get())
    config.set("Ads", "Adc3CrVar", Adc3CrVar.get())
    config.set("Ads", "Adc0AvgVar", str(Adc0AvgVar.get()))
    config.set("Ads", "Adc1AvgVar", str(Adc1AvgVar.get()))
    config.set("Ads", "Adc2AvgVar", str(Adc2AvgVar.get()))
    config.set("Ads", "Adc3AvgVar", str(Adc3AvgVar.get()))
    config.set("Ads", "AdcCurrVar", AdcCurrVar.get())
    config.set("Ads", "AdcSpVar", AdcSpVar.get())

    config.set("Gps", "gps3dExist", gps3dExist.get())
    config.set("Gps", "gpsTimeExist", gpsTimeExist.get())
    config.set("Gps", "gpsRateVar", gpsRateVar.get())

    config.set("AddFields", "fillTest1VarioTemperatureVar", fillTest1VarioTemperatureVar.get())
    config.set("AddFields", "fillTest2VarioTemperatureVar", fillTest2VarioTemperatureVar.get())
    config.set("AddFields", "fillTest3VarioTemperatureVar", fillTest3VarioTemperatureVar.get())
    config.set("AddFields", "fillTest3ExpectedAltitudeVar", fillTest3ExpectedAltitudeVar.get())
    config.set("AddFields", "expAltTimeVar", str(expAltTimeVar.get()))
    config.set("AddFields", "fillTest123LinearAccVar", fillTest123LinearAccVar.get())
    config.set("AddFields", "fillTest12VspeedAltVar", fillTest12VspeedAltVar.get())
    config.set("AddFields", "fillTest1DteVar", fillTest1DteVar.get())
    config.set("AddFields", "fillTest2PpmAirspeedCompVar", fillTest2PpmAirspeedCompVar.get())
    config.set("AddFields", "fillTest1YawRateVar", fillTest1YawRateVar.get())
    config.set("AddFields", "fillTest1HeadingVar", fillTest1HeadingVar.get())
    config.set("AddFields", "fillTest123FlowVar", fillTest123FlowVar.get())
    config.set("AddFields", "fillTest1GpsNbrSatVar", fillTest1GpsNbrSatVar.get())
    config.set("AddFields", "fillTest2GpsHdopVar", fillTest2GpsHdopVar.get())
        
    config.set("Frsky", "frskyVfasVar", frskyVfasVar.get())
    config.set("Frsky", "frskyFuelVar", frskyFuelVar.get())
    config.set("Frsky", "frskyA3Var", frskyA3Var.get())
    config.set("Frsky", "frskyA4Var", frskyA4Var.get())
    config.set("Frsky", "frskyAccxVar", frskyAccxVar.get())
    config.set("Frsky", "frskyAccyVar", frskyAccyVar.get())
    config.set("Frsky", "frskyAcczVar", frskyAcczVar.get())
    config.set("Frsky", "frskyT1Var", frskyT1Var.get())
    config.set("Frsky", "frskyT2Var", frskyT2Var.get())

    config.set("Multiplex", "multiplexLine1Var", multiplexLine1Var.get())
    config.set("Multiplex", "multiplexLine2Var", multiplexLine2Var.get())
    config.set("Multiplex", "multiplexLine3Var", multiplexLine3Var.get())
    config.set("Multiplex", "multiplexLine4Var", multiplexLine4Var.get())
    config.set("Multiplex", "multiplexLine5Var", multiplexLine5Var.get())
    config.set("Multiplex", "multiplexLine6Var", multiplexLine6Var.get())
    config.set("Multiplex", "multiplexLine7Var", multiplexLine7Var.get())
    config.set("Multiplex", "multiplexLine8Var", multiplexLine8Var.get())
    config.set("Multiplex", "multiplexLine9Var", multiplexLine9Var.get())
    
    config.set("Hott", "hottBattery1SourceVar", hottBattery1SourceVar.get())
    config.set("Hott", "hottBattery2SourceVar", hottBattery2SourceVar.get())
    config.set("Hott", "hottCellUndervoltageVar", str(hottCellUndervoltageVar.get()))
    config.set("Hott", "hottMainBatterySourceVar", hottMainBatterySourceVar.get())
    config.set("Hott", "hottT1SourceVar", hottT1SourceVar.get())
    config.set("Hott", "hottT2SourceVar", hottT2SourceVar.get()) 

    config.set("Jeti", "jetiVoltageSourceVar", jetiVoltageSourceVar.get())
    config.set("Jeti", "jetiTemperatureSourceVar", jetiTemperatureSourceVar.get())

    configSaveFileName = filedialog.asksaveasfilename(title="Save as...", defaultextension="*.ini",\
         filetypes=[("Ini files","*.ini"),("All files", "*")], initialfile=configUploadFileName)
    print(configSaveFileName)
    config.write(open(configSaveFileName ,'w'))
    #configUploadFileName = configSaveFileName
    #config.write(open('config.ini','w'))

def protocolChanged(*arg):
    #print(protocolVar.get() )
    nb.tab(16, state="hidden")
    nb.tab(17, state="hidden")
    nb.tab(18, state="hidden")
    nb.tab(19, state="hidden")
    if protocolVar.get() == "FRSKY_SPORT" or protocolVar.get() == "FRSKY_HUB" or protocolVar.get() == "FRSKY_SPORT_HUB" :
        nb.tab(16, state="normal")
    elif protocolVar.get() == "JETI":
        nb.tab(17, state="normal")
    elif protocolVar.get() == "HOTT":
        nb.tab(18, state="normal")
    elif protocolVar.get() == "MULTIPLEX":
        nb.tab(19, state="normal")

def ppmChanged():
    if ppmExist.get() == 'On':
        nb.tab(1, state="normal")
    else:
        nb.tab(1, state="hidden")
def ppmTypeChanged(*arg):
    if ppmTypeVar.get() == 'Rx chanel':
        fPpmType.grid(row=8)
    else:    
       fPpmType.grid_remove()
def varioChanged():
    if varioExist.get() == 'On':
        nb.tab(2, state="normal")
    else:
        nb.tab(2, state="hidden")
def voltageChanged():
    if voltageExist.get() == 'On':
        nb.tab(3, state="normal")
    else:
        nb.tab(3, state="hidden")
def currentChanged():
    if currentExist.get() == 'On':
        nb.tab(4, state="normal")
    else:
        nb.tab(4, state="hidden")
def gpsChanged():
    if gpsExist.get() == 'On':
        nb.tab(5, state="normal")
    else:
        nb.tab(5, state="hidden")
def airspeedChanged():
    if airspeedExist.get() == 'On':
        nb.tab(6, state="normal")
    else:
        nb.tab(6, state="hidden")
def rpmChanged():
    if rpmExist.get() == 'On':
        nb.tab(7, state="normal")
    else:
        nb.tab(7, state="hidden")
def imuChanged():
    if imuExist.get() == 'On':
        nb.tab(8, state="normal")
    else:
        nb.tab(8, state="hidden")
def magnetChanged():
    if magnetExist.get() == 'On':
        nb.tab(9, state="normal")
    else:
        nb.tab(9, state="hidden")
def flowChanged():
    if flowExist.get() == 'On':
        nb.tab(10, state="normal")
    else:
        nb.tab(10, state="hidden")
def adcChanged():
    if adcExist.get() == 'On':
        nb.tab(11, state="normal")
    else:
        nb.tab(11, state="hidden")
def locatorChanged():
    if locatorExist.get() == 'On':
        nb.tab(12, state="normal")
    else:
        nb.tab(12, state="hidden")        
def sequenceChanged():
    if sequenceExist.get() == 'On':
        nb.tab(13, state="normal")
    else:
        nb.tab(13, state="hidden")
def persistentChanged():
    if persistentExist.get() == 'On':
        nb.tab(14, state="normal")
    else:
        nb.tab(14, state="hidden")
def addFieldsChanged():
    if addFieldsExist.get() == 'On':
        nb.tab(15, state="normal")
    else:
        nb.tab(15, state="hidden")        

def fillTest3ExpectedAltitudeChanged():
    if fillTest3ExpectedAltitudeVar.get() == 'On':
        fFillTest3ExpectedAltitude.grid(row=4)
    else:
        fFillTest3ExpectedAltitude.grid_remove()
def analogVarioChanged():
    if analogVarioExist.get() == 'On':
        fAnalogVario.grid(row=8)
    else:    
        fAnalogVario.grid_remove()


root = Tk()
root.title("openXsensor configurator V1.0.4")
nb = ttk.Notebook(root)
nb.enable_traversal()
fMain = ttk.Frame(nb)   
fPpm = ttk.Frame(nb)   # page 1
fVario = ttk.Frame(nb)   # page 2
fVoltage = ttk.Frame(nb) # page 3
fCurrent = ttk.Frame(nb) # page 4
fGps = ttk.Frame(nb) # page 5
fAirspeed = ttk.Frame(nb) # page 6
fRpm = ttk.Frame(nb) # page 7
fImu = ttk.Frame(nb) # page 8
fMagnet = ttk.Frame(nb) # page 9
fFlow = ttk.Frame(nb) # page 10
fAdc = ttk.Frame(nb) # page 11
fLocator = ttk.Frame(nb) # page 12
fSequence = ttk.Frame(nb) # page 13
fMemory = ttk.Frame(nb) # page 14
fAddFields = ttk.Frame(nb) # page 15
fFrsky = ttk.Frame(nb) # page 16
fJeti = ttk.Frame(nb) # page 17
fHott = ttk.Frame(nb) # page 18
fMultiplex = ttk.Frame(nb) # page 19

ppmExist = StringVar(value='Off')
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
persistentExist = StringVar(value='Off')
adcExist = StringVar(value='Off')
sequenceExist = StringVar(value='Off')
addFieldsExist = StringVar(value='Off')

fPpmType = ttk.Frame(fPpm)
ppmTypeVar = StringVar(value='Rx chanel')
ppmPinVar = IntVar(value = 2)
ppmForMin100Var = IntVar(value = 1000)
ppmForPlus100Var = IntVar(value = 2000)

imuPinVar = IntVar(value = 3)
imuDisplayOffset = StringVar(value='Off')
imuOffsetXVar = IntVar(value=0)
imuOffsetYVar = IntVar(value=0)
imuOffsetZVar = IntVar(value=0)

flowPulsesmlVar = IntVar(value = 10)
flowTankCapVar = IntVar(value = 1000)
flowL1ValueVar = IntVar(value = 0)
flowL1CorrVar = IntVar(value = 0)
flowL2ValueVar = IntVar(value = 0)
flowL2CorrVar = IntVar(value = 0)
flowL3ValueVar = IntVar(value = 0)
flowL3CorrVar = IntVar(value = 0)
flowL4ValueVar = IntVar(value = 0)
flowL4CorrVar = IntVar(value = 0)
flowPPMResetVar = IntVar(value = 95)

pushButtPinVar = IntVar(value = 10)

GeneratemagCalData = StringVar(value='Off')
xMagOffsetVar= IntVar(value=0)
yMagOffsetVar= IntVar(value=0)
zMagOffsetVar= IntVar(value=0)
xxMagCorrectionVar = DoubleVar(value=0)
xyMagCorrectionVar = DoubleVar(value=0)
xzMagCorrectionVar = DoubleVar(value=0)
yxMagCorrectionVar = DoubleVar(value=0)
yyMagCorrectionVar = DoubleVar(value=0)
yzMagCorrectionVar = DoubleVar(value=0)
zxMagCorrectionVar = DoubleVar(value=0)
zyMagCorrectionVar = DoubleVar(value=0)
zzMagCorrectionVar = DoubleVar(value=0)

varioTypeVar = StringVar(value='MS5611')

secondBaroExist = StringVar(value='Off')
varioSourceVar = StringVar(value='FIRST_BARO')
varioHysteresisVar = IntVar(value=5)


fAnalogVario = ttk.Frame(fVario)
analogVarioExist = StringVar(value='Off')
varioAnalogPinVar = StringVar(value='3')
varioAnalogMinVar = DoubleVar(value=-3)
varioAnalogMaxVar = DoubleVar(value=3)

airspeedTypeVar = StringVar(value='MS4525')
airspeedAtSea15CVar = StringVar(value='Off') 
airspeedInKmhVar = StringVar(value='Off')
airspeedResetAtPpmVar = IntVar(value=100)
airspeedCompMinAtPpmVar = IntVar(value=60)
airspeedCompMaxAtPpmVar = IntVar(value=90)
airspeedCompMinVar = IntVar(value=80)
airspeedCompMaxVar = IntVar(value=140)

Adc0TypeVar= StringVar(value='ADS_OFF')
Adc1TypeVar= StringVar(value='ADS_OFF')
Adc2TypeVar= StringVar(value='ADS_OFF')
Adc3TypeVar= StringVar(value='ADS_OFF')
Adc0FsVar= StringVar(value='MV6144')
Adc1FsVar= StringVar(value='MV6144')
Adc2FsVar= StringVar(value='MV6144')
Adc3FsVar= StringVar(value='MV6144')
Adc0OffVar = IntVar(value=0)
Adc1OffVar = IntVar(value=0)
Adc2OffVar = IntVar(value=0)
Adc3OffVar = IntVar(value=0)
Adc0ScVar = DoubleVar(value=1)
Adc1ScVar = DoubleVar(value=1)
Adc2ScVar = DoubleVar(value=1)
Adc3ScVar = DoubleVar(value=1)
Adc0CrVar= StringVar(value='MS2')
Adc1CrVar= StringVar(value='MS2')
Adc2CrVar= StringVar(value='MS2')
Adc3CrVar= StringVar(value='MS2')
Adc0AvgVar = IntVar(value=10)
Adc1AvgVar = IntVar(value=10)
Adc2AvgVar = IntVar(value=10)
Adc3AvgVar = IntVar(value=10)
AdcCurrVar = StringVar(value='NO')
AdcSpVar = StringVar(value='NO')

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
voltRefValueVar = IntVar(value='5000')

currentPinVar = StringVar(value='0')
currentMvoltAt0Var = DoubleVar(value='0.0')
currentMvoltPerAmpVar = DoubleVar(value='0.0')
currentRgVar = DoubleVar(value='0.0')
currentRcVar = DoubleVar(value='0.0')

pulsesPerRotationVar = IntVar(value=2)

pushButtonPin=StringVar(value='10')

gps3dExist = StringVar(value='Off')
gpsTimeExist = StringVar(value='Off')
gpsRateVar = StringVar(value='5')

SeqUnitVar = IntVar(value='1')
SeqMinVoltVar = IntVar(value='0')
SeqMinCellVar = IntVar(value='0')
SeqOut1Var = StringVar(value='0')
SeqOut2Var = StringVar(value='0')
SeqOut3Var = StringVar(value='0')
SeqOut4Var = StringVar(value='0')
SeqOut5Var = StringVar(value='0')
SeqOut6Var = StringVar(value='0')
SeqB5Var = []
SeqB4Var = []
SeqB3Var = []
SeqB2Var = []
SeqB1Var = []
SeqB0Var = []
SeqTVar = []

for x in range(0,100):
    SeqB0Var.append(IntVar(value='0'))
    SeqB1Var.append(IntVar(value='0'))
    SeqB2Var.append(IntVar(value='0'))
    SeqB3Var.append(IntVar(value='0'))
    SeqB4Var.append(IntVar(value='0'))
    SeqB5Var.append(IntVar(value='0'))
    SeqTVar.append(StringVar(value=''))

fFillTest3ExpectedAltitude = ttk.Frame(fAddFields)
fillTest3ExpectedAltitudeVar = StringVar(value='Off')
expAltTimeVar = DoubleVar(value='1.5')
fillTest1VarioTemperatureVar = StringVar(value='Off')
fillTest2VarioTemperatureVar = StringVar(value='Off')
fillTest3VarioTemperatureVar = StringVar(value='Off')
fillTest123LinearAccVar = StringVar(value='Off')
fillTest12VspeedAltVar = StringVar(value='Off')
fillTest1DteVar = StringVar(value='Off')
fillTest2PpmAirspeedCompVar = StringVar(value='Off')
fillTest1YawRateVar = StringVar(value='Off')
fillTest1HeadingVar = StringVar(value='Off')
fillTest123FlowVar = StringVar(value='Off')
fillTest1GpsNbrSatVar = StringVar(value='Off')
fillTest2GpsHdopVar = StringVar(value='Off')

protocolVar = StringVar(value='FRSKY_SPORT')
pinToRxVar = StringVar(value='4')

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

hottCellUndervoltageVar = IntVar(value=3300)
hottBattery1SourceVar = StringVar(value='Not sent')
hottBattery2SourceVar = StringVar(value='Not sent')
hottMainBatterySourceVar = StringVar(value='Not sent')
hottT1SourceVar = StringVar(value='Not sent')
hottT2SourceVar = StringVar(value='Not sent')

jetiVoltageSourceVar = StringVar(value='Not sent')
jetiTemperatureSourceVar = StringVar(value='Not sent')
