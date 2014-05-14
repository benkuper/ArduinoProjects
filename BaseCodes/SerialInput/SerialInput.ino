byte buffer[32];
int bufferIndex = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  processSerial();
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
  bufferIndex = -1;
  switch(buffer[0])
  {
     case 'c':
     //command c here
     break;
     
     default:
     break;
  }
}
