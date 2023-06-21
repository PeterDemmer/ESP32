// wifiSetup.cpp


// TO DO: adding WLAN credentials into Preferences


#include "Arduino.h"
#include "WiFi.h"
#include "myCli.h"
#include "myPlayer.h"
#include "myStations.h"
#include "myWiFi.h"
#include "myWire.h"


struct WLAN {
  String ssid;
  String pass;
};
WLAN WLANs[] = {
  { "ssid1",      "password1"},
  { "ssid2",      "password2"},
  { "",           ""         }
};
const uint8_t nWLANs = sizeof(WLANs) / sizeof(WLAN);


bool wlanSetup(void) {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.printf("wlsu%d\tWLAN reset done\n", __LINE__);

  wlanScan();   // scan and attempt to connect

  return true;
}


void wlanLoop(void) {
  static uint8_t oldStatus = 0xFF;   // cycle on WiFi signal loss: 3=CONNECTED, 5=CONNECTION_LOST, 1=NO_SSID_AVAIL, 0=IDLE_STATUS, 3
  
  uint8_t newStatus = WiFi.status();
  if (newStatus != oldStatus) {
    Serial.printf("wllp%d\tWiFi '%s' new status=%d\n", __LINE__, sSSID.c_str(), newStatus);
    //if (newStatus == 1 || newStatus == 5) {
      //wlanSetup();
    //}
    if (newStatus == 3) {
      mp3.connecttohost(urls[kStation]);
    }
    oldStatus = newStatus;
  }
}


// Function: control WLAN from Serial CLI;   usage: status: "wl;"   reset: "wl=r;"   
void cliWlan(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    snprintf(cliBuf, sizeof(cliBuf), "cwl%d\tlist WiFi status", __LINE__);
    Serial.println(cliBuf);
    wlanStatus();
  } else {
    switch (rxmsg[3]) {
      case 'r':
        snprintf(cliBuf, sizeof(cliBuf), "cwl%d\treset WiFi", __LINE__);
        wlanSetup();
        Serial.println(cliBuf);
        break;
      //case 's':
        //snprintf(cliBuf, sizeof(cliBuf), "cwl%d\tscan+connect WiFi", __LINE__);
        //Serial.println(cliBuf);
        //wlanScan();
        //break;
      default:
        snprintf(cliBuf, sizeof(cliBuf), "cwl%d\tWiFi: CLI error: '%c'=0x%02X", __LINE__, rxmsg[3], rxmsg[3]);
        Serial.println(cliBuf);
        break;
    }
  }

  //delay(DEBUG_KEEPTIME);
}


uint8_t iNet;
String sSSID;

String wlanScan(void) {
  String sIP;

  Serial.printf("wlsc%d\tWLAN scan ... ", __LINE__);

  uint8_t nNet = WiFi.scanNetworks();   // sorts by RSSI descending, returns the number of networks found
  Serial.print("done, ");
  if (nNet == 0) {
    Serial.printf("no networks found.\n");
  } else {
    Serial.printf("%d networks found.\n", nNet);
    //String sSSID;
    char cSSID[32];
    iNet = 0;
    while ((iNet < nNet) && (WiFi.status() != WL_CONNECTED)) {
      sSSID = WiFi.SSID(iNet);
      sSSID.toCharArray((char*)cSSID, 32);
      Serial.printf("    %2d: %30s   %d dBm %s    ", iNet + 1, cSSID, WiFi.RSSI(iNet), WiFi.encryptionType(iNet) == WIFI_AUTH_OPEN ? " " : "*");
      if (WiFi.status() != WL_CONNECTED) {
        String pass = wlanSsid2pass(cSSID);
        if (pass == "") {
          Serial.printf("SSID unknown\n");
        } else {
          Serial.printf("try:\n", __LINE__);
          wlanConnect(cSSID, pass);
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
      Serial.printf("    no WLAN connection\n");
      sIP = "";
    } else {
      sIP =  WiFi.localIP().toString();
      Serial.printf("wlsc%d\tconnection to '%s' OK, IP=%s RSSI=%d dBm\n", __LINE__, cSSID, sIP.c_str(), WiFi.RSSI(iNet));
      snprintf(disBuf, sizeof(disBuf), "%-16.16s %d", cSSID, WiFi.RSSI(iNet));
      disShow(3, disBuf);
      delay(1000);
    }
  }

  // Delete the scan result to free memory
  //WiFi.scanDelete();

  // TO DO: return (status == CONNECTED)
  return sIP;
}


bool wlanConnect(char* ssid, String password) {
  //Serial.printf("wlco%d\tssid='%s' pass='%s'\n", __LINE__, ssid, password);
  Serial.printf("wlco%d\tssid='%s' pass='*****'\n", __LINE__, ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("wlco%d\tconnecting to '%s' ", __LINE__, ssid);
  uint16_t iTry = 0, timeout = 20;   // check each second for 20 s
  while (iTry < timeout && WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    iTry++;
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) Serial.printf(" not");
  Serial.printf(" connected after %.0fs\n", ((float)iTry) / 1.);

  return (WiFi.status() == WL_CONNECTED);
}


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
    Serial.printf("wlst%d\tWiFi is not connected", __LINE__);
    return false;
  } else {
    Serial.printf("wlst%d\tWiFi is connected,  ", __LINE__);
    Serial.printf("SSID=%s,  ", WiFi.SSID());
    Serial.printf("IP=%s\n", WiFi.localIP().toString().c_str());
    return true;
  }
}
