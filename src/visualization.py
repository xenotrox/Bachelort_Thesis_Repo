import matplotlib.pyplot as plt
import shutil
import os
import sys

# Try to import config, handle running from different dirs
try:
    from src import config
except ImportError:
    # Fallback if run directly or path issues
    class Config:
        ENABLE_LATEX = True
        SAVE_PLOTS = True
    config = Config()

def set_plot_style():
    """
    Sets the plot style based on config and system capabilities.
    Returns: Boolean indicating if LaTeX is active.
    """
    # 1. Check User Config
    wants_tex = config.ENABLE_LATEX
    
    # 2. Check System Capability
    has_tex = shutil.which('latex') is not None
    
    # 3. Decide
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
        'xtick.labelsize': 11,
        'ytick.labelsize': 11,
        'lines.linewidth': 1.2,
        'axes.prop_cycle': plt.cycler(color=['slategray', 'tab:blue', 'tab:orange', 'tab:green'])
    })
    
    status = "ENABLED" if use_tex else "DISABLED"
    reason = "" if use_tex else ("(System missing LaTeX)" if wants_tex else "(Disabled in config)")
    print(f"Plotting Style: LaTeX {status} {reason}")
    return use_tex

def save_fig(fig, filename):
    """
    Saves the figure safely.
    - If LaTeX is ON: Saves to results/figures/ (Production)
    - If LaTeX is OFF: Saves to results/drafts/ (Draft) to prevent overwriting thesis plots.
    """
    if not config.SAVE_PLOTS:
        print("Skipping save (SAVE_PLOTS = False)")
        return

    # Check actual matplotlib state, not just config
    is_latex_active = plt.rcParams['text.usetex']
    
    if is_latex_active:
        output_dir = os.path.join("..", "results", "figures")
        msg = "PRODUCTION"
    else:
        output_dir = os.path.join("..", "results", "drafts")
        msg = "DRAFT (LaTeX disabled)"

    os.makedirs(output_dir, exist_ok=True)
    path = os.path.join(output_dir, filename)
    
    fig.tight_layout()
    fig.savefig(path, dpi=300)
    print(f"Saved [{msg}]: {path}")