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
#include "animations/balle_match.c"


// Les constantes ANIM_ sont maintenant centralisées dans config.h

int active_anim = ANIM_NONE;
int anim_frame = 0;
unsigned long last_anim_ms = 0;
unsigned long start_anim_ms = 0;

void requestAnimation(int type) {
  active_anim = type;
  anim_frame = 0;
  start_anim_ms = millis();
  
  // Declenchement sonore automatique pour les animations speciales
  if (type == ANIM_BALLE_MATCH) playSFX(SFX_MATCH_PT); 
}

bool isAnimationActive() { return active_anim != ANIM_NONE; }

int current_frame_standby = 0;
unsigned long last_anim_millis = 0;

void drawStarWarsGIF() {
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

      // Miroir Ambilight UNIQUEMENT sur les bords
      if (x == 0 || x == 63 || y == 31) {
        edge_color(x, y, strip1.Color(r * 17, g * 17, b * 17));
      }
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
      
      // Ambilight "Old School" : Miroir du bord UNIQUEMENT
      if (x == 0 || x == 63 || y == 31) {
        edge_color(x, y, strip1.Color(r * 17, g * 17, b * 17));
      }
    }
  }
  anim_frame++;
  if (anim_frame >= max_frames) {
    if (loop) {
        anim_frame = 0;
    } else if (active_anim == ANIM_BALLE_MATCH && (millis() - start_anim_ms < 7000)) {
        anim_frame = max_frames - 1; // On reste sur la derniere image
    } else if ((active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) && (millis() - start_anim_ms < 5000)) {
        anim_frame = max_frames - 1; // On reste sur la derniere image (Victoire)
    } else {
      if (active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) {
        requestAnimation(ANIM_BIERE);
      } else {
        // Fin d'un but, de la bière ou de la balle de match -> on relance l'ambiance
        if (active_anim == ANIM_BIERE) playSFX(SFX_INTRO, true); 
        else playSFX(SFX_AMBIANCE, true); 
        active_anim = ANIM_NONE;
      }
    }
  }
}

void updateAnimations() {
  switch(active_anim) {
    case ANIM_BUT_J1: drawGenericAnim(but_bleu_data, BUT_BLEU_FRAME_COUNT, false); break;
    case ANIM_BUT_J2: drawGenericAnim(but_rouge_data, BUT_ROUGE_FRAME_COUNT, false); break;
    case ANIM_GAM_J1: drawGenericAnim(gamelle_bleu_data, GAMELLE_BLEU_FRAME_COUNT, false); break;
    case ANIM_GAM_J2: drawGenericAnim(gamelle_rouge_data, GAMELLE_ROUGE_FRAME_COUNT, false); break;
    case ANIM_BIERE:  drawGenericAnim(pause_biere_data, PAUSE_BIERE_FRAME_COUNT, true); break;
    case ANIM_VIC_J1: drawGenericAnim(victoire_bleu_data, VICTOIRE_BLEU_FRAME_COUNT, false); break;
    case ANIM_VIC_J2: drawGenericAnim(victoire_rouge_data, VICTOIRE_ROUGE_FRAME_COUNT, false); break;
    case ANIM_BALLE_MATCH: drawGenericAnim(balle_match_data, BALLE_MATCH_FRAME_COUNT, false); break;
  }
}
