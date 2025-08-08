import yfinance as yf
import pandas as pd

tickers = ["SPY", "QQQ", "GLD"]

# Download adjusted close prices directly
df = yf.download(tickers, period="2y", auto_adjust=True)["Close"]

# Calculate daily returns
rets = df.pct_change().dropna()
rets.index.name = "date"

# Save to CSV
rets.reset_index().to_csv("/home/aaron/C++ save files/returns.csv", index=False)
print("Wrote returns.csv with daily returns.")
