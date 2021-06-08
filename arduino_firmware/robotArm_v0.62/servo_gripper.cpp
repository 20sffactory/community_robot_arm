#include "servo_gripper.h"
#include <Arduino.h>
#include <Servo.h>


Servo_Gripper::Servo_Gripper(int pin, float grip_degree, float ungrip_degree){
  servo_pin = pin;
  servo_grip_deg = grip_degree;
  servo_ungrip_deg = ungrip_degree;
  Servo servo_motor;
}

void Servo_Gripper::cmdOn(){
  servo_motor.attach(servo_pin);
  servo_motor.write(servo_grip_deg);
  delay(300);
  servo_motor.detach();
}

void Servo_Gripper::cmdOff(){
  servo_motor.attach(servo_pin);
  servo_motor.write(servo_ungrip_deg);
  delay(300);
  servo_motor.detach();
}
