/*
  RgbLed.cpp
  One command per color.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "RgbLed.h"

RgbLed::RgbLed(int redPin, int greenPin, int bluePin, CommandHandler* parentHandler)
  : Actuator(redPin, parentHandler), greenPin(greenPin), bluePin(bluePin) {
  pinMode(pin, OUTPUT);
  pinMode(this->greenPin, OUTPUT);
  pinMode(this->bluePin, OUTPUT);
  write(false, false, false);
}

void RgbLed::write(bool red, bool green, bool blue) const {
  digitalWrite(pin, red ? HIGH : LOW);
  digitalWrite(greenPin, green ? HIGH : LOW);
  digitalWrite(bluePin, blue ? HIGH : LOW);
}

void RgbLed::executeCommand(Command command) {
  switch (command.identifier) {
    case SHOW_RED_COMMAND_IDENTIFIER: write(true, false, false); break;
    case SHOW_GREEN_COMMAND_IDENTIFIER: write(false, true, false); break;
    case SHOW_BLUE_COMMAND_IDENTIFIER: write(false, false, true); break;
    case TURN_OFF_COMMAND_IDENTIFIER: write(false, false, false); break;
  }
}
