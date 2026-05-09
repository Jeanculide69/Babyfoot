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
  if (n < 0) {
    matrix->fillRect(x, y + 5, 4, 2, color); // Trait moins
    drawBoldDigit(abs(n) % 10, x + 5, y, color);
    return;
  }
  
  if (n > 9) {
    int d1 = n / 10;
    int d2 = n % 10;
    // On dessine les deux chiffres tres serres (10px d'ecart)
    drawBoldDigit(d1, x, y, color);
    drawBoldDigit(d2, x + 10, y, color); 
  } else {
    drawBoldDigit(n, x, y, color);
  }
}

// Calcule la largeur réelle en pixels d'un nombre dessiné avec drawBoldDigit
int getScoreWidth(int n) {
  if (n < 0) return 20;      // Signe "-" + Chiffre
  else if (n > 9) return 24;  // Deux chiffres (marge augmentée pour centrage)
  return 12;                 // Un chiffre seul
}

void drawScoreCentered(int n, int zoneX, int y, uint16_t color) {
  int scoreWidth = getScoreWidth(n);
  int startX;

  if (zoneX == 0) {
    // Espace visuel libre à gauche : X=0 à X=24 (largeur 25px)
    startX = (25 - scoreWidth) / 2;
  } else {
    // Espace visuel libre à droite : X=39 à X=63 (largeur 25px)
    startX = 39 + (25 - scoreWidth) / 2;
  }

  drawBoldNumber(n, startX, y, color);
}

// --- DESSIN DES LETTRES BOLD (A-Z Simplifié) ---
void drawCenteredText(const String& text, int y, uint16_t color, int x_offset, int width) {
  if (text.length() == 0) return;

  int charWidth = 6; 
  int textWidth = text.length() * charWidth;

  // On efface uniquement la zone de texte pour eviter les trainees 
  matrix->fillRect(x_offset, y, width, 8, C_BLACK);
  matrix->setTextWrap(false);

  if (textWidth <= width) {
    int startX = x_offset + (width - textWidth) / 2;
    matrix->setCursor(startX, y);
    matrix->setTextColor(color);
    matrix->print(text);
    matrix->setCursor(startX + 1, y); matrix->print(text); // Bold
  } else {
    int scrollRange = textWidth - width + 10;
    int offset = (millis() / 200) % scrollRange; 

    matrix->setTextColor(color);
    for (int i = 0; i < text.length(); i++) {
      int charX = x_offset + (i * charWidth) - offset;
      
      // CLIPPING STRICT : On ne dessine le caractere que s'il est dans sa zone
      if (charX >= x_offset && (charX + charWidth - 1) <= (x_offset + width)) {
        matrix->setCursor(charX, y);
        matrix->print(text[i]);
        matrix->setCursor(charX + 1, y);
        matrix->print(text[i]);
      }
    }
  }
}
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;

// Variables globales déportées dans le fichier principal
extern String team1_name, team2_name;
extern bool tournament_mode;
extern bool is_ap_mode;
extern volatile int score_p1, score_p2, ball;
extern volatile uint32_t statut_game;
extern volatile unsigned int inputs;
int sens_set = SENS_SET; 

volatile int less_buttom_start = 0;
volatile int more_buttom_start = 0;
volatile int ok_buttom_start = 0;
volatile int waiting_goal = 0;
volatile int last_points = 0;

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
    sendDFCommand(0x19, 0x00, 0x00); // Activer la boucle
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
  int baseY = 31; // On force l'utilisation de la toute derniere ligne

  int headY, bodyTop, bodyHeight;
  bool drawLegs = true;
  int leg_offset = (f.frame / 3) % 2; // Vitesse d'animation des jambes adoucie

  if (f.state == 3) { // Etat 3 : Accroupi
    headY = baseY - 3;      
    bodyTop = baseY - 2;    
    bodyHeight = 2;         
    drawLegs = false;       
  } 
  else if (f.state == 2) { // Etat 2 : Saut (Tuck jump)
    headY = baseY - 5;      
    bodyTop = baseY - 4;    
    bodyHeight = 3;         
    drawLegs = false;       
  } 
  else { // Etat 0 et 1 : Normal ou Attaque
    headY = baseY - 5;      
    bodyTop = baseY - 4;    
    bodyHeight = 3;         
    drawLegs = true;        
  }

  // 1. Dessin de la Tête (Blanc)
  matrix->drawPixel(x, headY, C_WHITE);

  // 2. Dessin du Tronc (Couleur de l'équipe)
  matrix->drawFastVLine(x, bodyTop, bodyHeight, f.color);

  // 3. Dessin des Jambes
  if (drawLegs) {
    if (leg_offset == 1) {
      // Animation : Jambes ecartees (course)
      matrix->drawFastVLine(x - 1, baseY - 1, 2, f.color); 
      matrix->drawFastVLine(x + 1, baseY - 1, 2, f.color); 
    } else {
      // Animation : Jambes droites
      matrix->drawFastVLine(x, baseY - 1, 2, f.color);
    }
  } else if (f.state == 2) {
    // Animation en l'air : Petits pieds recroquevilles
    matrix->drawPixel(x - 1, baseY - 2, f.color);
    matrix->drawPixel(x + 1, baseY - 2, f.color);
  } else if (f.state == 3) {
    // Animation accroupi : Pieds au sol ecartez
    matrix->drawPixel(x - 1, baseY, f.color);
    matrix->drawPixel(x + 1, baseY, f.color);
  }

  // 4. Bras, Main et Sabre Laser
  int shoulderX = x;
  int shoulderY = bodyTop + 1;  // L'epaule est au milieu du corps

  int hx = shoulderX + f.dir;   // Position de la main (X)
  int hy = shoulderY;           // Position de la main (Y)
  int sx2, sy2;                 // Pointe du sabre

  uint16_t C_GREY = 0x8410;     // Code couleur RGB565 pour un Gris Moyen

  if (f.state == 1) { 
    // ETAT 1 : ATTAQUE (La main se deplace par rapport a l'epaule !)
    int attackPhase = f.frame % 12;
    if (attackPhase < 4) {
      // Phase 1 : Armement
      hx = shoulderX - f.dir; 
      hy = shoulderY;
      sx2 = hx - (f.dir * 3); sy2 = hy - 3;
    } else if (attackPhase < 8) {
      // Phase 2 : Frappe
      hx = shoulderX + (f.dir * 2); 
      hy = shoulderY;
      sx2 = hx + (f.dir * 4); sy2 = hy;
    } else {
      // Phase 3 : Fin de course
      hx = shoulderX + f.dir; 
      hy = shoulderY + 1;
      sx2 = hx + (f.dir * 3); sy2 = hy + 3;
    }
  } 
  else if (f.state == 2) { 
    // ETAT 2 : SAUT
    hx = shoulderX; 
    hy = shoulderY - 2;
    sx2 = hx + (f.dir * 3); sy2 = hy - 3;
  } 
  else if (f.state == 3) { 
    // ETAT 3 : ACCROUPI
    hx = shoulderX + (f.dir * 2); 
    hy = shoulderY;
    sx2 = hx + (f.dir * 4); sy2 = hy;
  } 
  else { 
    // ETAT 0 : REPOS
    hx = shoulderX + f.dir; 
    hy = shoulderY + (sin(f.frame * 0.2) > 0 ? 1 : 0); 
    sx2 = hx + (f.dir * 3); sy2 = hy - 3; 
  }

  // Securite anti-bug (zone d'effacement Y=26 a Y=31)
  if (sy2 < 26) sy2 = 26; 
  if (sy2 > 31) sy2 = 31;

  // A. Dessin du bras (relie l'epaule a la main avec la couleur de l'equipe)
  matrix->drawLine(shoulderX, shoulderY, hx, hy, f.color);

  // --- DESSIN FINAL DU SABRE CORRIGÉ ---
  
  // A. Bras
  matrix->drawLine(shoulderX, shoulderY, hx, hy, f.color);

  // B. Laser (Dessiné ENTIER d'un seul coup pour éviter qu'il se plie)
  matrix->drawLine(hx, hy, sx2, sy2, f.color);

  // C. Calcul du manche (Gris)
  int dX = (sx2 > hx) ? 1 : ((sx2 < hx) ? -1 : 0);
  int dY = (sy2 > hy) ? 1 : ((sy2 < hy) ? -1 : 0);

  // Ajustement pour les lignes très aplaties
  if (abs(sx2 - hx) > abs(sy2 - hy) * 2) {
      dY = 0; 
  } else if (abs(sy2 - hy) > abs(sx2 - hx) * 2) {
      dX = 0; 
  }

  // D. Manche (Gris) posé exactement sur la ligne du sabre
  matrix->drawPixel(hx + dX, hy + dY, C_GREY); 

  // E. Main (Couleur équipe) redessinée par-dessus
  matrix->drawPixel(hx, hy, f.color);
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
  
  static int last_s1 = -1, last_s2 = -1, last_b = -1, last_waiting = -1;
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
    return;
  }

  // 3. Dessin du HUD (Scores/Noms)
  static bool last_blink = false;
  bool is_adjust = bitRead(statut_game, SCORE_ADJUST);
  bool blink = is_adjust && ((millis() / 250) % 2);
  
  // Force le rafraîchissement au changement d'état du clignotement pour animer sans scintiller
  if (is_adjust && (blink != last_blink)) {
      last_blink = blink;
      needsFullRedraw = true;
  }

  if (needsFullRedraw || score_p1 != last_s1 || score_p2 != last_s2 || ball != last_b || waiting_goal != last_waiting) {
    matrix->fillRect(0, 0, 64, 25, C_BLACK);
    
    // Dessin du cadre "Cockpit" Star Wars
    matrix->drawFastHLine(0, 0, 64, 0x18E3);  
    matrix->drawFastHLine(0, 9, 64, 0x18E3);  
    matrix->drawFastVLine(31, 0, 10, 0x18E3); 

    bool is_confirmed = bitRead(statut_game, PLAYER_CONFIRMED);
    bool sel_p1 = bitRead(statut_game, SELECT_P1);
    bool sel_p2 = bitRead(statut_game, SELECT_P2);

    uint16_t color_p1 = C_BLUE;
    uint16_t color_p2 = C_RED;

    if (is_adjust) {
        if (!is_confirmed) {
            // Etape 1 : Sélection du joueur (Le score clignote)
            if (sel_p1 && blink) color_p1 = C_BLACK;
            if (sel_p2 && blink) color_p2 = C_BLACK;
        } else {
            // Etape 2 : Modification du score (Clignote en JAUNE pour confirmer l'édition)
            if (sel_p1) color_p1 = blink ? C_YELLOW : C_BLUE;
            if (sel_p2) color_p2 = blink ? C_YELLOW : C_RED;
        }
    }

    if (color_p1 != C_BLACK) drawScoreCentered(score_p1, 0, 12, color_p1);
    if (color_p2 != C_BLACK) drawScoreCentered(score_p2, 32, 12, color_p2);
    
    // Compteur de balles central miniaturise
    matrix->drawRect(25, 11, 14, 9, 0x4208); 
    if (waiting_goal > 0) matrix->drawRect(24, 10, 16, 11, C_YELLOW); 
    matrix->setCursor((ball > 9 ? 26 : 29), 12);
    matrix->setTextColor(C_YELLOW);
    matrix->print(ball);

    needsFullRedraw = false;
    last_s1 = score_p1; last_s2 = score_p2; last_b = ball; last_waiting = waiting_goal;
  }

  // Affichage dynamique des noms (en dehors du bloc IF pour permettre le scroll permanent)
  drawCenteredText(team1_name, 1, C_BLUE, 0, 31);   
  drawCenteredText(team2_name, 1, C_RED, 32, 32);

  // --- NETTOYAGE LARGE (y=26 a 31) ---
  static unsigned long lastCombat = 0;
  if (millis() - lastCombat > 50) {
    lastCombat = millis();
    matrix->fillRect(0, 25, 64, 7, C_BLACK); 
    updateFighter(jedi1, jedi2);
    updateFighter(jedi2, jedi1);
    drawStickman(jedi1);
    drawStickman(jedi2);
    updateBlasters();
    drawBlasters();
    drawParticles();
  }
}

void drawAPMode() {
  static uint32_t lastPulse = 0;
  static bool pulseState = false;
  if (millis() - lastPulse > 500) { lastPulse = millis(); pulseState = !pulseState; }

  matrix->fillScreen(C_BLACK);
  drawCenteredText("WIFI SETUP", 1, C_YELLOW, 0, 64);
  drawCenteredText("Babyfoot-Force", 11, C_BLUE, 0, 64);
  
  if (pulseState) {
    drawCenteredText("192.168.4.1", 22, C_WHITE, 0, 64);
  } else {
    drawCenteredText("CONNECTER ICI", 22, C_GOLD, 0, 64);
  }
}

void drawAnimStandby() {
  if (is_ap_mode) {
    drawAPMode();
    return;
  }

  if (tournament_mode) {
    matrix->fillScreen(C_BLACK);
    
    // 1. Team 1 - Haut Gauche (Zone 32px)
    drawCenteredText(team1_name, 0, C_BLUE, 0, 32);
    
    // 2. VS - Milieu (Remonte de 10 a 8)
    drawCenteredText("VS", 8, C_WHITE, 0, 64);
    
    // 3. Team 2 - Bas Droite (Zone 32px, Remonte de 18 a 16)
    drawCenteredText(team2_name, 16, C_RED, 32, 32);
    
    // 4. OK - Sous Team 2 (Remonte de 26 a 24)
    if ((millis() / 500) % 2) {
      drawCenteredText("OK", 24, C_YELLOW, 32, 32);
    }
  } else {
    // Animation GIF Habituelle
    extern void drawStarWarsGIF();
    drawStarWarsGIF();
  }
}

void setupGame() {
  score_p1 = 0; score_p2 = 0; ball = 11; statut_game = 0;
  bitSet(statut_game, START_GAME);
  raz_but(); // Reset des verrous au démarrage
  if (matrix) matrix->fillScreen(C_BLACK);
}

bool check_touch(int pin, volatile int &counter, bool current_state) {
  int val = touchRead(pin);
  if (val < SENS_SET && val > 1) { 
    if (counter < 100) counter++; 
  }
  else if (counter > 0) { counter--; }
  
  if (counter > DELAY_BUTTOM) return true; 
  else if (counter <= 0) return false;
  else return current_state; // Hysteresis!
}

// --- FLAGS DE SIMULATION GLOBAUX (Pour WiFi et Serial) ---
static volatile bool sim_ok = false, sim_b1 = false, sim_g1 = false, sim_b2 = false, sim_g2 = false;

void handleAction(String act) {
  if (act == "B1") sim_b1 = true;
  if (act == "B2") sim_b2 = true;
  if (act == "G1") sim_g1 = true;
  if (act == "G2") sim_g2 = true;
  if (act == "OK") sim_ok = true;
  if (act == "REBOOT") { ESP.restart(); }
  if (act == "M1") { score_p1--; last_points = -1; bitSet(statut_game, LAST_GOAL_P1); bitClear(statut_game, LAST_GOAL_P2); }
  if (act == "P1") { score_p1++; last_points = 1; bitSet(statut_game, LAST_GOAL_P1); bitClear(statut_game, LAST_GOAL_P2); }
  if (act == "M2") { score_p2--; last_points = -1; bitClear(statut_game, LAST_GOAL_P1); bitSet(statut_game, LAST_GOAL_P2); }
  if (act == "P2") { score_p2++; last_points = 1; bitClear(statut_game, LAST_GOAL_P1); bitSet(statut_game, LAST_GOAL_P2); }
  if (act == "DEMI") {
    if (bitRead(statut_game, LAST_GOAL_P1)) {
        score_p1 -= last_points; waiting_goal = last_points; 
        bitSet(statut_game, DEMI); bitClear(statut_game, LAST_GOAL_P1);
        addLog("PORTAIL: DEMI J1"); addTvEvent("demi_j1");
        requestAnimation(ANIM_DEMI); playSFX(9, false);
    } else if (bitRead(statut_game, LAST_GOAL_P2)) {
        score_p2 -= last_points; waiting_goal = last_points; 
        bitSet(statut_game, DEMI); bitClear(statut_game, LAST_GOAL_P2);
        addLog("PORTAIL: DEMI J2"); addTvEvent("demi_j2");
        requestAnimation(ANIM_DEMI); playSFX(9, false);
    }
  }
  if (act == "BIERE") { playSFX(SFX_GAMELLE, true); requestAnimation(5); } // ANIM_BIERE = 5, SFX = 4
  Serial.print("[WIFI-SIM] Command Received: "); Serial.println(act);
}

void pollGoalSensors() {
  static bool last_phys_gr = false, last_phys_gl = false, last_phys_gamr = false, last_phys_gaml = false;
  
  bool phys_gr = digitalRead(GOAL_RIGHT);
  bool phys_gl = digitalRead(GOAL_LEFT);
  bool phys_gamr = digitalRead(GAMELLE_RIGHT);
  bool phys_gaml = digitalRead(GAMELLE_LEFT);

  // Fronts montants physiques (0 -> 1)
  bool edge_gr = phys_gr && !last_phys_gr; last_phys_gr = phys_gr;
  bool edge_gl = phys_gl && !last_phys_gl; last_phys_gl = phys_gl;
  bool edge_gamr = phys_gamr && !last_phys_gamr; last_phys_gamr = phys_gamr;
  bool edge_gaml = phys_gaml && !last_phys_gaml; last_phys_gaml = phys_gaml;

  // Déclencheur final : Front physique OU Simulation Web
  bool g_r = edge_gr || sim_b2;
  bool g_l = edge_gl || sim_b1;
  bool gam_r = edge_gamr || sim_g2;
  bool gam_l = edge_gaml || sim_g1;

  // Reset des flags de simulation pour les buts
  sim_b1 = sim_g1 = sim_b2 = sim_g2 = false;

  auto update_sensor = [](int bit, bool triggered) {
    if (triggered) { 
      if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); } 
    } else { 
      bitClear(inputs, bit); 
    }
  };

  update_sensor(3, g_l);
  update_sensor(4, gam_l);
  update_sensor(5, g_r);
  update_sensor(6, gam_r);
}

void pollButtons() {
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

  // Lecture des boutons capacitifs
  bool ok_raw = check_touch(BTN_OK, ok_buttom_start, bitRead(inputs, 0)) || sim_ok;
  bool less_raw = check_touch(BTN_LESS, less_buttom_start, bitRead(inputs, 1));
  bool more_raw = check_touch(BTN_MORE, more_buttom_start, bitRead(inputs, 2));

  sim_ok = false;

  auto update_edge = [](int bit, bool current) {
    if (current) { 
      if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); } 
    }
    else { bitClear(inputs, bit); }
  };

  update_edge(0, ok_raw); 
  update_edge(1, less_raw); 
  update_edge(2, more_raw);
}


void raz_but(){
    int max_attempts = 5;
    while((digitalRead(GOAL_RIGHT) || digitalRead(GOAL_LEFT) || digitalRead(GAMELLE_RIGHT) || digitalRead(GAMELLE_LEFT)) && max_attempts > 0){
        digitalWrite(RESET_PIN, 0);
        unsigned long over_time = 0;
        while((digitalRead(GOAL_RIGHT) || digitalRead(GOAL_LEFT) || digitalRead(GAMELLE_RIGHT) || digitalRead(GAMELLE_LEFT))) {
           over_time++;
           if (over_time > 100000) break; // Timeout rapide pour ne pas ralentir le jeu
        }
        delay(5);
        digitalWrite(RESET_PIN, 1);
        delay(5);
        max_attempts--;
    }
    
    // Si la balle reste bloquée devant le capteur physiquement
    if (max_attempts == 0) {
        Serial.println("[ATTENTION] Un capteur est toujours declenche (balle bloquee ?) -> on continue.");
    }
}

void handleGameLogic() {
  // 1. Lecture haute fréquence des capteurs (Appelé à chaque loop ~1ms)
  pollGoalSensors();

  // 2. Throttle à 30ms pour la logique de jeu et les boutons
  static unsigned long lastLoop = 0;
  if (millis() - lastLoop < 30) return; 
  lastLoop = millis();

  // 3. Lecture "calme" des boutons (Seulement toutes les 30ms)
  pollButtons();


  if (bitRead(statut_game, MATCH_FINISHED)) {
    static unsigned long stateTime = 0;
    static int phase = 0; // 0: Victoire, 1: Biere
    
    if (stateTime == 0) { 
        stateTime = millis(); phase = 0; 
        if (score_p1 > score_p2) requestAnimation(ANIM_VIC_J1);
        else requestAnimation(ANIM_VIC_J2);
    }
    
    if (isAnimationActive()) updateAnimations();
    else {
        if (phase == 0 && (millis() - stateTime > 7000)) { // 7s de victoire
            phase = 1; stateTime = millis();
            requestAnimation(ANIM_BIERE);
            addLog("Pause Biere (15s)...");
        }
    }
    score_screen_starwars();

    // Transition automatique après la pause biere (ou si OK pressé)
    bool skip = bitRead(inputs, 8);
    if (skip || (phase == 1 && (millis() - stateTime > 15000))) {
        extern bool autoLoadNextMatch();
        bool nextFound = autoLoadNextMatch();

        portENTER_CRITICAL(&stateMutex);
        bitClear(statut_game, MATCH_FINISHED);
        
        if (nextFound && tournament_mode) {
            bitSet(statut_game, RUN); // Lancement AUTO
            bitClear(statut_game, START_GAME);
        } else {
            bitSet(statut_game, START_GAME); // Retour STANDBY
        }
        
        ball = 11; score_p1 = 0; score_p2 = 0;
        waiting_goal = 0;
        portEXIT_CRITICAL(&stateMutex);

        extern void playSFX(int id, bool loop);
        playSFX(SFX_INTRO, true);
        requestAnimation(ANIM_NONE);
        stateTime = 0; phase = 0; // Reset pour le prochain match
    }
    inputs &= 0x00FF; // Clear les fronts (edges) après traitement
    return;
  }

  if (bitRead(statut_game, START_GAME)) {
    if (isAnimationActive()) updateAnimations();
    else drawAnimStandby();

    if (bitRead(inputs, 8)) { // Touche OK
      portENTER_CRITICAL(&stateMutex);
      score_p1 = 0; score_p2 = 0; ball = 11;
      waiting_goal = 0;
      bitClear(statut_game, START_GAME); bitSet(statut_game, RUN);
      portEXIT_CRITICAL(&stateMutex);
      
      raz_but(); // Nettoyage total au coup d'envoi
      bitClear(inputs, 8); 
      requestAnimation(ANIM_NONE); 
      playSFX(SFX_MATCH_PT, false); // On lance le jingle de debut (7 secondes)
      score_screen_starwars(true);
      Serial.println("[GAME] --- MATCH STARTED ! ---");
      addTvEvent("lance");
      static unsigned long startMatchMs = millis();
    }
  }

  // --- GESTION NON-BLOQUANTE DU SON DE MATCH (008 -> 007) ---
  static unsigned long startMatchTimer = 0;
  static bool matchAmbienceTriggered = false;
  if (bitRead(statut_game, RUN) && !bitRead(statut_game, START_GAME)) {
      if (startMatchTimer == 0) startMatchTimer = millis();
      if (!matchAmbienceTriggered && (millis() - startMatchTimer > 7000)) { // Attente de 7 secondes pour le son 008
          playSFX(SFX_AMBIANCE, true);
          matchAmbienceTriggered = true;
      }
  } else {
      startMatchTimer = 0;
      matchAmbienceTriggered = false;
  }

  if (bitRead(statut_game, RUN)) {
    // Mode Correction
    static unsigned long adjustStartTime = 0;
    if (bitRead(inputs, 8) && !bitRead(statut_game, SCORE_ADJUST)) { 
      bitSet(statut_game, SCORE_ADJUST); bitSet(statut_game, SELECT_P1); 
      adjustStartTime = millis(); 
    }

    if (bitRead(statut_game, SCORE_ADJUST)) {
      if (!bitRead(statut_game, PLAYER_CONFIRMED)) {
        if (bitRead(inputs, 9) || bitRead(inputs, 10)) { 
          if (bitRead(statut_game, SELECT_P1)) { bitClear(statut_game, SELECT_P1); bitSet(statut_game, SELECT_P2); }
          else { bitSet(statut_game, SELECT_P1); bitClear(statut_game, SELECT_P2); }
        }
        if (bitRead(inputs, 8) && (millis() - adjustStartTime > 500)) bitSet(statut_game, PLAYER_CONFIRMED);
      } else {
        if (bitRead(inputs, 9)) { if(bitRead(statut_game, SELECT_P1)) score_p1--; else score_p2--; }
        if (bitRead(inputs, 10)) { if(bitRead(statut_game, SELECT_P1)) score_p1++; else score_p2++; }
        if (bitRead(inputs, 8)) { bitClear(statut_game, SCORE_ADJUST); bitClear(statut_game, PLAYER_CONFIRMED); }
      }
      score_screen_starwars();
      return;
    }

    // Buts et Gamelles (Bits 11 a 14)
    static unsigned long lastGoalMs = 0;
    if (millis() - lastGoalMs > 3000) { // Lockout de 3s pour éviter les doubles comptes
        if (bitRead(inputs, 11)) { // But Cote Gauche (Point pour P1)
            portENTER_CRITICAL(&stateMutex);
            last_points = 1 + waiting_goal;
            score_p1 += last_points; // GAUCHE -> P1
            waiting_goal = 0; bitClear(statut_game, DEMI);
            bitSet(statut_game, LAST_GOAL_P1); bitClear(statut_game, LAST_GOAL_P2);
            portEXIT_CRITICAL(&stateMutex);

            playSFX(SFX_BUT_J1); requestAnimation(ANIM_BUT_J1); 
            if (ball > 0) ball--; 
            addLog("B1"); addTvEvent("B1");
            raz_but();
            lastGoalMs = millis();
            matchAmbienceTriggered = true; 
        } 
        if (bitRead(inputs, 13)) { // But Cote Droit (Point pour P2)
            portENTER_CRITICAL(&stateMutex);
            last_points = 1 + waiting_goal;
            score_p2 += last_points; // DROITE -> P2
            waiting_goal = 0; bitClear(statut_game, DEMI);
            bitSet(statut_game, LAST_GOAL_P2); bitClear(statut_game, LAST_GOAL_P1);
            portEXIT_CRITICAL(&stateMutex);

            playSFX(SFX_BUT_J2); requestAnimation(ANIM_BUT_J2); 
            if (ball > 0) ball--; 
            addLog("B2"); addTvEvent("B2");
            raz_but();
            lastGoalMs = millis();
            matchAmbienceTriggered = true; 
        } 
        
        // Gamelles (Regle de la Penalite : Retire un point a l'ADVERSAIRE)
        if (bitRead(inputs, 12)) { // Gamelle Cote Gauche (Faite par P1)
            portENTER_CRITICAL(&stateMutex);
            score_p2--; // P2 perd un point
            portEXIT_CRITICAL(&stateMutex);
            playSFX(SFX_GAMELLE); requestAnimation(ANIM_GAM_J1); 
            addLog("G1"); addTvEvent("G1");
            raz_but();
            lastGoalMs = millis();
            matchAmbienceTriggered = true; 
        } 
        if (bitRead(inputs, 14)) { // Gamelle Cote Droit (Faite par P2)
            portENTER_CRITICAL(&stateMutex);
            score_p1--; // P1 perd un point
            portEXIT_CRITICAL(&stateMutex);
            playSFX(SFX_GAMELLE); requestAnimation(ANIM_GAM_J2); 
            addLog("G2"); addTvEvent("G2");
            raz_but();
            lastGoalMs = millis();
            matchAmbienceTriggered = true; 
        } 
    }

    // --- LOGIQUE DU DEMI (+ et - ensemble) ---
    // On utilise les bits d'état (1 et 2) et non les fronts (9 et 10) pour plus de souplesse
    if (bitRead(inputs, 1) && bitRead(inputs, 2)) { 
        if (bitRead(statut_game, LAST_GOAL_P1)) {
            score_p1 -= last_points; 
            waiting_goal = last_points; 
            bitSet(statut_game, DEMI); bitClear(statut_game, LAST_GOAL_P1);
            addLog("DEMI J1: Point mis en attente."); addTvEvent("demi_j1");
            requestAnimation(ANIM_DEMI); playSFX(9, false);
        } else if (bitRead(statut_game, LAST_GOAL_P2)) {
            score_p2 -= last_points; 
            waiting_goal = last_points; 
            bitSet(statut_game, DEMI); bitClear(statut_game, LAST_GOAL_P2);
            addLog("DEMI J2: Point mis en attente."); addTvEvent("demi_j2");
            requestAnimation(ANIM_DEMI); playSFX(9, false);
        }
    }

    // Reset Long OK (5 secondes)
    if (ok_buttom_start > 80) { 
      extern void resetTournament();
      resetTournament();
      ok_buttom_start = 0; // Reset counter
      matrix->fillScreen(C_BLACK); 
      requestAnimation(ANIM_NONE); 
      extern void playSFX(int id, bool loop);
      playSFX(SFX_INTRO, true); 
    }
    
    score_screen_starwars();

    // BALLE DE MATCH (Golden Goal) : On lance une alerte une seule fois
    static bool goldenGoalAlert = false;
    if (ball <= 0 && score_p1 == score_p2 && !goldenGoalAlert) {
        requestAnimation(ANIM_BALLE_MATCH); 
        goldenGoalAlert = true;
    }
    if (!bitRead(statut_game, RUN)) goldenGoalAlert = false; // Reset pour le prochain match

    // FIN DE MATCH : On passe en mode verrouille "MATCH_FINISHED"
    if (ball <= 0 && score_p1 != score_p2) { 
      bitClear(statut_game, RUN); 
      bitSet(statut_game, MATCH_FINISHED);
      addTvEvent(score_p1 > score_p2 ? "victoire_p1" : "victoire_p2");
      
      // MISE A JOUR AUTONOME DU TOURNOI (Backend-Driven)
      extern void updateTournamentProgress(int s1, int s2);
      updateTournamentProgress(score_p1, score_p2);

      if (score_p1 > score_p2) { playSFX(SFX_VIC_J1); requestAnimation(ANIM_VIC_J1); } 
      else { playSFX(SFX_VIC_J2); requestAnimation(ANIM_VIC_J2); }
      Serial.println("[GAME] Match over. Autonomous update done.");
    }
  }
  
  inputs &= 0x00FF; // Clear les fronts (edges) en fin de frame 30ms
}
