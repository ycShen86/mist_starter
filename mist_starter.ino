// ESP32 S3 Mist Generator Controller with AHT20 Temperature/Humidity Sensor
// Touch pin: GPIO4 (T4)
// Switch control pin: GPIO3
// LED pin: GPIO7
// I2C: SDA - GPIO8, SCL - GPIO9 (AHT20 sensor + OLED display)
// OLED: 128x64 SSD1306 I2C display (address 0x3C)

#include <Adafruit_AHTX0.h>
#include "mist_control.h"
#include "light_effects.h"
#include "eink_display.h"

#define TOUCH_PIN T4           // Capacitive touch pin (GPIO4)
#define SWITCH_PIN 3           // Digital pin to control the physical switch
#define LED_PIN 7              // WS281X LED data pin (GPIO7)
#define NUM_LEDS 12            // Number of LEDs in the strip
#define LED_BRIGHTNESS 50      // LED brightness (0-100%)

// OLED display pins (I2C - shared with AHT20 sensor)
#define OLED_SDA 8             // I2C Data
#define OLED_SCL 9             // I2C Clock

#define TOUCH_THRESHOLD 40000  // Touch detection threshold (lower = more sensitive)
#define DEBOUNCE_DELAY 50      // Debounce delay in milliseconds
#define SENSOR_PRINT_INTERVAL 2000   // Print sensor data every 2 seconds
#define OLED_UPDATE_INTERVAL 1000    // Update OLED every 1 second (OLED is fast!)

// Create instances
Adafruit_AHTX0 aht;
MistControl mistControl(TOUCH_PIN, SWITCH_PIN, TOUCH_THRESHOLD, DEBOUNCE_DELAY);
LightEffects lightEffects(LED_PIN, NUM_LEDS, LED_BRIGHTNESS);
EInkDisplay einkDisplay(OLED_SDA, OLED_SCL, OLED_UPDATE_INTERVAL);

Adafruit_Sensor *aht_humidity, *aht_temp;

// Timing variables for non-blocking operation
unsigned long lastSensorPrint = 0;

// Track mist state for triggering light effects
bool lastMistState = false;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    // Wait up to 3 seconds for serial, then continue
  }

  Serial.println("ESP32 S3 Mist Generator with AHT20 Sensor");
  Serial.println("==========================================");

  // Initialize AHT20 sensor
  if (!aht.begin()) {
    Serial.println("Failed to find AHT10/AHT20 chip");
    while (1) {
      // Stuck here if sensor not found
    }
  }

  Serial.println("AHT10/AHT20 Found!");
  aht_temp = aht.getTemperatureSensor();
  aht_temp->printSensorDetails();

  aht_humidity = aht.getHumiditySensor();
  aht_humidity->printSensorDetails();

  // Initialize mist control
  mistControl.begin();

  // Initialize light effects
  lightEffects.begin();

  // Initialize e-ink display
  einkDisplay.begin();
  einkDisplay.setStatus("System Ready");

  Serial.println("==========================================");
  Serial.println("System Ready!");
}

void loop() {
  // Update mist control (non-blocking, handles touch sensor and button presses)
  mistControl.update();

  // Update light effects (non-blocking, handles animations)
  lightEffects.update();

  // Update e-ink display (non-blocking, updates at specified interval)
  einkDisplay.update();

  // Check if mist was just triggered to activate light effect
  bool currentMistState = mistControl.isSwitchActive();
  if (currentMistState && !lastMistState) {
    // Mist just activated - trigger light effect
    lightEffects.triggerMistEffect();
  }
  lastMistState = currentMistState;

  // Update mist status on e-ink display
  einkDisplay.setMistStatus(currentMistState);

  // Print sensor data at a slower rate (non-blocking)
  unsigned long currentTime = millis();
  if (currentTime - lastSensorPrint >= SENSOR_PRINT_INTERVAL) {
    lastSensorPrint = currentTime;

    // Get sensor readings from AHT20
    sensors_event_t humidity;
    sensors_event_t temp;
    aht_humidity->getEvent(&humidity);
    aht_temp->getEvent(&temp);

    // Display the results to Serial
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" % rH");

    // Update e-ink display with sensor data
    einkDisplay.setTemperature(temp.temperature);
    einkDisplay.setHumidity(humidity.relative_humidity);
  }
}
