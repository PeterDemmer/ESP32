#pragma once

// myLCD.h


#include "Arduino.h"
#include "LiquidCrystal_I2C.h"


#define LCDADDRESS 0x27
#define LCDCOLS 16
#define LCDROWS 2
//#define LCDCOLS 20
//#define LCDROWS 4


extern LiquidCrystal_I2C lcd;
extern char lcdBuf[LCDCOLS+1];


extern bool lcdSetup(void);
extern bool lcdShow(uint8_t, const char*);
