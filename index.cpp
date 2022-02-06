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




