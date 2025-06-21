#ifndef CONTROLLER_WIIMOTE_H_
#define CONTROLLER_WIIMOTE_H_

#include "config.h"

#if BOARD_CHOICE == WEMOSD1R32

#include "ESP32Wiimote.h"

#define WII_LEFT 0x0800
#define WII_RIGHT 0x0400
#define WII_UP 0x0200
#define WII_DOWN 0x0100
#define WII_A 0x0008
#define WII_B 0x0004
#define WII_PLUS 0x1000
#define WII_HOME 0x0080
#define WII_MINUS 0x0010
#define WII_ONE 0x0002
#define WII_TWO 0x0001

class Controller_Wiimote {
  public:
    Controller_Wiimote();
    uint16_t button;
    void setup();
    void update();
  private:
    ESP32Wiimote wiimote;
};

#endif
#endif
