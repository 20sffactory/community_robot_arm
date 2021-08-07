#ifndef SERVO_GRIPPER_H_
#define SERVO_GRIPPER_H_

#include "config.h"

#if BOARD_CHOICE == WEMOSD1R32
  #include <ESP32Servo.h>
#else 
  #include <Servo.h>
#endif

class Servo_Gripper{
public:
  Servo_Gripper(int pin, float grip_degree, float ungrip_degree);
  void cmdOn();
  void cmdOff();
  float readDegree();
  bool isOn();
private:
  Servo servo_motor;
  int servo_pin;
  float servo_grip_deg;
  float servo_ungrip_deg;
};

#endif