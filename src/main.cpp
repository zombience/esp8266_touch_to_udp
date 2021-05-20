#include <Arduino.h>
#include <WiFiUDP.h>

#include "touchsensor.h"
#include "udphandler.h"

/* pin settings */
#define INPUTPIN 5

// multiple touch handlers can listen to the same pin
TouchSensor *instantSensor;
LongpressTouchSensor *longPressSensor;
DoubletapTouchSensor *doubleTapSensor;

void setup() 
{
  
  // initialize inside udphandler.h
  initializeUDP();
  // assign delegate to TouchSensor class for UDP send access
  TouchSensor::sendMessage = &sendMessage;
  instantSensor = new TouchSensor(INPUTPIN, "instant press");
  longPressSensor = new LongpressTouchSensor(INPUTPIN, "long press");
  doubleTapSensor = new DoubletapTouchSensor(INPUTPIN, "double tap");
}

void loop() 
{
  instantSensor->pollTouch();
  longPressSensor->pollTouch();
  doubleTapSensor->pollTouch();

  // defined in udphandler.h
  #ifdef LISTENMODE
  packetListen();
  #endif // LISTENMODE
}

