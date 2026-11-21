/*
  <Product> - <Company>

  Only creates the device and calls it: setup() -> begin(), loop() -> update().

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "MyIoTDevice.h"

const unsigned long SERIAL_BAUD_RATE = 115200;

const MyIoTDeviceWiring DEVICE_WIRING = {
  34,    // sensorPin (potentiometer)
  2,     // alertLedPin
  0x27   // displayAddress (LCD I2C)
};

MyIoTDevice device(DEVICE_WIRING);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  device.begin();
}

void loop() {
  device.update();
}
