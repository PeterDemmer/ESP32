#pragma once

// myWebServer.h


#include "Arduino.h"
#include <WebServer.h>

#define WEBNOLINES 30
//struct wline { String techName = ""; String funcName = ""; String value = ""; };
struct wline { String techName; String funcName; String value; uint32_t lastSeen; };
//static wline webBuf[WEBNOLINES];
extern wline webBuf[WEBNOLINES];

extern WebServer server;


extern bool webSetup(void);
extern void webLoop(void);
extern void handleRoot(void);
