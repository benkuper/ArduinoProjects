//////////////////////
// SETUP /////////////
//////////////////////
void setup() 
{
  Serial.begin(9600);
  
  initMakey();
  initializeInputs();
  danceLeds();
}

////////////////////
// MAIN LOOP ///////
////////////////////
void loop() 
{
  updateMakey();
  
  /*
  sendMouseButtonEvents();
  sendMouseMovementEvents();
  */
  cycleLEDs();
  updateOutLEDs();
  addDelay();
  
  
  //Serial echo for write test
  while(Serial.available() > 0)
  {
    Serial.write(Serial.read());
  }
}




///////////////////////////
// ADD DELAY
///////////////////////////
void addDelay() {

  loopTime = micros() - prevTime;
  if (loopTime < TARGET_LOOP_TIME) {
    int wait = TARGET_LOOP_TIME - loopTime;
    delayMicroseconds(wait);
  }
  
  prevTime = micros();
}

void sendTouchInfo(int pin,int val)
{
   Serial.write('p');
  Serial.write(pin);
  Serial.write(val);
  Serial.write(255);
}

