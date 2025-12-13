import yfinance as yf
data = yf.download("RELIANCE.BO", start="2020-01-01", end="2025-12-13")
data.to_csv("reliance.csv")