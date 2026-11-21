/*
  WaterTemperatureSensor.cpp
  Reads the DHT22 and notifies the device only when the temperature changes.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "WaterTemperatureSensor.h"

WaterTemperatureSensor::WaterTemperatureSensor(int signalPin, float minimumCelsius, float maximumCelsius,
                                               EventHandler* parentHandler)
  : Sensor(signalPin, parentHandler), dht(signalPin, DHT22), minimumCelsius(minimumCelsius),
    maximumCelsius(maximumCelsius), temperatureCelsius(NAN) {}

bool WaterTemperatureSensor::isWithinRange() const {
  return hasReading() && temperatureCelsius >= minimumCelsius && temperatureCelsius <= maximumCelsius;
}

void WaterTemperatureSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  float reading = dht.readTemperature();
  if (isnan(reading)) {  // Sensor error: keep the last valid value
    Serial.printf("[DHT22 pin %d] Read error.\n", pin);
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  if (hasReading() && fabs(reading - temperatureCelsius) < CHANGE_TOLERANCE_C) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  temperatureCelsius = reading;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
