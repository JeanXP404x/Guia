/*
  LcdDisplay.cpp
  Pads every line to 20 characters so old text is always overwritten.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t i2cAddress, CommandHandler* parentHandler)
  : Actuator(i2cAddress, parentHandler), screen(i2cAddress, COLUMNS, ROWS) {
  for (uint8_t row = 0; row < ROWS; row++) setLine(row, "");
}

void LcdDisplay::begin() {
  screen.init();
  screen.backlight();
  screen.clear();
}

void LcdDisplay::setLine(uint8_t row, const char* text) {
  if (row >= ROWS || text == nullptr) return;
  size_t length = strlen(text);
  for (uint8_t column = 0; column < COLUMNS; column++) {
    lines[row][column] = column < length ? text[column] : ' ';
  }
  lines[row][COLUMNS] = '\0';
}

void LcdDisplay::executeCommand(Command command) {
  if (command.identifier != UPDATE_TEXT_COMMAND_IDENTIFIER) return;
  for (uint8_t row = 0; row < ROWS; row++) {
    screen.setCursor(0, row);
    screen.print(lines[row]);
  }
}
