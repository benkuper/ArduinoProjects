#include "MPR121.h"
#include <Wire.h>

#define MPR_ADD 0x5C

int irqpin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  
  Serial.begin(9600);
  Wire.begin();
  
  Serial.println("MPR setup...");
  mpr121_setup();
  Serial.println("MPR setup done.");
}

void loop(){
  readTouchInputs();
}


void readTouchInputs(){
  //Serial.println("read Touch inputs !");
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(MPR_ADD,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");
        
        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }  
      
        touchStates[i] = 1;      
      }else{
        if(touchStates[i] == 1){
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");
          
          //pin i is no longer being touched
       }
        
        touchStates[i] = 0;
      }
    
    }
    
  }
}




void mpr121_setup(void){

  set_register(MPR_ADD, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(MPR_ADD, MHD_R, 0x01);
  set_register(MPR_ADD, NHD_R, 0x01);
  set_register(MPR_ADD, NCL_R, 0x00);
  set_register(MPR_ADD, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(MPR_ADD, MHD_F, 0x01);
  set_register(MPR_ADD, NHD_F, 0x01);
  set_register(MPR_ADD, NCL_F, 0xFF);
  set_register(MPR_ADD, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(MPR_ADD, ELE0_T, TOU_THRESH);
  set_register(MPR_ADD, ELE0_R, REL_THRESH);
 
  set_register(MPR_ADD, ELE1_T, TOU_THRESH);
  set_register(MPR_ADD, ELE1_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE2_T, TOU_THRESH);
  set_register(MPR_ADD, ELE2_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE3_T, TOU_THRESH);
  set_register(MPR_ADD, ELE3_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE4_T, TOU_THRESH);
  set_register(MPR_ADD, ELE4_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE5_T, TOU_THRESH);
  set_register(MPR_ADD, ELE5_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE6_T, TOU_THRESH);
  set_register(MPR_ADD, ELE6_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE7_T, TOU_THRESH);
  set_register(MPR_ADD, ELE7_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE8_T, TOU_THRESH);
  set_register(MPR_ADD, ELE8_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE9_T, TOU_THRESH);
  set_register(MPR_ADD, ELE9_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE10_T, TOU_THRESH);
  set_register(MPR_ADD, ELE10_R, REL_THRESH);
  
  set_register(MPR_ADD, ELE11_T, TOU_THRESH);
  set_register(MPR_ADD, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(MPR_ADD, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(MPR_ADD, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(MPR_ADD, ATO_CFG0, 0x0B);
  set_register(MPR_ADD, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(MPR_ADD, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(MPR_ADD, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(MPR_ADD, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
 
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
    
}
