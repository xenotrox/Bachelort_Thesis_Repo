# Configuration Settings
from pathlib import Path

# --- Plotting & LaTeX ---
ENABLE_LATEX = True 
SAVE_PLOTS = True

# --- System Constants ---
FS = 1000.0
V_REF = 5.0
ADC_BITS = 10

# --- Robust Pathing ---
# Finds the Project Root relative to this file (software/src/config.py)
# software/src/config.py -> parent -> parent -> software -> parent -> PROJECT_ROOT
SRC_DIR = Path(__file__).resolve().parent
SOFTWARE_DIR = SRC_DIR.parent
PROJECT_ROOT = SOFTWARE_DIR.parent

DATA_DIR = PROJECT_ROOT / "data"
RESULTS_DIR = PROJECT_ROOT / "results"

# Helper to ensure paths work as strings if needed
def get_data_path(filename):
    return DATA_DIR / filename