#pragma once


// myWiFi.h


extern uint8_t iNet;
extern String sSSID;


extern bool   wlanSetup(void);
extern void   wlanLoop(void);
extern void   cliWlan(char*, char*);
extern bool   wlanStatus(void);
extern String wlanScan(void);              // scan all WLANs and try to log in into the strongest known
extern bool   wlanConnect(char*, char*);   // try to connect to a WLAN using an SSID and a password
extern String wlanSsid2pass(String);       // search for the password for an SSID
