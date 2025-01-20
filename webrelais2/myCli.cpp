// myCli.cpp

// handle CLI commands


#include <Arduino.h>
#include "myCli.h"
#include "myWlan.h"


char cliBuf[BUFLEN];
char rxmsg[MSGLEN + 1];
char txmsg[MSGLEN + 1];
uint8_t lDeb = 1;




void cliSetup(void) {
  Serial.begin(57600);
  delay(1500);
  if (lDeb > 0) {
    snprintf(cliBuf, sizeof(cliBuf), "\n\nclsu%d\tcliSetup done\n", __LINE__);
    Serial.print(cliBuf);
  }
}


// Receive commands from Serial command line, split in separate commands, handle each command and show results
void cliLoop(void) {
  if (cliSerial(rxmsg)) {
    if (strlen(rxmsg) > 0) {
      cliSplit(rxmsg, txmsg);
    }
  }

  if (strlen(txmsg)) {
    Serial.println(txmsg);
    txmsg[0] = 0;
  }
}




// get CLI commands from Serial and execute them:
bool cliSerial(char* rxmsg) {
  char c;
  int myI = 0;
  bool myStop = false, sereceived = false;

  // Read commands string from Serial
  while (Serial.available() && !myStop) {
    c = Serial.read();
    if (c == '\n') {
      myStop = true;
    } else {
      rxmsg[myI++] = c;
    }
    if (myI >= MSGLEN)
      myStop = true;
    sereceived = true;
    delay(10);  // to allow for next char
  }
  rxmsg[myI] = 0;

  return sereceived;
}


// split command line in rxmsg separated by ';' into single commands:
void cliSplit(char* rxmsg, char* txmsg) {
  txmsg[0] = 0;

  int myC = 0;
  char myCmd[MSGLEN + 1];
  for (uint8_t myI = 0; myI < strlen(rxmsg); myI++) {
    if (rxmsg[myI] == ';' || rxmsg[myI] == 0 || rxmsg[myI] == '\n') {
      myCmd[myC] = 0;
      myC = 0;
      if (myCmd[0] != 0) {
        cliExec(myCmd, txmsg);  // writes and serialprints txmsg
      }
    } else {
      myCmd[myC] = rxmsg[myI];
      myC++;
    }
  }

  rxmsg[0] = 0;
}


// execute a single CLI command:
void cliExec(char* rxmsg, char* txmsg) {
  if (strstart(rxmsg, "de")) {  // de; de=0; de=1;
    cliDeb(rxmsg, txmsg);
  } else if (strstart(rxmsg, "id")) {  // "id;"
    cliId(rxmsg, txmsg);
  } else if (strstart(rxmsg, "wl")) {  // scan: wl=s;   connect: wl=c;
    cliWlan(rxmsg, txmsg);
  } else {
    snprintf(cliBuf, sizeof(cliBuf), "cli:%d\tE: '%s'", __LINE__, rxmsg);
    Serial.println(cliBuf);
  }
  //Serial.printf("-%s\n", txmsg);
  //txmsg[0] = 0;
}


// Function: set debug level;   usage: de;  de=0;   de=1;
void cliDeb(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    //snprintf(cliBuf, sizeof(cliBuf), "cli:%d\tdebug=%d", __LINE__, lDeb);
    sprintf(txmsg, "de=%d;", lDeb);
  } else {
    int8_t result = -2;
    result = sscanf(&rxmsg[3], "%d", &lDeb);
    if (result < 1) {
      snprintf(cliBuf, sizeof(cliBuf), "cli:%d\tE: set debug mislukt: '%s'", __LINE__, &rxmsg[3]);
      sprintf(txmsg, "de=E;");
    } else {
      //snprintf(cliBuf, sizeof(cliBuf), "cli:%d\tdebug=>%d", __LINE__, lDeb);
      sprintf(txmsg, "de=%d;", lDeb);
    }
  }

  Serial.println(cliBuf);
}


// Function: show ID;   usage: id;
void cliId(char* rxmsg, char* txmsg) {
  sprintf(txmsg, "id=E%02d, SEEED XIAO ESP32C3 with 2 relays;", NODE);
}


// delay until the next multiple of <period> ms
void delayLoop(uint32_t period) {
  uint32_t wait = millis() % period;
  delay(period - wait);
}
