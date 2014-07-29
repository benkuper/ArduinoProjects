
#include <OctoWS2811.h>

#define ledsPerStrip 250
#define numStrips 6
#define numLeds ledsPerStrip*numStrips

DMAMEM int displayMemory[numLeds];
int drawingMemory[numLeds];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

//const int maxBuffer = numLeds *3;
//byte ledBuffer[maxBuffer];

int ledBufferIndex = 0;
uint8_t colorBuffer[3];
int colorBufferIndex = 0;

void setup()
{
  Serial.begin(460800);
  leds.begin();
  pinMode(13,OUTPUT);
}

void loop()
{
  processSerial();
}

void processSerial()
{
    while(Serial.available() > 0)
    {
      
      byte b = Serial.read();
      if(b == 255) processBuffer();
      //if(b == 'c') processBuffer();
      else if(ledBufferIndex < numLeds) 
      {
        colorBuffer[colorBufferIndex] = (uint8_t)b;
        colorBufferIndex++;
        if(colorBufferIndex == 3)
        {
          //int targetLed = ledBufferIndex;
          //if(targetLed >= ledsPerStrip && targetLed < ledsPerStrip*2) targetLed += ledsPerStrip*4;
          leds.setPixel(ledBufferIndex,colorBuffer[0],colorBuffer[1],colorBuffer[2]);
          colorBufferIndex = 0;
          ledBufferIndex++;
        }
      }
    }
}

void processBuffer()
{
  digitalWrite(13,HIGH);
  leds.show();
  ledBufferIndex = 0;
  digitalWrite(13,LOW);
  /*
  //Serial.print("processBuffer : ");
  //Serial.println(numLeds);
  //Serial.println(maxBuffer);
  digitalWrite(13,HIGH);
  
  for(uint8_t i=0;i<numLeds;i++)
  {
    int bi = i*3;
    if(i >= ledsPerStrip*5) bi -= ledsPerStrip*3; //set 5 to 2 
    uint8_t red = ledBuffer[bi];
    uint8_t green = ledBuffer[bi+1];
    uint8_t blue = ledBuffer[bi+2];
    //int c = (red << 16) | (green << 8) | blue;
    leds.setPixel(i,red,green,blue);
  }
  leds.show();
  digitalWrite(13,LOW);
  //Serial.print("byte index : ");
  //Serial.println(ledBufferIndex);
  ledBufferIndex = 0;
  memset(ledBuffer,0,maxBuffer);
  */
  
}
