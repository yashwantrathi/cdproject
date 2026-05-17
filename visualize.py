import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load CSV
df = pd.read_csv("results/results.csv")

# Create comparison label
df["Comparison"] = df["Function1"] + " vs " + df["Function2"]

# =========================
# BAR GRAPH
# =========================

plt.figure(figsize=(10,5))

plt.bar(df["Comparison"], df["Similarity"])

plt.xticks(rotation=20)

plt.title("Clone Similarity Scores")

plt.ylabel("Similarity %")

plt.tight_layout()

plt.savefig("results/similarity_bar.png")

print("Saved similarity_bar.png")

# =========================
# HEATMAP
# =========================

pivot = df.pivot_table(
    index="Function1",
    columns="Function2",
    values="Similarity"
)

plt.figure(figsize=(6,5))

sns.heatmap(
    pivot,
    annot=True,
    cmap="coolwarm"
)

plt.title("Clone Similarity Heatmap")

plt.tight_layout()

plt.savefig("results/similarity_heatmap.png")

print("Saved similarity_heatmap.png")