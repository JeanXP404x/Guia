/*
  SmartTankDevice.cpp
  Device logic: sampling, automatic valve control, remote commands and telemetry.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "SmartTankDevice.h"
#include <ArduinoJson.h>

namespace {
  const char WIFI_SSID[] = "Wokwi-GUEST";
  const char WIFI_PASSWORD[] = "";
  const char REST_ENDPOINT[] = "http://httpbin.org/post";  // Replace with the AquaSense Cloud endpoint
  const char DEVICE_ID[] = "AQS-TANK-001";
  const char COMPANY_NAME[] = "AquaSense Tech";
  const char PRODUCT_NAME[] = "AquaSense Smart Tank Controller";
  const char TEAM_NAME[] = "GreenWave Inc.";
  const char DEVELOPER_NAME[] = "Nombres Apellidos";  // TODO: replace with your names
  const char STATUS_LOW[] = "CRITICAL_LOW";
  const char STATUS_HIGH[] = "CRITICAL_HIGH";
  const char STATUS_NORMAL[] = "NORMAL";
  const char STATUS_UNKNOWN[] = "UNKNOWN";
}

SmartTankDevice::SmartTankDevice(const SmartTankWiring& wiring)
  : levelSensor(wiring.triggerPin, wiring.echoPin, TANK_DEPTH_CM, this),
    remoteReceiver(this),
    valve(wiring.valvePin, this),
    restClient(WIFI_SSID, WIFI_PASSWORD, REST_ENDPOINT),
    mode(OperationMode::Automatic),
    lastSamplingTime(0),
    lastTelemetryTime(0) {}

void SmartTankDevice::begin() {
  printCompanyInformation();
  valve.begin();
  restClient.connect();
}

void SmartTankDevice::update() {
  unsigned long now = millis();
  if (now - lastSamplingTime >= SAMPLING_INTERVAL_MS) {
    lastSamplingTime = now;
    levelSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
    remoteReceiver.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
  }
  if (now - lastTelemetryTime >= TELEMETRY_INTERVAL_MS) {
    sendTelemetry("PERIODIC");
  }
}

void SmartTankDevice::on(Event event) {
  switch (event.identifier) {
    case Sensor::DATA_READ_EVENT_IDENTIFIER:
      handleLevelChange();
      break;
    case RemoteCommandReceiver::FORCE_OPEN_EVENT_IDENTIFIER:
      setMode(OperationMode::Manual);
      setValve(true, "REMOTE_OPEN");
      break;
    case RemoteCommandReceiver::FORCE_CLOSE_EVENT_IDENTIFIER:
      setMode(OperationMode::Manual);
      setValve(false, "REMOTE_CLOSE");
      break;
    case RemoteCommandReceiver::AUTOMATIC_MODE_EVENT_IDENTIFIER:
      setMode(OperationMode::Automatic);
      handleLevelChange();
      break;
  }
}

void SmartTankDevice::handleLevelChange() {
  float level = levelSensor.getLevelInPercentage();
  if (level < 0) return;
  Serial.printf("[Level] %.1f %% (%s)\n", level, getLevelStatus());

  if (mode == OperationMode::Manual) return;  // Remote command has priority over the sensor

  if (level < OPEN_BELOW_PERCENTAGE && !valve.isOpen()) setValve(true, "AUTO_LOW_LEVEL");
  else if (level > CLOSE_ABOVE_PERCENTAGE && valve.isOpen()) setValve(false, "AUTO_HIGH_LEVEL");
}

void SmartTankDevice::setValve(bool open, const char* reason) {
  valve.handle(Command(open ? ValveActuator::OPEN_COMMAND_IDENTIFIER : ValveActuator::CLOSE_COMMAND_IDENTIFIER));
  Serial.printf("[Valve] %s (%s)\n", open ? "OPEN" : "CLOSED", reason);
  sendTelemetry(reason);
}

void SmartTankDevice::setMode(OperationMode newMode) {
  mode = newMode;
  Serial.printf("[Mode] %s\n", mode == OperationMode::Automatic ? "AUTOMATIC" : "MANUAL");
}

const char* SmartTankDevice::getLevelStatus() const {
  float level = levelSensor.getLevelInPercentage();
  if (level < 0) return STATUS_UNKNOWN;
  if (level < OPEN_BELOW_PERCENTAGE) return STATUS_LOW;
  if (level > CLOSE_ABOVE_PERCENTAGE) return STATUS_HIGH;
  return STATUS_NORMAL;
}

void SmartTankDevice::sendTelemetry(const char* reason) {
  lastTelemetryTime = millis();
  JsonDocument document;
  document["deviceId"] = DEVICE_ID;
  document["levelPercentage"] = levelSensor.getLevelInPercentage();
  document["distanceCm"] = levelSensor.getDistanceInCentimeters();
  document["levelStatus"] = getLevelStatus();
  document["valve"] = valve.isOpen() ? "OPEN" : "CLOSED";
  document["mode"] = mode == OperationMode::Automatic ? "AUTOMATIC" : "MANUAL";
  document["reason"] = reason;

  String json;
  serializeJson(document, json);
  if (!restClient.postJson(json)) {
    Serial.printf("[REST] Not sent (offline or error): %s\n", json.c_str());
  }
}

void SmartTankDevice::printCompanyInformation() const {
  Serial.println("========================================");
  Serial.printf("Company   : %s\n", COMPANY_NAME);
  Serial.printf("Product   : %s (%s)\n", PRODUCT_NAME, DEVICE_ID);
  Serial.printf("Developer : %s - %s\n", DEVELOPER_NAME, TEAM_NAME);
  Serial.printf("Rules     : open valve < %.0f %%, close valve > %.0f %%\n", OPEN_BELOW_PERCENTAGE, CLOSE_ABOVE_PERCENTAGE);
  Serial.println("Commands  : OPEN | CLOSE | AUTO (Serial Monitor)");
  Serial.println("========================================");
}
