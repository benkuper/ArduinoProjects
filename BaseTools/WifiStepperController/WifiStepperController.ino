#include "BufferedSerial.h"
#include "ByteBuffer.h"
BufferedSerial serial = BufferedSerial(256, 256);
ByteBuffer send_buffer;

#include <AccelStepper.h>

// Define a stepper and the pins it will use
#define NUM_STEPPERS 3

AccelStepper stepper1(2, 2, 3); // Nb pins, step, dir
AccelStepper stepper2(2, 4, 5);
AccelStepper stepper3(2, 6, 7);
AccelStepper steppers[3] = {stepper1,stepper2,stepper3};

bool stepperSpeedModes[3] = {false,false,false};

byte buffer[32];
int bufferIndex = -1;

#include <Adafruit_CC3000.h>
#include <SPI.h>
#include "utility/debug.h"
#include "utility/socket.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  8
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "belkin.d2c"           // cannot be longer than 32 characters!
#define WLAN_PASS       "baeb9646"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           7    // What TCP port to listen on for connections.  The echo protocol uses port 7.

Adafruit_CC3000_Server echoServer(LISTEN_PORT);

void setup()
{
  // initialize the serial communication:
  serial.init(0, 9600);
  serial.setPacketHandler(handlePacket);

  // Initialize the send buffer that we will use to send data
  send_buffer.init(30);
  
  for(int i=0;i<NUM_STEPPERS;i++)
  {
    setMotorMaxSpeed(i,800);
    setMotorAcceleration(i,1000);
    homeMotor(i);
  }
  
  //setMotorSpeed(-500);
 // setMotorPosition(-20);
 
 
 
 //Setup wifi
 /* Initialise the module */
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
  Serial.println(F("Connected!"));
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  
  
 echoServer.begin();
 Serial.println(F("Listening for connections..."));
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
  sendValue('m',stepperIndex);
  
  switch (command)
  {
    case 'h':
      homeMotor(stepperIndex);
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
  steppers[sid].setCurrentPosition(0);
  setMotorPosition(sid,0);
}

void setMotorSpeed(int sid, float value)
{
  stepperSpeedModes[sid] = true;
  steppers[sid].setSpeed(value);
  sendValue('s',steppers[sid].speed());
}

void setMotorPosition(int sid, long value)
{
  stepperSpeedModes[sid] = false;
  steppers[sid].moveTo(value);
  sendLong('d',steppers[sid].targetPosition());
}

void setMotorAcceleration(int sid, float value)
{
  steppers[sid].setAcceleration(value);
}

void setMotorMaxSpeed(int sid, float value)
{
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

