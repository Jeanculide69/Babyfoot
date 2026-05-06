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

extern volatile int score_p1, score_p2, ball;
extern volatile unsigned long statut_game;

void handleStatus() {
  String json = "{";
  json += "\"p1\":" + String(score_p1) + ",";
  json += "\"p2\":" + String(score_p2) + ",";
  json += "\"ball\":" + String(ball) + ",";
  json += "\"heap\":" + String(ESP.getFreeHeap()/1024);
  json += "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><title>Babyfoot Force Console</title>";
  html += "<style>body{background:#0b0e14;color:#FFE000;font-family:'Segoe UI',sans-serif;text-align:center;margin:0;padding:20px;}";
  html += ".card{background:#161b22;border-radius:15px;padding:20px;margin-bottom:20px;box-shadow:0 4px 15px rgba(0,0,0,0.5);border:1px solid #30363d;}";
  html += ".score-grid{display:grid;grid-template-columns:1fr 1fr;gap:15px;margin-bottom:20px;}";
  html += ".score-val{font-size:48px;font-weight:bold;text-shadow:0 0 10px currentColor;}";
  html += ".btn{display:block;padding:15px;margin:10px 0;font-size:18px;font-weight:bold;border-radius:8px;text-decoration:none;transition:0.2s;border:2px solid;}";
  html += ".jedi{color:#58a6ff;border-color:#58a6ff;background:rgba(88,166,255,0.1);}";
  html += ".sith{color:#f85149;border-color:#f85149;background:rgba(248,81,73,0.1);}";
  html += ".btn:active{transform:scale(0.95);opacity:0.8;}";
  html += "#info{font-size:12px;color:#8b949e;margin-top:10px;}</style>";
  html += "<script>function doAct(id){fetch('/action?id='+id);}";
  html += "function setBright(v){fetch('/action?id=BR&val='+v);}";
  html += "setInterval(()=>{fetch('/status').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('s1').innerText=d.p1;";
  html += "document.getElementById('s2').innerText=d.p2;";
  html += "document.getElementById('bl').innerText=d.ball;";
  html += "document.getElementById('hp').innerText=d.heap+' KB';});},1000);</script></head><body>";
  
  html += "<div class='card'><h2>LIVE SCORE</h2><div class='score-grid'>";
  html += "<div><div style='color:#58a6ff'>JEDI</div>";
  html += "<div class='score-grid' style='grid-template-columns:1fr 2fr 1fr;align-items:center;'>";
  html += "<div class='btn jedi' style='padding:5px' onclick='doAct(\"M1\")'>-</div>";
  html += "<div id='s1' class='score-val' style='color:#58a6ff'>0</div>";
  html += "<div class='btn jedi' style='padding:5px' onclick='doAct(\"P1\")'>+</div></div></div>";
  
  html += "<div><div style='color:#f85149'>SITH</div>";
  html += "<div class='score-grid' style='grid-template-columns:1fr 2fr 1fr;align-items:center;'>";
  html += "<div class='btn sith' style='padding:5px' onclick='doAct(\"M2\")'>-</div>";
  html += "<div id='s2' class='score-val' style='color:#f85149'>0</div>";
  html += "<div class='btn sith' style='padding:5px' onclick='doAct(\"P2\")'>+</div></div></div>";
  
  html += "</div><div style='font-size:20px'>BALLES: <span id='bl'>11</span></div></div>";

  html += "<div class='card'><h2>LUMINOSITE</h2>";
  html += "<input type='range' min='10' max='255' value='128' style='width:100%;height:30px;' onchange='setBright(this.value)'></div>";

  html += "<div class='card'><h2>COMMANDES RAPIDES</h2>";
  html += "<div class='score-grid'><div class='btn jedi' onclick='doAct(\"B1\")'>BUT</div><div class='btn sith' onclick='doAct(\"B2\")'>BUT</div></div>";
  html += "<div class='score-grid'><div class='btn jedi' onclick='doAct(\"G1\")'>GAMELLE</div><div class='btn sith' onclick='doAct(\"G2\")'>GAMELLE</div></div>";
  html += "<div class='btn' style='color:#fff;border-color:#fff;' onclick='doAct(\"OK\")'>START / RESET</div></div>";
  
  html += "<div id='info'>RAM: <span id='hp'>-</span> | babyfoot.local</div></body></html>";
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
  server.on("/status", handleStatus);
  server.on("/action", []() {
    if (server.hasArg("id")) {
      String id = server.arg("id");
      if (id == "BR" && server.hasArg("val")) {
        int v = server.arg("val").toInt();
        if (matrix) matrix->setPanelBrightness(v);
        Serial.print("[WIFI] Brightness set to: "); Serial.println(v);
      } else {
        handleAction(id);
      }
    }
    server.send(200, "text/plain", "OK");
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
  extern void setupLEDs();
  setupGame();
  setupLEDs();

  // Lancer le serveur web sur le Core 0
  xTaskCreatePinnedToCore(webTask, "WebTask", 4096, NULL, 1, NULL, 0);
}

// Tâche dédiée au serveur Web sur le Core 0
void webTask(void *pvParameters) {
  while (true) {
    server.handleClient();
    delay(10); // Laisse le processeur respirer
  }
}

void loop() {
  extern void handleGameLogic();
  extern void updateLEDs();
  handleGameLogic();
  updateLEDs();
}
