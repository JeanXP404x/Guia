/*
  UltrasonicSensor.h
  HC-SR04 distance sensor. Notifies DATA_READ (sourceId = trigger pin) when the distance changes.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include "ModestIoT.h"

class UltrasonicSensor : public Sensor {
private:
  static const unsigned long ECHO_TIMEOUT_US = 30000;
  static constexpr float CM_PER_MICROSECOND = 0.01715f;
  static constexpr float CHANGE_TOLERANCE_CM = 1.0f;
  int echoPin;
  float distanceInCentimeters;

protected:
  void processEvent(Event& event) override;

public:
  UltrasonicSensor(int triggerPin, int echoPin, EventHandler* parentHandler = nullptr);
  float getDistanceInCentimeters() const { return distanceInCentimeters; }
};

#endif
