#ifndef UDP_h
#define UDP_h

#define PRINT
// use to test button feedback and ignore networking
//#define BUTTONTESTONLY

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
	const int LISTEN_PORT = 0000; // port that ESP listens for return messages
*/
#include "wifi_info.h"

WiFiUDP UDP;

void initializeUDP()
{
  Serial.begin(115200);
	#ifndef BUTTONTESTONLY
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

  UDP.begin(LISTEN_PORT);
  Serial.print("listening on port: ");
  Serial.println(LISTEN_PORT);

  UDP.beginPacket(HOST_IP, HOST_PORT);
  UDP.write("esp connected");
  UDP.endPacket();
  #endif
};

// assign function pointer to touchsensor static method 
void sendMessage(const char * msg)
{
	#ifdef PRINT
	Serial.print("sending msg: ");
	Serial.println(msg);
	#endif
	#ifndef BUTTONTESTONLY
  UDP.beginPacket(HOST_IP, HOST_PORT);
  UDP.write(msg);
  UDP.endPacket();
	#endif
};
#endif