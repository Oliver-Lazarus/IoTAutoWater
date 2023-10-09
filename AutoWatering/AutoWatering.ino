#include "arduino_secrets_mod.h"
/*
  Sketch produced for the Arduino IoT Cloud Service
  
  Arduino IoT Cloud Variable Overview

  Automatically updated variables when alterations occur in the Thing

  float present_Temperature;
  float present_Humidity;
  int present_SoilMoisture;
  int activation_Level;
  bool actuator_State;

  Cloud Thing variables set as READ/WRITE will also invoke functions
  when their values undergo changes from the Dashboard.
  These functions are embedded with the Thing and appended to this sketch.
*/

// Graphics library from Adafruit - Current Version
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

// Adafruit SSD1306 Library - Current Version
#include <Adafruit_SSD1306.h>
#include <splash.h>

// Library for DHT sensor - Current Version
#include <DHT.h>
#include <DHT_U.h>

// I2C Communication Library
#include <Wire.h>

// Define OLED dimensions in pixels
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Establish OLED configurations
#define OLED_RESET -1
#define DISPLAY_ADDRESS 0x3C

Adafruit_SSD1306 screen(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

// DHT22 Configuration
#define DHTPIN 8
#define DHTMODEL DHT22
DHT dht(DHTPIN, DHTMODEL);

// Storage for Temperature and Humidity
float ambientTemp;
float ambientHum;

// Calibration values for the sensor

// Dry sensor constant
const int DryMeasurement = 2650;

// Wet sensor constant
const int WetMeasurement = 1800;

// Soil hydration readings
int hydrationReadout;
int hydrationPercentage;

// Analog input assignment
#define SENSOR_PORT 0

// Designation for Relay
#define RELAY_PORT 3

// Text for pump state
String actuator_text = "INACTIVE";

// Threshold for pump activation
int actuator_threshold = 30;

// IoT Cloud Service's Properties Sketch
#include "thingProperties_mod.h"

void setup() {

  // Start serial communication and await connection:
  Serial.begin(9600);
  delay(1500); // Wait for potential Serial Monitor connection

  initProperties();  // Defined within thingProperties_mod.h

  // Establish connection to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Diagnostic function for detailed connection insights
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Start I2C display at 3.3 volts
  screen.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  screen.clearDisplay();

  // Boot up DHT22
  dht.begin();

  // Adjust ADC to 12-bit precision
  analogReadResolution(12);

  // Configure Relay as Output
  pinMode(RELAY_PORT, OUTPUT);
  digitalWrite(RELAY_PORT, LOW);  // Default relay state

  // Set initial state for the pump
  actuator_State = false;

}

void loop() {
  ArduinoCloud.update();  // Sync with Arduino IoT Cloud

  // Fetch environmental readings
  ambientTemp = dht.readTemperature();
  ambientHum = dht.readHumidity();

  // Update cloud variables with recent readings
  present_Temperature = ambientTemp;
  present_Humidity = ambientHum;

  // Fetch hydration readings
  hydrationReadout = analogRead(SENSOR_PORT);

  // Compute hydration as a percentage
  hydrationPercentage = map(hydrationReadout, DryMeasurement, WetMeasurement, 0, 100);
  hydrationPercentage = constrain(hydrationPercentage, 0, 100);

  // Relay raw data for calibration purposes
  Serial.println(hydrationReadout);

  // Update hydration data to cloud
  present_SoilMoisture = hydrationPercentage;

  // Assess if the pump needs activation
  if (hydrationPercentage <= actuator_threshold) {
    // Start pump
    pumpStart();
  } else {
    // Stop pump
    pumpStop();
  }
  
  // Exhibit values on the OLED Display
  displayOLED(35, "PUMP", 40, actuator_text, 2000);
  displayOLED(35, "TEMP", 10, String(ambientTemp) + "C", 2000);
  displayOLED(30, "HUMID", 10, String(ambientHum) + "%", 2000);
  displayOLED(35, "WETNESS", 30, String(hydrationPercentage) + "%", 2000);
}

void pumpStart() {
  digitalWrite(RELAY_PORT, HIGH);
  actuator_text = "ACTIVE";
  actuator_State = true;
}

void pumpStop() {
  digitalWrite(RELAY_PORT, LOW);
  actuator_text = "INACTIVE";
  actuator_State = false;
}

void displayOLED(int top_position, String top_label, int main_position, String main_label, int pause_duration){
  screen.setCursor(top_position, 0);
  screen.setTextSize(2);
  screen.setTextColor(WHITE);
  screen.println(top_label);

  screen.setCursor(main_position, 40);
  screen.setTextSize(3);
  screen.setTextColor(WHITE);
  screen.print(main_label);
  screen.display();

  delay(pause_duration);
  screen.clearDisplay();
}

void onActivationLevelChange()  {
  actuator_threshold = activation_Level;
}

