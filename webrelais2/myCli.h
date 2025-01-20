#pragma once

// myCli.h
// handle CLI commands


#define DEBUG_KEEPTIME 2000
#define strstart(string, s) (! strncmp(string, s, strlen(s)))
#define BUFLEN 128
#define MSGLEN 64
#define NODE 15


extern uint8_t lDeb;
extern char cliBuf[BUFLEN];
extern char rxmsg[MSGLEN + 1];
extern char txmsg[MSGLEN + 1];


void cliSetup(void);           // handle CLI commands
void cliLoop(void);            // handle commands over Serial
bool cliSerial(char *);        // read command line from Serial
void cliSplit(char*, char*);   // split command line in rxmsg separated by ';' into single commands
void cliExec(char *, char*);   // execute a single CLI command
void cliDeb(char*, char*);     // set the debug level, 0 = off
void cliId(char*, char*);      // show this controller's functional ID: current sensor
void delayLoop(uint32_t);      // delay until the next multiple of <period> ms
