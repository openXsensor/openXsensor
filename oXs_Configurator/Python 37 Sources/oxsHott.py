from oxsConfig import ttk , W , E ,  fHott , hottBattery1SourceVar , hottBattery2SourceVar , hottCellUndervoltageVar ,\
    hottMainBatterySourceVar , hottT1SourceVar , hottT2SourceVar

#define CELL_UNDERVOLTAGE_WARNING 3300            // Warning threshold in mV; 
#define BATTERY_1_SOURCE          ADS_VOLT_1      // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define BATTERY_2_SOURCE          ADS_VOLT_2      // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define MAIN_BATTERY_SOURCE       ADS_VOLT_3      // select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, ADS_VOLT_1, ADS_VOLT_2, ADS_VOLT_3, ADS_VOLT_4
#define TEMPERATURE_1_SOURCE      VOLT_1          //  select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY , PPM
#define TEMPERATURE_2_SOURCE      PPM             //  select between VOLT_1, VOLT_2, VOLT_3, VOLT_4, VOLT_5, VOLT_6, TEST_1, TEST_2, TEST_3 , GLIDER_RATIO , SENSITIVITY, PPM


ttk.Label(fHott, text="Undervoltage warning").grid(column= 0, row=1, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fHott, text="Based on" ).grid(column= 1, row=2, pady=(30,5) , sticky=('WE'))
ttk.Label(fHott, text="Battery 1" ).grid(column= 0, row=3, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fHott, text="Battery 2" ).grid(column= 0, row=4, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fHott, text="Main battery" ).grid(column= 0, row=5, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fHott, text="Temperature 1" ).grid(column= 0, row=6, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fHott, text="Temperature 2" ).grid(column= 0, row=7, padx= (15,5), pady=(10,5) , sticky=('E'))
hottCellUndervoltageBox = ttk.Entry(fHott, textvariable=hottCellUndervoltageVar , width='6' )
hottCellUndervoltageBox.grid(column=1, row=1, sticky=('W'),  pady=(10,5 ) )
hottBattery1SourceBox = ttk.Combobox(fHott, textvariable=hottBattery1SourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 
                'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
hottBattery1SourceBox.grid(column=1, row=3, pady=(10,5) ,sticky=('W') )
hottBattery2SourceBox = ttk.Combobox(fHott, textvariable=hottBattery2SourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 
                'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
hottBattery2SourceBox.grid(column=1, row=4, pady=(10,5) ,sticky=('W') )
hottMainBatterySourceBox = ttk.Combobox(fHott, textvariable=hottMainBatterySourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 
                'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
hottMainBatterySourceBox.grid(column=1, row=5, pady=(10,5) ,sticky=('W') )
hottT1SourceBox = ttk.Combobox(fHott, textvariable=hottT1SourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 
                'TEST_1', 'TEST_2', 'TEST_3' , 'GLIDER_RATIO' , 'SENSITIVITY' , 'PPM'  ),
                state="readonly")
hottT1SourceBox.grid(column=1, row=6, pady=(10,5) ,sticky=('W') )
hottT2SourceBox = ttk.Combobox(fHott, textvariable=hottT2SourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 
                'TEST_1', 'TEST_2', 'TEST_3' , 'GLIDER_RATIO' , 'SENSITIVITY' , 'PPM'  ),
                state="readonly")
hottT2SourceBox.grid(column=1, row=7, pady=(10,5) ,sticky=('W') )

