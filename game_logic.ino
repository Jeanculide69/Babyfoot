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
  if (n < 0 || n > 9 || !matrix) return;
  for (int r = 0; r < 6; r++) {
    uint8_t row = BOLD_BITMAPS[n][r];
    for (int c = 0; c < 8; c++) {
      if (row & (1 << (7 - c))) { matrix->fillRect(x + c*2, y + r*2, 2, 2, color); }
    }
  }
}
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;

// Variables globales
volatile int score_p1 = 0, score_p2 = 0, ball = 11;
volatile unsigned long statut_game = 0;
volatile unsigned int inputs = 0;
int sens_set = SENS_SET; 

volatile int less_buttom_start = 0;
volatile int more_buttom_start = 0;
volatile int ok_buttom_start = 0;

Particle particles[5];
Fighter jedi1 = {15, 31, 0, 0, C_BLUE, 1, 30};
Fighter jedi2 = {45, 31, 0, 0, C_RED, -1, 34};
Blaster blasters[4]; // Max 4 tirs simultanes

extern void sendDFCommand(uint8_t cmd, uint8_t p1, uint8_t p2);
extern volatile int cur_vol;
extern volatile int target_vol;
 
 extern void requestAnimation(int type);
 extern void updateAnimations();
 extern bool isAnimationActive();

void playSFX(int id, bool loop = false) { 
  sendDFCommand(0x0F, 0x01, (uint8_t)id); // Dossier 01, Fichier 'id'
  if (loop) {
    delay(200); 
    sendDFCommand(0x19, 0x00, 0x00); // Activer la boucle sur le morceau en cours
  }
}

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

  // --- REACTION AUX BLASTERS (ESQUIVE) ---
  for (int i = 0; i < 4; i++) {
    if (blasters[i].active) {
      if (abs(blasters[i].x - f.x) < 5 && f.state == 0) {
        if (random(0, 10) > 5) f.state = (blasters[i].y > f.y - 2) ? 2 : 3; // Sauter ou s'accroupir
      }
    }
  }
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

void updateBlasters() {
  for (int i = 0; i < 4; i++) {
    if (!blasters[i].active) {
      if (random(0, 500) < 5) { // Spawn aléatoire
        blasters[i].active = true;
        blasters[i].color = (random(0, 2) == 0) ? C_BLUE : C_RED;
        blasters[i].vx = (blasters[i].color == C_BLUE) ? 1.5 : -1.5;
        blasters[i].x = (blasters[i].vx > 0) ? -5 : 68;
        blasters[i].y = random(27, 31);
      }
    } else {
      blasters[i].x += blasters[i].vx;
      if (blasters[i].x < -10 || blasters[i].x > 74) blasters[i].active = false;
    }
  }
}

void drawBlasters() {
  for (int i = 0; i < 4; i++) {
    if (blasters[i].active) {
      matrix->drawLine((int)blasters[i].x, (int)blasters[i].y, (int)blasters[i].x + 2, (int)blasters[i].y, blasters[i].color);
    }
  }
}

void score_screen_starwars(bool reset = false) {
  if (!matrix) return;
  
  static int last_s1 = -1, last_s2 = -1, last_b = -1;
  static bool needsFullRedraw = false;
  static bool wasAnimActive = false;
  static unsigned long cooldown_timer = 0;

  if (reset) { 
    matrix->fillScreen(C_BLACK);
    last_s1 = -1; last_s2 = -1; last_b = -1; 
    needsFullRedraw = true; return; 
  }

  bool animActive = isAnimationActive();

  // Transition Animation -> Cooldown
  if (!animActive && wasAnimActive) {
    cooldown_timer = millis() + 1500; // Pause de 1.5s après l'animation
    wasAnimActive = false;
  }

  // 1. Affichage de l'animation (Plein écran, pas de HUD)
  if (animActive) {
    updateAnimations();
    wasAnimActive = true;
    needsFullRedraw = true; 
    return; // ON S'ARRÊTE ICI : Pas de HUD pendant le GIF
  }

  // 2. Gestion du Cooldown (On attend avant de montrer le score)
  if (millis() < cooldown_timer) {
    // On peut laisser le dernier pixel ou effacer. On va laisser pour l'instant.
    return; 
  }

  // 3. Dessin du HUD (Scores/Noms)
  if (needsFullRedraw || score_p1 != last_s1 || score_p2 != last_s2 || ball != last_b) {
    matrix->fillRect(0, 0, 64, 25, C_BLACK);
    
    // 1. DÉCOR COCKPIT ET NOMS
    matrix->drawFastHLine(0, 0, 64, 0x18E3);
    matrix->drawFastHLine(0, 9, 64, 0x18E3);
    matrix->setTextWrap(false); matrix->setTextSize(1);
    matrix->setTextColor(C_BLUE); matrix->setCursor(1, 1); matrix->print("JEDI");
    matrix->setTextColor(C_RED); matrix->setCursor(39, 1); matrix->print("SITH");

    matrix->setTextSize(2); 
    bool flash = (millis() / 250) % 2; 
    bool p1_v = !bitRead(statut_game, SCORE_ADJUST) || !bitRead(statut_game, SELECT_P1) || flash;
    bool p2_v = !bitRead(statut_game, SCORE_ADJUST) || !bitRead(statut_game, SELECT_P2) || flash;

    // Score JEDI BOLD
    if (p1_v) drawBoldNumber(score_p1 % 10, score_p1 > 9 ? 1 : 5, 10, C_BLUE);
  
    // Score SITH BOLD
    if (p2_v) drawBoldNumber(score_p2 % 10, score_p2 > 9 ? 42 : 51, 10, C_RED);
    
    matrix->setTextSize(1); matrix->setTextColor(C_YELLOW); 
    if (ball > 9) matrix->setCursor(27, 13); else matrix->setCursor(30, 13); 
    matrix->print(ball);

    needsFullRedraw = false;
    last_s1 = score_p1; last_s2 = score_p2; last_b = ball;
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
    updateBlasters();
    drawBlasters();
    drawParticles();
  }
}

void setupGame() {
  score_p1 = 0; score_p2 = 0; ball = 11; statut_game = 0;
  bitSet(statut_game, START_GAME);
  if (matrix) matrix->fillScreen(C_BLACK);
}

bool check_touch(int pin, volatile int &counter) {
  int val = touchRead(pin);
  if (val < SENS_SET && val > 1) { 
    if (counter < 20) counter++; 
  }
  else if (counter > 0) { counter--; }
  
  if (counter > 10) return true; // Augmenté de 5 à 10 pour plus de stabilité
  if (counter <= 0) return false;
  return false;
}

// --- FLAGS DE SIMULATION GLOBAUX (Pour WiFi et Serial) ---
static bool sim_ok = false, sim_b1 = false, sim_g1 = false, sim_b2 = false, sim_g2 = false;

void handleAction(String act) {
  if (act == "B1") sim_b1 = true;
  if (act == "B2") sim_b2 = true;
  if (act == "G1") sim_g1 = true;
  if (act == "G2") sim_g2 = true;
  if (act == "OK") sim_ok = true;
  if (act == "P1") score_p1++;
  if (act == "M1") score_p1--; 
  if (act == "P2") score_p2++;
  if (act == "M2") score_p2--; 
  Serial.print("[WIFI-SIM] Command Received: "); Serial.println(act);
}

void read_inputs_old() {
  // --- COMMANDES DE SIMULATION SERIE ---
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "B1") { sim_b1 = true; Serial.println("[SIM] BUT J1 Command"); }
    if (cmd == "G1") { sim_g1 = true; Serial.println("[SIM] GAM J1 Command"); }
    if (cmd == "B2") { sim_b2 = true; Serial.println("[SIM] BUT J2 Command"); }
    if (cmd == "G2") { sim_g2 = true; Serial.println("[SIM] GAM J2 Command"); }
    if (cmd == "OK") { sim_ok = true; Serial.println("[SIM] OK Command"); }
  }

  bool ok_raw = check_touch(BTN_OK, ok_buttom_start) || sim_ok;
  bool less_raw = check_touch(BTN_LESS, less_buttom_start);
  bool more_raw = check_touch(BTN_MORE, more_buttom_start);

  bool g_r = !digitalRead(GOAL_RIGHT) || sim_b2;
  bool g_l = !digitalRead(GOAL_LEFT) || sim_b1;
  bool gam_r = !digitalRead(GAMELLE_RIGHT) || sim_g2;
  bool gam_l = !digitalRead(GAMELLE_LEFT) || sim_g1;

  // Reset des flags de simulation
  sim_ok = sim_b1 = sim_g1 = sim_b2 = sim_g2 = false;

  auto update_edge = [](int bit, bool current) {
    if (current) { 
      if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); } 
      else { bitClear(inputs, bit + 8); } 
    }
    else { bitClear(inputs, bit); bitClear(inputs, bit + 8); }
  };

  update_edge(0, ok_raw); 
  update_edge(1, less_raw); 
  update_edge(2, more_raw);

  auto update_sensor = [](int bit, bool current) {
    if (current) { 
      if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); } 
      else { bitClear(inputs, bit + 8); }
    } else { bitClear(inputs, bit); bitClear(inputs, bit + 8); }
  };

  update_sensor(3, g_l);
  update_sensor(4, gam_l);
  update_sensor(5, g_r);
  update_sensor(6, gam_r);
}

void handleGameLogic() {
  static unsigned long lastLoop = 0;
  if (millis() - lastLoop < 60) return; // Limite à ~15 FPS
  lastLoop = millis();

  read_inputs_old();

  extern void drawAnimStandby();
  if (bitRead(statut_game, START_GAME)) {
    if (isAnimationActive()) updateAnimations();
    else drawAnimStandby();

    if (bitRead(inputs, 8)) { // Touche OK
      score_p1 = 0; score_p2 = 0; ball = 11;
      bitClear(statut_game, START_GAME); bitSet(statut_game, RUN);
      bitClear(inputs, 8); 
      requestAnimation(ANIM_NONE); 
      score_screen_starwars(true);
      playSFX(8, false); // Signal départ
      Serial.println("[GAME] --- MATCH STARTED ! ---");
      static unsigned long startMatchMs = millis();
    }
  }

  // --- GESTION NON-BLOQUANTE DU SON DE MATCH (008 -> 007) ---
  static unsigned long startMatchTimer = 0;
  static bool matchAmbienceTriggered = false;
  if (bitRead(statut_game, RUN) && !bitRead(statut_game, START_GAME)) {
      if (startMatchTimer == 0) startMatchTimer = millis();
      if (!matchAmbienceTriggered && (millis() - startMatchTimer > 2500)) {
          playSFX(7, true);
          matchAmbienceTriggered = true;
      }
  } else {
      startMatchTimer = 0;
      matchAmbienceTriggered = false;
  }

  if (bitRead(statut_game, RUN)) {
    // Mode Correction
    if (bitRead(statut_game, SCORE_ADJUST)) {
      if (!bitRead(statut_game, PLAYER_CONFIRMED)) {
        if (bitRead(inputs, 9) || bitRead(inputs, 10)) { 
          if (bitRead(statut_game, SELECT_P1)) { bitClear(statut_game, SELECT_P1); bitSet(statut_game, SELECT_P2); }
          else { bitSet(statut_game, SELECT_P1); bitClear(statut_game, SELECT_P2); }
        }
        if (bitRead(inputs, 8)) bitSet(statut_game, PLAYER_CONFIRMED);
      } else {
        if (bitRead(inputs, 9)) { if(bitRead(statut_game, SELECT_P1)) score_p1--; else score_p2--; }
        if (bitRead(inputs, 10)) { if(bitRead(statut_game, SELECT_P1)) score_p1++; else score_p2++; }
        if (bitRead(inputs, 8)) { bitClear(statut_game, SCORE_ADJUST); bitClear(statut_game, PLAYER_CONFIRMED); }
      }
      score_screen_starwars();
      return;
    }

    if (bitRead(inputs, 8)) { bitSet(statut_game, SCORE_ADJUST); bitSet(statut_game, SELECT_P1); }

    // Buts et Gamelles (Bits 11 a 14 sont les FRONT MONTANTS)
    if (bitRead(inputs, 11)) { score_p1++; ball--; playSFX(2); requestAnimation(ANIM_BUT_J1); matchAmbienceTriggered = false; startMatchTimer = millis(); } 
    if (bitRead(inputs, 13)) { score_p2++; ball--; playSFX(3); requestAnimation(ANIM_BUT_J2); matchAmbienceTriggered = false; startMatchTimer = millis(); } 
    if (bitRead(inputs, 12)) { score_p2--; playSFX(4); requestAnimation(ANIM_GAM_J1); matchAmbienceTriggered = false; startMatchTimer = millis(); } 
    if (bitRead(inputs, 14)) { score_p1--; playSFX(4); requestAnimation(ANIM_GAM_J2); matchAmbienceTriggered = false; startMatchTimer = millis(); } 

    // Reset Long OK
    if (ok_buttom_start > 20) { 
      bitClear(statut_game, RUN); bitSet(statut_game, START_GAME); 
      matrix->fillScreen(C_BLACK); requestAnimation(ANIM_NONE); playSFX(1, true); 
    }
    
    score_screen_starwars();

    if (ball <= 0) { 
      bitClear(statut_game, RUN); bitSet(statut_game, START_GAME); 
      matrix->fillScreen(C_BLACK); 
      if (score_p1 > score_p2) { playSFX(5); requestAnimation(ANIM_VIC_J1); } 
      else { playSFX(6); requestAnimation(ANIM_VIC_J2); }
    }
  }
}
