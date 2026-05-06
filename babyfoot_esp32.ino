#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <WebServer.h>
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

WebServer server(80);

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>";
  html += "body { background: #000; color: #FFE000; font-family: sans-serif; text-align: center; }";
  html += ".btn { display: block; width: 80%; margin: 20px auto; padding: 20px; font-size: 24px; border: 2px solid #FFE000; border-radius: 10px; background: #222; color: #FFE000; text-decoration: none; }";
  html += ".btn:active { background: #444; } .jedi { border-color: #00F; color: #00F; } .sith { border-color: #F00; color: #F00; }";
  html += "</style></head><body>";
  html += "<h1>BABYFOOT CONTROL</h1>";
  html += "<a href='/action?id=B1' class='btn jedi'>BUT JEDI (P1)</a>";
  html += "<a href='/action?id=B2' class='btn sith'>BUT SITH (P2)</a>";
  html += "<a href='/action?id=G1' class='btn jedi'>GAMELLE JEDI</a>";
  html += "<a href='/action?id=G2' class='btn sith'>GAMELLE SITH</a>";
  html += "<a href='/action?id=OK' class='btn' style='border-color:#FFF;color:#FFF;'>BOUTON OK</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

extern void handleAction(String act);

#include <ESPmDNS.h>

void setup() {
  Serial.begin(115200);
  Serial.println("\n[SYSTEM] Babyfoot Star Wars Starting...");

  // Connexion WiFi à la Livebox
  WiFi.mode(WIFI_STA);
  WiFi.begin("Livebox-6E60", "gQszNPUotXSt7jKKH3");
  
  Serial.print("[WIFI] Connecting to Livebox...");
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] Connected !");
    Serial.print("[WIFI] IP Address: "); Serial.println(WiFi.localIP());
    if (MDNS.begin("babyfoot")) {
      Serial.println("[WIFI] mDNS started: http://babyfoot.local");
    }
  } else {
    Serial.println("\n[WIFI] Connection failed. Check credentials.");
  }

  server.on("/", handleRoot);
  server.on("/action", []() {
    if (server.hasArg("id")) handleAction(server.arg("id"));
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.begin();

  // IMPULSION RESET HARDWARE (Obligatoire pour débloquer les capteurs)
  pinMode(25, OUTPUT);
  digitalWrite(25, 0); 
  delay(20);
  digitalWrite(25, 1);

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
  server.handleClient();
  extern void handleGameLogic();
  handleGameLogic();
  delay(1);
}
