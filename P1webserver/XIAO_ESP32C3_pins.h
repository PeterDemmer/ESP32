
/*
 * GPIO2   A0   D0    UUSB 5V
 * GPIO3   A1   D1    GND
 * GPIO4   A2   D2    3V3
 * GPIO5        D3    D10  MOSI  GPIO10
 * GPIO6   SDA  D4    D9   MISO  GPIO9
 * GPIO7   SCL  D5    D8   SCK   GPIO8
 * GPIO21  TX   D6    D7   RX    GPIO20
 */

/*
 1602 LCD connected to GND, 5V, SDA, SCL via 3V->5V level shifter

 GPS connected to HWserial0, GPS-TX => XIAO-RX = pin D7
 TX pin D6 can still be used for something else (but with the same baudrate)
*/
