/*
  SmartLifeWatchingDevice.h
  Smart LifeWatching device (LifeWatching line). Registers the patient's weight
  and heart rate, drives the signaling LEDs and shows the data on the LCD 2004.

  Rules:
  - Weight > 80 kg            -> weight LED (blue) on.
  - Heart rate < 60 PPM       -> LOW,    blue LED.
  - Heart rate 60 to 100 PPM  -> NORMAL, green LED.
  - Heart rate > 100 PPM      -> HIGH,   red LED.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef SMART_LIFE_WATCHING_DEVICE_H
#define SMART_LIFE_WATCHING_DEVICE_H

#include "ModestIoT.h"
#include "WeightSensor.h"
#include "HeartRateSensor.h"
#include "Led.h"
#include "LcdDisplay.h"

struct LifeWatchingWiring {
  int scaleDataPin;
  int scaleClockPin;
  int heartRatePin;
  int weightLedPin;
  int heartRateLowLedPin;
  int heartRateNormalLedPin;
  int heartRateHighLedPin;
  uint8_t displayAddress;
};

class SmartLifeWatchingDevice : public Device {
public:
  enum class HeartRateStatus { Unknown, Low, Normal, High };

  explicit SmartLifeWatchingDevice(const LifeWatchingWiring& wiring);
  void begin() override;
  void update() override;
  void on(Event event) override;

private:
  static constexpr float WEIGHT_ALERT_THRESHOLD_KG = 80.0f;
  static const int LOW_HEART_RATE_LIMIT_PPM = 60;
  static const int HIGH_HEART_RATE_LIMIT_PPM = 100;
  static const int SIMULATED_MINIMUM_PPM = 30;
  static const int SIMULATED_MAXIMUM_PPM = 180;
  // Patient platform with 4 load cells sharing the load: one Wokwi 50 kg cell (21000 raw = 50 kg)
  // represents the platform, so the slider covers 0 to 200 kg (21000 raw / 200 kg).
  static constexpr float RAW_UNITS_PER_KILOGRAM = 105.0f;
  static const unsigned long SAMPLING_INTERVAL_MS = 500;

  WeightSensor weightSensor;
  HeartRateSensor heartRateSensor;
  Led weightLed;
  Led heartRateLowLed;
  Led heartRateNormalLed;
  Led heartRateHighLed;
  LcdDisplay display;

  HeartRateStatus heartRateStatus;
  unsigned long lastSamplingTime;

  void handleWeightRead();
  void handleHeartRateRead();
  HeartRateStatus classifyHeartRate(int pulsesPerMinute) const;
  void updateHeartRateLeds();
  void refreshDisplay();
  void printCompanyInformation() const;
  static const char* getStatusLabel(HeartRateStatus status);
};

#endif
