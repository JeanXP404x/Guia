/*
  RemoteCommandReceiver.cpp
  Reads one line without blocking and converts it into an event.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#include "RemoteCommandReceiver.h"

void RemoteCommandReceiver::processEvent(Event& event) {
  event.identifier = SUPPRESSED_EVENT_IDENTIFIER;

  while (Serial.available() > 0) {
    char character = Serial.read();
    if (character != '\n' && character != '\r') {
      buffer += character;
      continue;
    }
    if (buffer.length() == 0) continue;

    buffer.trim();
    buffer.toUpperCase();
    if (buffer == "OPEN") event = Event(FORCE_OPEN_EVENT_IDENTIFIER);
    else if (buffer == "CLOSE") event = Event(FORCE_CLOSE_EVENT_IDENTIFIER);
    else if (buffer == "AUTO") event = Event(AUTOMATIC_MODE_EVENT_IDENTIFIER);
    else Serial.println("[Remote] Unknown command. Use OPEN, CLOSE or AUTO.");
    buffer = "";
    return;
  }
}
