#!/usr/bin/env python3
"""
Plot speedup charts for VIEW 1 and VIEW 2.

Reads CSVs:
  ./static/data/view1.csv
  ./static/data/view2.csv

Each CSV should have columns:
  t,serial_ms,thread_ms,speedup

Outputs PNGs into:
  ./static/diagram/
    - speedup_view1.png
    - speedup_view2.png
    - speedup_comparison.png  (both views overlayed)
    - time_view1.png          (optional: thread_ms vs threads)
    - time_view2.png
"""

import os
import sys
import pandas as pd
import matplotlib.pyplot as plt

DATA_DIR = os.path.join(".", "static", "data")
OUT_DIR  = os.path.join(".", "static", "diagram")

VIEW1_CSV = os.path.join(DATA_DIR, "view1.csv")
VIEW2_CSV = os.path.join(DATA_DIR, "view2.csv")

def load_csv(path: str) -> pd.DataFrame:
    if not os.path.exists(path):
        raise FileNotFoundError(f"CSV not found: {path}")
    df = pd.read_csv(path)
    # Normalize/validate columns
    expected_cols = ["t", "serial_ms", "thread_ms", "speedup"]
    missing = [c for c in expected_cols if c not in df.columns]
    if missing:
        raise ValueError(f"{path} is missing columns: {missing}. Found: {list(df.columns)}")
    # Ensure sorting by threads
    df = df.sort_values(by="t")
    return df

def ensure_outdir(path: str) -> None:
    os.makedirs(path, exist_ok=True)

def plot_speedup(df: pd.DataFrame, title: str, out_path: str) -> None:
    plt.figure(figsize=(8, 5))
    plt.plot(df["t"], df["speedup"], marker="o", label="Measured speedup")
    plt.plot(df["t"], df["t"], linestyle="--", label="Ideal linear (y = x)")
    plt.title(title)
    plt.xlabel("Threads")
    plt.ylabel("Speedup")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()

def plot_time(df: pd.DataFrame, title: str, out_path: str) -> None:
    plt.figure(figsize=(8, 5))
    plt.plot(df["t"], df["thread_ms"], marker="o", label="Parallel (mandelbrot thread)")
    plt.hlines(y=df["serial_ms"].iloc[0], xmin=df["t"].min(), xmax=df["t"].max(), linestyles="--", label="Serial baseline")
    plt.title(title)
    plt.xlabel("Threads")
    plt.ylabel("Time (ms)")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()

def plot_comparison(df1: pd.DataFrame, df2: pd.DataFrame, out_path: str) -> None:
    # Overlay speedup of view1 and view2 for comparison
    plt.figure(figsize=(8, 5))
    plt.plot(df1["t"], df1["speedup"], marker="o", label="VIEW 1")
    plt.plot(df2["t"], df2["speedup"], marker="o", label="VIEW 2")
    # Ideal line spans the union range of t
    t_min = min(df1["t"].min(), df2["t"].min())
    t_max = max(df1["t"].max(), df2["t"].max())
    plt.plot([t_min, t_max], [t_min, t_max], linestyle="--", label="Ideal linear (y = x)")
    plt.title("Speedup Comparison: VIEW 1 vs VIEW 2")
    plt.xlabel("Threads")
    plt.ylabel("Speedup")
    plt.grid(True, linestyle="--", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close()

def main(argv=None):
    ensure_outdir(OUT_DIR)

    df1 = load_csv(VIEW1_CSV)
    df2 = load_csv(VIEW2_CSV)

    # Individual speedup plots
    plot_speedup(df1, "Speedup vs Threads (VIEW 1)", os.path.join(OUT_DIR, "speedup_view1.png"))
    plot_speedup(df2, "Speedup vs Threads (VIEW 2)", os.path.join(OUT_DIR, "speedup_view2.png"))

    # Optional: time plots (parallel time vs threads)
    plot_time(df1, "Runtime vs Threads (VIEW 1)", os.path.join(OUT_DIR, "time_view1.png"))
    plot_time(df2, "Runtime vs Threads (VIEW 2)", os.path.join(OUT_DIR, "time_view2.png"))

    # Comparison overlay
    plot_comparison(df1, df2, os.path.join(OUT_DIR, "speedup_comparison.png"))

    print(f"Saved figures to {OUT_DIR}:\n"
          f" - speedup_view1.png\n"
          f" - speedup_view2.png\n"
          f" - time_view1.png\n"
          f" - time_view2.png\n"
          f" - speedup_comparison.png")

if __name__ == "__main__":
    sys.exit(main())
