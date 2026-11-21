#ifndef REMOTE_RESERVATION_RECEIVER_H
#define REMOTE_RESERVATION_RECEIVER_H

/**
 * @file RemoteReservationReceiver.h
 * @brief Declares the RemoteReservationReceiver, the device inbound channel for app commands.
 *
 * In the Wokwi prototype the remote reservations sent from the mobile app
 * (through Libelium Cloud) are simulated with text commands typed in the
 * Serial Monitor: RESERVE, CANCEL and STATUS.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>

/**
 * @brief Logical sensor that polls the serial channel and converts commands into events.
 */
class RemoteReservationReceiver : public Sensor {
private:
    static constexpr int    NO_PHYSICAL_PIN                    = -1;  ///< Logical sensor without GPIO.
    static constexpr int    PROPAGATION_SUPPRESSION_IDENTIFIER = -1;  ///< Stops event propagation.
    static constexpr size_t MAXIMUM_COMMAND_LENGTH             = 16;  ///< Max characters per command.

    static constexpr const char* RESERVE_COMMAND_TEXT = "RESERVE"; ///< Remote reservation command.
    static constexpr const char* CANCEL_COMMAND_TEXT  = "CANCEL";  ///< Reservation cancellation command.
    static constexpr const char* STATUS_COMMAND_TEXT  = "STATUS";  ///< Tracking report command.

    char commandBuffer[MAXIMUM_COMMAND_LENGTH + 1]; ///< Characters received for the current line.
    size_t commandLength;                           ///< Number of buffered characters.

    /**
     * @brief Reads available characters without blocking.
     * @return `true` when a complete command line is buffered.
     */
    bool readCommandLine();

    /**
     * @brief Maps the buffered command text to an event identifier.
     * @return Event identifier, or PROPAGATION_SUPPRESSION_IDENTIFIER when unknown.
     */
    int resolveCommandEventIdentifier() const;

protected:
    /**
     * @brief Template-method hook: polls the channel and emits the matching event.
     * @param event Event being processed.
     */
    void processEvent(Event& event) override;

public:
    static constexpr int RESERVATION_REQUESTED_EVENT_IDENTIFIER     = 40; ///< Remote reservation received.
    static constexpr int RESERVATION_CANCELLED_EVENT_IDENTIFIER     = 41; ///< Remote cancellation received.
    static constexpr int TRACKING_REPORT_REQUESTED_EVENT_IDENTIFIER = 42; ///< Tracking report requested.

    /**
     * @brief Creates the receiver.
     * @param parentHandler Mediator that receives propagated events.
     */
    explicit RemoteReservationReceiver(EventHandler* parentHandler = nullptr);

    /**
     * @brief Prints the list of supported remote commands to the console.
     */
    void printSupportedCommands() const;
};

#endif // REMOTE_RESERVATION_RECEIVER_H
