/*
  AnalogSensor.h
  Generic analog sensor (potentiometer, gas, light, simulated pulse, valve
  position...). Maps the ADC reading to a range in real units.
  ADC max: 4095 on ESP32, 1023 on Arduino Uno.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include "ModestIoT.h"

class AnalogSensor : public Sensor {
private:
  int adcMaximum;
  float minimumValue;
  float maximumValue;
  float changeTolerance;
  float value;

protected:
  void processEvent(Event& event) override;

public:
  AnalogSensor(int analogPin, int adcMaximum, float minimumValue, float maximumValue, float changeTolerance,
               EventHandler* parentHandler = nullptr);
  float getValue() const { return value; }
  bool hasReading() const { return value >= minimumValue; }
};

#endif
