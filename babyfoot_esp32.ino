#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "config.h"

MatrixPanel_I2S_DMA *matrix = nullptr;

void addLog(String msg) { Serial.println(msg); }

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n[SYS] --- BABYFOOT CORE BOOT (V9.1 DMA) ---");

  HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, 
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, 
    LAT_PIN, OE_PIN, CLK_PIN
  };

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);

  matrix = new MatrixPanel_I2S_DMA(mxconfig);
  matrix->begin();
  matrix->setRotation(2); // <--- ROTATION 180° pour remettre l'ecran a l'endroit
  matrix->setBrightness8(60); 
  matrix->clearScreen();

  pinMode(GOAL_LEFT, INPUT);
  pinMode(GAMELLE_LEFT, INPUT);
  pinMode(GOAL_RIGHT, INPUT);
  pinMode(GAMELLE_RIGHT, INPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);

  extern void setupGame();
  setupGame();
}

void loop() {
  extern void handleGameLogic();
  handleGameLogic();
  delay(1);
}
