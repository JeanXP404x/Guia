/*
  ValveFlowSensor.h
  Potentiometer attached to a water valve (cold or hot). The knob position is
  converted into the water flow of that line in liters per minute.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef VALVE_FLOW_SENSOR_H
#define VALVE_FLOW_SENSOR_H

#include "ModestIoT.h"

class ValveFlowSensor : public Sensor {
private:
  static const int ADC_MAX = 4095;
  static constexpr float CHANGE_TOLERANCE_LPM = 0.1f;
  float maximumFlowLitersPerMinute;
  float flowLitersPerMinute;

protected:
  void processEvent(Event& event) override;

public:
  ValveFlowSensor(int analogPin, float maximumFlowLitersPerMinute, EventHandler* parentHandler = nullptr);
  float getFlowLitersPerMinute() const { return flowLitersPerMinute; }
};

#endif
