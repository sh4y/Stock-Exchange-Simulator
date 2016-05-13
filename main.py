import pickle
from Stock import *

class Portfolio:
    def __init__(self):
        self.portfolio = []
        self.cash = 0
        
    
    def savePortfolio(self, filename):
        f = open(filename, 'wb')
        pickle.dump(self, f)
        f.close()
        
    
    def loadPortfolio(filename):
        f = open(filename)
        temp = pickle.load(f)
        f.close()
        return temp

    def addCash(self, c):
        self.cash += c

    def addStock(self, Stock):
        self.portfolio.append(Stock)
