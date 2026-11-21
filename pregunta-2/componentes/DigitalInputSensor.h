/*
  DigitalInputSensor.h
  Digital input that fires ACTIVATED on the rising edge: PIR motion sensor,
  push button (use activeLow = true with INPUT_PULLUP), tilt or door sensor.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef DIGITAL_INPUT_SENSOR_H
#define DIGITAL_INPUT_SENSOR_H

#include "ModestIoT.h"

class DigitalInputSensor : public Sensor {
private:
  bool activeLow;
  bool previousActive;

protected:
  void processEvent(Event& event) override;

public:
  static const int ACTIVATED_EVENT_IDENTIFIER = 30;

  DigitalInputSensor(int signalPin, bool activeLow = false, EventHandler* parentHandler = nullptr);
  bool isActive() const;
};

#endif
