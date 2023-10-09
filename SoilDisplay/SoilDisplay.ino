/*
  Soil Hydration Monitor
  soil_monitor.ino
  Computes soil hydration ratio
  Uses Capacitive probe
  Needs reference points from calibration
*/

// Integrate essential libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Specify OLED dimensions in pixels
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Establish OLED configurations
#define DISPLAY_RESET -1
#define DISPLAY_ADDR 0x3C

Adafruit_SSD1306 screen(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET);

// Probe constants - adjust based on calibration script

// Reference for arid condition
const int AridValue = 2590;

// Reference for damp condition
const int DampValue = 1430;

// Storage for soil hydration data
int hydrationReading;
int hydrationPercentage;

// Denotation for Analog input
#define PROBE_INPUT 0

void setup() {
  // Configure Serial Console
  Serial.begin(9600);

  // Start I2C screen powered by 3.3-volts directly from VCC
  screen.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDR);
  screen.clearDisplay();

  // Adjust ADC for 12-bit precision
  analogReadResolution(12);
}

void loop() {
  // Retrieve soil hydration data
  hydrationReading = analogRead(PROBE_INPUT);

  // Relay reading to Serial Console
  Serial.print(hydrationReading);
  Serial.print(" - ");

  // Compute hydration percentage
  hydrationPercentage = map(hydrationReading, AridValue, DampValue, 0, 100);

  // Limit percentage to range between 0 and 100
  hydrationPercentage = constrain(hydrationPercentage, 0, 100);

  // Display on Serial Console
  Serial.println(hydrationPercentage);

  // Format and project data onto OLED screen
  screen.setCursor(20, 0);
  screen.setTextSize(2);
  screen.setTextColor(WHITE);
  screen.println("Hydration");

  screen.setCursor(30, 40);
  screen.setTextSize(3);
  screen.setTextColor(WHITE);
  screen.print(hydrationPercentage);
  screen.println("%");
  screen.display();

  delay(250);
  screen.clearDisplay();
  delay(100);
}
