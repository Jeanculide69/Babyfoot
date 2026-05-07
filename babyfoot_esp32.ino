#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <HTTPUpdateServer.h>
#include "config.h"
#include "web_pages.h"

// --- VARIABLES TOURNOI & WEB (DYNAMIQUE) ---
String team1_name = "JEDI";
String team2_name = "SITH";
bool tournament_mode = false;

// --- CONFIG WIFI (PAR DEFAUT) ---
String wifi_ssid = "Livebox-6E60";
String wifi_pass = "gQszNPUotXSt7jKKH3";
const byte DNS_PORT = 53;
DNSServer dnsServer;
bool is_ap_mode = false;

void loadWiFiConfig() {
  if (LittleFS.exists("/wifi.json")) {
    File f = LittleFS.open("/wifi.json", "r");
    if (f) {
      StaticJsonDocument<256> doc;
      DeserializationError err = deserializeJson(doc, f);
      if (!err) {
        if (doc.containsKey("ssid")) wifi_ssid = doc["ssid"].as<String>();
        if (doc.containsKey("pass")) wifi_pass = doc["pass"].as<String>();
      }
      f.close();
    }
  }
}

void saveWiFiConfig(String s, String p) {
  File f = LittleFS.open("/wifi.json", "w");
  if (f) {
    StaticJsonDocument<256> doc;
    doc["ssid"] = s;
    doc["pass"] = p;
    serializeJson(doc, f);
    f.close();
  }
}

// --- PROTECTION MULTI-CŒUR ---
portMUX_TYPE stateMutex = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t audioMutex;
SemaphoreHandle_t fsMutex;

// --- LOGS SYSTÈME ---
String system_logs[20];
int log_idx = 0;
void addLog(String m) {
  system_logs[log_idx % 20] = m;
  log_idx++;
  Serial.println("[LOG] " + m);
}

// --- MOTEUR DE TOURNOI C++ (Backend-Driven) ---
void updateTournamentProgress(int s1, int s2) {
  if (!tournament_mode) return;
  addLog("Tournoi: Debut mise a jour avec scores " + String(s1) + "-" + String(s2));

  // 1. Prendre le Mutex pour TOUTE l'operation (Lecture + Ecriture)
  if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    addLog("Tournoi: FS occupe (Mutex Timeout)");
    return;
  }

  // 2. Lecture en RAM
  if (!LittleFS.exists("/tournament.json")) {
    xSemaphoreGive(fsMutex);
    addLog("Tournoi: Fichier tournament.json inexistant");
    return;
  }

  File f = LittleFS.open("/tournament.json", "r");
  if (!f) {
    xSemaphoreGive(fsMutex);
    addLog("Tournoi: Erreur ouverture lecture");
    return;
  }

  if (f.size() == 0) {
    f.close();
    xSemaphoreGive(fsMutex);
    addLog("Tournoi: Fichier vide (EmptyInput)");
    return;
  }

  DynamicJsonDocument doc(16384); 
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    xSemaphoreGive(fsMutex);
    addLog("Tournoi: Erreur JSON: " + String(error.c_str()));
    return;
  }

  // 3. Mise a jour des donnees en RAM
  if (doc.containsKey("activeMatch") && !doc["activeMatch"].isNull()) {
    int r = doc["activeMatch"]["r"];
    int m = doc["activeMatch"]["m"];
    addLog("Tournoi: Match actif trouve: [" + String(r) + "][" + String(m) + "]");
    
    // On verifie que la structure est coherente
    if (r < (int)doc["rounds"].size() && m < (int)doc["rounds"][r].size()) {
      JsonObject match = doc["rounds"][r][m];
      match["s1"] = s1;
      match["s2"] = s2;
      int winner = (s1 > s2) ? 1 : 2;
      match["winner"] = winner;
      
      String winnerName = (winner == 1) ? match["t1"].as<String>() : match["t2"].as<String>();
      addLog("Tournoi: Vainqueur: " + winnerName);

      // Avancement automatique vers le round suivant
      if (r + 1 < (int)doc["rounds"].size()) {
        int nextM = m / 2;
        addLog("Tournoi: Avancement vers [" + String(r+1) + "][" + String(nextM) + "]");
        if (m % 2 == 0) {
            doc["rounds"][r + 1][nextM]["t1"] = winnerName;
            addLog("Tournoi: " + winnerName + " place en T1 du match suivant");
        } else {
            doc["rounds"][r + 1][nextM]["t2"] = winnerName;
            addLog("Tournoi: " + winnerName + " place en T2 du match suivant");
        }
      } else {
        addLog("Tournoi: FINALE TERMINEE !");
      }

      // Desactiver le match actif
      doc.remove("activeMatch"); 
      addLog("Tournoi: Match actif supprime du JSON");

      // 4. Ecriture securisee (on n'ouvre en "w" que maintenant)
      addLog("Tournoi: Ouverture fichier pour ecriture...");
      f = LittleFS.open("/tournament.json", "w");
      if (f) {
        if (serializeJson(doc, f) == 0) {
          addLog("Tournoi: ERREUR CRITIQUE SERIALISATION !");
        } else {
          addLog("Tournoi: Sauvegarde OK (" + String(f.size()) + " octets)");
        }
        f.close();
      } else {
        addLog("Tournoi: Erreur ouverture mode 'w'");
      }
    } else {
      addLog("Tournoi: Indices r/m hors limites !");
    }
  } else {
    addLog("Tournoi: Aucun activeMatch dans le JSON");
  }

  xSemaphoreGive(fsMutex);
  addLog("Tournoi: Fin de procedure");
}

// --- CHARGEMENT AUTOMATIQUE DU PROCHAIN MATCH ---
bool autoLoadNextMatch() {
  if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    addLog("Auto-Load: FS occupe");
    return false;
  }
  
  File f = LittleFS.open("/tournament.json", "r");
  if (!f) { xSemaphoreGive(fsMutex); return false; }
  
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  bool found = false;
  if (!error) {
    for (int r = 0; r < (int)doc["rounds"].size() && !found; r++) {
      for (int m = 0; m < (int)doc["rounds"][r].size() && !found; m++) {
        JsonObject match = doc["rounds"][r][m];
        if (match["winner"].isNull() && !match["t1"].isNull() && !match["t2"].isNull() && match["t1"] != "BYE" && match["t2"] != "BYE") {
          team1_name = match["t1"].as<String>();
          team2_name = match["t2"].as<String>();
          
          doc["activeMatch"]["r"] = r;
          doc["activeMatch"]["m"] = m;
          found = true;
          addLog("Auto-Load: " + team1_name + " vs " + team2_name);
        }
      }
    }
    
    if (found) {
      f = LittleFS.open("/tournament.json", "w");
      if (f) { serializeJson(doc, f); f.close(); }
    } else {
      tournament_mode = false;
      addLog("Tournoi: CHAMPIONNAT TERMINE !");
    }
  }
  xSemaphoreGive(fsMutex);
  return found;
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
  
  if (audioMutex != NULL) {
    if (xSemaphoreTake(audioMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      Serial1.write(buf, 10);
      xSemaphoreGive(audioMutex);
    }
  }
  
  Serial.printf("[AUDIO] Send CMD: 0x%02X | P1: 0x%02X | P2: 0x%02X\n", cmd, p1, p2);
}

volatile int target_vol = 25;

WebServer server(80);
HTTPUpdateServer httpUpdater;

void handleStatusLegacy() {
  String json = "{\"p1\":" + String(score_p1) + ",\"p2\":" + String(score_p2) + ",\"ball\":" + String(ball) + "}";
  server.send(200, "application/json", json);
}

extern void handleAction(String act);

#include <ESPmDNS.h>

void setup() {
  fsMutex = xSemaphoreCreateMutex();
  audioMutex = xSemaphoreCreateMutex();
  Serial.begin(115200);
  addLog("Systeme Pret ! babyfoot.local");
  Serial.println("[SYSTEM] Ready.");
  
  // --- CHARGEMENT CONFIG WIFI ---
  loadWiFiConfig();
  
  if (wifi_ssid != "") {
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    Serial.print("[WIFI] Connexion a " + wifi_ssid + "...");
    
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
      delay(500); Serial.print(".");
      timeout++;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] Connecte !");
    Serial.print("[WIFI] IP Locale: "); Serial.println(WiFi.localIP());
    if (MDNS.begin("babyfoot")) {
      Serial.println("[WIFI] mDNS: http://babyfoot.local");
      MDNS.addService("http", "tcp", 80);
    }
  } else {
    Serial.println("\n[WIFI] Connexion échouée. Basculement en mode Point d'Accès (AP)...");
    is_ap_mode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP("Babyfoot-Force-StarWars");
    dnsServer.start(DNS_PORT, "*", IPAddress(192, 168, 4, 1));
    Serial.print("[WIFI] Reseau Ouvert: Babyfoot-Force-StarWars | IP: "); 
    Serial.println(WiFi.softAPIP());
    addLog("Mode AP Active: Babyfoot-Force");
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
    StaticJsonDocument<2048> doc;
    JsonArray logs = doc.createNestedArray("logs");
    for(int i=0; i<20; i++) {
      int idx = (log_idx - 1 - i + 20) % 20;
      if(system_logs[idx] != "") {
        JsonObject entry = logs.createNestedObject();
        entry["t"] = millis()/1000;
        entry["m"] = system_logs[idx];
      }
    }
    String out; serializeJson(doc, out);
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
    doc["finished"] = bitRead(statut_game, MATCH_FINISHED);
    String out; serializeJson(doc, out);
    server.send(200, "application/json", out);
  });

  server.on("/api/save_settings", []() {
    if (server.hasArg("t1")) team1_name = server.arg("t1");
    if (server.hasArg("t2")) team2_name = server.arg("t2");
    if (server.hasArg("vol")) target_vol = server.arg("vol").toInt();
    if (server.hasArg("mode")) {
      tournament_mode = (server.arg("mode") == "1");
      if (tournament_mode && server.hasArg("t1")) {
         portENTER_CRITICAL(&stateMutex);
         score_p1 = 0; score_p2 = 0; ball = 11;
         statut_game = 0; 
         bitSet(statut_game, START_GAME); // Bit 5 : Attente du bouton OK
         bitClear(statut_game, RUN);      // Bit 0 : Pas encore en cours
         portEXIT_CRITICAL(&stateMutex);
         
         extern void playSFX(int id, bool loop);
         playSFX(SFX_INTRO, true); 
         addLog("Match Prepare : " + team1_name + " vs " + team2_name);
      }
    }
    server.send(200, "text/plain", "OK");
  });

  // --- API TOURNOI (PERSISTANCE) ---
  server.on("/api/get_tournament", HTTP_GET, []() {
    if (LittleFS.exists("/tournament.json")) {
      if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
        File f = LittleFS.open("/tournament.json", "r");
        server.streamFile(f, "application/json");
        f.close();
        xSemaphoreGive(fsMutex);
      } else {
        server.send(503, "text/plain", "FS Busy");
      }
    } else {
      server.send(200, "application/json", "{\"teams\":[],\"rounds\":[]}");
    }
  });

  server.on("/api/set_tournament", HTTP_POST, []() {
    if (!server.hasArg("plain")) {
      server.send(400, "text/plain", "Body Missing");
      return;
    }
    
    String body = server.arg("plain");
    size_t len = body.length();

    if (xSemaphoreTake(fsMutex, pdMS_TO_TICKS(2000)) != pdTRUE) { 
        server.send(503, "text/plain", "FS Busy"); 
        return; 
    }
    
    // 1. Ecriture atomique via tampon interne
    File f = LittleFS.open("/tournament.json.tmp", "w");
    if (!f) { 
        xSemaphoreGive(fsMutex);
        server.send(500, "text/plain", "FS Error"); 
        return; 
    }

    size_t written = f.print(body);
    f.close();

    // 2. Verification et renommage
    if (written == len && len > 0) {
      LittleFS.remove("/tournament.json");
      if (LittleFS.rename("/tournament.json.tmp", "/tournament.json")) {
        addLog("Tournoi: Configuration enregistree (" + String(written) + " octets)");
        server.send(200, "text/plain", "Saved");
      } else {
        addLog("Tournoi: Erreur renommage !");
        server.send(500, "text/plain", "Rename Error");
      }
    } else {
      LittleFS.remove("/tournament.json.tmp");
      addLog("Tournoi: Echec ecriture (" + String(written) + "/" + String(len) + ")");
      server.send(500, "text/plain", "Write Incomplete");
    }

    xSemaphoreGive(fsMutex);
  });

  server.on("/api/test_sfx", HTTP_GET, []() {
    if (server.hasArg("id")) {
      extern void playSFX(int id, bool loop);
      playSFX(server.arg("id").toInt(), false);
      server.send(200, "text/plain", "SFX OK");
    } else server.send(400);
  });

  server.on("/api/confirm_match", HTTP_GET, []() {
    extern void requestAnimation(int type);
    portENTER_CRITICAL(&stateMutex);
    bitClear(statut_game, MATCH_FINISHED);
    bitSet(statut_game, START_GAME);
    ball = 11; score_p1 = 0; score_p2 = 0;
    portEXIT_CRITICAL(&stateMutex);
    
    requestAnimation(ANIM_NONE);
    extern void playSFX(int id, bool loop);
    playSFX(SFX_INTRO, true); // Relance l'intro
    addLog("Match Confirme. Pret pour le suivant.");
    server.send(200, "text/plain", "OK");
  });

  server.on("/api/test_anim", HTTP_GET, []() {
    if (server.hasArg("id")) {
      extern void requestAnimation(int id);
      requestAnimation(server.arg("id").toInt());
      server.send(200, "text/plain", "ANIM OK");
    } else server.send(400);
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

  server.on("/update", HTTP_GET, []() {
    server.send_P(200, "text/html", UPDATE_HTML);
  });
  server.on("/action", HTTP_GET, []() {
    String id = server.arg("id");
    if (id == "BR" && server.hasArg("val")) {
      int v = server.arg("val").toInt();
      if (matrix) matrix->setPanelBrightness(v);
    } else if (id == "VL" && server.hasArg("val")) {
      target_vol = server.arg("val").toInt();
      sendDFCommand(0x06, 0x00, (uint8_t)target_vol);
    } else if (id == "AB" && server.hasArg("val")) {
      extern void setNeoBrightness(int b);
      setNeoBrightness(server.arg("val").toInt());
    } else if (id == "NEXT_MATCH") {
      handleAction("OK"); // Relance le cycle de jeu
    } else {
      handleAction(id);
    }
    addLog("Action recue: " + id);
    server.send(200, "text/plain", "OK");
  });

  server.on("/wifi", HTTP_GET, []() {
    server.send_P(200, "text/html", WIFI_HTML);
  });

  server.on("/api/wifi_scan", HTTP_GET, []() {
    int n = WiFi.scanNetworks();
    StaticJsonDocument<2048> doc;
    JsonArray root = doc.to<JsonArray>();
    for (int i = 0; i < n; ++i) {
      JsonObject item = root.createNestedObject();
      item["ssid"] = WiFi.SSID(i);
      item["rssi"] = WiFi.RSSI(i);
    }
    String out; serializeJson(doc, out);
    server.send(200, "application/json", out);
  });

  server.on("/api/save_wifi", HTTP_GET, []() {
    String s = server.arg("ssid");
    String p = server.arg("pass");
    if (s != "") {
      saveWiFiConfig(s, p);
      server.send(200, "text/plain", "OK");
      delay(2000);
      ESP.restart();
    } else server.send(400);
  });
  server.onNotFound([]() {
    if (is_ap_mode) {
      server.sendHeader("Location", String("http://192.168.4.1/wifi"), true);
      server.send(302, "text/plain", "");
    } else {
      server.send(404, "text/plain", "Not Found");
    }
  });

  httpUpdater.setup(&server, "/do_update");
  const char* headerkeys[] = {"Content-Length"};
  server.collectHeaders(headerkeys, 1);
  server.begin();

  // IMPULSION RESET HARDWARE (Obligatoire pour débloquer les capteurs)
  pinMode(25, OUTPUT);
  digitalWrite(25, 0); 
  delay(20);
  digitalWrite(25, 1);

  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(500);
  Serial.println("\n\n[SYS] --- BABYFOOT MASTER CONSOLE V1.0 (TOURNAMENT) ---");
  
  // Initialisation volume
  target_vol = 25;
  sendDFCommand(0x09, 0x00, 0x02); // Force source SD
  delay(500);
  sendDFCommand(0x06, 0x00, 30); // Volume MAX (30)
  delay(500);
  extern void playSFX(int id, bool loop);
  playSFX(SFX_INTRO, true); // 001.mp3 dans dossier 01 est votre Intro

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
  xTaskCreatePinnedToCore(webTask, "WebTask", 8192, NULL, 1, NULL, 0);
}

// Tâche dédiée au serveur Web sur le Core 0
void webTask(void *pvParameters) {
  while (true) {
    if (is_ap_mode) dnsServer.processNextRequest();
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10)); // Utilise le delai FreeRTOS natif pour liberer le CPU
  }
}

void loop() {
  extern void handleGameLogic();
  extern void updateLEDs();
  handleGameLogic();
  updateLEDs();
}
