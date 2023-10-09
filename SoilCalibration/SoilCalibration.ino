/*
  Calibration for Soil Humidity Probe
  soil_calibrate.ino
  Retrieves primary reading from ground hydration probe and projects onto Serial Console
  Helps in determining base and peak values
  Compatible with both Capacitive and Resistive Probes
  Utilizes Raspberry Arduino Uno
  
*/

// Storage for probe reading
int readingValue;

// Designation for Analog input
#define PROBE_PORT 0

void setup() {
  // Initialize Serial Console
  Serial.begin(9600);

  // Adjust ADC for 12-bit precision
  analogReadResolution(12);
}

void loop() {

  // Capture reading from probe
  readingValue = analogRead(PROBE_PORT);

  // Relay value onto Serial Console
  Serial.println(readingValue);

  delay(100);
}
