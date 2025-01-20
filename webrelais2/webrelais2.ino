/* 
 *  E15 - Web-controlled 2-relais
 *  
 *  Hardware:
 *  Seeed XIAO ESP32C3 (14 pins), 2 relais (active low), JDVCC-ingang kan nog gevoed met 3V3 van de ESP32.
 *    
 *  Access (e.g. @thuis):
 *  http://192.168.11.117/
 *  
 *  URL status request:
 *  http://192.168.11.117/
 *  http://192.168.11.117/?rel=0&set=1&typ=M&time=1    set relay 0 to on, mode to Manual for 1 minute
 *  Manual overrules Automatic for the duration of its time-out.
 *  When an Automatic setting is done during Manual mode, the Manual time-out is reset.
 *  
 *  Sketch uses 977084 bytes (74%) of program storage space. Maximum is 1310720 bytes.
 *  Global variables use 35860 bytes (10%) of dynamic memory, leaving 291820 bytes for local variables. Maximum is 327680 bytes.
 *
 */


#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include "myCli.h"
#include "myRelay.h"
#include "myWlan.h"
#include "myWebServer.h"


WebServer server(80);


void setup() {
  cliSetup();

  relSetup();

  wlanSetup();

  wsSetup();
}

 
void loop() {
  cliLoop();

  wlanLoop();

  wsLoop();    
}
