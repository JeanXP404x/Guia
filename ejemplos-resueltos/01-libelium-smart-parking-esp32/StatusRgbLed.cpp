/**
 * @file StatusRgbLed.cpp
 * @brief Implements the StatusRgbLed actuator.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include "StatusRgbLed.h"

StatusRgbLed::StatusRgbLed(int redGpioPin, int greenGpioPin, int blueGpioPin, CommandHandler* parentHandler)
    : Actuator(redGpioPin, parentHandler),
      greenGpioPin(greenGpioPin),
      blueGpioPin(blueGpioPin),
      activeCommandIdentifier(TURN_OFF_COMMAND_IDENTIFIER)
{
    pinMode(pin, OUTPUT);
    pinMode(this->greenGpioPin, OUTPUT);
    pinMode(this->blueGpioPin, OUTPUT);
    writeChannels(false, false, false);
}

void StatusRgbLed::writeChannels(bool redOn, bool greenOn, bool blueOn) const {
    digitalWrite(pin, redOn ? HIGH : LOW);
    digitalWrite(greenGpioPin, greenOn ? HIGH : LOW);
    digitalWrite(blueGpioPin, blueOn ? HIGH : LOW);
}

void StatusRgbLed::executeCommand(Command command) {
    if (command == SHOW_GREEN_COMMAND) {
        writeChannels(false, true, false);
    } else if (command == SHOW_RED_COMMAND) {
        writeChannels(true, false, false);
    } else if (command == SHOW_BLUE_COMMAND) {
        writeChannels(false, false, true);
    } else if (command == TURN_OFF_COMMAND) {
        writeChannels(false, false, false);
    } else {
        return; // Unknown command: keep current color
    }
    activeCommandIdentifier = command.identifier;
}

int StatusRgbLed::getActiveCommandIdentifier() const {
    return activeCommandIdentifier;
}
