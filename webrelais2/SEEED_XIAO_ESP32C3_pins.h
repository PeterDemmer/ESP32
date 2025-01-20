/*
 *  E14 MAC address: 64:E8:33:86:D4:EC, IP address @thuis: 192.168.11.116
 *  E15 MAC address: 64:E8:33:85:21:BC, IP address @thuis: 192.168.11.117
 *  E16 MAC address: 64:E8:33:85:10:9C, IP address @thuis: 192.168.11.118
 *  E17 MAC address: 64:E8:33:86:DA:64, IP address @thuis: 192.168.11.119
 *  E18 MAC address: F0:9E:9E:B5:35:04, IP address @thuis: 192.168.11.120
 *  E19 MAC address: F0:9E:9E:B5:39:04, IP address @thuis: 192.168.11.121
 *
 *  GPIO2   A0   p0    +5V
 *  GPIO3   A1   p1    GND
 *  GPIO4   A2   p2    3V3
 *  GPIO5        p3    p10  MOSI  GPIO10
 *  GPIO6   SDA  p4    p9   MISO  GPIO9
 *  GPIO7   SCL  p5    p8   SCK   GPIO8
 *  GPIO21  TX   p6    p7   RX    GPIO20
 *
 *  Connections to relais:
 *    +3V3  JD-VCC - GN
 *    GND - GND    - BK
 *    IN1 - GPIO2
 *    IN2 - GPIO3
 *    VCC - +5V    - YE
 *
 *  Relaispinnen en I2C voorzien van een levewlshifter; voor de relaispinnen was dat niet nodig geweest
 */
