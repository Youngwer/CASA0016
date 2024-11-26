#include <Adafruit_NeoPixel.h>

// Ultrasonic sensor
#define AOUT A0           // URM09 water level sensor AOUT connected to A0
#define LED_PIN 5        // LED data pin connected to D6
#define NUMPIXELS 8       // Number of LEDs
#define CLOSE_DISTANCE 3  // Threshold for close distance (cm)
#define MID_DISTANCE 5    // Threshold for medium distance (cm)
#define MAX_DISTANCE 50   // Maximum allowable distance (cm)

// Flow sensor
#define FLOW_SENSOR_PIN 2 // Flow sensor signal connected to D2
volatile int pulseCount = 0; // Used for interrupt counting
float flowRate = 0;          // Flow rate (unit: L/min)
unsigned long oldTime = 0;   // Timestamp of the last calculation

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN);

// Interrupt service function
void pulseCounter() {
  pulseCount++; // Increment count on each detected falling edge
}

void setup() {
  Serial.begin(9600);         // Initialize serial communication
  pixels.begin();             // Initialize the LED
  pixels.clear();             // Ensure all LEDs are off
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP); // Configure the flow sensor pin
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // Set interrupt mode

  Serial.println("System initialized!");
}

void loop() {
  static unsigned long lastPrintTime = 0; // Used to control water level output frequency
  unsigned long currentTime = millis();

  // Ultrasonic sensor section
  int sensorValue = analogRead(AOUT);
  float distance = sensorValue * (3.0 / 1023.0) * 100 / 0.6; // Calibration formula, may need adjustment

  // Limit water level data output frequency to once every 200ms
  if (currentTime - lastPrintTime >= 200) {
    Serial.print("Water level: ");
    Serial.print(distance);
    Serial.println(" cm");
    lastPrintTime = currentTime;
  }

  // Flow sensor section, calculate flow rate every 200ms
  if (currentTime - oldTime >= 200) { // Adjusted to 200ms
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)); // Temporarily disable interrupt
    flowRate = (pulseCount / 7.5) * 5; // 7.5 is the sensor calibration coefficient, ×5 for finer units
    pulseCount = 0;                   // Reset pulse count
    oldTime = currentTime;            // Update timestamp
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // Re-enable interrupt

    Serial.print("Flow rate: ");
    Serial.print(flowRate, 2); // Print with two decimal places
    Serial.println(" L/min");
  }

  // Control the number of LEDs to light up based on flow rate
  int numLEDs;
  if (flowRate <= 0) {
    numLEDs = 1; // Light 1 LED when there is no flow
  } else {
    // Map flow rate 0.1~4 L/min to 2~8 LEDs
    numLEDs = map(flowRate, 0, 4, 2, NUMPIXELS);
  }
  numLEDs = constrain(numLEDs, 1, NUMPIXELS); // Ensure LED count is between 1 and NUMPIXELS

  // Control LED color based on ultrasonic sensor
  uint8_t red = 0, green = 0, blue = 0; // Initialize as black (off)
  if (distance > MAX_DISTANCE || distance <= 0) {
    red = 255; green = 0; blue = 0;     // Red warning
  } else if (distance <= CLOSE_DISTANCE) {
    red = 255; green = 0; blue = 0;     // Red
  } else if (distance <= MID_DISTANCE) {
    red = 255; green = 255; blue = 0;   // Yellow
  } else {
    red = 0; green = 255; blue = 0;     // Green
  }

  // Set LED color and the number of LEDs to light up
  setLEDColor(red, green, blue, numLEDs);
  delay(50); // Ensure sufficient loop frequency
}

// Set LED color and the number of LEDs to light up
void setLEDColor(uint8_t red, uint8_t green, uint8_t blue, int numLEDs) {
  pixels.clear();  // Clear current LED display
  for (int i = 0; i < numLEDs; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); // Set each pixel's color
  }
  pixels.show();  // Update the display
}
