/*
  MyIoTDevice.h
  TEMPLATE of the IoT device (rename it, for example SmartTankDevice).
  Example behavior: a potentiometer gives a value 0-100; above the threshold
  the alert LED turns on; the LCD shows the value and the state.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef MY_IOT_DEVICE_H
#define MY_IOT_DEVICE_H

#include "ModestIoT.h"
#include "AnalogSensor.h"
#include "Led.h"
#include "LcdDisplay.h"

// STEP 1: one field per pin of the diagram.
struct MyIoTDeviceWiring {
  int sensorPin;
  int alertLedPin;
  uint8_t displayAddress;
};

class MyIoTDevice : public Device {
public:
  // STEP 2: operation states named in the statement.
  enum class OperationState { Normal, Alert };

  explicit MyIoTDevice(const MyIoTDeviceWiring& wiring);
  void begin() override;
  void update() override;
  void on(Event event) override;

private:
  // STEP 3: every number of the statement becomes a constant (no magic numbers).
  static const int ADC_MAXIMUM = 4095;  // 1023 on Arduino Uno
  static constexpr float MINIMUM_VALUE = 0.0f;
  static constexpr float MAXIMUM_VALUE = 100.0f;
  static constexpr float CHANGE_TOLERANCE = 1.0f;
  static constexpr float ALERT_THRESHOLD = 80.0f;
  static const uint8_t DISPLAY_COLUMNS = 16;
  static const uint8_t DISPLAY_ROWS = 2;
  static const unsigned long SAMPLING_INTERVAL_MS = 500;

  // STEP 4: sensors and actuators of the device (composition).
  AnalogSensor sensor;
  Led alertLed;
  LcdDisplay display;

  // STEP 5: internal state.
  OperationState state;
  unsigned long lastSamplingTime;

  // STEP 6: one method per rule.
  OperationState evaluateState() const;
  void applyState();
  void refreshDisplay();
  void printCompanyInformation() const;
  static const char* getStateLabel(OperationState state);
};

#endif
