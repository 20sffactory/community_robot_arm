
#include <Arduino.h>

#include "config_esp32.h"
#include "controller_ps4.h"
#include "logger.h"

#if BOARD_CHOICE == WEMOSD1R32
#include <PS4Controller.h>
Controller_PS4::Controller_PS4(char* aMacAddress){
  macAddress = aMacAddress;
}

void Controller_PS4::setup(){
  PS4.begin(macAddress);
  while (!PS4.isConnected()){
    if (PS4.isConnected()){
      Logger::logINFO("PS4 CONTROLLER CONNECTED");
      break;
    }
    Logger::logINFO("CONNECTING TO PS4 CONTROLLER");
    delay(1000);
  }
}

bool Controller_PS4::checkConnection(){
  return PS4.isConnected();
}

void Controller_PS4::update(){
  buttons[PS4_RIGHT] = PS4.Right();
  buttons[PS4_DOWN] = PS4.Down();
  buttons[PS4_UP] = PS4.Up();
  buttons[PS4_LEFT] = PS4.Left();
  buttons[PS4_SQUARE] = PS4.Square();
  buttons[PS4_CROSS] = PS4.Cross();
  buttons[PS4_CIRCLE] = PS4.Circle();
  buttons[PS4_TRIANGLE] = PS4.Triangle();
  buttons[PS4_L1] = PS4.L1();
  buttons[PS4_R1] = PS4.R1();
  buttons[PS4_SHARE] = PS4.Share();
  buttons[PS4_OPTIONS] = PS4.Options();
  buttons[PS4_L3] = PS4.L3();
  buttons[PS4_R3] = PS4.R3();
  buttons[PS4_PSBUTTON] = PS4.PSButton();
  buttons[PS4_TOUCHPAD] = PS4.Touchpad();
  buttons[PS4_LSTICKX] = PS4.LStickX();
  buttons[PS4_LSTICKY] = PS4.LStickY();
  buttons[PS4_RSTICKX] = PS4.RStickX();
  buttons[PS4_RSTICKY] = PS4.RStickY();
  buttons[PS4_L2VALUE] = PS4.L2Value();
  buttons[PS4_R2VALUE] = PS4.R2Value();
}
#endif
