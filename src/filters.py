import numpy as np
import os
from scipy.signal import butter, filtfilt, iirnotch, sosfilt

def generate_butterworth_coeffs(order, fs, low, high, output_dir):
    """Generates and saves SOS coefficients if they don't exist."""
    os.makedirs(output_dir, exist_ok=True)
    filepath = os.path.join(output_dir, f"butterworth_{order}th_coeffs.npy")
    if not os.path.exists(filepath):
        print(f"Generating {order}th order coefficients...")
        sos = butter(order, [low, high], btype='bandpass', output='sos', fs=fs)
        np.save(filepath, sos)
    return filepath

def apply_original_filter(raw_data, fs=1000):
    """Notch(50Hz) -> HP(20Hz) -> LP(150Hz)"""
    data = np.array(raw_data, dtype=float)
    b_notch, a_notch = iirnotch(50, Q=30.0, fs=fs)
    data = filtfilt(b_notch, a_notch, data)
    b_high, a_high = butter(4, 20, btype='high', fs=fs)
    data = filtfilt(b_high, a_high, data)
    b_low, a_low = butter(4, 150, btype='low', fs=fs)
    data = filtfilt(b_low, a_low, data)
    return data

def apply_butterworth_sos(raw_data, order, fs=1000, low=65, high=135, coeffs_dir="filter_coeffs"):
    """Applies Butterworth Bandpass (65-135Hz)."""
    coeff_file = generate_butterworth_coeffs(order, fs, low, high, coeffs_dir)
    sos = np.load(coeff_file)
    return sosfilt(sos, raw_data)