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

#define RX_PIN 13
#define TX_PIN 12
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

#define DELAY_BUTTOM 5
#define SENS_SET     700 

#define RUN              0
#define START_GAME       5
#define MATCH_FINISHED   6
#define SCORE_ADJUST     8
#define SELECT_P1        9
#define SELECT_P2        10
#define PLAYER_CONFIRMED 11
#define DEMI             12
#define LAST_GOAL_P1     13
#define LAST_GOAL_P2     14

// --- TYPES ANIMATIONS ---
#define ANIM_NONE    0
#define ANIM_BUT_J1  1
#define ANIM_BUT_J2  2
#define ANIM_GAM_J1  3
#define ANIM_GAM_J2  4
#define ANIM_BIERE   5
#define ANIM_VIC_J1  6
#define ANIM_VIC_J2  7
#define ANIM_BALLE_MATCH 8

// --- DEFINITIONS AUDIO (SFX) ---
#define SFX_INTRO      1
#define SFX_BUT_J1     2
#define SFX_BUT_J2     3
#define SFX_GAMELLE    4
#define SFX_VIC_J1     5
#define SFX_VIC_J2     6
#define SFX_AMBIANCE   7
#define SFX_MATCH_PT   8

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

struct Blaster {
  float x, y;
  float vx;
  uint16_t color;
  bool active;
};

#endif
