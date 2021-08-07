//ESP32 SETTINGS (FOR ESP32 USERS ONLY, OTHERWISE LEAVE UNCHANGED)
  //-------
  //TO USE WEMOSD1R32 OPTION, BOARD ESP32 V1.0.4 NEEDS TO BE INSTALLED IN ARDUINO IDE
  //TUTORIAL: www.randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
  //-------
  //LIBARIES OF ESP32 TO BE ADDED
  //  1) ESP32Servo < www.arduino.cc/reference/en/libraries/esp32servo/ >
  //  2) PS4Controller < https://github.com/aed3/PS4-esp32 >
  //  3) ESP32Wiimote https://github.com/bigw00d/Arduino-ESP32Wiimote
  //-------
  //ESP32 SUPPORTS PS4 CONTROLLER: USE 'SIXAXISPAIRTOOL' TO FIND MAC ADDRESS < https://sixaxispairtool.software.informer.com/ >

#define ESP32_JOYSTICK NONE
    //CHOICES:
    //  NONE              < NOT USING JOYSTICK >
    //  DUALSHOCK4        < PLAYSTATION 4 CONTROLLER >
    //  WIIMOTE           < WII REMOTE >

    //..DO NOT CHANGE BELOW VALUES..//
    #define NONE            0
    #define DUALSHOCK4      1
    #define WIIMOTE         2

//JOYSTICK CONTROL SPEED MULTIPLIER - REDUCE TO ADJUST SENSITIVITY/SPEED
#define JOYSTICK_SPEED_MULTIPLIER 1.0

//PS4 CONTROLLER TARGET MAC ADDRESS
#define PS4_MAC "20:ff:fa:cc:00:ff"
