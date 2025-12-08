import numpy as np
import pandas as pd
from pathlib import Path

def _load_and_select_column(path):
    """Helper to load generic text data, handling delimiters and columns."""
    try:
        # Try loading as comma-separated
        data = np.loadtxt(path, delimiter=',')
    except ValueError:
        # Fallback to whitespace default
        data = np.loadtxt(path)
        
    # If the file has multiple columns (2D array), take the first one (Index 0)
    if data.ndim > 1:
        return data[:, 0]
    return data

def load_txt_signal(filepath, v_ref=5.0, adc_bits=10):
    """
    Loads a single-column ADC TXT file, converts to Voltage, and centers (removes DC).
    Use this for filter and feature evaluation where offset doesn't matter.
    """
    try:
        path = Path(filepath)
        if not path.exists():
            print(f"File not found: {path}")
            return None
            
        # Use robust loader
        data = _load_and_select_column(path)
        
        max_adc = (2**adc_bits) - 1
        voltage = (data / max_adc) * v_ref
        
        # Center signal around 0V
        return voltage - np.mean(voltage)
    except Exception as e:
        print(f"Error loading {filepath}: {e}")
        return None

def load_txt_signal_raw_offset(filepath, v_ref=5.0, adc_bits=10):
    """
    Loads a single-column ADC TXT file and converts to Voltage.
    KEEPS the DC offset (does not subtract mean).
    Use this for Figure 6 (Raw Signal) to show the 1.5V bias.
    """
    try:
        path = Path(filepath)
        if not path.exists():
            print(f"File not found: {path}")
            return None
            
        # Use robust loader
        data = _load_and_select_column(path)
        
        max_adc = (2**adc_bits) - 1
        return (data / max_adc) * v_ref
    except Exception as e:
        print(f"Error loading {filepath}: {e}")
        return None

def load_labeled_csv(filepath):
    """
    Loads CSV with 'RawValue' and 'Label' columns.
    Used for Feature Evaluation (Figure 10).
    """
    try:
        path = Path(filepath)
        if not path.exists():
            print(f"File not found: {path}")
            return None
            
        df = pd.read_csv(path)
        
        # Standardize labels (contract/relax -> 1/0)
        label_map = {'contract': 1, 'relax': 0}
        if 'Label' in df.columns:
             df['LabelNumeric'] = df['Label'].astype(str).str.strip().str.lower().map(label_map).fillna(0).astype(int)
        return df
    except Exception as e:
        print(f"Error loading CSV {filepath}: {e}")
        return None