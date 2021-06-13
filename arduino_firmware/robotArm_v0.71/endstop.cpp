#include "endstop.h"
#include <Arduino.h>

Endstop::Endstop(int a_min_pin, int a_dir_pin, int a_step_pin, int a_en_pin, int a_switch_input, int a_step_offset, int a_home_dwell, bool does_swap_pin){
  min_pin = a_min_pin;
  dir_pin = a_dir_pin;
  step_pin = a_step_pin;
  en_pin = a_en_pin;
  switch_input = a_switch_input;
  home_dwell = a_home_dwell;
  step_offset = a_step_offset;
  swap_pin = does_swap_pin;
  if (swap_pin == false){
    pinMode(min_pin, INPUT_PULLUP);   
  }
}

void Endstop::home(bool dir) {
  if (swap_pin == true){
    pinMode(min_pin, INPUT_PULLUP);
    delayMicroseconds(5);
  }
  digitalWrite(en_pin, LOW);
  delayMicroseconds(5);
  if (dir==1){
    digitalWrite(dir_pin, HIGH);
  } else {
    digitalWrite(dir_pin, LOW);
  }
  delayMicroseconds(5);
  bState = !(digitalRead(min_pin) ^ switch_input);
  while (!bState) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(home_dwell);
    bState = !(digitalRead(min_pin) ^ switch_input);
  }
  homeOffset(dir);
  if (swap_pin == true){
    pinMode(min_pin, OUTPUT);
    delayMicroseconds(5);
  }
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
  if (swap_pin == true){
    pinMode(min_pin, INPUT_PULLUP);
  }
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
  if (swap_pin == true){
    pinMode(min_pin, INPUT_PULLUP);
    delayMicroseconds(5);
  }
  bState = !(digitalRead(min_pin) ^ switch_input);
  if (swap_pin == true){
    pinMode(min_pin, OUTPUT);
    delayMicroseconds(5);
  }
  return bState;
}