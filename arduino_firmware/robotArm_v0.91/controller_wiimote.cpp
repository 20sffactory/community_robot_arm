
#include <Arduino.h>

#include "config_esp32.h"
#include "controller_wiimote.h"
#include "logger.h"

#if BOARD_CHOICE == WEMOSD1R32
#include "ESP32Wiimote.h"

Controller_Wiimote::Controller_Wiimote(){
  ESP32Wiimote wiimote;
}

void Controller_Wiimote::setup(){
  Logger::logINFO("HOLD 1+2 BUTTON TO CONNECT WIIMOTE");
  Logger::logINFO("WHEN LED1 LIGHTS UP. PRESS ANY BUTTON TO START");
  wiimote.init();
  wiimote.addFilter(ACTION_IGNORE, FILTER_NUNCHUK_ACCEL);
  while (wiimote.available() == 0){
    wiimote.task();
    if (wiimote.available() > 0){
      Logger::logINFO("WIIMOTE CONNECTED");
      break;
    }
  Logger::logINFO("CONNECTING TO WIIMOTE");
  delay(10);
  }
}

void Controller_Wiimote::update(){
  wiimote.task();
  if (wiimote.available() > 0){
    button = wiimote.getButtonState();
  }
  delayMicroseconds(10);
}

#endif
