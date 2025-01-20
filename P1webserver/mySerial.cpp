// myLCD.cpp


#include "Arduino.h"
//#include "Wire.h
#include "mySerial.h"
#include "myLCD.h"
#include "myCli.h"
#include "myWebServer.h"


const tech2func tabTech2func[] = {
  // translate technical to functional field names
  { 1, "1-3:0.2.8", "version", 1 },
  { 2, "0-0:1.0.0", "ElTimestamp", 1 },
  { 3, "0-0:96.1.1", "ElEquipId", 1 },
  { 4, "1-0:1.8.1", "ElDel1", 1 },
  { 5, "1-0:1.8.2", "ElDel2", 1 },
  { 6, "1-0:2.8.1", "ElRcv1", 1 },
  { 7, "1-0:2.8.2", "ElRcv2", 1 },
  { 8, "0-0:96.14.0", "ElTariff", 1 },
  { 9, "1-0:1.7.0", "AvgActPowDel", 1 },
  { 10, "1-0:2.7.0", "AvgActPowRcv", 1 },
  //  { "0-0:96.7.21", "noPowFails", 1 },
  //  { "0-0:96.7.9", "noLPowFails", 1 },
  //  { "1-0:99.97.0", "PowFailEvLog", 1 },
  //  { "0-0:96.13.0", "text", 1 },
  { 11, "1-0:32.7.0", "VoltageL1", 1 },
  { 12, "1-0:52.7.0", "VoltageL2", 1 },
  { 13, "1-0:72.7.0", "VoltageL3", 1 },
  //  { "1-0:31.7.0", "CurrentL1", 1 },
  //  { "1-0:51.7.0", "CurrentL2", 1 },
  //  { "1-0:71.7.0", "CurrentL3", 1 },
  { 14, "1-0:21.7.0", "InstActPowDelL1", 1 },
  { 15, "1-0:41.7.0", "InstActPowDelL2", 1 },
  { 16, "1-0:61.7.0", "InstActPowDelL3", 1 },
  { 17, "1-0:22.7.0", "InstActPowRcvL1", 1 },
  { 18, "1-0:42.7.0", "InstActPowRcvL2", 1 },
  { 19, "1-0:62.7.0", "InstActPowRcvL3", 1 },
  { 20, "0-1:24.1.0", "GasDevType", 1 },
  { 21, "0-1:96.1.0", "GasEquipId", 1 },
  { 22, "0-1:24.2.1", "GasReading", 2 },  // 2 fields: gas-timestamp + reading
  { 24, "", "", 0 },                      // end-of-table indicater
};


uint8_t nWeb = 0;


HardwareSerial mySerial0(0);


bool serSetup(void) {
  mySerial0.begin(115200);  // P1-meter entrance

  return true;
}


bool serLoop(void) {
  bool bFound = false;  // true when at least a newline is received

  uint8_t iLine = 0;
  uint8_t iWeb;
  iWeb = 0;

  while (mySerial0.available() > 0 && iLine < P1NOLINES) {
    // Create a place to hold the incoming message
    static char serBuf[P1LINELEN];
    static uint8_t pos = 0;
    // Read the next available byte in the serial receive buffer
    char inByte = mySerial0.read();
    // Message coming in (check not terminating character) and guard for over message size
    if (inByte != '\n' && (pos < P1LINELEN - 1)) {
      // Add the incoming byte to our message
      serBuf[pos++] = inByte;
    } else {               // Full message received...
      serBuf[pos] = '\0';  // Add null character to string
      pos = 0;             // Reset for the next message
      bFound = true;
      if (lDeb > 1) {
        snprintf(cliBuf, sizeof(cliBuf), "selp%d\t%d\t%s\n", __LINE__, iLine, serBuf);
        Serial.printf(cliBuf);
      }
      strncpy(P1buf[iLine].tekst, serBuf, P1LINELEN - 1);
      iLine++;
    }

    // wait at maximum 200 ms if maybe another line arrives
    uint32_t timeout = millis() + (uint32_t)200;
    while (!mySerial0.available() && millis() < timeout) {};
  }

  uint8_t nLine = iLine;
  // Parse the following format: techName ( value1 ) ( value2 )
  for (iLine = 0; iLine < nLine; iLine++) {
    P1buf[iLine].datapos1 = 0xFF;
    char* pText1 = P1buf[iLine].tekst;
    P1buf[iLine].datapos2 = 0xFF;
    char* pText2 = P1buf[iLine].tekst;
    uint8_t textlen = strlen(P1buf[iLine].tekst);


    uint8_t iSep;
    for (iSep = 0; iSep < textlen; iSep++) {  // techName until first '('
      if (P1buf[iLine].tekst[iSep] == '(') {
        P1buf[iLine].tekst[iSep] = 0;      // techName terminator
        P1buf[iLine].datapos1 = iSep + 1;  // value1 starts after this '('
        pText1 = &P1buf[iLine].tekst[P1buf[iLine].datapos1];
        break;
      }
    }

    for (iSep = P1buf[iLine].datapos1 + 1; iSep < textlen; iSep++) {
      if (P1buf[iLine].tekst[iSep] == ')') {
        P1buf[iLine].tekst[iSep] = 0;  // value1 terminator
        break;
      }
    }

    if (iSep + 1 < textlen) {            // is there a value 2 ?
      P1buf[iLine].datapos2 = iSep + 2;  // start of value 2
      pText2 = &P1buf[iLine].tekst[P1buf[iLine].datapos2];
    }

    char* pEnd = &P1buf[iLine].tekst[textlen - 1];
    if (*pEnd == ')') *pEnd = 0;
    P1buf[iLine].tekst[iSep] = 0;

    // look up funcName from techName:
    String techName = String(P1buf[iLine].tekst);
    tech2func t2f = P1tech2func(techName);
    uint8_t index = t2f.index;
    if (index != 0xFF) iWeb = index;
    uint8_t nFields = t2f.nFields;
    String funcName = t2f.funcName;

    // write into web buffer:
    if (nFields == 1) {  // techName, funcName1, value1
      if (lDeb > 0) {
        snprintf(cliBuf, sizeof(cliBuf), "selp%d\t%d\t%d\t%s\t%-16.16s\t%s\n", __LINE__, iLine, index, techName.c_str(), funcName.c_str(), pText1);
        Serial.printf(cliBuf);
      }

      if (funcName != "" && iWeb < WEBNOLINES) {
        webBuf[iWeb].techName = techName;
        webBuf[iWeb].funcName = funcName;
        webBuf[iWeb].value = String(pText1);
        webBuf[iWeb].lastSeen = millis();
      }
    } else if (nFields == 2) {  // 2 values are printed on separate lines:
                                // techName, funcName1, value1;   techName, funcName2, value2
      String funcName1 = "", funcName2 = "";
      if (funcName != "") {
        funcName1 = funcName + "1";
        funcName2 = funcName + "2";
      }
      if (lDeb > 0) {
        snprintf(cliBuf, sizeof(cliBuf), "selp%d\t%d\t%d\t%s\t%-16.16s\t%s\n", __LINE__, iLine, index, techName.c_str(), funcName1.c_str(), pText1);
        Serial.printf(cliBuf);
        snprintf(cliBuf, sizeof(cliBuf), "selp%d\t%d\t%d\t%s\t%-16.16s\t%s\n", __LINE__, iLine, index + 1, techName.c_str(), funcName2.c_str(), pText2);
        Serial.printf(cliBuf);
      }

      if (funcName != "") {
        if (iWeb < WEBNOLINES - 1) {
          webBuf[iWeb].techName = techName;
          webBuf[iWeb].funcName = funcName1;
          webBuf[iWeb].value = String(pText1);
          webBuf[iWeb].lastSeen = millis();
          iWeb++;
          webBuf[iWeb].techName = techName;
          webBuf[iWeb].funcName = funcName2;
          webBuf[iWeb].value = String(pText2);
          webBuf[iWeb].lastSeen = millis();
        }
      }
    }
  }

  nWeb = iWeb;

  if (iLine > 0 || iWeb > 0) {  // function runs most of the times with both == 0
    lcdSetup();                 // hot-pluggable
    lcdShow(0, "");
    lcdShow(1, "");

    snprintf(cliBuf, sizeof(cliBuf), "ser=%d lines", iLine + 1);
    lcdShow(0, cliBuf);
    snprintf(cliBuf, sizeof(cliBuf), "web=%d lines", iWeb);
    lcdShow(1, cliBuf);
    
    if (lDeb > 0) {
      snprintf(cliBuf, sizeof(cliBuf), "selp%d\tiLine=%d\tnWeb=%d\tFound=%d\n", __LINE__, iLine, nWeb, bFound);
      Serial.printf(cliBuf);
    }
  }

  return bFound;
}


// translate the technical field name to the functional field name:
tech2func P1tech2func(String techName) {
  static String funcName;
  uint8_t lDeb = 1;
  uint8_t i = 0;
  static tech2func retVal;

  if (lDeb > 1)
    Serial.printf("setf%d\tsearch '%s'\n", __LINE__, techName.c_str());

  if (techName == "") {
    funcName = "";
    retVal.funcName = "";
  } else {
    // find matching techName
    while ((tabTech2func[i].techName != "") && (tabTech2func[i].techName != techName)) {
      if (lDeb > 1)
        Serial.printf("setf%d\t'%s' '%s'\n", __LINE__, tabTech2func[i].techName.c_str(), tabTech2func[i].funcName.c_str());
      i++;
    }
    if (tabTech2func[i].techName != techName) {
      funcName = "";
      retVal.index = 0xFF;
      retVal.funcName = "";
      retVal.nFields = 0;
    } else {
      funcName = tabTech2func[i].funcName;
      retVal = tabTech2func[i];
    }
  }

  if (lDeb > 1) {
    Serial.printf("setf%d\t%d\t'%s' => '%s' [%d]\n", __LINE__, retVal.index, retVal.techName.c_str(), retVal.funcName.c_str(), retVal.nFields);
  }

  return retVal;
}
