/*
  DigitalInputSensor.cpp
  Edge detection: only the change from inactive to active is notified.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "DigitalInputSensor.h"

DigitalInputSensor::DigitalInputSensor(int signalPin, bool activeLow, EventHandler* parentHandler)
  : Sensor(signalPin, parentHandler), activeLow(activeLow), previousActive(false) {
  pinMode(pin, activeLow ? INPUT_PULLUP : INPUT);
}

bool DigitalInputSensor::isActive() const {
  return (digitalRead(pin) == HIGH) != activeLow;
}

void DigitalInputSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  bool active = isActive();
  bool activated = active && !previousActive;
  previousActive = active;
  event = activated ? Event(ACTIVATED_EVENT_IDENTIFIER, pin) : Event(SUPPRESSED_EVENT_IDENTIFIER);
}
