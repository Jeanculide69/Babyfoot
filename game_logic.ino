#include "config.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

extern MatrixPanel_I2S_DMA *matrix;

// Variables globales
volatile int score_p1 = 0;
volatile int score_p2 = 0;
volatile int ball = 11;
volatile unsigned long statut_game = 0;
volatile unsigned int inputs = 0;

int sens_set = 500; 

void setupGame() {
  score_p1 = 0; score_p2 = 0; ball = 11;
  statut_game = 0;
  bitSet(statut_game, START_GAME);
  if (matrix) matrix->fillScreen(C_BLACK); 
  Serial.println("[GAME] Logique V9.0 (DMA Ready)");
}

void read_inputs_old() {
  int val_ok = touchRead(BTN_OK);
  bool ok_raw = (val_ok < sens_set && val_ok > 1);
  bool g_r = digitalRead(GOAL_RIGHT);
  bool g_l = digitalRead(GOAL_LEFT);

  auto update_edge = [](int bit, bool current) {
    if (current) {
      if (!bitRead(inputs, bit)) { bitSet(inputs, bit); bitSet(inputs, bit + 8); }
      else { bitClear(inputs, bit + 8); }
    } else {
      bitClear(inputs, bit); bitClear(inputs, bit + 8);
    }
  };

  update_edge(0, ok_raw);
  update_edge(3, g_l);
  update_edge(5, g_r);
}

void score_screen_old() {
  if (!matrix) return;
  static int last_s1 = -1, last_s2 = -1, last_b = -1;
  
  if (score_p1 != last_s1) {
    matrix->setTextSize(2);
    matrix->setTextColor(C_WHITE, C_BLACK);
    if (score_p1 > 9) matrix->setCursor(0, 9); else matrix->setCursor(8, 9);
    matrix->print(score_p1);
    last_s1 = score_p1;
  }

  if (score_p2 != last_s2) {
    matrix->setTextSize(2);
    matrix->setTextColor(C_WHITE, C_BLACK);
    if (score_p2 > 9) matrix->setCursor(40, 9); else matrix->setCursor(46, 9);
    matrix->print(score_p2);
    last_s2 = score_p2;
  }

  if (ball != last_b) {
    matrix->drawRect(24, 11, 15, 11, C_YELLOW);
    matrix->setTextSize(1);
    matrix->setTextColor(C_YELLOW, C_BLACK);
    if (ball > 9) matrix->setCursor(26, 13); else matrix->setCursor(29, 13);
    matrix->print(ball);
    last_b = ball;
  }
}

void handleGameLogic() {
  read_inputs_old();
  extern void drawAnimStandby();

  if (bitRead(statut_game, START_GAME)) {
    drawAnimStandby();
    if (bitRead(inputs, 8)) { // EDGE_OK
      Serial.println("[GAME] Match Start (DMA)");
      score_p1 = 0; score_p2 = 0; ball = 11;
      bitClear(statut_game, START_GAME);
      bitSet(statut_game, RUN);
      matrix->fillScreen(C_BLACK);
      
      matrix->setTextSize(1);
      matrix->setCursor(4, 1); matrix->setTextColor(C_MAGENTA); matrix->print("HOME");
      matrix->setCursor(34, 1); matrix->setTextColor(C_CYAN); matrix->print("GUEST");
    }
  }

  if (bitRead(statut_game, RUN)) {
    if (bitRead(inputs, 11)) { score_p1++; ball--; }
    if (bitRead(inputs, 13)) { score_p2++; ball--; }
    
    score_screen_old();

    if (ball <= 0) {
        bitClear(statut_game, RUN);
        bitSet(statut_game, START_GAME);
        matrix->fillScreen(C_BLACK);
    }
  }
}

void handleAction(String act) { }
