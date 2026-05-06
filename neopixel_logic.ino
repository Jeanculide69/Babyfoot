#include <Adafruit_NeoPixel.h>
#include "config.h"

#define LED_PIN_1 26
#define LED_PIN_2 14
#define LED_COUNT 40

Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN_1, NEO_RBG + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN_2, NEO_RBG + NEO_KHZ800);

extern bool isAnimationActive();
extern volatile unsigned long statut_game;

void setupLEDs() {
  strip1.begin();
  strip2.begin();
  strip1.setBrightness(150);
  strip2.setBrightness(150);
  strip1.show(); 
  strip2.show();
}

void updateLEDs() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 30) return;
  lastUpdate = millis();

  static uint16_t j = 0;
  j++;

  if (isAnimationActive()) {
    // --- ANIMATION COULEUR (But / Victoire) ---
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = strip1.gamma32(strip1.ColorHSV((i * 65536 / LED_COUNT) + (j * 512)));
      strip1.setPixelColor(i, color);
      strip2.setPixelColor(i, color);
    }
  } 
  else if (bitRead(statut_game, 1)) { // Mode RUN (Match en cours)
    // --- BLANC FIXE PENDANT LE MATCH ---
    for (int i = 0; i < LED_COUNT; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 255, 255));
      strip2.setPixelColor(i, strip2.Color(255, 255, 255));
    }
  }
  else {
    // --- VEILLE / STANDBY (Petit pulse bleu) ---
    uint8_t pulse = (sin(millis() / 500.0) * 50) + 60;
    for (int i = 0; i < LED_COUNT; i++) {
      strip1.setPixelColor(i, strip1.Color(0, 0, pulse));
      strip2.setPixelColor(i, strip2.Color(0, 0, pulse));
    }
  }

  strip1.show();
  strip2.show();
}
