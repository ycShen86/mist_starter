// mist_control.h
// Header file for mist generator control via capacitive touch

#ifndef MIST_CONTROL_H
#define MIST_CONTROL_H

#include <Arduino.h>

class MistControl {
  private:
    int touchPin;
    int switchPin;
    int touchThreshold;
    int debounceDelay;

    int touchValue;
    bool switchState;
    unsigned long lastDebounceTime;

    // State machine for non-blocking button presses
    enum PressState {
      IDLE,
      PRESS1_DOWN,
      PRESS1_UP,
      PRESS2_DOWN,
      PRESS2_UP,
      PRESS3_DOWN,
      PRESS3_UP
    };

    PressState pressState;
    unsigned long stateStartTime;

    const unsigned long PRESS_DURATION = 100;  // 100ms button press
    const unsigned long RELEASE_DURATION = 150; // 150ms between presses

    void pressButton();
    void releaseButton();
    void updatePressStateMachine();

  public:
    MistControl(int touchPin, int switchPin, int threshold = 40000, int debounce = 50);

    void begin();
    void update();
    int getTouchValue();
    bool isSwitchActive();
};

#endif
