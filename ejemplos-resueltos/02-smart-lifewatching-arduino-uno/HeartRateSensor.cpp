/*
  HeartRateSensor.cpp
  Converts the analog reading into PPM and notifies only when it changes.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#include "HeartRateSensor.h"

HeartRateSensor::HeartRateSensor(int analogPin, int minimumPulsesPerMinute, int maximumPulsesPerMinute,
                                 EventHandler* parentHandler)
  : Sensor(analogPin, parentHandler), minimumPulsesPerMinute(minimumPulsesPerMinute),
    maximumPulsesPerMinute(maximumPulsesPerMinute), pulsesPerMinute(-1) {
  pinMode(pin, INPUT);
}

void HeartRateSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  int measuredPulses = map(analogRead(pin), ANALOG_MIN, ANALOG_MAX, minimumPulsesPerMinute, maximumPulsesPerMinute);
  if (measuredPulses == pulsesPerMinute) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  pulsesPerMinute = measuredPulses;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
