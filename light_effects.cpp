// light_effects.cpp
// Implementation of WS281X LED light effects

#include "light_effects.h"

LightEffects::LightEffects(int pin, int numLEDs, int brightness) {
  this->ledPin = pin;
  this->numLEDs = numLEDs;
  this->brightness = brightness;
  this->currentEffect = EFFECT_OFF;
  this->effectStartTime = 0;
  this->lastUpdateTime = 0;
  this->effectStep = 0;
  this->primaryColor = 0;
  this->secondaryColor = 0;

  // Create NeoPixel strip object
  strip = new Adafruit_NeoPixel(numLEDs, pin, NEO_GRB + NEO_KHZ800);
}

void LightEffects::begin() {
  strip->begin();
  strip->setBrightness(map(brightness, 0, 100, 0, 255));
  strip->show(); // Initialize all pixels to 'off'

  Serial.println("Light Effects Initialized");
  Serial.print("LED Pin: ");
  Serial.println(ledPin);
  Serial.print("Number of LEDs: ");
  Serial.println(numLEDs);
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.println("%");
}

void LightEffects::update() {
  unsigned long currentTime = millis();

  switch (currentEffect) {
    case EFFECT_OFF:
      // Do nothing
      break;

    case EFFECT_STATIC:
      // Static color, no updates needed
      break;

    case EFFECT_RAINBOW:
      if (currentTime - lastUpdateTime >= 20) {
        updateRainbow();
        lastUpdateTime = currentTime;
      }
      break;

    case EFFECT_PULSE:
      if (currentTime - lastUpdateTime >= 10) {
        updatePulse();
        lastUpdateTime = currentTime;
      }
      break;

    case EFFECT_SPARKLE:
      if (currentTime - lastUpdateTime >= 50) {
        updateSparkle();
        lastUpdateTime = currentTime;
      }
      break;

    case EFFECT_WAVE:
      if (currentTime - lastUpdateTime >= 30) {
        updateWave();
        lastUpdateTime = currentTime;
      }
      break;
  }
}

void LightEffects::off() {
  currentEffect = EFFECT_OFF;
  strip->clear();
  strip->show();
  Serial.println("LEDs off");
}

void LightEffects::setStatic(uint32_t color) {
  currentEffect = EFFECT_STATIC;
  primaryColor = color;

  for (int i = 0; i < numLEDs; i++) {
    strip->setPixelColor(i, color);
  }
  strip->show();
  Serial.println("Static color effect");
}

void LightEffects::rainbow() {
  currentEffect = EFFECT_RAINBOW;
  effectStep = 0;
  lastUpdateTime = millis();
  Serial.println("Rainbow effect");
}

void LightEffects::pulse(uint32_t color) {
  currentEffect = EFFECT_PULSE;
  primaryColor = color;
  effectStep = 0;
  lastUpdateTime = millis();
  Serial.println("Pulse effect");
}

void LightEffects::sparkle(uint32_t color) {
  currentEffect = EFFECT_SPARKLE;
  primaryColor = color;
  lastUpdateTime = millis();
  Serial.println("Sparkle effect");
}

void LightEffects::wave(uint32_t color1, uint32_t color2) {
  currentEffect = EFFECT_WAVE;
  primaryColor = color1;
  secondaryColor = color2;
  effectStep = 0;
  lastUpdateTime = millis();
  Serial.println("Wave effect");
}

void LightEffects::updateRainbow() {
  for (int i = 0; i < numLEDs; i++) {
    strip->setPixelColor(i, Wheel(((i * 256 / numLEDs) + effectStep) & 255));
  }
  strip->show();
  effectStep = (effectStep + 1) % 256;
}

void LightEffects::updatePulse() {
  // Sine wave breathing effect
  float breath = (sin(effectStep * 0.05) + 1.0) / 2.0; // 0.0 to 1.0
  uint8_t brightness = (uint8_t)(breath * 255);

  uint32_t dimmed = dimColor(primaryColor, brightness);

  for (int i = 0; i < numLEDs; i++) {
    strip->setPixelColor(i, dimmed);
  }
  strip->show();
  effectStep = (effectStep + 1) % 126; // Full sine cycle
}

void LightEffects::updateSparkle() {
  // Random sparkles
  strip->clear();
  int numSparkles = random(1, numLEDs / 3);
  for (int i = 0; i < numSparkles; i++) {
    int pixel = random(numLEDs);
    strip->setPixelColor(pixel, primaryColor);
  }
  strip->show();
}

void LightEffects::updateWave() {
  for (int i = 0; i < numLEDs; i++) {
    // Create wave pattern
    int offset = (effectStep + i * 10) % 100;
    if (offset < 50) {
      strip->setPixelColor(i, primaryColor);
    } else {
      strip->setPixelColor(i, secondaryColor);
    }
  }
  strip->show();
  effectStep = (effectStep + 1) % 100;
}

void LightEffects::triggerMistEffect() {
  // Special effect when mist is triggered - cyan/blue wave
  Serial.println("Mist effect triggered!");
  wave(Color(0, 255, 255), Color(0, 0, 255)); // Cyan to Blue wave
}

void LightEffects::setBrightness(int brightness) {
  this->brightness = constrain(brightness, 0, 100);
  strip->setBrightness(map(this->brightness, 0, 100, 0, 255));
  strip->show();
  Serial.print("Brightness set to ");
  Serial.print(this->brightness);
  Serial.println("%");
}

void LightEffects::setEffect(int effectType) {
  switch (effectType) {
    case 0: off(); break;
    case 1: rainbow(); break;
    case 2: pulse(Color(255, 255, 255)); break;
    case 3: sparkle(Color(255, 255, 255)); break;
    case 4: wave(Color(255, 0, 0), Color(0, 0, 255)); break;
    default: off(); break;
  }
}

uint32_t LightEffects::Color(uint8_t r, uint8_t g, uint8_t b) {
  return strip->Color(r, g, b);
}

bool LightEffects::isActive() {
  return currentEffect != EFFECT_OFF;
}

// Helper function: Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t LightEffects::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t LightEffects::dimColor(uint32_t color, uint8_t brightness) {
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)color;

  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;

  return strip->Color(r, g, b);
}
