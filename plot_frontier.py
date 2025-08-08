import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV
df = pd.read_csv("/home/aaron/C++ save files/portfolios.csv")

# Find max Sharpe ratio portfolio
max_sharpe_idx = df["sharpe"].idxmax()
max_sharpe = df.loc[max_sharpe_idx]

plt.figure(figsize=(10, 6))
sc = plt.scatter(df["vol"], df["ret"], c=df["sharpe"], cmap="viridis", alpha=0.6)
plt.colorbar(sc, label="Sharpe Ratio")

# Highlight max Sharpe
plt.scatter(max_sharpe["vol"], max_sharpe["ret"], c="red", marker="*", s=200, label="Max Sharpe")

plt.xlabel("Volatility (σ)")
plt.ylabel("Expected Return (μ)")
plt.title("Random Portfolios (Efficient Frontier)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("efficient_frontier.png", dpi=300)
plt.show()
