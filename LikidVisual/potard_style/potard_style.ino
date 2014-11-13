
#include <OctoWS2811.h>

const int ledsPerStrip = 300;
const int numStrips = 6; //6 bandes de 5m
const int numLeds = numStrips * ledsPerStrip; //nb total de leds
  
//Réglage vitesse et visuel leds
const int ledsPerGroup = 10; //nombre de leds par tranche
const int gap = 20; //espace entre les tranches
const int minSpeed = 0; //vitesse quand potard à 0 (tu peux mettre une valeur négative pour que ca aille dans l'autre sens)
const int maxSpeed = 10; //vitesse quand potard à fond.
const int inactiveColor = 0x000000;
const int activeColor = 0x333333;

//variables de calcul
int pos = 0; //position actuelle

const int spacing = ledsPerGroup+gap;
int numGroups = floor(numLeds/spacing);


//WS2811
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  leds.begin();
  
}

void loop() {
  
  int potardVal = analogRead(0);
  //int ledIndex = map(potardVal,0,1024,0,numLeds);
  int curSpeed = map(potardVal,0,1024,minSpeed,maxSpeed);
  
  pos = ((pos+curSpeed)+numLeds)%numLeds;
  
  //Clean des leds, on met tout à la couleur inactive
  for(int k=0;k<numLeds;k++)
  {
    leds.setPixel(k,inactiveColor);
  }
  
  for(int i=0;i<numGroups;i++)
  {
    int groupStart = i*spacing;
    
    for(int j=0;j<ledsPerGroup;j++)
    {
      int ledPos = groupStart+j;
      int ledPosOffset = (ledPos+pos+numLeds)%numLeds; //avec modulo pour gérer la liaison fin/début de strip
      leds.setPixel(ledPosOffset,activeColor); //on assigne à la led la couleur calculée
    }
  }
  
  leds.show();
  
  delay(10); //évite de surcharger le bordel. A diminuer si nécessaire
}
