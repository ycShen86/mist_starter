// light_effects.h
// Header file for WS281X LED light effects control

#ifndef LIGHT_EFFECTS_H
#define LIGHT_EFFECTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LightEffects {
  private:
    Adafruit_NeoPixel* strip;
    int ledPin;
    int numLEDs;
    int brightness;

    // Effect state
    enum EffectType {
      EFFECT_OFF,
      EFFECT_STATIC,
      EFFECT_RAINBOW,
      EFFECT_PULSE,
      EFFECT_SPARKLE,
      EFFECT_WAVE
    };

    EffectType currentEffect;
    unsigned long effectStartTime;
    unsigned long lastUpdateTime;
    int effectStep;

    // Color state
    uint32_t primaryColor;
    uint32_t secondaryColor;

    // Update functions for each effect
    void updateRainbow();
    void updatePulse();
    void updateSparkle();
    void updateWave();

    // Helper functions
    uint32_t Wheel(byte WheelPos);
    uint32_t dimColor(uint32_t color, uint8_t brightness);

  public:
    LightEffects(int pin, int numLEDs = 12, int brightness = 50);

    // Initialize the LED strip
    void begin();

    // Non-blocking update (call in loop)
    void update();

    // Control effects
    void setEffect(int effectType);
    void off();
    void setStatic(uint32_t color);
    void rainbow();
    void pulse(uint32_t color);
    void sparkle(uint32_t color);
    void wave(uint32_t color1, uint32_t color2);

    // Set brightness (0-100)
    void setBrightness(int brightness);

    // Trigger mist effect (special effect when mist is activated)
    void triggerMistEffect();

    // Color helpers
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b);

    // Check if effect is active
    bool isActive();
};

#endif
