/**
 * @file RemoteReservationReceiver.cpp
 * @brief Implements the RemoteReservationReceiver (simulated app -> device channel).
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include "RemoteReservationReceiver.h"
#include <ctype.h>
#include <string.h>

RemoteReservationReceiver::RemoteReservationReceiver(EventHandler* parentHandler)
    : Sensor(NO_PHYSICAL_PIN, parentHandler),
      commandBuffer{},
      commandLength(0) {}

bool RemoteReservationReceiver::readCommandLine() {
    while (Serial.available() > 0) {
        const char receivedCharacter = static_cast<char>(Serial.read());

        if (receivedCharacter == '\n' || receivedCharacter == '\r') {
            if (commandLength > 0) {
                commandBuffer[commandLength] = '\0';
                return true;
            }
            continue; // Ignore empty lines
        }

        if (commandLength < MAXIMUM_COMMAND_LENGTH) {
            commandBuffer[commandLength++] = static_cast<char>(toupper(receivedCharacter));
        }
    }
    return false;
}

int RemoteReservationReceiver::resolveCommandEventIdentifier() const {
    if (strcmp(commandBuffer, RESERVE_COMMAND_TEXT) == 0) return RESERVATION_REQUESTED_EVENT_IDENTIFIER;
    if (strcmp(commandBuffer, CANCEL_COMMAND_TEXT) == 0)  return RESERVATION_CANCELLED_EVENT_IDENTIFIER;
    if (strcmp(commandBuffer, STATUS_COMMAND_TEXT) == 0)  return TRACKING_REPORT_REQUESTED_EVENT_IDENTIFIER;
    return PROPAGATION_SUPPRESSION_IDENTIFIER;
}

void RemoteReservationReceiver::processEvent(Event& event) {
    if (event.identifier != MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER) {
        return;
    }

    if (!readCommandLine()) {
        event.identifier = PROPAGATION_SUPPRESSION_IDENTIFIER;
        return;
    }

    const int resolvedIdentifier = resolveCommandEventIdentifier();
    if (resolvedIdentifier == PROPAGATION_SUPPRESSION_IDENTIFIER) {
        Serial.printf("[Remote] Unknown command '%s'.\n", commandBuffer);
        printSupportedCommands();
    }
    commandLength = 0;
    event = Event(resolvedIdentifier);
}

void RemoteReservationReceiver::printSupportedCommands() const {
    Serial.printf("[Remote] Supported app commands (type in Serial Monitor): %s | %s | %s\n",
                  RESERVE_COMMAND_TEXT, CANCEL_COMMAND_TEXT, STATUS_COMMAND_TEXT);
}
