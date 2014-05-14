#include <CapacitiveSensor.h>
#include <YunServer.h>
#include <Bridge.h>
#include <YunClient.h>

const int numCapas = 1;
int capaSenderPin = 2;
int capaReceiverStartPin = 3;
CapacitiveSensor capas[numCapas]; // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
byte capaVals[numCapas];
int maxCapaVal = 3000; //arbitrary

int plugPin = 12;
bool plugged = false;


YunServer server(5678);
YunClient client;
bool hasClient;

void setup()                    
{
   //network
   Bridge.begin();
   server.noListenOnLocalhost();
   server.begin();
   
  //Console
  // Console.begin();
  //Console.println("Setup !");
    //pins 
   pinMode(plugPin,INPUT_PULLUP);
   pinMode(13,OUTPUT);
   
   digitalWrite(13,HIGH);
   delay(500);
   //capa
   for(int i=0;i<numCapas;i++)
   {
     capas[i].setPins(capaSenderPin,capaReceiverStartPin+i);
     capas[i].set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   }
   
   digitalWrite(13,LOW);
   
   //Console.println("setup complete");
   
  

  
}

void loop()                    
{
    processServer();
   
    processPlug();
    
    //if(plugged)
    //{
      processCapa();
    //}
    
    delay(30); 
}

void processPlug()
{
  bool newPlugged = digitalRead(plugPin); 
    
  if(plugged != newPlugged)
  {
    plugged = newPlugged;
    //Console.println(plugged ? "Plugged":"Unplugged !");
    sendPlugChange();
  }
}

void processServer()
{
  if(!hasClient)
  {
    client = server.accept();
    if(client) 
    {
      hasClient = true;
      digitalWrite(13,HIGH);
    }
  }
  
  if(hasClient) processClient();
}

void processClient()
{
  while (client.available() > 0) {
    byte b = client.read();
  }
  
  if(!client.connected()) 
  {
    digitalWrite(13,LOW);
    hasClient = false;
  }
}

void sendPlugChange()
{
  if(!hasClient) return;
  
  client.write('p'); //plug change command
  client.write(plugged?0:1);
  client.write(255); //end message
  client.flush();
}


void processCapa()
{  
  // Console.print("Capa values :");
   for(int i=0;i<numCapas;i++)
   {
      capaVals[i] = min(map(capas[i].capacitiveSensor(30),0,maxCapaVal,0,254),254); // force not upper than 254
   //   Console.print(capaVals[i]);
   //   Console.print('\t');
   }
   
   //Console.println("");
   
   sendCapaValues();
}

void sendCapaValues()
{
  if(!hasClient) return;
  
  client.write('c'); //capa values command
  client.write(numCapas);
  for(int i=0;i<numCapas;i++)
  {
    client.write(capaVals[i]);  
  }
  
  client.write(255); //end message
  client.flush();
}
