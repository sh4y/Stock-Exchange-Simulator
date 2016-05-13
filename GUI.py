from yahoo_finance import *
from tkinter import *
import main

class Window(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.master = master
        self.init_window()
        
    def init_window(self):
        self.master.title("Shay's Stock Exchange")
        self.pack(fill=BOTH, expand=1)
        qButton = Button(self, text="Quit", command=self.client_exit)
        qButton.place(x=0, y=0)

        '''self.stockBox = Text(self, height = 1, width = 5)
        self.stockBox.insert(END, "STOCK")
        self.stockBox.place(x=50,y=50)

        stockButton = Button(self, text="Go",command=self.lookup, height=1, width=5)
        stockButton.place(x=90,y=50)
    def lookup(self):
        share = self.stockBox.get("1.0", END)
        print(main.returnStockPrice(share))    '''

    def client_exit(self):
        exit()
        
        
root = Tk()
root.geometry("400x150")
app = Window(root)
root.mainloop()
