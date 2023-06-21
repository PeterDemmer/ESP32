// myBL.cpp

// handle ambient light sensing and backlight control


#include "Arduino.h"
#include "BH1750.h"
#include "myBL.h"
#include "myCli.h"


uint32_t lastL = 0L;
float luxAvg = 100.;
const float minLux = 2.;


BH1750 bh1750(0x5C);   // ADDC pin connected to VCC




void blSetup(void) {
  // PWM out for backlight:
  ledcSetup(0, 60, 12);   // 60 c/s due to internal display refresh frequency I guess
  ledcAttachPin(25, 0);   // GPIO25 PWM high = control LCD display backlight (pin K) via pulldown by inverting transistor
  ledcWrite(0, luxAvg);   // initially 2.5% backlight

  // I2C ambient light sensor input for controlling backlight:
  bh1750.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C);

  blSet();
}




void blSet(void) {
  static bool firsttime = true;
  float lux = bh1750.readLightLevel();
  if (lux < minLux) lux = minLux;

  if (firsttime) {
    luxAvg = lux;
    firsttime = false;
  } else {
    // average over 16 samples:
    luxAvg = (15 * luxAvg + lux) / 16;
  }
  uint32_t backlight1 = (int) (4095. / 120. * luxAvg);   // below 120 lux = 4095 dim the backlight
  if (backlight1 > 4095) backlight1 = 4095;
  ledcWrite(0, backlight1);

  if (lDeb > 2) {
    snprintf(cliBuf, sizeof(cliBuf), "pll%d\tlux=%.0f ave=%.1f pwm=%d=%.1f%%", __LINE__, lux, luxAvg, backlight1, 100.*backlight1 / 4095.);
    Serial.println(cliBuf);
  }
}


void blLoop(void) {
  // control backlight from light I2C sensor input
  uint32_t nowL = millis();
  if (nowL - lastL > 40L) {   // refresh every 1/25s
    blSet();
    lastL = nowL;
  }
}
