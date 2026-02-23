# Firmware

This directory contains all Arduino C++ code and related scripts required to operate the bionic hand prototype. 

## Structure
- **Hand_Control/**: The main production code for the Arduino Uno. It processes analog sEMG signals using a 6th-order Butterworth bandpass filter, calculates the Waveform Length (WL), and actuates the servo motors based on trained thresholds. It also includes `serial_plotter.py` for real-time visualization on a connected PC.
- **tests/**: A collection of isolated Arduino sketches used during development to verify individual components. Includes tests for LEDs, potentiometers, servo limits (`Hand_Test`), filter performance, and sensor connectivity.

## Setup
Upload the `Hand_Control.ino` sketch to your Arduino Uno using the Arduino IDE. Ensure the custom C++ filter libraries (`BandpassFilter6thOrder.h`/`.cpp`) are in the same directory.