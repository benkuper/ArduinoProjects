#include "MPR121.h"
#include <Wire.h>

#define mpr1 0x5A
#define mpr2 0x5D
#define mpr1Index 0
#define mpr2Index 12

int irqpin = 2;  // Digital 2

int touchThresholds[24];
int releaseThresholds[24];

byte buffer[32];
int bufferIndex = 0;


boolean touchStates[12*2]; //Two MPR, 12 electrodes each //to keep track of the previous touch states

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  
  Serial.begin(9600);
  Wire.begin();
  
  for(int i=0;i<24;i++)
  {
    touchThresholds[i] = 0x0A;
    releaseThresholds[i] = 0x06;
  }
  
  Serial.println("MPR setup...");
  mpr121_setup(mpr1);
  mpr121_setup(mpr2);
  Serial.println("MPR setup done.");
}

void loop(){
  processSerial(); 
  
  readTouchInputs(mpr1,mpr1Index);
  readTouchInputs(mpr2,mpr2Index);
}

void processSerial()
{
  while(Serial.available() > 0)
  {
    byte b = Serial.read();
    if(b == 255) 
    {
      processBuffer();
      bufferIndex = 0;
    }else if(bufferIndex < 31)
    {
      buffer[bufferIndex] = b;
      bufferIndex++;
    }
  }
}

void processBuffer()
{
  digitalWrite(13,HIGH);
  /*
  Serial.print("Process Buffer :");
  Serial.print(buffer[0]);
  Serial.println("");
  */
  
  switch(buffer[0])
  {
     case 't':
       setTouchThreshold(buffer[1],buffer[2]);
     break;
     
      case 'r':
       setReleaseThreshold(buffer[1],buffer[2]);
     break;
     
     
     default:
     break;
  }
  
  digitalWrite(13,LOW);
}


void setTouchThreshold(int pin,int value)
{
  /*
  Serial.print("Set touch treshold [");
  Serial.print(pin);
  Serial.print(" ] -> ");
  Serial.println(value);
  */
  
  touchThresholds[pin] = value;
  
  mpr121_setup(pin < 12?mpr1:mpr2);
  
  Serial.write('t');
  Serial.write(pin);
  Serial.write(touchThresholds[pin]);
  Serial.write(255);
  
  //updateThresholds(pin < 12?mpr1:mpr2);
}

void setReleaseThreshold(int pin, int value)
{
  /*
  Serial.print("Set release treshold [");
  Serial.print(pin);
  Serial.print(" ] -> ");
  Serial.println(value);
  */
  releaseThresholds[pin] = value;
  mpr121_setup(pin < 12?mpr1:mpr2);
  
  Serial.write('r');
  Serial.write(pin);
  Serial.write(releaseThresholds[pin]);
  Serial.write(255);
  
  //updateThresholds(pin < 12?mpr1:mpr2);
}

void readTouchInputs(int address,int startIndex){
  //Serial.println("read Touch inputs !");
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(address,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      int arrI = i+startIndex;
      
      int pin = i+((address == mpr2)?12:0);
      
      if(touched & (1<<i)){
      
        
        if(touchStates[arrI] == 0){
          //pin i was just touched
          /*
          Serial.print("[");
          Serial.print(address,HEX);
          Serial.print("] ");
          Serial.print("pin ");
          Serial.print(pin);
          Serial.print(" was just touched");
          Serial.print(" (threshold = ");
          Serial.print(touchThresholds[pin]);
          Serial.println(")");
          */
          sendTouchInfo(pin,1);
          
        }else if(touchStates[arrI] == 1){
          //pin i is still being touched
          
        }  
      
        touchStates[arrI] = 1;      
      }else{
        if(touchStates[arrI] == 1){
          /*
          Serial.print("[");
          Serial.print(address,HEX);
          Serial.print("] ");
          Serial.print("pin ");
          Serial.print(pin);
          Serial.print(" is no longer being touched");
          Serial.print(" (threshold = ");
          Serial.print(releaseThresholds[pin]);
          Serial.println(")");
          */
          
          //pin i is no longer being touched
          
          sendTouchInfo(pin,0);
         
       }
        
        touchStates[arrI] = 0;
      }
    
    }
    
  }
}


void sendTouchInfo(int pin,int val)
{
   Serial.write('p');
  Serial.write(pin);
  Serial.write(val);
  Serial.write(255);
}

void mpr121_setup(int address){

  set_register(address, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(address, MHD_R, 0x01);
  set_register(address, NHD_R, 0x01);
  set_register(address, NCL_R, 0x00);
  set_register(address, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(address, MHD_F, 0x01);
  set_register(address, NHD_F, 0x01);
  set_register(address, NCL_F, 0xFF);
  set_register(address, FDL_F, 0x02);
  
  updateThresholds(address);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(address, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(address, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  
  /*
  set_register(address, ATO_CFG0, 0x0B);
  set_register(address, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(address, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(address, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  
*/
  set_register(address, ELE_CFG, 0x0C);
  
}


void updateThresholds(int address)
{
  int index = 0;
  if(address == 0x5D) index = 12;
  
  //Serial.print("Update thresholds, base index :");
  //Serial.println(index);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(address, ELE0_T, touchThresholds[index+0]);
  set_register(address, ELE0_R, releaseThresholds[index+0]);
 
  set_register(address, ELE1_T, touchThresholds[index+1]);
  set_register(address, ELE1_R, releaseThresholds[index+1]);
  
  set_register(address, ELE2_T, touchThresholds[index+2]);
  set_register(address, ELE2_R, releaseThresholds[index+2]);
  
  set_register(address, ELE3_T, touchThresholds[index+3]);
  set_register(address, ELE3_R, releaseThresholds[index+3]);
  
  set_register(address, ELE4_T, touchThresholds[index+4]);
  set_register(address, ELE4_R, releaseThresholds[index+4]);
  
  set_register(address, ELE5_T, touchThresholds[index+5]);
  set_register(address, ELE5_R, releaseThresholds[index+5]);
  
  set_register(address, ELE6_T, touchThresholds[index+6]);
  set_register(address, ELE6_R, releaseThresholds[index+6]);
  
  set_register(address, ELE7_T, touchThresholds[index+7]);
  set_register(address, ELE7_R, releaseThresholds[index+7]);
  
  set_register(address, ELE8_T, touchThresholds[index+8]);
  set_register(address, ELE8_R, releaseThresholds[index+8]);
  
  set_register(address, ELE9_T, touchThresholds[index+9]);
  set_register(address, ELE9_R, releaseThresholds[index+9]);
  
  set_register(address, ELE10_T, touchThresholds[index+10]);
  set_register(address, ELE10_R, releaseThresholds[index+10]);
  
  set_register(address, ELE11_T, touchThresholds[index+11]);
  set_register(address, ELE11_R, releaseThresholds[index+11]);
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
