// Global variables
volatile int pulseCount = 0; // Used for interrupt counting
float flowRate = 0;          // Flow rate (unit: L/min)
unsigned long oldTime = 0;   // Last calculated time

// Interrupt service function
void pulseCounter() {
  pulseCount++; // Increment count by 1 each time a falling edge is detected
}

void setup() {
  Serial.begin(9600); // Initialize serial communication
  delay(2000); // Delay to ensure the Serial Monitor starts properly
  Serial.println("Water flow sensor is ready!");

  // Configure the water flow sensor signal pin
  pinMode(2, INPUT_PULLUP); // Yellow signal wire connected to D2
  attachInterrupt(digitalPinToInterrupt(2), pulseCounter, FALLING); // Set interrupt trigger mode
}

void loop() {
  unsigned long currentTime = millis();

  // Calculate flow rate every second
  if (currentTime - oldTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(2)); // Temporarily disable interrupt to prevent data conflict

    // Calculate flow rate (L/min), 7.5 is the calibration factor for the sensor
    flowRate = (pulseCount / 7.5);

    // Print flow rate to Serial Monitor
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");

    // Reset counter and time
    pulseCount = 0;
    oldTime = currentTime;

    attachInterrupt(digitalPinToInterrupt(2), pulseCounter, FALLING); // Restore interrupt
  }
}
