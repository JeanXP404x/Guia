/*
  ModestIoT.cpp
  Implementation of the Sensor and Actuator template methods.

  Author: Moen, Inc. - Nebia Spa Shower team
  Date: 2026-11-20
  Version: 1.0
*/
#include "ModestIoT.h"

void Sensor::on(Event event) {
  processEvent(event);
  if (event.identifier == SUPPRESSED_EVENT_IDENTIFIER) return;
  if (eventHandler != nullptr) eventHandler->on(event);
}

void Actuator::handle(Command command) {
  executeCommand(command);
  if (commandHandler != nullptr) commandHandler->handle(command);
}
