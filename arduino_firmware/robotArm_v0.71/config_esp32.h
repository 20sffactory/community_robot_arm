//ESP32 SETTINGS (FOR ESP32 USERS ONLY, OTHERWISE LEAVE UNCHANGED)
  //-------
  //TO USE WEMOSD1R32 OPTION, BOARD ESP32 V1.0.4 NEEDS TO BE INSTALLED IN ARDUINO IDE
  //TUTORIAL: www.randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
  //-------
  //LIBARIES OF ESP32 TO BE ADDED
  //  1) ESP32Servo < www.arduino.cc/reference/en/libraries/esp32servo/ >
  //  2) PS4Controller < https://github.com/aed3/PS4-esp32 >
  //-------
  //ESP32 SUPPORTS PS4 CONTROLLER: USE 'SIXAXISPAIRTOOL' TO FIND MAC ADDRESS < https://sixaxispairtool.software.informer.com/ >

//PS4 CONTROLLER TARGET MAC ADDRESS
#define ESP32_PS4_CONTROLLER true
#define PS4_MAC "20:ff:fa:cc:00:ff"

//BUTTON DECLARATIONS
#define PS4_RIGHT             0
#define PS4_DOWN              1
#define PS4_UP                2
#define PS4_LEFT              3
#define PS4_SQUARE            4
#define PS4_CROSS             5
#define PS4_CIRCLE            6
#define PS4_TRIANGLE          7
#define PS4_L1                8
#define PS4_R1                9
#define PS4_SHARE             10
#define PS4_OPTIONS           11
#define PS4_L3                12
#define PS4_R3                13
#define PS4_PSBUTTON          14
#define PS4_TOUCHPAD          15
#define PS4_LSTICKX           16
#define PS4_LSTICKY           17
#define PS4_RSTICKX           18
#define PS4_RSTICKY           19
#define PS4_L2VALUE           20
#define PS4_R2VALUE           21
