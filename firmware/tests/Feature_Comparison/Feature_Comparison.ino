#include "Arduino.h"
#include "EMGFilters.h"
#include "BandpassFilter4thOrder.h"
#include "BandpassFilter6thOrder.h"

#define SensorInputPin A1

EMGFilters originalFilter;
BandpassFilter4thOrder fourthOrderFilter;
BandpassFilter6thOrder sixthOrderFilter;

const int sampleRate = 1000; 
const unsigned long samplePeriod = 1000000 / sampleRate;

// 4th Order SOS Coeffs (from your provided file) 
float sosCoeffs4th[2][6] = {
  {0.03657484, 0.07314967, 0.03657484, 1.0, -1.20017304, 0.68088355},
  {1.0, -2.0, 1.0, 1.0, -1.6108166, 0.78896695}
};

// 6th Order SOS Coeffs for 65-135Hz @ 1000Hz
float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

void setup() {
  Serial.begin(115200);
  
  // Original Filter: Notch 50Hz, HP 20Hz, LP 150Hz 
  originalFilter.init(SAMPLE_FREQ_1000HZ, NOTCH_FREQ_50HZ, true, true, true);
  
  fourthOrderFilter.init(sosCoeffs4th);
  sixthOrderFilter.init(sosCoeffs6th);
}

void loop() {
  unsigned long startTime = micros();

  int rawValue = analogRead(SensorInputPin);

  // Update all filters for comparison
  float origVal = (float)originalFilter.update(rawValue);
  float fourthVal = fourthOrderFilter.update((float)rawValue);
  float sixthVal = sixthOrderFilter.update((float)rawValue);

  // CSV Output for Serial Plotter/Recording 
  Serial.print(rawValue); Serial.print(",");
  Serial.print(origVal); Serial.print(",");
  Serial.print(fourthVal); Serial.print(",");
  Serial.println(sixthVal);

  // Maintain 1000Hz
  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < samplePeriod) {
    delayMicroseconds(samplePeriod - elapsedTime);
  }
}