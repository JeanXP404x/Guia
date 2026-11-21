/*
  Led.h
  Single color LED (connect it with a 220 ohm resistor).

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef LED_H
#define LED_H

#include "ModestIoT.h"

class Led : public Actuator {
private:
  bool turnedOn;

protected:
  void executeCommand(Command command) override;

public:
  static const int TURN_ON_COMMAND_IDENTIFIER = 100;
  static const int TURN_OFF_COMMAND_IDENTIFIER = 101;
  static const int TOGGLE_COMMAND_IDENTIFIER = 102;

  Led(int signalPin, CommandHandler* parentHandler = nullptr);
  void turn(bool on) { handle(Command(on ? TURN_ON_COMMAND_IDENTIFIER : TURN_OFF_COMMAND_IDENTIFIER)); }
  bool isOn() const { return turnedOn; }
};

#endif
