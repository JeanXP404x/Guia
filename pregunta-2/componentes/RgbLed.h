/*
  RgbLed.h
  Common cathode RGB LED (COM to GND, one 220 ohm resistor per color).

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef RGB_LED_H
#define RGB_LED_H

#include "ModestIoT.h"

class RgbLed : public Actuator {
private:
  int greenPin;
  int bluePin;
  void write(bool red, bool green, bool blue) const;

protected:
  void executeCommand(Command command) override;

public:
  static const int SHOW_RED_COMMAND_IDENTIFIER = 110;
  static const int SHOW_GREEN_COMMAND_IDENTIFIER = 111;
  static const int SHOW_BLUE_COMMAND_IDENTIFIER = 112;
  static const int TURN_OFF_COMMAND_IDENTIFIER = 113;

  RgbLed(int redPin, int greenPin, int bluePin, CommandHandler* parentHandler = nullptr);
};

#endif
