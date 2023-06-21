#pragma once


// myWiFi.h


extern const char ssid[];
extern const char password[];
extern uint8_t iNet;
extern String sSSID;


extern bool   wlanSetup(void);
extern void   wlanLoop(void);
extern void   cliWlan(char*, char*);
extern bool   wlanStatus(void);
extern String wlanScan(void);
extern bool   wlanConnect(char*, String);
extern String wlanSsid2pass(String);
