void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(12,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool plugged = digitalRead(12);
  Serial.print(plugged);
  if(plugged)
  {
    Serial.print('\t');
    Serial.print(analogRead(A0));  
  }
  
  Serial.println("");
  
  delay(20);
}
