from oxsConfig import ttk, W , E , fSequence, SeqUnitVar, SeqOut6Var, SeqOut5Var, SeqOut4Var, SeqOut3Var, SeqOut2Var, SeqOut1Var
from oxsConfig import SeqB5Var, SeqB4Var, SeqB3Var, SeqB2Var, SeqB1Var, SeqB0Var, SeqTVar, SeqMinVoltVar, SeqMinCellVar


#//#define SEQUENCE_OUTPUTS 0b100000  
##define SEQUENCE_UNIT 2
##define SEQUENCE_m100    1 , 0b100000 , 3 , 0b000000 , 1 , 0b100000 , 3 , 0b000000 
#//#define SEQUENCE_m75     1 , 0b100000 , 1 , 0b000000 , 2 , 0b100000 , 2 , 0b000000
#//#define SEQUENCE_m50     5 , 0b100000 , 5 , 0b000000
#//#define SEQUENCE_m25     5 , 0b100000 , 5 , 0b000000 , 0 , 0b100000
##define SEQUENCE_0       3 , 0b100000 , 1 , 0b000000
#//#define SEQUENCE_25      2 , 0b100000 , 2 , 0b000000
#//#define SEQUENCE_50      5 , 0b100000 , 5 , 0b000000
#//#define SEQUENCE_75      7 , 0b100000 , 7 , 0b000000
##define SEQUENCE_100     8 , 0b100000 , 16 , 0b000000 , 24 , 0b100000 , 32 , 0b000000  
#//#define SEQUENCE_LOW    10 , 0b100000 ,10 , 0b000000   // sequence for Low voltage
#//#define SEQUENCE_MIN_VOLT_6 4000 // sequence_100 will be activated if voltage 6 is lower that the value.
#//#define SEQUENCE_MIN_CELL   3000 // sequence_100 will be activated if lowest cell is lower that the value.


ttk.Label(fSequence, text="Seq. Unit" ).grid(column= 0, row=3, padx= 1 , pady= 1, columnspan=2, sticky=(W))
ttk.Entry(fSequence, textvariable=SeqUnitVar , width='15', justify='right').grid(column=2 , row=3, columnspan=6, sticky=(E))
ttk.Label(fSequence, text="D13" ).grid(column= 2, row=4, padx= 0 , pady= 1, sticky=(W))
ttk.Label(fSequence, text="D12" ).grid(column= 3, row=4, padx= 0 , pady= 1, sticky=(W))
ttk.Label(fSequence, text="D11" ).grid(column= 4, row=4, padx= 0 , pady= 1, sticky=(W))
ttk.Label(fSequence, text="D10" ).grid(column= 5, row=4, padx= 0 , pady= 1, sticky=(W))
ttk.Label(fSequence, text="D09" ).grid(column= 6, row=4, padx= 0 , pady= 1, sticky=(W))
ttk.Label(fSequence, text="D08" ).grid(column= 7, row=4, padx= 0 , pady= 1, columnspan=2, sticky=(W))

ttk.Label(fSequence, text="Output" ).grid(column= 0, row=5, padx= 1 , pady= 1, columnspan=2, sticky=(W,E))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut6Var, onvalue='1', offvalue='0').grid(column=2, row=5, padx=0, pady=1, sticky=(W))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut5Var, onvalue='1', offvalue='0').grid(column=3, row=5, padx=0, pady=1, sticky=(W))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut4Var, onvalue='1', offvalue='0').grid(column=4, row=5, padx=0, pady=1, sticky=(W))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut3Var, onvalue='1', offvalue='0').grid(column=5, row=5, padx=0, pady=1, sticky=(W))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut2Var, onvalue='1', offvalue='0').grid(column=6, row=5, padx=0, pady=1, sticky=(W))
ttk.Checkbutton(fSequence, text=" ", variable=SeqOut1Var, onvalue='1', offvalue='0').grid(column=7, row=5, padx=0, pady=1, columnspan=2, sticky=(W))


ttk.Label(fSequence, text="Sequence -100" , justify='center').grid(column= 0, row=6, padx= 1 , pady= 1, columnspan=7, sticky=(W))
ttk.Label(fSequence, text="Sequence 0" , justify='center').grid(column= 7, row=6, padx= 1 , pady= 1, columnspan=7, sticky=(W,E))
ttk.Label(fSequence, text="Sequence 100" , justify='center').grid(column= 14, row=6, padx= 1 , pady= 1, columnspan=7, sticky=(W,E))
ttk.Label(fSequence, text="Sequence LOW" , justify='center').grid(column= 21, row=6, padx= 1 , pady= 1, columnspan=7, sticky=(W,E))
for x in range(0,4):
    ttk.Label(fSequence, text="Time" ).grid(column= 0+(7*x), row=7, padx= 1 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D13" ).grid(column= 1+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D12" ).grid(column= 2+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D11" ).grid(column= 3+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D10" ).grid(column= 4+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D09" ).grid(column= 5+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))
    ttk.Label(fSequence, text="D08" ).grid(column= 6+(7*x), row=7, padx= 0 , pady= 1, sticky=(W))

for y in range(0,25):
    for x in range(0,4):
        ttk.Entry(fSequence, textvariable=SeqTVar[y*4+x] , width='5', justify='right').grid(column=0+(7*x), row=8+y, padx=10, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB5Var[y*4+x], onvalue='1', offvalue='0').grid(column=1+(7*x), row=8+y, padx=0, pady=1, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB4Var[y*4+x], onvalue='1', offvalue='0').grid(column=2+(7*x), row=8+y, padx=0, pady=1, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB3Var[y*4+x], onvalue='1', offvalue='0').grid(column=3+(7*x), row=8+y, padx=0, pady=1, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB2Var[y*4+x], onvalue='1', offvalue='0').grid(column=4+(7*x), row=8+y, padx=0, pady=1, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB1Var[y*4+x], onvalue='1', offvalue='0').grid(column=5+(7*x), row=8+y, padx=0, pady=1, sticky=(E))
        ttk.Checkbutton(fSequence, variable=SeqB0Var[y*4+x], onvalue='1', offvalue='0').grid(column=6+(7*x), row=8+y, padx=0, pady=1, sticky=(E))

ttk.Label(fSequence, text="Min Volt" ).grid(column= 0, row=34, padx= 1 , pady= 1, columnspan=2, sticky=(W))
ttk.Entry(fSequence, textvariable=SeqMinVoltVar , width='15', justify='right').grid(column=2 , row=34, columnspan=6, sticky=(E))
ttk.Label(fSequence, text="Min Cell" ).grid(column= 0, row=35, padx= 1 , pady= 1, columnspan=2, sticky=(W))
ttk.Entry(fSequence, textvariable=SeqMinCellVar , width='15', justify='right').grid(column=2 , row=35, columnspan=6, sticky=(E))
