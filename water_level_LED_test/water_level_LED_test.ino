#include <Adafruit_NeoPixel.h>

// Ultrasonic sensor pins
#define trigPin 2
#define echoPin 3

// LED strip configuration
#define PIN 6              // LED data pin
#define NUMPIXELS 8        // Number of LEDs
Adafruit_NeoPixel pixels(NUMPIXELS, PIN);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize ultrasonic sensor
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input

  // Initialize LED strip
  pixels.begin();
  pixels.clear();
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH); 
  float distance = (duration * 0.0343) / 2; // Converts time to distance in cm
  
  // Print debugging information
  Serial.print("Distance in cm: ");
  Serial.println(distance);
  
  // Calculate the number of LEDs to light up and their color based on distance
  int numLedsToLight = mapDistanceToLEDs(distance);
  uint32_t color = mapDistanceToColor(distance);
  
  // Update the LED strip state
  updateLEDs(numLedsToLight, color);
  
  delay(100); // Delay to improve response speed
}

// Map distance to the number of LEDs to light up
int mapDistanceToLEDs(float distance) {
  if (distance < 2) distance = 2;  // Handle very close distances
  if (distance > 50) distance = 50; // Handle very far distances

  return map(distance, 2, 50, NUMPIXELS, 0); // The closer the distance, the more LEDs are lit
}

// Map distance to LED color
uint32_t mapDistanceToColor(float distance) {
  if (distance < 2) distance = 2;  // Handle very close distances
  if (distance > 50) distance = 50; // Handle very far distances

  // Close distance (red), medium distance (yellow), far distance (green)
  if (distance <= 20) {
    return pixels.Color(255, 0, 0); // Red
  } else if (distance <= 35) {
    return pixels.Color(255, 255, 0); // Yellow
  } else {
    return pixels.Color(0, 255, 0); // Green
  }
}

// Update the LED strip state
void updateLEDs(int numLedsToLight, uint32_t color) {
  pixels.clear(); // Clear the strip

  for (int i = 0; i < numLedsToLight; i++) {
    pixels.setPixelColor(i, color); // Set color based on distance
  }

  pixels.show(); // Update the strip state
}
