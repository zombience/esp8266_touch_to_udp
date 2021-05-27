#include <Arduino.h>
#include <WiFiUDP.h>

#include "sensorhandlers.h"
#include "udphandler.h"

/* pin settings */
#define SENSOR0 5
#define SENSOR1 4 
#define SENSOR2 14 

// multiple touch handlers can listen to the same pin
std::vector<WhileDownSensor> multiTouchSensors {
	WhileDownSensor(SENSOR0),
	WhileDownSensor(SENSOR1),
	WhileDownSensor(SENSOR2),
};

std::vector<TouchSensorBase*> singleTouchSensors{
	new OnTouchSensor(SENSOR1, "sensor one touch down"),
	new OnReleaseSensor(SENSOR1, "sensor one released"),
	new LongpressSensor(SENSOR2, "sensor two long press"),
	new DoubletapSensor(SENSOR2, "sensor two double tap"),
};

MultiSensor *allSensors;


void setup() 
{
  
	// initialize inside udphandler.h
	initializeUDP();
	// assign delegate to TouchSensor class for UDP send access
	SensorAction::sendMessage = &sendMessage;
	
	allSensors = new MultiSensor(multiTouchSensors, "all sensors down");
	
}

void loop() 
{
	if(allSensors->pollSensors())
	{
		for (unsigned int i = 0; i < singleTouchSensors.size(); i++)
		{
			singleTouchSensors[i]->resetSensor();
		}
		return;
	}

	for (unsigned int i = 0; i < singleTouchSensors.size(); i++)
	{
		singleTouchSensors[i]->pollSensor();
	}

	// defined in udphandler.h
	#ifdef LISTENMODE
	packetListen();
	#endif // LISTENMODE
}

