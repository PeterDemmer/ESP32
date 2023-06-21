/*
  Internet Radio using ESP32, BH1750 light sensor
  By: Peter Demmer


  Features:
    Internat Radio, supporting 10s of radio stations, also plays mp3s on websites
    Contains list of stations
    Memorises last selected station and volume over shutdown
    Metadata (Station, Song, Artist, Composer) shown on display
    Supports list of WLAN SSIDs and attempts to login to the strongest known
    Lists found I2C deviced found at startup
    Infrared remote control for station and volume
    Automatic LCD display brightness control
    Serial CLI control supported for most functions 
    Debug messages via Serial and debug level support for development and troubleshooting
    Modular code setup using a .h and a .cpp file and a Setup() and a Loop() for each function
    Logical function names 
    Operational data controlled by simple tables that can easily be adjusted

  Core inspiration for this code: https://github.com/schreibfaul1/ESP32-vs1053_ext

  Hardware: 
    LilyGO TTGO T8 ESP32 - 4MB Flash - 8MB PSRAM (30-pins)
    Infrared sensor (3v3)
    BH1750 I2C light sensor (3v3)
    3.3->5V level shift for I2C
    I2C 20x4 LCD display (5V), modified for PWM backlight control


  Tools settings:
    Board: ESP32 WRover Module
    Upload speed: "921600"
    CPU Frequency: "240 MHz (WiFi/BT)"
    Flash Frequency: "80 MHz"
    Flash Mode: "QIO"
    Flash Size: "4MB (32 MB)"
    Partition Scheme: "default 4 MB with spiffs"
    Core Debug Level: "None"
    PSRAM: "Enabled"
    Arduino runs on: "Core 1"
    Events run on: "Core 1"
    Erase All Flash Before scetch upload: "Disabled"
    Port: /dev/ttyACM0
    Programmer: Esptool

  Sketch uses 997581 bytes (76%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 49040 bytes (14%) of dynamic memory, leaving 278640 bytes for local variables. Maximum is 327680 bytes.

  My reference: E08_vs1053radio_3529
*/


#include "LilyGO-TTGO-T8.pinout.h"   // no program code, text only
#include "Arduino.h"
#include "WiFi.h"
#include "SPI.h"
//#include "SD.h"   // not used yet
//#include "FS.h"   // not used yet
#include "vs1053_ext.h"
#include "myBL.h"
#include "myCli.h"
#include "myIR.h"
#include "myWiFi.h"
#include "myWire.h"
#include "myStations.h"
#include "myPlayer.h"
#include "myWire.h"




void setup() {
  lDeb = 1;

  cliSetup();

  wireSetup();   // required for Backlight and InfraRed

  blSetup();

  irSetup();

  wlanSetup();

  plSetup();
}




void loop() {
  cliLoop();    // handle CLI commands from the Serial

  wlanLoop();

  blLoop();     // handle backlight

  irLoop();     // handle IR commands

  mp3.loop();   // continue MP3-playing
}
