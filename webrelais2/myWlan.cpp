// myWiFi.cpp

/*
 *  Handles Login into WLAN, multiple SSIDs, including reconnect, supported
 *
 *  Based on the following examples:
 *  WiFi > WiFiMulti: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiMulti/WiFiMulti.ino
 *  WiFi > WiFiScan: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiScan/WiFiScan.ino
 *  Complete project details at our blog: https://RandomNerdTutorials.com/
 *  
 *  TO DO: check if all this code is needed, maybe some of it can be pruned
 *  
 */


#include "Arduino.h"
#include <WiFi.h>       // /home/ardesp/.arduino15/packages/esp32/hardware/esp32/3.0.7/libraries/WiFi/src/*
#include <WiFiMulti.h>  // /home/ardesp/.arduino15/packages/esp32/hardware/esp32/3.0.7/libraries/WiFi/src/*
#include "myCli.h"
#include "myRelay.h"
#include "myWlan.h"


WiFiMulti wifiMulti;


struct WLAN {
  char* ssid;
  char* pass;
};
WLAN WLANs[] = {
  { (char*)"ssid1", (char*)"password1" },
  { (char*)"ssid2", (char*)"password2" },
};
const uint8_t nWLANs = sizeof(WLANs) / sizeof(WLAN);
const uint32_t connectTimeoutMs = 10000;




bool wlanSetup(void) {
  Serial.printf("wlsu%d\twlanSetup started\n", __LINE__);

  // Set WLAN to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // ???
  delay(100);

  for (uint8_t iW = 0; iW < sizeof(WLANs) / sizeof(WLAN); iW++) {
    wifiMulti.addAP(WLANs[iW].ssid, WLANs[iW].pass);
  }

  uint8_t nNet = WiFi.scanNetworks();
  Serial.printf("wlsu%d\tscan done (%d), ", __LINE__, nNet);
  if (nNet == 254) {
    Serial.printf("stopped\n");
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
  } else if (nNet == 255) {
    Serial.printf("no shield\n");
  } else if (nNet == 0) {
    Serial.printf("no networks found\n");
  } else {
    Serial.printf("%d networks found\n", nNet);
    for (int iNet = 0; iNet < nNet; iNet++) {
      // Print SSID and RSSI for each network found
      String sSSID = WiFi.SSID(iNet);
      char cSSID[32];
      sSSID.toCharArray((char*)cSSID, 32);
      if (WiFi.RSSI(iNet) > -80) {
        Serial.printf("wlsu%d\t%d\t%-24s\t%d dBm\t%s\n", __LINE__, iNet + 1, cSSID, WiFi.RSSI(iNet), WiFi.encryptionType(iNet) == WIFI_AUTH_OPEN ? " " : "*");
        delay(10);
      }
    }
  }

  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.printf("wlsu%d\tWLAN setup result ... ", __LINE__);
  if (wifiMulti.run() == WL_CONNECTED) {
    String sSSID = WiFi.SSID();
    char cSSID[32];
    sSSID.toCharArray((char*)cSSID, 32);
    Serial.printf("connected to %s, IP=%s\n\n", cSSID, WiFi.localIP().toString().c_str());
    // TO DO: don't show red light
    return true;
  } else {
    Serial.printf("not connected!\n\n");
    // TO DO: show red light
    return false;
  }
}


void wlanLoop(void) {
  static uint8_t oldStatus = 253;  // cycle on WLAN signal loss: 255=NO_SHIELD 254=STOPPED 0=IDLE_STATUS, 1=NO_SSID_AVAIL, 3=CONNECTED, 5=CONNECTION_LOST   (WiFiType.h)
  char* statusName[] = { (char*)"idle", (char*)"no SSID avail", (char*)"scan completed", (char*)"connected", (char*)"connect failed", (char*)"connection lost", (char*)"disconnected", (char*)"unknown" };

  // NOT WORKING: if the connection to the stongest hotstop is lost, it doesn't reconnect to the next network on the list
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    if (lDeb > 2) {
      String sSSID = WiFi.SSID();
      char cSSID[32];
      sSSID.toCharArray((char*)cSSID, 32);
      Serial.printf("wllp%d\tWiFi connected\tSSID=%s\tIP=%s\tRSSI=%d\n", __LINE__, cSSID, WiFi.localIP().toString().c_str(), WiFi.RSSI());
      // TO DO: show green light for e.g 2 seconds
    }
  } else {
    if (lDeb > 1) {
      Serial.printf("wllp%d\tWiFi not connected\n", __LINE__);
      // TO DO: show red light
    }
  }

  uint8_t newStatus = WiFi.status();
  if (newStatus != oldStatus) {
    uint8_t newStatus1 = newStatus;
    if (newStatus1 > 7) newStatus1 = 7;
    String sSSID = WiFi.SSID();  // BUG: value disappears after status change
    char cSSID[32];
    sSSID.toCharArray((char*)cSSID, 32);
    Serial.printf("wllp%d\tWLAN %s status=%d (%s)\n", __LINE__, cSSID, newStatus, statusName[newStatus1]);
    oldStatus = newStatus;
    // TO DO: show green/red statuslight for e.g. 10 seconds
  }

  static uint64_t nextTime = (uint64_t)0;
  static uint64_t lastConn = (uint64_t)0;
  if (newStatus != 0 && newStatus != 3) {
    uint8_t nNet = WiFi.scanNetworks();
    Serial.printf("wllp%d\tscan done (%d), ", __LINE__, nNet);
    if (nNet == 254) {
      Serial.printf("stopped\n");
      delay(1000);
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
    } else if (nNet == 255) {
      Serial.printf("no shield\n");
    } else if (nNet == 0) {
      Serial.printf("no networks found\n");
    } else {
      Serial.printf("%d networks found\n", nNet);
      for (int iNet = 0; iNet < nNet; iNet++) {
        // Print SSID and RSSI for each network found
        String sSSID = WiFi.SSID(iNet);
        char cSSID[32];
        sSSID.toCharArray((char*)cSSID, 32);
        if (WiFi.RSSI(iNet) > -80) {
          Serial.printf("wllp%d\t%d\t%-24s\t(%d)\t%s\n", __LINE__, iNet + 1, cSSID, WiFi.RSSI(iNet), WiFi.encryptionType(iNet) == WIFI_AUTH_OPEN ? " " : "*");
          delay(10);
        }
      }
    }

    // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
    Serial.printf("wllp%d\tWLAN setup result ... ", __LINE__);
    int8_t run = wifiMulti.run();
    if (run == WL_CONNECTED) {
      String sSSID = WiFi.SSID();
      char cSSID[32];
      sSSID.toCharArray((char*)cSSID, 32);
      Serial.printf("connected (%d): %s, IP=%s\n", run, cSSID, WiFi.localIP().toString().c_str());
    } else {
      Serial.printf("not connected (%d)!\n", run);
    }

    if (esp_timer_get_time() > nextTime) {  // make message appear at most every 10 sec
      Serial.printf("wllp%d\tWLAN disconnected\n", __LINE__);
      nextTime = esp_timer_get_time() + (uint64_t)10000000;
    }
  }
}




// Function: control WLAN from Serial CLI;   usage: status: "wl;"   reset: "wl=r;"
void cliWlan(char* rxmsg, char* txmsg) {
  if (rxmsg[2] != '=') {
    Serial.printf("wlcl%d\tWLAN status:\n", __LINE__);
    wlanStatus();
  } else {
    char cliBuf1[40];
    uint8_t nNet;
    switch (rxmsg[3]) {
      case 'd':
        snprintf(cliBuf1, sizeof(cliBuf1), "wlcl%d\tWLAN disconnect", __LINE__);
        Serial.println(cliBuf1);
        WiFi.disconnect();
        break;
      case 'r':
        snprintf(cliBuf1, sizeof(cliBuf1), "wlcl%d\tWLAN reset", __LINE__);
        Serial.println(cliBuf1);
        wlanSetup();
        break;
      case 's':
        wlanScan();
        break;
      default:
        Serial.printf("wlcl%d\tWLAN: CLI error: '%c'=0x%02X\n", __LINE__, rxmsg[3], rxmsg[3]);
        break;
    }
  }
}

uint8_t iNet;
String sSSID;

// scan all WLANs and try to log in into the strongest known
String wlanScan(void) {
  String sIP;

  Serial.printf("wlsc%d\tScanning WLAN ... ", __LINE__);

  int8_t nNet = WiFi.scanNetworks();  // sorts by RSSI descending, returns the number of networks found or -1
  Serial.print("done, ");
  if (nNet < 1) {
    Serial.printf("no networks found (%d).\n", nNet);
  } else {
    Serial.printf("%d networks found.\n", nNet);
    char cSSID[32];
    iNet = 0;
    //while ((iNet < nNet) && (WiFi.status() != WL_CONNECTED)) {
    while (iNet < nNet) {
      if (WiFi.RSSI(iNet) > -80) {
        sSSID = WiFi.SSID(iNet);
        sSSID.toCharArray((char*)cSSID, 32);
        Serial.printf("\t%2d: %-32s   %d dBm %s    ", iNet + 1, cSSID, WiFi.RSSI(iNet), WiFi.encryptionType(iNet) == WIFI_AUTH_OPEN ? " " : "*");
        Serial.printf("\n");
        delay(10);
        iNet++;
      }
    }
    iNet--;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.printf("    no WLAN connection");
      sIP = "";
    } else {
      sIP = WiFi.localIP().toString();
      Serial.printf("wlsc%d\t    OK, IP=%s, RSSI=%d dBm\n", __LINE__, sIP.c_str(), WiFi.RSSI(iNet));
    }
  }

  // Delete the scan result to free memory
  //WiFi.scanDelete();

  // TO DO: return (status == CONNECTED)
  return sIP;
}


bool wlanStatus(void) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("wlst%d\tWLAN (%d) is not connected\n", __LINE__, WiFi.status());
    return false;
  } else {
    Serial.printf("wlst%d\tWLAN is connected to ", __LINE__);
    Serial.printf("%s, ", WiFi.SSID());
    Serial.printf("IP=%s\n", WiFi.localIP().toString().c_str());
    return true;
  }
}
