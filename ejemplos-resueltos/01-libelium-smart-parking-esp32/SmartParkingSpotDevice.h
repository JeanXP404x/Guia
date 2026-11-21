#ifndef SMART_PARKING_SPOT_DEVICE_H
#define SMART_PARKING_SPOT_DEVICE_H

/**
 * @file SmartParkingSpotDevice.h
 * @brief Declares the SmartParkingSpotDevice, the ModestIoT mediator of the Smart Parking Spot S1.
 *
 * Coordinates the occupancy detector (HC-SR04), the PIR maneuver sensor, the
 * remote reservation channel, the bollard servo, the RGB status LED and the
 * 16x2 I2C display, implementing the parking spot state machine:
 * FREE -> BUSY -> OVERTIME -> FREE, and FREE -> RESERVED -> FREE/BUSY.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>
#include "OccupancyDetector.h"
#include "ManeuverMotionSensor.h"
#include "RemoteReservationReceiver.h"
#include "StatusRgbLed.h"
#include "ParkingTrackingRecord.h"

/**
 * @brief GPIO / bus wiring of the device, grouped to keep the constructor readable.
 */
struct ParkingSpotWiring {
    int ultrasonicTriggerPin;  ///< HC-SR04 TRIG pin.
    int ultrasonicEchoPin;     ///< HC-SR04 ECHO pin.
    int motionSensorPin;       ///< PIR OUT pin.
    int bollardServoPin;       ///< Servo PWM pin.
    int ledRedPin;             ///< RGB LED red pin.
    int ledGreenPin;           ///< RGB LED green pin.
    int ledBluePin;            ///< RGB LED blue pin.
    uint8_t displayI2cAddress; ///< LCD 16x2 I2C address.
};

/**
 * @brief Smart Parking Spot S1 IoT device (event-driven mediator).
 */
class SmartParkingSpotDevice : public Device {
public:
    /**
     * @brief Operation modes of the parking spot.
     */
    enum class ParkingSpotState { Free, Busy, Overtime, Reserved };

    /**
     * @brief Creates the device and all of its sensors and actuators.
     * @param wiring GPIO / I2C wiring description.
     */
    explicit SmartParkingSpotDevice(const ParkingSpotWiring& wiring);

    /**
     * @brief Prints device information, renders the initial state and starts the asynchronous engine.
     * Must be called once from setup().
     */
    void start();

    /**
     * @brief Inversion of Control handler for every sensor event and the scheduler clock tick.
     * @param event Event received.
     */
    void on(Event event) override;

protected:
    /**
     * @brief Publishes queued tracking records (simulated Libelium Cloud uplink through the console).
     * @param rawQueueItemPayload Queued telemetry payload.
     */
    void processQueuedTelemetryData(const TelemetryPackage* rawQueueItemPayload) const override;

private:
    // --- Business rules ---
    static constexpr float         OCCUPANCY_THRESHOLD_IN_CENTIMETERS        = 50.0f;  ///< Occupied when <= 50 cm.
    static constexpr unsigned long MAXIMUM_PARKING_TIME_IN_MINUTES           = 120;    ///< Maximum allowed time.
    static constexpr unsigned long OVERTIME_ALERT_PERCENTAGE                 = 80;     ///< Alert at 80 % of max.
    static constexpr unsigned long PERCENTAGE_BASE                           = 100;    ///< Percentage divisor.
    static constexpr unsigned long OVERTIME_THRESHOLD_IN_MINUTES             =
        MAXIMUM_PARKING_TIME_IN_MINUTES * OVERTIME_ALERT_PERCENTAGE / PERCENTAGE_BASE; ///< 96 minutes.

    /// Duration of one occupancy minute. DEMO acceleration: 1 s = 1 min (use 60000 in production).
    static constexpr unsigned long OCCUPANCY_MINUTE_DURATION_IN_MILLISECONDS = 1000;
    static constexpr unsigned long MILLISECONDS_PER_SECOND                   = 1000;

    // --- Scheduler configuration ---
    static constexpr unsigned long SCHEDULER_TICK_IN_MILLISECONDS = 100; ///< Round-robin tick (4 slots -> 400 ms each).
    static constexpr uint8_t       HARDWARE_TIMER_INDEX           = 0;   ///< ESP32 timer used by the scheduler.
    static constexpr uint8_t       TELEMETRY_QUEUE_DEPTH          = 8;   ///< Tracking records queue size.
    static constexpr int           CLOCK_TICK_EVENT_IDENTIFIER    = 50;  ///< Self-scheduled clock event.

    // --- Actuator configuration ---
    static constexpr int     BOLLARD_LOWERED_ANGLE_IN_DEGREES = 0;    ///< Bollard retracted.
    static constexpr int     BOLLARD_RAISED_ANGLE_IN_DEGREES  = 90;   ///< Bollard raised (spot reserved).
    static constexpr uint8_t DISPLAY_COLUMNS                  = 16;   ///< LCD columns.
    static constexpr uint8_t DISPLAY_ROWS                     = 2;    ///< LCD rows.
    static constexpr uint8_t DISPLAY_STATE_ROW                = 0;    ///< Row with the state label.
    static constexpr uint8_t DISPLAY_TIME_ROW                 = 1;    ///< Row with the occupancy time.
    static constexpr bool    DISPLAY_BACKLIGHT_ON             = true; ///< Initial backlight state.

    // --- Text resources (avoid magic strings) ---
    static constexpr const char* FREE_LABEL            = "FREE";
    static constexpr const char* BUSY_LABEL            = "BUSY";
    static constexpr const char* RESERVED_LABEL        = "RESERVED";
    static constexpr const char* OVERTIME_LABEL        = "OVERTIME";
    static constexpr const char* TIME_LINE_FORMAT      = "Time: %lu Min";
    static constexpr const char* DEVICE_IDENTIFIER     = "LIB-SPS-S1-0001";
    static constexpr const char* REASON_BOOT           = "BOOT";
    static constexpr const char* REASON_STATE_CHANGE   = "STATE_CHANGE";
    static constexpr const char* REASON_OVERTIME_ALERT = "OVERTIME_ALERT";
    static constexpr const char* REASON_ON_DEMAND      = "ON_DEMAND";

    static constexpr const char* COMPANY_NAME          = "Libelium";
    static constexpr const char* COMPANY_WEBSITE       = "https://www.libelium.com/";
    static constexpr const char* COMPANY_PRODUCT_LINE  = "Libelium Smart Cities";
    static constexpr const char* DEVICE_MODEL          = "Smart Parking Spot S1";
    static constexpr const char* FIRMWARE_VERSION      = "1.0.0";
    /// TODO: replace with your first and last names.
    static constexpr const char* DEVELOPER_FULL_NAME   = "Nombres Apellidos";
    static constexpr const char* DEVELOPER_ROLE        = "Embedded Developer - Libelium Team";

    // --- Composed sensors ---
    OccupancyDetector occupancyDetector;
    ManeuverMotionSensor maneuverMotionSensor;
    RemoteReservationReceiver remoteReservationReceiver;

    // --- Composed actuators ---
    ServoMotor bollardServo;
    StatusRgbLed statusLed;
    CharacterLcdDisplay statusDisplay;

    // --- Internal state ---
    ParkingSpotState currentState;
    unsigned long occupancyStartTimestamp;
    unsigned long displayedOccupancyMinutes;
    bool overtimeBlinkPhaseOn;
    bool maximumTimeExceededNotified;
    ParkingTrackingRecord::OperationCounters operationCounters;

    // --- Event handlers (one per event type) ---
    void handleDistanceUpdate();
    void handleMotionDetected();
    void handleReservationRequested();
    void handleReservationCancelled();
    void handleClockTick();

    // --- State machine helpers ---
    /**
     * @brief Changes the current state and refreshes every actuator.
     * @param newState Target state.
     */
    void transitionTo(ParkingSpotState newState);

    /**
     * @brief Sets bollard, LED and display according to the current state.
     */
    void applyStateOutputs();

    /**
     * @brief Returns to FREE or BUSY depending on the current occupancy reading.
     */
    void resumeOccupancyDetection();

    // --- Actuator helpers ---
    void moveBollard(int angleInDegrees);
    void renderDisplay();

    // --- Query helpers ---
    unsigned long getOccupancyMinutes() const;
    bool isOccupancyActive() const;
    static const char* getStateLabel(ParkingSpotState state);

    // --- Tracking helpers ---
    void publishTrackingRecord(const char* reason);
    void printDeviceInformation() const;
};

#endif // SMART_PARKING_SPOT_DEVICE_H
