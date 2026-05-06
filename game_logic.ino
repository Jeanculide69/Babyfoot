#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;

// Variables globales
volatile int score_p1 = 0, score_p2 = 0, ball = 11;
volatile unsigned long statut_game = 0;
volatile unsigned int inputs = 0;
int sens_set = 500; 

Particle particles[5];
Fighter jedi1 = {15, 31, 0, 0, C_BLUE, 1, 30};
Fighter jedi2 = {45, 31, 0, 0, C_RED, -1, 34};

void updateFighter(Fighter &f, Fighter &other);
void drawStickman(Fighter &f);
void drawParticles();

void updateFighter(Fighter &f, Fighter &other) {
  f.frame++;
  
  // Changement de cible aleatoire pour mouvement "organique"
  if (f.frame % 60 == 0 || f.x == f.targetX) {
    f.targetX = random(2, 61);
  }

  // Animation d'attaque/esquive aleatoire
  if (f.frame % 25 == 0 && f.state == 0) {
    int r = random(0, 100);
    if (abs(f.x - other.x) < 15) { // Attaque si proche
       if (r < 50) f.state = 1; 
    }
    if (r > 80) f.state = 2; // Saut
    else if (r > 90) f.state = 3; // Accroupi
  }

  // Mouvement vers la cible
  if (f.state == 0) {
    if (f.frame % 2 == 0) { // Vitesse moderee
      if (f.x < f.targetX) f.x++;
      else if (f.x > f.targetX) f.x--;
    }
  }

  // Toujours faire face a l'adversaire
  f.dir = (f.x < other.x) ? 1 : -1;

  if (f.state != 0 && f.frame % 12 == 0) f.state = 0;
}

void drawStickman(Fighter &f) {
  int x = f.x;
  int y = f.y - 1; 
  int ly = y; 
  
  if (f.state == 3) { // Crouch
    matrix->drawFastVLine(x, y - 1, 2, f.color);
    matrix->drawPixel(x, y - 2, C_WHITE);
  } else if (f.state == 2) { // Jump
    y -= 1; // Saut reduit pour ne pas depasser
    matrix->drawFastVLine(x, y - 2, 3, f.color);
    matrix->drawPixel(x, y - 3, C_WHITE);
    ly = y;
  } else { // Normal
    matrix->drawFastVLine(x, y - 2, 3, f.color);
    matrix->drawPixel(x, y - 3, C_WHITE);
  }

  int leg_offset = (f.frame / 2) % 2;
  matrix->drawPixel(x - leg_offset, ly, f.color);
  matrix->drawPixel(x + leg_offset, ly, f.color);

  // Sabres (longueur reduite pour ne pas depasser la zone de nettoyage)
  int sx2, sy2;
  if (f.state == 1) { 
    sx2 = x + (f.dir * 4); 
    sy2 = y - 1 + (sin(f.frame * 0.5) * 2); 
  } else { 
    sx2 = x + (f.dir * 3); 
    sy2 = y - 2 + (cos(f.frame * 0.2) * 1); 
  }
  matrix->drawLine(x + f.dir, y - 1, sx2, sy2, f.color);
}

void drawParticles() {
  for (int i = 0; i < 5; i++) {
    if (particles[i].life > 0) {
      matrix->drawPixel((int)particles[i].x, (int)particles[i].y, C_YELLOW);
      particles[i].x += particles[i].vx;
      particles[i].y += particles[i].vy;
      particles[i].life--;
    }
  }
}

void score_screen_starwars(bool reset = false) {
  if (!matrix) return;
  static int last_s1 = -1, last_s2 = -1, last_b = -1;
  if (reset) { last_s1 = -1; last_s2 = -1; last_b = -1; return; }

  if (last_s1 == -1) {
    matrix->fillScreen(C_BLACK); matrix->setTextWrap(false); matrix->setTextSize(1);
    matrix->setTextColor(C_BLUE); matrix->setCursor(1, 0); matrix->print("JEDI");
    matrix->setTextColor(C_RED); matrix->setCursor(39, 0); matrix->print("SITH");
    last_s1 = -2; 
  }

  // Scores a y=10 (Compromis pour laisser la place a l'animation dessous)
  if (score_p1 != last_s1) { 
    matrix->setTextSize(2); matrix->setTextColor(C_BLUE, C_BLACK); 
    matrix->setCursor(score_p1 > 9 ? 1 : 5, 10); matrix->print(score_p1); 
    last_s1 = score_p1; 
  }
  if (score_p2 != last_s2) { 
    matrix->setTextSize(2); matrix->setTextColor(C_RED, C_BLACK); 
    // Decale de 8 pixels vers la droite (max 63)
    // 1 chiffre : 43 + 8 = 51. 2 chiffres : 39 + 3 (max possible) = 42.
    matrix->setCursor(score_p2 > 9 ? 42 : 51, 10); matrix->print(score_p2); 
    last_s2 = score_p2; 
  }
  if (ball != last_b) { 
    matrix->fillRect(26, 10, 12, 14, C_BLACK); 
    matrix->setTextSize(1); matrix->setTextColor(C_YELLOW, C_BLACK); 
    if (ball > 9) matrix->setCursor(27, 13); else matrix->setCursor(30, 13); 
    matrix->print(ball); 
    last_b = ball; 
  }

  // --- NETTOYAGE LARGE (y=26 a 31) ---
  static unsigned long lastCombat = 0;
  if (millis() - lastCombat > 50) {
    lastCombat = millis();
    matrix->fillRect(0, 26, 64, 6, C_BLACK); 
    updateFighter(jedi1, jedi2);
    updateFighter(jedi2, jedi1);
    drawStickman(jedi1);
    drawStickman(jedi2);
    drawParticles();
  }
}

void setupGame() {
  score_p1 = 0; score_p2 = 0; ball = 11; statut_game = 0;
  bitSet(statut_game, START_GAME);
  if (matrix) matrix->fillScreen(C_BLACK);
}

void read_inputs_old() {
  int val_ok = touchRead(BTN_OK);
  bool ok_raw = (val_ok < sens_set && val_ok > 1);
  bool g_r = digitalRead(GOAL_RIGHT), g_l = digitalRead(GOAL_LEFT);
  auto update_edge = [](int bit, bool current) {
    if (current) { if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); } else { bitClear(inputs, bit + 8); } }
    else { bitClear(inputs, bit); bitClear(inputs, bit + 8); }
  };
  update_edge(0, ok_raw); update_edge(3, g_l); update_edge(5, g_r);
}

void handleGameLogic() {
  read_inputs_old();
  extern void drawAnimStandby();
  if (bitRead(statut_game, START_GAME)) {
    drawAnimStandby();
    if (bitRead(inputs, 8)) {
      score_p1 = 0; score_p2 = 0; ball = 11;
      bitClear(statut_game, START_GAME); bitSet(statut_game, RUN);
      score_screen_starwars(true);
    }
  }
  if (bitRead(statut_game, RUN)) {
    if (bitRead(inputs, 11)) { score_p1++; ball--; }
    if (bitRead(inputs, 13)) { score_p2++; ball--; }
    score_screen_starwars();
    if (ball <= 0) { bitClear(statut_game, RUN); bitSet(statut_game, START_GAME); matrix->fillScreen(C_BLACK); }
  }
}
void handleAction(String act) { }
