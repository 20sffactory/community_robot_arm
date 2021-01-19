#include "robotGeometry.h"

#include <math.h>
#include <Arduino.h>

RobotGeometry::RobotGeometry(float a_ee_offset, float a_shank_length) {
  ee_offset = a_ee_offset;
  shank_length = a_shank_length;
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
   
   float rrot =  sqrt((xmm * xmm) + (ymm * ymm)) - ee_offset;    //radius from Top View
   float rrot_ee = rrot + ee_offset;
   float rside = sqrt((rrot * rrot) + (zmm * zmm));  //radius from Side View. Use rrot instead of ymm..for everything
   
   rot = asin(xmm / rrot_ee);
   high = acos((rside * 0.5) / shank_length) * 2.0;
   
   //Angle of Lower Stepper Motor  (asin()=Angle To Gripper)
   if (zmm > 0) {
     low =      asin(rrot / rside) + ((PI - high) / 2.0) - (PI / 2.0);
   } else {
     low = PI - asin(rrot / rside) + ((PI - high) / 2.0) - (PI / 2.0);
   }
   //correct higher Angle as it is mechanically bounded width lower Motor
   high = high + low;
}
