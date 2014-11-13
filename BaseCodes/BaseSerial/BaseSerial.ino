const int maxBuffer = 1024;
byte buffer[maxBuffer];
int bufferIndex = 0;

void setup()
{
  Serial.begin(460800);
  pinMode(13,OUTPUT);
}

void loop()
{
  processSerial();
}

void processSerial()
{
  if(Serial.available() > 0)
  {
    byte c = Serial.read();
    if(c == 255)
    {
      processBuffer();
      bufferIndex = 0;
      return;
    }
    if(bufferIndex < maxBuffer)
    {
      buffer[bufferIndex] = c;
      bufferIndex++;
    }
  }
}

void processBuffer()
{
  digitalWrite(13,HIGH);
  delay(10);
  //Serial.println(bufferIndex);
  digitalWrite(13,LOW);
}
