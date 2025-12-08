import numpy as np

def sliding_window_view(arr, window_size):
    """Efficient sliding window view."""
    shape = arr.shape[:-1] + (arr.shape[-1] - window_size + 1, window_size)
    strides = arr.strides + (arr.strides[-1],)
    return np.lib.stride_tricks.as_strided(arr, shape=shape, strides=strides)

def calculate_rms(signal, window_size):
    if len(signal) < window_size: return np.array([])
    windows = sliding_window_view(signal**2, window_size)
    return np.sqrt(np.mean(windows, axis=1))

def calculate_var(signal, window_size):
    if len(signal) < window_size: return np.array([])
    windows = sliding_window_view(signal, window_size)
    return np.var(windows, axis=1)

def calculate_wl(signal, window_size):
    if len(signal) < window_size: return np.array([])
    abs_diff = np.abs(np.diff(signal))
    windows = sliding_window_view(abs_diff, window_size - 1)
    return np.sum(windows, axis=1)