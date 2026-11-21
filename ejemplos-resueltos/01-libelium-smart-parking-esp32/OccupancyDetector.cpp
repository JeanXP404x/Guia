/**
 * @file OccupancyDetector.cpp
 * @brief Implements the OccupancyDetector sensor (HC-SR04 ultrasound distance sensor).
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include "OccupancyDetector.h"
#include <cmath>

OccupancyDetector::OccupancyDetector(int triggerGpioPin, int echoGpioPin, float occupancyThresholdInCentimeters,
                                     EventHandler* parentHandler)
    : Sensor(triggerGpioPin, parentHandler),
      echoGpioPin(echoGpioPin),
      occupancyThresholdInCentimeters(occupancyThresholdInCentimeters),
      cachedDistanceInCentimeters(UNKNOWN_DISTANCE_IN_CENTIMETERS)
{
    pinMode(pin, OUTPUT);
    pinMode(this->echoGpioPin, INPUT);
    digitalWrite(pin, LOW);
}

float OccupancyDetector::measureDistanceInCentimeters() const {
    digitalWrite(pin, LOW);
    delayMicroseconds(TRIGGER_LOW_SETTLE_TIME_IN_MICROSECONDS);
    digitalWrite(pin, HIGH);
    delayMicroseconds(TRIGGER_PULSE_WIDTH_IN_MICROSECONDS);
    digitalWrite(pin, LOW);

    const unsigned long echoDurationInMicroseconds = pulseIn(echoGpioPin, HIGH, ECHO_TIMEOUT_IN_MICROSECONDS);
    if (echoDurationInMicroseconds == 0) {
        return UNKNOWN_DISTANCE_IN_CENTIMETERS; // Timeout: no echo received
    }
    return static_cast<float>(echoDurationInMicroseconds) * SOUND_SPEED_HALF_CM_PER_MICROSECOND;
}

void OccupancyDetector::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) {
        return;
    }

    const float measuredDistance = measureDistanceInCentimeters();

    // Exception handling: discard timeouts and physically impossible readings
    if (measuredDistance < MINIMUM_VALID_DISTANCE_IN_CENTIMETERS ||
        measuredDistance > MAXIMUM_VALID_DISTANCE_IN_CENTIMETERS) {
        event.identifier = PROPAGATION_SUPPRESSION_IDENTIFIER;
        return;
    }

    // Change detection: only notify the mediator when the distance really changed
    if (cachedDistanceInCentimeters >= 0.0f &&
        std::fabs(measuredDistance - cachedDistanceInCentimeters) < DISTANCE_CHANGE_TOLERANCE_IN_CENTIMETERS) {
        event.identifier = PROPAGATION_SUPPRESSION_IDENTIFIER;
        return;
    }

    cachedDistanceInCentimeters = measuredDistance;
    event = Event(DATA_READ_EVENT_IDENTIFIER, pin);
}

float OccupancyDetector::getDistanceInCentimeters() const {
    return cachedDistanceInCentimeters;
}

bool OccupancyDetector::isOccupied() const {
    return cachedDistanceInCentimeters >= 0.0f && cachedDistanceInCentimeters <= occupancyThresholdInCentimeters;
}

int OccupancyDetector::getSourceIdentifier() const {
    return pin;
}
