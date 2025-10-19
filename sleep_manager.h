// sleep_manager.h
// Header file for ESP32-S3 light sleep management with periodic wake-up

#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"

class SleepManager {
  private:
    unsigned long sleepDuration;        // How long to sleep in microseconds
    unsigned long wakeInterval;         // How long to stay awake in milliseconds
    unsigned long lastWakeTime;         // When we last woke up
    bool sleepEnabled;                  // Is sleep mode enabled
    bool inSleepCycle;                  // Are we in a sleep cycle

    // Wake sources
    bool touchWakeEnabled;
    int touchWakePin;
    int touchWakeThreshold;

  public:
    // Constructor: sleepDuration in seconds, wakeInterval in seconds
    SleepManager(unsigned long sleepDurationSec = 5, unsigned long wakeIntervalSec = 10);

    // Initialize the sleep manager
    void begin();

    // Check if it's time to sleep and enter light sleep if needed
    void update();

    // Manually trigger sleep
    void sleep();

    // Enable/disable sleep mode
    void enable();
    void disable();
    bool isEnabled();

    // Configure sleep duration (in seconds)
    void setSleepDuration(unsigned long seconds);

    // Configure wake interval (in seconds)
    void setWakeInterval(unsigned long seconds);

    // Enable touch wake-up source
    void enableTouchWake(int pin, int threshold = 40000);

    // Get wake-up cause
    void printWakeupReason();

    // Force wake (exits sleep cycle)
    void forceWake();
};

#endif
