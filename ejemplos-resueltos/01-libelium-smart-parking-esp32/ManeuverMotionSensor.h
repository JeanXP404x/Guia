#ifndef MANEUVER_MOTION_SENSOR_H
#define MANEUVER_MOTION_SENSOR_H

/**
 * @file ManeuverMotionSensor.h
 * @brief Declares the ManeuverMotionSensor (PIR motion sensor).
 *
 * Detects movement during a parking maneuver. While the spot is reserved, a
 * detected motion means the authorized vehicle is approaching.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>

/**
 * @brief PIR motion sensor that propagates an event on each new motion detection (rising edge).
 */
class ManeuverMotionSensor : public Sensor {
private:
    bool previousMotionState; ///< Last sampled PIR output level (true = motion).

    static constexpr int PROPAGATION_SUPPRESSION_IDENTIFIER = -1; ///< Stops event propagation.

protected:
    /**
     * @brief Template-method hook: samples the PIR output and detects rising edges.
     * @param event Event being processed (mutated to MOTION_DETECTED or suppressed).
     */
    void processEvent(Event& event) override;

public:
    /// Event raised when a new motion is detected.
    static constexpr int MOTION_DETECTED_EVENT_IDENTIFIER = 30;

    /**
     * @brief Creates the PIR motion sensor.
     * @param signalGpioPin GPIO pin connected to the PIR OUT line.
     * @param parentHandler Mediator that receives propagated events.
     */
    explicit ManeuverMotionSensor(int signalGpioPin, EventHandler* parentHandler = nullptr);

    /**
     * @brief Indicates whether motion is currently being detected.
     * @return `true` while the PIR output is HIGH.
     */
    bool isMotionDetected() const;
};

#endif // MANEUVER_MOTION_SENSOR_H
