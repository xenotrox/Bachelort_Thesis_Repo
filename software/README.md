# Analysis & Control Software

This directory contains the Python code and Jupyter Notebooks used for processing sEMG signals, training threshold models, and evaluating filter performance for the Bachelor Thesis.

## 📦 Prerequisites

This project requires **Python 3.x** and the following scientific libraries. You can install them via pip:

```bash
pip install numpy pandas scipy matplotlib jupyter
```
Note: If you wish to generate production-ready plots with LaTeX formatting, ensure you have a local LaTeX distribution installed (e.g., TeX Live or MiKTeX) and set ENABLE_LATEX = True in src/config.py.

## 📂 Directory Structure

- notebooks/: The main entry points for analysis. These are numbered to indicate the recommended execution order.
- src/: Helper modules containing the core logic (filters, feature calculation, data loading).
  - config.py: Global settings (sampling rate, constants, plotting options).
  - filters.py: Implementation of Notch and Butterworth filters.
  - features.py: Signal feature extractors (RMS, Variance, Waveform Length).
  - data_loader.py: Utilities for loading raw ADC text files and labeled CSVs.
- data/: Place your raw signal files (.txt) and labeled training data (.csv) here.
- results/: Generated plots, trained threshold values, and filter coefficients are saved here automatically.

## 🚀 Usage Guide

It is recommended to run the notebooks in the following order to replicate the full analysis pipeline:
### 1. Raw Signal Analysis

- File: 01_Raw_Signal_Analysis.ipynb
- Goal: Visual inspection of the raw sEMG signal directly from the ADC.
- Key Output: Plots showing the DC offset (approx. 1.5V) and baseline noise.

### 2. Filter Evaluation

- File: 02_Filter_Evaluation.ipynb
- Goal: Compare the "Original Filter" (Notch + Simple High/Low pass) against 4th and 6th Order Butterworth designs.
- Key Output: Power Spectral Density (PSD) plots and signal-to-noise comparisons to justify the final filter choice.

### 3. Model Training

- File: 03_Model_Training_Evaluation.ipynb
- Goal: Load labeled training sequences to determine optimal thresholds for muscle activation.
- Key Output: Generates trained_thresholds.csv in the results folder.

### 4. Feature Evaluation

- File: 04_Feature_Evaluation.ipynb
- Goal: Compare different feature extraction methods (RMS vs. Variance vs. Waveform Length) for responsiveness and stability.

### 5. Real-Time Control Simulation

- File: 05_RealTime_Control.ipynb
- Goal: Simulates the final control logic using the selected filter and trained thresholds on test data.
    
## ⚙️ Configuration

You can adjust global settings in src/config.py:
- SAVE_PLOTS: Set to True to save figures to results/drafts or results/figures.
- ENABLE_LATEX: Set to True to use LaTeX fonts for plots (requires LaTeX installation).
- FS: Sampling frequency (Default: 1000 Hz).
- V_REF: ADC Reference Voltage (Default: 5.0 V).
