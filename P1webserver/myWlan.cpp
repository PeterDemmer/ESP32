// myWiFi.cpp

/*
   Handles Login into WLAN and NTP time retrieval
   host: esp16, MAC = 64:E8:33:85:10:9C, IP @thuis = 192.168.11.118
*/

// TO DO: adding WLAN credentials into Preferences


#include "Arduino.h"
#include <WiFi.h>     
#include "myCli.h"
#include "myLCD.h"
#include "myWlan.h"
//#include "myWire.h"


// Variable to store the HTTP request
String header;


struct WLAN {
  String ssid;
  String pass;
};
WLAN WLANs[] = {
  { "ssid1",            "password1"},
  { "ssid2",            "password2"},
  { "",                 ""         }
};
const uint8_t nWLANs = sizeof(WLANs) / sizeof(WLAN);


bool wlanSetup(void) {
  // Set WLAN to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wlanScan();   // scan and attempt to connect

  return true;
}


void wlanLoop(void) {
  // TO DO: add text to numeric:
  static uint8_t oldStatus = 0xFF;   // cycle on WLAN signal loss: 0=IDLE_STATUS, 1=NO_SSID_AVAIL, 3=CONNECTED, 5=CONNECTION_LOST

  uint8_t newStatus = WiFi.status();
  if (newStatus != oldStatus) {
    if (lDeb > 0) Serial.printf("wllp%d\tWLAN '%s' status=%d\n", __LINE__, sSSID.c_str(), newStatus);
    oldStatus = newStatus;
  }
}


// Function: control WLAN from Serial CLI;   usage: status: "wl;"   reset: "wl=r;"
void cliWlan(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    Serial.printf("wlcl%d\tlist WLAN status\n", __LINE__);
    wlanStatus();
  } else {
    char cliBuf1[40];
    switch (rxmsg[3]) {
      case 'r':
        snprintf(cliBuf1, sizeof(cliBuf1), "wlcl%d\treset WLAN", __LINE__);
        wlanSetup();
        Serial.println(cliBuf1);
        break;
      case 's':
        snprintf(cliBuf1, sizeof(cliBuf1), "wlcl%d\tscan WLAN", __LINE__);
        wlanScan();
        Serial.println(cliBuf1);
        break;
      default:
        Serial.printf("wlcl%d\tWLAN: CLI error: '%c'=0x%02X\n", __LINE__, rxmsg[3], rxmsg[3]);
        break;
    }
  }

  //delay(DEBUG_KEEPTIME);
}


uint8_t iNet;
String sSSID;

// scan all WLANs and try to log in into the strongest known
String wlanScan(void) {
  String sIP;

  Serial.printf("wlsc%d\tScanning WLAN ... ", __LINE__);

  uint8_t nNet = WiFi.scanNetworks();   // sorts by RSSI descending, returns the number of networks found
  Serial.print("done, ");
  if (nNet == 0) {
    Serial.printf("no networks found.\n");
  } else {
    Serial.printf("%d networks found.\n", nNet);
    char cSSID[32];
    iNet = 0;
    while ((iNet < nNet) && (WiFi.status() != WL_CONNECTED)) {
      sSSID = WiFi.SSID(iNet);
      sSSID.toCharArray((char*)cSSID, 32);
      Serial.printf("\t%d: %32s   %d dBm %s    ", iNet + 1, cSSID, WiFi.RSSI(iNet), WiFi.encryptionType(iNet) == WIFI_AUTH_OPEN ? " " : "*");
      if (WiFi.status() != WL_CONNECTED) {
        String pass = wlanSsid2pass(cSSID);
        if (pass == "") {
          Serial.printf("unregistered;\n");
        } else {
          Serial.printf("try:\n", __LINE__);
          char cPass[32];
          pass.toCharArray((char*)cPass, 32);
          wlanConnect(cSSID, cPass);
          if (WiFi.status() != WL_CONNECTED) {
            Serial.printf("wlsc%d\tconnection to '%s' failed\n", __LINE__, sSSID);
          }
        }
      }

      delay(10);
      iNet++;
    }
    iNet--;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf("    no WLAN connection");
      sIP = "";
    } else {
      sIP =  WiFi.localIP().toString();
      Serial.printf("wlsc%d\t    OK, IP=%s, RSSI=%d dBm\n", __LINE__, sIP.c_str(), WiFi.RSSI(iNet));
      snprintf(lcdBuf, sizeof(lcdBuf), "%-13.13s%d", cSSID, WiFi.RSSI(iNet));
      lcdShow(0, lcdBuf);
      snprintf(lcdBuf, sizeof(lcdBuf), "%s", sIP.c_str());
      lcdShow(1, lcdBuf);
      delay(2000);
    }
  }

  // Delete the scan result to free memory
  //WiFi.scanDelete();

  // TO DO: return (status == CONNECTED)
  return sIP;
}


// try to connect to a WLAN using an SSID and a password:
bool wlanConnect(char* ssid, char* password) {   
  Serial.printf("wlco%d\t    ssid='%s' pass='*****'\n", __LINE__, ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("wlco%d\t    connecting to '%s' ", __LINE__, ssid);
  uint16_t iTry = 0, timeout = 20;   // check each second for 20 s
  while (iTry < timeout && WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    iTry++;
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) Serial.printf(" not");
  Serial.printf(" connected after %.0f s\n", ((float)iTry) / 1.);

  return (WiFi.status() == WL_CONNECTED);
}


// search for the password for an SSID:
String wlanSsid2pass(String ssid) {
  static String pass;
  uint8_t lDeb = 0;
  uint8_t i = 0;

  // find matching SSID
  while ((WLANs[i].ssid != "") && (WLANs[i].ssid != ssid)) {
    if (lDeb > 1)
      Serial.printf("'%s' '%s'\n", WLANs[i].ssid, WLANs[i].pass);
    i++;
  }
  if (WLANs[i].ssid != ssid) {
    pass = "";
  } else {
    pass = WLANs[i].pass;
  }

  return pass;
}


bool wlanStatus(void) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("wlst%d\tWLAN is not connected", __LINE__);
    return false;
  } else {
    Serial.printf("wlst%d\tWLAN is connected,  ", __LINE__);
    Serial.printf("SSID=%s,  ", WiFi.SSID());
    Serial.printf("IP=%s\n", WiFi.localIP().toString().c_str());
    return true;
  }
}
