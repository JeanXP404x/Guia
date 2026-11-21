/*
  ValveFlowSensor.cpp
  Maps the 12-bit ADC reading to 0 .. maximum L/min.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "ValveFlowSensor.h"

ValveFlowSensor::ValveFlowSensor(int analogPin, float maximumFlowLitersPerMinute, EventHandler* parentHandler)
  : Sensor(analogPin, parentHandler), maximumFlowLitersPerMinute(maximumFlowLitersPerMinute), flowLitersPerMinute(-1) {
  pinMode(pin, INPUT);
}

void ValveFlowSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  float flow = analogRead(pin) * maximumFlowLitersPerMinute / ADC_MAX;
  if (flowLitersPerMinute >= 0 && fabs(flow - flowLitersPerMinute) < CHANGE_TOLERANCE_LPM) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  flowLitersPerMinute = flow;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
