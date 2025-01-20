#pragma once

// mySerial.h


#include "Arduino.h"


#define P1LINELEN 64
#define P1NOLINES 40


extern HardwareSerial mySerial0;


struct sline { uint8_t datapos1; uint8_t datapos2; char tekst[P1LINELEN]; };
static sline P1buf[P1NOLINES];
struct tech2func { uint8_t index; String techName; String funcName; uint8_t nFields; };
extern uint8_t nWeb;


bool serSetup(void);
bool serLoop(void);
tech2func P1tech2func(String); 