#include <ArduinoOTA.h>
#include <Update.h>
#include <WebServer.h>

// Déclarations externes (Définies dans le fichier principal)
extern const char* host_name;
extern WebServer server;

void setupOTA() {
  // ArduinoOTA (pour mise à jour via l'IDE)
  ArduinoOTA.setHostname(host_name);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
    else type = "filesystem";
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
  });
  ArduinoOTA.begin();

  // Web Update (pour mise à jour via navigateur)
  // On utilise [&] pour s'assurer que les variables globales sont bien accessibles dans le scope
  server.on("/update", HTTP_GET, []() {
    String html = "<html><head><title>OTA Update</title>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>body{font-family:sans-serif; background:#1a1a1a; color:white; text-align:center; padding:20px;}";
    html += "input{padding:10px; margin:10px; background:#333; color:white; border:1px solid #555;}";
    html += ".btn{padding:10px 20px; background:#27ae60; color:white; border:none; border-radius:5px; cursor:pointer;}</style></head><body>";
    html += "<h1>Mise à jour Firmware</h1>";
    html += "<form method='POST' action='/do_update' enctype='multipart/form-data'>";
    html += "<input type='file' name='update'><br>";
    html += "<input type='submit' value='Flash !' class='btn' onclick=\"this.value='En cours...';\">";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/do_update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "ECHEC" : "SUCCES - Redémarrage...");
    delay(1000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
}

void handleOTA() {
  ArduinoOTA.handle();
}
