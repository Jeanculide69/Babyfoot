#include "config.h"

// --- DESSIN DES CHIFFRES MODERN BOLD ---
const uint8_t BOLD_BITMAPS[10][6] = {
  {0x7C,0xC6,0xC6,0xC6,0xC6,0x7C}, {0x18,0x38,0x18,0x18,0x18,0x18},
  {0x7C,0xC6,0x06,0x3C,0x60,0xFE}, {0x7C,0xC6,0x1C,0x06,0xC6,0x7C},
  {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C}, {0xFE,0xC0,0xFC,0x06,0xC6,0x7C},
  {0x3C,0x60,0xFC,0xC6,0xC6,0x7C}, {0xFE,0xC6,0x0C,0x18,0x18,0x18},
  {0x7C,0xC6,0x7C,0xC6,0xC6,0x7C}, {0x7C,0xC6,0x7E,0x06,0x06,0x3C}
};

void drawBoldDigit(int n, int x, int y, uint16_t color) {
  for (int r = 0; r < 6; r++) {
    uint8_t row = BOLD_BITMAPS[n][r];
    for (int c = 0; c < 8; c++) {
      if (row & (1 << (7 - c))) { matrix->fillRect(x + c*2, y + r*2, 2, 2, color); }
    }
  }
}

void drawBoldNumber(int n, int x, int y, uint16_t color) {
  if (!matrix) return;
  if (n == 10) {
    matrix->fillRect(x, y, 2, 12, color);
    drawBoldDigit(0, x + 4, y, color);
    return;
  }
  int val = abs(n) % 10;
  if (n < 0) matrix->fillRect(x - 3, y + 5, 2, 2, color);
  drawBoldDigit(val, x, y, color);
}

void drawCenteredText(String text, int zoneX, int y, uint16_t color) {
  int len = text.length();
  int charWidth = (len > 4) ? 5 : 6;
  int totalWidth = len * charWidth;
  int startX = zoneX + (32 - totalWidth) / 2;
  matrix->setTextWrap(false);
  matrix->setTextColor(color);
  matrix->setCursor(startX, y);
  matrix->print(text);
}

// --- LOGIQUE DE JEU ---
extern volatile int score_p1, score_p2, ball;
extern volatile uint32_t statut_game;
extern volatile unsigned int inputs;
extern void requestAnimation(int type);
extern void playSFX(int id, bool loop);
extern bool isAnimationActive();
extern void updateAnimations();

void handleAction(String act) {
  if (act == "B1") { score_p1++; ball--; playSFX(2, false); requestAnimation(ANIM_BUT_J1); if(score_p1 >= 10) requestAnimation(ANIM_VIC_J1); }
  if (act == "B2") { score_p2++; ball--; playSFX(3, false); requestAnimation(ANIM_BUT_J2); if(score_p2 >= 10) requestAnimation(ANIM_VIC_J2); }
  if (act == "G1") { score_p2--; playSFX(4, false); requestAnimation(ANIM_GAM_J1); }
  if (act == "G2") { score_p1--; playSFX(4, false); requestAnimation(ANIM_GAM_J2); }
  if (act == "OK") { bitSet(statut_game, RUN); bitClear(statut_game, START_GAME); playSFX(1, false); }
  if (act == "BIERE") { playSFX(4, true); requestAnimation(5); }
  if (act == "P1") score_p1++; if (act == "M1") score_p1--; 
  if (act == "P2") score_p2++; if (act == "M2") score_p2--; 
}

void score_screen_starwars(bool reset = false) {
  if (!matrix) return;
  static bool wasAnimActive = false;
  static unsigned long cooldown_timer = 0;
  if (reset) { matrix->fillScreen(C_BLACK); return; }
  if (isAnimationActive()) { updateAnimations(); wasAnimActive = true; return; }
  if (!isAnimationActive() && wasAnimActive) { cooldown_timer = millis() + 1500; wasAnimActive = false; }
  if (millis() < cooldown_timer) return;

  matrix->fillScreen(C_BLACK);
  matrix->drawFastHLine(0, 0, 64, 0x18E3);
  matrix->drawFastHLine(0, 9, 64, 0x18E3);
  matrix->drawFastVLine(31, 0, 10, 0x18E3);
  drawCenteredText("JEDI", 0, 1, C_BLUE);
  drawCenteredText("SITH", 32, 1, C_RED);
  
  bool flash = (millis() / 250) % 2; 
  bool p1_v = !bitRead(statut_game, SCORE_ADJUST) || !bitRead(statut_game, SELECT_P1) || flash;
  bool p2_v = !bitRead(statut_game, SCORE_ADJUST) || !bitRead(statut_game, SELECT_P2) || flash;
  if (p1_v) drawScoreCentered(score_p1, 0, 12, C_BLUE); // Attention: drawScoreCentered doit etre defini
  if (p2_v) drawScoreCentered(score_p2, 32, 12, C_RED);
  
  matrix->drawRect(25, 17, 14, 10, 0x4208);
  matrix->setTextColor(C_YELLOW);
  matrix->setCursor(ball > 9 ? 28 : 31, 19); matrix->print(ball);
  
  extern void updateFighter(int id, int x, int y, uint16_t color);
  updateFighter(0, 10, 26, C_BLUE);
  updateFighter(1, 50, 26, C_RED);
}

// Alias pour compatibilite
void drawScoreCentered(int n, int zoneX, int y, uint16_t color) { drawBoldNumber(n, zoneX + (n >= 10 || n < 0 ? 6 : 8), y, color); }

void updateFighter(int id, int x, int y, uint16_t color) {
  static int pose[2] = {0, 0};
  pose[id] = (pose[id] + 1) % 4;
  matrix->drawPixel(x, y, color);
  matrix->drawFastVLine(x, y+1, 3, C_WHITE);
  if (pose[id] < 2) {
    matrix->drawPixel(x-1, y+4, C_WHITE); matrix->drawPixel(x-2, y+5, C_WHITE);
    matrix->drawPixel(x+1, y+4, C_WHITE); matrix->drawPixel(x+2, y+5, C_WHITE);
  } else {
    matrix->drawFastVLine(x, y+4, 2, C_WHITE);
  }
  int dir = (id == 0) ? 1 : -1;
  matrix->drawLine(x+dir, y+2, x+(dir*5), y+2, color);
}

void setupGame() { 
  score_p1 = 0; score_p2 = 0; ball = 11; 
  bitClear(statut_game, RUN); 
  score_screen_starwars(true);
}

void handleGameLogic() {
  static unsigned long lastTick = 0;
  if (millis() - lastTick < 60) return;
  lastTick = millis();
  extern void read_inputs_old();
  read_inputs_old();
  
  if (bitRead(inputs, 11)) handleAction("B1");
  if (bitRead(inputs, 13)) handleAction("B2");
  if (bitRead(inputs, 12)) handleAction("G1");
  if (bitRead(inputs, 14)) handleAction("G2");
  if (bitRead(inputs, 8))  handleAction("OK");
  if (bitRead(inputs, 1))  handleAction("M1");
  if (bitRead(inputs, 2))  handleAction("P1");

  if (!isAnimationActive() && !bitRead(statut_game, RUN)) {
    extern void drawAnimStandby(); drawAnimStandby();
  } else {
    score_screen_starwars();
  }

  if (bitRead(statut_game, RUN) && ball <= 0) { 
    bitClear(statut_game, RUN); bitSet(statut_game, START_GAME); 
    matrix->fillScreen(C_BLACK); 
    if (score_p1 > score_p2) { playSFX(5, false); requestAnimation(ANIM_VIC_J1); } 
    else { playSFX(6, false); requestAnimation(ANIM_VIC_J2); }
    for(int i=0; i<100; i++) { if(isAnimationActive()) updateAnimations(); delay(50); }
  }
}
