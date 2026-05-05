#include "animations/but_rouge.c"
#include "animations/but_bleu.c"
#include "animations/gamelle_rouge.c"
#include "animations/gamelle_bleu.c"
#include "animations/veille.c"
#include "animations/pause_biere.c"
#include "animations/victoire_rouge.c"
#include "animations/victoire_bleu.c"

extern MatrixPanel_I2S_DMA *dma_display;
extern int current_frame;

// Fonction de lecture générique optimisée
void playGif(const uint16_t data[][2048], int frame_count) {
  if (current_frame < frame_count) {
    for (int y = 0; y < 32; y++) {
      for (int x = 0; x < 64; x++) {
        dma_display->drawPixel(x, y, data[current_frame][y * 64 + x]);
      }
    }
    current_frame++;
  } else {
    current_frame = 0;
  }
}

// Mappage avec les noms EXACTS de vos fichiers
void drawAnimButJ1() { playGif(but_rouge_data, BUT_ROUGE_FRAME_COUNT); }
void drawAnimButJ2() { playGif(but_bleu_data, BUT_BLEU_FRAME_COUNT); }
void drawAnimGamelleJ1() { playGif(gamelle_rouge_data, GAMELLE_ROUGE_FRAME_COUNT); }
void drawAnimGamelleJ2() { playGif(gamelle_bleu_data, GAMELLE_BLEU_FRAME_COUNT); }
void drawAnimBiere() { playGif(pause_biere_data, PAUSE_BIERE_FRAME_COUNT); }
void drawAnimVictoireJ1() { playGif(victoire_rouge_data, VICTOIRE_ROUGE_FRAME_COUNT); }
void drawAnimVictoireJ2() { playGif(victoire_bleu_data, VICTOIRE_BLEU_FRAME_COUNT); }
void drawAnimStandby() { playGif(veille_data, VEILLE_FRAME_COUNT); }
void drawAnimIntro() { playGif(veille_data, VEILLE_FRAME_COUNT); }
