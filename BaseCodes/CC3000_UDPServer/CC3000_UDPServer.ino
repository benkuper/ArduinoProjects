/*
CC3000udpServerTest.ino

Version 0.0.1
Last Modified 10/27/2013
By Jim Mayhugh

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

This software uses multiple libraries that are subject to additional
licenses as defined by the author of that software. It is the user's
and developer's responsibility to determine and adhere to any additional
requirements that may arise.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNE$$ FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


*********************/

/*

  This is a simple test of the UDP server software. whatever is sent to the server is
  echoed back to the client as a message.

*/

#include <Adafruit_CC3000.h>
#include "utility/socket.h"
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// Wireless UDP Stuff
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// Define CC3000 chip pins
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  4
#define ADAFRUIT_CC3000_CS    10
 
const unsigned long
  dhcpTimeout     = 60L * 1000L, // Max time to wait for address from DHCP
  connectTimeout  = 15L * 1000L, // Max time to wait for server connection
  responseTimeout = 15L * 100000L; // Max time to wait for data from server
uint32_t t;

// WiFi network (change with your settings !)
#define WLAN_SSID             "SFR_9BA8"        // cannot be longer than 32 characters!
#define WLAN_PASS             "YourPassword"
#define WLAN_SECURITY         WLAN_SEC_UNSEC // This can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
//#define  SOL_SOCKET           0xffff
//#define  SOCKOPT_RECV_TIMEOUT 1 
// Create CC3000 instances
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);                                
Adafruit_CC3000_Server server;
                                         
unsigned int localPort = 2652;      // local port to listen on

// buffers for receiving and sending data
const uint16_t UDP_TX_PACKET_MAX_SIZE = 101;
const uint16_t UDP_RX_PACKET_MAX_SIZE = 101;

char PacketBuffer[UDP_RX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[1024];  // a string to send back

char PartialBuffer[UDP_TX_PACKET_MAX_SIZE]; // Partial Buffer for oversized messages

uint16_t baudRate = 115200;

void setup(void)
{
  Serial.begin(baudRate);
  delay(5000);

  Serial.print(F("Serial Debug running at "));
  Serial.print(baudRate);
  Serial.println(F(" baud"));

  Serial.println(F("Hello, CC3000!\n")); 

  displayDriverMode();
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  delay(1000);

  Serial.println(F("Initializing CC3000 nic"));

  // Initialise the CC3000 module
  if (!cc3000.begin())
  {
    while(1);
  } 

// Per http://e2e.ti.com/support/low_power_rf/f/851/t/292664.aspx
// aucInactivity needs to be set to 0 (never timeout) or the socket will close after
// 60 seconds of no activity
  unsigned long aucDHCP       = 14400;
  unsigned long aucARP        = 3600;
  unsigned long aucKeepalive  = 30;
  unsigned long aucInactivity = 0;
  if(aucInactivity == 0)
  {
    Serial.println(F("Setting netapp to not timeout..."));
  }else{
    Serial.print(F("Setting netapp to timeout in "));
    Serial.print(aucInactivity);
    Serial.println(F(" Seconds"));
  }
  long iRet = netapp_timeout_values(&aucDHCP, &aucARP, &aucKeepalive, &aucInactivity);
  if (iRet!=0)
  {
    Serial.print(F("Could not set netapp option, iRet = "));
    Serial.println(iRet);
    Serial.println(F(", aborting..."));
    while(1);
  }else{
    Serial.print(F("set netapp option, iRet = "));
    Serial.println(iRet);
  }

  displayMACAddress();
  
  // Connect to  WiFi network
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println("Connected to WiFi network!");


  // Check DHCP
  Serial.print(F("Requesting address from DHCP server..."));
  for(t=millis(); !cc3000.checkDHCP() && ((millis() - t) < dhcpTimeout); delay(1000));
  if(cc3000.checkDHCP()) {
    Serial.println(F("OK"));
    displayConnectionDetails();
  } else {
    Serial.println(F("failed"));
    return;
  }

  
  for(int q = 0; q < UDP_TX_PACKET_MAX_SIZE; q++) // clear the buffer udp buffers
  {
    ReplyBuffer[q] = 0x00;
    PacketBuffer[q] = 0x00;
  }
  unsigned long startTime;

  Serial.println(F("Attempting connection..."));
  
  startTime = millis();
    
    do
    {
      server = cc3000.connectAndBindUDP(0x0, localPort);
    } while((!server.connected()) &&
            ((millis() - startTime) < connectTimeout));

    if(server.connected())
    {
      Serial.println(F("connected!"));
//      server.println("@");
    }else{
      Serial.println(F("NOT connected!"));
      while(1);
    }
}

void loop(void)
{
  uint16_t rpBufferCnt = 0, replyCnt = 0;
  uint8_t c;
  if(!server.connected())
  {
      Serial.println(F("server not connected"));
  }
  
  while(server.available() > 0);
  {
     c = server.read();
     Serial.println(c);
  
  }

  
  /*
  replyCnt += sprintf(ReplyBuffer, "You sent %s, so I'm sending %s back.\n", (char *) PacketBuffer, (char *) PacketBuffer);
  
  Serial.println(ReplyBuffer);
  
  server.write(ReplyBuffer, replyCnt);

  for(int q = 0; q < UDP_TX_PACKET_MAX_SIZE; q++) // clear the buffer udp buffers
  {
    ReplyBuffer[q] = 0x00;
    PacketBuffer[q] = 0x00;
  }
  */
}

/**************************************************************************/
/*!
    @brief  Displays the driver mode (tiny of normal), and the buffer
            size if tiny mode is not being used

    @note   The buffer size and driver mode are defined in cc3000_common.h
*/
/**************************************************************************/
void displayDriverMode(void)
{
  #ifdef CC3000_TINY_DRIVER
    Serial.println(F("CC3000 is configure in 'Tiny' mode"));
  #else
    Serial.print(F("RX Buffer : "));
    Serial.print(CC3000_RX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
    Serial.print(F("TX Buffer : "));
    Serial.print(CC3000_TX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
  #endif
}

/**************************************************************************/
/*!
    @brief  Tries to read the CC3000's internal firmware patch ID
*/
/**************************************************************************/
uint16_t checkFirmwareVersion(void)
{
  uint8_t major, minor;
  uint16_t version;
  
#ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor))
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = major; version <<= 8; version |= minor;
  }
#endif
  return version;
}

/**************************************************************************/
/*!
    @brief  Tries to read the 6-byte MAC address of the CC3000 module
*/
/**************************************************************************/
void displayMACAddress(void)
{
  uint8_t macAddress[6];
  
  if(!cc3000.getMacAddress(macAddress))
  {
    Serial.println(F("Unable to retrieve MAC Address!\r\n"));
  }
  else
  {
    Serial.print(F("MAC Address : "));
    cc3000.printHex((byte*)&macAddress, 6);
  }
}


/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/

void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}
