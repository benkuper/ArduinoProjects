#include <Servo.h> 

Servo servo1;
Servo servo2;

Servo servos[] = {servo1,servo2};

int targetVal;
int servoVal;

boolean prevState;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,HIGH);
  
  servo1.attach(9);
  servo2.attach(10);
  pinMode(2,INPUT_PULLUP);
  
  
  servoVal = 0;
  targetVal = 0;
}

void loop() {

  boolean state = !digitalRead(2);
  if(state != prevState)
  {
    if(state)
    {
      servo2.write(90);
    }else
    {
      servo2.write(0);
    }
    
    prevState = state;
    
    
  }  
}
