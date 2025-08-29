#include "esp_camera.h"
#include <WiFi.h>

// === WLAN: Verbindung zum ESP32-WROOM AP ===
const char* ssid = "MeinVerstecktesAP";     // muss mit WROOM-AP-SSID übereinstimmen
const char* password = "GeheimesPasswort";  // muss mit WROOM-Passwort übereinstimmen

// Statische IP (empfohlen, damit WROOM sie findet)
IPAddress local_IP(192,168,4,101);   // Kamera 1
IPAddress gateway(192,168,4,1);      // WROOM AP
IPAddress subnet(255,255,255,0);

#define FLASH_GPIO_NUM   4   // LED-Blitz

// === Pinbelegung AI Thinker ESP32-CAM ===
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

// 4 for flash led or 33 for normal led
#define LED_GPIO_NUM   4

#include <WebServer.h>
WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Statische IP setzen
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("⚠️ Statische IP konnte nicht gesetzt werden!");
  }

  WiFi.begin(ssid, password);
  Serial.println("🔌 Verbinde mit WROOM-AP...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WLAN verbunden");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // Blitz-Pin
  pinMode(FLASH_GPIO_NUM, OUTPUT);
  digitalWrite(FLASH_GPIO_NUM, LOW);

  // Kamera konfigurieren
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QVGA;  // klein, stabil
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  // Init
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ Kamera Init fehlgeschlagen!");
    return;
  }

  // Endpoint /takePhoto
  server.on("/takePhoto", HTTP_GET, []() {
    digitalWrite(FLASH_GPIO_NUM, HIGH);
    delay(100);  // Blitz an
    camera_fb_t* fb = esp_camera_fb_get();
    digitalWrite(FLASH_GPIO_NUM, LOW); // Blitz aus

    if (!fb) {
      server.send(500, "text/plain", "Kamera Fehler");
      return;
    }

    server.setContentLength(fb->len);
    server.send(200, "image/jpeg", "");
    WiFiClient client = server.client();
    client.write(fb->buf, fb->len);


    esp_camera_fb_return(fb);
  });

  server.begin();
  Serial.println("🌐 Server gestartet → /takePhoto verfügbar");
}

void loop() {
  server.handleClient();
}
