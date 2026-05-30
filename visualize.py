import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load CSV safely
df = pd.read_csv(
    "results/results.csv",
    on_bad_lines='skip'
)

# Use only last 4 comparisons
df = df.tail(4)

# Create comparison label
df["Comparison"] = (
    df["Function1"]
    + " vs "
    + df["Function2"]
)

# =========================
# BAR GRAPH
# =========================

plt.figure(figsize=(12,6))

colors = []

for sim in df["Similarity"]:

    if sim >= 70:
        colors.append("green")

    elif sim >= 40:
        colors.append("orange")

    else:
        colors.append("red")

bars = plt.bar(
    df["Comparison"],
    df["Similarity"],
    color=colors
)

# Add similarity labels
for bar in bars:

    height = bar.get_height()

    plt.text(
        bar.get_x() + bar.get_width()/2,
        height + 1,
        f"{height:.1f}%",
        ha='center'
    )

plt.xticks(rotation=15)

plt.title("Clone Similarity Scores")

plt.ylabel("Similarity %")

plt.ylim(0, 110)

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

if not pivot.empty:

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

else:

    print("Not enough data for heatmap.")