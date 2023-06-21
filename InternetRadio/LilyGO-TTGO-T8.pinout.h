/*
  LilyGO-TTGO-T8.pinout.h

  
  S_VP  - WH/GY - 36 - IR receive pin
  S_VN  - WH/BK - 39 - not used anymore
  Reset - vs1053-XRST
  34    - vs1053-DREQ
  35
  32
  33
  25 - OR - PWM out via - R 1k - inverting transistor to 2004A display backlight = K; I = 27 mA
  26
  27
  14
  12
  13
  15


  GND- BK
  3V3- GN
  21 - BN    - I2C-SDA - via level shifter to SDA 2004A display
  TXD
  RXD
  22 - GY    - I2C-SCL - via level shifter to SCL 2004A display
  19 - BU    - vs1053-MISO
  23 - RD    - vs1053-MOSI
  18 - WH/GY - vs1053-SCLK
  5  - SD-CS
  2  - PU    - vs1053-XCS
  0
  4  - GY    - vs1053-XDCS
  5V - YE


  Standard color codes:
  BK - black  - GND
  BN - brown  - I2C SDA
  RD - red    - +6V++, MOSI, serial out
  OR - orange
  YE - yellow - 5V
  GN - green  - 3V3
  BU - blue   - MISO, serial in
  PU - purple
  GY - gray   - I2C SCL
  WH - white

*/
