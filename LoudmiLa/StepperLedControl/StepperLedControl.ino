#include "BufferedSerial.h"
#include "ByteBuffer.h"
BufferedSerial serial = BufferedSerial(256, 256);
ByteBuffer send_buffer;

#include <AccelStepper.h>

// Define a stepper and the pins it will use
#define NUM_STEPPERS 2

AccelStepper stepper1(2, 2, 3); // Nb pins, step, dir
AccelStepper stepper2(2, 4, 5);
AccelStepper steppers[NUM_STEPPERS] = {stepper1,stepper2};

bool stepperSpeedModes[NUM_STEPPERS] = {false,false};

byte buffer[32];
int bufferIndex = -1;


void setup()
{
  // initialize the serial communication:
  serial.init(0, 9600);
  serial.setPacketHandler(handlePacket);

  // Initialize the send buffer that we will use to send data
  send_buffer.init(30);
  
  for(int i=0;i<NUM_STEPPERS;i++)
  {
    setMotorMaxSpeed(i,1000);
    setMotorAcceleration(i,500);
    homeMotor(i);
  }
  
  //setMotorSpeed(-500);
 // setMotorPosition(-20);
}

void loop()
{
  serial.update();
  processMotors();
}


void handlePacket(ByteBuffer* packet)
{
  // Here we could do anything we want to the data but for now we will just send it back
  byte command = packet->get();
  long stepperIndex = packet->getLong();
  
  if(stepperIndex >= NUM_STEPPERS) return;
  
  sendValue('m',stepperIndex);
  
  
  switch (command)
  {
    case 'h':
      homeMotor(stepperIndex);
      break;
      
   case 'd':
     defineMotorPosition(stepperIndex,packet->getLong());
     break;

    case 's':
      setMotorSpeed(stepperIndex,packet->getFloat());
      break;

    case 'p':
      setMotorPosition(stepperIndex,packet->getLong());
      break;

    case 'a':
      setMotorAcceleration(stepperIndex,packet->getFloat());
      break;

    case 'm':
      setMotorMaxSpeed(stepperIndex,packet->getFloat());
      break;
  }
  
}

void processMotors()
{
  for(int i=0;i<NUM_STEPPERS;i++)
  {
    if (stepperSpeedModes[i]) steppers[i].runSpeed();
    else steppers[i].run();
  }
}

void homeMotor(int sid)
{
  if(sid >= NUM_STEPPERS) return;
  steppers[sid].setCurrentPosition(0);
  setMotorPosition(sid,0);
}

void defineMotorPosition(int sid, long value)
{
  if(sid >= NUM_STEPPERS) return;
  steppers[sid].setCurrentPosition(value);
  setMotorPosition(sid,value);
}

void setMotorSpeed(int sid, float value)
{
  if(sid >= NUM_STEPPERS) return;
  stepperSpeedModes[sid] = true;
  steppers[sid].setSpeed(value);
  sendValue('s',steppers[sid].speed());
}

void setMotorPosition(int sid, long value)
{
  if(sid >= NUM_STEPPERS) return;
  stepperSpeedModes[sid] = false;
  steppers[sid].moveTo(value);
  sendLong('d',steppers[sid].targetPosition());
}
  
void setMotorAcceleration(int sid, float value)
{
  if(sid >= NUM_STEPPERS) return;
  steppers[sid].setAcceleration(value);
}

void setMotorMaxSpeed(int sid, float value)
{
  if(sid >= NUM_STEPPERS) return;
  steppers[sid].setMaxSpeed(value);
}


//Send messages
void sendValue(byte command, float value)
{
  send_buffer.clear();
  send_buffer.put(command);
  send_buffer.putFloat(value);  
  
  serial.sendSerialPacket( &send_buffer );
}

void sendLong(byte command, long value)
{
  send_buffer.clear();
  send_buffer.put(command);
  send_buffer.putLong(value);  
  
  serial.sendSerialPacket( &send_buffer );
}

