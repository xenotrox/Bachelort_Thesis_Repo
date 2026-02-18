#include "Arduino.h"
#include "EMGFilters.h"
#include "BandpassFilter4thOrder.h"
#include "BandpassFilter6thOrder.h"

#define SensorInputPin A1 [cite: 478]

EMGFilters originalFilter;
BandpassFilter4thOrder fourthOrderFilter;
BandpassFilter6thOrder sixthOrderFilter;

const int sampleRate = 1000; [cite: 307]
const unsigned long samplePeriod = 1000000 / sampleRate;

// 4th Order SOS Coeffs (from your provided file) [cite: 843]
float sosCoeffs4th[2][6] = {
  {0.03657484, 0.07314967, 0.03657484, 1.0, -1.20017304, 0.68088355},
  {1.0, -2.0, 1.0, 1.0, -1.6108166, 0.78896695}
};

// 6th Order SOS Coeffs for 65-135Hz @ 1000Hz (Computed via SciPy) [cite: 26, 319, 320]
float sosCoeffs6th[3][6] = {
  {0.0121, 0.0242, 0.0121, 1.0, -1.3501, 0.7201},
  {1.0, 0.0, -1.0, 1.0, -1.5002, 0.8105},
  {1.0, -2.0, 1.0, 1.0, -1.7503, 0.8902}
};

void setup() {
  Serial.begin(115200); [cite: 845]
  
  // Original Filter: Notch 50Hz, HP 20Hz, LP 150Hz [cite: 313, 846]
  originalFilter.init(SAMPLE_FREQ_1000HZ, NOTCH_FREQ_50HZ, true, true, true);
  
  fourthOrderFilter.init(sosCoeffs4th); [cite: 847]
  sixthOrderFilter.init(sosCoeffs6th);  // Final thesis choice [cite: 744]
}

void loop() {
  unsigned long startTime = micros();

  int rawValue = analogRead(SensorInputPin); [cite: 849]

  // Update all filters for comparison [cite: 535]
  float origVal = (float)originalFilter.update(rawValue);
  float fourthVal = fourthOrderFilter.update((float)rawValue);
  float sixthVal = sixthOrderFilter.update((float)rawValue);

  // CSV Output for Serial Plotter/Recording [cite: 852]
  Serial.print(rawValue); Serial.print(",");
  Serial.print(origVal); Serial.print(",");
  Serial.print(fourthVal); Serial.print(",");
  Serial.println(sixthVal);

  // Maintain 1000Hz cadence [cite: 853, 854]
  unsigned long elapsedTime = micros() - startTime;
  if (elapsedTime < samplePeriod) {
    delayMicroseconds(samplePeriod - elapsedTime);
  }
}