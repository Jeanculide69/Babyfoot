#include "config.h"

// --- CONSTANTES DUEL V3 ---
#define GRAVITY 0.25
#define MAX_LASERS 4
#define JUMP_FORCE -3.2
#define RUN_SPEED 0.15

// Couleurs HUD
#define HUD_BLUE  0x00D2FF
#define HUD_RED   0xFF2A2A
#define HUD_GOLD  0xFFE0

// --- STRUCTURES ---
struct Platform { int x, y, w, h; };
Platform p_jedi = {5, 12, 12, 14};
Platform p_sith = {47, 12, 12, 14};
Platform p_ball = {26, 18, 12, 8};

struct Laser { float x, y, vx; bool active; uint16_t color; };
Laser lasers[MAX_LASERS];

// --- CLASSE FIGHTER (IA & Physique) ---
class Fighter {
public:
  float x, y, vx, vy;
  int id; uint16_t color;
  bool facingRight;
  unsigned long lastShoot = 0;

  Fighter(int _id, float _x, float _y, uint16_t _c) {
    id = _id; x = _x; y = _y; color = _c;
    vx = 0; vy = 0; facingRight = (id == 0);
  }

  void update(Fighter &opponent) {
    // 1. IA Organique
    if (x < opponent.x - 6) { vx += RUN_SPEED; facingRight = true; }
    else if (x > opponent.x + 6) { vx -= RUN_SPEED; facingRight = false; }
    
    // Tir laser aléatoire
    if (millis() - lastShoot > 2000 && random(100) > 95) {
      shoot(); lastShoot = millis();
    }

    // 2. Physique
    vy += GRAVITY;
    x += vx; y += vy;
    vx *= 0.85;

    // 3. Collisions
    bool onGround = false;
    if (y >= 28) { y = 28; vy = 0; onGround = true; } // Sol
    
    // Plateformes (Scores et Balles)
    checkPlatform(p_jedi, onGround);
    checkPlatform(p_sith, onGround);
    checkPlatform(p_ball, onGround);

    // Saut aléatoire si au sol
    if (onGround && random(100) > 98) vy = JUMP_FORCE;
  }

  void checkPlatform(Platform &p, bool &onGround) {
    if (vy > 0 && y + 3 >= p.y && y + 3 <= p.y + 4 && x >= p.x - 1 && x <= p.x + p.w) {
      y = p.y - 4; vy = 0; onGround = true;
    }
  }

  void shoot() {
    for (int i = 0; i < MAX_LASERS; i++) {
      if (!lasers[i].active) {
        lasers[i].active = true; lasers[i].x = facingRight ? x + 2 : x - 2;
        lasers[i].y = y + 1; lasers[i].vx = facingRight ? 2.0 : -2.0;
        lasers[i].color = color; break;
      }
    }
  }

  void draw() {
    int ix = (int)x; int iy = (int)y;
    matrix->drawPixel(ix, iy, color); // Tête
    matrix->drawPixel(ix, iy+1, C_WHITE); matrix->drawPixel(ix, iy+2, C_WHITE); // Corps
    int dir = facingRight ? 1 : -1;
    matrix->drawLine(ix+dir, iy+1, ix+(dir*3), iy+1, color); // Sabre
    // Jambes animées
    if (abs(vx) > 0.1 && (millis()/100)%2) {
      matrix->drawPixel(ix-1, iy+3, C_WHITE); matrix->drawPixel(ix+1, iy+3, C_WHITE);
    } else {
      matrix->drawPixel(ix, iy+3, C_WHITE);
    }
  }
};

Fighter jedi(0, 10, 28, HUD_BLUE);
Fighter sith(1, 50, 28, HUD_RED);

// --- DESSIN DES CHIFFRES MODERN BOLD ---
const uint8_t BOLD_BITMAPS[10][6] = {
  {0x7C,0xC6,0xC6,0xC6,0xC6,0x7C}, // 0
  {0x18,0x38,0x18,0x18,0x18,0x18}, // 1
  {0x7C,0xC6,0x06,0x3C,0x60,0xFE}, // 2
  {0x7C,0xC6,0x1C,0x06,0xC6,0x7C}, // 3
  {0x1C,0x3C,0x6C,0xCC,0xFE,0x0C}, // 4
  {0xFE,0xC0,0xFC,0x06,0xC6,0x7C}, // 5
  {0x3C,0x60,0xFC,0xC6,0xC6,0x7C}, // 6
  {0xFE,0xC6,0x0C,0x18,0x18,0x18}, // 7
  {0x7C,0xC6,0x7C,0xC6,0xC6,0x7C}, // 8
  {0x7C,0xC6,0x7E,0x06,0x06,0x3C}  // 9
};

void drawBoldNumber(int n, int x, int y, uint16_t color) {
  if (n < 0 || n > 9) return;
  for (int r = 0; r < 6; r++) {
    uint8_t row = BOLD_BITMAPS[n][r];
    for (int c = 0; c < 8; c++) {
      if (row & (1 << (7 - c))) {
        matrix->fillRect(x + c*2, y + r*2, 2, 2, color);
      }
    }
  }
}

// --- LOGIQUE DE JEU ET ACTIONS ---
volatile int score_p1 = 0, score_p2 = 0, ball = 11;
extern volatile uint32_t statut_game;
extern void requestAnimation(int type);
extern void playSFX(int id, bool loop = false);

void handleAction(String act) {
  if (act == "B1") { score_p1++; ball--; playSFX(2); requestAnimation(ANIM_BUT_J1); }
  if (act == "B2") { score_p2++; ball--; playSFX(3); requestAnimation(ANIM_BUT_J2); }
  if (act == "G1") { score_p2--; playSFX(4); requestAnimation(ANIM_GAM_J1); }
  if (act == "G2") { score_p1--; playSFX(4); requestAnimation(ANIM_GAM_J2); }
  if (act == "OK") { bitClear(statut_game, RUN); bitSet(statut_game, START_GAME); playSFX(1, true); }
  if (act == "P1") score_p1++;
  if (act == "M1") score_p1--; 
  if (act == "P2") score_p2++;
  if (act == "M2") score_p2--; 
}

void score_screen_starwars(bool reset = false) {
  if (!matrix) return;
  static bool wasAnimActive = false;
  static unsigned long cooldown_timer = 0;

  if (reset) { matrix->fillScreen(C_BLACK); return; }

  bool animActive = isAnimationActive();
  if (!animActive && wasAnimActive) { cooldown_timer = millis() + 1500; wasAnimActive = false; }
  if (animActive) { updateAnimations(); wasAnimActive = true; return; }
  if (millis() < cooldown_timer) return;

  // 1. DÉCOR COCKPIT (HUD)
  matrix->fillScreen(C_BLACK);
  matrix->drawFastHLine(0, 2, 64, 0x3186); // Ligne technique haut
  matrix->drawFastHLine(0, 30, 64, 0x3186); // Ligne technique bas
  
  // 2. SCORES MODERN BOLD
  drawBoldNumber(score_p1 % 10, p_jedi.x, p_jedi.y, HUD_BLUE);
  drawBoldNumber(score_p2 % 10, p_sith.x, p_sith.y, HUD_RED);

  // 3. BALLES CENTRALES
  matrix->fillRect(p_ball.x, p_ball.y, p_ball.w, p_ball.h, 0x0841); // Fond gris sombre
  matrix->setTextSize(1); matrix->setTextColor(C_YELLOW);
  matrix->setCursor(ball > 9 ? 28 : 31, 19); matrix->print(ball);

  // 4. DUEL ET LASERS
  for (int i = 0; i < MAX_LASERS; i++) {
    if (lasers[i].active) {
      lasers[i].x += lasers[i].vx;
      if (lasers[i].x < 0 || lasers[i].x > 63) lasers[i].active = false;
      else matrix->drawPixel((int)lasers[i].x, (int)lasers[i].y, lasers[i].color);
    }
  }

  jedi.update(sith); sith.update(jedi);
  jedi.draw(); sith.draw();
}

void setupGame() {
  score_p1 = 0; score_p2 = 0; ball = 11;
  score_screen_starwars(true);
}

void handleGameLogic() {
  static unsigned long lastTick = 0;
  if (millis() - lastTick < 60) return; // 16 FPS
  lastTick = millis();

  extern void read_inputs_old();
  read_inputs_old();
  
  extern volatile unsigned int inputs;
  if (bitRead(inputs, 11)) handleAction("B1");
  if (bitRead(inputs, 13)) handleAction("B2");
  if (bitRead(inputs, 12)) handleAction("G1");
  if (bitRead(inputs, 14)) handleAction("G2");

  if (!isAnimationActive() && !bitRead(statut_game, RUN)) {
    extern void drawAnimStandby(); drawAnimStandby();
  } else {
    score_screen_starwars();
  }
}
