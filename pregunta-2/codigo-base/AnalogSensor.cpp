/*
  AnalogSensor.cpp
  value = minimum + reading * (maximum - minimum) / adcMaximum

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "AnalogSensor.h"

AnalogSensor::AnalogSensor(int analogPin, int adcMaximum, float minimumValue, float maximumValue,
                           float changeTolerance, EventHandler* parentHandler)
  : Sensor(analogPin, parentHandler), adcMaximum(adcMaximum), minimumValue(minimumValue),
    maximumValue(maximumValue), changeTolerance(changeTolerance), value(minimumValue - 1) {
  pinMode(pin, INPUT);
}

void AnalogSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  float reading = minimumValue + analogRead(pin) * (maximumValue - minimumValue) / adcMaximum;
  if (hasReading() && fabs(reading - value) < changeTolerance) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  value = reading;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
