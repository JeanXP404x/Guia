/*
  SmartLifeWatchingDevice.cpp
  Device logic: periodic sampling, event handling and output updates.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#include "SmartLifeWatchingDevice.h"

namespace {
  const char COMPANY_NAME[] = "Smart Lifemonitor, inc.";
  const char PRODUCT_LINE[] = "LifeWatching";
  const char DEVICE_MODEL[] = "Smart LifeWatching";
  const char DEVELOPER_NAME[] = "Nombres Apellidos";  // TODO: replace with your names
  const char DISPLAY_TITLE[] = "Smart LifeWatching";
  const char LOW_LABEL[] = "LOW";
  const char NORMAL_LABEL[] = "NORMAL";
  const char HIGH_LABEL[] = "HIGH";
  const char UNKNOWN_LABEL[] = "---";
  const char WEIGHT_ALERT_LABEL[] = ">80";
  const uint8_t TITLE_ROW = 0;
  const uint8_t WEIGHT_ROW = 1;
  const uint8_t HEART_RATE_ROW = 2;
  const uint8_t STATUS_ROW = 3;
  const uint8_t WEIGHT_TEXT_WIDTH = 5;
  const uint8_t WEIGHT_DECIMALS = 1;
}

SmartLifeWatchingDevice::SmartLifeWatchingDevice(const LifeWatchingWiring& wiring)
  : weightSensor(wiring.scaleDataPin, wiring.scaleClockPin, RAW_UNITS_PER_KILOGRAM, this),
    heartRateSensor(wiring.heartRatePin, SIMULATED_MINIMUM_PPM, SIMULATED_MAXIMUM_PPM, this),
    weightLed(wiring.weightLedPin, this),
    heartRateLowLed(wiring.heartRateLowLedPin, this),
    heartRateNormalLed(wiring.heartRateNormalLedPin, this),
    heartRateHighLed(wiring.heartRateHighLedPin, this),
    display(wiring.displayAddress, this),
    heartRateStatus(HeartRateStatus::Unknown),
    lastSamplingTime(0) {}

void SmartLifeWatchingDevice::begin() {
  printCompanyInformation();
  display.begin();
  if (!weightSensor.tare()) {
    Serial.println(F("[Scale] HX711 not ready, tare skipped."));
  }
  refreshDisplay();
}

void SmartLifeWatchingDevice::update() {
  unsigned long now = millis();
  if (now - lastSamplingTime < SAMPLING_INTERVAL_MS) return;
  lastSamplingTime = now;

  weightSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
  heartRateSensor.on(Event(Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER));
}

void SmartLifeWatchingDevice::on(Event event) {
  if (event.identifier != Sensor::DATA_READ_EVENT_IDENTIFIER) return;

  if (event.sourceId == weightSensor.getPin()) handleWeightRead();
  else if (event.sourceId == heartRateSensor.getPin()) handleHeartRateRead();
  refreshDisplay();
}

void SmartLifeWatchingDevice::handleWeightRead() {
  float weight = weightSensor.getWeightInKilograms();
  weightLed.turn(weight > WEIGHT_ALERT_THRESHOLD_KG);
  Serial.print(F("[Scale] Weight: "));
  Serial.print(weight, WEIGHT_DECIMALS);
  Serial.println(weightLed.isOn() ? F(" kg (above 80 kg)") : F(" kg"));
}

void SmartLifeWatchingDevice::handleHeartRateRead() {
  int pulses = heartRateSensor.getPulsesPerMinute();
  HeartRateStatus newStatus = classifyHeartRate(pulses);
  if (newStatus != heartRateStatus) {
    heartRateStatus = newStatus;
    updateHeartRateLeds();
  }
  Serial.print(F("[Heart] "));
  Serial.print(pulses);
  Serial.print(F(" PPM - "));
  Serial.println(getStatusLabel(heartRateStatus));
}

SmartLifeWatchingDevice::HeartRateStatus SmartLifeWatchingDevice::classifyHeartRate(int pulsesPerMinute) const {
  if (pulsesPerMinute < LOW_HEART_RATE_LIMIT_PPM) return HeartRateStatus::Low;
  if (pulsesPerMinute > HIGH_HEART_RATE_LIMIT_PPM) return HeartRateStatus::High;
  return HeartRateStatus::Normal;
}

void SmartLifeWatchingDevice::updateHeartRateLeds() {
  heartRateLowLed.turn(heartRateStatus == HeartRateStatus::Low);
  heartRateNormalLed.turn(heartRateStatus == HeartRateStatus::Normal);
  heartRateHighLed.turn(heartRateStatus == HeartRateStatus::High);
}

void SmartLifeWatchingDevice::refreshDisplay() {
  char line[LcdDisplay::COLUMNS + 1];
  char weightText[WEIGHT_TEXT_WIDTH + 2];

  display.setLine(TITLE_ROW, DISPLAY_TITLE);

  float weight = weightSensor.getWeightInKilograms();
  if (weight < 0) {
    snprintf(line, sizeof(line), "Weight: %s kg", UNKNOWN_LABEL);
  } else {
    dtostrf(weight, WEIGHT_TEXT_WIDTH, WEIGHT_DECIMALS, weightText);  // %f is not supported on AVR
    snprintf(line, sizeof(line), "Weight:%s kg %s", weightText, weightLed.isOn() ? WEIGHT_ALERT_LABEL : "");
  }
  display.setLine(WEIGHT_ROW, line);

  int pulses = heartRateSensor.getPulsesPerMinute();
  if (pulses < 0) snprintf(line, sizeof(line), "Heart rate: %s PPM", UNKNOWN_LABEL);
  else snprintf(line, sizeof(line), "Heart rate: %d PPM", pulses);
  display.setLine(HEART_RATE_ROW, line);

  snprintf(line, sizeof(line), "Status: %s", getStatusLabel(heartRateStatus));
  display.setLine(STATUS_ROW, line);

  display.handle(Command(LcdDisplay::UPDATE_TEXT_COMMAND_IDENTIFIER));
}

const char* SmartLifeWatchingDevice::getStatusLabel(HeartRateStatus status) {
  switch (status) {
    case HeartRateStatus::Low: return LOW_LABEL;
    case HeartRateStatus::Normal: return NORMAL_LABEL;
    case HeartRateStatus::High: return HIGH_LABEL;
    default: return UNKNOWN_LABEL;
  }
}

void SmartLifeWatchingDevice::printCompanyInformation() const {
  Serial.println(F("========================================"));
  Serial.print(F("Company   : ")); Serial.println(COMPANY_NAME);
  Serial.print(F("Line      : ")); Serial.println(PRODUCT_LINE);
  Serial.print(F("Device    : ")); Serial.println(DEVICE_MODEL);
  Serial.print(F("Developer : ")); Serial.println(DEVELOPER_NAME);
  Serial.println(F("Rules: weight > 80 kg -> blue LED"));
  Serial.println(F("Heart: <60 LOW blue | 60-100 NORMAL green | >100 HIGH red"));
  Serial.println(F("========================================"));
}
