/*
  WaterTemperatureSensor.h
  DHT22 used to measure the temperature of the cold or hot water line and
  check it against its threshold range.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef WATER_TEMPERATURE_SENSOR_H
#define WATER_TEMPERATURE_SENSOR_H

#include "ModestIoT.h"
#include <DHT.h>

class WaterTemperatureSensor : public Sensor {
private:
  static constexpr float CHANGE_TOLERANCE_C = 0.1f;
  DHT dht;
  float minimumCelsius;
  float maximumCelsius;
  float temperatureCelsius;

protected:
  void processEvent(Event& event) override;

public:
  WaterTemperatureSensor(int signalPin, float minimumCelsius, float maximumCelsius, EventHandler* parentHandler = nullptr);
  void begin() { dht.begin(); }
  float getTemperatureCelsius() const { return temperatureCelsius; }
  bool hasReading() const { return !isnan(temperatureCelsius); }
  bool isWithinRange() const;
};

#endif
