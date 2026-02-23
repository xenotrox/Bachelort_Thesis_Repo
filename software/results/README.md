# Results

This directory is the output destination for the Python analysis scripts.

- **drafts/**: Contains generated PNG plots (e.g., PSD comparisons, raw signal visualizations, feature accuracy graphs). 
- **filter_coeffs/**: Saved NumPy array files (`.npy`) containing the calculated A and B coefficients for the 4th and 6th-order digital Butterworth filters, ready to be hardcoded into the Arduino C++ firmware.
- **thresholds/**: The final output of the training models. `trained_thresholds.csv` contains the calculated parameters for determining muscle activation states.