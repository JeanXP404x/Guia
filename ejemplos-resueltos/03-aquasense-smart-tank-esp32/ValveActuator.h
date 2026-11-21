/*
  ValveActuator.h
  Servo motor coupled to the ball valve that controls the water flow.

  Author: AquaSense Tech - Smart Tank team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef VALVE_ACTUATOR_H
#define VALVE_ACTUATOR_H

#include "ModestIoT.h"
#include <ESP32Servo.h>

class ValveActuator : public Actuator {
private:
  static const int CLOSED_ANGLE = 0;
  static const int OPEN_ANGLE = 90;
  Servo servo;
  bool open;

protected:
  void executeCommand(Command command) override;

public:
  static const int OPEN_COMMAND_IDENTIFIER = 100;
  static const int CLOSE_COMMAND_IDENTIFIER = 101;

  ValveActuator(int signalPin, CommandHandler* parentHandler = nullptr);
  void begin();
  bool isOpen() const { return open; }
};

#endif
