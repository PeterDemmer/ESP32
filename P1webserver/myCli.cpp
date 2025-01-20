// myCli.cpp

// handle CLI commands


#include <Arduino.h>
#include "myCli.h"
//#include "myWire.h"
#include "myLCD.h"
#include "myWlan.h"


#define NODE 16


char cliBuf[BUFLEN];
char rxmsg[MSGLEN + 1];
char txmsg[MSGLEN + 1];
uint8_t lDeb = 1;


void cliSetup(void) {
  Serial.begin(57600);
  delay(1500);
  Serial.println("\n");  // 2 empty lines
}


// Receive commands from Serial command line, split in separate commands, handle each command and show results
void cliLoop(void) {
  if (cliSerial(rxmsg)) {
    if (rxmsg[0]) {
      cliSplit(rxmsg, txmsg);
    }
  }

  if (strlen(txmsg)) {
    Serial.print("-");
    Serial.println(txmsg);
    txmsg[0] = 0;
  }
}


// get CLI commands from Serial and execute them:
bool cliSerial(char *rxmsg) {
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
    delay(10);   // to allow for next char
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
        cliExec(myCmd, txmsg);   // writes and serialprints txmsg
      }
    } else {
      myCmd[myC] = rxmsg[myI];
      myC++;
    }
  }

  rxmsg[0] = 0;
}


// execute a single CLI command:
void cliExec(char *rxmsg, char* txmsg) {
  if (strstart(rxmsg, "de")) {   // de; de=0; de=1;
    cliDeb(rxmsg, txmsg);
  } else if (strstart(rxmsg, "id")) {   // "id;"
    cliWho(rxmsg, txmsg);
  } else if (strstart(rxmsg, "wh")) {   // "who;"
    cliWho(rxmsg, txmsg);
//  } else if (strstart(rxmsg, "wi")) {   // "wire/I2C"
//    cliWire(rxmsg, txmsg);
  } else if (strstart(rxmsg, "wl")) {   // "Wlan/WiFi"
    cliWlan(rxmsg, txmsg);
  } else  {
    snprintf(cliBuf, sizeof(cliBuf), "clex%d\terr: '%s'", __LINE__, rxmsg);
    Serial.println(cliBuf);
  }
}


// Function: set debug level;   usage: de;  de=0;   de=1;
void cliDeb(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    if (lDeb > 1) {
      snprintf(cliBuf, sizeof(cliBuf), "clde%d\tlDeb=%d", __LINE__, lDeb);
      Serial.println(cliBuf);
    }
    sprintf(&txmsg[strlen(txmsg)], "de=%d;", lDeb);
  } else {
    int8_t result = -2;
    result = sscanf(&rxmsg[3], "%d", &lDeb);
    if (result < 1) {
      if (lDeb > 1) {
        snprintf(cliBuf, sizeof(cliBuf), "clde%d\terr: set lDeb failed: '%s'", __LINE__, &rxmsg[3]);
        Serial.println(cliBuf);
      }
      sprintf(&txmsg[strlen(txmsg)], "de=err;");
    } else {
      if (lDeb > 1) {
        snprintf(cliBuf, sizeof(cliBuf), "clde%d\tlDeb=>%d", __LINE__, lDeb);
        Serial.println(cliBuf);
      }
      sprintf(&txmsg[strlen(txmsg)], "de=%d;", lDeb);
    }
  }
}


// Function: show ID;   usage: id;   who;
void cliWho(char* rxmsg, char* txmsg) {
  sprintf(&txmsg[strlen(txmsg)], "id=E%02d, displays received serial data on web interface", NODE);
}
