/*
  WaterLevelSensor.h
  HC-SR04 mounted on the tank lid. Converts the distance to the water surface
  into a water level percentage.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#include "ModestIoT.h"

class WaterLevelSensor : public Sensor {
private:
  static const unsigned long ECHO_TIMEOUT_US = 30000;
  static constexpr float CM_PER_MICROSECOND = 0.01715f;
  static constexpr float MAX_PERCENTAGE = 100.0f;
  static constexpr float CHANGE_TOLERANCE_PERCENTAGE = 0.5f;

  int echoPin;
  float tankDepthInCentimeters;
  float distanceInCentimeters;
  float levelInPercentage;

  float measureDistance() const;

protected:
  void processEvent(Event& event) override;

public:
  WaterLevelSensor(int triggerPin, int echoPin, float tankDepthInCentimeters, EventHandler* parentHandler = nullptr);
  float getLevelInPercentage() const { return levelInPercentage; }
  float getDistanceInCentimeters() const { return distanceInCentimeters; }
};

#endif
