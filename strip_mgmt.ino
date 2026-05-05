#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel strip1;
extern Adafruit_NeoPixel strip2;

void color_neo(uint32_t color) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip1.setPixelColor(i, color);
    strip2.setPixelColor(i, color);
  }
  strip1.show();
  strip2.show();
}

void flashGoal(uint32_t color) {
  for (int j = 0; j < 3; j++) {
    color_neo(color);
    delay(100);
    color_neo(0);
    delay(100);
  }
}

void setAmbilightMode(int mode) {
  switch(mode) {
    case 0: // STANDBY
      color_neo(strip1.Color(20, 20, 50)); // Bleu nuit
      break;
    case 1: // MATCH
      color_neo(strip1.Color(255, 255, 255)); // Blanc pur
      break;
    case 2: // VICTORY
      color_neo(strip1.Color(255, 215, 0)); // Or
      break;
  }
}
