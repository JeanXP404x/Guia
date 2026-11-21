/*
  SerialCommandReceiver.h
  Simulates remote commands from the mobile app / cloud: the user types a word
  in the Serial Monitor (for example OPEN, RESERVE, STATUS) and the device
  receives COMMAND_RECEIVED. Read the text with getLastCommand().

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef SERIAL_COMMAND_RECEIVER_H
#define SERIAL_COMMAND_RECEIVER_H

#include "ModestIoT.h"

class SerialCommandReceiver : public Sensor {
private:
  static const int NO_PIN = -1;
  static const uint8_t MAXIMUM_LENGTH = 16;
  char buffer[MAXIMUM_LENGTH + 1];
  uint8_t length;
  char lastCommand[MAXIMUM_LENGTH + 1];

protected:
  void processEvent(Event& event) override;

public:
  static const int COMMAND_RECEIVED_EVENT_IDENTIFIER = 40;

  explicit SerialCommandReceiver(EventHandler* parentHandler = nullptr);
  const char* getLastCommand() const { return lastCommand; }
  bool isCommand(const char* text) const { return strcmp(lastCommand, text) == 0; }
};

#endif
