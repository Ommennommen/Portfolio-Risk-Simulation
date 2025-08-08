# Portfolio Risk Simulation – Efficient Frontier

This project simulates portfolio performance using **Monte Carlo simulation in C++** and visualises the resulting **Efficient Frontier** in Python.

## Overview
The Efficient Frontier, a concept from Modern Portfolio Theory, represents the set of optimal portfolios that offer the highest expected return for a given level of risk.  
This project:
- Reads historical daily returns from a CSV
- Simulates thousands of random portfolios
- Calculates expected return, volatility, and Sharpe ratio
- Visualises the efficient frontier and highlights the max-Sharpe portfolio

---

## Data
- **Assets:** SPY (S&P 500 ETF), QQQ (Nasdaq-100 ETF), GLD (Gold ETF)  
- **Period:** Last 2 years of daily adjusted closes  
- **Source:** [Yahoo Finance](https://finance.yahoo.com) via `yfinance` (Python)

Daily returns are saved to `returns.csv` and used as input to the C++ simulation.

---

## Methodology

### 1. Data Preparation (Python)
- Download historical prices using `yfinance`
- Calculate daily returns
- Save to `returns.csv`

### 2. Monte Carlo Simulation (C++)
- Read `returns.csv`
- Compute mean returns vector & covariance matrix
- Generate `N` random portfolios (weights sum to 1)
- Calculate:
  - Expected return
  - Volatility (σ)
  - Sharpe ratio

- Save results to `portfolios.csv`

### 3. Visualisation (Python)
- Plot portfolios as a scatter plot coloured by Sharpe ratio
- Highlight the **maximum Sharpe** portfolio with a red star
- Label axes: Volatility (σ) vs Expected Return (μ)

---

## How to Run

### Prerequisites
- **C++ compiler** (g++)
- **Python 3** with `yfinance`, `pandas`, `matplotlib`

### Steps
1. **Get returns data (Python)**:
   ```bash
   python make_returns.py
