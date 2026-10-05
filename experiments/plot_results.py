"""Draw the 8 PA2 graphs (mean latency + throughput per experiment) from results/exp<N>.csv.

Usage:  python plot_results.py            -> writes plots/exp<N>_latency.png and plots/exp<N>_throughput.png
"""
import csv
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

HERE = os.path.dirname(os.path.abspath(__file__))
RESULTS = os.path.join(HERE, "results")
PLOTS = os.path.join(HERE, "plots")

TITLES = {
    1: "Exp 1 (regular robots)",
    2: "Exp 2 (special, 1 expert)",
    3: "Exp 3 (special, 16 experts)",
    4: "Exp 4 (special, experts = customers)",
}

# light chart surface, text inks and categorical slot 1 (dataviz reference palette)
SURFACE, INK, INK_2, GRID, SERIES = "#fcfcfb", "#0b0b0b", "#52514e", "#e4e3df", "#2a78d6"


def load(exp):
    with open(os.path.join(RESULTS, f"exp{exp}.csv")) as f:
        rows = sorted(csv.DictReader(f), key=lambda r: int(r["customers"]))
    customers = [int(r["customers"]) for r in rows]
    avg = [float(r["avg_us"]) for r in rows]
    thr = [float(r["throughput"]) for r in rows]
    return customers, avg, thr


def thousands(v, _pos):
    return f"{v / 1000:g}k" if v >= 1000 else f"{v:g}"


def draw(exp, x, y, ylabel, kind):
    fig, ax = plt.subplots(figsize=(5.2, 3.3), dpi=200)
    fig.patch.set_facecolor(SURFACE)
    ax.set_facecolor(SURFACE)

    ax.plot(x, y, color=SERIES, linewidth=2, marker="o", markersize=5,
            markeredgecolor=SURFACE, markeredgewidth=1.5, zorder=3)

    ax.set_xscale("log", base=2)
    ax.set_xticks(x)
    ax.set_xticklabels([str(c) for c in x])
    ax.set_ylim(bottom=0, top=max(y) * 1.15)
    ax.yaxis.set_major_formatter(FuncFormatter(thousands))

    # direct label on the last point only
    last = y[-1]
    label = f"{last / 1000:,.1f}k" if kind == "throughput" else f"{last:,.0f} µs"
    ax.annotate(label, (x[-1], last), textcoords="offset points", xytext=(-4, 8),
                ha="right", fontsize=8, color=INK)

    ax.set_title(f"{TITLES[exp]}, {'mean latency' if kind == 'latency' else 'throughput'}",
                 fontsize=10, color=INK, loc="left")
    ax.set_xlabel("number of customers (log scale)", fontsize=8.5, color=INK_2)
    ax.set_ylabel(ylabel, fontsize=8.5, color=INK_2)

    ax.grid(axis="y", color=GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(GRID)
    ax.tick_params(colors=INK_2, labelsize=8, length=0)

    fig.tight_layout()
    out = os.path.join(PLOTS, f"exp{exp}_{kind}.png")
    fig.savefig(out, facecolor=SURFACE)
    plt.close(fig)
    return out


def main():
    os.makedirs(PLOTS, exist_ok=True)
    for exp in (1, 2, 3, 4):
        x, avg, thr = load(exp)
        print(draw(exp, x, avg, "mean latency (µs)", "latency"))
        print(draw(exp, x, thr, "throughput (orders/s)", "throughput"))


if __name__ == "__main__":
    main()
