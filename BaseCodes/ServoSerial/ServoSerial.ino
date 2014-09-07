#include <Servo.h> 

byte buffer[32];
int bufferIndex = -1;

Servo servo1;
Servo servo2;

Servo servos[] = {servo1,servo2};

int targetVal;
boolean prevState;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,HIGH);
  
  servo1.attach(9);
  servo2.attach(10);
  pinMode(2,INPUT_PULLUP);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  processSerial();
  
  boolean state = digitalRead(2);
  if(state != prevState)
  {
    
    prevState = state;
  }
  
}

void processSerial()
{
  while(Serial.available() > 0)
  {
    byte b = Serial.read();
    if(b == 255) processBuffer();
    else if(bufferIndex < 31) buffer[bufferIndex++] = b;
  }
}

void processBuffer()
{
  digitalWrite(13,HIGH);
  Serial.print("Process Buffer :");
  Serial.print(buffer[0]);
  Serial.println("");
  
  switch(buffer[0])
  {
     case 's':
     {
       int val = map(buffer[2],0,254,0,179);
       Serial.print("--> ");
       Serial.println(val);
       servos[buffer[1]].write(val);
     }
     break;
     
     default:
     break;
  }
  
  bufferIndex = 0;
  digitalWrite(13,LOW);
}
