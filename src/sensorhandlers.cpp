#include <Arduino.h>
#include <WiFiUDP.h>
#include "sensorhandlers.h"

// static function pointer 
// assign UDP send method from udphandler.h
void(*SensorAction::sendMessage)(const char * msg){};


MultiSensor::MultiSensor(std::vector<WhileDownSensor> sensors, const char *msg)
{
	_sensors = sensors;
	_message = msg;
};

bool
MultiSensor::pollSensors()
{
	bool allSensorsDown = true;
	for (unsigned int i = 0; i < _sensors.size(); i++)
	{
		if(!_sensors[i].pollSensor())
		{
			allSensorsDown = false;
			_hasSentMessage = false;
			break;
		}
	}
	if(allSensorsDown && !_hasSentMessage)
	{
		SensorAction::sendMessage(_message);
		_hasSentMessage= true;
	}
	return allSensorsDown;
};

TouchSensorBase::TouchSensorBase(int hwpin, const char *msg)
{
  pinMode(hwpin, INPUT);
  _pin = hwpin;
  _lastTouchTime = millis();
	_message = msg;
	_allowInput = true;
};

void
TouchSensorBase::resetSensor()
{
	_allowInput = false;
}

bool
TouchSensorBase::isDown(int state)
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
	if(isDown(state))
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
	return isDown(state);
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
	bool isTouched = false;
	bool allowTouch = abs(millis() - _lastTouchTime) > _debounceTime;
	if(_lastSensorState != state && allowTouch)
	{
		// listen for touch up (release) rather than touch down
		if(isDown(state))
		{
			isTouched = true;
			_lastTouchTime = millis();
			SensorAction::sendMessage(_message);
		}
		_lastSensorState = state;
	}
	return isTouched;
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
	bool isTouched = false;
	bool allowTouch = abs(millis() - _lastTouchTime) > _debounceTime;
	if(_lastSensorState != state && allowTouch)
	{
		// listen for touch up (release) rather than touch down
		if(!isDown(state))
		{
			isTouched = true;
			_lastTouchTime = millis();
			SensorAction::sendMessage(_message);
		}
		_lastSensorState = state;
	}
	return isTouched;
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
	bool isTouched = false;

	if(_lastSensorState != state)
	{
		if(isDown(state))
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
		isTouched = true;
		SensorAction::sendMessage(_message);
	}
	return isTouched;
};

bool 
DoubletapSensor::pollSensor()
{

	if(!allowInputProcessing()) 
	{
		_hasReceivedFirstTouch = false;
		return false;
	}

	int state = digitalRead(_pin);
	bool isTouched = false;
	if(abs(millis() - _lastTouchTime) > _tapInterval)
	{
		_hasReceivedFirstTouch = false;
	}
	if(_lastSensorState != state)
	{
		if(isDown(state))
		{
			if(_hasReceivedFirstTouch && abs(millis() - _lastTouchTime) < _tapInterval)
			{
				isTouched = true;
				_hasReceivedFirstTouch = false;
				SensorAction::sendMessage(_message);
			}
			else if(!_hasReceivedFirstTouch)
			{
				_hasReceivedFirstTouch = true;
				_lastTouchTime = millis();
			}
		}
		_lastSensorState = state;
	}
	return isTouched;
}