/*
  DhtSensor.cpp
  Ignores failed readings (NaN) and notifies only when a value changes.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "DhtSensor.h"

DhtSensor::DhtSensor(int signalPin, uint8_t dhtType, EventHandler* parentHandler)
  : Sensor(signalPin, parentHandler), dht(signalPin, dhtType), temperatureCelsius(NAN), humidityPercentage(NAN) {}

void DhtSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  if (hasReading() && fabs(temperature - temperatureCelsius) < CHANGE_TOLERANCE &&
      fabs(humidity - humidityPercentage) < CHANGE_TOLERANCE) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  temperatureCelsius = temperature;
  humidityPercentage = humidity;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
