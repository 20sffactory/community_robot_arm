#include "fanControl.h"
#include "Arduino.h"


FanControl::FanControl(int aPin, int aFanDelay) {
  fan_delay = aFanDelay * 1000;
  nextShutdown = 0;
  pin = aPin;
  pinMode(pin , OUTPUT);
  digitalWrite(pin , LOW);
  state = false;
}

void FanControl::enable(bool value) {
  if (value) {
    state = true;
    digitalWrite(pin, HIGH);
  } else {
    disable();
  }
}

void FanControl::disable() {
  state = false;
  nextShutdown = millis() + fan_delay;
  update();
}

void FanControl::update() {
  if (!state) {
     if (millis() >= nextShutdown) {
       digitalWrite(pin, LOW);
     }
  }
}
