import yfinance as yf
import sys
import numpy as np
import pandas as pd
import yfinance as yf
import datetime as dt

"""
This file takes the arguments ticker and days to calculate the VaR (Value at Risk). The VaR is printed as a number between -1 and 1 and represents a percentage. If the 1d VaR of a stock is -2.5%, there is a 5% probability of losing 2,5% of an investment
Arguments:
- Ticker: String, remember to use suffix '.OL' after ticker for stocks on the OSE
- days: String, user chooses between daily, 5-day or 20-day VaR
"""

def calculate_var(returns, confidence_level=0.95):
    return np.percentile(returns, (1-confidence_level)*100)

if __name__ == "__main__":
    # Check if the correct number of command-line arguments is provided
    if len(sys.argv) > 3 or len(sys.argv) < 2:
        raise Exception("Manglende argumenter")

    # Extract command-line arguments
    ticker = sys.argv[1]
    time_slot = sys.argv[2]

    available_time_slot = "1d, 1wk, 1mo, ytd, 1y, 3y, 5y, 10y".split(", ")

    if time_slot not in available_time_slot:
        raise Exception("Ikke støttet tidsvindu. Please choose from: 1d, 1wk, 1mo, ytd, 1y, 3y, 5y, 10y")
    
    today = dt.datetime.now()
    start_date = dt.datetime(2005, 12, 31)

    try:
        data = yf.download(ticker, start=start_date, end=today)
        if time_slot == '1d':
            var = calculate_var(data['Adj Close'].pct_change().dropna())
        elif time_slot == '1wk':
            var = calculate_var(data['Adj Close'].pct_change(periods=7).dropna())
        elif time_slot == '1mo':
            var = calculate_var(data['Adj Close'].pct_change(periods=31).dropna())
        elif time_slot == 'ytd':
            year = today.year
            first = dt.datetime(year,1,1)
            diff = today.day - first.day
            var = calculate_var(data['Adj Close'].pct_change(periods=diff).dropna())
        elif time_slot == '1y':
            var = calculate_var(data['Adj Close'].pct_change(periods=365).dropna())
        elif time_slot == '3y':
            var = calculate_var(data['Adj Close'].pct_change(periods=365*3).dropna())
        elif time_slot == '5y':
            var = calculate_var(data['Adj Close'].pct_change(periods=365*5).dropna())
        elif time_slot == '10y':
            var = calculate_var(data['Adj Close'].pct_change(periods=365*10).dropna())
        print(var)
    except IndexError:
        print('Ikke nok data tilgjengelig til å regne VaR. Velg en kortere tidsperiode')
    except:
        print('Noe er galt med ticker. Har du husket .OL bak norske aksjer? Det kan også hende aksjen ikke er tilgjengelig på yfinance for øyeblikket')
    
        

       
    


    