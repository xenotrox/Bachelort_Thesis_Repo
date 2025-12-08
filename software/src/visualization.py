import matplotlib.pyplot as plt
import shutil
import os
import sys

try:
    from src import config
except ImportError:
    class Config: ENABLE_LATEX =True; SAVE_PLOTS = True
    config = Config()

# --- Defined Color Scheme ---
COLORS = {
    "Raw Signal": "gray",
    "Raw": "gray",
    "Original Filter": "tab:blue",
    "4th Order Butterworth": "tab:orange",
    "6th Order Butterworth": "tab:green",
    "RMS": "tab:blue",
    "VAR": "tab:orange",
    "WL": "tab:green"
}

def set_plot_style():
    wants_tex = config.ENABLE_LATEX
    has_tex = shutil.which('latex') is not None
    use_tex = wants_tex and has_tex
    
    font_family = 'serif' if use_tex else 'DejaVu Serif'
    
    plt.rcParams.update({
        'text.usetex': use_tex,
        'font.family': font_family,
        'axes.grid': True,
        'grid.alpha': 0.5,
        'grid.linestyle': '--',
        'font.size': 12,
        'axes.labelsize': 14,
        'axes.titlesize': 16,
        'lines.linewidth': 1.2,
        # Set default cycle to match your specific order (Gray, Blue, Orange, Green)
        'axes.prop_cycle': plt.cycler(color=[COLORS["Raw"], COLORS["Original Filter"], COLORS["4th Order Butterworth"], COLORS["6th Order Butterworth"]])
    })
    
    print(f"Style set. LaTeX: {use_tex}")

def save_fig(fig, filename):
    if not config.SAVE_PLOTS: return

    if plt.rcParams['text.usetex']:
        output_dir = os.path.join("..", "results", "figures")
        msg = "PRODUCTION"
    else:
        output_dir = os.path.join("..", "results", "drafts")
        msg = "DRAFT"

    os.makedirs(output_dir, exist_ok=True)
    path = os.path.join(output_dir, filename)
    fig.tight_layout()
    fig.savefig(path, dpi=300)
    print(f"Saved [{msg}]: {path}")