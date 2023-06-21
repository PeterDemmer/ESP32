// myWire.cpp

#include "Arduino.h"
#include "Wire.h"
#include "myCli.h"
#include "myWire.h"
#include "LiquidCrystal_I2C.h"


// TO DO: move LiquidCrystal/lcd to myDisplay.*
LiquidCrystal_I2C lcd(0x24, 20, 4);
char disBuf[21];



bool wireSetup(void) {
  Wire.begin();   // default: SDA=GPIO21, SCL=GPIO22

  wireScan();

  lcd.init();
  lcd.backlight();

  return true;
}




void cliWire(char* rxmsg,  char* txmsg) {
  if (lDeb > 0) {
    snprintf(cliBuf, sizeof(cliBuf), "wis:%d\tcliWire():", __LINE__);
    Serial.println(cliBuf);
  };

  if (rxmsg[2] == 0) {   // "wi;"
    wireScan();
  }
}


// TO DO: specify range of addresses and return 1st address of that range (but scan and list all)
void wireScan(void) {
  byte error, address;
  int nDevices;
  snprintf(cliBuf, sizeof(cliBuf), "\n\nwisc%d\tScanning I2C ...", __LINE__);
  Serial.println(cliBuf);

  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tI2C device found at address 0x%02X", __LINE__, address);
      Serial.print(cliBuf);
      if (address == 0x23 || address == 0x5C) {   // ADDR connected to ground. NOTE: DON'T use address 0x23 here, that can also be a display address !
        snprintf(cliBuf, sizeof(cliBuf), " - BH1750/GY-302 light sensor");
        Serial.println(cliBuf);
        delay(1000);
      } else if (0x20 <= address && address <= 0x27) {
        snprintf(cliBuf, sizeof(cliBuf), " - LCD display");
        Serial.println(cliBuf);
        delay(1000);
      } else {
        Serial.println();
      }
      nDevices++;
    } else if (error == 4) {
      snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tUnknown error at address 0x02X", __LINE__, address);
      Serial.println(cliBuf);
    }
  }

  if (nDevices == 0) {
    snprintf(cliBuf, sizeof(cliBuf), "wisc%d\tNo I2C devices found\n", __LINE__);
    Serial.println(cliBuf);
  } else {
    snprintf(cliBuf, sizeof(cliBuf), "wisc%d\t%d I2C devices found, done.\n", __LINE__, nDevices);
    Serial.println(cliBuf);
  }
}


bool disShow(uint8_t line, const char* text) {
  // long texts are  printed into lines 0, 2, 1, 3, this code fixes that:

  char lText[LCDCOLS + 1];
  line %= LCDROWS;

  uint8_t lenText = strlen(text);

  sprintf(lText, "%-20.20s", text);
  lcd.setCursor(0, line++);
  lcd.print(lText);


  if (line < LCDROWS) {
    if (lenText <= LCDCOLS) {
      sprintf(lText, "%-20.20s", " ");
    } else {
      sprintf(lText, "%-20.20s", &text[LCDCOLS]);
    }
    lcd.setCursor(0, line++);
    lcd.print(lText);
  }

  if (line < LCDROWS)  {
    if (lenText <= 2 * LCDCOLS) {
      sprintf(lText, "%-20.20s", " ");
    } else {
      sprintf(lText, "%-20.20s", &text[2 * LCDCOLS]);
    }
    lcd.setCursor(0, line++);
    lcd.print(lText);
  }

  if (line < LCDROWS)  {
    if (lenText <= 3 * LCDCOLS) {
      sprintf(lText, "%-20.20s", " ");
    } else {
      sprintf(lText, "%-20.20s", &text[3 * LCDCOLS]);
    }
    lcd.setCursor(0, line++);
    lcd.print(lText);
  }

  return true;
}
