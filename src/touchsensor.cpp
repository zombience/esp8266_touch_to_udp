#include <Arduino.h>
#include <WiFiUDP.h>
#include "touchsensor.h"

// static function pointer 
// assign UDP send method from udphandler.h
void(*TouchSensor::sendMessage)(const char * msg){};

TouchSensor::TouchSensor(int pin, const char * msg)
{
  pinMode(pin, INPUT);
  _pin = pin;
  //Serial.print("initializing touch with message: ");
  //Serial.println(msg);
  message = msg;
  _lastTouchTime = millis();
}

bool
TouchSensor::pollTouch()
{
  int state = digitalRead(_pin);
  bool isTouched = false;
  bool allowTouch = abs(millis() - _lastTouchTime) > _debounceTime;
	if(_lastSensorState != state && allowTouch)
  {
    // listen for touch up (release) rather than touch down
    if(state == LOW)
    {
      isTouched = true;
      _lastTouchTime = millis();  

      // send stored UDP message via udphandler.h
      sendMessage(message);
    }
  _lastSensorState = state;
  }
  return isTouched;
};

bool 
LongpressTouchSensor::pollTouch()
{
  int state = digitalRead(_pin);
  bool isTouched = false;

  if(_lastSensorState != state)
  {
    if(state == HIGH)
    {
      _isTouchDown = true;
      _touchStartTime = millis();
    }
    else
    {
      _isTouchDown = false;
    }
      _lastSensorState = state;
    }
  
  if(_isTouchDown && abs(millis() - _touchStartTime) > _holdLength)
  {
    // reset
    _isTouchDown = false;
    isTouched = true;
    sendMessage(message);
  }
  return isTouched;
};

bool 
DoubletapTouchSensor::pollTouch()
{
  int state = digitalRead(_pin);
  bool isTouched = false;
  if(abs(millis() - _touchStartTime) > _tapInterval)
  {
    _hasReceivedFirstTouch = false;
  }
  if(_lastSensorState != state)
  {
    if(state == HIGH)
    {
      if(_hasReceivedFirstTouch && abs(millis() - _touchStartTime) < _tapInterval)
      {
        sendMessage(message);
        isTouched = true;
        _hasReceivedFirstTouch = false;
      }
      else if(!_hasReceivedFirstTouch)
      {
        _hasReceivedFirstTouch = true;
        _touchStartTime = millis();
      }
    }
  _lastSensorState = state;
  }
  return isTouched;
}