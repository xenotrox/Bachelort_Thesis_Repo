# Hardware

This directory contains all the 3D models and CAD files needed to physically build the bionic hand and its display stand.

## Structure
- **InMoov/**: Contains the files for the robotic hand.
  - **Original/**: Unmodified STL files directly from the open-source InMoov project (Fingers, Forearm, RightHand, RotationWrist).
  - **Remix/**: Modified parts customized for this project, including a reinforced forearm with 5 mm bolt holes and updated servo pulleys for the tendon system.
- **Custom Stand/**: Original CAD designs for the educational display stand, including the Arduino holder, bottom box and top box.
- **Printer Calibration/**: Small test STLs used to dial in the 3D printer's Z-offset and dimensional accuracy before printing the final parts.

## Printing Notes
All parts are designed to be printed in PLA. Please refer to the specific README files in the subdirectories (e.g., `InMoov/Remix/Forearm/README.txt`) for specific infill and speed recommendations.