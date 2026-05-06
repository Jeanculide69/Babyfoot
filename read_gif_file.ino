#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;

// Inclusion des donnees d'animation (Format 444)
#include "animations/veille.c"

int current_frame_standby = 0;
unsigned long last_anim_millis = 0;

void drawAnimStandby() {
  if (!matrix) return;
  if (millis() - last_anim_millis < 50) return;
  last_anim_millis = millis();

  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      int pix = y * 64 + x;
      uint16_t color444 = veille_data[current_frame_standby][pix];
      
      // Conversion 444 -> 565 pour DMA
      uint16_t r = (color444 >> 8) & 0x0F;
      uint16_t g = (color444 >> 4) & 0x0F;
      uint16_t b = color444 & 0x0F;
      
      matrix->drawPixel(x, y, matrix->color444(r, g, b));
    }
  }

  current_frame_standby++;
  if (current_frame_standby >= VEILLE_FRAME_COUNT) {
    current_frame_standby = 0;
  }
}

void drawAnimButJ1() { }
void drawAnimButJ2() { }
void drawAnimGamelleJ1() { }
void drawAnimGamelleJ2() { }
void drawAnimBiere() { }
void drawAnimVictoireJ1() { }
void drawAnimVictoireJ2() { }
void drawAnimIntro() { }
