// myWebServer.cpp


#include "Arduino.h"
#include "myCli.h"
#include "myRelay.h"
#include "myWebServer.h"

/*
 *  Access (e.g. @thuis):
 *  E14: http://192.168.11.116/   P1meter thuis
 *  E15: http://192.168.11.117/   2 relays 230 V
 *  E16: http://192.168.11.118/   8 relais 12,230 V
 *  E17: http://192.168.11.119/   P1meter VvE
 *  E18: http://192.168.11.120/   2 relays 12 V
 *  E19: http://192.168.11.121/   breadboard
 *
 *  URL status request:
 *  http://192.168.11.117/
 *  
 *  URL commands:
 *  http://192.168.11.117/?rel=0&set=0&typ=M    set relay 0 off
 *  http://192.168.11.117/?rel=0&set=1&typ=M    set relay 0 on 
 *  http://192.168.11.117/?rel=1&set=0&typ=M    set relay 1 off
 *  http://192.168.11.117/?rel=1&set=1&typ=M    set relay 1 on
 *  http://192.168.11.117/?rel=1&set=1&typ=M&tim=60    set relay 1 on and fall back to automatic after 60 minutes
 *  
 */




bool wsSetup(void) {
  server.on("/", handleRoot);
  server.begin();

  if (lDeb > 0) {
    snprintf(cliBuf, sizeof(cliBuf), "wssu%d\tWeb server started\n\n", __LINE__);
    Serial.print(cliBuf);
  }

  return true;
};


void wsLoop(void) {
  server.handleClient();  // located in library

  for (uint8_t iPin = 0; iPin < nRelayPins; iPin++) {
    if (relayMode[iPin] == 'M') {
      if (uptime() > relayMExpire[iPin]) {
        relayMode[iPin] = 'A';
        snprintf(cliBuf, sizeof(cliBuf), "wslp%d\tpin %d from M=%d to A=%d\n", __LINE__, iPin, relayMState[iPin], relayAState[iPin]);
        Serial.print(cliBuf);
      }
    }
  }
}


uint32_t uptime(void) {
  return (uint32_t)(esp_timer_get_time() / (uint64_t)1000000);  // in seconds
}


void handleRoot() {
  static uint32_t tUntil = 0x7FFFFFFF;

  uint8_t relIndex = 255;
  uint8_t relSet = 255;
  char relMode = 'I';
  uint16_t relTime = 0;

  String sRel = server.arg("rel");
  String sSet = server.arg("set");
  String sTyp = server.arg("typ");
  String sTime = server.arg("time");
  if (sRel == "" || sSet == "") {
    if (lDeb > 0) {
      // TO DO: add status
      snprintf(cliBuf, sizeof(cliBuf), "wshr%d\tno arguments\n", __LINE__);
      Serial.print(cliBuf);
    }

    //if (relSet) digitalWrite(relayPins[relIndex], R1);
    //else digitalWrite(relayPins[relIndex], R0);

    showPage(0);  // no refresh
    // TO DO: set a refresh for the next time a timer will expire (if any)
  } else {
    // read the URL command line:
    relIndex = sRel.toInt();
    if (relIndex < 0) relIndex = 0;
    if (relIndex >= nRelayPins) relIndex = nRelayPins - 1;

    relSet = sSet.toInt();
    if (relSet != 1) relSet = 0;

    char buf[4];
    sTyp.toCharArray(buf, sizeof(buf));
    relMode = buf[0];
    if (relMode != 'M') relMode = 'A';

    relTime = sTime.toInt();
    if (relTime < 0) relTime = 0;
    if (relTime > 720) relTime = 720;                   // 12 hours
    if (relTime > 0) tUntil = uptime() + relTime * 60;  // seconds since boot

    if (lDeb > 1) {
      snprintf(cliBuf, sizeof(cliBuf), "wshr%d\tuptime=%d s\ttUntil=%d s\n", __LINE__, uptime(), tUntil);
      Serial.print(cliBuf);
    }

    if (lDeb > 0) {
      snprintf(cliBuf, sizeof(cliBuf), "wshr%d\trel=%d&set=%d&typ=%c&time=%d\texpires in %d s\n", __LINE__, relIndex, relSet, relMode, relTime, tUntil - uptime());
      Serial.print(cliBuf);
    }

    // store command line settings:
    if (relMode == 'M') {
      relayMState[relIndex] = relSet;
      relayMExpire[relIndex] = tUntil;
    } else if (relMode == 'A') {
      relayAState[relIndex] = relSet;
    }
    relayMode[relIndex] = relMode;

    if (relSet) digitalWrite(relayPins[relIndex], R1);
    else digitalWrite(relayPins[relIndex], R0);

    showPage(2);  // with refresh
  }
}


void showPage(uint16_t tRefresh) {
  String html = "<!DOCTYPE html>\n<HTML>\n";
  if (tRefresh > 0) {
    // TO DO: add html title and use larger fixed-width font
    snprintf(cliBuf, sizeof(cliBuf), "  <HEAD>\n    <META http-equiv=refresh content='%d; URL=/' />\n  </head>\n", tRefresh);
    html += cliBuf;
  }
  html += "  <BODY>\n    <FONT SIZE=+2><TABLE CELLPADDING=10>\n";

  for (uint8_t iPin = 0; iPin < nRelayPins; iPin++) {
    if (relayMode[iPin] == 'M') {
      if (relayMState[iPin]) {
        sprintf(cliBuf, "    <TR><TD>relay%d=on,M,%d</td>\n", iPin, (relayMExpire[iPin] - uptime()) / 60);
        html += String(cliBuf);

        sprintf(cliBuf, "        <TD><A HREF=/?rel=%d&set=%d&", iPin, relayAState[iPin] ? 1 : 0);
        html += String(cliBuf);
        sprintf(cliBuf, "typ=A>auto (%s)</a></td></tr>\n", relayAState[iPin] ? "on" : "off");
        html += String(cliBuf);
      } else {
        sprintf(cliBuf, "    <TR><TD>relay%d=off,M,%d</td>\n", iPin, (relayMExpire[iPin] - uptime()) / 60);
        html += String(cliBuf);

        sprintf(cliBuf, "        <TD><A HREF=/?rel=%d&set=%d&", iPin, relayAState[iPin] ? 1 : 0);
        html += String(cliBuf);
        sprintf(cliBuf, "typ=A>auto (%s)</a></td></tr>\n", relayAState[iPin] ? "on" : "off");
        html += String(cliBuf);
      }
    } else {  // relayMode[iPin] == 'A' or 'I'
      if (relayAState[iPin]) {
        sprintf(cliBuf, "    <TR><TD>relay%d=on,A</td>\n", iPin);
        html += String(cliBuf);

        for (uint8_t iTime = 60; iTime <= 120; iTime *= 2) {
          sprintf(cliBuf, "        <TD><A HREF=/?rel=%d&set=0&", iPin);
          html += String(cliBuf);
          sprintf(cliBuf, "typ=M&time=%d>off %d</a></td>\n", iTime, iTime);
          html += String(cliBuf);
        }
      } else {
        sprintf(cliBuf, "    <TR><TD>relay%d=off,A</td>\n", iPin);
        html += String(cliBuf);

        for (uint8_t iTime = 60; iTime <= 120; iTime *= 2) {
          sprintf(cliBuf, "        <TD><A HREF=/?rel=%d&set=1&", iPin);
          html += String(cliBuf);
          sprintf(cliBuf, "typ=M&time=%d>on %d</a></td>\n", iTime, iTime);
          html += String(cliBuf);
        }
      }
    }
  }
  html += "      </table>  </body>\n</html>\n";
  server.send(200, "text/html", html);

  if (lDeb > 1) {
    snprintf(cliBuf, sizeof(cliBuf), "wshr%d\thtml='%s'\n", __LINE__, html.c_str());
    Serial.print(cliBuf);
  }
}
