#ifndef FANCONTROL_H_
#define FANCONTROL_H_

class FanControl {
public:
  FanControl(int aPin, int aFanDelay);
  void enable(bool value = true);
  void disable();
  void update();
private:
  bool state;
  int pin;
  long fan_delay;
  long nextShutdown;
};

#endif
