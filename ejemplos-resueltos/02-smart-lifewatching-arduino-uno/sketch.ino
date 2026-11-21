/*
  Smart LifeWatching - Smart Lifemonitor, inc.

  Creates the IoT device and runs it. setup() starts the device and loop()
  only delegates to device.update().

  Demo in Wokwi:
  - Click the load cell and move the slider: 0-50 on the slider = 0-200 kg on the device
    (above 80 kg, slider > 20, turns on the blue weight LED).
  - Turn the potentiometer to simulate the heart rate (30 to 180 PPM).

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#include "SmartLifeWatchingDevice.h"

const unsigned long SERIAL_BAUD_RATE = 9600;

const LifeWatchingWiring DEVICE_WIRING = {
  3,     // scaleDataPin (HX711 DT)
  2,     // scaleClockPin (HX711 SCK)
  A0,    // heartRatePin (potentiometer)
  8,     // weightLedPin (blue)
  9,     // heartRateLowLedPin (blue)
  10,    // heartRateNormalLedPin (green)
  11,    // heartRateHighLedPin (red)
  0x27   // displayAddress (LCD 2004 I2C)
};

SmartLifeWatchingDevice smartLifeWatching(DEVICE_WIRING);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  smartLifeWatching.begin();
}

void loop() {
  smartLifeWatching.update();
}
