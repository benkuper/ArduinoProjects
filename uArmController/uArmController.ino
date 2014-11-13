#include <EEPROM.h>
#include <UF_uArm.h>

UF_uArm uarm;           // initialize the uArm library 

byte buffer[32];
int bufferIndex = 0;

void setup() 
{
  Serial.begin(9600);  // start serial port at 9600 bps
//  while(!Serial);     // wait for serial port to connect. Needed for Leonardo only
  uarm.init();          // initialize the uArm position
  uarm.setServoSpeed(SERVO_R,    0);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_L,    0);  // 0=full speed, 1-255 slower to faster
  uarm.setServoSpeed(SERVO_ROT, 0);  // 0=full speed, 1-255 slower to faster
}

void loop() {
  // put your main code here, to run repeatedly:
  uarm.calibration(); 
  processSerial();
  delay(2); //safe
}

void processSerial()
{
  while(Serial.available() > 0)
  {
    byte b = Serial.read();
    //Serial.print(buffer[0]);
    if(b == 255) processBuffer();
    else if(bufferIndex < 32)
     {
       buffer[bufferIndex] = b;
       bufferIndex++;
     }
   }
}

void processBuffer()
{
  switch(buffer[0])
  {
     case 'p': //position
     {
       if(bufferIndex == 5)
       {
         int stretch = map(buffer[1],0,254,0,210);
         int height = map(buffer[2],0,254,130,-90);
         int rotation = map(buffer[3],0,254,-90,90);
         int handRot = map(buffer[4],0,254,-55,55);
         
         uarm.setPosition(stretch, height, rotation, handRot);
       }
       
       
       
       
     }
       break;
       
     case 's': //speed
     {
       bufferIndex >= 5;
       int target = int(buffer[1]);
       uarm.setServoSpeed(target, buffer[2]);
     }
       break;
     
     case 'g': //Gripper
     {
       boolean doCatch = buffer[1] > 0;
       
       if(doCatch) uarm.gripperCatch();
       else uarm.gripperRelease();
     }
       break;
       
     
     default:
     break;
  }
  
  bufferIndex = 0;
}
