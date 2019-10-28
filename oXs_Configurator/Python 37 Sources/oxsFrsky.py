from oxsConfig import ttk , W , E , fFrsky , frskyVfasVar , frskyFuelVar , frskyA3Var , frskyA4Var , \
  frskyAccxVar , frskyAccyVar , frskyAcczVar , frskyT1Var , frskyT2Var

#here start the definition of the FRSKY tab
ttk.Label(fFrsky, text="VFAS" ).grid(column= 0, row=1, padx= (15,5), pady=(25,5) , sticky=(E))
frskyVfasBox = ttk.Combobox(fFrsky, textvariable=frskyVfasVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1',
                'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
frskyVfasBox.grid(column=1, row=1, pady=(25,5) ,sticky=(W) )
ttk.Label(fFrsky, text="Fuel" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(E))
frskyFuelBox = ttk.Combobox(fFrsky, textvariable=frskyFuelVar, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1',
                'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
frskyFuelBox.grid(column=1, row=2, sticky=(W) )
ttk.Label(fFrsky, text="A3" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(E))
frskyA3Box = ttk.Combobox(fFrsky, textvariable=frskyA3Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1',
                'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
frskyA3Box.grid(column=1, row=3, sticky=(W) )
ttk.Label(fFrsky, text="A4" ).grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(E))
frskyA4Box = ttk.Combobox(fFrsky, textvariable=frskyA4Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1',
                'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'  ),
                state="readonly")
frskyA4Box.grid(column=1, row=4, sticky=(W) )

ttk.Label(fFrsky, text="Acc X" ).grid(column= 0, row=5, padx= (15,5), pady=5 , sticky=(E))
frskyAccxBox = ttk.Combobox(fFrsky, textvariable=frskyAccxVar, 
                values=('Not sent','TEST_1', 'TEST_2', 'TEST_3', 'GLIDER_RATIO' , 'SECONDS_SINCE_T0'
                 ,'AVERAGE_VSPEED_SINCE_TO' , 'VOLT_1', 'VOLT_2', 'VOLT_3', 'VOLT_4', 'VOLT_5'
                 , 'VOLT_6', 'PITCH', 'ROLL', 'YAW', 'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'   ),
                state="readonly" , width=30)
frskyAccxBox.grid(column=1, row=5, sticky=(W) )
ttk.Label(fFrsky, text="Acc Y" ).grid(column= 0, row=6, padx= (15,5), pady=5 , sticky=(E))
frskyAccyBox = ttk.Combobox(fFrsky, textvariable=frskyAccyVar, 
                values=('Not sent','TEST_1', 'TEST_2', 'TEST_3', 'GLIDER_RATIO' , 'SECONDS_SINCE_T0'
                 ,'AVERAGE_VSPEED_SINCE_TO' , 'VOLT_1', 'VOLT_2', 'VOLT_3', 'VOLT_4', 'VOLT_5'
                 , 'VOLT_6', 'PITCH', 'ROLL', 'YAW', 'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'   ),
                state="readonly" , width=30)
frskyAccyBox.grid(column=1, row=6, sticky=(W) )
ttk.Label(fFrsky, text="Acc Z" ).grid(column= 0, row=7, padx= (15,5), pady=5 , sticky=(E))
frskyAcczBox = ttk.Combobox(fFrsky, textvariable=frskyAcczVar, 
                values=('Not sent','TEST_1', 'TEST_2', 'TEST_3', 'GLIDER_RATIO' , 'SECONDS_SINCE_T0'
                 ,'AVERAGE_VSPEED_SINCE_TO' , 'VOLT_1', 'VOLT_2', 'VOLT_3', 'VOLT_4', 'VOLT_5'
                 , 'VOLT_6', 'PITCH', 'ROLL', 'YAW', 'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'   ),
                state="readonly" , width=30)
frskyAcczBox.grid(column=1, row=7, sticky=(W) )
ttk.Label(fFrsky, text="T1" ).grid(column= 0, row=8, padx= (15,5), pady=5 , sticky=(E))
frskyT1Box = ttk.Combobox(fFrsky, textvariable=frskyT1Var, 
                values=('Not sent','TEST_1', 'TEST_2', 'TEST_3', 'GLIDER_RATIO' , 'SECONDS_SINCE_T0'
                 ,'AVERAGE_VSPEED_SINCE_TO' ,'SENSITIVITY' , 'PPM', 'VOLT_1', 'VOLT_2', 'VOLT_3',
                  'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'   ),
                state="readonly" , width=30)
frskyT1Box.grid(column=1, row=8, sticky=(W) )
ttk.Label(fFrsky, text="T2" ).grid(column= 0, row=9, padx= (15,5), pady=5 , sticky=(E))
frskyT2Box = ttk.Combobox(fFrsky, textvariable=frskyT2Var, 
                values=('Not sent','TEST_1', 'TEST_2', 'TEST_3', 'GLIDER_RATIO' , 'SECONDS_SINCE_T0'
                 ,'AVERAGE_VSPEED_SINCE_TO' ,'SENSITIVITY' , 'PPM', 'VOLT_1', 'VOLT_2', 'VOLT_3',
                  'VOLT_4', 'VOLT_5' , 'VOLT_6', 'ADS_VOLT_1', 'ADS_VOLT_2', 'ADS_VOLT_3', 'ADS_VOLT_4'   ),
                state="readonly" , width=30)
frskyT2Box.grid(column=1, row=9, sticky=(W) )
