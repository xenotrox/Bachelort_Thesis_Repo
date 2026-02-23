# Firmware Tests

This folder contains isolated Arduino sketches used for debugging and validating system components prior to integration into the main `Hand_Control` code.

- **Feature_Comparison/**: Compares calculation times for different signal features (RMS, Variance, WL).
- **Filter_Comparison/**: Tests 4th vs. 6th-order digital filters on the microcontroller.
- **Hand_Test/**: Actuates the InMoov servos to calibrate finger limits and tendon tension.
- **LED_Test / Potentiometer_Test**: Validates the user interface components.
- **Sensor_Connection**: Code to detect if the sEMG sensor has been physically disconnected from the user.
- **State_Machine / WL_Test**: Validates the core logic loop and Waveform Length math.