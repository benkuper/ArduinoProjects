
import processing.net.*;

Client c;

int[] buffer;
int maxBuffer = 3;
int bufferIndex = 0;

void setup() 
{
  size(450, 255);
  
  buffer = new int[maxBuffer];
  
  frameRate(30);
  c = new Client(this, "192.168.1.22", 5678); // Replace with your server's IP and port
}


void draw() 
{
  try
  {
    c.write(byte(mouseX*254/width));
  }catch(Exception e)
  {
    println("Connection closed");
  }
 while(c.available() > 0)
 {
   
   int b = c.read();
   //print(b);
   switch(b)
   {
     case 255:
       processBuffer();
       bufferIndex = 0;
       break;
     
     default:
       
       if(bufferIndex < maxBuffer) buffer[bufferIndex] = b;
       bufferIndex++;
       break;
   }
   
   
 }
}

void processBuffer()
{
  background(0);
  fill(255,0,0);
  rect(0,0,buffer[0],30);
  fill(0,255,0);
  rect(0,40,buffer[1],30);
  fill(0,0,255);
  rect(0,80,buffer[2],30);
}
