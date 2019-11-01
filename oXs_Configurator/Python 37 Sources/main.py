from tkinter import *

class queryrunner(Tk):
    def __init__(self,parent):
        Tk.__init__(self,parent)
        self.parent = parent
        self.minsize(width=800,height=500)
        self.initialize()

    def initialize(self):
        #self.grid_columnconfigure(2,weight=1)
        #self.grid_columnconfigure(3,weight=1)
        #self.grid_columnconfigure(6,weight=2)
        #self.grid_rowconfigure(0,weight=1)
        #self.grid_rowconfigure(1,weight=1)
        #self.grid_rowconfigure(2,weight=1)
        #self.grid_rowconfigure(3,weight=1)
        #self.grid_rowconfigure(4,weight=1)
        # BUCKET AND N1QL LABEL + INPUT PAIRS: 
        #self.label = Label(self,text="Bucket", width=10, anchor="w")
        #self.label.grid(column=0,row=0,columnspan=1,sticky='W')
        #self.entry = Entry(self);
        #self.entry.grid(column=1,row=0, columnspan=3, sticky='EW')
        #self.entry.insert(0, "couchbase://couchbase1.mycompany.com/beer-sample" )
        # EXECUTE N1QL QUERY AGAINST BUCKET WHEN BUTTON CLICKED:
        #self.button = Button(self,text="Go",width=20)
        #self.button.grid(column=6,row=0)
        #self.label2 = Label(self,text="N1QL", anchor="w")
        #self.label2.grid(column=0,row=1,columnspan=1,sticky='W')
        #self.entry2 = Text(self,height=5);
        #self.entry2.grid(column=1,row=1, columnspan=3, rowspan=1, sticky='W')

        self.label3 = Label(self,text="Output:", width=50,anchor="w")
        self.label3.grid(column=0,row=4,columnspan=5,sticky='W')
        self.entry3 = Text(self,height=18)
        self.entry3.grid(column=1,row=5, columnspan=5, rowspan=1, sticky='W')
        # PROBLEM: GET SCROLLBAR TO BE THE RIGHT SIZE (IT'S NOT THE SIZE OF THE THING ITS BESIDE)
        self.scrollbarY = Scrollbar(self) # height= not permitted here!
        self.entry3.config(yscrollcommand= self.scrollbarY.set)
        self.scrollbarY.config(command= self.entry3.yview)

        self.scrollbarX = Scrollbar(self ,  orient=HORIZONTAL) # height= not permitted here!
        self.entry3.config(yscrollcommand= self.scrollbarX.set)
        self.scrollbarX.config(command= self.entry3.xview)



        self.grid()
        self.scrollbarY.grid(column=6, row=5, rowspan=5,  sticky='NSW')
        self.scrollbarX.grid(column=1, row=6, columnspan=5,  sticky='SEW')


if __name__ == "__main__":
    app = queryrunner(None)
    #font.nametofont('TkDefaultFont').configure(size = 10 )
    app.title('Couchbase N1QL Query Runner')
    app.mainloop()

#import tkinter as tk
#from tkinter import X, Y, BOTTOM, RIGHT, LEFT, Y, HORIZONTAL
#class TextExample(tk.Frame):
#    def __init__(self, master=None):
#        super().__init__()
#
#       sy = tk.Scrollbar(self)
#       sx = tk.Scrollbar(self,  orient=HORIZONTAL)
#       editor = tk.Text(self, height=500, width=300, wrap='none')
#       sx.grid(column = 1 , row=2, padx=5 , pady=5)
#       sy.grid(column = 2 , row=1, padx=5 , pady=5)
#       editor.grid(column = 1 , row=1, padx=5 , pady=5)
        #sx.pack(side=BOTTOM, fill=X)
        #sy.pack(side=RIGHT, fill=Y)
        #editor.pack(side=LEFT, fill=Y)
#        sy.config(command=editor.yview)
#        sx.config(command=editor.xview)
#       #self.pack()
#        self.grid()
#def main():
#    root = tk.Tk()
#    root.geometry("800x500+0+0")
#    app = TextExample(master=root)
#    root.mainloop()  
#if __name__ == '__main__':
#    main()  