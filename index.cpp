// libraries
#include <EEPROM.h>
#include <Servo.h>

//---------------------------------------------------------
//class joystick

class Joystick {
  
  public:
  
  Joystick(int pinX,int pinY,int pinBtn = -1){
    _pinX = pinX 
    _pinY = pinY
    _pinBtn = pinBtn
    pinMode(_pinX, INPUT);
    pinMode(_pinY, INPUT);
    if(_pinBtn > -1)
      pinMode(_pinBtn, INPUT_PULLUP);

    _meanX = 500;
    _meanY = 500;
  }
