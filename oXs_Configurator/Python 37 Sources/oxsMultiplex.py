from oxsConfig import ttk , W , E , fMultiplex , multiplexLine1Var , multiplexLine2Var , multiplexLine3Var , multiplexLine4Var , multiplexLine5Var ,\
    multiplexLine6Var , multiplexLine7Var , multiplexLine8Var , multiplexLine9Var

#define SETUP_MULTIPLEX_DATA_TO_SEND    \
#                        5 , REL_ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \
#                        6 , VERTICAL_SPEED , 1 , 1 , 0, -500 , 500  , \
#                        7 , ALTIMETER_MAX , 1 , 1 , 0 , -16384 , 16383 
#                        // 8 , VOLT_1 , 1, 1 , 0 , -16384 , 16383
#                       // 6 , CURRENTMA , 1 , 1, 0 , -16384 , 16383 ,\
#                        // 7 , MILLIAH  , 1 , 1, 0 , -16384 , 16383
#                        // 7 , CELL_TOT , 1 , 1 , 0 , -16384 , 16383 , \
#                        // 4 , VOLT_4 , 1 , 1 , 0 , -16384 , 16383 , \
#                        // 9 , PPM , 1 , 1 , 0 , -16384 , 16383 
#                        // 3 , ALTIMETER , 1 , 1 , 0 , -16384 , 16383 , \
ttk.Label(fMultiplex, text="For one or several display line number, select the data to be sent" ).grid(column= 0, row=1, columnspan=2, padx= (15,5), pady=(25,5) , sticky=(E))
ttk.Label(fMultiplex, text="1" ).grid(column= 0, row=2, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="2" ).grid(column= 0, row=3, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="3" ).grid(column= 0, row=4, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="4" ).grid(column= 0, row=5, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="5" ).grid(column= 0, row=6, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="6" ).grid(column= 0, row=7, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="7" ).grid(column= 0, row=8, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="8" ).grid(column= 0, row=9, padx= (15,5), pady=(10,5) , sticky=('E'))
ttk.Label(fMultiplex, text="9" ).grid(column= 0, row=10, padx= (15,5), pady=(10,5) , sticky=('E'))

'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'

MultiplexLine1Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine1Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine1Box.grid(column=1, row=2, pady=(10,5) ,sticky=('W') )
MultiplexLine2Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine2Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine2Box.grid(column=1, row=3, pady=(10,5) ,sticky=('W') )
MultiplexLine3Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine3Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine3Box.grid(column=1, row=4, pady=(10,5) ,sticky=('W') )
MultiplexLine4Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine4Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine4Box.grid(column=1, row=5, pady=(10,5) ,sticky=('W') )
MultiplexLine5Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine5Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine5Box.grid(column=1, row=6, pady=(10,5) ,sticky=('W') )
MultiplexLine6Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine6Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine6Box.grid(column=1, row=7, pady=(10,5) ,sticky=('W') )
MultiplexLine7Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine7Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine7Box.grid(column=1, row=8, pady=(10,5) ,sticky=('W') )
MultiplexLine8Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine8Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine8Box.grid(column=1, row=9, pady=(10,5) ,sticky=('W') )
MultiplexLine9Box = ttk.Combobox(fMultiplex, textvariable=multiplexLine9Var, 
                values=('Not sent','VOLT_1', 'VOLT_2', 'VOLT_3' , 'VOLT_4', 'VOLT_5' , 'VOLT_6', 'REL_ALTIMETER',
                'VERTICAL_SPEED', 'ALTIMETER_MAX', 'CURRENTMA' , 'MILLIAH' , 'CELL_TOT', 'PPM', 'GPS_COURSE', 'GPS_SPEED', 'GPS_ALTITUDE', 'GPS_DISTANCE', 'GPS_BEARING'  ),
                state="readonly")
MultiplexLine9Box.grid(column=1, row=10, pady=(10,5) ,sticky=('W') )
