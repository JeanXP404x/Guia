/*
  Moen Nebia Spa Shower - Moen, Inc.

  Creates the IoT device; setup() starts it and loop() delegates to update().

  Demo in Wokwi:
  - Turn the potentiometers (cold / hot valve, 0-8 L/min each). Below 4 L/min -> INACTIVE.
  - Click each DHT22 and change its temperature: cold 1-20 C, hot 55-65 C -> ACTIVE,
    otherwise WARM-UP (for example, hot water still below 55 C).

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "NebiaSpaShowerDevice.h"

const unsigned long SERIAL_BAUD_RATE = 115200;

const ShowerWiring DEVICE_WIRING = {
  15,    // coldTemperaturePin (DHT22 cold water)
  4,     // hotTemperaturePin (DHT22 hot water)
  34,    // coldValvePin (potentiometer)
  35,    // hotValvePin (potentiometer)
  0x27   // displayAddress (LCD 1602 I2C)
};

NebiaSpaShowerDevice nebiaSpaShower(DEVICE_WIRING);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  nebiaSpaShower.begin();
}

void loop() {
  nebiaSpaShower.update();
}
