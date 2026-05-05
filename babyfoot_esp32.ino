/* 
 * BABYFOOT DMA SANDBOX V1.22
 * Cible : ESP32 Core v3.3.8
 * Affichage : HUB75 64x32 via ESP32-HUB75-MatrixPanel-I2S-DMA
 * Animations : Echantillonnage 15 frames sur toute la durée du GIF
 */
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// --- CONFIGURATION DALLE LED ---
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1

MatrixPanel_I2S_DMA *dma_display = nullptr;

// --- INCLUSION DE TOUTES LES ANIMATIONS ---
#include "animations/soccer.c"         // veille
#include "animations/But_J1.c" 
#include "animations/But_J2.c" 
#include "animations/Gamelle_J1.c" 
#include "animations/Gamelle_J2.c" 
#include "animations/Victoire_J1.c" 
#include "animations/Victoire_J2.c" 
#include "animations/Pause_Biere.c" 

// Structure pour gérer les animations simplement
struct Animation {
  const char* name;
  const uint16_t (*data)[2048]; 
  int frameCount;
  int loops; // Nombre de répétitions pour cette animation
};

Animation playlist[] = {
  {"VEILLE", soccer_data, SOCCER_FRAME_COUNT, 2},         // 15 frames * 40 * 120ms = 72s
  {"BUT ROUGE", but_j1_data, BUT_J1_FRAME_COUNT, 1}, 
  {"BUT BLEU", but_j2_data, BUT_J2_FRAME_COUNT, 1}, 
  {"GAMELLE ROUGE", gamelle_j1_data, GAMELLE_J1_FRAME_COUNT, 1}, 
  {"GAMELLE BLEU", gamelle_j2_data, GAMELLE_J2_FRAME_COUNT, 1}, 
  {"VICTOIRE ROUGE", victoire_j1_data, VICTOIRE_J1_FRAME_COUNT, 1}, 
  {"VICTOIRE BLEU", victoire_j2_data, VICTOIRE_J2_FRAME_COUNT, 1}, 
  {"PAUSE BIERE", pause_biere_data, PAUSE_BIERE_FRAME_COUNT, 2}  // 15 frames * 15 * 120ms = 27s
};

int current_anim_idx = 0;
int current_frame = 0;
int current_loop_count = 0;
unsigned long last_frame_time = 0;
int animation_speed = 130; // Plus lent pour bien voir les détails

// Conversion BGR444 vers RGB565
uint16_t convert444to565(uint16_t c444) {
  uint8_t r = (c444 & 0x00F) << 4;
  uint8_t g = (c444 & 0x0F0);
  uint8_t b = (c444 & 0xF00) >> 4;
  return dma_display->color565(r, g, b);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- MODE PLAYLIST FULL POWER V1.14 ---");

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
  mxconfig.gpio.r1 = 15; mxconfig.gpio.g1 = 2; mxconfig.gpio.b1 = 0;
  mxconfig.gpio.r2 = 4;  mxconfig.gpio.g2 = 16; mxconfig.gpio.b2 = 17;
  mxconfig.gpio.a  = 5;  mxconfig.gpio.b  = 18; mxconfig.gpio.c  = 19; mxconfig.gpio.d = 21;
  mxconfig.gpio.lat = 3; mxconfig.gpio.oe  = 23; mxconfig.gpio.clk = 22;
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  
  // LUMINOSITE MINIMUM (1)
  dma_display->setBrightness8(1); 
  dma_display->clearScreen();

  Serial.println("C'est parti pour le show !");
}

void loop() {
  if (millis() - last_frame_time > animation_speed) {
    last_frame_time = millis();

    Animation& anim = playlist[current_anim_idx];
    
    // Dessin de la frame
    for (int pix = 0; pix < 2048; pix++) {
      int y = pix / PANEL_RES_X;
      int x = pix % PANEL_RES_X;
      dma_display->drawPixel(x, y, convert444to565(anim.data[current_frame][pix]));
      
      // On laisse respirer le processeur tous les 512 pixels pour éviter le Watchdog
      if (pix % 512 == 0) yield(); 
    }

    // Gestion des frames et des boucles
    current_frame++;
    if (current_frame >= anim.frameCount) {
      current_frame = 0;
      current_loop_count++;
      
      // Si on a fini le nombre de boucles prévu pour cette animation, on passe à la suivante
      if (current_loop_count >= anim.loops) {
        current_loop_count = 0;
        current_anim_idx++;
        if (current_anim_idx >= (sizeof(playlist) / sizeof(Animation))) {
          current_anim_idx = 0;
        }
        Serial.print("Next animation: ");
        Serial.println(playlist[current_anim_idx].name);
      }
    }
  }
}
