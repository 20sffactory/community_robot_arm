#ifndef CONTROLLER_PS4_H_
#define CONTROLLER_PS4_H_

#include "config.h"

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