	#ifndef SensorHandlers_h
	#define SensorHandlers_h
	#include <Arduino.h>
	#include <WiFiUDP.h>

// base sensor class
class TouchSensorBase
{
	public: 
		TouchSensorBase(int hwpin, const char *msg);
		virtual bool pollSensor() = 0;
		// if a multitouch event ocurred 
		// disallow new touches
		void resetSensor();
		
	protected:
		int 
			_pin,
			_lastSensorState,
			_lastTouchTime;
		// set flag true from resetSensor()
		// do not allow touch processing until 
		// prevState and curState change to LOW (i.e. no touch)
		bool _allowInput;
		const char *_message;
		bool isDown(int state);
		// monitor reset state
		// prevent sensor checks until state has cleared
		bool allowInputProcessing();

};

// report both touch down and up with no logic processing
// intended to be used with MultiSensorAction
// does not send message when triggered
class WhileDownSensor : public TouchSensorBase
{
	public: 
		WhileDownSensor(int hwpin) : TouchSensorBase(hwpin, NULL)
		{
			_pin = hwpin;
		};
		virtual bool pollSensor();
};

// will automatically send message on touch
class OnTouchSensor : public TouchSensorBase
{
	public: 
		using TouchSensorBase::TouchSensorBase;
		virtual bool pollSensor();

	private:
		// time in milliseconds
		const int 
			_debounceTime = 400;
};


// will automatically send message on release
class OnReleaseSensor : public TouchSensorBase
{
	public: 
		using TouchSensorBase::TouchSensorBase;
		virtual bool pollSensor();

	private:
		// time in milliseconds
		const int 
			_debounceTime = 400;
};

// will automatically send message on long press
class LongpressSensor : public TouchSensorBase
{
	public: 
		using TouchSensorBase::TouchSensorBase;
		virtual bool pollSensor();

	private:
		// time in milliseconds
		bool _isTouchDown;
		const int _holdLength = 800;
};

// // will automatically send message on doubletap
class DoubletapSensor : public TouchSensorBase
{
	public: 
		using TouchSensorBase::TouchSensorBase;
		virtual bool pollSensor();

	private:
		bool _hasReceivedFirstTouch;
		// time in milliseconds
		const int _tapInterval = 500;
};

// provide static reference to sendMessage (delegate pointing to udphandler.h)
class SensorAction
{
	public:
		static void (*sendMessage)(const char *);
};


// handle multiple 
class MultiSensor
{
	public:
		MultiSensor(std::vector<WhileDownSensor> sensors, const char *msg);
		bool pollSensors();
	private:
		std::vector<WhileDownSensor> _sensors;
		const char *_message;
		bool _hasSentMessage;
};


#endif