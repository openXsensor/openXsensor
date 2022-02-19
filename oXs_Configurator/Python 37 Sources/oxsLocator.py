from oxsConfig import ttk , W , E , fLocator

ttk.Label(fLocator, text="In order to user OpenXSensor as locator you need a SX1276/RFM95 module" ).grid(column= 0, row=4, padx= 5 , pady= 20, sticky=(W))
ttk.Label(fLocator, text="Connect the module in the following way" ).grid(column= 0, row=5, padx= 10 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - Arduino digital pin 10 <=> NSS from module" ).grid(column= 0, row=6, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - Arduino digital pin 11 <=> MOSI from module" ).grid(column= 0, row=7, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - Arduino digital pin 12 <=> MISO from module" ).grid(column= 0, row=8, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - Arduino digital pin 13 <=> SCK from module" ).grid(column= 0, row=9, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - Arduino GRND           <=> GRND from module" ).grid(column= 0, row=10, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="*  - external (or Arduino?) 3.3V <=> 3.3V from module" ).grid(column= 0, row=11, padx= 15 , pady= 0 , sticky=(W))
ttk.Label(fLocator, text="Be careful as Arduino 3.3V LDO may not be sufficent for powerful modules" ).grid(column= 0, row=12, padx= 5 , pady= 0, sticky=(W))
