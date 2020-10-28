#include "interpolation.h"
#include "config.h"

void Interpolation::setCurrentPos(float px, float py, float pz, float pe) {
  Point p;
  p.xmm = px;
  p.ymm = py;
  p.zmm = pz;
  p.emm = pe;
  setCurrentPos(p);
}

void Interpolation::setInterpolation(float px, float py, float pz, float pe, float v) {
  Point p;
  p.xmm = px;
  p.ymm = py;
  p.zmm = pz;
  p.emm = pe;
  setInterpolation(p, v);
}

void Interpolation::setInterpolation(float p1x, float p1y, float p1z, float p1e, float p2x, float p2y, float p2z, float p2e, float v) {
  Point p1;
  Point p2;
  p1.xmm = p1x;
  p1.ymm = p1y;
  p1.zmm = p1z;
  p1.emm = p1e;
  p2.xmm = p2x;
  p2.ymm = p2y;
  p2.zmm = p2z;
  p2.emm = p2e;
  setInterpolation(p1, p2, v);
}

void Interpolation::setInterpolation(Point p1, float v) {
  Point p0;
  p0.xmm = xStartmm + xDelta;
  p0.ymm = yStartmm + yDelta;
  p0.zmm = zStartmm + zDelta;
  p0.emm = eStartmm + eDelta;
  setInterpolation(p0, p1, v);
}

void Interpolation::setInterpolation(Point p0, Point p1, float av) {
  v = av; //mm/s
  
  float a = (p1.xmm - p0.xmm);
  float b = (p1.ymm - p0.ymm);
  float c = (p1.zmm - p0.zmm);
  float e = abs(p1.emm - p0.emm);
  float dist = sqrt(a*a + b*b + c*c);

  if (dist < e) {
    dist = e; 
  }
//  if (dist = 0) {
//    dist = e;
//  } 
  if (v < 5) { //includes 0 = default value
    v = sqrt(dist) * 10; //set a good value for v
  }
  if (v < 5) {
     v = 5; 
  }
  
  tmul = v / dist;
  
  xStartmm = p0.xmm;
  yStartmm = p0.ymm;
  zStartmm = p0.zmm;
  eStartmm = p0.emm;
  
  xDelta = (p1.xmm - p0.xmm);
  yDelta = (p1.ymm - p0.ymm);
  zDelta = (p1.zmm - p0.zmm);
  eDelta = (p1.emm - p0.emm);
   
  state = 0;
  
  startTime = micros();
}

void Interpolation::setCurrentPos(Point p) {
  xStartmm = p.xmm;
  yStartmm = p.ymm;
  zStartmm = p.zmm;
  eStartmm = p.emm;
  xDelta = 0;
  yDelta = 0;
  zDelta = 0;
  eDelta = 0;
}

void Interpolation::updateActualPosition() {
  if (state != 0) {
    return;
  }    
  long microsek = micros();
  float t = (microsek - startTime) / 1000000.0;
  float progress;
  switch (SPEED_PROFILE){
    // FLAT SPEED CURVE
    case 0:
      progress = t * tmul;
      if (progress >= 1.0){
        progress = 1.0;
        state = 1;
      }
      break;
    // ARCTAN APPROX
    case 1:
      progress = atan((PI * t * tmul) - (PI * 0.5)) * 0.5 + 0.5;
      if (progress >= 1.0) {
        progress = 1.0; 
        state = 1;
      }
      break;
    // COSIN APPROX
    case 2:
      progress = -cos(t * tmul * PI) * 0.5 + 0.5;
      if ((t * tmul) >= 1.0) {
        progress = 1.0; 
        state = 1;
      }
      break;
  }

  xPosmm = xStartmm + progress * xDelta;
  yPosmm = yStartmm + progress * yDelta;
  zPosmm = zStartmm + progress * zDelta;
  ePosmm = eStartmm + progress * eDelta;

  //Serial.print("xPosmm:");
  //Serial.print(xPosmm);
  //Serial.print(" yPosmm:");
  //Serial.print(yPosmm);
  //Serial.print(" zPosmm:");
  //Serial.println(zPosmm);
}


bool Interpolation::isFinished() const {
  return state != 0; 
}

float Interpolation::getXPosmm() const {
  return xPosmm;
}

float Interpolation::getYPosmm() const {
  return yPosmm;
}

float Interpolation::getZPosmm() const {
  return zPosmm;
}

float Interpolation::getEPosmm() const {
  return ePosmm;
}

Point Interpolation::getPosmm() const {
  Point p;
  p.xmm = xPosmm;
  p.ymm = yPosmm;
  p.zmm = zPosmm;
  p.emm = ePosmm;
  return p;
}

float Interpolation::getDistance() const {
  
}
