#pragma once

// myWebServer.h


#include "Arduino.h"
#include <WebServer.h>


extern WebServer  server;


bool wsSetup(void);
void wsLoop(void);
void handleRoot(void);
void showPage(uint16_t);
uint32_t uptime(void);
