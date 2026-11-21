/*
  LcdDisplay.h
  LCD 2004 (20x4, I2C) actuator. Lines are buffered and written on UPDATE_TEXT.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "ModestIoT.h"
#include <LiquidCrystal_I2C.h>

class LcdDisplay : public Actuator {
public:
  static const uint8_t COLUMNS = 20;
  static const uint8_t ROWS = 4;
  static const int UPDATE_TEXT_COMMAND_IDENTIFIER = 200;

  LcdDisplay(uint8_t i2cAddress, CommandHandler* parentHandler = nullptr);
  void begin();
  void setLine(uint8_t row, const char* text);

protected:
  void executeCommand(Command command) override;

private:
  LiquidCrystal_I2C screen;
  char lines[ROWS][COLUMNS + 1];
};

#endif
