#ifndef OCCUPANCY_DETECTOR_H
#define OCCUPANCY_DETECTOR_H

/**
 * @file OccupancyDetector.h
 * @brief Declares the OccupancyDetector sensor (HC-SR04 ultrasound distance sensor).
 *
 * The sensor is mounted above the parking spot pointing down. When the spot is
 * free it measures the distance to the pavement (about 300 cm); a vehicle is
 * considered to be occupying the spot when the measured distance is 50 cm or less.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>

/**
 * @brief Ultrasound distance sensor specialized as a parking spot occupancy detector.
 *
 * Implements the ModestIoT `Sensor` template method: on every
 * `MEASURE_DATA_REQUESTED_EVENT` it triggers a measurement and propagates a
 * `DATA_READ_EVENT` (sourceId = trigger pin) only when the distance changed.
 * Invalid readings (timeouts / out of range) are suppressed.
 */
class OccupancyDetector : public Sensor {
private:
    int echoGpioPin;                        ///< GPIO pin connected to the ECHO line.
    float occupancyThresholdInCentimeters;  ///< Distance at or below which the spot is occupied.
    float cachedDistanceInCentimeters;      ///< Last valid distance measured.

    static constexpr int   PROPAGATION_SUPPRESSION_IDENTIFIER       = -1;       ///< Stops event propagation.
    static constexpr int   TRIGGER_LOW_SETTLE_TIME_IN_MICROSECONDS  = 2;        ///< Pre-trigger low time.
    static constexpr int   TRIGGER_PULSE_WIDTH_IN_MICROSECONDS      = 10;       ///< Trigger pulse width.
    static constexpr unsigned long ECHO_TIMEOUT_IN_MICROSECONDS     = 30000UL;  ///< Echo timeout (~5 m).
    static constexpr float SOUND_SPEED_HALF_CM_PER_MICROSECOND      = 0.01715f; ///< (343 m/s / 2) in cm/us.
    static constexpr float MINIMUM_VALID_DISTANCE_IN_CENTIMETERS    = 2.0f;     ///< HC-SR04 lower limit.
    static constexpr float MAXIMUM_VALID_DISTANCE_IN_CENTIMETERS    = 400.0f;   ///< HC-SR04 upper limit.
    static constexpr float DISTANCE_CHANGE_TOLERANCE_IN_CENTIMETERS = 1.0f;     ///< Change detection tolerance.
    static constexpr float UNKNOWN_DISTANCE_IN_CENTIMETERS          = -1.0f;    ///< Value before first reading.

    /**
     * @brief Fires the ultrasound burst and measures the echo.
     * @return Distance in centimeters, or a negative value when the reading is invalid.
     */
    float measureDistanceInCentimeters() const;

protected:
    /**
     * @brief Template-method hook: performs the measurement and decides whether to propagate.
     * @param event Event being processed (mutated to DATA_READ or suppressed).
     */
    void processEvent(Event& event) override;

public:
    /**
     * @brief Creates the occupancy detector.
     * @param triggerGpioPin GPIO pin connected to TRIG.
     * @param echoGpioPin GPIO pin connected to ECHO.
     * @param occupancyThresholdInCentimeters Distance at or below which the spot is occupied.
     * @param parentHandler Mediator that receives propagated events.
     */
    OccupancyDetector(int triggerGpioPin, int echoGpioPin, float occupancyThresholdInCentimeters,
                      EventHandler* parentHandler = nullptr);

    /**
     * @brief Returns the last valid measured distance.
     * @return Distance in centimeters (negative when no valid reading yet).
     */
    float getDistanceInCentimeters() const;

    /**
     * @brief Indicates whether a vehicle is occupying the spot.
     * @return `true` when a valid distance is at or below the occupancy threshold.
     */
    bool isOccupied() const;

    /**
     * @brief Returns the GPIO pin used as event source identifier.
     * @return Trigger GPIO pin.
     */
    int getSourceIdentifier() const;
};

#endif // OCCUPANCY_DETECTOR_H
