#include <YunServer.h>
#include <Bridge.h>
#include <YunClient.h>

YunServer server(5678);

void setup() {
  Serial.begin(9600);

  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  //Serial.begin(9600);
  
  server.noListenOnLocalhost();
  server.begin();
}


bool hasClient;
YunClient client;

void loop() {
  if(!hasClient)
  {
    client = server.accept();
    if(client) hasClient = true;
  }
  
  if(hasClient) processClient();
}


void sendToClient(byte * data, int numData)
{
    for(int i=0;i<numData;i++)
    {
      client.write(data[i]);
     }
     client.write(255); //end character
}

void processClient()
{
  while (client.available() > 0) {
    byte b = client.read();
    analogWrite(13,b);
    byte data[3] = {b, b/2 , b/3} ;
    sendToClient(data,3);   
  }
  
  if(!client.connected()) 
  {
    digitalWrite(13,LOW);
    hasClient = false;
  }
}



