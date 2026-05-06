#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;
extern void playSFX(int id, bool loop);

// Inclusion des donnees d'animation (Format 444)
#include "animations/veille.c"
#include "animations/but_bleu.c"
#include "animations/but_rouge.c"
#include "animations/gamelle_bleu.c"
#include "animations/gamelle_rouge.c"
#include "animations/pause_biere.c"
#include "animations/victoire_bleu.c"
#include "animations/victoire_rouge.c"

#define ANIM_NONE 0
#define ANIM_BUT_J1 1
#define ANIM_BUT_J2 2
#define ANIM_GAM_J1 3
#define ANIM_GAM_J2 4
#define ANIM_BIERE  5
#define ANIM_VIC_J1 6
#define ANIM_VIC_J2 7

int active_anim = ANIM_NONE;
int anim_frame = 0;
unsigned long last_anim_ms = 0;

void requestAnimation(int type) {
  active_anim = type;
  anim_frame = 0;
}

bool isAnimationActive() { return active_anim != ANIM_NONE; }

int current_frame_standby = 0;
unsigned long last_anim_millis = 0;

void drawAnimStandby() {
  if (!matrix) return;
  if (millis() - last_anim_millis < 80) return;
  last_anim_millis = millis();

  extern void edge_color(int x, int y, uint32_t color);
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      int pix = y * 64 + x;
      uint16_t color444 = veille_data[current_frame_standby][pix];
      
      uint8_t r = (color444 >> 8) & 0x0F;
      uint8_t g = (color444 >> 4) & 0x0F;
      uint8_t b = color444 & 0x0F;
      
      uint32_t c565 = matrix->color444(r, g, b);
      matrix->drawPixel(x, y, c565);

      // Miroir Ambilight
      edge_color(x, y, strip1.Color(r << 4, g << 4, b << 4));
    }
  }

  current_frame_standby++;
  if (current_frame_standby >= VEILLE_FRAME_COUNT) {
    current_frame_standby = 0;
  }
}

void drawGenericAnim(const uint16_t data[][2048], int max_frames, bool loop) {
  if (millis() - last_anim_ms < 100) return;
  last_anim_ms = millis();
  extern void edge_color(int x, int y, uint32_t color);
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      uint16_t c = data[anim_frame][y * 64 + x];
      uint8_t r = (c >> 8) & 0x0F;
      uint8_t g = (c >> 4) & 0x0F;
      uint8_t b = c & 0x0F;
      matrix->drawPixel(x, y, matrix->color444(r, g, b));
      
      // Ambilight "Old School" : Miroir du bord
      edge_color(x, y, strip1.Color(r << 4, g << 4, b << 4));
    }
  }
  anim_frame++;
  if (anim_frame >= max_frames) {
    if (loop) anim_frame = 0;
    else {
      if (active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) {
        requestAnimation(ANIM_BIERE);
      } else {
        // Fin d'un but ou de la bière -> on relance l'ambiance sonore appropriée
        if (active_anim == ANIM_BIERE) playSFX(1, true); // 001.mp3 = Intro
        else playSFX(7, true); // 007.mp3 = Match
        active_anim = ANIM_NONE;
      }
    }
  }
}

void updateAnimations() {
  switch(active_anim) {
    case ANIM_BUT_J1: drawGenericAnim(but_rouge_data, BUT_ROUGE_FRAME_COUNT, false); break;
    case ANIM_BUT_J2: drawGenericAnim(but_bleu_data, BUT_BLEU_FRAME_COUNT, false); break;
    case ANIM_GAM_J1: drawGenericAnim(gamelle_rouge_data, GAMELLE_ROUGE_FRAME_COUNT, false); break;
    case ANIM_GAM_J2: drawGenericAnim(gamelle_bleu_data, GAMELLE_BLEU_FRAME_COUNT, false); break;
    case ANIM_BIERE:  drawGenericAnim(pause_biere_data, PAUSE_BIERE_FRAME_COUNT, true); break;
    case ANIM_VIC_J1: drawGenericAnim(victoire_rouge_data, VICTOIRE_ROUGE_FRAME_COUNT, false); break;
    case ANIM_VIC_J2: drawGenericAnim(victoire_bleu_data, VICTOIRE_BLEU_FRAME_COUNT, false); break;
  }
}
