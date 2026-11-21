/**
 * @file ManeuverMotionSensor.cpp
 * @brief Implements the ManeuverMotionSensor (PIR motion sensor).
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include "ManeuverMotionSensor.h"

ManeuverMotionSensor::ManeuverMotionSensor(int signalGpioPin, EventHandler* parentHandler)
    : Sensor(signalGpioPin, parentHandler),
      previousMotionState(false)
{
    pinMode(pin, INPUT);
}

void ManeuverMotionSensor::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) {
        return;
    }

    const bool currentMotionState = isMotionDetected();
    const bool motionStarted = currentMotionState && !previousMotionState;
    previousMotionState = currentMotionState;

    if (motionStarted) {
        event = Event(MOTION_DETECTED_EVENT_IDENTIFIER, pin);
    } else {
        event.identifier = PROPAGATION_SUPPRESSION_IDENTIFIER;
    }
}

bool ManeuverMotionSensor::isMotionDetected() const {
    return digitalRead(pin) == HIGH;
}
