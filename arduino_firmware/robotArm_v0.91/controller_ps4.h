#ifndef CONTROLLER_PS4_H_
#define CONTROLLER_PS4_H_

#include "config.h"

//PS4 BUTTON DECLARATIONS
#define PS4_RIGHT             0
#define PS4_DOWN              1
#define PS4_UP                2
#define PS4_LEFT              3
#define PS4_SQUARE            4
#define PS4_CROSS             5
#define PS4_CIRCLE            6
#define PS4_TRIANGLE          7
#define PS4_L1                8
#define PS4_R1                9
#define PS4_SHARE             10
#define PS4_OPTIONS           11
#define PS4_L3                12
#define PS4_R3                13
#define PS4_PSBUTTON          14
#define PS4_TOUCHPAD          15
#define PS4_LSTICKX           16
#define PS4_LSTICKY           17
#define PS4_RSTICKX           18
#define PS4_RSTICKY           19
#define PS4_L2VALUE           20
#define PS4_R2VALUE           21

#if BOARD_CHOICE == WEMOSD1R32

class Controller_PS4 {
  public:
    Controller_PS4(char* aMacAddress);
    int buttons[22];
    void setup();
    void update();
    bool checkConnection();

  private:
    char* macAddress;
};

#endif
#endif