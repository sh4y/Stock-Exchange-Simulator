from googlefinance import getQuotes
import json

def truncate(f, n):
    '''Truncates/pads a float f to n decimal places without rounding'''
    s = '{}'.format(f)
    if 'e' in s or 'E' in s:
        return '{0:.{1}f}'.format(f, n)
    i, p, d = s.partition('.')
    return '.'.join([i, (d+'0'*n)[:n]])

class Stock:
    def __init__(self, symbol, qty):
        self.symbol = symbol
        self.data = getQuotes(self.symbol)[0]
        self.bp = float(self.data['LastTradePrice'])
        self.cp = self.bp
        self.q = qty
        self.ovalue = self.q * self.bp
        self.cvalue = self.q * self.cp
        
    def refresh(self):
        self.data = getQuotes(self.symbol)[0]
        self.cp = float(self.data['LastTradePrice'])
        self.cvalue = self.q * self.cp

    def printGains(self):
        self.gain = truncate(float(self.cvalue) - float(self.ovalue),2)
        self.pgain = truncate(float(float(self.gain)/float(self.ovalue))*100,2)
        print('{self.symbol}: {self.gain} ({self.pgain})%'.format(self=self))
