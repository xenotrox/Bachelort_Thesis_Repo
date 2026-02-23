#include "Arduino.h"
#include "BandpassFilter6thOrder.h" 

// Hardware and Timing Definitions
#define SensorInputPin A1 
const int sampleRate = 1000; 
const unsigned long samplePeriod = 1000000 / sampleRate;

// Thesis Parameters: 90ms window size @ 1000Hz = 90 samples
const int windowSize = 90; 

// Thesis Feature Thresholds (derived from offline evaluation)
const float rmsThreshold = 0.0435; 
const float varThreshold = 0.0014; 
const float wlThreshold = 2.7;     

// Filter Coefficients for 6th-Order Butterworth (65-135 Hz)

float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

BandpassFilter6thOrder thesisFilter;


// Circular Buffer for Sliding Window Features
float buffer[windowSize];
int bufferIndex = 0;
bool bufferFilled = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize thesis-preferred filter 
  thesisFilter.init(sosCoeffs6th);
}

// RMS: Root Mean Square (Primary Feature)
// Note: Thesis mentioned using Mean Square (sumSquares/N) to save computation 
float calculateRMS() {
  float sumSquares = 0.0;
  for (int i = 0; i < windowSize; i++) {
    sumSquares += buffer[i] * buffer[i]; 
  }
  return sqrt(sumSquares / windowSize); 
}

// VAR: Variance (Found to be less stable due to window mean fluctuations) 
float calculateVariance() {
  float mean = 0.0;
  for (int i = 0; i < windowSize; i++) mean += buffer[i];
  mean /= windowSize;

  float sumVariance = 0.0;
  for (int i = 0; i < windowSize; i++) {
    sumVariance += pow(buffer[i] - mean, 2); 
  }
  return sumVariance / windowSize;
}

// WL: Waveform Length (Validated as highly correlated to RMS) 
float calculateWaveformLength() {
  float wl = 0.0;
  for (int i = 1; i < windowSize; i++) {
    wl += abs(buffer[i] - buffer[i - 1]); 
  }
  return wl;
}

void loop() {
  unsigned long startTime = micros();

  // 1. Sampling (10-bit ADC) 
  int rawValue = analogRead(SensorInputPin);
  
  // 2. 6th Order Filtering (Thesis Choice) 
  // Processing raw ADC values directly to maintain consistency with thesis evaluation
  float filteredValue = thesisFilter.update((float)rawValue);

  // 3. Update Sliding Window
  buffer[bufferIndex] = filteredValue;
  bufferIndex = (bufferIndex + 1) % windowSize;
  if (bufferIndex == 0) bufferFilled = true;

  if (bufferFilled) {
    // 4. Feature Computation 
    float rms = calculateRMS();
    float var = calculateVariance();
    float wl = calculateWaveformLength();

    // 5. Comparison Output (Matches Figures 10 & 11 in thesis)
    Serial.print("Raw:"); Serial.print(rawValue);
    Serial.print(",Filtered:"); Serial.print(filteredValue);
    Serial.print(",RMS:"); Serial.print(rms, 4);
    Serial.print(",VAR:"); Serial.print(var, 4);
    Serial.print(",WL:"); Serial.println(wl, 4);
    
  }

  // 6. Timing Discipline 
  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < samplePeriod) {
    delayMicroseconds(samplePeriod - elapsedTime);
  }
}