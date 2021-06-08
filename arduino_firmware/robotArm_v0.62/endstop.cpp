#include "endstop.h"
#include <Arduino.h>

Endstop::Endstop(int a_min_pin, int a_dir_pin, int a_step_pin, int a_en_pin, int a_switch_input, int a_step_offset, int a_home_dwell, int a_check_delay){
  min_pin = a_min_pin;
  dir_pin = a_dir_pin;
  step_pin = a_step_pin;
  en_pin = a_en_pin;
  switch_input = a_switch_input;
  home_dwell = a_home_dwell;
  step_offset = a_step_offset;
  check_delay = a_check_delay;
  bCheckDelay = false;
  pinMode(min_pin, INPUT_PULLUP);
}

void Endstop::home(bool dir) {
  digitalWrite(en_pin, LOW);
  delayMicroseconds(5);
  if (dir==1){
    digitalWrite(dir_pin, HIGH);
  } else {
    digitalWrite(dir_pin, LOW);
  }
  delayMicroseconds(5);

  // analiza si en un tiempo dado los endstops cambian de estado
  // si no: fallo electromecanico
  bCheckDelay = false;
  long cDelay = 0;
/* en v51 
  bState = digitalRead(min_pin);
  while (bState != switch_input) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(home_dwell);
    bState = digitalRead(min_pin);
    cDelay += home_dwell;
    if(cDelay/1000000 > check_delay and bState != switch_input){
      bCheckDelay = true;
      break;
    }
  }
  homeOffset(dir);
*/
  bState = !(digitalRead(min_pin) ^ switch_input);
  while (!bState) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(home_dwell);
    bState = !(digitalRead(min_pin) ^ switch_input);
    cDelay += home_dwell;
    if(!bState and cDelay/1000000 > check_delay){
      bCheckDelay = true;
      break;
    }
  }
  homeOffset(dir);
}

void Endstop::homeOffset(bool dir){
  if (dir==1){
    digitalWrite(dir_pin, LOW);
  }
  else{
    digitalWrite(dir_pin, HIGH);
  }
  delayMicroseconds(5);
  for (int i = 1; i <= step_offset; i++) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(home_dwell);
  }
}

void Endstop::oneStepToEndstop(bool dir){
  digitalWrite(en_pin, LOW);
  delayMicroseconds(5);
  if (dir==1){
    digitalWrite(dir_pin, HIGH);
  } else {
    digitalWrite(dir_pin, LOW);
  }
  delayMicroseconds(5);
  bState = !(digitalRead(min_pin) ^ switch_input);

  if (!bState) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(home_dwell);
  }
  bState = !(digitalRead(min_pin) ^ switch_input);
}

bool Endstop::state(){
  bState = !(digitalRead(min_pin) ^ switch_input);
  return bState;
}

bool Endstop::checkDelay(){
  return bCheckDelay;
}
