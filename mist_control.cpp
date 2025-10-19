// mist_control.cpp
// Implementation of mist generator control via capacitive touch

#include "mist_control.h"

MistControl::MistControl(int touchPin, int switchPin, int threshold, int debounce) {
  this->touchPin = touchPin;
  this->switchPin = switchPin;
  this->touchThreshold = threshold;
  this->debounceDelay = debounce;
  this->touchValue = 0;
  this->switchState = false;
  this->lastDebounceTime = 0;
  this->pressState = IDLE;
  this->stateStartTime = 0;
}

void MistControl::begin() {
  // Start with pin as INPUT (high impedance - button not pressed)
  pinMode(switchPin, INPUT);

  Serial.println("Mist Control Initialized");
  Serial.print("Touch Pin: ");
  Serial.println(touchPin);
  Serial.print("Switch Pin: ");
  Serial.println(switchPin);

  // Print initial touch value for calibration
  int initialValue = touchRead(touchPin);
  Serial.print("Initial touch value: ");
  Serial.println(initialValue);
  Serial.print("Touch threshold set to: ");
  Serial.println(touchThreshold);
}

void MistControl::pressButton() {
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
}

void MistControl::releaseButton() {
  pinMode(switchPin, INPUT);
}

void MistControl::updatePressStateMachine() {
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - stateStartTime;

  switch (pressState) {
    case IDLE:
      // Do nothing, waiting for trigger
      break;

    case PRESS1_DOWN:
      if (elapsed >= PRESS_DURATION) {
        releaseButton();
        pressState = PRESS1_UP;
        stateStartTime = currentTime;
      }
      break;

    case PRESS1_UP:
      if (elapsed >= RELEASE_DURATION) {
        pressButton();
        pressState = PRESS2_DOWN;
        stateStartTime = currentTime;
      }
      break;

    case PRESS2_DOWN:
      if (elapsed >= PRESS_DURATION) {
        releaseButton();
        pressState = PRESS2_UP;
        stateStartTime = currentTime;
      }
      break;

    case PRESS2_UP:
      if (elapsed >= RELEASE_DURATION) {
        pressButton();
        pressState = PRESS3_DOWN;
        stateStartTime = currentTime;
      }
      break;

    case PRESS3_DOWN:
      if (elapsed >= PRESS_DURATION) {
        releaseButton();
        pressState = PRESS3_UP;
        stateStartTime = currentTime;
      }
      break;

    case PRESS3_UP:
      Serial.println("Triple press complete!");
      pressState = IDLE;
      switchState = true;
      break;
  }
}

void MistControl::update() {
  // Update the state machine for non-blocking button presses
  updatePressStateMachine();

  // Only check for new touches when idle
  if (pressState == IDLE) {
    // Read the capacitive touch sensor
    touchValue = touchRead(touchPin);

    // Check if touch is detected (value drops below threshold when touched)
    if (touchValue < touchThreshold) {
      // Debounce check
      if ((millis() - lastDebounceTime) > debounceDelay) {
        if (!switchState) {
          // Touch detected - start triple button press sequence
          Serial.println("Touch detected! Sending triple press...");
          Serial.print("Touch value: ");
          Serial.println(touchValue);

          pressButton();
          pressState = PRESS1_DOWN;
          stateStartTime = millis();
        }
        lastDebounceTime = millis();
      }
    } else {
      // No touch - reset state
      if (switchState) {
        switchState = false;
        Serial.println("Touch released. Ready for next trigger.");
      }
    }
  }
}

int MistControl::getTouchValue() {
  return touchValue;
}

bool MistControl::isSwitchActive() {
  return switchState;
}
