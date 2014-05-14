#define pinEnable 8 // Activation du driver/pilote
#define pinStep    2 // Signal de PAS (avancement)
#define pinDir     3 // Direction 

#define ledPin     13

boolean isActive;
boolean motorDirection = true;
int motorDelay = 2;
int serialCommand;

void setup(){
  Serial.begin(9600);
  
  pinMode( pinEnable, OUTPUT );
  pinMode( pinDir   , OUTPUT );
  pinMode( pinStep  , OUTPUT );
  pinMode( ledPin  , OUTPUT );
  
  isActive = false;
}

void loop()
{
  digitalWrite(pinEnable, LOW);
  digitalWrite(pinDir, motorDirection);
  
  if (Serial.available() > 0)
  {
    char incomingChar = Serial.read();
    serialCommand = incomingChar;
    
    //Serial.print(serialCommand);
    
    if ( serialCommand < 48)
    {
      motorDelay = serialCommand;
      isActive = false;
      //Serial.println(" / V");
      return;
    }
    
    switch (serialCommand)
    {
      case 48:
        isActive = false;
        //Serial.println(" off");
        break;
      
      case 49:
        isActive = true;
        Serial.println(" on");
        break;
        
      case 50:
        motorDirection = true;
        digitalWrite( pinDir   , motorDirection);
        Serial.println(" / =>");
        break;
        
      case 51:
        motorDirection = false;
        digitalWrite( pinDir   , motorDirection);
        Serial.println(" / <=");
        break;
    }
  }
  
  if (isActive)
  {
    digitalWrite( pinStep, HIGH );
    digitalWrite( ledPin   , HIGH);
    
    //delay( motorDelay );
    
    digitalWrite( pinStep  , LOW);
    digitalWrite( ledPin   , LOW);
    
    delay( motorDelay );
    
  } else
  {
    
    digitalWrite( pinStep, HIGH );
    digitalWrite( ledPin   , HIGH);
    
    delay( motorDelay );
  }
}

