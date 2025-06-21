#include "equipment.h"
#include <Arduino.h>

Equipment::Equipment(int equipment_pin){
  pin = equipment_pin;
  pinMode(pin, OUTPUT);
}

void Equipment::cmdOn(){
  digitalWrite(pin, HIGH);  
}

void Equipment::cmdOff(){
  digitalWrite(pin, LOW);  
}
