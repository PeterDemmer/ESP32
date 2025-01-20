#pragma once

// myRelais.h


// E15: Relais pins: GPIO 2, 3   (PCB pins 1,2 )
// E16: Relais pins: 8-port parallel I/O pins 10..17 = bits 0..7
// E18: Relais pins: GPIO 10, 9   (PCB pins 11,10 )
// E19: Relais pins: GPIO 10, 9   (PCB pins 11,10 )

#include "Arduino.h"


// translate relay status -> digitalWrite:
#define R0 1
#define R1 0


// There is an A-mode and an M-mode.
// A-mode is intended for automated control, M-mode for manual control
// A-mode is the default.
// The M-mode is activated for a period of time e.g. 60 minutes, after which a fall-back to the A-mode takes place.
extern uint8_t nRelayPins;
extern const int relayPins[];      // GPIO pin numbers
extern bool relayAState[];         // state when in A-mode, false=off true=on
extern bool relayMState[];         // state when in M-mode, false=off true=on
extern char relayMode[];           // 'M' manual, 'A' automatic
extern uint32_t relayMExpire[];    // time until 'M' mode expires


bool relSetup(void);
