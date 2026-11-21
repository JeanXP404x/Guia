/*
  UltrasonicSensor.cpp
  Sends a 10 us trigger pulse and measures the echo time.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int triggerPin, int echoPin, EventHandler* parentHandler)
  : Sensor(triggerPin, parentHandler), echoPin(echoPin), distanceInCentimeters(-1) {
  pinMode(pin, OUTPUT);
  pinMode(this->echoPin, INPUT);
}

void UltrasonicSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0) {  // No echo: keep the last valid value
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  float distance = duration * CM_PER_MICROSECOND;
  if (distanceInCentimeters >= 0 && fabs(distance - distanceInCentimeters) < CHANGE_TOLERANCE_CM) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  distanceInCentimeters = distance;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
