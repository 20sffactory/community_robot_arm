#ifndef RAMPSSTEPPER_H_
#define RAMPSSTEPPER_H_

class RampsStepper {
public:
  RampsStepper(int aStepPin, int aDirPin, int aEnablePin, bool aInverse);
  void enable(bool value = true);
    
  bool isOnPosition() const;
  int getPosition() const;
  void setPosition(int value);
  void stepToPosition(int value);
  void stepToPositionMM(float mm, float steps_per_mm);
  void stepRelative(int value);
  float getPositionRad() const;
  void setPositionRad(float rad);
  void stepToPositionRad(float rad);
  void stepRelativeRad(float rad);
  
  void update();
  
  void setReductionRatio(float gearRatio, int stepsPerRev);
private:
  int stepperStepTargetPosition;
  int stepperStepPosition;
  int stepPin;
  int dirPin;
  int enablePin;  
  bool inverse;
  float radToStepFactor;
};

#endif
