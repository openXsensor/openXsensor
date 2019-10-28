from oxsConfig import ttk , W , E , fJeti , jetiTemperatureSourceVar , jetiVoltageSourceVar

#define VOLTAGE_SOURCE   VOLT_1                     // select between VOLT_1, VOLT_2, VOLT_3 , VOLT_4, VOLT_5 , VOLT_6
#define TEMPERATURE_SOURCE NTC                   // select between MS5611 and NTC

ttk.Label(fJeti, text="Voltage source").grid(column= 0, row=1, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fJeti, text="Temperature source" ).grid(column= 0, row=2, pady=(15,5) , sticky=('WE'))
jetiVoltageSourceBox = ttk.Combobox(fJeti, textvariable=jetiVoltageSourceVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6' ),
                state="readonly")
jetiVoltageSourceBox.grid(column=1, row=1, pady=(15,5) ,sticky=('W') )
jetiTemperatureSourceBox = ttk.Combobox(fJeti, textvariable=jetiTemperatureSourceVar, 
                values=('Not sent','MS5611', 'NTC'  ),
                state="readonly")
jetiTemperatureSourceBox.grid(column=1, row=2, pady=(15,5) ,sticky=('W') )
