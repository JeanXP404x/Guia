/*
  AquaSense Smart Tank Controller - AquaSense Tech

  Creates the IoT device; setup() starts it and loop() delegates to update().

  Demo in Wokwi:
  - Click the HC-SR04 and change the distance (tank depth 400 cm):
    distance > 300 cm -> level < 25 % -> valve opens.
    distance < 20 cm  -> level > 95 % -> valve closes.
  - Type OPEN or CLOSE in the Serial Monitor to force the valve, AUTO to return.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "SmartTankDevice.h"

const unsigned long SERIAL_BAUD_RATE = 115200;

const SmartTankWiring DEVICE_WIRING = {
  5,   // triggerPin (HC-SR04 TRIG)
  18,  // echoPin (HC-SR04 ECHO)
  13   // valvePin (servo PWM)
};

SmartTankDevice smartTank(DEVICE_WIRING);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  smartTank.begin();
}

void loop() {
  smartTank.update();
}
