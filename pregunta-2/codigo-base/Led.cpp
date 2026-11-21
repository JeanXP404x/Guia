/*
  Led.cpp
  ON, OFF and TOGGLE (TOGGLE is useful for blinking alerts).

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "Led.h"

Led::Led(int signalPin, CommandHandler* parentHandler) : Actuator(signalPin, parentHandler), turnedOn(false) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Led::executeCommand(Command command) {
  if (command.identifier == TURN_ON_COMMAND_IDENTIFIER) turnedOn = true;
  else if (command.identifier == TURN_OFF_COMMAND_IDENTIFIER) turnedOn = false;
  else if (command.identifier == TOGGLE_COMMAND_IDENTIFIER) turnedOn = !turnedOn;
  else return;
  digitalWrite(pin, turnedOn ? HIGH : LOW);
}
