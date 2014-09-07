#define EOM 255 //define End of Message

#include "FastLED.h"

byte buffer[250];
int bufferIndex = -1;

#define NUM_LEDS 5
CRGB leds[NUM_LEDS];

const int numButtons = NUM_LEDS;
boolean buttons[numButtons];
boolean prevStates[numButtons];
int pins[] = {8,9,10,11,12};

void setup() {
  
  for(int i=0;i<numButtons;i++)
  {
    pinMode(pins[i],INPUT_PULLUP);
  }
  
  LEDS.addLeds<WS2811, 7, GRB>(leds, NUM_LEDS);
  
  
  leds[0] = CRGB(255,0,255);
  LEDS.show();
  
  Serial.begin(9600);
}

void loop() { 
  
  processSerial();
  
  boolean changed = false;
  
  for(int i=0;i<numButtons;i++)
  {
    buttons[i] = !digitalRead(pins[i]);
    if(buttons[i] != prevStates[i])
    {
      if(buttons[i]) buttonPressed(i);
      else buttonReleased(i);
      
     changed = true;
    }
    
    prevStates[i] = buttons[i];
  }
  
  LEDS.show();
  if(changed) delay(40);
}

void buttonPressed(byte bt)
{
  sendButton(bt,1);
}

void buttonReleased(byte bt)
{
  sendButton(bt,0);
}

void sendButton(byte bt, byte value)
{
  Serial.write('b');
  Serial.write(bt);
  Serial.write(value);
  Serial.write(EOM);
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
  
  switch(buffer[0])
  {
     case 'c':
     if(bufferIndex == 5)
     {
      leds[buffer[1]] = CRGB(buffer[2],buffer[3],buffer[4]);
     }
     break;
     
     case 'l':
       for(int i=0;i<NUM_LEDS;i++)
      {
        leds[i] = CRGB(buffer[1+i*3],buffer[2+i*3],buffer[3+i*3]);
      }
     break;
     
     case 'a':
     if(bufferIndex == 4)
     {
      for(int i=0;i<NUM_LEDS;i++)
      {
        leds[i] = CRGB(buffer[1],buffer[2],buffer[3]);
      }
      LEDS.show();
     }
     break;
     
     default:
     break;
  }
  
  bufferIndex = 0;
}
