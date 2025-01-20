// myLCD.cpp


#include "Arduino.h"
#include "Wire.h"
#include "myLCD.h"
#include "myCli.h"


char lcdBuf[LCDCOLS + 1];


LiquidCrystal_I2C lcd(LCDADDRESS, LCDCOLS, LCDROWS);


bool lcdSetup(void) {
  // Wire.begin() must have run before
  lcd.init();
  lcd.setBacklight(true);
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(0, 1);
  lcd.print("");
  if (LCDROWS == 4) {
    lcd.setCursor(0, 2);
    lcd.print("");
    lcd.setCursor(0, 3);
    lcd.print("");
  }
  return true;
}


bool lcdShow(uint8_t line, const char* text) {
  // print text onto specific line on the display

  line %= LCDROWS;
  uint8_t lenText = strlen(text);

  if (LCDCOLS == 16) sprintf(lcdBuf, "%-16.16s", text);
  if (LCDCOLS == 20) sprintf(lcdBuf, "%-20.20s", text);
  lcd.setCursor(0, line);
  lcd.print(lcdBuf);

  return true;
}
