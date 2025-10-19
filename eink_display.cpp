// eink_display.cpp
// Implementation of OLED display management (SSD1306)

#include "eink_display.h"

EInkDisplay::EInkDisplay(int sda, int scl, unsigned long updateInterval) {
  this->sdaPin = sda;
  this->sclPin = scl;
  this->updateInterval = updateInterval;
  this->lastUpdateTime = 0;
  this->needsUpdate = false;
  this->displayInitialized = false;
  this->temperatureText = "---";
  this->humidityText = "---";
  this->statusText = "Initializing...";
  this->mistText = "OFF";

  // Initialize OLED display object (128x64 pixels)
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  Serial.println("Using SSD1306 128x64 OLED display");
}

void EInkDisplay::begin() {
  Serial.println("OLED Display Initializing...");
  Serial.print("I2C SDA: GPIO");
  Serial.print(sdaPin);
  Serial.print(" | SCL: GPIO");
  Serial.println(sclPin);

  // Initialize I2C with custom pins
  Wire.begin(sdaPin, sclPin);

  // Initialize OLED display with I2C address
  if(!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    return;
  }

  display->clearDisplay();
  display->setTextColor(SSD1306_WHITE);
  display->setTextSize(1);

  // Initial display
  display->setCursor(10, 5);
  display->setTextSize(2);
  display->println("ESP32-S3");
  display->setTextSize(1);
  display->setCursor(10, 30);
  display->println("Mist Controller");
  display->setCursor(10, 45);
  display->println("Initializing...");
  display->display();

  displayInitialized = true;
  lastUpdateTime = millis();

  Serial.println("OLED Display Initialized");
}

void EInkDisplay::update() {
  if (!displayInitialized) {
    return;
  }

  unsigned long currentTime = millis();

  // Check if it's time to update
  if (needsUpdate && (currentTime - lastUpdateTime >= updateInterval)) {
    updateContent();
    lastUpdateTime = currentTime;
    needsUpdate = false;
  }
}

void EInkDisplay::updateContent() {
  Serial.println("Updating OLED display...");

  display->clearDisplay();

  // Draw header (compact for 128x64)
  display->setTextSize(1);
  display->setCursor(0, 0);
  display->println("Mist System");

  // Draw separator line
  display->drawLine(0, 10, display->width(), 10, SSD1306_WHITE);

  // Draw temperature
  display->setTextSize(1);
  display->setCursor(0, 14);
  display->print("Temp: ");
  display->print(temperatureText);
  display->println(" C");

  // Draw humidity
  display->setCursor(0, 26);
  display->print("Humid: ");
  display->print(humidityText);
  display->println(" %");

  // Draw mist status (larger text)
  display->setCursor(0, 38);
  display->print("Mist: ");
  display->setTextSize(2);
  display->println(mistText);

  // Draw status at bottom
  display->setTextSize(1);
  display->setCursor(0, 56);
  display->println(statusText);

  // Display the buffer
  display->display();

  Serial.println("OLED display updated");
}

void EInkDisplay::setTemperature(float temp) {
  temperatureText = String(temp, 1);
  needsUpdate = true;
}

void EInkDisplay::setHumidity(float humidity) {
  humidityText = String(humidity, 1);
  needsUpdate = true;
}

void EInkDisplay::setStatus(const char* status) {
  statusText = String(status);
  needsUpdate = true;
}

void EInkDisplay::setMistStatus(bool active) {
  mistText = active ? "ON" : "OFF";
  needsUpdate = true;
}

void EInkDisplay::forceUpdate() {
  needsUpdate = true;
  lastUpdateTime = 0; // Force immediate update on next call to update()
}

void EInkDisplay::clear() {
  if (displayInitialized) {
    display->clearDisplay();
    display->display();
  }
}

int EInkDisplay::getWidth() {
  return display->width();
}

int EInkDisplay::getHeight() {
  return display->height();
}

void EInkDisplay::setUpdateInterval(unsigned long interval) {
  updateInterval = interval;
  Serial.print("OLED update interval set to ");
  Serial.print(interval / 1000);
  Serial.println(" seconds");
}

void EInkDisplay::drawLayout() {
  // Reserved for future layout improvements
}
