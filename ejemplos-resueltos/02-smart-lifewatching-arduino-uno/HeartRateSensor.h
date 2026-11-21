/*
  HeartRateSensor.h
  Heart rate sensor. In the Wokwi prototype it is simulated with a
  potentiometer mapped to pulses per minute (PPM).

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef HEART_RATE_SENSOR_H
#define HEART_RATE_SENSOR_H

#include "ModestIoT.h"

class HeartRateSensor : public Sensor {
private:
  static const int ANALOG_MIN = 0;
  static const int ANALOG_MAX = 1023;

  int minimumPulsesPerMinute;
  int maximumPulsesPerMinute;
  int pulsesPerMinute;

protected:
  void processEvent(Event& event) override;

public:
  HeartRateSensor(int analogPin, int minimumPulsesPerMinute, int maximumPulsesPerMinute,
                  EventHandler* parentHandler = nullptr);
  int getPulsesPerMinute() const { return pulsesPerMinute; }
};

#endif
