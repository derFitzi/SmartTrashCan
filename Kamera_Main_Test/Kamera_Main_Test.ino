#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "base64.h"

// === AP-Konfiguration ===
const char* ap_ssid = "MeinVerstecktesAP";
const char* ap_password = "GeheimesPasswort";

// === Kameras IPs im AP (statisch zuweisen empfohlen) ===
const char* camIPs[3] = {
  "http://192.168.4.101/takePhoto",
  "http://192.168.4.102/takePhoto",
  "http://192.168.4.103/takePhoto"
};

// === Raspberry Pi Endpoint (wenn er über AP erreichbar ist) ===
const char* raspberryURL = "http://192.168.4.200:5000/upload";

// Zeitintervall in ms
const unsigned long interval = 10000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);

  // AP starten (verborgene SSID)
  WiFi.softAP(ap_ssid, ap_password, 1, true); // letzter Parameter = hidden SSID
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP gestartet, IP: ");
  Serial.println(IP);

  // Kurze Wartezeit, damit die Kameras sich verbinden
  delay(5000);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    StaticJsonDocument<4096> doc;

    for (int i = 0; i < 3; i++) {
      String b64 = getCameraImage(camIPs[i]);
      if (b64 != "") {
        doc["camera" + String(i+1)] = b64;
      } else {
        doc["camera" + String(i+1)] = "ERROR";
      }
    }

    // JSON serialisieren
    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println("JSON für Raspberry fertig.");

    // An Raspberry Pi senden
    if (WiFi.status() == WL_CONNECTED || true) { // AP-Modus, WiFi.status() zeigt nicht immer richtig
      HTTPClient http;
      http.begin(raspberryURL);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsonString);
      if (httpResponseCode > 0) {
        Serial.printf("POST erfolgreich: %d\n", httpResponseCode);
      } else {
        Serial.printf("POST Fehler: %s\n", http.errorToString(httpResponseCode).c_str());
      }
      http.end();
    }
  }
}

// Funktion: Bild von Kamera holen und Base64 kodieren
String getCameraImage(const char* url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.printf("Fehler bei %s: %d\n", url, httpCode);
    http.end();
    return "";
  }

  WiFiClient* stream = http.getStreamPtr();
  int len = http.getSize();
  if (len <= 0) len = 50000; // fallback, wenn Größe unbekannt

  uint8_t* buf = (uint8_t*)malloc(len);
  if (!buf) {
    Serial.println("Nicht genug RAM!");
    http.end();
    return "";
  }

  stream->readBytes(buf, len);
  String b64 = base64::encode(buf, len);
  free(buf);
  http.end();
  return b64;
}
