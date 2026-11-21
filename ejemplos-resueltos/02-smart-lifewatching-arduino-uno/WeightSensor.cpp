/*
  WeightSensor.cpp
  Reads the HX711 (24-bit, gain 128) and converts the value to kilograms.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#include "WeightSensor.h"

WeightSensor::WeightSensor(int dataPin, int clockPin, float rawUnitsPerKilogram, EventHandler* parentHandler)
  : Sensor(dataPin, parentHandler), clockPin(clockPin), rawUnitsPerKilogram(rawUnitsPerKilogram),
    tareOffset(0), weightInKilograms(-1.0f) {
  pinMode(pin, INPUT);
  pinMode(this->clockPin, OUTPUT);
  digitalWrite(this->clockPin, LOW);
}

long WeightSensor::readRawValue() const {
  unsigned long startTime = millis();
  while (digitalRead(pin) == HIGH) {
    if (millis() - startTime > READY_TIMEOUT_MS) return READ_ERROR;
  }

  long value = 0;
  noInterrupts();
  for (int bit = 0; bit < HX711_BIT_COUNT; bit++) {
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
    value = (value << 1) | digitalRead(pin);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
  }
  for (int pulse = 0; pulse < GAIN_128_EXTRA_PULSES; pulse++) {
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    delayMicroseconds(1);
  }
  interrupts();

  if (value & SIGN_BIT_MASK) value |= SIGN_EXTENSION_MASK;
  return value;
}

bool WeightSensor::tare() {
  long rawValue = readRawValue();
  if (rawValue == READ_ERROR) return false;
  tareOffset = rawValue;
  return true;
}

void WeightSensor::processEvent(Event& event) {
  if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) return;

  long rawValue = readRawValue();
  if (rawValue == READ_ERROR) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }

  float measuredWeight = (rawValue - tareOffset) / rawUnitsPerKilogram;
  if (measuredWeight < 0) measuredWeight = 0;

  if (weightInKilograms >= 0 && fabs(measuredWeight - weightInKilograms) < CHANGE_TOLERANCE_KG) {
    event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
    return;
  }
  weightInKilograms = measuredWeight;
  event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}
