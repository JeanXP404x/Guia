/*
  LcdDisplay.cpp
  Every line is padded with spaces so old text is always overwritten.

  Author: <Company> - <Your name>
  Date: <yyyy-mm-dd>
  Version: 1.0
*/
#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t i2cAddress, uint8_t columns, uint8_t rows, CommandHandler* parentHandler)
  : Actuator(i2cAddress, parentHandler), screen(i2cAddress, columns, rows),
    columns(min(columns, MAXIMUM_COLUMNS)), rows(min(rows, MAXIMUM_ROWS)) {
  for (uint8_t row = 0; row < MAXIMUM_ROWS; row++) setLine(row, "");
}

void LcdDisplay::begin() {
  screen.init();
  screen.backlight();
  screen.clear();
}

void LcdDisplay::setLine(uint8_t row, const char* text) {
  if (row >= rows || text == nullptr) return;
  size_t length = strlen(text);
  for (uint8_t column = 0; column < columns; column++) {
    lines[row][column] = column < length ? text[column] : ' ';
  }
  lines[row][columns] = '\0';
}

void LcdDisplay::executeCommand(Command command) {
  if (command.identifier != UPDATE_TEXT_COMMAND_IDENTIFIER) return;
  for (uint8_t row = 0; row < rows; row++) {
    screen.setCursor(0, row);
    screen.print(lines[row]);
  }
}
