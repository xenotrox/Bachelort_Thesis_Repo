# Configuration Settings

# --- Plotting & LaTeX ---
# Set to True to attempt using LaTeX for plots. 
# If LaTeX is not found on the system, visualization.py will auto-disable this.
ENABLE_LATEX = True 

# Set to False to disable saving completely (e.g., for quick debugging)
SAVE_PLOTS = True

# --- System Constants (Centralized) ---
FS = 1000.0        # Sampling Frequency (Hz)
V_REF = 5.0        # Reference Voltage (V)
ADC_BITS = 10      # ADC Resolution

# --- Paths ---
# You can define global paths here if needed
DATA_DIR = "../data"
RESULTS_DIR = "../results"

