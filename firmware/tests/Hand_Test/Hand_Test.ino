#include <Servo.h>

// Define the number of fingers
const int NUM_FINGERS = 5;

// Create an array of servos for the fingers
Servo fingers[NUM_FINGERS];

// Define the opening angles for each finger
const int openAngles[NUM_FINGERS] = {93, 120, 120, 110, 100}; // Thumb, Index, Middle, Ring, Pinky

// Define the pin numbers for each servo
const int servoPins[NUM_FINGERS] = {7, 6, 5, 4, 3}; // Thumb, Index, Middle, Ring, Pinky

void setup() {
  // Attach all servos to the corresponding pins
  for (int i = 0; i < NUM_FINGERS; i++) {
    fingers[i].attach(servoPins[i]);
  }
  
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  open_hand();  // Open the entire hand at the start
  delay(1500);
}

void loop() {
  // Check if any data is available on the serial port
  if (Serial.available() > 0) {
    // Read the incoming byte as a command and subtract '0' to convert it to an integer
    int command = Serial.read() - '0';

    // Execute the corresponding function based on the command
    switch (command) {
      case 0: open_hand(); break;           // Open the hand
      case 1: close_hand(); break;          // Close the hand
      case 2: single_finger_test(); break;  // Run the single finger test
      case 3: mittelfinger(); break;        // Show the "mittelfinger" gesture
      case 4: peace(); break;               // Show the "peace" gesture
      default: Serial.println("Invalid command. Please send a number between 0 and 4."); break;
    }
    
    delay(500);  // Prevent spamming commands too quickly
  }
}

// Function to open the entire hand
void open_hand() {
  for (int i = 0; i < NUM_FINGERS; i++) {
    fingers[i].write(openAngles[i]);
  }
}

// Function to close the entire hand
void close_hand() {
  for (int i = 0; i < NUM_FINGERS; i++) {
    fingers[i].write(0);  // All fingers close to 0 degrees
  }
}

// Function to test single finger movements
void single_finger_test() {
  open_hand();
  delay(500);
  
  for (int i = 0; i < NUM_FINGERS; i++) {
    close_finger(i);
    delay(500);
  }

  for (int i = 0; i < NUM_FINGERS; i++) {
    open_finger(i);
    delay(500);
  }
}

// Function for a "mittelfinger" gesture
void mittelfinger() {
  open_hand();
  close_finger(0);  // Close thumb
  close_finger(1);  // Close index finger
  open_finger(2);   // Open middle finger
  close_finger(3);  // Close ring finger
  close_finger(4);  // Close pinky finger
  delay(500);
}

// Function for a "peace" gesture
void peace() {
  open_hand();
  close_finger(0);  // Close thumb
  open_finger(1);   // Open index finger
  open_finger(2);   // Open middle finger
  close_finger(3);  // Close ring finger
  close_finger(4);  // Close pinky finger
  delay(500);
}

// Function to open a specific finger
// Finger Index: 0 - Thumb, 1 - Index Finger, 2 - Middle Finger, 3 - Ring Finger, 4 - Pinky Finger
void open_finger(int fingerIndex) {
  fingers[fingerIndex].write(openAngles[fingerIndex]);
}

// Function to close a specific finger
// Finger Index: 0 - Thumb, 1 - Index Finger, 2 - Middle Finger, 3 - Ring Finger, 4 - Pinky Finger
void close_finger(int fingerIndex) {
  fingers[fingerIndex].write(0);
}
