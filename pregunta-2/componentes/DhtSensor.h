/*
  DhtSensor.h
  DHT22 / DHT11 temperature and humidity sensor. Read it every 2 s or more.
  Library: "DHT sensor library" + "Adafruit Unified Sensor".

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "ModestIoT.h"
#include <DHT.h>

class DhtSensor : public Sensor {
private:
  static constexpr float CHANGE_TOLERANCE = 0.1f;
  DHT dht;
  float temperatureCelsius;
  float humidityPercentage;

protected:
  void processEvent(Event& event) override;

public:
  DhtSensor(int signalPin, uint8_t dhtType, EventHandler* parentHandler = nullptr);
  void begin() { dht.begin(); }
  float getTemperatureCelsius() const { return temperatureCelsius; }
  float getHumidityPercentage() const { return humidityPercentage; }
  bool hasReading() const { return !isnan(temperatureCelsius); }
};

#endif
