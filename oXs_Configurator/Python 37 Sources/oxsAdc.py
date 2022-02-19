from oxsConfig import ttk , W , E , fAdc , Spinbox , Adc0TypeVar, Adc1TypeVar , Adc2TypeVar , Adc3TypeVar, Adc0FsVar, Adc1FsVar, Adc2FsVar, Adc3FsVar, Adc0OffVar, Adc1OffVar, Adc2OffVar, Adc3OffVar
from oxsConfig import Adc0ScVar, Adc1ScVar, Adc2ScVar, Adc3ScVar, Adc0CrVar, Adc1CrVar, Adc2CrVar, Adc3CrVar, Adc0AvgVar, Adc1AvgVar, Adc2AvgVar, Adc3AvgVar, AdcCurrVar, AdcSpVar

#define ADS_MEASURE A0_TO_A1 ,  ADS_OFF , ADS_OFF , ADS_OFF // select 4 values between A0_TO_A1, A0_TO_A3, A1_TO_A3, A2_TO_A3, A0_TO_GND, A1_TO_GND, A2_TO_GND, A3_TO_GND, ADS_OFF
#define ADS_FULL_SCALE_VOLT  MV2048, MV4096, MV6144, MV4096 //  select between MV6144 MV4096 MV2048 MV1024 MV512 MV256
#define ADS_OFFSET 0, 0 , 0 , 0 // must be an integer (positive or negative)
#define ADS_SCALE 1, 1, 1, 1 // can be a float
#define ADS_RATE  MS2 , MS9, MS9 , MS2 // select between MS137, MS69, MS35, MS18, MS9, MS5, MS3 , MS2
#define ADS_AVERAGING_ON 10 , 20, 30, 50 // number of values used for averaging (must be between 1 and 254) 
#//#define ADS_CURRENT_BASED_ON ADS_VOLT_1  // uncomment if current, and comsumption have to be calculated based on one of ADS voltage measurement; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define ADS_AIRSPEED_BASED_ON ADS_VOLT_1  // uncomment if airspeed (and dte) have to be calculated based on one of ADS voltage measurement ; select then the voltage to be used between ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4


ttk.Label(fAdc, text="Input configuration").grid(row=1, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))

Adc0TypeBox = ttk.Combobox(fAdc, textvariable=Adc0TypeVar , justify='right', values=('ADS_OFF', "A0_TO_GND", "A0_TO_A1", "A0_TO_A3"), state="readonly", width=15).grid(column=0, row=2 , padx=10, sticky=(E))
Adc1TypeBox = ttk.Combobox(fAdc, textvariable=Adc1TypeVar , justify='right', values=('ADS_OFF', "A1_TO_GND", "A1_TO_A3"), state="readonly", width=15).grid(column=1, row=2 , padx=10, sticky=(E))
Adc2TypeBox = ttk.Combobox(fAdc, textvariable=Adc2TypeVar , justify='right', values=('ADS_OFF', "A2_TO_GND", "A2_TO_A3"), state="readonly", width=15).grid(column=2, row=2 , padx=10, sticky=(E))
Adc3TypeBox = ttk.Combobox(fAdc, textvariable=Adc3TypeVar , justify='right', values=('ADS_OFF', "A3_TO_GND"), state="readonly", width=15).grid(column=3, row=2 , padx=10, sticky=(E))

ttk.Label(fAdc, text="Full Scale Volt").grid(row=3, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
Adc0FsBox = ttk.Combobox(fAdc, textvariable=Adc0FsVar , justify='right', values=('MV256', "MV512", "MV1024", "MV2048", "MV4096", "MV6144"), state="readonly", width=15).grid(column=0, row=4 , padx=10, sticky=(E))
Adc1FsBox = ttk.Combobox(fAdc, textvariable=Adc1FsVar , justify='right', values=('MV256', "MV512", "MV1024", "MV2048", "MV4096", "MV6144"), state="readonly", width=15).grid(column=1, row=4 , padx=10, sticky=(E))
Adc2FsBox = ttk.Combobox(fAdc, textvariable=Adc2FsVar , justify='right', values=('MV256', "MV512", "MV1024", "MV2048", "MV4096", "MV6144"), state="readonly", width=15).grid(column=2, row=4 , padx=10, sticky=(E))
Adc3FsBox = ttk.Combobox(fAdc, textvariable=Adc3FsVar , justify='right', values=('MV256', "MV512", "MV1024", "MV2048", "MV4096", "MV6144"), state="readonly", width=15).grid(column=3, row=4 , padx=10, sticky=(E))

ttk.Label(fAdc, text="Measure offset").grid(row=5, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
Adc0OffBox = ttk.Entry(fAdc, textvariable=Adc0OffVar , width='15', justify='right').grid(column=0 , row=6, padx=10, sticky=(E))
Adc1OffBox = ttk.Entry(fAdc, textvariable=Adc1OffVar , width='15', justify='right').grid(column=1 , row=6, padx=10, sticky=(E))
Adc2OffBox = ttk.Entry(fAdc, textvariable=Adc2OffVar , width='15', justify='right').grid(column=2 , row=6, padx=10, sticky=(E))
Adc3OffBox = ttk.Entry(fAdc, textvariable=Adc3OffVar , width='15', justify='right').grid(column=3 , row=6, padx=10, sticky=(E))

ttk.Label(fAdc, text="Scale").grid(row=7, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
Adc0ScBox = ttk.Entry(fAdc, textvariable=Adc0ScVar , width='15', justify='right').grid(column=0 , row=8, padx=10, sticky=(E))
Adc1ScBox = ttk.Entry(fAdc, textvariable=Adc1ScVar , width='15', justify='right').grid(column=1 , row=8, padx=10, sticky=(E))
Adc2ScBox = ttk.Entry(fAdc, textvariable=Adc2ScVar , width='15', justify='right').grid(column=2 , row=8, padx=10, sticky=(E))
Adc3ScBox = ttk.Entry(fAdc, textvariable=Adc3ScVar , width='15', justify='right').grid(column=3 , row=8, padx=10, sticky=(E))

ttk.Label(fAdc, text="Convertion Rate").grid(row=9, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
Adc0CrBox = ttk.Combobox(fAdc, textvariable=Adc0CrVar , justify='right', values=('MS137', "MS69", "MS35", "MS18", "MS9", "MS5", "MS3", "MS2"), state="readonly", width=15).grid(column=0, row=10 , padx=10, sticky=(E))
Adc1CrBox = ttk.Combobox(fAdc, textvariable=Adc1CrVar , justify='right', values=('MS137', "MS69", "MS35", "MS18", "MS9", "MS5", "MS3", "MS2"), state="readonly", width=15).grid(column=1, row=10 , padx=10, sticky=(E))
Adc2CrBox = ttk.Combobox(fAdc, textvariable=Adc2CrVar , justify='right', values=('MS137', "MS69", "MS35", "MS18", "MS9", "MS5", "MS3", "MS2"), state="readonly", width=15).grid(column=2, row=10 , padx=10, sticky=(E))
Adc3CrBox = ttk.Combobox(fAdc, textvariable=Adc3CrVar , justify='right', values=('MS137', "MS69", "MS35", "MS18", "MS9", "MS5", "MS3", "MS2"), state="readonly", width=15).grid(column=3, row=10 , padx=10, sticky=(E))

ttk.Label(fAdc, text="Number of samples for averaging").grid(row=11, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
Adc0AvgBox = Spinbox(fAdc, from_=1 , to=254, textvariable=Adc0AvgVar , justify='right' , increment='1', width=15).grid(column=0, row=12 , padx=10 , sticky=(E))
Adc1AvgBox = Spinbox(fAdc, from_=1 , to=254, textvariable=Adc1AvgVar , justify='right' , increment='1', width=15).grid(column=1, row=12 , padx=10 , sticky=(E))
Adc2AvgBox = Spinbox(fAdc, from_=1 , to=254, textvariable=Adc2AvgVar , justify='right' , increment='1', width=15).grid(column=2, row=12 , padx=10 , sticky=(E))
Adc3AvgBox = Spinbox(fAdc, from_=1 , to=254, textvariable=Adc3AvgVar , justify='right' , increment='1', width=15).grid(column=3, row=12 , padx=10 , sticky=(E))

ttk.Label(fAdc, text="Use ADS1115 to calculate current").grid(row=13, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
AdcCurrBox = ttk.Combobox(fAdc, textvariable=AdcCurrVar , justify='right', values=('NO', "ADS_VOLT_1", "ADS_VOLT_2", "ADS_VOLT_3", "ADS_VOLT_4"), state="readonly", width=15).grid(column=0, row=14 , padx=10, sticky=(E))

ttk.Label(fAdc, text="Use ADS1115 to calculate airspeed").grid(row=15, padx= 5 , pady=(20,2), columnspan = 4, sticky=(W))
AdcSpBox = ttk.Combobox(fAdc, textvariable=AdcSpVar , justify='right', values=('NO', "ADS_VOLT_1", "ADS_VOLT_2", "ADS_VOLT_3", "ADS_VOLT_4"), state="readonly", width=15).grid(column=0, row=16 , padx=10, sticky=(E))

#Adc1TypeBox.bind('<<ComboboxSelected>>', ppmTypeChanged)
