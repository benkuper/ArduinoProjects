byte buffer[32];
int bufferIndex = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  processSerial();
}

void processSerial()
{
  while(Serial.available() > 0)
  {
    digitalWrite(13,HIGH);
    delay(5);
    digitalWrite(13,LOW);
    
    byte b = Serial.read();
    if(b == 255) processBuffer();
    else if(bufferIndex < 31) buffer[bufferIndex++] = b;
  }
}

void processBuffer()
{
  digitalWrite(13,HIGH);
  Serial.print("Process Buffer :");
  Serial.print(buffer[0]);
  Serial.println("");
  
  switch(buffer[0])
  {
     case 'c':
     //command c here
     break;
     
     default:
     break;
  }
  
  bufferIndex = 0;
  digitalWrite(13,LOW);
}
