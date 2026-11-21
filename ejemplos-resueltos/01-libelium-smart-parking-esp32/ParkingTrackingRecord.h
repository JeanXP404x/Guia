#ifndef PARKING_TRACKING_RECORD_H
#define PARKING_TRACKING_RECORD_H

/**
 * @file ParkingTrackingRecord.h
 * @brief Declares the ParkingTrackingRecord telemetry package (operation tracking for the Device Maker).
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>

/**
 * @brief Immutable snapshot of the device operation, serialized as JSON for Libelium Cloud.
 */
class ParkingTrackingRecord : public TelemetryPackage {
public:
    /**
     * @brief Operation counters accumulated since boot.
     */
    struct OperationCounters {
        unsigned long occupancySessions;     ///< Vehicles that occupied the spot.
        unsigned long overtimeAlerts;        ///< Overtime alerts raised.
        unsigned long reservationsAccepted;  ///< Remote reservations accepted.
        unsigned long reservationsRejected;  ///< Remote reservations rejected (spot busy/reserved).
        unsigned long reservationsCancelled; ///< Remote reservations cancelled.
        unsigned long reservationsFulfilled; ///< Authorized vehicles that arrived.
        unsigned long motionDetections;      ///< PIR maneuver detections.
    };

    /**
     * @brief Creates the snapshot.
     * @param deviceIdentifier Device serial / model identifier.
     * @param triggerReason Reason that produced the record (state change, on demand...).
     * @param stateLabel Current parking spot state label.
     * @param distanceInCentimeters Last measured distance.
     * @param occupancyMinutes Current occupancy time in minutes.
     * @param uptimeInSeconds Seconds since boot.
     * @param counters Operation counters.
     */
    ParkingTrackingRecord(const char* deviceIdentifier, const char* triggerReason, const char* stateLabel,
                          float distanceInCentimeters, unsigned long occupancyMinutes,
                          unsigned long uptimeInSeconds, const OperationCounters& counters)
        : deviceIdentifier(deviceIdentifier), triggerReason(triggerReason), stateLabel(stateLabel),
          distanceInCentimeters(distanceInCentimeters), occupancyMinutes(occupancyMinutes),
          uptimeInSeconds(uptimeInSeconds), counters(counters) {}

    /**
     * @brief Serializes the record into the given JSON document.
     * @param serializationDestination Destination JSON document.
     */
    void serialize(JsonDocument& serializationDestination) const override {
        serializationDestination["deviceId"]        = deviceIdentifier;
        serializationDestination["reason"]          = triggerReason;
        serializationDestination["state"]           = stateLabel;
        serializationDestination["distanceCm"]      = distanceInCentimeters;
        serializationDestination["occupancyMin"]    = occupancyMinutes;
        serializationDestination["uptimeSec"]       = uptimeInSeconds;
        serializationDestination["sessions"]        = counters.occupancySessions;
        serializationDestination["overtimeAlerts"]  = counters.overtimeAlerts;
        serializationDestination["resAccepted"]     = counters.reservationsAccepted;
        serializationDestination["resRejected"]     = counters.reservationsRejected;
        serializationDestination["resCancelled"]    = counters.reservationsCancelled;
        serializationDestination["resFulfilled"]    = counters.reservationsFulfilled;
        serializationDestination["motionEvents"]    = counters.motionDetections;
    }

private:
    const char* deviceIdentifier;    ///< Device identifier (static string).
    const char* triggerReason;       ///< Record reason (static string).
    const char* stateLabel;          ///< State label (static string).
    float distanceInCentimeters;     ///< Last measured distance.
    unsigned long occupancyMinutes;  ///< Occupancy time in minutes.
    unsigned long uptimeInSeconds;   ///< Seconds since boot.
    OperationCounters counters;      ///< Counters snapshot.
};

#endif // PARKING_TRACKING_RECORD_H
