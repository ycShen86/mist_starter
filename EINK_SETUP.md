# OLED Display Setup Guide

## Display Type

This project uses an **SSD1306 OLED Display Module**:
- Size: 0.96" or 1.3" (128x64 pixels)
- Interface: I2C (IIC)
- Controller: SSD1306
- Colors: Monochrome (white on black)

## Hardware Connections

### I2C Pins (ESP32-S3 to OLED Display)

| OLED Pin | ESP32-S3 Pin | Description |
|----------|--------------|-------------|
| VCC/VIN  | 3.3V         | Power       |
| GND      | GND          | Ground      |
| SCL      | GPIO9        | I2C Clock   |
| SDA      | GPIO8        | I2C Data    |

**Note:** The OLED display shares the I2C bus with the AHT20 temperature/humidity sensor.

## I2C Address

- Default address: **0x3C** (most common)
- Alternative address: **0x3D** (some modules)

If your display doesn't work, try changing the address in `eink_display.h`:
```cpp
#define SCREEN_ADDRESS 0x3C  // or 0x3D
```

## Installing Required Library

1. Open Arduino IDE
2. Go to: Sketch → Include Library → Manage Libraries
3. Search for "Adafruit SSD1306"
4. Install "Adafruit SSD1306" library
5. It will also install dependencies (Adafruit GFX, Adafruit BusIO)

## Display Features

- **Fast refresh**: Updates every 1 second (configurable)
- **Displays**:
  - Temperature (°C)
  - Humidity (% rH)
  - Mist status (ON/OFF)
  - System status messages
- **Non-blocking**: Uses millis() timing
- **High contrast**: Bright white pixels on black background
- **Low power**: Uses minimal current when displaying static content

## Customization

### Change Update Interval

In `mist_starter.ino`, modify:
```cpp
#define OLED_UPDATE_INTERVAL 1000   // milliseconds (1 second)
```

### Force Immediate Update

```cpp
einkDisplay.forceUpdate();
```

### Change I2C Pins

In `mist_starter.ino`, modify:
```cpp
#define OLED_SDA 8
#define OLED_SCL 9
```

### Change I2C Address

In `eink_display.h`, modify:
```cpp
#define SCREEN_ADDRESS 0x3C  // or 0x3D
```

## Troubleshooting

**Display not initializing:**
- Check all I2C pin connections (SDA, SCL, VCC, GND)
- Verify I2C address (try 0x3C or 0x3D)
- Ensure Adafruit SSD1306 library is installed
- Check that no other device conflicts with I2C address

**Display shows nothing:**
- Check if VCC is connected to 3.3V (not 5V on some modules)
- Verify SDA and SCL are not swapped
- Try an I2C scanner sketch to detect the device

**Display flickers:**
- This is normal for OLED when updating frequently
- Increase the update interval if flickering is bothersome

**No response from I2C:**
- Make sure Wire.begin() is called with correct pins
- Check that AHT20 sensor is not conflicting on the I2C bus

## References

- [Adafruit SSD1306 OLED Displays](https://learn.adafruit.com/monochrome-oled-breakouts)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
