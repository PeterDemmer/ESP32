/*
 * P1-to web interface
 * Measurement data from a household energy meter with a P1-interface can be retrieved by using a WLAN web-interface.
 *
 * Note: When a certain parateter has once been received and is not refreshed from the P1-interface anymore, 
 * the web-interface keeps reporting its last received value, tagged with e.g. "5m" if the value is 5 minutes old
 *
 * Hardware: SEEED studio XIAO_ESP32C3 (14-pins)
 */


/*  Tools settings:
      Board: "XIAO ESP32C3"
      Upload Speed: "921600"
      CPU Frequency: 160 MHz
      Flash Frequency: 80 MHz
      Flash Mode: QIO
      Flash Size: 4 MB
      Partition Scheme: Default 4MB
      Core Debug Level: None
      PSRAM: Disabled
      Arduino runs on: Core 1
      Events ruin on: Core 1
      Erase All Flash Befor Sketch Upload: Disabled
      JTAG Adapter: Disabled
      Port: /dev/ttyACM0
      Programmer: esptool
*/


#include "myCli.h"
#include "myLCD.h"
#include "mySerial.h"
#include "myWlan.h"
#include "myWebServer.h"


void setup(void) {
  cliSetup();

  lcdSetup();

  serSetup();

  wlanSetup();

  webSetup();

  Serial.printf("su%d\tSetup complete\n\n", __LINE__);
}


void loop(void) {
  cliLoop();

  serLoop();

  wlanLoop();

  webLoop();

  delay(1);
}
