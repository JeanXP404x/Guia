/*
  SerialCommandReceiver.cpp
  Reads characters without blocking; a line is complete on Enter. Text is upper-cased.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "SerialCommandReceiver.h"

SerialCommandReceiver::SerialCommandReceiver(EventHandler* parentHandler)
  : Sensor(NO_PIN, parentHandler), buffer{}, length(0), lastCommand{} {}

void SerialCommandReceiver::processEvent(Event& event) {
  event.identifier = SUPPRESSED_EVENT_IDENTIFIER;
  while (Serial.available() > 0) {
    char character = Serial.read();
    if (character == '\n' || character == '\r') {
      if (length == 0) continue;
      buffer[length] = '\0';
      strcpy(lastCommand, buffer);
      length = 0;
      event = Event(COMMAND_RECEIVED_EVENT_IDENTIFIER);
      return;
    }
    if (length < MAXIMUM_LENGTH) buffer[length++] = toupper(character);
  }
}
