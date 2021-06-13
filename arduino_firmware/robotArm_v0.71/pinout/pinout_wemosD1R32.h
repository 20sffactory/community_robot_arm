#ifndef PINOUT_ESP32WEMOS_H_
#define PINOUT_ESP32WEMOS_H_

#define X_STEP_PIN         26
#define X_DIR_PIN          16 //SHARE WITH E0_MIN_PIN
#define X_ENABLE_PIN       12
#define X_MIN_PIN          13
#define X_MAX_PIN          -1
 
#define Y_STEP_PIN         25
#define Y_DIR_PIN          27
#define Y_ENABLE_PIN       12
#define Y_MIN_PIN           5
#define Y_MAX_PIN          -1

#define Z_STEP_PIN         17
#define Z_DIR_PIN          14
#define Z_ENABLE_PIN       12
#define Z_MIN_PIN          23 //SHARE WITH SERVO_PIN
#define Z_MAX_PIN          -1

#define E0_STEP_PIN        19
#define E0_DIR_PIN         18
#define E0_ENABLE_PIN      12
#define E0_MIN_PIN         16 //SHARE WITH X_DIR_PIN

#define E1_STEP_PIN        -1
#define E1_DIR_PIN         -1
#define E1_ENABLE_PIN      -1

#define BYJ_PIN_0          -1
#define BYJ_PIN_1          -1
#define BYJ_PIN_2          -1
#define BYJ_PIN_3          -1

#define SERVO_PIN          23 //SHARE WITH Z_MIN_PIN

#define PUMP_PIN           -1
#define LASER_PIN          -1
#define LED_PIN            -1

#define SDPOWER            -1
#define SDSS               -1

#define FAN_PIN            -1

#define PS_ON_PIN          -1
#define KILL_PIN           -1

#define TEMP_0_PIN         -1
#define TEMP_1_PIN         -1

#endif
