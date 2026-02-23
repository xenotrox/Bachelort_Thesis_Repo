// Define the LED pins
const int ledPin1 = 12;
const int ledPin2 = 13;

void setup() {
  // Initialize the digital pins as outputs
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void loop() {
  // Turn both LEDs on
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  delay(1000); // Wait for 1000 milliseconds (1 second)

  // Turn both LEDs off
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  delay(1000); // Wait for 1 second
}