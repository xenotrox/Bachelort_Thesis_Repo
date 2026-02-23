# Source Code (src)

This directory contains the core Python modules used by the Jupyter Notebooks for analysis.

- **config.py**: Global configuration variables (e.g., 1000 Hz sampling rate, ADC voltage, plotting preferences).
- **data_loader.py**: Functions to parse raw `.txt` serial logs and labeled `.csv` training data into Pandas DataFrames.
- **features.py**: Implementations of feature extraction algorithms (Waveform Length, Root Mean Square, Variance).
- **filters.py**: Signal processing functions, including Notch filters for powerline noise and digital Butterworth bandpass filters.
- **visualization.py**: Helper functions for generating standardized matplotlib graphs.