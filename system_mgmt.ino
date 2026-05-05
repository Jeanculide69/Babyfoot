#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <EEPROM.h>
#include <Update.h>

WebServer server(80);
extern bool recovery_mode;
extern bool wifi_connected;
extern int score_rouge;
extern int score_bleu;
extern void handleAction(String action);

const char* ssid = "Livebox-6E60";         
const char* password = "gQszNPUotXSt7jKKH3"; 

#define MAX_LOGS 50
String system_logs[MAX_LOGS];
int log_index = 0;

void addLog(String msg) {
  system_logs[log_index] = "[" + String(millis()/1000) + "s] " + msg;
  log_index = (log_index + 1) % MAX_LOGS;
  Serial.println(msg);
}

#define RECOVERY_PIN 0

void setupWiFi() {
  addLog("Tentative WiFi : " + String(ssid));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[SYS] Connecté ! RSSI: " + String(WiFi.RSSI()));
  }
}

void setupSystem() {
  setCpuFrequencyMhz(240); // Puissance maximale pour le WiFi
  setupWiFi();
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.setSleep(false); 
    esp_wifi_set_ps(WIFI_PS_NONE); 
    WiFi.setTxPower(WIFI_POWER_19_5dBm); 
    
    if (MDNS.begin("Babyfoot")) {
      addLog("mDNS actif : http://Babyfoot.local");
    }
  }

  setupWebHandlers();
  server.begin();
  wifi_connected = true;
  addLog("Portail Web Actif : http://" + WiFi.localIP().toString());
}

void setupWebHandlers() {
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      else Update.printError(Serial);
    }
  });

  server.on("/", HTTP_GET, []() {
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'></head>";
    html += "<body style='background:#111;color:#eee;font-family:sans-serif;text-align:center;'>";
    html += "<h1>Babyfoot Live</h1>";
    html += "<div style='font-size:3em;margin:20px;'>" + String(score_rouge) + " - " + String(score_bleu) + "</div>";
    html += "<button style='padding:15px;margin:5px;' onclick=\"fetch('/act?a=r1').then(()=>location.reload())\">+1 ROUGE</button>";
    html += "<button style='padding:15px;margin:5px;' onclick=\"fetch('/act?a=b1').then(()=>location.reload())\">+1 BLEU</button><br><br>";
    html += "<button style='background:gray;' onclick=\"location.href='/logs'\">VOIR LOGS</button>";
    html += "<button style='background:darkred;' onclick=\"location.href='/update'\">MAJ OTA</button>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/logs", HTTP_GET, []() {
    String l = "--- DERNIERS LOGS SYSTEME ---\n\n";
    for (int i = 0; i < MAX_LOGS; i++) {
      int idx = (log_index + i) % MAX_LOGS;
      if (system_logs[idx] != "") l += system_logs[idx] + "\n";
    }
    server.send(200, "text/plain", l);
  });

  server.on("/act", HTTP_GET, []() {
    if (server.hasArg("a")) handleAction(server.arg("a"));
    server.send(204);
  });

  server.on("/state", HTTP_GET, []() {
    extern int balles_restantes;
    extern int current_game_state;
    String json = "{\"r\":" + String(score_rouge) + ",\"b\":" + String(score_bleu) + ",\"l\":" + String(balles_restantes) + ",\"s\":" + String(current_game_state) + "}";
    server.send(200, "application/json", json);
  });
}

void handleSystem() {
  static unsigned long last_debug = 0;
  if (millis() - last_debug > 2000) {
    Serial.printf("[SYS] WiFi:%d | RSSI:%d | Heap:%d\n", WiFi.status(), WiFi.RSSI(), ESP.getFreeHeap());
    last_debug = millis();
  }
  server.handleClient();
}
