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
std::vector<WhileDownSensor> multiTouchSensors {
	WhileDownSensor(SENSOR0),
	WhileDownSensor(SENSOR1),
	WhileDownSensor(SENSOR2),
};

// create multisensor container to handle multiple WhileDownSensors
MultiSensor *allSensors;

std::vector<TouchSensorBase*> singleTouchSensors{
	new OnTouchSensor(SENSOR0, "sensor zero touch down"),
	new OnReleaseSensor(SENSOR0, "sensor zero released"),
	new LongpressSensor(SENSOR0, "sensor zero long press"),
	new OnTouchSensor(SENSOR1, "sensor one touch down"),
	new OnReleaseSensor(SENSOR1, "sensor one released"),
	new LongpressSensor(SENSOR1, "sensor one long press"),
	new OnTouchSensor(SENSOR2, "sensor two touch down"),
	new OnReleaseSensor(SENSOR2, "sensor two released"),
	new LongpressSensor(SENSOR2, "sensor two long press"),
};


std::vector<const char*> s0Messages{
	"s0_single_tap", 
	"s0_double_tap", 
	"s0_triple_tap"
};

std::vector<const char*> s1Messages{
	"s1_single_tap", 
	"s1_double_tap", 
	"s1_triple_tap"
};

std::vector<const char*> s2Messages{
	"s2_single_tap", 
	"s2_double_tap", 
	"s2_triple_tap"
};

std::vector<TouchSensorBase*> multiTapSensors{
	new MultitapSensor(SENSOR0, s0Messages),
	new MultitapSensor(SENSOR1, s1Messages),
	new MultitapSensor(SENSOR2, s2Messages),
};

void setup() 
{
	// initialize inside udphandler.h
	initializeUDP();

	// assign delegate to SensorAction class for UDP send access
	SensorAction::sendMessage = &sendMessage;

	allSensors = new MultiSensor(multiTouchSensors, "all sensors down");
	Serial.begin(115200);
	Serial.println("touch sensor test ready");	
}

void loop() 
{

	// will be true if more than one sensor is simultaneously touched
	bool isMultitouchEngaged = allSensors->pollSensors();

	// stop processing other touch types 
	// and reset other touch processors
	if(isMultitouchEngaged)
	{
		for(unsigned int i = 0; i < multiTapSensors.size(); i++)
			multiTapSensors[i]->resetSensor();
		
		for (unsigned int i = 0; i < singleTouchSensors.size(); i++)
			singleTouchSensors[i]->resetSensor();

		return;
	}

	
	int tappedIndex = -1;
	for(unsigned int i = 0; i < multiTapSensors.size(); i++)
	{
		if(multiTapSensors[i]->pollSensor())
		{
			tappedIndex = i;
			break;
		}
	}
	
	if(tappedIndex >= 0)
	{
		for(unsigned int i = 0; i < multiTapSensors.size(); i++)
		{
			if(i == tappedIndex) continue;
			multiTapSensors[i]->resetSensor();
		}
	}

	
	// using multitap and singleTouch sensors together on the same sensor
	// will send extra touches. this is included purely for example purposes
	for (unsigned int i = 0; i < singleTouchSensors.size(); i++)
	{
		singleTouchSensors[i]->pollSensor();
	}
}

