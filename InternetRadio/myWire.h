#pragma once

// myWire.h


#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#define LCDCOLS 20
#define LCDROWS 4


extern LiquidCrystal_I2C lcd;
//extern char lcdBuf[LCDCOLS+1];
extern char disBuf[LCDCOLS+1];


extern bool wireSetup(void);
extern void cliWire(char*, char*);
extern void wireScan(void);
//extern bool wireSend(uint8_t, const char*);
extern bool disShow(uint8_t, const char*);
