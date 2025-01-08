#include <Adafruit_NeoPixel.h>

// Ultrasonic sensor related
#define AOUT A0           // URM09 AOUT connected to A0
#define LED_PIN 6         // LED data pin connected to D6
#define NUMPIXELS 8       // Number of LEDs
#define CLOSE_DISTANCE 5  // Close distance threshold (cm)
#define MID_DISTANCE 10   // Mid distance threshold (cm)
#define MAX_DISTANCE 200   // Maximum allowed distance (cm)

// Water flow sensor related
#define FLOW_SENSOR_PIN 2 // Water flow sensor signal connected to D2
volatile int pulseCount = 0; // Used for interrupt counting
float flowRate = 0;          // Flow rate (unit: L/min)
unsigned long oldTime = 0;   // Last calculated time

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN);

// Interrupt service function
void pulseCounter() {
  pulseCount++; // Increment count by 1 each time a falling edge is detected
}

void setup() {
  Serial.begin(9600);         // Initialize serial communication
  pixels.begin();             // Initialize LED
  pixels.clear();             // Ensure all LEDs are off
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP); // Water flow sensor
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // Configure interrupt trigger mode

  Serial.println("System initialized!");
}

void loop() {
  // Ultrasonic sensor part
  int sensorValue = analogRead(AOUT);
  float voltage = sensorValue * (3.0 / 1023.0); // Assume AOUT output range is 0-3.0V
  float distance = voltage * 100;              // 1cm = 10mV

  // Water flow sensor part
  unsigned long currentTime = millis();
  if (currentTime - oldTime >= 1000) { // Calculate flow rate every second
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)); // Temporarily disable interrupt
    flowRate = (pulseCount / 7.5); // 7.5 is the calibration factor for the sensor
    pulseCount = 0;               // Reset pulse count
    oldTime = currentTime;        // Update timestamp
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING); // Restore interrupt

    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
  }

  // Set LED color based on distance
  if (distance > MAX_DISTANCE || distance <= 0) {
    setLEDColor(255, 0, 0, NUMPIXELS); // Show red warning
  } else if (distance <= CLOSE_DISTANCE) {
    int redLEDs = map(distance, 0, CLOSE_DISTANCE, NUMPIXELS, 0);
    setLEDColor(255, 0, 0, redLEDs);
  } else if (distance <= MID_DISTANCE) {
    setLEDColor(128, 128, 0, NUMPIXELS / 2); // Show yellow
  } else {
    setLEDColor(0, 128, 0, NUMPIXELS / 2);   // Show green
  }

  // Change LED brightness or color based on flow rate
  if (flowRate > 10.0) {
    setLEDColor(0, 0, 255, NUMPIXELS); // Show blue to indicate high flow rate
  } else if (flowRate > 0) {
    setLEDColor(0, 255, 255, NUMPIXELS / 2); // Show cyan to indicate medium flow rate
  }

  delay(200); // Delay 200ms
}

// Set LED color and number of lit LEDs
void setLEDColor(uint8_t red, uint8_t green, uint8_t blue, int numLEDs) {
  pixels.clear();  // Clear current LED display
  for (int i = 0; i < numLEDs; i++) {
    pixels.setPixelColor(i, red, green, blue); // Set color for each pixel
  }
  pixels.show();  // Update display
}
