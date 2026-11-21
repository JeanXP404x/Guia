/*
  ValveActuator.cpp
  Moves the servo to 90 degrees (open) or 0 degrees (closed).

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "ValveActuator.h"

ValveActuator::ValveActuator(int signalPin, CommandHandler* parentHandler)
  : Actuator(signalPin, parentHandler), open(false) {}

void ValveActuator::begin() {
  servo.attach(pin);
  servo.write(CLOSED_ANGLE);
}

void ValveActuator::executeCommand(Command command) {
  if (command.identifier == OPEN_COMMAND_IDENTIFIER) open = true;
  else if (command.identifier == CLOSE_COMMAND_IDENTIFIER) open = false;
  else return;
  servo.write(open ? OPEN_ANGLE : CLOSED_ANGLE);
}
