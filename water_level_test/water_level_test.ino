#define AOUT A0 // Analog signal output pin of URM09

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(AOUT, INPUT);
}

void loop() {
  int sensorValue = analogRead(AOUT); // Read the analog value from the sensor
  float distance = sensorValue / 2.0; // Calculate distance based on the fitting formula

  // Output the analog value and the calculated distance
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Distance: ");
  Serial.print(distance, 2); // Distance rounded to two decimal places
  Serial.println(" cm");

  delay(500); // Output data every 500ms
}
