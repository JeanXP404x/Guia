/*
  MyIoTDevice.cpp
  TEMPLATE: sampling in update(), rules in on(), outputs in applyState().

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "MyIoTDevice.h"

namespace {
  // Text constants (no magic strings).
  const char COMPANY_NAME[] = "<Company>";
  const char PRODUCT_NAME[] = "<Product>";
  const char DEVELOPER_NAME[] = "<Your names and surnames>";
  const char NORMAL_LABEL[] = "NORMAL";
  const char ALERT_LABEL[] = "ALERT";
  const uint8_t VALUE_ROW = 0;
  const uint8_t STATE_ROW = 1;
}

MyIoTDevice::MyIoTDevice(const MyIoTDeviceWiring& wiring)
  : sensor(wiring.sensorPin, ADC_MAXIMUM, MINIMUM_VALUE, MAXIMUM_VALUE, CHANGE_TOLERANCE, this),
    alertLed(wiring.alertLedPin, this),
    display(wiring.displayAddress, DISPLAY_COLUMNS, DISPLAY_ROWS, this),
    state(OperationState::Normal),
    lastSamplingTime(0) {}

void MyIoTDevice::begin() {
  printCompanyInformation();
  display.begin();
  applyState();
}

// Polls every sensor periodically without delay().
void MyIoTDevice::update() {
  unsigned long now = millis();
  if (now - lastSamplingTime < SAMPLING_INTERVAL_MS) return;
  lastSamplingTime = now;
  sensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
}

// Mediator: every sensor notifies here; identify it by sourceId (its pin).
void MyIoTDevice::on(Event event) {
  if (event.identifier == Sensor::DATA_READ_EVENT_IDENTIFIER && event.sourceId == sensor.getPin()) {
    OperationState newState = evaluateState();
    if (newState != state) {
      Serial.printf("[State] %s -> %s\n", getStateLabel(state), getStateLabel(newState));
      state = newState;
    }
    applyState();
  }
}

MyIoTDevice::OperationState MyIoTDevice::evaluateState() const {
  return sensor.getValue() > ALERT_THRESHOLD ? OperationState::Alert : OperationState::Normal;
}

void MyIoTDevice::applyState() {
  alertLed.turn(state == OperationState::Alert);
  refreshDisplay();
}

void MyIoTDevice::refreshDisplay() {
  char line[LcdDisplay::MAXIMUM_COLUMNS + 1];
  if (sensor.hasReading()) snprintf(line, sizeof(line), "Value: %.1f", sensor.getValue());
  else snprintf(line, sizeof(line), "Value: --");
  display.setLine(VALUE_ROW, line);
  snprintf(line, sizeof(line), "State: %s", getStateLabel(state));
  display.setLine(STATE_ROW, line);
  display.refresh();
}

const char* MyIoTDevice::getStateLabel(OperationState state) {
  return state == OperationState::Alert ? ALERT_LABEL : NORMAL_LABEL;
}

void MyIoTDevice::printCompanyInformation() const {
  Serial.println("========================================");
  Serial.printf("Company   : %s\n", COMPANY_NAME);
  Serial.printf("Product   : %s\n", PRODUCT_NAME);
  Serial.printf("Developer : %s\n", DEVELOPER_NAME);
  Serial.println("========================================");
}
