#include <Arduino.h>
#include <WiFiUDP.h>
#include "sensorhandlers.h"

// static function pointer 
// assign UDP send method from udphandler.h
void(*SensorAction::sendMessage)(const char * msg){};

// listen for multiple sensors down simultaneously 
// requires WhileDownSensors to be defined and passed in
MultiSensor::MultiSensor(std::vector<WhileDownSensor> sensors, const char *msg)
{
	_sensors = sensors;
	_message = msg;
};

TouchSensorBase::TouchSensorBase(int hwpin, const char *msg)
{
  pinMode(hwpin, INPUT);
  _pin = hwpin;
  _lastTouchTime = millis();
	_message = msg;
	_allowInput = true;
};

bool
MultiSensor::pollSensors()
{
	// if more than one sensor is down, return true
	// prevent processing any other sensors while waiting for multisensor input
	uint8 sensorDownCount = 0;

	for (unsigned int i = 0; i < _sensors.size(); i++)
	{
		if(!_sensors[i].pollSensor())
		{
			_hasSentMessage = false;
		}
		else
		{
			sensorDownCount++;
		}
	}
	if(sensorDownCount == _sensors.size() && !_hasSentMessage)
	{
		SensorAction::sendMessage(_message);
		_hasSentMessage= true;
	}

	// return true if more than one sensor is down
	// prevent processing other types of touches 
	// if multitouch is partially engaged
	return sensorDownCount > 1;
};


void
TouchSensorBase::resetSensor()
{
	_allowInput = false;
}

bool
TouchSensorBase::isTouched(int state)
{
	return state == HIGH;
};

bool 
TouchSensorBase::allowInputProcessing()
{
	// if alrady reset, exit
	if(_allowInput) return true;
	int state = digitalRead(_pin);
	// false if sensor is currently being touched
	if(isTouched(state))
	{
		_allowInput = false;
	} 
	// if both lastState and curstate are up, clear reset and exit
	else if(_lastSensorState == state) 
	{
		_allowInput = true;
	}

	_lastSensorState = state;
	return _allowInput;
}

bool 
WhileDownSensor::pollSensor()
{
	// prevent touch processing if reset is not cleared
	if(!allowInputProcessing()) return false;

	int state = digitalRead(_pin);
	_lastSensorState = state;
	return isTouched(state);
};

bool
OnTouchSensor::pollSensor()
{
	if(!allowInputProcessing()) 
	{
		_lastTouchTime = millis();
		return false;	
	}

	int state = digitalRead(_pin);
	bool hasBeentouched = false;
	bool allowTouch = abs(millis() - _lastTouchTime) > _debounceTime;
	if(_lastSensorState != state && allowTouch)
	{
		// listen for touch up (release) rather than touch down
		if(isTouched(state))
		{
			hasBeentouched = true;
			_lastTouchTime = millis();
			SensorAction::sendMessage(_message);
		}
		_lastSensorState = state;
	}
	return hasBeentouched;
};

bool
OnReleaseSensor::pollSensor()
{
	if(!allowInputProcessing())
	{
		_lastTouchTime = millis();
		return false;
	} 

	int state = digitalRead(_pin);
	bool hasBeentouched = false;
	bool allowTouch = abs(millis() - _lastTouchTime) > _debounceTime;
	if(_lastSensorState != state && allowTouch)
	{
		// listen for touch up (release) rather than touch down
		if(!isTouched(state))
		{
			hasBeentouched = true;
			_lastTouchTime = millis();
			SensorAction::sendMessage(_message);
		}
		_lastSensorState = state;
	}
	return hasBeentouched;
};

bool 
LongpressSensor::pollSensor()
{
	if(!allowInputProcessing()) 
	{
		_isTouchDown = false;
		return false;
	}

	int state = digitalRead(_pin);
	bool hasBeentouched = false;

	if(_lastSensorState != state)
	{
		if(isTouched(state))
		{
			_isTouchDown = true;
			_lastTouchTime = millis();
		}
		else
		{
			_isTouchDown = false;
		}
		_lastSensorState = state;
	}

	if(_isTouchDown && abs(millis() - _lastTouchTime) > _holdLength)
	{
		// reset
		_isTouchDown = false;
		hasBeentouched = true;
		SensorAction::sendMessage(_message);
	}
	return hasBeentouched;
};

// multitapsensor will process touches on release
// multitapsensor can handle up to 255 taps
// idk why you'd want that... but you can
bool 
MultitapSensor::pollSensor()
{
	// use _allowInput as reset
	if(!_allowInput)
	{
		_lastSensorState = LOW;
		_lastTouchTime = millis();
		_tapCount = 0;
		_allowInput = true;
		return false;
	}

	if(_tapCount > 0 && abs(millis() - _lastTouchTime) > _tapWindow)
	{
		bool wasTriggered = false;

		// only execute if tapCount is within range of messages defined
		if(_tapCount - 1 < _tapMessages.size())
		{
			const char * msg = _tapMessages[_tapCount - 1];
			SensorAction::sendMessage(msg);
			wasTriggered = true;
		}

		resetSensor();
		return wasTriggered;
	}

	int state = digitalRead(_pin);

	if(state == _lastSensorState)
	{
		return false;
	}
	_lastSensorState = state;

	if(!isTouched(state))
	{
		_lastTouchTime = millis();
		_tapCount ++;
		// Serial.print("sensor tap count: ");
		// Serial.println(_tapCount);
	} 
	// return true in order to cancel other multitap sensor processors
	return true;
};
