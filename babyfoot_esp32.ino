#include <Adafruit_GFX.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Adafruit_NeoPixel.h>
#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>
#include "config.h"

// NeoPixels
Adafruit_NeoPixel strip1(LED_COUNT, LED_PIN_1, NEO_RBG + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN_2, NEO_RBG + NEO_KHZ800);

// Audio (Serial 1)
HardwareSerial mp3Serial(1);
DFRobotDFPlayerMini DFPlayer;

// --- ETAT GLOBAL ---
MatrixPanel_I2S_DMA *dma_display = nullptr;
bool recovery_mode = false;
bool wifi_connected = false;
int current_frame = 0;
unsigned long last_frame = 0;

// Task Handles
TaskHandle_t TaskSystemHandle;
TaskHandle_t TaskAnimationHandle;

// Externs
extern void setupSystem();
extern void handleSystem();
extern void setupWebHandlers();
extern void handleGameLogic();
extern unsigned long event_display_end;
extern String current_event_text;
extern uint16_t current_event_color;
extern int score_rouge;
extern int score_bleu;
extern int balles_restantes;
extern bool match_active;
extern String getMatchTime();

// Animations Externs
extern void drawAnimButJ1();
extern void drawAnimButJ2();
extern void drawAnimGamelleJ1();
extern void drawAnimGamelleJ2();
extern void drawAnimIntro();
extern void drawAnimBiere();
extern void drawAnimVictoireJ1();
extern void drawAnimVictoireJ2();
extern void drawAnimStandby();

void setup() {
  // Console
  Serial.begin(115200, SERIAL_8N1, -1, 1);
  
  // Detection du mode RECOVERY (Bouton BOOT maintenu au démarrage)
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) == LOW) {
    recovery_mode = true;
    Serial.println("\n[SYS] !!! MODE RECOVERY ACTIVE !!!");
  }

  Serial.println("\n[SYS] Booting Babyfoot V5.4...");

  pinMode(GOAL_RIGHT, INPUT);
  pinMode(GAMELLE_RIGHT, INPUT);
  pinMode(GOAL_LEFT, INPUT);
  pinMode(GAMELLE_LEFT, INPUT);
  pinMode(BTN_LESS, INPUT_PULLUP);
  pinMode(BTN_MORE, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, 1);

  mp3Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  if (DFPlayer.begin(mp3Serial)) {
    Serial.println("[SYS] Audio OK");
    DFPlayer.volume(20);
  }

  strip1.begin();
  strip2.begin();
  strip1.show();
  strip2.show();

  setupSystem();

  xTaskCreatePinnedToCore(TaskSystem, "TaskSystem", 8192, NULL, 10, &TaskSystemHandle, 0);

  if (!recovery_mode) {
    xTaskCreatePinnedToCore(TaskAnimation, "TaskAnimation", 20480, NULL, 2, &TaskAnimationHandle, 1);
  }
}

void loop() { vTaskDelete(NULL); }

void TaskSystem(void * pvParameters) {
  for (;;) {
    handleSystem();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void TaskAnimation(void * pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(2000));

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
  mxconfig.gpio.lat = 3; mxconfig.gpio.oe = 23; mxconfig.gpio.clk = 22;
  mxconfig.i2sspeed = static_cast<HUB75_I2S_CFG::clk_speed>(2000000); 
  mxconfig.double_buff = false;
  
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(20);

  for (;;) {
    handleGameLogic(); 
    
    if (dma_display) {
      if (millis() < event_display_end) {
        // --- MODE ANIMATION GIF ---
        if (current_event_text == "BUT ROUGE!") drawAnimButJ1();
        else if (current_event_text == "BUT BLEU!") drawAnimButJ2();
        else if (current_event_text == "GAMELLE!") drawAnimGamelleJ1();
        else if (current_event_text == "DEMI ! BERE?") drawAnimBiere();
        else if (current_event_text == "VICTOIRE ROUGE") drawAnimVictoireJ1();
        else if (current_event_text == "VICTOIRE BLEUE") drawAnimVictoireJ2();
        else if (current_event_text == "KICK OFF!") drawAnimIntro();
        else {
           dma_display->clearScreen();
           dma_display->setTextSize(1);
           dma_display->setTextColor(current_event_color);
           dma_display->setCursor(2, 12); dma_display->print(current_event_text);
        }
      } else {
        // Reset current_frame quand l'animation est finie
        current_frame = 0;

        if (match_active) {
          // --- INTERFACE SCORE ORIGINALE ---
          dma_display->clearScreen();
          dma_display->setTextSize(1);
          dma_display->setTextColor(dma_display->color565(255, 0, 255));
          dma_display->setCursor(4, 1); dma_display->print("HOME");
          dma_display->setTextColor(dma_display->color565(0, 255, 255));
          dma_display->setCursor(34, 1); dma_display->print("GUEST");

          dma_display->setTextSize(2);
          dma_display->setTextColor(0xFFFF);
          dma_display->setCursor(8, 9); dma_display->print(score_rouge);
          dma_display->setCursor(46, 9); dma_display->print(score_bleu);
          
          dma_display->drawRect(24, 11, 15, 11, 0xFFE0);
          dma_display->setTextSize(1);
          dma_display->setTextColor(0xFFE0);
          dma_display->setCursor(29, 13); dma_display->print(balles_restantes);

          dma_display->setTextColor(0x07E0);
          String t = getMatchTime();
          int16_t x1, y1; uint16_t w, h;
          dma_display->getTextBounds(t, 0, 0, &x1, &y1, &w, &h);
          dma_display->setCursor((64 - w) / 2, 24);
          dma_display->print(t);
        } else {
          // --- VEILLE : ANIMATION SOCCER ---
          drawAnimStandby();
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // ~20 FPS pour les GIFs
  }
}
