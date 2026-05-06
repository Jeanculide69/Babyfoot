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
  if (isAnimationActive()) {
    // En mode animation, les LEDs sont mises à jour pixel par pixel par edge_color()
    // appelée depuis read_gif_file.ino. On fait juste le show() à la fin de la frame.
    strip1.show();
    strip2.show();
  } 
  else {
    // SINON BLANC PUR (Comme dans le old)
    color_neo(0x00FFFFFF); 
    strip1.show();
    strip2.show();
  }
}
