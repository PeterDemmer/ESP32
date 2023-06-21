// myPlayer.cpp
// Handle the vs1053 MP3 player, set station and volume, show metadata including unprintable characters


#include "Arduino.h"
#include <Preferences.h>
#include "Wire.h"
#include "vs1053_ext.h"
#include "myCli.h"
#include "myStations.h"
#include "myPlayer.h"
#include "myWire.h"


Preferences preferences;


VS1053 mp3(VS1053_CS, VS1053_DCS, VS1053_DREQ, VSPI, VS1053_MOSI, VS1053_MISO, VS1053_SCK);
uint8_t volume;
const uint8_t maxVolume = 21;




bool plSetup(void) {
  if (lDeb > 0) {
    if (noStations != noUrls) {
      snprintf(cliBuf, sizeof(cliBuf), "plsu%d\tWARNING: noStations=%d != %d=noURLs", __LINE__, noStations, noUrls);
      Serial.println(cliBuf);
    }
  }

  // persistent settings:
  preferences.begin("settings", false);
  volume = preferences.getUInt("volume", 8);
  kStation = preferences.getUInt("station", 0);
  if (lDeb > 0) {
    snprintf(cliBuf, sizeof(cliBuf), "plsu%d\tstation=%d volume=%d\n", __LINE__, kStation, volume);
    Serial.println(cliBuf);
  }

  // MP3 player
  SPI.begin(VS1053_SCK, VS1053_MISO, VS1053_MOSI);
  mp3.begin();
  mp3.setVolume(volume);
  mp3.connecttohost(urls[kStation]);
  snprintf(disBuf, sizeof(disBuf), "%-18.18s%02d", stations[kStation], kStation);
  disShow(0, disBuf);

  // no SD card implemented yet:
  //pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
  //SD.begin(SD_CS);

  return true;
}


void plSetVolume(uint8_t kCommand) {
  if (kCommand == UP && volume < maxVolume) {
    volume++;
  } else if (kCommand == DN && volume > 0) {
    volume--;
  } else {
    return;
  }

  mp3.setVolume(volume);
  preferences.putUInt("volume", volume);

  return;
}


void plSetStation(uint8_t kChannel) {
  if (kChannel == UP && kStation < noStations - 1) {
    kStation++;
  } else if (kChannel == DN && kStation > 0) {
    kStation--;
  } else if (kChannel < 0 || kChannel > noStations - 1) {
    return;
  } else {
    kStation = kChannel;
  }

  volume = mp3.getVolume();
  mp3.setVolume(0);
  //sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
  snprintf(disBuf, sizeof(disBuf), "%-18.18s%02d", stations[kStation], kStation);
  disShow(0, disBuf);
  disShow(1, "");   // will be filled by vs1053_showstreamtitle()
  mp3.connecttohost(urls[kStation]);
  preferences.putUInt("station", kStation);
  // TO DO: still some glitches are heard, restore volume a few 100 ms later
  mp3.setVolume(volume);

  return;
}


// next code is optional, allows using the MP3-player's data:

void vs1053_info(const char *info) {                // called from vs1053
  if (lDeb > 2) {
    Serial.print("DEBUG:        ");
    Serial.println(info);                           // debug info
  }
}

void vs1053_showstation(const char *info) {         // called from vs1053
  Serial.print("STATION:      ");
  Serial.println(info);                           // Show station name
}


// Convert (by the display) unprintable characters (with accents) to printables:
// Ref: https://www.utf8-chartable.de/
struct trans {
  uint16_t lCh;
  String sCh;
};
struct trans translist[] = {
  { /* Á */ 0xC381,  "A",  },
  { /* ß */ 0xC39F,  "ss", },
  { /* à */ 0xC3A0,  "a",  },
  { /* á */ 0xC3A1,  "a",  },
  { /* â */ 0xC3A2,  "a",  },
  { /* ä */ 0xC3A4,  "ae", },
  { /* ç */ 0xC3A7,  "c",  },
  { /* è */ 0xC3A8,  "e",  },
  { /* é */ 0xC3A9,  "e",  },
  { /* ê */ 0xC3AA,  "e",  },
  { /* ï */ 0xC3AF,  "i",  },
  { /* ° */ 0xC2B0,  "o",  },
  { /* ò */ 0xC3B2,  "o", },
  { /* ö */ 0xC3B6,  "oe", },
  { /* ù */ 0xC3B9,  "u",  },
  { /* ü */ 0xC3BC,  "ue", },
};
uint8_t nTrans = sizeof(translist) / sizeof(trans);

String translate(uint16_t lCh) {
  if (lCh < 0x80) {   // standard 7-bit ASCII, convert to 16-bit
    char lStr[2] = { 0, 0 };
    lStr[0] = lCh & 0x7F;
    return String(lStr);
  }

  // map 16-bit char to 7-bit ASCII char:
  for (uint8_t iTrans = 0; iTrans < nTrans; iTrans++) {
    if (lCh == translist[iTrans].lCh) {
      return translist[iTrans].sCh;
    }
  }
  return "";   // not found = skipped, e.g.  « »
}

void vs1053_showstreamtitle(const char *info) {     // called from vs1053
  char tmpmsg[16];
  uint16_t lCh;
  String infoStr = "";

  Serial.print("STREAMTITLE:  ");
  Serial.println(info);                           // Show title with accents etc.
  for (uint8_t iCh = 0; iCh < strlen(info); iCh++) {
    if (info[iCh] < 0x80) {   // normal ASCII character
      lCh = info[iCh];
    } else if (info[iCh] == 0xC2 || info[iCh] == 0xC3) {   // special character
      lCh = info[iCh] << 8 | info[++iCh];
    }
    if (lDeb > 2) {   // list every single character
      sprintf(tmpmsg, "%c=%02X=>%s ", lCh, lCh, translate(lCh).c_str());
      Serial.print(tmpmsg);
      if ((iCh + 1) % 8 == 0) Serial.println();
    }
    // TO DO: skip first space, do not repeat spaces, squeeze "  -  " sequences
    infoStr += translate(lCh);
  }
  if (lDeb > 2)Serial.println();

  Serial.println("              " + infoStr);

  disShow(1, infoStr.c_str());   // updates display bottom 3 lines: 1,2,3
}

void vs1053_showstreaminfo(const char *info) {      // called from vs1053
  Serial.print("STREAMINFO:   ");
  Serial.println(info);                           // Show streaminfo
}

void vs1053_eof_mp3(const char *info) {             // called from vs1053
  Serial.print("vs1053_eof:   ");
  Serial.print(info);                             // end of mp3 file (filename)
}

void vs1053_bitrate(const char *br) {               // called from vs1053
  Serial.print("BITRATE:      ");
  Serial.println(String(br) + "kBit/s");          // bitrate of current stream
}

void vs1053_commercial(const char *info) {          // called from vs1053
  Serial.print("ADVERTISING:  ");
  Serial.println(String(info) + " ms");           // info is the duration of advertising
}

void vs1053_icyurl(const char *info) {              // called from vs1053
  Serial.print("Homepage:     ");
  Serial.println(info);                           // info contains the URL
}

void vs1053_eof_speech(const char *info) {          // called from vs1053
  Serial.print("end of speech:");
  Serial.println(info);
}

void vs1053_lasthost(const char *info) {            // really connected URL
  Serial.print("lastURL:      ");
  Serial.println(info);
}


// Function: set Station from CLI;   usage: "st;"   "st+;"   "st-;"   "st=l;"   "st=1;"
void cliStation(char* rxmsg, char* txmsg) {
  if (rxmsg[2] == 0) {   // "st;" - show station
    sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
  } else if (rxmsg[2] == '+') {   // "st+;" - station up
    if (kStation >= noStations - 1) {
      sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
    } else {
      kStation++;
      disShow(1, "");   // clear 3 bottom lines
      mp3.connecttohost(urls[kStation]);
      sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
    }
  } else if (rxmsg[2] == '-') {   // "st-;" - station down
    if (kStation <= 0) {
      sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
    } else {
      kStation--;
      disShow(1, "");   // clear 3 bottom lines
      mp3.connecttohost(urls[kStation]);
      sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
    }
  } else if (rxmsg[2] == '=') {
    if (rxmsg[3] == 'l') {   // "st=l;" - list all stations
      // list all stations
      for (uint8_t iSt = 0; iSt < noUrls; iSt++) {
        if (iSt == kStation) {
          sprintf(txmsg, "*%d,%s;", iSt, stations[iSt]);
        } else {
          sprintf(txmsg, " %d,%s;", iSt, stations[iSt]);
        }
        Serial.println(txmsg);
      }
      txmsg[0] = 0;
      return;
    } else {   // e.g. "st=4;" - set station
      // set new station
      int8_t result = -2;
      uint8_t jStation = 255;
      result = sscanf(&rxmsg[3], "%d", &jStation);
      if (result < 1) {
        sprintf(txmsg, "E:%s;", rxmsg);
        return;
      } else {
        if (jStation > noStations - 1) jStation = noStations - 1;
        kStation = jStation;
        disShow(1, "");   // clear 3 bottom lines
        mp3.connecttohost(urls[kStation]);
        sprintf(txmsg, "st=%d,%s;", kStation, stations[kStation]);
      }
    }    // case rxmsg[3]
  }   // case rxmsg[2]

  snprintf(disBuf, sizeof(disBuf), "%d %s", kStation, stations[kStation]);
  disShow(0, disBuf);
  //disShow(1, "");
}


// Function: set Volume (0...21) from CLI;   usage: "vo;"   "vo+;"   "vo-;"   "vo=6;"
void cliVolume(char* rxmsg, char* txmsg) {
  volume = mp3.getVolume();
  if (rxmsg[2] == 0) {  // "vo;" - show volume
    sprintf(txmsg, "vo=%d; ", volume);
    return;
  } else if (rxmsg[2] == '+') {   // "vo+;" - increase volume
    if (volume < maxVolume) {
      mp3.setVolume(++volume);
      sprintf(txmsg, "vo=%d; ", volume);
    }
    return;
  } else if (rxmsg[2] == '-') {   // "vo-;" - decrease volume
    if (volume > 0) {
      mp3.setVolume(--volume);
      sprintf(txmsg, "vo=%d;", volume);
    }
    return;
  } else if (rxmsg[2] == '=') {   // "vo=7;" - set volume
    int8_t result = -2;
    uint8_t jVolume = 255;
    result = sscanf(&rxmsg[3], "%d", &jVolume);
    if (result < 1) {
      //snprintf(cliBuf, sizeof(cliBuf), "pv: % d\tE: set volume mislukt: '%s'", __LINE__, &rxmsg[3]);
      //Serial.println(cliBuf);
      sprintf(txmsg, "E: %s; ", rxmsg);
      return;
    } else {
      if (jVolume > maxVolume) jVolume = maxVolume;
      if (jVolume < 0) jVolume = 0;
      volume = jVolume;
      mp3.setVolume(volume);
      volume = mp3.getVolume();
      sprintf(txmsg, "vo=%d; ", volume);
      return;
    }
  }
}
