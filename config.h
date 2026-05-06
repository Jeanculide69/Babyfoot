#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- CONFIGURATION MATERIELLE (SYNCHRO DMA) ---
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 1

#define R1_PIN  15
#define G1_PIN  2
#define B1_PIN  0
#define R2_PIN  4
#define G2_PIN  16
#define B2_PIN  17
#define A_PIN   5
#define B_PIN   18
#define C_PIN   19
#define D_PIN   21
#define E_PIN   -1
#define LAT_PIN 3
#define OE_PIN  23
#define CLK_PIN 22

#define RX_PIN 12
#define TX_PIN 13
#define LED_PIN_1 26
#define LED_PIN_2 14
#define LED_COUNT 40

#define GOAL_RIGHT    36
#define GAMELLE_RIGHT 39
#define GOAL_LEFT     34
#define GAMELLE_LEFT  35

#define BTN_LESS 32
#define BTN_MORE 33
#define BTN_OK   27
#define RESET_PIN 25

#define RUN              0
#define START_GAME       5

#define C_BLACK   0x0000
#define C_WHITE   0xFFFF
#define C_YELLOW  0xFFE0
#define C_RED     0xF800
#define C_GREEN   0x07E0
#define C_BLUE    0x001F
#define C_CYAN    0x07FF
#define C_MAGENTA 0xF81F

// --- STRUCTURES POUR ANIMATIONS ---
struct Particle {
  float x, y, vx, vy;
  int life;
};

struct Fighter {
  int x, y;
  int state; 
  int frame;
  uint16_t color;
  int dir;
  int targetX;
};

#endif
