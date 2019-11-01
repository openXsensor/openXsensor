from tkinter import StringVar
from oxsConfig import ttk , W , E , fMagnet


#define GENERATE_MAG_CALIBRATION_DATA   // uncomment this line when HMC5883 calibration has to be performed. Set back as comment once calibration parameters have been introduced 

#define    XMAG_OFFSET 2.4683     // must be an integer
#define    YMAG_OFFSET -1.3694     // must be an integer
#define    ZMAG_OFFSET 138.9683     // must be an integer
#define    XXMAG_CORRECTION  0.122082   // can have decimals
#define    XYMAG_CORRECTION  -0.00204026
#define    XZMAG_CORRECTION  0.00377534 
#define    YXMAG_CORRECTION  -0.00204026
#define    YYMAG_CORRECTION  0.130413
#define    YZMAG_CORRECTION  -0.00491189
#define    ZXMAG_CORRECTION  0.00377534
#define    ZYMAG_CORRECTION  -0.00491189
#define    ZZMAG_CORRECTION  0.138038



ttk.Label(fMagnet, text="Not yet implemented (so you have to edit manually the config files)" ).grid(column= 0, row=4, padx= 1 , pady= 20)
#dummyMagnetVar = StringVar(value='-')
#dummyBox = ttk.Combobox(fMagnet, textvariable=dummyMagnetVar, 
#                values=( "-"  ),
#                state="readonly")
