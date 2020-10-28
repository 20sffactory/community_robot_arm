#ifndef ROBOTGEOMETRY_H_
#define ROBOTGEOMETRY_H_

class RobotGeometry {
public:
  RobotGeometry(float a_ee_offset, float a_shank_length);
  void set(float axmm, float aymm, float azmm);
  float getXmm() const;
  float getYmm() const;
  float getZmm() const;
  float getRotRad() const;
  float getLowRad() const;
  float getHighRad() const;
private:
  void calculateGrad();
  float ee_offset;
  float shank_length;
  float xmm;
  float ymm;
  float zmm;
  float rot;
  float low;
  float high;
};

#endif
