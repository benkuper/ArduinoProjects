#include "FastLED.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// test code
//
//////////////////////////////////////////////////

#define NUM_LEDS 30
CRGB leds[NUM_LEDS];

void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(500);

  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  
  LEDS.setBrightness(64);
  LEDS.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS);
}

void loop() { 
  memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
  
  leds[5] = CRGB(255,0,255);
  leds[6].g = 255;
  
  LEDS.show();
  delay(10);
}
