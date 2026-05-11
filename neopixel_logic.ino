#include <Adafruit_NeoPixel.h>
#include "config.h"

#define LED_PIN_1 26
#define LED_PIN_2 14
#define LED_COUNT 40

Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN_2, NEO_GRB + NEO_KHZ800);

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

void setNeoBrightness(int b) {
  strip1.setBrightness(b);
  strip2.setBrightness(b);
  strip1.show();
  strip2.show();
}

void color_neo(uint32_t c) {
  for(int i = 0; i < LED_COUNT; i++) {
    strip1.setPixelColor(i, c);
    strip2.setPixelColor(i, c);
  }
}

// Fonction "Miroir" de l'ancien code
void edge_color(int x, int y, uint32_t color) {  
  // Bord gauche vertical (0-31)
  if (x == 0 && y < 32) {               
    strip1.setPixelColor(y, color);
  }
  // Bord droit vertical (0-31)
  if (x == 63 && y < 32) { 
    strip2.setPixelColor(y, color);
  }
  // Bord bas - on repartit les 64 pixels sur les 8 LED restantes de chaque strip (32-39)
  if (y == 31) {
    if (x < 32) {
        // Moitie gauche du bas vers Strip 1 (LED 32 a 39)
        int ledIdx = 32 + (x / 4); 
        strip1.setPixelColor(ledIdx, color);
    } else {
        // Moitie droite du bas vers Strip 2 (LED 32 a 39)
        int ledIdx = 32 + ((63 - x) / 4);
        strip2.setPixelColor(ledIdx, color);
    }
  }
}

void updateLEDs() {
  bool animActive = isAnimationActive();

  if (animActive) {
    // En cours d'animation : on laisse edge_color piloter les pixels et on affiche
    strip1.show();
    strip2.show();
  } 
  else {
    // Si on est en veille (START_GAME), on laisse edge_color faire le miroir du GIF
    if (bitRead(statut_game, START_GAME)) {
        strip1.show();
        strip2.show();
    } else {
        // En match (tableau des scores) : Eclairage blanc fixe
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 500) {
            color_neo(strip1.Color(255, 255, 255));
            strip1.show();
            strip2.show();
            lastUpdate = millis();
        }
    }
  }
}