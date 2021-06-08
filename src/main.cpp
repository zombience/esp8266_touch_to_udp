#include <Arduino.h>
#include <WiFiUDP.h>

#include "sensorhandlers.h"
#include "udphandler.h"

/* pin settings */
#define SENSOR0 5
#define SENSOR1 4 
#define SENSOR2 14
//#define SENSOR2 16 

// multiple touch handlers can listen to the same pin
// std::vector<WhileDownSensor> multiTouchSensors {
// 	WhileDownSensor(SENSOR0),
// 	WhileDownSensor(SENSOR1),
// 	WhileDownSensor(SENSOR2),
// };

std::vector<TouchSensorBase*> singleTouchSensors{
	new OnTouchSensor(SENSOR0, "sensor zero touch down"),
	new OnReleaseSensor(SENSOR0, "sensor zero released"),
	new OnTouchSensor(SENSOR1, "sensor one touch down"),
	new OnReleaseSensor(SENSOR1, "sensor one released"),
	new OnTouchSensor(SENSOR2, "sensor two touch down"),
	new OnReleaseSensor(SENSOR2, "sensor two released"),
	// new LongpressSensor(SENSOR2, "sensor two long press"),
	// new DoubletapSensor(SENSOR2, "sensor two double tap"),
};

MultiSensor *allSensors;


void setup() 
{
  
	// initialize inside udphandler.h
	initializeUDP();
	// assign delegate to TouchSensor class for UDP send access
	SensorAction::sendMessage = &sendMessage;
	allSensors = new MultiSensor(multiTouchSensors, "all sensors down");
	Serial.begin(115200);
	Serial.println("touch sensor test ready");	
}

void loop() 
{

	for (unsigned int i = 0; i < singleTouchSensors.size(); i++)
	{
		singleTouchSensors[i]->pollSensor();
	}
}

