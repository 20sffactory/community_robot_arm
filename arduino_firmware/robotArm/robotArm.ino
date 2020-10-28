//20sffactory community_robot v0.21

#include "config.h"
#include "pinout.h"

#include <Arduino.h>

#include "robotGeometry.h"
#include "interpolation.h"
#include "fanControl.h"
#include "rampsStepper.h"
#include "queue.h"
#include "command.h"
#include "byj_gripper.h"
#include "equipment.h"
#include "endstop.h"

//STEPPER OBJECTS
RampsStepper stepperHigher(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, INVERSE_X_STEPPER);
RampsStepper stepperLower(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, INVERSE_Y_STEPPER);
RampsStepper stepperRotate(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, INVERSE_Z_STEPPER);

//RAIL OBJECTS
RampsStepper stepperRail(E0_STEP_PIN, E0_DIR_PIN, E0_ENABLE_PIN, INVERSE_E0_STEPPER);
Endstop endstopE0(E0_MIN_PIN, E0_DIR_PIN, E0_STEP_PIN, E0_ENABLE_PIN, E0_MIN_INPUT, E0_HOME_STEPS, HOME_DWELL);

//ENDSTOP OBJECTS
Endstop endstopX(X_MIN_PIN, X_DIR_PIN, X_STEP_PIN, X_ENABLE_PIN, X_MIN_INPUT, X_HOME_STEPS, HOME_DWELL);
Endstop endstopY(Y_MIN_PIN, Y_DIR_PIN, Y_STEP_PIN, Y_ENABLE_PIN, Y_MIN_INPUT, Y_HOME_STEPS, HOME_DWELL);
Endstop endstopZ(Z_MIN_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_ENABLE_PIN, Z_MIN_INPUT, Z_HOME_STEPS, HOME_DWELL);

//EQUIPMENT OBJECTS
BYJ_Gripper byj_gripper(BYJ_PIN_0, BYJ_PIN_1, BYJ_PIN_2, BYJ_PIN_3, GRIP_STEPS);
Equipment laser(LASER_PIN);
Equipment pump(PUMP_PIN);
Equipment led(LED_PIN);
FanControl fan(FAN_PIN, FAN_DELAY);

//EXECUTION & COMMAND OBJECTS
RobotGeometry geometry(END_EFFECTOR_OFFSET, SHANK_LENGTH);
Interpolation interpolator;
Queue<Cmd> queue(QUEUE_SIZE);
Command command;

void setup()
{
  Serial.begin(BAUD);
  stepperHigher.setPositionRad(PI / 2.0); // 90°
  stepperLower.setPositionRad(0);         // 0°
  stepperRotate.setPositionRad(0);        // 0°
  stepperRail.setPosition(0);
  if (HOME_ON_BOOT) { //HOME DURING SETUP() IF HOME_ON_BOOT ENABLED
    homeSequence(); 
    Serial.println("Robot Online.");
  } else {
    setStepperEnable(false); //ROBOT ADJUSTABLE BY HAND AFTER TURNING ON
    if (HOME_X_STEPPER && HOME_Y_STEPPER && !HOME_Z_STEPPER){
      Serial.println("Robot Online.");
      Serial.println("Rotate robot to face front centre & send G28.");
    }
    if (HOME_X_STEPPER && HOME_Y_STEPPER && HOME_Z_STEPPER){
      Serial.println("Robot Online.");
      Serial.println("Send G28 to calibrate.");
    }
    if (!HOME_X_STEPPER && !HOME_Y_STEPPER){
      Serial.println("Robot Online.");
      Serial.println("Home robot manually and send G28 to calibrate.");
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
  if (RAIL){
    stepperRail.stepToPositionMM(interpolator.getEPosmm(), STEPS_PER_MM_RAIL);
  }
  stepperRotate.update();
  stepperLower.update();
  stepperHigher.update();
  if (RAIL){
    stepperRail.update();
  }
  fan.update();
  if (!queue.isFull()) {
    if (command.handleGcode()) {
      queue.push(command.getCmd());
    }
  }
  if ((!queue.isEmpty()) && interpolator.isFinished()) {
    executeCommand(queue.pop());
    if (PRINT_REPLY) {Serial.println(PRINT_REPLY_MSG);}
  }

  if (millis() % 500 < 250) {
    led.cmdOn();
  }
  else {
    led.cmdOff();
  }
}

void executeCommand(Cmd cmd) {

  if (cmd.id == -1) {
    printErr();
    return;
  }

  // ABSOLUTE MODE: INTERPOLATOR COORDINATES USED IF NONE PROVIDED
  if (isnan(cmd.valueX) && !command.isRelativeCoord){
    cmd.valueX = interpolator.getXPosmm();
  }
  if (isnan(cmd.valueY) && !command.isRelativeCoord){
    cmd.valueY = interpolator.getYPosmm();
  }
  if (isnan(cmd.valueZ) && !command.isRelativeCoord){
    cmd.valueZ = interpolator.getZPosmm();
  }
  if (isnan(cmd.valueE) && !command.isRelativeCoord){
    cmd.valueE = interpolator.getEPosmm();
  }

  if (cmd.id == 'G') {
    switch (cmd.num) {
    case 0:
      fan.enable(true);
      cmdMove(cmd,interpolator.getPosmm(), command.isRelativeCoord);
      interpolator.setInterpolation(cmd.valueX, cmd.valueY, cmd.valueZ, cmd.valueE, cmd.valueF);
      break;
    //case 1: cmdMove(cmd); break;
    case 4: cmdDwell(cmd); break;
    case 28: homeSequence(); break;
    case 90: command.cmdToAbsolute(); break; // ABSOLUTE COORDINATE MODE
    case 91: command.cmdToRelative(); break; // RELATIVE COORDINATE MODE
    default: printErr();
    }
  }
  else if (cmd.id == 'M') {
    switch (cmd.num) {
    case 1: pump.cmdOn(); break;
    case 2: pump.cmdOff(); break;
    case 3: byj_gripper.cmdOn(); break;
    case 5: byj_gripper.cmdOff(); break;
    case 6: laser.cmdOn(); break;
    case 7: laser.cmdOff(); break;
    case 17: setStepperEnable(true); break;
    case 18: setStepperEnable(false); break;
    case 106: fan.enable(true); break;
    case 107: fan.enable(false); break;
    case 114: command.cmdGetPosition(interpolator.getPosmm()); break;// Return the current positions of all axis 
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
  if(RAIL){
    stepperRail.enable(enable);
  }
  fan.enable(enable);
}

void homeSequence(){
  setStepperEnable(false);
  fan.enable(true);
  if (HOME_Y_STEPPER){
    endstopY.home(!INVERSE_Y_STEPPER); //INDICATE STEPPER HOMING DIRECTION
  }
  if (HOME_X_STEPPER){
    endstopX.home(!INVERSE_X_STEPPER); //INDICATE STEPPER HOMING DIRECTION
  }
  if (HOME_Z_STEPPER){
    endstopZ.home(INVERSE_Z_STEPPER); //INDICATE STEPPER HOMING DIRECDTION
  }
  if (RAIL){
    if (HOME_E0_STEPPER){
      endstopE0.home(!INVERSE_E0_STEPPER); //
    }
  }
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
}
