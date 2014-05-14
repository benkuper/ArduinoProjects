#include "BufferedSerial.h"
#include "ByteBuffer.h"
BufferedSerial serial = BufferedSerial(256, 256);
ByteBuffer send_buffer;

#include <AccelStepper.h>
// Define a stepper and the pins it will use
//AccelStepper stepper(2,2,3); // Nb pins, step, dir
AccelStepper stepper(2, 4, 5);
//AccelStepper stepper(2, 6, 7);
bool stepperSpeedMode = false;

byte buffer[32];
int bufferIndex = -1;


void setup()
{
  // initialize the serial communication:
  serial.init(0, 9600);
  serial.setPacketHandler(handlePacket);

  // Initialize the send buffer that we will use to send data
  send_buffer.init(30);
  
  setMotorMaxSpeed(1000);
  setMotorAcceleration(1000);
  homeMotor();
  //setMotorSpeed(-500);
 // setMotorPosition(-20);
}

void loop()
{
  serial.update();
  processMotor();
}


void handlePacket(ByteBuffer* packet)
{
  // Here we could do anything we want to the data but for now we will just send it back
  byte command = packet->get();
  
  switch (command)
  {
    case 'h':
      homeMotor();
      break;

    case 's':
      setMotorSpeed(packet->getFloat());
      break;

    case 'p':
      setMotorPosition(packet->getLong());
      break;

    case 'a':
      setMotorAcceleration(packet->getFloat());
      break;

    case 'm':
      setMotorMaxSpeed(packet->getFloat());
      break;
  }
  
}

void processMotor()
{
  if (stepperSpeedMode) stepper.runSpeed();
  else stepper.run();
}

void homeMotor()
{
  stepper.setCurrentPosition(0);
  setMotorPosition(0);
}

void setMotorSpeed(float value)
{
  stepperSpeedMode = true;
  stepper.setSpeed(value);
  sendValue('s',stepper.speed());
}

void setMotorPosition(long value)
{
  stepperSpeedMode = false;
  stepper.moveTo(value);
  sendLong('d',stepper.targetPosition());
}

void setMotorAcceleration(float value)
{
  stepper.setAcceleration(value);
}

void setMotorMaxSpeed(float value)
{
  stepper.setMaxSpeed(value);
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

