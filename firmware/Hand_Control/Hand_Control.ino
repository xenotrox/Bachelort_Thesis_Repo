/**
 * Prosthetic Hand Control Logic
 * Signal: EMG/Sensor -> 6th Order Bandpass -> Waveform Length -> Servo Control
 * Features: Automatic timeout safety and sensor disconnection detection.
 */

// Library Imports
#include "Arduino.h"
#include "BandpassFilter6thOrder.h"
#include "Servo.h"

// Set to 0 to disable Serial output and speed up the loop
const bool DEBUG_MODE = 1;

// Pin Definitions
const uint8_t SERVO_PINS[5] = {3, 4, 5, 6, 7}; // D3-D7 {Pinky, Ring, Middle, Index, Thumb}
const uint8_t LED_OUT_OF_RANGE_PIN  = 12;      // Visual alert for loop timing issues
const uint8_t POTENTIOMETER_PIN  = 14;         // Sensitivity adjustment dial (A0)
const uint8_t SENSOR_PIN         = 15;         // Main EMG/Sensor input (A1)

// Servo configuration: default resting positions
const int openAngles[5] = {100, 110, 120, 120, 93}; // Specific angles for "Relaxed" pose
bool handStatus = 0;                                // Current state: 0 for Open, 1 for Closed
Servo fingers[5];

// Timing & Signal Processing Math
const int SAMPLE_RATE = 1000;                              // Aiming for 1kHz sampling
const unsigned long SAMPLE_PERIOD = 1000000 / SAMPLE_RATE; // Period in microseconds (1000us)
const int WINDOW_SIZE = 90;                                // Number of samples used to calculate signal strength

// Thresholds for Muscle Activation
const float THRESHOLD_MIN = 200;  // Minimum floor for activation
const byte SENSITIVITY = 5;       // Noise margin for the potentiometer dial

// Safety: Sensor Disconnection Logic (Detects if wire is pulled or floating)
const int UPPER_DISCONNECTION_THRESHOLD = 600;
const int LOWER_DISCONNECTION_THRESHOLD = 0;
const int COUNTER_MAX = 2000;     
const int DISCONNECT_TOLERANCE = 90;

// Motor Protection: Prevent stall heat by limiting contraction duration
const unsigned long MAX_CONTRACT_TIME = 5000;   // Release hand after 5s of continuous grip
const unsigned long COOLDOWN_MS = 1000;         // Mandatory 1s rest after a timeout

// State machine variables
unsigned long contractionStartTime = 0;
unsigned long pauseStartTime = 0;
bool isTimedOut = false;              // True if we are in the mandatory rest period
bool isCurrentlyContracting = false;  // Tracks if the muscle is actively engaged

int disconnect_counter = DISCONNECT_TOLERANCE;
int thresholdOld = THRESHOLD_MIN;
bool lastDetection = false;

// Optimization variables for running sum calculation
float currentWL = 0.0;
float lastFilteredValue = 0.0;

// Filter coefficients (generated for 6th order signal cleaning)
float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

BandpassFilter6thOrder thesisFilter;

// Data structure for the sliding window
float buffer[WINDOW_SIZE];
int bufferIndex = 0;
bool bufferFilled = false;

void setup() {
  // Setup hardware pins
  for (int i = 0; i < 5; i++) {
    fingers[i].attach(SERVO_PINS[i]);
  }

  Serial.begin(115200);

  // Ready the filter and clear the data buffer
  thesisFilter.init(sosCoeffs6th);
  for(int i = 0; i < WINDOW_SIZE; i++) buffer[i] = 0.0f;

  pinMode(LED_OUT_OF_RANGE_PIN, OUTPUT);

  open_hand();
}


// Adds new difference and removes oldest from the total
void updateWaveformLength(float newFilteredValue) {
  float oldestValue = buffer[bufferIndex];
  int prevToIndex = (bufferIndex + 1) % WINDOW_SIZE;
  float valueBeforeOldest = buffer[prevToIndex];

  // Remove the contribution of the data point that is leaving the window
  if (bufferFilled) {
    currentWL -= abs(oldestValue - valueBeforeOldest);
  }

  // Add the contribution of the newest data point
  currentWL += abs(newFilteredValue - buffer[bufferIndex == 0 ? WINDOW_SIZE - 1 : bufferIndex - 1]);

  buffer[bufferIndex] = newFilteredValue;
  bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;
  if (bufferIndex == 0) bufferFilled = true;
}

// Read the user-adjustable threshold poti
int getThreshold() {
  int currentRead = analogRead(POTENTIOMETER_PIN);
  int targetThreshold = map(currentRead, 0, 1023, THRESHOLD_MIN, 1000);

  // Small deadzone to prevent the threshold from flickering
  if (abs(targetThreshold - thresholdOld) >= SENSITIVITY) {
    thresholdOld = targetThreshold;
  } 
  return thresholdOld;
}

void loop() {
  unsigned long startTime = micros();

  // Data Acquisition
  int rawValue = analogRead(SENSOR_PIN);
  bool sensorDisconnected = updateSensorStatus(rawValue);
  float filteredValue = thesisFilter.update((float)rawValue);

  // Calculate WL
  updateWaveformLength(filteredValue);

  // Logic Execution (Only once the buffer is full)
  if (bufferFilled) {
    float wlValue = currentWL;
    int threshold = getThreshold();
    bool signalDetected = (wlValue >= threshold) && !sensorDisconnected;
    unsigned long now = millis();

    // -- State Machine: Manage Contraction & Safety Timers --
    
    if (isTimedOut) {
      // We are in a forced cooldown period
      if (now - pauseStartTime >= COOLDOWN_MS) {
        isTimedOut = false; // Cooldown finished, ready for next read
      }
      open_hand(); 
    } 
    else if (signalDetected) {
      // Muscle contraction is detected
      if (!isCurrentlyContracting) {
        contractionStartTime = now; // Start the 5-second stopwatch
        isCurrentlyContracting = true;
      }

      // If the user contracts for too long, force the hand open to protect motors
      if (now - contractionStartTime >= MAX_CONTRACT_TIME) {
        isTimedOut = true;
        pauseStartTime = now;
        isCurrentlyContracting = false;
        open_hand();
      } else {
        close_hand();
      }
    } 
    else {
      // No muscle contraction
      isCurrentlyContracting = false;
      open_hand();
    }

    if (DEBUG_MODE) {
      debug(rawValue, filteredValue, wlValue, threshold, signalDetected, sensorDisconnected,  isTimedOut);
    }   
  }

  // Ensures the loop runs at exactly the SAMPLE_RATE (1kHz)
  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < SAMPLE_PERIOD) {
    delayMicroseconds(SAMPLE_PERIOD - elapsedTime);
  }
}

// Logic to detect if the sensor is likely disconnected based on extreme voltage readings
bool updateSensorStatus(int raw) {
  if (raw >= UPPER_DISCONNECTION_THRESHOLD || raw <= LOWER_DISCONNECTION_THRESHOLD) {
    disconnect_counter++;
  } else {
    disconnect_counter--;
  }

  disconnect_counter = constrain(disconnect_counter, 0, COUNTER_MAX);

  // Returns true if the sensor has been "out of bounds" for a sustained period
  return (disconnect_counter > DISCONNECT_TOLERANCE);
}

// Actuate motors to the open (resting) position
void open_hand() {
  if (handStatus) { // Only send command if we aren't already open
    for (int i = 0; i < 5; i++) {
      fingers[i].write(openAngles[i]);
    }
    handStatus = 0;
  }
}

// Actuate motors to the closed (grip) position
void close_hand() {
  if (!handStatus) { // Only send command if we aren't already closed
    for (int i = 0; i < 5; i++) {
      fingers[i].write(0);
    }
    handStatus = 1;
  }
}

// Standard CSV-formatted output for Serial Plotter
void debug(int rawValue, float filteredValue, float wlValue, int threshold, int detection, bool sensorDisconnected,bool timedOut) {
    Serial.print(",Raw:");          Serial.print(rawValue);           
    Serial.print(",Filtered:");     Serial.print(filteredValue);
    Serial.print(",WL:");           Serial.print(wlValue);            
    Serial.print(",Threshold:");    Serial.print(threshold);    
    Serial.print(",Detection:");    Serial.print(detection * 1000);
    Serial.print(",Disconnected:"); Serial.print(sensorDisconnected * 1000);
    Serial.print(",Counter:");      Serial.print(disconnect_counter);
    Serial.print(",TimedOut:");     Serial.println(timedOut * 1000);
}