#ifndef ENDSTOP_H_
#define ENDSTOP_H_

class Endstop {
  public:
    Endstop(int a_min_pin, int a_dir_pin, int a_step_pin, int a_en_pin, int a_switch_input, int a_step_offset, int a_home_dwell);
    void home(bool dir);

  private:
    int min_pin;
    int dir_pin;
    int step_pin;
    int en_pin;
    int switch_input;
    int home_dwell;
    int step_offset;
    bool bState;
    void homeOffset(bool dir);
};

#endif
