#from tkinter import StringVar
from oxsConfig import ttk , W , E , fMagnet, GeneratemagCalData, xMagOffsetVar, yMagOffsetVar, zMagOffsetVar
from oxsConfig import xxMagCorrectionVar, xyMagCorrectionVar, xzMagCorrectionVar, yxMagCorrectionVar, yyMagCorrectionVar, yzMagCorrectionVar, zxMagCorrectionVar, zyMagCorrectionVar, zzMagCorrectionVar


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

fMagnet.grid(column= 1 , row=8) #by default make it visible

ttk.Checkbutton(fMagnet, text='Generate MAG Calibration data',  variable=GeneratemagCalData,
	    onvalue='On', offvalue='Off').grid(column=0, row=0, columnspan = 8, sticky=(W,E), padx=15 , pady=(10,2))
ttk.Label(fMagnet, text="XMAG_OFFSET" ).grid(column= 0, row=1, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="YMAG_OFFSET" ).grid(column= 0, row=2, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="ZMAG_OFFSET" ).grid(column= 0, row=3, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="XXMAG_CORRECTION" ).grid(column= 0, row=4, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="XYMAG_CORRECTION" ).grid(column= 0, row=5, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="XZMAG_CORRECTION" ).grid(column= 0, row=6, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="YXMAG_CORRECTION" ).grid(column= 0, row=7, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="YYMAG_CORRECTION" ).grid(column= 0, row=8, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="YZMAG_CORRECTION" ).grid(column= 0, row=9, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="ZXMAG_CORRECTION" ).grid(column= 0, row=10, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="ZYMAG_CORRECTION" ).grid(column= 0, row=11, padx= (15,5), pady=5 , sticky=(W))
ttk.Label(fMagnet, text="ZZMAG_CORRECTION" ).grid(column= 0, row=12, padx= (15,5), pady=5 , sticky=(W))
ttk.Entry(fMagnet, textvariable=xMagOffsetVar , width='5').grid(column=1 , row=1)
ttk.Entry(fMagnet, textvariable=yMagOffsetVar , width='5').grid(column=1 , row=2)
ttk.Entry(fMagnet, textvariable=zMagOffsetVar , width='5').grid(column=1 , row=3)
ttk.Entry(fMagnet, textvariable=xxMagCorrectionVar , width='5').grid(column=1 , row=4)
ttk.Entry(fMagnet, textvariable=xyMagCorrectionVar , width='5').grid(column=1 , row=5)
ttk.Entry(fMagnet, textvariable=xzMagCorrectionVar , width='5').grid(column=1 , row=6)
ttk.Entry(fMagnet, textvariable=yxMagCorrectionVar , width='5').grid(column=1 , row=7)
ttk.Entry(fMagnet, textvariable=yyMagCorrectionVar , width='5').grid(column=1 , row=8)
ttk.Entry(fMagnet, textvariable=yzMagCorrectionVar , width='5').grid(column=1 , row=9)
ttk.Entry(fMagnet, textvariable=zxMagCorrectionVar , width='5').grid(column=1 , row=10)
ttk.Entry(fMagnet, textvariable=zyMagCorrectionVar , width='5').grid(column=1 , row=11)
ttk.Entry(fMagnet, textvariable=zzMagCorrectionVar , width='5').grid(column=1 , row=12)
