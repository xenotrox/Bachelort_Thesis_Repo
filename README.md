# Real-Time sEMG Control of a Bionic Hand (Bachelor Thesis)

This repository contains the complete hardware, firmware, and software implementation for a surface Electromyography (sEMG) controlled bionic hand. The project serves as a functioning prototype developed for a Bachelor Thesis, focusing on real-time signal processing, feature extraction, and motor control.

The physical hand is based on a remixed version of the open-source **InMoov** robotic hand, optimized for reliability and educational demonstration.

## 📂 Repository Structure

The project is divided into four main directories, each containing its own specific documentation:

- **`/doc`**: Contains the final academic thesis (`Bachelorarbeit-1.pdf`) and the presentation slides used for the defense.
- **`/firmware`**: Contains the C++ code for the microcontroller (Arduino). It handles the real-time acquisition of analog sEMG signals (sampled at 1000 Hz), applies digital Butterworth bandpass filters, calculates muscle activation thresholds (Waveform Length), and actuates the servos safely.
- **`/hardware`**: Contains all 3D printing files (STLs and CAD). This includes the original and remixed InMoov hand parts (reinforced forearm with 5 mm bolt holes, upgraded servo pulleys, teflon tube guides) as well as the custom-designed display stand and electronics housing.
- **`/software`**: Contains the data science and analysis pipeline. Includes Jupyter Notebooks and Python modules used for evaluating raw signal noise, comparing filter performances (4th vs. 6th-order), extracting features, and training the activation thresholds used by the firmware.

## 🚀 Getting Started

To replicate or understand this project, it is recommended to explore the sub-modules in the following order:

1. **Hardware Assembly**: See `/hardware` to print the PLA components and assemble the tendon-driven hand.
2. **Data Analysis**: Navigate to `/software` to see how raw sEMG signals were analyzed and how the filter coefficients and muscle activation thresholds were calculated.
3. **Microcontroller Setup**: Go to `/firmware` to flash the main control code onto the Arduino and start reading real-time muscle inputs.

## 🛠️ Key Technologies & Requirements
- **Microcontroller:** Arduino Uno (or compatible)
- **Programming Languages:** C++ (Firmware), Python 3.x (Software Analysis)
- **Data Analysis Libraries:** `numpy`, `pandas`, `scipy`, `matplotlib`, `jupyter`
- **3D Printing:** FDM Printer, PLA Filament

## 📜 Credits
The physical hand design is heavily based on the [InMoov open-source 3D printable robot](https://inmoov.fr/) created by Gael Langevin, utilizing remixes specifically tailored for improved tendon tension and forearm strength.