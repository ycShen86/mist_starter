// sleep_manager.cpp
// Implementation of ESP32-S3 light sleep management

#include "sleep_manager.h"

SleepManager::SleepManager(unsigned long sleepDurationSec, unsigned long wakeIntervalSec) {
  this->sleepDuration = sleepDurationSec * 1000000ULL; // Convert to microseconds
  this->wakeInterval = wakeIntervalSec * 1000;         // Convert to milliseconds
  this->lastWakeTime = 0;
  this->sleepEnabled = true;
  this->inSleepCycle = false;
  this->touchWakeEnabled = false;
  this->touchWakePin = -1;
  this->touchWakeThreshold = 40000;
}

void SleepManager::begin() {
  Serial.println("Sleep Manager Initialized");
  Serial.print("Sleep duration: ");
  Serial.print(sleepDuration / 1000000);
  Serial.println(" seconds");
  Serial.print("Wake interval: ");
  Serial.print(wakeInterval / 1000);
  Serial.println(" seconds");

  lastWakeTime = millis();

  // Check if we just woke up from sleep
  printWakeupReason();
}

void SleepManager::printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup: Timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup: Touch sensor");
      break;
    case ESP_SLEEP_WAKEUP_GPIO:
      Serial.println("Wakeup: GPIO");
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default:
      Serial.println("Wakeup: Power on or reset");
      break;
  }
}

void SleepManager::sleep() {
  if (!sleepEnabled) {
    return;
  }

  Serial.println("Entering light sleep mode...");
  Serial.flush(); // Make sure message is sent before sleeping

  // Configure timer wake-up
  esp_sleep_enable_timer_wakeup(sleepDuration);

  // Configure touch wake-up if enabled
  if (touchWakeEnabled && touchWakePin >= 0) {
    // Note: Touch wake-up configuration for ESP32-S3
    esp_sleep_enable_touchpad_wakeup();
    // The touch threshold is set separately in the touch configuration
  }

  // Enter light sleep (RAM and RTC peripherals stay powered)
  esp_light_sleep_start();

  // Code continues here after wake-up
  lastWakeTime = millis();
  inSleepCycle = false;

  Serial.println("Woke up from light sleep!");
  printWakeupReason();
}

void SleepManager::update() {
  if (!sleepEnabled || inSleepCycle) {
    return;
  }

  // Check if it's time to sleep
  unsigned long currentTime = millis();
  if (currentTime - lastWakeTime >= wakeInterval) {
    inSleepCycle = true;
    sleep();
  }
}

void SleepManager::enable() {
  sleepEnabled = true;
  Serial.println("Sleep mode enabled");
}

void SleepManager::disable() {
  sleepEnabled = false;
  Serial.println("Sleep mode disabled");
}

bool SleepManager::isEnabled() {
  return sleepEnabled;
}

void SleepManager::setSleepDuration(unsigned long seconds) {
  sleepDuration = seconds * 1000000ULL;
  Serial.print("Sleep duration set to ");
  Serial.print(seconds);
  Serial.println(" seconds");
}

void SleepManager::setWakeInterval(unsigned long seconds) {
  wakeInterval = seconds * 1000;
  Serial.print("Wake interval set to ");
  Serial.print(seconds);
  Serial.println(" seconds");
}

void SleepManager::enableTouchWake(int pin, int threshold) {
  touchWakeEnabled = true;
  touchWakePin = pin;
  touchWakeThreshold = threshold;

  Serial.print("Touch wake enabled on pin ");
  Serial.print(pin);
  Serial.print(" with threshold ");
  Serial.println(threshold);
}

void SleepManager::forceWake() {
  inSleepCycle = false;
  lastWakeTime = millis();
  Serial.println("Sleep cycle reset - staying awake");
}
