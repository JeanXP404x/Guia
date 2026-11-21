/**
 * @file SmartParkingSpotDevice.cpp
 * @brief Implements the SmartParkingSpotDevice state machine and orchestration logic.
 *
 * Operation modes:
 * - FREE:     green LED, bollard lowered, display "FREE".
 * - BUSY:     red LED, display "BUSY" + elapsed minutes (distance <= 50 cm).
 * - OVERTIME: blinking red LED, display "OVERTIME" + minutes (busy and > 96 min).
 * - RESERVED: blue LED, bollard raised, display "RESERVED"; occupancy detection is
 *             disabled until the reservation is cancelled or the PIR detects the
 *             authorized vehicle approaching.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include "SmartParkingSpotDevice.h"

SmartParkingSpotDevice::SmartParkingSpotDevice(const ParkingSpotWiring& wiring)
    : Device(SCHEDULER_TICK_IN_MILLISECONDS, HARDWARE_TIMER_INDEX),
      occupancyDetector(wiring.ultrasonicTriggerPin, wiring.ultrasonicEchoPin, OCCUPANCY_THRESHOLD_IN_CENTIMETERS, this),
      maneuverMotionSensor(wiring.motionSensorPin, this),
      remoteReservationReceiver(this),
      bollardServo(wiring.bollardServoPin, BOLLARD_LOWERED_ANGLE_IN_DEGREES, this),
      statusLed(wiring.ledRedPin, wiring.ledGreenPin, wiring.ledBluePin, this),
      statusDisplay(wiring.displayI2cAddress, DISPLAY_COLUMNS, DISPLAY_ROWS, DISPLAY_BACKLIGHT_ON, this),
      currentState(ParkingSpotState::Free),
      occupancyStartTimestamp(0),
      displayedOccupancyMinutes(0),
      overtimeBlinkPhaseOn(true),
      maximumTimeExceededNotified(false),
      operationCounters{}
{
    // Round-robin polling plan: three sensors plus the device's own clock tick
    appendSensorToScheduler(&occupancyDetector,         Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER);
    appendSensorToScheduler(&maneuverMotionSensor,      Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER);
    appendSensorToScheduler(&remoteReservationReceiver, Sensor::MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER);
    appendSensorToScheduler(this,                       CLOCK_TICK_EVENT_IDENTIFIER);
}

void SmartParkingSpotDevice::start() {
    printDeviceInformation();
    remoteReservationReceiver.printSupportedCommands();

    applyStateOutputs();
    publishTrackingRecord(REASON_BOOT);

    // Started last, so no background worker touches the hardware while it is being initialized
    initializeAsynchronousEngine(TELEMETRY_QUEUE_DEPTH);
}

// ---------------------------------------------------------------------------
// Event dispatching (Mediator + Inversion of Control)
// ---------------------------------------------------------------------------

void SmartParkingSpotDevice::on(Event event) {
    switch (event.identifier) {
        case Sensor::DATA_READ_EVENT_IDENTIFIER:
            if (event.sourceId == occupancyDetector.getSourceIdentifier()) {
                handleDistanceUpdate();
            }
            break;
        case ManeuverMotionSensor::MOTION_DETECTED_EVENT_IDENTIFIER:
            handleMotionDetected();
            break;
        case RemoteReservationReceiver::RESERVATION_REQUESTED_EVENT_IDENTIFIER:
            handleReservationRequested();
            break;
        case RemoteReservationReceiver::RESERVATION_CANCELLED_EVENT_IDENTIFIER:
            handleReservationCancelled();
            break;
        case RemoteReservationReceiver::TRACKING_REPORT_REQUESTED_EVENT_IDENTIFIER:
            publishTrackingRecord(REASON_ON_DEMAND);
            break;
        case CLOCK_TICK_EVENT_IDENTIFIER:
            handleClockTick();
            break;
        default:
            break; // Events not relevant for this device
    }
}

// ---------------------------------------------------------------------------
// Event handlers
// ---------------------------------------------------------------------------

void SmartParkingSpotDevice::handleDistanceUpdate() {
    const bool occupied = occupancyDetector.isOccupied();
    Serial.printf("[Occupancy] Distance: %.1f cm (%s)\n",
                  occupancyDetector.getDistanceInCentimeters(), occupied ? "vehicle" : "no vehicle");

    switch (currentState) {
        case ParkingSpotState::Reserved:
            Serial.println("[Occupancy] Spot reserved: occupancy detection disabled.");
            break;
        case ParkingSpotState::Free:
            if (occupied) {
                operationCounters.occupancySessions++;
                transitionTo(ParkingSpotState::Busy);
            }
            break;
        case ParkingSpotState::Busy:
        case ParkingSpotState::Overtime:
            if (!occupied) {
                Serial.printf("[Occupancy] Vehicle left after %lu min.\n", getOccupancyMinutes());
                transitionTo(ParkingSpotState::Free);
            }
            break;
    }
}

void SmartParkingSpotDevice::handleMotionDetected() {
    operationCounters.motionDetections++;

    if (currentState == ParkingSpotState::Reserved) {
        Serial.println("[Motion] Authorized vehicle approaching: lowering bollard.");
        operationCounters.reservationsFulfilled++;
        resumeOccupancyDetection();
    } else {
        Serial.printf("[Motion] Parking maneuver detected (state: %s).\n", getStateLabel(currentState));
    }
}

void SmartParkingSpotDevice::handleReservationRequested() {
    switch (currentState) {
        case ParkingSpotState::Free:
            Serial.println("[Remote] Reservation accepted: raising bollard.");
            operationCounters.reservationsAccepted++;
            transitionTo(ParkingSpotState::Reserved);
            break;
        case ParkingSpotState::Reserved:
            Serial.println("[Remote] Reservation ignored: spot is already reserved.");
            break;
        case ParkingSpotState::Busy:
        case ParkingSpotState::Overtime:
            Serial.println("[Remote] Reservation rejected: spot is occupied.");
            operationCounters.reservationsRejected++;
            publishTrackingRecord(REASON_ON_DEMAND);
            break;
    }
}

void SmartParkingSpotDevice::handleReservationCancelled() {
    if (currentState != ParkingSpotState::Reserved) {
        Serial.println("[Remote] Cancellation ignored: there is no active reservation.");
        return;
    }
    Serial.println("[Remote] Reservation cancelled: lowering bollard.");
    operationCounters.reservationsCancelled++;
    resumeOccupancyDetection();
}

void SmartParkingSpotDevice::handleClockTick() {
    if (!isOccupancyActive()) {
        return;
    }

    const unsigned long occupancyMinutes = getOccupancyMinutes();

    if (currentState == ParkingSpotState::Busy && occupancyMinutes > OVERTIME_THRESHOLD_IN_MINUTES) {
        Serial.printf("[Alert] OVERTIME: %lu min exceeds %lu min (%lu%% of %lu min).\n",
                      occupancyMinutes, OVERTIME_THRESHOLD_IN_MINUTES,
                      OVERTIME_ALERT_PERCENTAGE, MAXIMUM_PARKING_TIME_IN_MINUTES);
        operationCounters.overtimeAlerts++;
        transitionTo(ParkingSpotState::Overtime);
        publishTrackingRecord(REASON_OVERTIME_ALERT);
        return;
    }

    if (currentState == ParkingSpotState::Overtime) {
        // Intermittent red LED until the vehicle leaves
        overtimeBlinkPhaseOn = !overtimeBlinkPhaseOn;
        statusLed.handle(overtimeBlinkPhaseOn ? StatusRgbLed::SHOW_RED_COMMAND : StatusRgbLed::TURN_OFF_COMMAND);

        if (occupancyMinutes > MAXIMUM_PARKING_TIME_IN_MINUTES && !maximumTimeExceededNotified) {
            Serial.printf("[Alert] Maximum parking time (%lu min) exceeded.\n", MAXIMUM_PARKING_TIME_IN_MINUTES);
            maximumTimeExceededNotified = true;
        }
    }

    if (occupancyMinutes != displayedOccupancyMinutes) {
        renderDisplay();
    }
}

// ---------------------------------------------------------------------------
// State machine helpers
// ---------------------------------------------------------------------------

void SmartParkingSpotDevice::transitionTo(ParkingSpotState newState) {
    const ParkingSpotState previousState = currentState;
    currentState = newState;

    if (newState == ParkingSpotState::Busy && previousState != ParkingSpotState::Busy) {
        occupancyStartTimestamp = millis();
        maximumTimeExceededNotified = false;
    }
    if (newState == ParkingSpotState::Overtime) {
        overtimeBlinkPhaseOn = true;
    }

    Serial.printf("[State] %s -> %s\n", getStateLabel(previousState), getStateLabel(newState));
    applyStateOutputs();
    publishTrackingRecord(REASON_STATE_CHANGE);
}

void SmartParkingSpotDevice::applyStateOutputs() {
    switch (currentState) {
        case ParkingSpotState::Free:
            moveBollard(BOLLARD_LOWERED_ANGLE_IN_DEGREES);
            statusLed.handle(StatusRgbLed::SHOW_GREEN_COMMAND);
            break;
        case ParkingSpotState::Busy:
        case ParkingSpotState::Overtime:
            moveBollard(BOLLARD_LOWERED_ANGLE_IN_DEGREES);
            statusLed.handle(StatusRgbLed::SHOW_RED_COMMAND);
            break;
        case ParkingSpotState::Reserved:
            moveBollard(BOLLARD_RAISED_ANGLE_IN_DEGREES);
            statusLed.handle(StatusRgbLed::SHOW_BLUE_COMMAND);
            break;
    }
    renderDisplay();
}

void SmartParkingSpotDevice::resumeOccupancyDetection() {
    if (occupancyDetector.isOccupied()) {
        operationCounters.occupancySessions++;
        transitionTo(ParkingSpotState::Busy);
    } else {
        transitionTo(ParkingSpotState::Free);
    }
}

// ---------------------------------------------------------------------------
// Actuator helpers
// ---------------------------------------------------------------------------

void SmartParkingSpotDevice::moveBollard(int angleInDegrees) {
    bollardServo.setTargetAngleBufferInDegrees(angleInDegrees);
    bollardServo.handle(ServoMotor::DRIVE_TO_BUFFERED_ANGLE_COMMAND);
}

void SmartParkingSpotDevice::renderDisplay() {
    displayedOccupancyMinutes = getOccupancyMinutes();

    char timeLineBuffer[DISPLAY_COLUMNS + 1];
    snprintf(timeLineBuffer, sizeof(timeLineBuffer), TIME_LINE_FORMAT, displayedOccupancyMinutes);

    statusDisplay.setLineBuffer(DISPLAY_STATE_ROW, getStateLabel(currentState));
    statusDisplay.setLineBuffer(DISPLAY_TIME_ROW, timeLineBuffer);
    statusDisplay.handle(CharacterLcdDisplay::UPDATE_TEXT_COMMAND);
}

// ---------------------------------------------------------------------------
// Query helpers
// ---------------------------------------------------------------------------

bool SmartParkingSpotDevice::isOccupancyActive() const {
    return currentState == ParkingSpotState::Busy || currentState == ParkingSpotState::Overtime;
}

unsigned long SmartParkingSpotDevice::getOccupancyMinutes() const {
    if (!isOccupancyActive()) {
        return 0;
    }
    return (millis() - occupancyStartTimestamp) / OCCUPANCY_MINUTE_DURATION_IN_MILLISECONDS;
}

const char* SmartParkingSpotDevice::getStateLabel(ParkingSpotState state) {
    switch (state) {
        case ParkingSpotState::Free:     return FREE_LABEL;
        case ParkingSpotState::Busy:     return BUSY_LABEL;
        case ParkingSpotState::Overtime: return OVERTIME_LABEL;
        case ParkingSpotState::Reserved: return RESERVED_LABEL;
    }
    return FREE_LABEL;
}

// ---------------------------------------------------------------------------
// Tracking (Device Maker) helpers
// ---------------------------------------------------------------------------

void SmartParkingSpotDevice::publishTrackingRecord(const char* reason) {
    TelemetryPackage* trackingRecord = new ParkingTrackingRecord(
        DEVICE_IDENTIFIER, reason, getStateLabel(currentState),
        occupancyDetector.getDistanceInCentimeters(), getOccupancyMinutes(),
        millis() / MILLISECONDS_PER_SECOND, operationCounters);

    // Before the engine starts (or when the queue is full) publish synchronously
    if (!enqueueTelemetryPayload(&trackingRecord)) {
        processQueuedTelemetryData(trackingRecord);
        delete trackingRecord;
    }
}

void SmartParkingSpotDevice::processQueuedTelemetryData(const TelemetryPackage* rawQueueItemPayload) const {
    if (rawQueueItemPayload == nullptr) {
        return;
    }
    Serial.printf("[Tracking -> Libelium Cloud] %s\n", rawQueueItemPayload->serializeAsString().c_str());
}

void SmartParkingSpotDevice::printDeviceInformation() const {
    Serial.println("==================================================");
    Serial.printf("Company   : %s (%s)\n", COMPANY_NAME, COMPANY_WEBSITE);
    Serial.printf("Line      : %s\n", COMPANY_PRODUCT_LINE);
    Serial.printf("Device    : %s - S/N %s - FW %s\n", DEVICE_MODEL, DEVICE_IDENTIFIER, FIRMWARE_VERSION);
    Serial.printf("Developer : %s (%s)\n", DEVELOPER_FULL_NAME, DEVELOPER_ROLE);
    Serial.println("--------------------------------------------------");
    Serial.printf("Rules     : busy when distance <= %.0f cm, overtime after %lu min (%lu%% of %lu min)\n",
                  OCCUPANCY_THRESHOLD_IN_CENTIMETERS, OVERTIME_THRESHOLD_IN_MINUTES,
                  OVERTIME_ALERT_PERCENTAGE, MAXIMUM_PARKING_TIME_IN_MINUTES);
    Serial.printf("Demo mode : 1 occupancy minute = %lu ms\n", OCCUPANCY_MINUTE_DURATION_IN_MILLISECONDS);
    Serial.println("==================================================");
}
