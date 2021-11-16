//20SFFACTORY COMMUNITY ROBOT FIRMWARE

//MAINTAINER: LEOYEUNG@20SFFACTORY
//CONTACT: yeung.kl.leo@gmail.com
//FORUM: www.facebook.com/groups/robotarm

//VERSION: V0.81

//VERSION HISTORY:
//   V0.31 WITH G92, M114, LOGGER, LIMIT_CHECK FUNCTIONS
//   V0.41 WITH DUAL SHANK LENGTH SUPPORT
//   V0.51 WITH SERVO GRIPPER
//   V0.61 WITH ARDUINO UNO OPTION
//   V0.71 WITH:
//       ESP32(WEMOS D1R32) WITH PS4 JOYSTICK CONTROL OPTION
//       COMMAND TO SET CUSTOM SPEED PROFILE 'M205 S0'
//       UNO OPTION WITH RAIL SUPPORT
//   V0.81 WITH WII REMOTE, JOYSTICK ADJUSTABLE SPEED MULTIPLIER


#include <Arduino.h>
//GENERAL CONFIG SETTINGS
#include "config.h"

#include "robotGeometry.h"
#include "interpolation.h"
#include "RampsStepper.h"
#include "queue.h"
#include "command.h"
#include "equipment.h"
#include "endstop.h"
#include "logger.h"
#include "fanControl.h"
//INCLUDE CORRESPONDING GRIPPER MOTOR CLASS
#if GRIPPER == SERVO
  #include "servo_gripper.h"
#elif GRIPPER == BYJ
  #include "byj_gripper.h"
#endif

//DETERMINE PINOUTS & CONFIG TO USE SUBJECT TO BOARD_CHOICE
#if BOARD_CHOICE == UNO
  #include "pinout/pinout_uno.h"
#elif BOARD_CHOICE == WEMOSD1R32
  #include "pinout/pinout_wemosD1R32.h"
  #include "config_esp32.h"
#elif BOARD_CHOICE == MEGA2560
  #include "pinout/pinout.h"
#endif

//STEPPER OBJECTS
RampsStepper stepperHigher(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, INVERSE_X_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
RampsStepper stepperLower(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, INVERSE_Y_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
RampsStepper stepperRotate(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, INVERSE_Z_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);

//RAIL OBJECTS
#if RAIL
  RampsStepper stepperRail(E0_STEP_PIN, E0_DIR_PIN, E0_ENABLE_PIN, INVERSE_E0_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
  #if BOARD_CHOICE == WEMOSD1R32 //PINSWAP REQIURED ON D1R32 DUE TO INSUFFICIENT DIGIAL PINS
    #define SERVO_PIN 23 // REDEFINE SERVO_PIN FOR RAIL // SHARE WITH Z_MIN_PIN
    Endstop endstopE0(E0_MIN_PIN, E0_DIR_PIN, E0_STEP_PIN, E0_ENABLE_PIN, E0_MIN_INPUT, E0_HOME_STEPS, HOME_DWELL, true);
  #else 
    Endstop endstopE0(E0_MIN_PIN, E0_DIR_PIN, E0_STEP_PIN, E0_ENABLE_PIN, E0_MIN_INPUT, E0_HOME_STEPS, HOME_DWELL, false);
  #endif
#endif

//ENDSTOP OBJECTS
Endstop endstopX(X_MIN_PIN, X_DIR_PIN, X_STEP_PIN, X_ENABLE_PIN, X_MIN_INPUT, X_HOME_STEPS, HOME_DWELL, false);
Endstop endstopY(Y_MIN_PIN, Y_DIR_PIN, Y_STEP_PIN, Y_ENABLE_PIN, Y_MIN_INPUT, Y_HOME_STEPS, HOME_DWELL, false);
#if BOARD_CHOICE == WEMOSD1R32
  Endstop endstopZ(Z_MIN_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_ENABLE_PIN, Z_MIN_INPUT, Z_HOME_STEPS, HOME_DWELL, true);
#else
  Endstop endstopZ(Z_MIN_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_ENABLE_PIN, Z_MIN_INPUT, Z_HOME_STEPS, HOME_DWELL, false);
#endif

//EQUIPMENT OBJECTS
#if GRIPPER == SERVO
  Servo_Gripper servo_gripper(SERVO_PIN, SERVO_GRIP_DEGREE, SERVO_UNGRIP_DEGREE);
#elif GRIPPER == BYJ
  BYJ_Gripper byj_gripper(BYJ_PIN_0, BYJ_PIN_1, BYJ_PIN_2, BYJ_PIN_3, BYJ_GRIP_STEPS);
#endif
Equipment laser(LASER_PIN);
Equipment pump(PUMP_PIN);
Equipment led(LED_PIN);
FanControl fan(FAN_PIN, FAN_DELAY);

//EXECUTION & COMMAND OBJECTS
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;
Queue<Cmd> queue(QUEUE_SIZE);
Command command;

//PS4 CONTROLLER OBJECT FOR ESP32
#if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == DUALSHOCK4
  #include "controller_ps4.h"
  Controller_PS4 controller_ps4(PS4_MAC);
#endif
#if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == WIIMOTE
  #include "controller_wiimote.h"
  Controller_Wiimote controller_wiimote;
#endif

void setup()
{
  Serial.begin(BAUD);
  #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == DUALSHOCK4
    controller_ps4.setup();
  #endif
  #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == WIIMOTE
    controller_wiimote.setup();
  #endif
  stepperHigher.setPositionRad(PI / 2.0); // 90°
  stepperLower.setPositionRad(0);         // 0°
  stepperRotate.setPositionRad(0);        // 0°
  #if RAIL
  stepperRail.setPosition(0);
  #endif
  if (HOME_ON_BOOT) { //HOME DURING SETUP() IF HOME_ON_BOOT ENABLED
    homeSequence(); 
    Logger::logINFO("ROBOT ONLINE");
  } else {
    setStepperEnable(false); //ROBOT ADJUSTABLE BY HAND AFTER TURNING ON
    if (HOME_X_STEPPER && HOME_Y_STEPPER && !HOME_Z_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("ROTATE ROBOT TO FACE FRONT CENTRE & SEND G28 TO CALIBRATE");
    }
    if (HOME_X_STEPPER && HOME_Y_STEPPER && HOME_Z_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("SEND G28 TO CALIBRATE");
    }
    if (!HOME_X_STEPPER && !HOME_Y_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("HOME ROBOT MANUALLY & SEND G28 TO CALIBRATE");
    }
  }
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
}

void loop() {
  interpolator.updateActualPosition();
  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());
  stepperRotate.stepToPositionRad(geometry.getRotRad());
  stepperLower.stepToPositionRad(geometry.getLowRad());
  stepperHigher.stepToPositionRad(geometry.getHighRad());
  #if RAIL
    stepperRail.stepToPositionMM(interpolator.getEPosmm(), STEPS_PER_MM_RAIL);
  #endif
  stepperRotate.update();
  stepperLower.update();
  stepperHigher.update();
  #if RAIL
    stepperRail.update();
  #endif
  fan.update();

  if (!queue.isFull()) {
    if (command.handleGcode()) {
      queue.push(command.getCmd());
    }
  }
  if ((!queue.isEmpty()) && interpolator.isFinished()) {
    executeCommand(queue.pop());
    if (PRINT_REPLY) {
      Serial.println(PRINT_REPLY_MSG);
    }
  }

  if (millis() % 500 < 250) {
    led.cmdOn();
  }
  else {
    led.cmdOff();
  }

  #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == DUALSHOCK4
    ps4_controller_loop();
  #endif
  #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == WIIMOTE
    wiimote_controller_loop();
  #endif
}

void executeCommand(Cmd cmd) {

  if (cmd.id == -1) {
    printErr();
    return;
  }

  if (cmd.id == 'G') {
    switch (cmd.num) {
    case 0:
    case 1:
      fan.enable(true);
      Point posoffset;
      posoffset = interpolator.getPosOffset();      
      cmdMove(cmd, interpolator.getPosmm(), posoffset, command.isRelativeCoord);
      interpolator.setInterpolation(cmd.valueX, cmd.valueY, cmd.valueZ, cmd.valueE, cmd.valueF);
      Logger::logINFO("LINEAR MOVE: [X:" + String(cmd.valueX-posoffset.xmm) + " Y:" + String(cmd.valueY-posoffset.ymm) + " Z:" + String(cmd.valueZ-posoffset.zmm) + " E:" + String(cmd.valueE-posoffset.emm)+"]");
      break;
    case 4: cmdDwell(cmd); break;
    case 28: 
      if (BOARD_CHOICE == UNO || BOARD_CHOICE == WEMOSD1R32){
        homeSequence_UNO();
        break;
      } else {
        homeSequence();
        break;
      }
    case 90: command.cmdToAbsolute(); break; // ABSOLUTE COORDINATE MODE
    case 91: command.cmdToRelative(); break; // RELATIVE COORDINATE MODE
    case 92: 
      interpolator.resetPosOffset();
      cmdMove(cmd, interpolator.getPosmm(), interpolator.getPosOffset(), false);
      interpolator.setPosOffset(cmd.valueX, cmd.valueY, cmd.valueZ, cmd.valueE);
      break;
    default: printErr();
    }
  }
  else if (cmd.id == 'M') {
    switch (cmd.num) {
    case 1: pump.cmdOn(); break;
    case 2: pump.cmdOff(); break;
    case 3: 
      #if GRIPPER == BYJ
        byj_gripper.cmdOn(); break;
      #elif GRIPPER == SERVO
        servo_gripper.cmdOn(); break;
      #endif
    case 5:
      #if GRIPPER == BYJ
        byj_gripper.cmdOff(); break;
      #elif GRIPPER == SERVO
        servo_gripper.cmdOff(); break;
      #endif
    case 6: laser.cmdOn(); break;
    case 7: laser.cmdOff(); break;
    case 17: setStepperEnable(true); break;
    case 18: setStepperEnable(false); break;
    case 106: fan.enable(true); break;
    case 107: fan.enable(false); break;
    case 114: command.cmdGetPosition(interpolator.getPosmm(), interpolator.getPosOffset(), stepperHigher.getPosition(), stepperLower.getPosition(), stepperRotate.getPosition()); break;// Return the current positions of all axis 
    case 119: {
      String endstopMsg = "ENDSTOP: [X:";
      endstopMsg += String(endstopX.state());
      endstopMsg += " Y:";
      endstopMsg += String(endstopY.state());
      endstopMsg += " Z:";
      endstopMsg += String(endstopZ.state());
      #if RAIL
        endstopMsg += " E:";
        endstopMsg += String(endstopE0.state());
      #endif
      endstopMsg += "]";
      //ORIGINAL LOG STRING UNDESIRABLE FOR UNO PROCESSING
      //Logger::logINFO("ENDSTOP STATE: [UPPER_SHANK(X):"+String(endstopX.state())+" LOWER_SHANK(Y):"+String(endstopY.state())+" ROTATE_GEAR(Z):"+String(endstopZ.state())+"]");
      Logger::logINFO(endstopMsg);
      break;}
    case 205:
      interpolator.setSpeedProfile(cmd.valueS); 
      Logger::logINFO("SPEED PROFILE: [" + String(interpolator.speed_profile) + "]");
      break;
    default: printErr();
    }
  }
  else {
    printErr();
  }
}

void setStepperEnable(bool enable){
  stepperRotate.enable(enable);
  stepperLower.enable(enable);
  stepperHigher.enable(enable);
  #if RAIL
    stepperRail.enable(enable);
  #endif
  fan.enable(enable);
}

void homeSequence(){
  setStepperEnable(false);
  fan.enable(true);
  if (HOME_Y_STEPPER && HOME_X_STEPPER){
    endstopY.home(!INVERSE_Y_STEPPER);
    endstopX.home(!INVERSE_X_STEPPER);
  } else {
    setStepperEnable(true);
    endstopY.homeOffset(!INVERSE_Y_STEPPER);
    endstopX.homeOffset(!INVERSE_X_STEPPER);
  }
  if (HOME_Z_STEPPER){
    endstopZ.home(INVERSE_Z_STEPPER);
  }
  #if RAIL
    if (HOME_E0_STEPPER){
      endstopE0.home(!INVERSE_E0_STEPPER);
    }
  #endif
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
  Logger::logINFO("HOMING COMPLETE");
}

//DUE TO UNO CNC SHIELD LIMIT, 1 EN PIN SERVES 3 MOTORS, HENCE DIFFERENT HOMESEQUENCE IS REQUIRED
void homeSequence_UNO(){
  #if GRIPPER == SERVO
  if (servo_gripper.readDegree() != SERVO_UNGRIP_DEGREE){
    servo_gripper.cmdOff();
  }
  #endif
  if (HOME_Y_STEPPER && HOME_X_STEPPER){
    while (!endstopY.state() || !endstopX.state()){
      endstopY.oneStepToEndstop(!INVERSE_Y_STEPPER);
      endstopX.oneStepToEndstop(!INVERSE_X_STEPPER);
    }
    endstopY.homeOffset(!INVERSE_Y_STEPPER);
    endstopX.homeOffset(!INVERSE_X_STEPPER);
  } else {
    setStepperEnable(true);
    endstopY.homeOffset(!INVERSE_Y_STEPPER);
    endstopX.homeOffset(!INVERSE_X_STEPPER);
  }
  if (HOME_Z_STEPPER){
    endstopZ.home(INVERSE_Z_STEPPER); //INDICATE STEPPER HOMING DIRECDTION
  }
  #if RAIL
    if (HOME_E0_STEPPER){
      endstopE0.home(!INVERSE_E0_STEPPER);
    }
  #endif
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
  Logger::logINFO("HOMING COMPLETE");
}

#if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == DUALSHOCK4
void ps4_controller_loop(){
  controller_ps4.update();
  interpolator.speed_profile = 0;
  if (controller_ps4.buttons[PS4_CROSS]){if (GRIPPER == SERVO){servo_gripper.cmdOn();}}
  if (controller_ps4.buttons[PS4_CIRCLE]){if (GRIPPER == SERVO){servo_gripper.cmdOff();}}
  if (controller_ps4.buttons[PS4_OPTIONS]){setStepperEnable(true);}
  if (controller_ps4.buttons[PS4_SHARE]){setStepperEnable(false);}
  if (controller_ps4.buttons[PS4_TOUCHPAD]){homeSequence_UNO();}
  float x_distance = 0.0;
  float y_distance = 0.0;
  float z_distance = 0.0;
  float e_distance = 0.0;
  if (abs(controller_ps4.buttons[PS4_RSTICKY]) > 10){
//    z_distance = float(controller_ps4.buttons[PS4_RSTICKY]) / 2500.0;
    z_distance = float(controller_ps4.buttons[PS4_RSTICKY])*JOYSTICK_SPEED_MULTIPLIER / 2500.0;
  }
  if (abs(controller_ps4.buttons[PS4_LSTICKX]) > 10){
    float turn_rad = float(controller_ps4.buttons[PS4_LSTICKX])*JOYSTICK_SPEED_MULTIPLIER / 300000.0;
    x_distance += sin(geometry.getRotRad() + turn_rad) * geometry.getHypot() - interpolator.getXPosmm();
    y_distance += cos(geometry.getRotRad() + turn_rad) * geometry.getHypot() - interpolator.getYPosmm();
  }
  if (abs(controller_ps4.buttons[PS4_LSTICKY]) > 10){
    float hp_distance = float(controller_ps4.buttons[PS4_LSTICKY])*JOYSTICK_SPEED_MULTIPLIER / 2500.0;
    float hp_ratio = hp_distance / geometry.getHypot();
    x_distance += interpolator.getXPosmm() * hp_ratio;
    y_distance += interpolator.getYPosmm() * hp_ratio;
  }
  if (abs(controller_ps4.buttons[PS4_L2VALUE]) > 10){
    e_distance -= float(controller_ps4.buttons[PS4_L2VALUE])*JOYSTICK_SPEED_MULTIPLIER/ 10000.0;
  }
  if (abs(controller_ps4.buttons[PS4_R2VALUE]) > 10){
    e_distance += float(controller_ps4.buttons[PS4_R2VALUE])*JOYSTICK_SPEED_MULTIPLIER/ 10000.0;
  }
  if (x_distance || y_distance || z_distance || e_distance){
    interpolator.speed_profile = 0;
    interpolator.setInterpolation(interpolator.getXPosmm()+x_distance, interpolator.getYPosmm()+y_distance, interpolator.getZPosmm()+z_distance, interpolator.getEPosmm()+e_distance, 5);
  }
}
#endif

#if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == WIIMOTE
void wiimote_controller_loop(){
  controller_wiimote.update();
  interpolator.speed_profile = 0;
  if (controller_wiimote.button == WII_A && GRIPPER == SERVO){
    if (!servo_gripper.isOn()){
      servo_gripper.cmdOn();
    } else {
      servo_gripper.cmdOff();
    }
  }
  if (controller_wiimote.button == WII_PLUS){setStepperEnable(true);}
  if (controller_wiimote.button == WII_MINUS){setStepperEnable(false);}
  if (controller_wiimote.button == WII_HOME){homeSequence_UNO();}
  
  float x_distance = 0.0;
  float y_distance = 0.0;
  float z_distance = 0.0;
  float e_distance = 0.0;
  float hp_distance = 0.0;
  float turn_rad = 0.0;

  if (controller_wiimote.button == WII_LEFT || controller_wiimote.button == WII_LEFT+WII_DOWN || controller_wiimote.button == WII_LEFT+WII_UP){
    hp_distance = float(0.015 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_RIGHT || controller_wiimote.button == WII_RIGHT+WII_DOWN || controller_wiimote.button == WII_RIGHT+WII_UP){
    hp_distance = float(-0.015 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_DOWN || controller_wiimote.button == WII_LEFT+WII_DOWN || controller_wiimote.button == WII_RIGHT+WII_DOWN){
    turn_rad = float(-0.0001 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_UP || controller_wiimote.button == WII_LEFT+WII_UP || controller_wiimote.button == WII_RIGHT+WII_UP){
    turn_rad = float(0.0001 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_LEFT+WII_B){
    z_distance = float (0.015 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_RIGHT+WII_B){
    z_distance = float (-0.015 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_DOWN+WII_B){
    e_distance = float (-0.008 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (controller_wiimote.button == WII_UP+WII_B){
    e_distance = float (0.008 * JOYSTICK_SPEED_MULTIPLIER);
  }
  if (abs(hp_distance) > 0.0){
    float hp_ratio = hp_distance / geometry.getHypot();
    x_distance += interpolator.getXPosmm() * hp_ratio;
    y_distance += interpolator.getYPosmm() * hp_ratio;    
  }
  if (abs(turn_rad) > 0.0){
    x_distance += sin(geometry.getRotRad() + turn_rad) * geometry.getHypot() - interpolator.getXPosmm();
    y_distance += cos(geometry.getRotRad() + turn_rad) * geometry.getHypot() - interpolator.getYPosmm();    
  }

  if (x_distance || y_distance || z_distance || e_distance){
    interpolator.speed_profile = 0;
    interpolator.setInterpolation(interpolator.getXPosmm()+x_distance, interpolator.getYPosmm()+y_distance, interpolator.getZPosmm()+z_distance, interpolator.getEPosmm()+e_distance, 5);
    //SOLUTION FOR WIIMOTE Y<0.05 FREEZE ISSUE
    if (interpolator.getYPosmm() < 0.5) {
      interpolator.setInterpolation(interpolator.getXPosmm(), 0.5, interpolator.getZPosmm(), interpolator.getEPosmm(), 5);
    }
  }
}
#endif
