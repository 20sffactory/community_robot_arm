#ifndef SERVO_GRIPPER_H_
#define SERVO_GRIPPER_H_
#include <Servo.h>

class Servo_Gripper{
public:
  Servo_Gripper(int pin, float grip_degree, float ungrip_degree);
  void cmdOn();
  void cmdOff();
private:
  Servo servo_motor;
  int servo_pin;
  float servo_grip_deg;
  float servo_ungrip_deg;
};

#endif
