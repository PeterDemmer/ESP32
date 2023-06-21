// myIR.cpp

// handle IR commands (station, volume)


#include <Arduino.h>
#define LED_BUILTIN 2
#include "IRremote.h"
#include "myCli.h"
#include "myIR.h"
#include "myPlayer.h"
#include "myStations.h"


// supported IR comands:
struct IRcommand {
  uint8_t irCommand;
  String sIcon;
  char cType;
  uint8_t kCommand;
};
const IRcommand irCommands[] = {
  { 0x01, "V+",  'v', UP },
  { 0x09, "V-",  'v', DN },
  { 0x06, "|<<", 'c', UP },
  { 0x04, ">>|", 'c', DN },
  { 0x0C, "0",   'c', 0  },
  { 0x10, "1",   'c', 1  },
  { 0x11, "2",   'c', 2  },
  { 0x12, "3",   'c', 3  },
  { 0x14, "4",   'c', 4  },
  { 0x15, "5",   'c', 5  },
  { 0x16, "6",   'c', 6  },
  { 0x18, "7",   'c', 7  },
  { 0x19, "8",   'c', 8  },
  { 0x1A, "9",   'c', 9  },
};
const uint8_t noIRcommands = sizeof(irCommands) / sizeof(IRcommand);


IRrecv irRecv(IRrecvPin, LED_BUILTIN);   // InfraRed receiver for volume and channel commands
decode_results results;                  // InfraRed receive results




void irSetup(void) {
  pinMode(IRrecvPin, INPUT_PULLUP);
  irRecv.enableIRIn();
}




void irLoop(void) {
  if (!irRecv.decode()) {
    if (lDeb > 2) {
      snprintf(cliBuf, sizeof(cliBuf), "irlp%d\tIR nothing", __LINE__);
      Serial.println(cliBuf);
    }
    return;
  }

  uint16_t addr = irRecv.decodedIRData.address;
  uint16_t command = irRecv.decodedIRData.command;
  uint8_t flags = irRecv.decodedIRData.flags;
  if (lDeb > 1) {
    snprintf(cliBuf, sizeof(cliBuf), "irlp%d\tIR received: addr=0x%04X command=0x%02X flags=0x%02X", __LINE__, addr, command, flags);
    Serial.println(cliBuf);
  }
  irRecv.resume();
  //irRecv.initDecodedIRData();

  if (flags & IRDATA_FLAGS_IS_REPEAT) {
    if (lDeb > 1) {
      snprintf(cliBuf, sizeof(cliBuf), "irlp%d\tIR is repeat", __LINE__);
      Serial.println(cliBuf);
    }
    return;
  }

  if (flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
    return;
  }

  if (flags & IRDATA_FLAGS_PARITY_FAILED) {
    return;
  }

  if (addr == 0x0000) {
    return;
  }

  if (addr != 0xBF00) {
    if (lDeb > 1) {
      snprintf(cliBuf, sizeof(cliBuf), "irlp%d\tIR other addr=0x%04X", __LINE__, addr);
      Serial.println(cliBuf);
    }
    return;
  }

  irCmd2act(command);

  return;
}


// from IR-command set volume and station:
void irCmd2act(uint8_t command) {
  for (uint8_t iCmd = 0; iCmd < noIRcommands; iCmd++) {
    if (irCommands[iCmd].irCommand == command) {
      if (irCommands[iCmd].cType == 'v') {
        plSetVolume(irCommands[iCmd].kCommand);
      } else if (irCommands[iCmd].cType == 'c') {
        plSetStation(irCommands[iCmd].kCommand);
      };

      if (lDeb > 0) {
        snprintf(cliBuf, sizeof(cliBuf), "irca%d\tstation=%d volume=%d", __LINE__, kStation, volume);
        Serial.println(cliBuf);
      }

      return;
    }
  }

  if (lDeb > 0) {
    snprintf(cliBuf, sizeof(cliBuf), "irca%d\tunknown command 0x%02X", __LINE__, command);
    Serial.println(cliBuf);
  }
  return;
}
