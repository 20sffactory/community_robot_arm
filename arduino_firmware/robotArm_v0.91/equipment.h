#ifndef EQUIPMENT_H_
#define EQUIPMENT_H_

class Equipment {
public:
  Equipment(int equipment_pin);
  void cmdOn();
  void cmdOff();
private:
  int pin;
};

#endif
