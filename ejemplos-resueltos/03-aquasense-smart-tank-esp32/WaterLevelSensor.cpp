/*
  WaterLevelSensor.cpp
  Measures the distance and notifies the device only when the level changes.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "WaterLevelSensor.h"

WaterLevelSensor::WaterLevelSensor(int triggerPin, int echoPin, float tankDepthInCentimeters, EventHandler* parentHandler)
  : Sensor(triggerPin, parentHandler), echoPin(echoPin), tankDepthInCentimeters(tankDepthInCentimeters),
    distanceInCentimeters(-1), levelInPercentage(-1) {
  pinMode(pin, OUTPUT);
  pinMode(this->echoPin, INPUT);
}

float WaterLevelSensor::measureDistance() const {
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);
  return duration == 0 ? -1 : duration * CM_PER_MICROSECOND;
}

void WaterLevelSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  float distance = measureDistance();
  if (distance < 0) {  // No echo: keep the last valid value
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }

  float level = (tankDepthInCentimeters - distance) / tankDepthInCentimeters * MAX_PERCENTAGE;
  level = constrain(level, 0.0f, MAX_PERCENTAGE);

  if (levelInPercentage >= 0 && fabs(level - levelInPercentage) < CHANGE_TOLERANCE_PERCENTAGE) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  distanceInCentimeters = distance;
  levelInPercentage = level;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
