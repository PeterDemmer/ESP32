// myWebServer.cpp


#include "Arduino.h"
#include "myCli.h"
#include "myWebServer.h"
#include "mySerial.h"

/*
 *  Access (e.g. @thuis):
 *  esp14: http://192.168.11.116/
 *  esp17: http://192.168.11.119/
 */


WebServer server(80);

wline webBuf[WEBNOLINES];


bool webSetup(void) {
  server.on("/", handleRoot);

  server.begin();

  snprintf(cliBuf, sizeof(cliBuf), "wssu%d\tWeb server started\n", __LINE__);
  Serial.printf(cliBuf);

  return true;
}


void webLoop(void) {
  server.handleClient();
}


void handleRoot(void) {
  String html = "<!DOCTYPE html><body>\n";
  for (uint8_t iWeb = 0; iWeb < WEBNOLINES; iWeb++) {
    char sAge[8];
    sAge[0] = 0;
    uint32_t ageMinutes = (millis() - webBuf[iWeb].lastSeen) / 60000L;
    if (ageMinutes >= 1L) snprintf(sAge, sizeof(sAge), "%ldm", ageMinutes);
    if (webBuf[iWeb].funcName != "") {
      if (lDeb > 0) {
        snprintf(cliBuf, sizeof(cliBuf), "wshr%d\t%d\t%s\t%-16.16s\t%s\t%s\n", __LINE__, iWeb, webBuf[iWeb].techName.c_str(), webBuf[iWeb].funcName.c_str(), webBuf[iWeb].value.c_str(), sAge);
        Serial.printf(cliBuf);
      }
      snprintf(cliBuf, sizeof(cliBuf), "%s\t%s\t%s\t%s<br>\n", webBuf[iWeb].techName.c_str(), webBuf[iWeb].funcName.c_str(), webBuf[iWeb].value.c_str(), sAge);
      html += String(cliBuf);
    }
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
  if (lDeb > 1) {
    snprintf(cliBuf, sizeof(cliBuf), "wshr%d\t'%s'\n", __LINE__, html.c_str());
    Serial.printf(cliBuf);
  }
}
