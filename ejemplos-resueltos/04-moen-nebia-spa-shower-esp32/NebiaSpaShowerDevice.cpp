/*
  NebiaSpaShowerDevice.cpp
  Device logic: sampling, mixed temperature (weighted average), state machine and display.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "NebiaSpaShowerDevice.h"

namespace {
  const char WIFI_SSID[] = "Wokwi-GUEST";
  const char WIFI_PASSWORD[] = "";
  const char COMPANY_NAME[] = "Moen, Inc. (Fortune Brands Innovations)";
  const char PRODUCT_NAME[] = "Moen Nebia Spa Shower";
  const char DEVELOPER_NAME[] = "Nombres Apellidos";  // TODO: replace with your names
  const char INACTIVE_LABEL[] = "INACTIVE";
  const char WARM_UP_LABEL[] = "WARM-UP";
  const char ACTIVE_LABEL[] = "ACTIVE";
  const uint8_t MEASURE_ROW = 0;
  const uint8_t STATE_ROW = 1;
}

NebiaSpaShowerDevice::NebiaSpaShowerDevice(const ShowerWiring& wiring)
  : coldTemperatureSensor(wiring.coldTemperaturePin, COLD_MIN_C, COLD_MAX_C, this),
    hotTemperatureSensor(wiring.hotTemperaturePin, HOT_MIN_C, HOT_MAX_C, this),
    coldValve(wiring.coldValvePin, MAX_FLOW_PER_VALVE_LPM, this),
    hotValve(wiring.hotValvePin, MAX_FLOW_PER_VALVE_LPM, this),
    display(wiring.displayAddress, this),
    wifi(WIFI_SSID, WIFI_PASSWORD),
    state(OperationState::Inactive),
    lastFlowSampling(0),
    lastTemperatureSampling(0) {}

void NebiaSpaShowerDevice::begin() {
  printCompanyInformation();
  coldTemperatureSensor.begin();
  hotTemperatureSensor.begin();
  display.begin();
  refresh();
  wifi.connect();
}

void NebiaSpaShowerDevice::update() {
  unsigned long now = millis();
  if (now - lastFlowSampling >= FLOW_SAMPLING_MS) {
    lastFlowSampling = now;
    coldValve.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
    hotValve.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
  }
  if (now - lastTemperatureSampling >= TEMPERATURE_SAMPLING_MS) {
    lastTemperatureSampling = now;
    coldTemperatureSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
    hotTemperatureSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
  }
}

void NebiaSpaShowerDevice::on(Event event) {
  if (event.identifier == Sensor::DATA_READ_EVENT_IDENTIFIER) refresh();
}

float NebiaSpaShowerDevice::getTotalFlow() const {
  return max(coldValve.getFlowLitersPerMinute(), 0.0f) + max(hotValve.getFlowLitersPerMinute(), 0.0f);
}

// T_mix = (Q_hot * T_hot + Q_cold * T_cold) / (Q_hot + Q_cold)
bool NebiaSpaShowerDevice::calculateMixTemperature(float& mixTemperature) const {
  float totalFlow = getTotalFlow();
  if (totalFlow <= 0 || !coldTemperatureSensor.hasReading() || !hotTemperatureSensor.hasReading()) return false;
  mixTemperature = (hotValve.getFlowLitersPerMinute() * hotTemperatureSensor.getTemperatureCelsius() +
                    coldValve.getFlowLitersPerMinute() * coldTemperatureSensor.getTemperatureCelsius()) / totalFlow;
  return true;
}

NebiaSpaShowerDevice::OperationState NebiaSpaShowerDevice::evaluateState() const {
  float totalFlow = getTotalFlow();
  if (totalFlow < MIN_FLOW_LPM || totalFlow > MAX_FLOW_LPM) return OperationState::Inactive;
  if (!coldTemperatureSensor.isWithinRange() || !hotTemperatureSensor.isWithinRange()) return OperationState::WarmUp;
  return OperationState::Active;
}

void NebiaSpaShowerDevice::refresh() {
  OperationState newState = evaluateState();
  if (newState != state) {
    Serial.printf("[State] %s -> %s\n", getStateLabel(state), getStateLabel(newState));
    state = newState;
  }

  char line[LcdDisplay::COLUMNS + 1];
  float mixTemperature;
  if (calculateMixTemperature(mixTemperature)) {
    snprintf(line, sizeof(line), "T:%.1fC Q:%.1fL", mixTemperature, getTotalFlow());
  } else {
    snprintf(line, sizeof(line), "T:--.-C Q:%.1fL", getTotalFlow());
  }
  display.setLine(MEASURE_ROW, line);
  Serial.printf("[Shower] %s | cold %.1fC %.1fL/min | hot %.1fC %.1fL/min\n", line,
                coldTemperatureSensor.getTemperatureCelsius(), coldValve.getFlowLitersPerMinute(),
                hotTemperatureSensor.getTemperatureCelsius(), hotValve.getFlowLitersPerMinute());

  snprintf(line, sizeof(line), "State: %s", getStateLabel(state));
  display.setLine(STATE_ROW, line);
  display.handle(Command(LcdDisplay::UPDATE_TEXT_COMMAND_IDENTIFIER));
}

const char* NebiaSpaShowerDevice::getStateLabel(OperationState state) {
  switch (state) {
    case OperationState::WarmUp: return WARM_UP_LABEL;
    case OperationState::Active: return ACTIVE_LABEL;
    default: return INACTIVE_LABEL;
  }
}

void NebiaSpaShowerDevice::printCompanyInformation() const {
  Serial.println("========================================");
  Serial.printf("Company   : %s\n", COMPANY_NAME);
  Serial.printf("Product   : %s\n", PRODUCT_NAME);
  Serial.printf("Developer : %s - Moen, Inc. team\n", DEVELOPER_NAME);
  Serial.printf("Cold water %.0f-%.0f C | Hot water %.0f-%.0f C | Flow %.0f-%.0f L/min\n",
                COLD_MIN_C, COLD_MAX_C, HOT_MIN_C, HOT_MAX_C, MIN_FLOW_LPM, MAX_FLOW_LPM);
  Serial.println("States: INACTIVE | WARM-UP | ACTIVE");
  Serial.println("========================================");
}
