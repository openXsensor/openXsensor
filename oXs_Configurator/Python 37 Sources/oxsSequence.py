from oxsConfig import ttk , W , E , fSequence

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


ttk.Label(fSequence, text="Not yet implemented (so you have to edit manually the config files)" ).grid(column= 0, row=4, padx= 1 , pady= 20)
