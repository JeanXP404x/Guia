/*
  LcdDisplay.h
  I2C character LCD: LCD 1602 (16x2) or LCD 2004 (20x4). Library: "LiquidCrystal I2C".
  Fill the lines with setLine() and write them all with refresh().

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "ModestIoT.h"
#include <LiquidCrystal_I2C.h>

class LcdDisplay : public Actuator {
public:
  static const uint8_t MAXIMUM_COLUMNS = 20;
  static const uint8_t MAXIMUM_ROWS = 4;
  static const int UPDATE_TEXT_COMMAND_IDENTIFIER = 200;

  LcdDisplay(uint8_t i2cAddress, uint8_t columns, uint8_t rows, CommandHandler* parentHandler = nullptr);
  void begin();
  void setLine(uint8_t row, const char* text);
  void refresh() { handle(Command(UPDATE_TEXT_COMMAND_IDENTIFIER)); }
  uint8_t getColumns() const { return columns; }

protected:
  void executeCommand(Command command) override;

private:
  LiquidCrystal_I2C screen;
  uint8_t columns;
  uint8_t rows;
  char lines[MAXIMUM_ROWS][MAXIMUM_COLUMNS + 1];
};

#endif
