// myRelay.cpp

// Handles relay data and statuses


#include <Arduino.h>
#include "myCli.h"
#include "myRelay.h"


uint8_t nRelayPins;
const int relayPins[]   = { 2, 3, };           // E15: GPIO numbers 2, 3 = pins 1, 2
bool relayAState[]      = { false, false, };   // E15
//bool relayAState[]      = { true, true, };     // E15
//bool relayAState[]      = { true, false, };    // E15
bool relayMState[]      = { false, false, };   // E15
char relayMode[]        = { 'I', 'I', };       // valid: 'A' (automatic) and 'M' (manual)
uint32_t relayMExpire[] = { 0x7FFFFFFF, 0x7FFFFFFF, };   // 'M' => 'A' fallback time in seconds since boot




bool relSetup(void) {
  nRelayPins = sizeof(relayPins) / sizeof(relayPins[0]);
  for (uint8_t iP = 0; iP < nRelayPins; iP++) {
    pinMode(relayPins[iP], OUTPUT);
    digitalWrite(relayPins[iP], R0);   // set all to off
  }

  return true;
}