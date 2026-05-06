#include "config.h"

// --- DESSIN DES CHIFFRES MODERN BOLD ---
const uint8_t BOLD_BITMAPS[10][6] = {
  {0x7C,0xC6,0xC6,0xC6,0xC6,0x7C}, {0x18,0x38,0x18,0x18,0x18,0x18},
  {0x7C,0xC6,0x06,0x3C,0x60,0xFE}, {0x7C,0xC6,0x1C,0x06,0xC6,0x7C},
  {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C}, {0xFE,0xC0,0xFC,0x06,0xC6,0x7C},
  {0x3C,0x60,0xFC,0xC6,0xC6,0x7C}, {0xFE,0xC6,0x0C,0x18,0x18,0x18},
  {0x7C,0xC6,0x7C,0xC6,0xC6,0x7C}, {0x7C,0xC6,0x7E,0x06,0x06,0x3C}
};

void drawBoldNumber(int n, int x, int y, uint16_t color) {
  if (n < 0 || n > 9) return;
  for (int r = 0; r < 6; r++) {
    uint8_t row = BOLD_BITMAPS[n][r];
    for (int c = 0; c < 8; c++) {
      if (row & (1 << (7 - c))) { matrix->fillRect(x + c*2, y + r*2, 2, 2, color); }
    }
  }
}

// --- LOGIQUE DE JEU ET ACTIONS ---
extern volatile int score_p1, score_p2, ball;
extern volatile uint32_t statut_game;
extern volatile unsigned int inputs;
extern void requestAnimation(int type);
extern void playSFX(int id, bool loop);
extern bool isAnimationActive();
extern void updateAnimations();

void handleAction(String act) {
  if (act == "B1") { score_p1++; ball--; playSFX(2, false); requestAnimation(ANIM_BUT_J1); }
  if (act == "B2") { score_p2++; ball--; playSFX(3, false); requestAnimation(ANIM_BUT_J2); }
  if (act == "G1") { score_p2--; playSFX(4, false); requestAnimation(ANIM_GAM_J1); }
  if (act == "G2") { score_p1--; playSFX(4, false); requestAnimation(ANIM_GAM_J2); }
  if (act == "OK") { bitSet(statut_game, RUN); bitClear(statut_game, START_GAME); playSFX(1, false); }
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
  
  // 1. DÉCOR COCKPIT ET NOMS
  matrix->drawFastHLine(0, 0, 64, 0x18E3);
  matrix->drawFastHLine(0, 9, 64, 0x18E3);
  matrix->setTextSize(1);
  matrix->setTextColor(C_BLUE); matrix->setCursor(1, 1); matrix->print("JEDI");
  matrix->setTextColor(C_RED); matrix->setCursor(39, 1); matrix->print("SITH");
  
  // 2. SCORES MODERN BOLD
  drawBoldNumber(score_p1 % 10, 5, 12, C_BLUE);
  drawBoldNumber(score_p2 % 10, 47, 12, C_RED);

  // 3. BALLS HUD
  matrix->drawRect(25, 17, 14, 10, 0x4208);
  matrix->setTextColor(C_YELLOW);
  matrix->setCursor(ball > 9 ? 28 : 31, 19); matrix->print(ball);

  // 4. COMBATTANTS (Logique simple et robuste)
  extern void updateFighter(int id, int x, int y, uint16_t color);
  updateFighter(0, 10, 28, C_BLUE);
  updateFighter(1, 50, 28, C_RED);
}

void updateFighter(int id, int x, int y, uint16_t color) {
  static int pose[2] = {0, 0};
  pose[id] = (pose[id] + 1) % 4;
  matrix->drawPixel(x, y, color);
  matrix->drawPixel(x, y+1, C_WHITE); matrix->drawPixel(x, y+2, C_WHITE);
  if (pose[id] < 2) { matrix->drawPixel(x-1, y+3, C_WHITE); matrix->drawPixel(x+1, y+3, C_WHITE); }
  else { matrix->drawPixel(x, y+3, C_WHITE); }
  int dir = (id == 0) ? 1 : -1;
  matrix->drawLine(x+dir, y+1, x+(dir*4), y+1, color); // Sabre long
}

void setupGame() { score_p1 = 0; score_p2 = 0; ball = 11; score_screen_starwars(true); }

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
  if (bitRead(inputs, 0)) handleAction("OK");
  if (bitRead(inputs, 1)) handleAction("M1");
  if (bitRead(inputs, 2)) handleAction("P1");

  if (!isAnimationActive() && !bitRead(statut_game, RUN)) {
    extern void drawAnimStandby(); drawAnimStandby();
  } else {
    score_screen_starwars();
  }
}
