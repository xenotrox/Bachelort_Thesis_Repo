#include "Arduino.h"

const int SENSOR_PIN = A1;

const int UPPER_DISCONNECTION_THRESHOLD = 600;
const int LOWER_DISCONNECTION_THRESHOLD = 0;

const int COUNTER_MAX = 2000;     
const int DISCONNECT_TOLERANCE = 90;

int disconnect_counter = DISCONNECT_TOLERANCE;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int rawValue = analogRead(SENSOR_PIN);
  bool isDisconnected = updateSensorStatus(rawValue);

  // Debug Output
  Serial.print("Raw:");         Serial.print(rawValue);
  Serial.print(",Disconnected:"); Serial.print(isDisconnected);
  Serial.print(",Counter:");      Serial.println(disconnect_counter);
  
}

bool updateSensorStatus(int raw) {
  if (raw >= UPPER_DISCONNECTION_THRESHOLD || raw <= LOWER_DISCONNECTION_THRESHOLD) {
    disconnect_counter++;
  } else {
    disconnect_counter--;
  }

  disconnect_counter = constrain(disconnect_counter, 0, COUNTER_MAX);

  return (disconnect_counter > DISCONNECT_TOLERANCE);
}