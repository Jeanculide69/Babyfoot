#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "config.h"

MatrixPanel_I2S_DMA *matrix = nullptr;

void addLog(String msg) { Serial.println(msg); }

void sendDFCommand(uint8_t cmd, uint8_t p1, uint8_t p2) {
  uint16_t checksum = -(0xFF + 0x06 + cmd + 0x00 + p1 + p2);
  uint8_t buf[10] = {
    0x7E, 0xFF, 0x06, cmd, 0x00, p1, p2, 
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF), 0xEF
  };
  Serial1.write(buf, 10);
  Serial.printf("[AUDIO] Send CMD: 0x%02X | P1: 0x%02X | P2: 0x%02X\n", cmd, p1, p2);
}

// --- GESTION VOLUME SMOOTH ---
volatile int cur_vol = 0;
volatile int target_vol = 25;
unsigned long last_vol_ms = 0;

void updateAudioFade() {
  // Désactivé pour coller au code original (volume fixe)
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(500);
  Serial.println("\n\n[SYS] --- BABYFOOT CORE BOOT (V10.0 DMA) ---");
  
  // Initialisation volume a zero pour fade-in
  cur_vol = 0;
  target_vol = 25;
  sendDFCommand(0x09, 0x00, 0x02); // Force source SD
  delay(500);
  sendDFCommand(0x06, 0x00, 30); // Volume MAX (30)
  delay(500);
  extern void playSFX(int id, bool loop);
  playSFX(1, true); // 001.mp3 dans dossier 01 est votre Intro

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
  updateAudioFade();
  extern void handleGameLogic();
  handleGameLogic();
  delay(1);
}
