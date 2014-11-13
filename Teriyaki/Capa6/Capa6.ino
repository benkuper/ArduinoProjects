#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

int arduinoID = 0;
CapacitiveSensor capa[6] = 
{
  CapacitiveSensor(2,3);
  CapacitiveSensor(2,4);
  CapacitiveSensor(2,5);
  CapacitiveSensor(2,6);
  CapacitiveSensor(2,7);
  CapacitiveSensor(2,8);
}
long values[6];
/*
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_4_3 = CapacitiveSensor(4,3);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_4_4 = CapacitiveSensor(4,4);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_4_5 = CapacitiveSensor(4,5);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_4_6 = CapacitiveSensor(4,6);        // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_4_7 = CapacitiveSensor(4,7);        // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
*/



void setup()                    
{
   for(int i=0;i<6;i++)
   {
     capa[i] = new CapacitiveSensor(2,3+i);
     values[i] = 0;
   }
  /*
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   cs_4_3.set_CS_AutocaL_Millis(0xFFFFFFFF); 
   cs_4_4.set_CS_AutocaL_Millis(0xFFFFFFFF); 
   cs_4_5.set_CS_AutocaL_Millis(0xFFFFFFFF); 
   cs_4_6.set_CS_AutocaL_Millis(0xFFFFFFFF); 
   cs_4_7.set_CS_AutocaL_Millis(0xFFFFFFFF); 
   */
   Serial.begin(9600);
}

void loop()                    
{
    long start = millis();
    for(int i=0;i<6;i++)
    {
      values[i] =  capa[i].capacitiveSensor(30);
      Serial.print(values[i]);
    }
    Serial.println("**");
  /*
    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug windown spacing

    Serial.print(total1);                  // print sensor output 1
    Serial.print("\t");
    Serial.print(total2);                  // print sensor output 2
    Serial.print("\t");
    Serial.println(total3);                // print sensor output 3
    */
    delay(10);                             // arbitrary delay to limit data to serial port 
}
