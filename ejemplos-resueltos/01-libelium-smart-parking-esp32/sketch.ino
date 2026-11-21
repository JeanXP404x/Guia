/**
 * @file sketch.ino
 * @brief Entry point of the Smart Parking Spot S1 (Libelium Smart Cities) Wokwi prototype.
 *
 * Only creates the SmartParkingSpotDevice and starts it. All behavior runs in the
 * ModestIoT asynchronous engine (FreeRTOS), so loop() only inhibits itself.
 *
 * Demo (Serial Monitor + Wokwi parts):
 * - Click the HC-SR04 and set distance <= 50 cm  -> BUSY (red), minutes counting.
 * - Keep it occupied > 96 min (96 s in demo mode) -> OVERTIME (blinking red).
 * - Set distance back to 300 cm                  -> FREE (green).
 * - Type RESERVE                                 -> RESERVED (blue, bollard raised).
 * - Click the PIR sensor (motion) or type CANCEL -> bollard lowered, detection resumes.
 * - Type STATUS                                  -> operation tracking record (JSON).
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <Arduino.h>
#include <ModestIoT.h>
#include "SmartParkingSpotDevice.h"

static const unsigned long SERIAL_BAUD_RATE     = 115200;
static const unsigned long LOOP_INHIBIT_DELAY_MS = 60000;

/// Wiring of the prototype (see diagram.json).
static const ParkingSpotWiring PARKING_SPOT_WIRING = {
    5,    // ultrasonicTriggerPin
    18,   // ultrasonicEchoPin
    19,   // motionSensorPin
    13,   // bollardServoPin
    25,   // ledRedPin
    26,   // ledGreenPin
    27,   // ledBluePin
    0x27  // displayI2cAddress
};

static SmartParkingSpotDevice* smartParkingSpot = nullptr;

/**
 * @brief Creates and starts the IoT device.
 */
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    smartParkingSpot = new SmartParkingSpotDevice(PARKING_SPOT_WIRING);
    smartParkingSpot->start();
}

/**
 * @brief Inhibited loop: all work is performed by the ModestIoT background workers.
 */
void loop() {
    vTaskDelay(pdMS_TO_TICKS(LOOP_INHIBIT_DELAY_MS));
}
