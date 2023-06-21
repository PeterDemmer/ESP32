#pragma once

// myPlayer.h


#include "vs1053_ext.h"


#define UP 0xFF
#define EQ 0xFE
#define DN 0xFD


// Digital I/O used
//#define SD_CS        5
#define VS1053_MOSI   23
#define VS1053_MISO   19
#define VS1053_SCK    18
#define VS1053_CS      2
#define VS1053_DCS     4
#define VS1053_DREQ   34   /* was: 36 */

extern VS1053 mp3;


extern uint8_t volume;


bool plSetup(void);
void cliStation(char*, char*);
void cliVolume(char*, char*);
void plSetStation(uint8_t);
void plSetVolume(uint8_t);
