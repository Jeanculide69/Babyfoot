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

void color_neo(uint32_t c) {
  for(int i = 0; i < LED_COUNT; i++) {
    strip1.setPixelColor(i, c);
    strip2.setPixelColor(i, c);
  }
}

// Fonction "Miroir" de l'ancien code
void edge_color(int x, int y, uint32_t color) {  
  // Bord gauche (Strip 1)
  if (x == 0 && y < 31) {               
    strip1.setPixelColor(y, color);
  }
  // Bord bas gauche (Strip 1 suite)
  if (y == 31 && x < 9) {
    strip1.setPixelColor(y + x, color);                        
  }  
  // Bord droit (Strip 2)
  if (x == 63 && y < 31) { 
    strip2.setPixelColor(y, color);
  }
  // Bord bas droit (Strip 2 suite)
  if (y == 31 && x > 52) {
    strip2.setPixelColor(x - 22, color);                          
  }
}

void updateLEDs() {
  static bool wasAnimActive = false;
  bool animActive = isAnimationActive();

  if (animActive) {
    strip1.show();
    strip2.show();
    wasAnimActive = true;
  } 
  else if (wasAnimActive || millis() < 5000) { // On force le blanc au démarrage ou après une anim
    color_neo(0x00FFFFFF); 
    strip1.show();
    strip2.show();
    wasAnimActive = false;
  }
}
