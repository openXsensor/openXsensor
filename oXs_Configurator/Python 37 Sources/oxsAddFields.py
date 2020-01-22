from oxsConfig import ttk , W , E , Spinbox ,fAddFields , addFieldsExist
from oxsConfig import fillTest3ExpectedAltitudeVar , fillTest3ExpectedAltitudeChanged , fFillTest3ExpectedAltitude , expAltTimeVar ,\
     fillTest123LinearAccVar , fillTest12VspeedAltVar , fillTest1DteVar , fillTest2PpmAirspeedCompVar ,\
     fillTest1YawRateVar , fillTest1HeadingVar , fillTest123FlowVar , fillTest1GpsNbrSatVar , fillTest2GpsHdopVar

#define FILL_TEST_3_WITH_EXPECTED_ALT                        // uncomment this line if oXs has to calculate an expected Alt in the future based on current Alt, Vspeed and vertical Acc
#define EXPECTED_ALT_AT_SEC 0.2                               // time (in sec) for expected Alt (this line must be uncommented wen previous is uncommented

#define FILL_TEST_1_2_3_WITH_LINEAR_ACC                     // uncomment this line if oXs has to fill TEST_1, TEST_2, TEST_3 with linear Acc

#define FILL_TEST_1_2_WITH_VSPEED_AND_ALT_FROM_SECOND_VARIO  // uncomment to activate this option

#define FILL_TEST_1_WITH_DTE                                 // uncomment to activate this option

#define FILL_TEST_2_WITH_PPM_AIRSPEED_COMPENSATION           // uncomment to activate this option

#define FILL_TEST_1_WITH_YAWRATE                             // uncomment to activate this option

#define FILL_TEST1_WITH_HEADING_FROM_MAGNETOMETER              // uncomment to activate this option

#define FILL_TEST_1_2_3_WITH_FLOW_SENSOR_CONSUMPTION             // uncomment to activate this option

#define FILL_TEST1_WITH_GPS_NUMBER_OF_SAT                      // uncomment to activate this option; note: when there is a GPS fix 3D (or higher), then number of sat is increased by 100

#define FILL_TEST2_WITH_GPS_HDOP                               // uncomment to activate this option

ttk.Checkbutton(fAddFields, text='Fill Test_3 with expected altitude',  variable=fillTest3ExpectedAltitudeVar , command=fillTest3ExpectedAltitudeChanged,
	    onvalue='On', offvalue='Off').grid(row=3, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Label(fFillTest3ExpectedAltitude, text="Exp.Alt to be calculated for" ).grid(column= 0, row=4, padx= 1 )
expAltTimeBox = Spinbox(fFillTest3ExpectedAltitude, from_=0.2, to=5.0, textvariable=expAltTimeVar, 
                        increment = '0.1', width=4)
expAltTimeBox.grid(column=1, row=4 , padx=10)
ttk.Label(fFillTest3ExpectedAltitude, text="sec in the future" ).grid(column= 2, row=4, padx= 1 )

ttk.Checkbutton(fAddFields, text='Fill Test_1 2 3 with linear Acc',  variable=fillTest123LinearAccVar ,
     onvalue='On', offvalue='Off').grid(row=5, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_1 2 with Vspeed and Alt from second sensor',  variable=fillTest12VspeedAltVar ,
     onvalue='On', offvalue='Off').grid(row=6, sticky=(W,E), padx=20 , pady=(20,2))


ttk.Checkbutton(fAddFields, text='Fill Test_1 with dtE (total energy vario)',  variable=fillTest1DteVar ,
     onvalue='On', offvalue='Off').grid(row=7, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_2 with PPM airspeed compensation',  variable=fillTest2PpmAirspeedCompVar ,
     onvalue='On', offvalue='Off').grid(row=8, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_1 with Yaw rate',  variable=fillTest1YawRateVar ,
     onvalue='On', offvalue='Off').grid(row=9, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_1 with Heading (from magnetometer)',  variable=fillTest1HeadingVar ,
     onvalue='On', offvalue='Off').grid(row=10, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_1 2 3 with Flow sensor consumption',  variable=fillTest123FlowVar ,
     onvalue='On', offvalue='Off').grid(row=11, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_1 with number of satellites',  variable=fillTest1GpsNbrSatVar ,
     onvalue='On', offvalue='Off').grid(row=12, sticky=(W,E), padx=20 , pady=(20,2))

ttk.Checkbutton(fAddFields, text='Fill Test_2 with GPS HDOP',  variable=fillTest2GpsHdopVar ,
     onvalue='On', offvalue='Off').grid(row=13, sticky=(W,E), padx=20 , pady=(20,2))
