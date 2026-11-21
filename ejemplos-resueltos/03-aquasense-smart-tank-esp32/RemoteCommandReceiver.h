/*
  RemoteCommandReceiver.h
  Receives the remote commands of the mobile app. In Wokwi they are typed in
  the Serial Monitor: OPEN, CLOSE or AUTO.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef REMOTE_COMMAND_RECEIVER_H
#define REMOTE_COMMAND_RECEIVER_H

#include "ModestIoT.h"

class RemoteCommandReceiver : public Sensor {
private:
  static const int NO_PIN = -1;
  String buffer;

protected:
  void processEvent(Event& event) override;

public:
  static const int FORCE_OPEN_EVENT_IDENTIFIER = 40;
  static const int FORCE_CLOSE_EVENT_IDENTIFIER = 41;
  static const int AUTOMATIC_MODE_EVENT_IDENTIFIER = 42;

  explicit RemoteCommandReceiver(EventHandler* parentHandler = nullptr) : Sensor(NO_PIN, parentHandler) {}
};

#endif
