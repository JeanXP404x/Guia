/*
  ServoActuator.h
  Servo motor (valve, barrier, bollard, door...). Library: "ESP32Servo" on ESP32,
  on Arduino Uno change the include to <Servo.h> (built in).

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef SERVO_ACTUATOR_H
#define SERVO_ACTUATOR_H

#include "ModestIoT.h"
#include <ESP32Servo.h>

class ServoActuator : public Actuator {
private:
  static const int MINIMUM_ANGLE = 0;
  static const int MAXIMUM_ANGLE = 180;
  Servo servo;
  int angle;
  int targetAngle;

protected:
  void executeCommand(Command command) override;

public:
  static const int MOVE_TO_TARGET_COMMAND_IDENTIFIER = 120;

  ServoActuator(int signalPin, int initialAngle = 0, CommandHandler* parentHandler = nullptr);
  void begin();
  void moveTo(int newAngle);
  int getAngle() const { return angle; }
};

#endif
