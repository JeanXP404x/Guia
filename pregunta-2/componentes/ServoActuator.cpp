/*
  ServoActuator.cpp
  The angle is limited to 0-180 degrees.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "ServoActuator.h"

ServoActuator::ServoActuator(int signalPin, int initialAngle, CommandHandler* parentHandler)
  : Actuator(signalPin, parentHandler), angle(initialAngle), targetAngle(initialAngle) {}

void ServoActuator::begin() {
  servo.attach(pin);
  servo.write(angle);
}

void ServoActuator::moveTo(int newAngle) {
  targetAngle = constrain(newAngle, MINIMUM_ANGLE, MAXIMUM_ANGLE);
  handle(Command(MOVE_TO_TARGET_COMMAND_IDENTIFIER));
}

void ServoActuator::executeCommand(Command command) {
  if (command.identifier != MOVE_TO_TARGET_COMMAND_IDENTIFIER) return;
  angle = targetAngle;
  servo.write(angle);
}
