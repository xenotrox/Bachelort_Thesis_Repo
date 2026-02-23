# Software Data

This directory stores the raw and processed datasets used by the Jupyter notebooks.

- **raw_signal.txt / realtime_stream.txt**: Unprocessed ADC serial dumps directly from the Arduino.
- **labeled_training_data/**: Contains sequence CSVs (`train_seq_01.csv` to `train_seq_10.csv`) used to train the activation threshold models. These files include the filtered signal and a binary label indicating muscle contraction.
- **test_data/**: Contains diverse validation sets (e.g., fatigue tests, sudden dropouts, quick pulses) to evaluate the system's robustness in real-world scenarios.