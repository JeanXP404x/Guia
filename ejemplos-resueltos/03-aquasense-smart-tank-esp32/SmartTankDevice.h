/*
  SmartTankDevice.h
  AquaSense Smart Tank Controller. Monitors the tank level and opens or closes
  the valve: opens below 25 %, closes above 95 %. The app can force OPEN or
  CLOSE (manual mode) and return to AUTO.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef SMART_TANK_DEVICE_H
#define SMART_TANK_DEVICE_H

#include "ModestIoT.h"
#include "WaterLevelSensor.h"
#include "RemoteCommandReceiver.h"
#include "ValveActuator.h"
#include "RestClient.h"

struct SmartTankWiring {
  int triggerPin;
  int echoPin;
  int valvePin;
};

class SmartTankDevice : public Device {
public:
  enum class OperationMode { Automatic, Manual };

  explicit SmartTankDevice(const SmartTankWiring& wiring);
  void begin() override;
  void update() override;
  void on(Event event) override;

private:
  static constexpr float TANK_DEPTH_CM = 400.0f;
  static constexpr float OPEN_BELOW_PERCENTAGE = 25.0f;
  static constexpr float CLOSE_ABOVE_PERCENTAGE = 95.0f;
  static const unsigned long SAMPLING_INTERVAL_MS = 500;
  static const unsigned long TELEMETRY_INTERVAL_MS = 10000;

  WaterLevelSensor levelSensor;
  RemoteCommandReceiver remoteReceiver;
  ValveActuator valve;
  RestClient restClient;

  OperationMode mode;
  unsigned long lastSamplingTime;
  unsigned long lastTelemetryTime;

  void handleLevelChange();
  void setValve(bool open, const char* reason);
  void setMode(OperationMode newMode);
  void sendTelemetry(const char* reason);
  const char* getLevelStatus() const;
  void printCompanyInformation() const;
};

#endif
