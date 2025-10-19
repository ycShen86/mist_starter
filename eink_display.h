// eink_display.h
// Header file for OLED display management
// Supports SSD1306 128x64 OLED displays via I2C

#ifndef EINK_DISPLAY_H
#define EINK_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // I2C address (0x3C for most 128x64, 0x3D for some)

class EInkDisplay {
  private:
    Adafruit_SSD1306* display;

    // I2C pins (for reference, managed by Wire library)
    int sdaPin;
    int sclPin;

    // Display state
    unsigned long lastUpdateTime;
    unsigned long updateInterval;
    bool needsUpdate;
    bool displayInitialized;

    // Text buffer for display content
    String temperatureText;
    String humidityText;
    String statusText;
    String mistText;

    // Helper functions
    void drawLayout();
    void updateContent();

  public:
    EInkDisplay(int sda = 8, int scl = 9,
                unsigned long updateInterval = 1000); // Default: update every 1 second (OLED is fast!)

    // Initialize the display
    void begin();

    // Non-blocking update (call in loop)
    void update();

    // Set content to display
    void setTemperature(float temp);
    void setHumidity(float humidity);
    void setStatus(const char* status);
    void setMistStatus(bool active);

    // Force immediate update
    void forceUpdate();

    // Clear display
    void clear();

    // Get display dimensions
    int getWidth();
    int getHeight();

    // Set update interval (milliseconds)
    void setUpdateInterval(unsigned long interval);
};

#endif
