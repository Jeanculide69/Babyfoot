#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <LittleFS.h>
#include <AnimatedGIF.h>
#include <Adafruit_NeoPixel.h>

extern MatrixPanel_I2S_DMA *matrix;
extern void playSFX(int id, bool loop);
extern Adafruit_NeoPixel strip1;
extern Adafruit_NeoPixel strip2;
extern void color_neo(uint32_t c);
extern volatile unsigned long statut_game;

// Prototypes pour AnimatedGIF (évite les erreurs de pré-processeur Arduino)
void * GIFOpenFile(const char *fname, int32_t *pSize);
void GIFCloseFile(void *pHandle);
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
void GIFDraw(GIFDRAW *pDraw);

AnimatedGIF gif;
File gifFile;

int active_anim = ANIM_NONE;
bool playingGif = false;
unsigned long nextFrameTime = 0;
unsigned long start_anim_ms = 0;

// ==========================================
// CALLBACKS ANIMATEDGIF (LittleFS)
// ==========================================
void * GIFOpenFile(const char *fname, int32_t *pSize) {
  gifFile = LittleFS.open(fname, "r");
  if (gifFile) {
    *pSize = gifFile.size();
    return (void *)&gifFile;
  }
  return NULL;
}
void GIFCloseFile(void *pHandle) {
  if (gifFile) gifFile.close();
}
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    int32_t iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0) return 0;
    iBytesRead = (int32_t)gifFile.read(pBuf, iBytesRead);
    pFile->iPos = gifFile.position();
    return iBytesRead;
}
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
  gifFile.seek(iPosition);
  pFile->iPos = (int32_t)gifFile.position();
  return pFile->iPos;
}

void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line
  if (y >= 32 || pDraw->iX >= 64) return;
  s = pDraw->pPixels;
  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > 64) iWidth = 64 - pDraw->iX;
  
  if (pDraw->ucHasTransparency) {
    uint8_t c, ucTransparent = pDraw->ucTransparent;
    for (x=0; x<iWidth; x++) {
      c = *s++;
      if (c != ucTransparent) {
        matrix->drawPixel(pDraw->iX + x, y, usPalette[c]);
      }
    }
  } else {
    for (int x=0; x<iWidth; x++) {
       matrix->drawPixel(pDraw->iX + x, y, usPalette[*s++]);
    }
  }
}

// ==========================================
// CONTROLEUR D'ANIMATION
// ==========================================

void startGIF(const char *fname, bool silent = false) {
    if (gifFile) gifFile.close();
    if (gif.open(fname, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        playingGif = true;
        nextFrameTime = millis();
    } else {
        if(!silent) Serial.printf("❌ Erreur : Impossible de lire %s\n", fname);
        playingGif = false; // Explicite
    }
}

void requestAnimation(int type) {
  active_anim = type;
  start_anim_ms = millis();
  
  if (type == ANIM_BALLE_MATCH) playSFX(SFX_MATCH_PT, false); 
  
  switch(type) {
    case ANIM_BUT_J1: startGIF("/But Bleu.gif"); break;
    case ANIM_BUT_J2: startGIF("/But Rouge.gif"); break;
    case ANIM_GAM_J1: startGIF("/gamelle_bleu.gif"); break;
    case ANIM_GAM_J2: startGIF("/gamelle_rouge.gif"); break;
    case ANIM_BIERE:  startGIF("/pause_biere.gif"); break;
    case ANIM_VIC_J1: 
      startGIF("/victoire_bleu.gif", true); 
      if(!playingGif) startGIF("/Victoire Bleu.gif"); 
      break;
    case ANIM_VIC_J2: 
      startGIF("/victoire_rouge.gif", true); 
      if(!playingGif) startGIF("/Victoire Rouge.gif"); 
      break;
    case ANIM_BALLE_MATCH: startGIF("/balle_de_match_expert.gif"); break;
    case ANIM_DEMI: startGIF("/demi.gif"); break;
  }
}

bool isAnimationActive() { return playingGif || active_anim != ANIM_NONE; }

// ==========================================
// FAKE AMBILIGHT (V2.0)
// ==========================================
void updateFakeAmbilight() {
   if (!playingGif) return;
   if (active_anim == ANIM_BUT_J1) {
      // Pulsation Bleue
      int val = (millis() % 500 > 250) ? 255 : 50;
      color_neo(strip1.Color(0, 0, val));
   } else if (active_anim == ANIM_BUT_J2) {
      // Pulsation Rouge
      int val = (millis() % 500 > 250) ? 255 : 50;
      color_neo(strip1.Color(val, 0, 0));
   } else if (active_anim == ANIM_GAM_J1 || active_anim == ANIM_GAM_J2) {
      // Clignotement Alterné (Police)
      if ((millis() / 150) % 2 == 0) color_neo(strip1.Color(255, 0, 0));
      else color_neo(strip1.Color(0, 0, 255));
   } else if (active_anim == ANIM_VIC_J1) {
      // Feu d'artifice Bleu/Vert
      if ((millis() / 200) % 2 == 0) color_neo(strip1.Color(0, 255, 0));
      else color_neo(strip1.Color(0, 100, 255));
   } else if (active_anim == ANIM_VIC_J2) {
      // Feu d'artifice Rouge/Orange
      if ((millis() / 200) % 2 == 0) color_neo(strip1.Color(255, 50, 0));
      else color_neo(strip1.Color(255, 0, 0));
   } else if (active_anim == ANIM_BALLE_MATCH) {
      // Pulsation rapide Tension
      int val = (millis() % 300 > 150) ? 255 : 20;
      color_neo(strip1.Color(val, val, val));
   } else if (active_anim == ANIM_DEMI) {
      // Stroboscope Jaune (Avertissement)
      int val = (millis() % 200 > 100) ? 255 : 0;
      color_neo(strip1.Color(val, val, 0));
   } else if (active_anim == ANIM_BIERE) {
      // Jaune biere fixe
      color_neo(strip1.Color(255, 200, 0));
   }
   strip1.show();
   strip2.show();
}

// ==========================================
// MISE A JOUR PRINCIPALE
// ==========================================
void drawStarWarsGIF() {
  if (!matrix) return;
  // Mode Veille -> on joue veille.gif en boucle
  if (!playingGif) {
      static unsigned long lastVeilleTry = 0;
      static bool firstVeille = true;
      if (firstVeille || millis() - lastVeilleTry > 5000) { // On ne réessaie que toutes les 5s si ça échoue (sauf la 1ere fois)
          lastVeilleTry = millis();
          firstVeille = false;
          startGIF("/veille.gif", true); // Essayer en minuscule d'abord
          if(!playingGif) startGIF("/Veille.gif"); // Fallback
      }
  }
  
  if (playingGif && millis() >= nextFrameTime) {
      int delayMs = 0;
      if (gif.playFrame(false, &delayMs)) {
          nextFrameTime = millis() + delayMs;
      } else {
          // Boucle sur veille : On ferme proprement et on relancera au prochain cycle avec le délai
          gif.close();
          playingGif = false;
      }
      // Veille Ambilight : Respiration lente
      int br = 50 + 50 * sin(millis() / 1000.0);
      color_neo(strip1.Color(br, br, br));
      strip1.show();
      strip2.show();
  }
}

void updateAnimations() {
  if (active_anim == ANIM_NONE) {
      return;
  }

  // Securité : Force l'arrêt des animations après un certain temps
  unsigned long maxDuration = 3000; 
  if (active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) maxDuration = 5000;
  if (active_anim == ANIM_BIERE) maxDuration = 15000;
  if (active_anim == ANIM_BALLE_MATCH) maxDuration = 7000;

  bool forceStop = (millis() - start_anim_ms > maxDuration);

  // Si le fichier GIF n'a pas pu être lu, on zappe directement la durée de l'animation
  if (!playingGif) {
      if (forceStop) {
          // On passe à la suite (simule la fin)
          if (active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) {
            requestAnimation(ANIM_BIERE);
          } else {
            if (active_anim == ANIM_BIERE) playSFX(SFX_INTRO, true); 
            else playSFX(SFX_AMBIANCE, true); 
            active_anim = ANIM_NONE;
          }
      }
      return;
  }

  if (playingGif) {
     updateFakeAmbilight();

     if (millis() >= nextFrameTime || forceStop) {
        int delayMs = 0;
        // Si forceStop, on simule la fin du GIF
        if (!forceStop && gif.playFrame(false, &delayMs)) {
            nextFrameTime = millis() + delayMs;
        } else {
            // FIN DE L'ANIMATION
            gif.close();
            playingGif = false;
            
            // Logique d'enchaînement
            if (active_anim == ANIM_VIC_J1 || active_anim == ANIM_VIC_J2) {
              requestAnimation(ANIM_BIERE);
            } else {
              if (active_anim == ANIM_BIERE) playSFX(SFX_INTRO, true); 
              else playSFX(SFX_AMBIANCE, true); 
              active_anim = ANIM_NONE;
            }
        }
     }
  }
}
