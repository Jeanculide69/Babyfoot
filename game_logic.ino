#include "config.h"

// --- CONSTANTES COMBAT AVANCÉ ---
#define GRAVITY 0.25
#define MAX_LASERS 6
#define JUMP_FORCE -3.8
#define RUN_SPEED 0.20

// --- STRUCTURES ---
struct Platform { int x, y, w, h; };
Platform p_jedi = {5, 12, 14, 14};
Platform p_sith = {45, 12, 14, 14};
Platform p_ball = {26, 18, 12, 8};

struct Laser { float x, y, vx; bool active; uint16_t color; };
Laser lasers[MAX_LASERS];

extern bool isAnimationActive();
extern void updateAnimations();
extern void requestAnimation(int type);

// --- CLASSE DUEL FIGHTER (IA COMBAT) ---
class DuelFighter {
public:
  float x, y, vx, vy;
  int id; uint16_t color;
  bool facingRight;
  int state; // 0: Idle/Run, 1: Jump, 2: Deflect
  unsigned long lastShoot = 0;
  unsigned long deflectTimer = 0;

  DuelFighter(int _id, float _x, float _y, uint16_t _c) {
    id = _id; x = _x; y = _y; color = _c;
    vx = 0; vy = 0; state = 0; facingRight = (id == 0);
  }

  void update(DuelFighter &opponent) {
    // 1. DÉTECTION DES DANGERS (Lasers ennemis)
    for (int i = 0; i < MAX_LASERS; i++) {
      if (lasers[i].active && lasers[i].color != color) {
        float dist = abs(lasers[i].x - x);
        if (dist < 12 && abs(lasers[i].y - y) < 4) {
          if (random(100) > 40) { state = 2; deflectTimer = millis() + 300; } // Parade
          else if (vy == 0) vy = JUMP_FORCE; // Saut d'esquive
        }
      }
    }

    // 2. IA DE POURSUITE
    if (state != 2) {
      float distOpp = opponent.x - x;
      if (abs(distOpp) > 10) { vx += (distOpp > 0) ? RUN_SPEED : -RUN_SPEED; facingRight = (distOpp > 0); }
      else { vx *= 0.5; if (random(100) > 95) vy = JUMP_FORCE; }
      
      if (millis() - lastShoot > 1800 && random(100) > 92) { shoot(); lastShoot = millis(); }
    }

    // 3. PHYSIQUE
    vy += GRAVITY; x += vx; y += vy; vx *= 0.85;
    if (millis() > deflectTimer) state = 0;

    // Collisions
    bool onGround = false;
    if (y >= 28) { y = 28; vy = 0; onGround = true; }
    checkPlatform(p_jedi, onGround);
    checkPlatform(p_sith, onGround);
    checkPlatform(p_ball, onGround);
  }

  void checkPlatform(Platform &p, bool &onGround) {
    if (vy > 0 && y + 3 >= p.y && y + 3 <= p.y + 4 && x >= p.x - 2 && x <= p.x + p.w + 1) {
      y = p.y - 4; vy = 0; onGround = true;
    }
  }

  void shoot() {
    for (int i = 0; i < MAX_LASERS; i++) {
      if (!lasers[i].active) {
        lasers[i].active = true; lasers[i].x = facingRight ? x + 3 : x - 3;
        lasers[i].y = y + 1; lasers[i].vx = facingRight ? 2.8 : -2.8;
        lasers[i].color = color; break;
      }
    }
  }

  void draw() {
    int ix = (int)x; int iy = (int)y;
    matrix->drawPixel(ix, iy, color); // Tête
    matrix->drawPixel(ix, iy+1, C_WHITE); matrix->drawPixel(ix, iy+2, C_WHITE); // Corps
    int dir = facingRight ? 1 : -1;
    
    if (state == 2) { // Parade (Sabre incliné)
      matrix->drawLine(ix+dir, iy-1, ix+dir, iy+2, color);
    } else { // Sabre en garde
      matrix->drawLine(ix+dir, iy+1, ix+(dir*4), iy+1, color);
      matrix->drawPixel(ix+(dir*2), iy, color);
    }
    
    if (abs(vx) > 0.1 && (millis()/100)%2) {
      matrix->drawPixel(ix-1, iy+3, C_WHITE); matrix->drawPixel(ix+1, iy+3, C_WHITE);
    } else {
      matrix->drawPixel(ix, iy+3, C_WHITE);
    }
  }
};

DuelFighter jedi(0, 10, 28, C_BLUE);
DuelFighter sith(1, 50, 28, C_RED);

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

// --- LOGIQUE DE JEU ---
extern volatile int score_p1, score_p2, ball;
extern volatile uint32_t statut_game;
extern volatile unsigned int inputs;
extern void playSFX(int id, bool loop);

void handleAction(String act) {
  if (act == "B1") { score_p1++; ball--; playSFX(2, false); requestAnimation(ANIM_BUT_J1); }
  if (act == "B2") { score_p2++; ball--; playSFX(3, false); requestAnimation(ANIM_BUT_J2); }
  if (act == "G1") { score_p2--; playSFX(4, false); requestAnimation(ANIM_GAM_J1); }
  if (act == "G2") { score_p1--; playSFX(4, false); requestAnimation(ANIM_GAM_J2); }
  if (act == "OK") { bitSet(statut_game, RUN); bitClear(statut_game, START_GAME); playSFX(1, true); }
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
  
  // HUD Cockpit
  matrix->setTextColor(C_BLUE); matrix->setCursor(1, 1); matrix->print("JEDI");
  matrix->setTextColor(C_RED); matrix->setCursor(39, 1); matrix->print("SITH");
  matrix->drawFastHLine(0, 0, 64, 0x18E3);
  matrix->drawFastHLine(0, 9, 64, 0x18E3);
  
  drawBoldNumber(score_p1 % 10, p_jedi.x, p_jedi.y, C_BLUE);
  drawBoldNumber(score_p2 % 10, p_sith.x, p_sith.y, C_RED);

  // Balles
  matrix->drawRect(p_ball.x-1, p_ball.y-1, p_ball.w+2, p_ball.h+2, 0x4208);
  matrix->setTextColor(C_YELLOW);
  matrix->setCursor(ball > 9 ? 28 : 31, 19); matrix->print(ball);

  // Blasters Logic
  for (int i = 0; i < MAX_LASERS; i++) {
    if (lasers[i].active) {
      lasers[i].x += lasers[i].vx;
      if (lasers[i].x < 0 || lasers[i].x > 63) lasers[i].active = false;
      else matrix->drawFastHLine((int)lasers[i].x, (int)lasers[i].y, 3, lasers[i].color);
    }
  }

  jedi.update(sith); sith.update(jedi);
  jedi.draw(); sith.draw();
}

void setupGame() { 
  score_p1 = 0; score_p2 = 0; ball = 11;
  bitClear(statut_game, RUN); // S'assurer qu'on commence en Standby
  requestAnimation(6); // Force l'intro au démarrage (ANIM_VEILLE)
}

void handleGameLogic() {
  static unsigned long lastTick = 0;
  if (millis() - lastTick < 60) return;
  lastTick = millis();

  extern void read_inputs_old();
  read_inputs_old();
  
  extern volatile unsigned int inputs;
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
