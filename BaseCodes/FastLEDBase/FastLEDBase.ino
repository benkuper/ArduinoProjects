#include "FastLED.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// test code
//
//////////////////////////////////////////////////

#define NUM_LEDS 50

CRGB leds[NUM_LEDS];

void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(500);

   	// For safety (to prevent too high of a power draw), the test case defaults to
   	// setting brightness to 25% brightness
   	LEDS.setBrightness(64);
   	 LEDS.addLeds<WS2811, 2>(leds, NUM_LEDS);
    LEDS.addLeds<WS2811, 3>(leds, NUM_LEDS);
     LEDS.addLeds<WS2811, 4>(leds, NUM_LEDS);
      LEDS.addLeds<WS2811, 5>(leds, NUM_LEDS);
       LEDS.addLeds<WS2811, 6>(leds, NUM_LEDS);
   pinMode(13,OUTPUT);
}

void loop() { 
  
	for(int i = 0; i < 3; i++) {
  digitalWrite(13,HIGH);
		for(int iLed = 0; iLed < NUM_LEDS; iLed++) {
			memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

			switch(i) { 
				// You can access the rgb values by field r, g, b
			 	case 0: leds[iLed].r = 128; break;

			 	// or by indexing into the led (r==0, g==1, b==2) 
			 	case 1: leds[iLed][i] = 128; break;

			 	// or by setting the rgb values for the pixel all at once
			 	case 2: leds[iLed] = CRGB(0, 0, 128); break;
			}

			// and now, show your led array! 
			LEDS.show();
			delay(10);
		}

digitalWrite(13,LOW);
		// fade up
		for(int x = 0; x < 128; x++) { 
			// The showColor method sets all the leds in the strip to the same color
			LEDS.showColor(CRGB(x, 0, 0));
			delay(10);
		}
digitalWrite(13,HIGH);
		// fade down
		for(int x = 128; x >= 0; x--) { 
			LEDS.showColor(CRGB(x, 0, 0));
			delay(10);
		}
digitalWrite(13,LOW);
		// let's fade up by scaling the brightness
		for(int scale = 0; scale < 128; scale++) { 
			LEDS.showColor(CRGB(0, 128, 0), scale);
			delay(10);
		}
digitalWrite(13,HIGH);
		// let's fade down by scaling the brightness
		for(int scale = 128; scale > 0; scale--) { 
			LEDS.showColor(CRGB(0, 128, 0), scale);
			delay(10);
		}
	}
}
