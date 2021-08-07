#include "servo_gripper.h"
#include "config.h"

#include <Arduino.h>

#if BOARD_CHOICE == WEMOSD1R32
  #include <ESP32Servo.h>
#else 
  #include <Servo.h>
#endif

Servo_Gripper::Servo_Gripper(int pin, float grip_degree, float ungrip_degree){
  servo_pin = pin;
  servo_grip_deg = grip_degree;
  servo_ungrip_deg = ungrip_degree;
  Servo servo_motor;
}

void Servo_Gripper::cmdOn(){
  servo_motor.attach(servo_pin);
  delayMicroseconds(10);
  servo_motor.write(servo_grip_deg);
  delay(300);
  //servo_motor.detach();
}

void Servo_Gripper::cmdOff(){
  //servo_motor.attach(servo_pin);
  servo_motor.write(servo_ungrip_deg);
  delay(300);
  servo_motor.detach();
  delayMicroseconds(50);
}

float Servo_Gripper::readDegree(){
  float servo_current_degree = servo_motor.read();
  return servo_current_degree;
}

bool Servo_Gripper::isOn(){
  float servo_current_degree = servo_motor.read();
  if (servo_current_degree == servo_grip_deg){
    return true;
  } else {
    return false;
  }
}
