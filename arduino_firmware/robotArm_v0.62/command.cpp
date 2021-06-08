#include "command.h"
#include "logger.h"
#include "config.h"
#include <Arduino.h>

Command::Command() {
  //initialize Command to a zero-move value;
  new_command.valueX = NAN; 
  new_command.valueY = NAN;
  new_command.valueZ = NAN;
  new_command.valueF = 0;
  new_command.valueE = NAN;
  new_command.valueS = 0;
  message = "";
  isRelativeCoord = false;
}

bool Command::handleGcode() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
       return false; 
    }
    if (c == '\r') {
       bool b = processMessage(message);
       message = "";
       return b;
    } else {
       message += c; 
    }
  }
  return false;
}

bool Command::processMessage(String msg){

  new_command.valueX = NAN; 
  new_command.valueY = NAN;
  new_command.valueZ = NAN;
  new_command.valueE = NAN;
  new_command.valueF = 0;
  new_command.valueS = 0;  
  msg.toUpperCase();
  msg.replace(" ", "");
  int active_index = 0;
  new_command.id = msg[active_index];
  if((new_command.id != 'G') && (new_command.id != 'M')){
    printErr();
    return false;
  }

  active_index++;
  int temp_index = active_index;
  while (temp_index<msg.length() && !isAlpha(msg[temp_index])){
    temp_index++;
  }
  new_command.num = msg.substring(active_index, temp_index).toInt();
  active_index = temp_index;
  temp_index++;
  while (temp_index<msg.length()){
    while (!isAlpha(msg[temp_index]) || msg[temp_index]=='.'){
      temp_index++;
      if (temp_index == msg.length()){
        break;
      }
    }
    value_segment(msg.substring(active_index, temp_index));
    active_index = temp_index;
    temp_index++;
  }
  return true;
}

void Command::value_segment(String msg_segment){
  float msg_value = msg_segment.substring(1).toFloat();
  switch (msg_segment[0]){
    case 'X': new_command.valueX = msg_value; break;
    case 'Y': new_command.valueY = msg_value; break;
    case 'Z': new_command.valueZ = msg_value; break;
    case 'E': new_command.valueE = msg_value; break;
    case 'F': new_command.valueF = msg_value; break;
    case 'S': new_command.valueS = msg_value; break;
  }
}


Cmd Command::getCmd() const {
  return new_command; 
}

void Command::cmdGetPosition(Point pos, Point pos_offset, float highRad, float lowRad, float rotRad){
  if(isRelativeCoord) {
    Logger::logINFO("RELATIVE MODE");
  } else {
    Logger::logINFO("ABSOLUTE MODE");
  }
  Logger::logINFO("CURRENT POSITION: [X:"+String(pos.xmm - pos_offset.xmm)+" Y:"+String(pos.ymm - pos_offset.ymm)+" Z:"+String(pos.zmm - pos_offset.zmm)+" E:"+String(pos.emm - pos_offset.emm)+"]");
  //Logger::logINFO("RADIANS: [HIGH:"+String(highRad)+" LOW:"+String(lowRad)+" ROT:"+String(rotRad));
}

void Command::cmdToRelative(){
  isRelativeCoord = true;
  Logger::logINFO("RELATIVE MODE ON");
}

void Command::cmdToAbsolute(){
  isRelativeCoord = false;
  Logger::logINFO("ABSOLUTE MODE ON");
}

void cmdMove(Cmd(&cmd), Point pos, Point pos_offset, bool isRelativeCoord){

  if(isRelativeCoord == true){
    cmd.valueX = isnan(cmd.valueX) ? pos.xmm : cmd.valueX + pos.xmm;
    cmd.valueY = isnan(cmd.valueY) ? pos.ymm : cmd.valueY + pos.ymm;
    cmd.valueZ = isnan(cmd.valueZ) ? pos.zmm : cmd.valueZ + pos.zmm;
    cmd.valueE = isnan(cmd.valueE) ? pos.emm : cmd.valueE + pos.emm; 
  } else {
    cmd.valueX = isnan(cmd.valueX) ? pos.xmm : cmd.valueX + pos_offset.xmm;
    cmd.valueY = isnan(cmd.valueY) ? pos.ymm : cmd.valueY + pos_offset.ymm;
    cmd.valueZ = isnan(cmd.valueZ) ? pos.zmm : cmd.valueZ + pos_offset.zmm;
    cmd.valueE = isnan(cmd.valueE) ? pos.emm : cmd.valueE + pos_offset.emm;
  }
}

void cmdDwell(Cmd(&cmd)){
  delay(int(cmd.valueS * 1000));
}

void printErr() {
  Logger::logERROR("COMMAND NOT RECOGNIZED");
}
