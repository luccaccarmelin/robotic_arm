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

  void calibrate(Stream = NULL){
	  if(stream  != NULL)
		  stream->print(F("Calibrating axis..."));

	  _meanX = 0; 
	  _meanY = 0;
	  const int iterations = 10;
	  for(int i=0 ; i < iterations ; i++){
		  _meanX /= iterations;
		  _meanY /= iterations;

		  if(stream != NULL)
			  stream->println(F(" done!"));
	  }

	  int getX(void){
		  return get(&_pinX);
	  }

	  int getY(void){
                  return get(&_pinY);


	  int getButton(void){
		  return get(&_pinBtn);
	  }

	            int meanX(void){
                  return get(_meanY);
          }
	  
	            int meanY(void){
                  return get(_meanY);
	  }


	  size_t printTo(Print& p) const {
		  size += p.print(F("x{ "));
		  size += p.print(getX());
		  size += p.print(F(" ; "));
		  size += p.print(meanX());
		  size += p.print(F(" } y{ "));
		  size += p.print(getY());
		  size += p.print(F(" ; "));
		  size += p.print(meanY());
		  if(pinBtn > -1){
			  size += p.print(F(" } b{ "));
			  size += p.print(getButton());
	 }
	 
	 size += p.println(F(" }"));
	 return size;
       }

     private:
	  int _pinX, _pinY, _pinBtn;
	  int _meanX, _meanY;

	  int get(int *pin){
		  if(pin == &_pinBtn){
			return digitalRead(*pin);
		      }     
		        else {
			analogRead(*pin);
			delay(5);
			return analogRead(*pin);
		      }
		  }
	};

	
	#define ROBOSERVO_MIN 0
	#define ROBOSERVO MAX 1

	
	class RoboServo : public printable, public servo {

		public:
		
			RoboServo(int pin)  : Servo() {
				_limits[ROBOSERVO_MIN] = 0;
				_limits[ROBOSERVO_MAX] = 180;
				_pin = pin;
				pinMode(pin, INPUT);
			}

			        unit_t attach(void){
				servo::attach(_pin);
				int mean = _limits[ROBOSERVO_MIN] + _limits[ROBOSERVO_MAX];
				mean /= 2;
				Servo::Write(mean);
			     }
				
				int getMAX(void){
					return getLimit(ROBOSERVO_MAX);
				int geMIN(void){
					return getLimit(ROBOSERVO_MIN);
					size_t printTo(print& p) const {
					 size_t size = 0;
					 size += p.print(Servo::attached());
					 size += p.print(F(" { "));
					 size += p.print(_limits[ROBOSERVO_MIN]);
					 size += p.print(F(" ; "));
					 size += p.print(Servo::read());
					 size += p.print(F(" ; "));
					 size += p.print(_limits[ROBOSERVO_MAX]);
					 size += p.print(F(" }"));
					 return size;
				  }

				 void setMAX(int value){
					 setlimit(ROBOSERVO_MAX, value);
				 }

				 void setMIN(int value){
                                         setlimit(ROBOSERVO_MIN, value);
                                 }

				 void write(int value){

					 if(value < _limits[ROBOSEERVO_MIN])
					       value = _limits[ROBOSERVO_MIN];
					 if(value > _limits[ROBOSERVO_MAX])
					       value = _limits[ROBOSERVO_MAX];
			private:
					 int _limits[2];
					 int _pin;

					 int getLimit(byte index){
						 if(index == ROBOSERVO_MAX){
							 return _limits[ROBOSERVO_MAX];
						 }	 else {
							 return _limits[ROBOSERVO_MIN];
							}
				        }





				  	void setLimit(byte index, int value){
						if(index > ROBOSERVO_MAX)


						if(value < 0)
							value = 0;
						if(value > 180)
							value = 180;

						_limits[index] = value;

						if(_limits[ROBOSERVO_MAX] < _limits[ROBOSERVO_MIN]){
							int
							_limits[ROBOSERVO_MIN] = _limits[ROBOSERVO_MAX];
							_limits[ROBOSERVO_MIN] = value;
						}
					     }
				 };

				#undef ROBOSERVO_MIN
				#undef ROBOSERVO_MAX

				#define JOYSTICK_DEADBAND  200

				 joystick jLeft(A2, A1, A0,);
				 joystick jRigth(A5, A4, A3,);
				
				 #define SERVO_STEP 5
				 enum Motor {Base , Reach , Heigth , Claw};
				 Roboservo servos[] = {RoboServo(8) , RoboServo(9) , RoboServo(10) , Roboservo(11)};
				 const int delay_loop = 20;
				 int current_motor = Motor::Base;
				 int jValue;
				 int AngleCorrection(Motor, int);
				 void ReadConfig(void);

				 void setup(){
					 Serial.begin(115200);
					 Serial.println(F("RoboARM - Demo"));
					 Serial.println(F("\t\t10/12/18\n"));
					 #ifndef RESET_CONFIG
					   ReadConfig();
					 #endif

					   jLeft.calibrate(&Serial);
					   jRigth.calibrate(&Serial);

					   for(int i=0 ; i <= Motor::Claw ; i++){
						   servos[i].attach();
						 }
				 	      }

				 	      void loop(){

						      if(Serial.available()){
							      char c = Serial.read();
							      switch(c){
								 case 'i': {
								  Serial.print(F("left: "));
								  jleft.printTo(Serial);
								  Serial.print(F("Rigth: "));
								  jRigth.printTo(Serial);
								  Serial.print(F("-----"));
								  for(int i=0 ; i <= Motor::Claw ; i++){
									  Serial.print('[');
	    Serial.print(i);
	    Serial.print("] ");
	    Servos[i].printTo(Serial);
	  }
	  break;
	}

        case '1':
	    current_motor = Motor::Base;
	    Serial.println(F("Base selected"));
	    break;
	case '2':
	    current_motor = Motor::Reach;
	    Serial.println(F("Reach selected"));
	    break;
	case '3':
	    current_motor = Motor::Heigth;
	    Serial.println(F("Heigth selected"));
	    break;
	case '4':
	    current_motor = Motor::Claw;
	    Serial.println(F("Claw selected"));
	    break;


	case 'a': {
			  int angle = servos[current_motor].read();
			  Serial.print(F("Current angle: "));
			  Serial.println(angle);
			  break;
			}

	case '+': {
			  int angle = servos[current_motor].read();
			  angle += SERVO_STEP
			  angle = angleCorrection(current_motor, angle);
			  servos[current_motor].write(angle);
			  Serial.print(F("current angle: "));
			  Serial.println(angle);
			  break;
			}

       case '<': {
			 int angle = servos[current_motor].read();
			 servos[current_motor].setMIN(angle);
			 servos[current_motor].printTo(Serial);
			 break;
		       }









