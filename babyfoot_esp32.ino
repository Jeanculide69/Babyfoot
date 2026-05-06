#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "config.h"

// Configuration de la matrice DMA
MatrixPanel_I2S_DMA *matrix = nullptr;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n[SYS] --- BABYFOOT CORE BOOT (V9.0 DMA) ---");
  Serial.println("[SYS] Migration vers driver I2S DMA (Anti-Flicker)");

  // Configuration des Pins DMA
  HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, 
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, 
    LAT_PIN, OE_PIN, CLK_PIN
  };

  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // largeur
    PANEL_RES_Y,   // hauteur
    PANEL_CHAIN,   // chainage
    _pins          // mapping pins
  );

  // Initialisation Matrice DMA
  matrix = new MatrixPanel_I2S_DMA(mxconfig);
  matrix->begin();
  matrix->setBrightness8(60); // Luminosité 0-255
  matrix->clearScreen();

  // Initialisation des Pins Capteurs
  pinMode(GOAL_LEFT, INPUT);
  pinMode(GAMELLE_LEFT, INPUT);
  pinMode(GOAL_RIGHT, INPUT);
  pinMode(GAMELLE_RIGHT, INPUT);
  
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);

  Serial.println("[SYS] Materiel pret (DMA Actif).");
  
  extern void setupGame();
  setupGame();
}

void loop() {
  extern void handleGameLogic();
  handleGameLogic();
  delay(1);
}
