#ifndef COMMAND_H_
#define COMMAND_H_

#include <Arduino.h>
#include "interpolation.h"

struct Cmd {
  char id;
  int num;
  float valueX;
  float valueY;
  float valueZ;
  float valueF;
  float valueE;
  float valueS; 
};

class Command {
  public:
    Command();
    bool handleGcode();
    bool processMessage(String msg);
    void value_segment(String msg_segment);
    Cmd getCmd() const;
    void cmdGetPosition(Point pos, Point pos_offset, float highRad, float lowRad, float rotRad);
    void cmdToRelative();
    void cmdToAbsolute();
    bool isRelativeCoord;
    Cmd new_command;

  private: 
    String message;
};

void cmdMove(Cmd(&cmd), Point pos, Point pos_offset, bool isRelativeCoord);
void cmdDwell(Cmd(&cmd));
void printErr();

#endif
