#ifndef UDP_h
#define UDP_h

#define PRINT
// listen for responses
//#define LISTENMODE

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

/* 
  externalize wifi name & password 
  if this file does not exist, create file: 
  include/wifi_info.h
  with contents: 
  #define SSID "yourssidname"
  #define WIFI_PW "yourpassword"
  const char * HOST_IP = "xxx.xxx.xxx.xxx"; // target ip address
  const int HOST_PORT = 0000; // target port
*/
#include "wifi_info.h"

WiFiUDP UDP;

void initializeUDP()
{
  Serial.begin(115200);
  Serial.print("connecting to");
  Serial.println(SSID);
  WiFi.begin(SSID, WIFI_PW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. received IP assignment: " );
  Serial.println(WiFi.localIP());

  #ifdef LISTENMODE
  Serial.print("listening on port: ");
  Serial.println(LISTEN_PORT);
  UDP.begin(LISTEN_PORT);
  #endif // LISTENMODE

  UDP.beginPacket(HOST_IP, HOST_PORT);
  UDP.write("esp connected");
  UDP.endPacket();
  
};

// assign function pointer to touchsensor static method 
void sendMessage(const char * msg)
{
	#ifdef PRINT
	Serial.print("sending msg: ");
	Serial.println(msg);
	#endif
  UDP.beginPacket(HOST_IP, HOST_PORT);
  UDP.write(msg);
  UDP.endPacket();
};


#ifdef LISTENMODE
// listen for heartbeats
char packet [255];
void packetListen()
{
  int packetSize = UDP.parsePacket();
  if(packetSize) 
  {
    int len = UDP.read(packet, 255);
    if(len > 0)
    {
      packet[len] = '\0';
    }
    Serial.print("received message from IP: ");
    Serial.print(UDP.remoteIP());
    Serial.print(" port: ");
    Serial.print(UDP.remotePort());
    Serial.print(" ");
    Serial.println(packet);

    UDP.beginPacket(HOST_IP, HOST_PORT);
    UDP.write("received heartbeat");
    UDP.endPacket();
  }
};
#endif // LISTENMODE 
#endif