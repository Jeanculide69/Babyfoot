#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <HTTPUpdateServer.h>
#include "config.h"
#include "web_pages.h"

// --- VARIABLES TOURNOI & WEB (DYNAMIQUE) ---
String team1_name = "JEDI";
String team2_name = "SITH";
bool tournament_mode = false;

// --- LOGS SYSTÈME ---
String system_logs[20];
int log_idx = 0;
void addLog(String m) {
  system_logs[log_idx % 20] = m;
  log_idx++;
  Serial.println("[LOG] " + m);
}

// Variables globales définies ici
volatile int score_p1 = 0, score_p2 = 0, ball = 11;
volatile uint32_t statut_game = 0;
volatile unsigned int inputs = 0;

MatrixPanel_I2S_DMA *matrix = nullptr;

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
HTTPUpdateServer httpUpdater;

void handleStatusLegacy() {
  String json = "{\"p1\":" + String(score_p1) + ",\"p2\":" + String(score_p2) + ",\"ball\":" + String(ball) + "}";
  server.send(200, "application/json", json);
}

extern void handleAction(String act);

#include <ESPmDNS.h>

void setup() {
  Serial.begin(115200);
  addLog("Systeme Pret ! babyfoot.local");
  Serial.println("[SYSTEM] Ready.");
  
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

  // --- ROUTES EMBARQUÉES (INSTANTANÉES) ---
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });

  server.on("/tv", HTTP_GET, []() {
    server.send_P(200, "text/html", TV_HTML);
  });

  server.on("/tournament", HTTP_GET, []() {
    server.send_P(200, "text/html", TOURNAMENT_HTML);
  });

  server.on("/logs", HTTP_GET, []() {
    server.send_P(200, "text/html", LOGS_HTML);
  });

  server.on("/api/logs", HTTP_GET, []() {
    String out = "{\"logs\":[";
    for(int i=0; i<20; i++) {
      int idx = (log_idx - 1 - i + 20) % 20;
      if(system_logs[idx] != "") {
        if(i > 0) out += ",";
        out += "{\"t\":\"" + String(millis()/1000) + "\",\"m\":\"" + system_logs[idx] + "\"}";
      }
    }
    out += "]}";
    server.send(200, "application/json", out);
  });

  server.on("/api/status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["p1"] = score_p1;
    doc["p2"] = score_p2;
    doc["ball"] = ball;
    doc["t1"] = team1_name;
    doc["t2"] = team2_name;
    doc["mode"] = tournament_mode ? "tournament" : "classic";
    doc["run"] = bitRead(statut_game, RUN);
    String out; serializeJson(doc, out);
    server.send(200, "application/json", out);
  });

  server.on("/api/save_settings", HTTP_POST, []() {
    if (server.hasArg("t1")) team1_name = server.arg("t1");
    if (server.hasArg("t2")) team2_name = server.arg("t2");
    if (server.hasArg("vol")) target_vol = server.arg("vol").toInt();
    if (server.hasArg("mode")) tournament_mode = (server.arg("mode") == "1");
    
    server.send(200, "text/plain", "Settings Saved");
    Serial.println("[WEB] Settings Updated");
  });

  server.on("/settings", HTTP_GET, []() {
    String h = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><title>Settings</title>";
    h += "<style>body{background:#0b0e14;color:#ffe000;font-family:sans-serif;padding:20px;} input{width:100%;padding:10px;margin:10px 0;background:#161b22;color:#fff;border:1px solid #30363d;}</style></head><body>";
    h += "<h1>SETTINGS</h1><form action='/api/save_settings' method='POST'>";
    h += "EQUIPE 1: <input name='t1' value='" + team1_name + "'>";
    h += "EQUIPE 2: <input name='t2' value='" + team2_name + "'>";
    h += "VOLUME (0-30): <input type='number' name='vol' value='" + String(target_vol) + "'>";
    h += "MODE TOURNOI: <select name='mode' style='width:100%;padding:10px;'><option value='0'>OFF</option><option value='1'>ON</option></select><br><br>";
    h += "<button type='submit' style='width:100%;padding:15px;background:#ffe000;font-weight:bold;'>ENREGISTRER</button></form>";
    h += "<br><a href='/' style='color:#8b949e;'>Retour Console</a></body></html>";
    server.send(200, "text/html", h);
  });

  server.on("/api/get_tournament", HTTP_GET, []() {
    File file = LittleFS.open("/tournament.json", "r");
    if (file) {
      server.streamFile(file, "application/json");
      file.close();
    } else {
      server.send(200, "application/json", "{\"teams\":[],\"matches\":[]}");
    }
  });

  server.on("/api/set_tournament", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      File file = LittleFS.open("/tournament.json", "w");
      if (file) {
        file.print(server.arg("plain"));
        file.close();
        server.send(200, "text/plain", "OK");
      } else {
        server.send(500, "text/plain", "Save Failed");
      }
    }
  });
  server.on("/action", HTTP_GET, []() {
    String id = server.arg("id");
    handleAction(id);
    addLog("Action recue: " + id);
    server.send(200, "text/plain", "OK");
  });
  httpUpdater.setup(&server);
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

  // Initialisation LittleFS
  if(!LittleFS.begin(true)){
    Serial.println("[SYSTEM] LittleFS Mount Failed");
  } else {
    Serial.println("[SYSTEM] LittleFS Mounted");
  }

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
