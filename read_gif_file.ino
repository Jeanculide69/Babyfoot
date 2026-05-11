#include <Adafruit_NeoPixel.h>
#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <LittleFS.h>
#include <AnimatedGIF.h>

extern MatrixPanel_I2S_DMA *matrix;
extern void playSFX(int id, bool loop);
extern Adafruit_NeoPixel strip1;
extern Adafruit_NeoPixel strip2;
extern void color_neo(uint32_t c);
extern volatile unsigned long statut_game;
extern SemaphoreHandle_t fsMutex;

// Prototype manuel pour l'IDE Arduino
void fadeAll(Adafruit_NeoPixel &strip, uint8_t amount);

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
  if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(100)) != pdTRUE) return NULL;
  gifFile = LittleFS.open(fname, "r");
  if (gifFile) {
    *pSize = gifFile.size();
    xSemaphoreGive(fsMutex);
    return (void *)&gifFile;
  }
  xSemaphoreGive(fsMutex);
  return NULL;
}
void GIFCloseFile(void *pHandle) {
  if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    if (gifFile) gifFile.close();
    xSemaphoreGive(fsMutex);
  }
}
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    int32_t iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0) return 0;
    if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(50)) != pdTRUE) return 0;
    iBytesRead = (int32_t)gifFile.read(pBuf, iBytesRead);
    pFile->iPos = gifFile.position();
    xSemaphoreGive(fsMutex);
    return iBytesRead;
}
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) {
  if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(50)) != pdTRUE) return pFile->iPos;
  gifFile.seek(iPosition);
  pFile->iPos = (int32_t)gifFile.position();
  xSemaphoreGive(fsMutex);
  return pFile->iPos;
}

void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t *usPalette;
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

bool startGIF(const char *fname, bool silent = false) {
    if (gifFile) gifFile.close();
    if (gif.open(fname, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
        playingGif = true;
        nextFrameTime = millis();
        return true;
    } else {
        if(!silent) Serial.printf("❌ Erreur : Impossible de lire %s\n", fname);
        playingGif = false;
        return false;
    }
}

void requestAnimation(int type) {
  active_anim = type;
  start_anim_ms = millis();
  
  if (type == ANIM_NONE) {
      gif.close();
      if (gifFile) gifFile.close();
      playingGif = false;
      return;
  }
  
  if (type == ANIM_BALLE_MATCH) playSFX(SFX_MATCH_PT, false); 
  
  switch(type) {
    case ANIM_BUT_J1: startGIF("/But Bleu.gif"); break;
    case ANIM_BUT_J2: startGIF("/But Rouge.gif"); break;
    case ANIM_GAM_J1: startGIF("/gamelle_bleu.gif"); break;
    case ANIM_GAM_J2: startGIF("/gamelle_rouge.gif"); break;
    case ANIM_BIERE:  startGIF("/pause_biere.gif"); break;
    case ANIM_VIC_J1: 
      if(!startGIF("/victoire_bleu.gif", true)) startGIF("/Victoire Bleu.gif"); 
      break;
    case ANIM_VIC_J2: 
      if(!startGIF("/victoire_rouge.gif", true)) startGIF("/Victoire Rouge.gif"); 
      break;
    case ANIM_BALLE_MATCH: startGIF("/balle_de_match_expert.gif"); break;
    case ANIM_DEMI: startGIF("/demi.gif"); break;
  }
}

bool isAnimationActive() { return active_anim != ANIM_NONE; }

// ==========================================
// FONCTION UTILITAIRE POUR EFFETS DE TRAÎNÉE
// ==========================================
void fadeAll(Adafruit_NeoPixel &strip, uint8_t amount) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    uint32_t c = strip.getPixelColor(i);
    uint8_t r = (uint8_t)(c >> 16);
    uint8_t g = (uint8_t)(c >> 8);
    uint8_t b = (uint8_t)c;
    strip.setPixelColor(i, strip.Color(r > amount ? r - amount : 0,
                                       g > amount ? g - amount : 0,
                                       b > amount ? b - amount : 0));
  }
}

// ==========================================
// MOTEUR D'ANIMATIONS "SPECTACLE" (V2.7 Premium)
// ==========================================
void updateFakeAmbilight() {
   if (!playingGif) return;
   
   uint32_t t = millis();
   static uint8_t last_anim = 255;
   static uint32_t anim_start = 0;

   // Réinitialisation au changement d'animation
   if (active_anim != last_anim) {
       anim_start = t;
       last_anim = active_anim;
       strip1.clear();
       strip2.clear();
   }
   
   uint32_t elapsed = t - anim_start;

   if (active_anim == ANIM_BALLE_MATCH) {
      float wave = (sin(t / 150.0) + 1.0) / 2.0; 
      int val = wave * 255;
      if (elapsed < 3500) { // Duel (Réduit de 5500 à 3500)
          strip1.fill(strip1.Color(0, 0, val));
          strip2.fill(strip2.Color(val, 0, 0));
      } else { // Final Yellow
          strip1.fill(strip1.Color(val, val, 0));
          strip2.fill(strip2.Color(val, val, 0));
      }

   } else if (active_anim == ANIM_VIC_J1) {
      fadeAll(strip1, 15); fadeAll(strip2, 15);
      if (random(10) > 5) {
          uint32_t c = strip1.Color(0, 255, 255);
          strip1.setPixelColor(random(strip1.numPixels()), c);
          strip2.setPixelColor(random(strip2.numPixels()), c);
      }

   } else if (active_anim == ANIM_VIC_J2) {
      fadeAll(strip1, 15); fadeAll(strip2, 15);
      if (random(10) > 5) {
          uint32_t c = strip1.Color(255, 0, 0);
          strip1.setPixelColor(random(strip1.numPixels()), c);
          strip2.setPixelColor(random(strip2.numPixels()), c);
      }

   } else if (active_anim == ANIM_BUT_J1 || active_anim == ANIM_BUT_J2) {
      fadeAll(strip1, 40); fadeAll(strip2, 40);
      uint16_t pos = (elapsed / 15) % strip1.numPixels(); 
    if (active_anim == ANIM_BUT_J1) {
          uint32_t c = strip1.Color(0, 0, 255); // Bleu JEDI
          strip1.setPixelColor(pos, c); 
          strip2.setPixelColor(pos, c);
      } else {
          uint32_t c = strip1.Color(255, 0, 0); // Rouge SITH
          strip1.setPixelColor(strip1.numPixels() - 1 - pos, c); 
          strip2.setPixelColor(strip2.numPixels() - 1 - pos, c);
      }

   } else if (active_anim == ANIM_DEMI) {
      if (elapsed < 3000) {
          strip1.fill(strip1.Color(0, 0, 150));
          strip2.fill(strip2.Color(150, 0, 0));
      } else {
          // Transition vers le JAUNE après 3 secondes
          strip1.fill(strip1.Color(150, 150, 0));
          strip2.fill(strip2.Color(150, 150, 0));
      }
      if (random(10) > 8) {
          strip1.fill(strip1.Color(255, 255, 255));
          strip2.fill(strip2.Color(255, 255, 255));
      }

   } else if (active_anim == ANIM_GAM_J1 || active_anim == ANIM_GAM_J2) {
      int b = 255 - ((elapsed % 400) * 255 / 400); 
      if (b < 0) b = 0;
      uint32_t c = (active_anim == ANIM_GAM_J1) ? strip1.Color(0, b, b) : strip1.Color(b, 0, 0);
      strip1.fill(c); strip2.fill(c);

   } else if (active_anim == ANIM_BIERE) {
      fadeAll(strip1, 30); fadeAll(strip2, 30);
      strip1.fill(strip1.Color(180, 100, 0));
      strip2.fill(strip2.Color(180, 100, 0));
      if (random(10) > 6) {
          strip1.setPixelColor(random(strip1.numPixels()), strip1.Color(255, 255, 255));
          strip2.setPixelColor(random(strip2.numPixels()), strip2.Color(255, 255, 255));
      }
   }
   strip1.show();
   strip2.show();
}

// ==========================================
// MISE A JOUR PRINCIPALE
// ==========================================
void drawStarWarsGIF() {
  if (!matrix) return;
  
  // Mode Veille -> on joue veille.gif en boucle si aucune animation n'est active
  if (!playingGif && active_anim == ANIM_NONE) {
      static unsigned long lastErrorTime = 0;
      if (millis() - lastErrorTime > 5000) { // Guard for file errors
          if (startGIF("/veille.gif", true) || startGIF("/Veille.gif", true)) {
              lastErrorTime = 0; // Reset error guard on success
          } else {
              lastErrorTime = millis(); // Set error guard on failure
          }
      }
  }
  
  if (playingGif && millis() >= nextFrameTime) {
      int delayMs = 0;
      if (gif.playFrame(false, &delayMs)) {
          nextFrameTime = millis() + delayMs;
      } else {
          // Fin du GIF : On ferme proprement
          gif.close();
          playingGif = false;
          // Si on est en veille, le prochain appel à drawStarWarsGIF() relancera le GIF au prochain loop()
      }

      // Effet Ambilight de veille (seulement si pas d'animation active)
      if (active_anim == ANIM_NONE) {
          int br = 40 + 40 * sin(millis() / 800.0);
          color_neo(strip1.Color(br, br, br));
          strip1.show();
          strip2.show();
      }
      
      // V2.7 Fix: Laisser respirer le stack WiFi entre les frames de veille
      vTaskDelay(pdMS_TO_TICKS(5));
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
  if (active_anim == ANIM_DEMI) maxDuration = 6000;

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