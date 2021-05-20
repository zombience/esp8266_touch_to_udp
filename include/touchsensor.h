#ifndef TouchSensor_h
#define TouchSensor_h
#include <Arduino.h>
#include <WiFiUDP.h>

// simple touch handler: 
// sends callback via *sendMessage function pointer
// when touchconditions are triggered
class TouchSensor 
{
  public: 
    TouchSensor(int pin, const char *msg);

    static void (*sendMessage)(const char *);
    // returns whether sensor received a touch
    // on this frame
    const char *message;
    virtual bool pollTouch();

  protected:
    int 
      _pin,
      _lastSensorState;
  private:
    // time in milliseconds
    const int 
      _debounceTime = 400;
    int _lastTouchTime;
};

class LongpressTouchSensor : TouchSensor
{
  public: 
    using TouchSensor::TouchSensor;
    virtual bool pollTouch();
  
  private:
    // time in milliseconds
    bool _isTouchDown;
    const int _holdLength = 800;
    int _touchStartTime = 0;
};

class DoubletapTouchSensor : TouchSensor
{
  public: 
    using TouchSensor::TouchSensor;
    virtual bool pollTouch();
  
  private:
    bool _hasReceivedFirstTouch;
    // time in milliseconds
    const int _tapInterval = 500;
    int _touchStartTime = 0;
};

#endif