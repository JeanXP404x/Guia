/*
  Led.cpp
  Turns the LED on or off according to the received command.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
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
  else return;
  digitalWrite(pin, turnedOn ? HIGH : LOW);
}
