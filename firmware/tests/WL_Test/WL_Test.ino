#include "Arduino.h"
#include "BandpassFilter6thOrder.h"

#define POTENTIOMETER_PIN A0
#define SENSOR_PIN A1


const int SAMPLE_RATE = 1000;
const unsigned long SAMPLE_PERIOD = 1000000 / SAMPLE_RATE;
const int WINDOW_SIZE = 90; 

const int ledOutOfRange = 12;
const int ledContractionDetected = 13;

const float thresholdMin = 200;
const byte sensitivity = 5;

int thresholdOld = thresholdMin;

float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

BandpassFilter6thOrder thesisFilter;

float buffer[WINDOW_SIZE];
int bufferIndex = 0;
bool bufferFilled = false;

void setup() {
  Serial.begin(115200);
  thesisFilter.init(sosCoeffs6th);
  
  pinMode(ledOutOfRange, OUTPUT);
  pinMode(ledContractionDetected, OUTPUT);

  for(int i = 0; i < WINDOW_SIZE; i++) buffer[i] = 0.0f;
}

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

void loop() {
  unsigned long startTime = micros();

  int rawValue = analogRead(SENSOR_PIN);
  float filteredValue = thesisFilter.update((float)rawValue);

  buffer[bufferIndex] = filteredValue;
  bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;
  if (bufferIndex == 0) bufferFilled = true;

  if (bufferFilled) {
    float wlValue = calculateWaveformLength();
    int threshold = getThreshold();
    int detection = (wlValue >= threshold) ? 1 : 0;

    if (detection) {
      digitalWrite(ledContractionDetected, HIGH);
    } else {
      digitalWrite(ledContractionDetected, LOW);
    }

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
    Serial.print("Detection:");
    Serial.println(detection * 100); 
  }

  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < SAMPLE_PERIOD) {
    delayMicroseconds(SAMPLE_PERIOD - elapsedTime);
  }
}