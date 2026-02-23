#include "Arduino.h"
#include "BandpassFilter6thOrder.h"

// --- Pin Definitions ---
#define POTENTIOMETER_PIN A0
#define SENSOR_PIN A1

const int ledOutOfRange = 12;
const int ledContractionDetected = 13;

// --- Signal Processing Constants ---
const int SAMPLE_RATE = 1000;
const unsigned long SAMPLE_PERIOD = 1000000 / SAMPLE_RATE;
const int WINDOW_SIZE = 90; 

const float thresholdMin = 200;
const byte sensitivity = 5;
const float IDLE_RANGE = 20.0;          // Your observed noise floor (+/- 20)
const float LOOSE_WL_THRESHOLD = 3500.0; // Based on your image showing ~4000 WL
const int RAW_MIN = 5;                   // Detection for signal hitting bottom rail
const int RAW_MAX = 1018;                // Detection for signal hitting top rail

// --- Timing Constants (ms) ---
const unsigned long CALIBRATION_TIME = 3000; 
const unsigned long MAX_CONTRACTION_TIME = 5000;
const unsigned long COOLDOWN_TIME = 2000;

// --- State Machine ---
enum SystemState { SENSOR_WAITING, READY, CONTRACTING, COOLDOWN, SENSOR_LOOSE };
SystemState currentState = SENSOR_WAITING;

// --- Global Variables ---
int thresholdOld = thresholdMin;
unsigned long stateTimer = 0;

float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

BandpassFilter6thOrder thesisFilter;

float buffer[WINDOW_SIZE];
int bufferIndex = 0;
bool bufferFilled = false;

// --- Functions ---

float calculateWaveformLength() {
  float wl = 0.0;
  for (int i = 1; i < WINDOW_SIZE; i++) {
    wl += abs(buffer[i] - buffer[i - 1]);
  }
  return wl;
}

int getThreshold() {
  int currentRead = analogRead(POTENTIOMETER_PIN);
  int targetThreshold = map(currentRead, 0, 1023, thresholdMin, 1000);

  if (abs(targetThreshold - thresholdOld) >= sensitivity) {
    thresholdOld = targetThreshold;
  } 
  return thresholdOld;
}

// --- Main Program ---

void setup() {
  Serial.begin(115200);
  thesisFilter.init(sosCoeffs6th);
  
  pinMode(ledOutOfRange, OUTPUT);
  pinMode(ledContractionDetected, OUTPUT);

  for(int i = 0; i < WINDOW_SIZE; i++) buffer[i] = 0.0f;
  
  stateTimer = millis();
}

void loop() {
  unsigned long startTime = micros();
  unsigned long now = millis();

  // 1. Data Acquisition
  int rawValue = analogRead(SENSOR_PIN);
  float filteredValue = thesisFilter.update((float)rawValue);

  // 2. Buffer Management
  buffer[bufferIndex] = filteredValue;
  bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;
  if (bufferIndex == 0) bufferFilled = true;

  if (bufferFilled) {
    float wlValue = calculateWaveformLength();
    int threshold = getThreshold();

    // 3. Global "Loose Sensor" Detection
    // Triggers if WL is abnormally high OR signal is clipping at 0 or 1023
    if (wlValue > LOOSE_WL_THRESHOLD || rawValue <= RAW_MIN || rawValue >= RAW_MAX) {
      if (currentState != SENSOR_LOOSE) {
        currentState = SENSOR_LOOSE;
        digitalWrite(ledContractionDetected, LOW);
        digitalWrite(ledOutOfRange, HIGH); 
      }
    }

    // 4. State Machine Logic
    switch (currentState) {
      
      case SENSOR_LOOSE:
        // Recovery: Signal must be steady within idle range and not clipping
        if (abs(filteredValue) < IDLE_RANGE && rawValue > RAW_MIN && rawValue < RAW_MAX) {
          currentState = SENSOR_WAITING;
          stateTimer = now;
          digitalWrite(ledOutOfRange, LOW);
        }
        break;

      case SENSOR_WAITING:
        // Must stay in range for 3 seconds
        if (abs(filteredValue) < IDLE_RANGE) {
          if (now - stateTimer >= CALIBRATION_TIME) {
            currentState = READY;
          }
        } else {
          stateTimer = now; // Reset timer if signal spikes
        }
        break;

      case READY:
        if (wlValue >= threshold) {
          currentState = CONTRACTING;
          stateTimer = now;
          digitalWrite(ledContractionDetected, HIGH);
        }
        break;

      case CONTRACTING:
        // Stop if muscle relaxes OR we exceed 5 seconds
        if (wlValue < threshold || (now - stateTimer >= MAX_CONTRACTION_TIME)) {
          currentState = COOLDOWN;
          stateTimer = now;
          digitalWrite(ledContractionDetected, LOW);
        }
        break;

      case COOLDOWN:
        // Forced 2-second wait before next contraction is allowed
        if (now - stateTimer >= COOLDOWN_TIME) {
          currentState = READY;
        }
        break;
    }

    // 5. Output for Serial Plotter
    Serial.print("Raw:");
    Serial.print(rawValue);           
    Serial.print(",");
    Serial.print("Filtered:");
    Serial.print(filteredValue);
    Serial.print(",");
    Serial.print("WL:");      
    Serial.print(wlValue);            
    Serial.print(",");
    Serial.print("Threshold:");
    Serial.print(threshold);    
    Serial.print(",");
    Serial.print("State:"); 
    Serial.println(currentState * 100); // Scaled for plotter visibility
  }

  // 6. Timing Management
  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < SAMPLE_PERIOD) {
    delayMicroseconds(SAMPLE_PERIOD - elapsedTime);
  }
}