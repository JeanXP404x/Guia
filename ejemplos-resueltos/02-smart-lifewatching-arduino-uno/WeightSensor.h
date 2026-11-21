/*
  WeightSensor.h
  HX711 load cell amplifier used to register the patient's weight.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "ModestIoT.h"

class WeightSensor : public Sensor {
private:
  static const int HX711_BIT_COUNT = 24;
  static const int GAIN_128_EXTRA_PULSES = 1;
  static const unsigned long READY_TIMEOUT_MS = 200;
  static const long READ_ERROR = 0x7FFFFFFFL;
  static const long SIGN_BIT_MASK = 0x800000L;
  static const long SIGN_EXTENSION_MASK = 0xFF000000L;
  static constexpr float CHANGE_TOLERANCE_KG = 0.1f;

  int clockPin;
  float rawUnitsPerKilogram;
  long tareOffset;
  float weightInKilograms;

  long readRawValue() const;

protected:
  void processEvent(Event& event) override;

public:
  WeightSensor(int dataPin, int clockPin, float rawUnitsPerKilogram, EventHandler* parentHandler = nullptr);
  bool tare();
  float getWeightInKilograms() const { return weightInKilograms; }
};

#endif
