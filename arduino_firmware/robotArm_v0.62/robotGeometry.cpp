#include "robotGeometry.h"

#include <math.h>
#include <Arduino.h>

RobotGeometry::RobotGeometry(float a_ee_offset, float a_low_shank_length, float a_high_shank_length) {
  ee_offset = a_ee_offset;
  low_shank_length = a_low_shank_length;
  high_shank_length = a_high_shank_length;
}

void RobotGeometry::set(float axmm, float aymm, float azmm) {
  xmm = axmm;
  ymm = aymm;
  zmm = azmm; 
  calculateGrad();
}

float RobotGeometry::getXmm() const {
  return xmm;
}

float RobotGeometry::getYmm() const {
  return ymm;
}

float RobotGeometry::getZmm() const {
  return zmm;
}

float RobotGeometry::getRotRad() const {
  return rot;
}

float RobotGeometry::getLowRad() const {
  return low;
}

float RobotGeometry::getHighRad() const {
  return high;
}

void RobotGeometry::calculateGrad() {
   float rrot_ee =  hypot(xmm, ymm);    
   float rrot = rrot_ee - ee_offset; //radius from Top View
   float rside = hypot(rrot, zmm);  //radius from Side View. Use rrot instead of ymm..for everything
   float rside_2 = sq(rside);
   float low_2 = sq(low_shank_length);
   float high_2 = sq(high_shank_length);
   
   rot = asin(xmm / rrot_ee);
   high = PI - acos((low_2 + high_2 - rside_2) / (2 * low_shank_length * high_shank_length));

   //Angle of Lower Stepper Motor  (asin()=Angle To Gripper)
   if (zmm > 0) {
     low =  acos(zmm / rside) - acos((low_2 - high_2 + rside_2) / (2 * low_shank_length * rside));
   } else {
     low = PI - asin(rrot / rside) - acos((low_2 - high_2 + rside_2) / (2 * low_shank_length * rside));
   }
   high = high + low;
}
