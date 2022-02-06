//#define RESET_CONFIG

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// Libraries

#include <EEPROM.h>
#include <Servo.h>

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// Class - Joystick

class Joystick {
  // -----------------------
  public:
    // -----------------------
    // Constructor
    //  @params {pinX} : the pin for the X axis [int]
    //          {pinY} : the pin for the Y axis [int]
    //          {pinBtn} : the pin for the button [int] (default -1)
    Joystick(int pinX, int pinY, int pinBtn = -1){
      _pinX = pinX;
      _pinY = pinY;
      _pinBtn = pinBtn;
      pinMode(_pinX, INPUT);
      pinMode(_pinY, INPUT);
      if(_pinBtn > -1)
        pinMode(_pinBtn, INPUT_PULLUP);

      _meanX = 500; // default
      _meanY = 500; // default
    }

    // -----------------------
    // Calibrate the mean axis of both axis
    //  @params {stream} : the stream to output the debug messages [Stream*] (default NULL)
    void calibrate(Stream *stream = NULL){
      if(stream != NULL)
        stream->print(F("Calibrating axis..."));

      // calibrate
      _meanX = 0; // reset
      _meanY = 0; // reset
      const int iterations = 10;
      for(int i=0 ; i < iterations ; i++){
        _meanX += getX();
        _meanY += getY();
      }
      _meanX /= iterations;
      _meanY /= iterations;
      
      if(stream != NULL)
        stream->println(F(" done!"));
    }

    // -----------------------
    // Get the value of the X axis
    //  @returns the X axis value [int]
    int getX(void){
      return get(&_pinX);
    }

    // -----------------------
    // Get the value of the Y axis
    //  @returns the Y axis value [int]
    int getY(void){
      return get(&_pinY);
    }

    // -----------------------
    // Get the value of the button
    //  @returns the button value [int]
    int getButton(void){
      return get(&_pinBtn);
    }

    // -----------------------
    // Get the mean value of the X axis
    //  @returns the X axis mean value [int]
    int meanX(void){
      return _meanX;
    }

    // -----------------------
    // Get the mean value of the Y axis
    //  @returns the Y axis mean value [int]
    int meanY(void){
      return _meanY;
    }

    // -----------------------
    // Print method overriden
    size_t printTo(Print& p) const {
      size_t size = 0;
      size += p.print(F("x{ "));
      size += p.print(getX());
      size += p.print(F(" ; "));
      size += p.print(meanX());
      size += p.print(F(" } y{ "));
      size += p.print(getY());
      size += p.print(F(" ; "));
      size += p.print(meanY());
      if(_pinBtn > -1){
        size += p.print(F(" } b{ "));
        size += p.print(getButton());
      }
      size += p.println(F(" }"));
      return size;
    }

  // -----------------------
  private:
    int _pinX, _pinY, _pinBtn;
    int _meanX, _meanY;

    // -----------------------
    // Get the value of a pin
    //  @params {pin*} : the pointer to the pin [int*]
    //  @returns [int]
    //            > [0 - 1023] for an analog pin
    //            > [HIGH / LOW] for a digital pin
    int get(int *pin){
      if(pin == &_pinBtn){
        return digitalRead(*pin);
      } else {
        analogRead(*pin); // firt call to change the channel
        delay(5); // pause to charge the sample & hold circuit
        return analogRead(*pin);
      }
    }
};

// ---------------------------------------------------------------
// Class - RoboServo

#define ROBOSERVO_MIN 0
#define ROBOSERVO_MAX 1

class RoboServo : public Printable, public Servo {
  // -----------------------
  public:
    // -----------------------
    // Constructor
    RoboServo(int pin) : Servo() {
      _limits[ROBOSERVO_MIN] = 0;     // default - MIN
      _limits[ROBOSERVO_MAX] = 180;   // default - MAX
      _pin = pin;
      pinMode(pin, INPUT); // leave floating until attached
    }

    // -----------------------
    // Attach the servo to the predefined pin
    //  @returns [uint8_t]
    //            > 0 if failure
    //            > the channel number
    uint8_t attach(void){
      Servo::attach(_pin); // attach the pin
      int mean = _limits[ROBOSERVO_MIN] + _limits[ROBOSERVO_MAX];
      mean /= 2;
      Servo::write(mean); // default angle
    }

    // -----------------------
    // Get the MAX limit
    //  @returns [int]
    int getMAX(void){
      return getLimit(ROBOSERVO_MAX);
    }

    // -----------------------
    // Get the MIN limit
    //  @returns [int]
    int getMIN(void){
      return getLimit(ROBOSERVO_MIN);
    }

    // -----------------------
    // Print method overriden
    size_t printTo(Print& p) const {
      size_t size = 0;
      size += p.print(Servo::attached());
      size += p.print(F(" { "));
      size += p.print(_limits[ROBOSERVO_MIN]);
      size += p.print(F(" ; "));
      size += p.print(Servo::read());
      size += p.print(F(" ; "));
      size += p.print(_limits[ROBOSERVO_MAX]);
      size += p.println(F(" }"));
      return size;
    }

    // -----------------------
    // Set the MAX limit
    //  @params {value} : the maximum angle [int]
    void setMAX(int value){
      setLimit(ROBOSERVO_MAX, value);
    }

    // -----------------------
    // Set the MIN limit
    //  @params {value} : the minimum angle [int]
    void setMIN(int value){
      setLimit(ROBOSERVO_MIN, value);
    }

    // -----------------------
    // Write the angle
    //  @params {value} : the desired angle [int]
    void write(int value){
      // constrain to limits
      if(value < _limits[ROBOSERVO_MIN])
        value = _limits[ROBOSERVO_MIN];
      if(value > _limits[ROBOSERVO_MAX])
        value = _limits[ROBOSERVO_MAX];

      Servo::write(value); // write the angle
    }

  // -----------------------
  private:
    int _limits[2]; // { min , max }
    int _pin;

    // -----------------------
    // Get an angle limit
    //  @params {index} : the desired limit [byte]
    //  @returns [int]
    int getLimit(byte index){
      if(index == ROBOSERVO_MAX){
        return _limits[ROBOSERVO_MAX];
      } else {
        return _limits[ROBOSERVO_MIN]; // default
      }
    }

    // -----------------------
    // Set an angle limit
    //  @params {index} : the desired limit [byte]
    //          {value} : the desired angle [int]
    void setLimit(byte index, int value){
      // check the index (redundant)
      if(index > ROBOSERVO_MAX)
        return;
      
      // constrain the value
      if(value < 0)
        value = 0;
      if(value > 180)
        value = 180;

      _limits[index] = value; // set

      // change the limits if necessary
      if(_limits[ROBOSERVO_MAX] < _limits[ROBOSERVO_MIN]){
        int value = _limits[ROBOSERVO_MIN];
        _limits[ROBOSERVO_MIN] = _limits[ROBOSERVO_MAX];
        _limits[ROBOSERVO_MIN] = value;
      }
    }
};

#undef ROBOSERVO_MIN
#undef ROBOSERVO_MAX

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// Variables

#define JOYSTICK_DEADBAND  200
Joystick jLeft(A2, A1, A0);
Joystick jRight(A5, A4, A3);

#define SERVO_STEP 5
enum Motor { Base , Reach , Height , Claw };
RoboServo servos[] = { RoboServo(8) , RoboServo(9) , RoboServo(10) , RoboServo(11) };

const int delay_loop = 20; // [ms]

int current_motor = Motor::Base;

int jValue; // auxiliary variable

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// Prototypes

int AngleCorrection(Motor, int);
void ReadConfig(void);

// ---------------------------------------------------------------
// ---------------------------------------------------------------

void setup(){
  // configure the serial output
  Serial.begin(115200);
  Serial.println(F("RoboARM - Demo"));
  Serial.println(F("\t\t10/12/18\n"));

#ifndef RESET_CONFIG
  ReadConfig();
#endif

  // calibrate the joysticks
  jLeft.calibrate(&Serial);
  jRight.calibrate(&Serial);

  // attach the servos and get the current angles
  for(int i=0 ; i <= Motor::Claw ; i++){
    servos[i].attach();
  }
}

// ---------------------------------------------------------------

void loop(){
  
  // -----------------------
  // -----------------------
  
  if(Serial.available()){
    char c = Serial.read();
    switch(c){
      // INFO
      case 'i': {
        Serial.print(F("Left: "));
        jLeft.printTo(Serial);
        Serial.print(F("Right: "));
        jRight.printTo(Serial);
        Serial.println(F("-----"));
        for(int i=0 ; i <= Motor::Claw ; i++){
          Serial.print('[');
          Serial.print(i);
          Serial.print("] ");
          servos[i].printTo(Serial);
        }
        break;
      }

      // SERVO SELECTION
      case '1':
        current_motor = Motor::Base;
        Serial.println(F("Base selected"));
        break;
      case '2':
        current_motor = Motor::Reach;
        Serial.println(F("Reach selected"));
        break;
      case '3':
        current_motor = Motor::Height;
        Serial.println(F("Height selected"));
        break;
      case '4':
        current_motor = Motor::Claw;
        Serial.println(F("Claw selected"));
        break;

      // ANGLE
      case 'a': {
        int angle = servos[current_motor].read();
        Serial.print(F("Current angle: "));
        Serial.println(angle);
        break;
      }
  
      // ANGLE +
      case '+': {
        int angle = servos[current_motor].read();
        angle += SERVO_STEP;
        angle = AngleCorrection(current_motor, angle); // validate the angle
        servos[current_motor].write(angle);
        Serial.print(F("Current angle: "));
        Serial.println(angle);
        break;
      }
  
      // ANGLE -
      case '-': {
        int angle = servos[current_motor].read();
        angle -= SERVO_STEP;
        angle = AngleCorrection(current_motor, angle); // validate the angle
        servos[current_motor].write(angle);
        Serial.print(F("Current angle: "));
        Serial.println(angle);
        break;
      }

      // SAVE - MIN
      case '<': {
        int angle = servos[current_motor].read();
        servos[current_motor].setMIN(angle);
        servos[current_motor].printTo(Serial);
        break;
      }
      // SAVE - MAX
      case '>': {
        int angle = servos[current_motor].read();
        servos[current_motor].setMAX(angle);
        servos[current_motor].printTo(Serial);
        break;
      }
    }
  }
  
  // -----------------------
  // -----------------------

  jValue = jLeft.getX() - jLeft.meanX();
  if(abs(jValue) > JOYSTICK_DEADBAND){
    int angle = servos[Height].read();
    if(jValue > 0){
      angle = AngleCorrection(Height, (angle - SERVO_STEP)); // validate the angle
      servos[Height].write(angle);
    } else {
      angle = AngleCorrection(Height, (angle + SERVO_STEP)); // validate the angle
      servos[Height].write(angle);
    }
  }
  
  jValue = jLeft.getY() - jLeft.meanY();
  if(abs(jValue) > JOYSTICK_DEADBAND){
    int angle = servos[Base].read();
    if(jValue > 0){
      angle = AngleCorrection(Base, (angle + SERVO_STEP)); // validate the angle
      servos[Base].write(angle);
    } else {
      angle = AngleCorrection(Base, (angle - SERVO_STEP)); // validate the angle
      servos[Base].write(angle);
    }
  }
  
  jValue = jRight.getX() - jRight.meanX();
  if(abs(jValue) > JOYSTICK_DEADBAND){
    int angle = servos[Reach].read();
    if(jValue > 0){
      angle = AngleCorrection(Reach, (angle - SERVO_STEP)); // validate the angle
      servos[Reach].write(angle);
    } else {
      angle = AngleCorrection(Reach, (angle + SERVO_STEP)); // validate the angle
      servos[Reach].write(angle);
    }
  }
  
  jValue = jRight.getY() - jRight.meanY();
  if(abs(jValue) > JOYSTICK_DEADBAND){
    int angle = servos[Claw].read();
    if(jValue > 0){
      angle = AngleCorrection(Claw, (angle + SERVO_STEP)); // validate the angle
      servos[Claw].write(angle);
    } else {
      angle = AngleCorrection(Claw, (angle - SERVO_STEP)); // validate the angle
      servos[Claw].write(angle);
    }
  }
  
  // -----------------------

  // open the claw
  jValue = jRight.getButton();
  if(jValue == LOW){
    delay(20);
    if(jRight.getButton() == LOW){
      int angle = servos[Claw].getMIN(); // get the minimum angle
      angle = AngleCorrection(Claw, angle); // validate the angle
      servos[Claw].write(angle);
    }
  }
  
  // -----------------------
  // -----------------------

  delay(delay_loop);
  
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------

// height = (1/a) * reach + b
#define ANGLE_CORRECTION_A   -0.75
#define ANGLE_CORRECTION_B     165

// Correct the angle according to the motor
//  @params {motor} : the motor being controlled [Motor]
//          {angle} : the angle being sent [int]
//  @returns the corrected angle [int]
int AngleCorrection(Motor motor, int angle){
  
  if(motor == Height){
    int max_angle = servos[Reach].read();
    max_angle = (float)max_angle * ANGLE_CORRECTION_A;
    max_angle += ANGLE_CORRECTION_B;

    if(angle < max_angle)
      angle = max_angle;
  } else if(motor == Reach){
    int max_angle = servos[Height].read();
    max_angle -= ANGLE_CORRECTION_B;
    max_angle = (float)max_angle / ANGLE_CORRECTION_A;

    if(angle < max_angle)
      angle = max_angle;
  }

  // check angle limits
  if(angle > servos[motor].getMAX())
    angle = servos[motor].getMAX();
  if(angle < servos[motor].getMIN())
    angle = servos[motor].getMIN();
  
  return angle;
}

// ---------------------------------------------------------------

#define EEPROM_ADDRESS_START  10

#define EEPROM_DEFAULT_MIN_BASE      10
#define EEPROM_DEFAULT_MIN_REACH     60
#define EEPROM_DEFAULT_MIN_HEIGHT    60
#define EEPROM_DEFAULT_MIN_CLAW     100
#define EEPROM_DEFAULT_MAX_BASE     170
#define EEPROM_DEFAULT_MAX_REACH    170
#define EEPROM_DEFAULT_MAX_HEIGHT   170
#define EEPROM_DEFAULT_MAX_CLAW     170

// Read the configuration data from the EEPROM
void ReadConfig(void){
  int address = EEPROM_ADDRESS_START;
  byte v1 = EEPROM.read(address++);
  byte v2 = EEPROM.read(address++);

  bool blank = ((v1 == 131) && (v2 == 237)) ? false : true;

  if(blank){
    // write data
    EEPROM.write(address++, EEPROM_DEFAULT_MIN_BASE);
    EEPROM.write(address++, EEPROM_DEFAULT_MIN_REACH);
    EEPROM.write(address++, EEPROM_DEFAULT_MIN_HEIGHT);
    EEPROM.write(address++, EEPROM_DEFAULT_MIN_CLAW);
    EEPROM.write(address++, EEPROM_DEFAULT_MAX_BASE);
    EEPROM.write(address++, EEPROM_DEFAULT_MAX_REACH);
    EEPROM.write(address++, EEPROM_DEFAULT_MAX_HEIGHT);
    EEPROM.write(address++, EEPROM_DEFAULT_MAX_CLAW);
    
    address = EEPROM_ADDRESS_START; // reset
    EEPROM.write(address++, 131);
    EEPROM.write(address++, 237);
  }

  // read data
  servos[Base].setMIN(EEPROM.read(address++));
  servos[Reach].setMIN(EEPROM.read(address++));
  servos[Height].setMIN(EEPROM.read(address++));
  servos[Claw].setMIN(EEPROM.read(address++));
  servos[Base].setMAX(EEPROM.read(address++));
  servos[Reach].setMAX(EEPROM.read(address++));
  servos[Height].setMAX(EEPROM.read(address++));
  servos[Claw].setMAX(EEPROM.read(address++));
}

#undef EEPROM_ADDRESS_START

// ---------------------------------------------------------------
