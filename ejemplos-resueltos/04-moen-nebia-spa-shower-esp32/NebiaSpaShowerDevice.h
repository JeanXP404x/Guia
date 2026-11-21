/*
  NebiaSpaShowerDevice.h
  Moen Nebia Spa Shower controller. Reads the cold and hot water temperatures
  (DHT22) and the flow of each valve (potentiometers), calculates the mixed
  water temperature and shows temperature, flow and operation state on the LCD.

  Thresholds: cold water 1-20 C, hot water 55-65 C, water flow 4-16 L/min.
  States:
  - INACTIVE: total flow below 4 L/min (valves closed).
  - WARM-UP:  flow in range, but cold or hot water out of its temperature range.
  - ACTIVE:   flow in range and both temperatures in range.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef NEBIA_SPA_SHOWER_DEVICE_H
#define NEBIA_SPA_SHOWER_DEVICE_H

#include "ModestIoT.h"
#include "WaterTemperatureSensor.h"
#include "ValveFlowSensor.h"
#include "LcdDisplay.h"
#include "WiFiConnection.h"

struct ShowerWiring {
  int coldTemperaturePin;
  int hotTemperaturePin;
  int coldValvePin;
  int hotValvePin;
  uint8_t displayAddress;
};

class NebiaSpaShowerDevice : public Device {
public:
  enum class OperationState { Inactive, WarmUp, Active };

  explicit NebiaSpaShowerDevice(const ShowerWiring& wiring);
  void begin() override;
  void update() override;
  void on(Event event) override;

private:
  static constexpr float COLD_MIN_C = 1.0f;
  static constexpr float COLD_MAX_C = 20.0f;
  static constexpr float HOT_MIN_C = 55.0f;
  static constexpr float HOT_MAX_C = 65.0f;
  static constexpr float MIN_FLOW_LPM = 4.0f;
  static constexpr float MAX_FLOW_LPM = 16.0f;
  static constexpr float MAX_FLOW_PER_VALVE_LPM = MAX_FLOW_LPM / 2;
  static const unsigned long FLOW_SAMPLING_MS = 500;
  static const unsigned long TEMPERATURE_SAMPLING_MS = 2000;  // DHT22 minimum read period

  WaterTemperatureSensor coldTemperatureSensor;
  WaterTemperatureSensor hotTemperatureSensor;
  ValveFlowSensor coldValve;
  ValveFlowSensor hotValve;
  LcdDisplay display;
  WiFiConnection wifi;

  OperationState state;
  unsigned long lastFlowSampling;
  unsigned long lastTemperatureSampling;

  float getTotalFlow() const;
  bool calculateMixTemperature(float& mixTemperature) const;
  OperationState evaluateState() const;
  void refresh();
  void printCompanyInformation() const;
  static const char* getStateLabel(OperationState state);
};

#endif
