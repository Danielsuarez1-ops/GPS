#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ================== CONFIG ================== */
const char* WIFI_SSID    = "MiHotspot24";       // <-- o "Redmi Note 13"
const char* WIFI_PASS    = "Prueba1234";        // <-- tu contraseña

// Usamos HTTP (no HTTPS) y puerto explícito para evitar problemas TLS.
const char* TS_WRITE_URL = "http://api.thingspeak.com:80/update";

// TU Write Key real:
const char* TS_WRITE_KEY = "XS4L6XY58TRL2MHB";

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define OLED_ADDR     0x3C
#define I2C_SDA       21
#define I2C_SCL       22

// LED de estado (ajusta pin/polaridad si es necesario)
#define LED_PIN        2         // si no ves LED, prueba 5 o 13
#define LED_ACTIVE_LOW 1         // 1 = activo LOW (GPIO2 típico)

const unsigned long TS_INTERVAL = 15000; // mínimo 15 s (plan free)

/* ================== LED Helper ================== */
inline void setLed(bool on) {
#if LED_ACTIVE_LOW
  digitalWrite(LED_PIN, on ? LOW : HIGH);
#else
  digitalWrite(LED_PIN, on ? HIGH : LOW);
#endif
}

/* ================== OBJETOS ================== */
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
unsigned long lastSend = 0;

/* ================== UI helpers ================== */
void showText(const String &l1, const String &l2 = "", const String &l3 = "",
              const String &l4 = "", const String &l5 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);  display.println(l1);
  display.setCursor(0,10); display.println(l2);
  display.setCursor(0,20); display.println(l3);
  display.setCursor(0,30); display.println(l4);
  display.setCursor(0,40); display.println(l5);
  display.display();
}

/* ================== WiFi ================== */
void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  Serial.printf("\nConectando a WiFi '%s'...\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 20000) {
    setLed(true);  delay(180);
    setLed(false); delay(180);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado!");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    Serial.print("RSSI: "); Serial.println(WiFi.RSSI());
    setLed(true);            // LED fijo = conectado
  } else {
    Serial.println("No se pudo conectar al WiFi.");
    setLed(false);
  }
}

/* ================== ThingSpeak (POST + plan B GET) ================== */
bool sendToThingSpeak(double lat, double lon, float vel_kmh, float alt_m, int sats) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClient client;
  HTTPClient http;
  http.setReuse(false);
  http.setTimeout(10000);

  // ---- POST estándar (application/x-www-form-urlencoded) ----
  if (!http.begin(client, TS_WRITE_URL)) {
    Serial.println("[TS POST] begin() fallo");
    return false;
  }

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String body = "api_key=" + String(TS_WRITE_KEY) +
                "&field1=" + String(lat, 6) +
                "&field2=" + String(lon, 6) +
                "&field3=" + String(vel_kmh, 1) +
                "&field4=" + String(alt_m, 1) +
                "&field5=" + String(sats);

  int code = http.POST(body);
  String resp = http.getString();
  http.end();

  Serial.printf("[TS POST] code=%d resp=%s\n", code, resp.c_str());

  // Mostrar código en la última línea de la OLED
  display.fillRect(0, 54, 128, 10, SSD1306_BLACK);
  display.setCursor(0, 54);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.printf("HTTP:%d", code);
  display.display();

  if (code == 200 && resp.toInt() > 0) return true;

  // ---- PLAN B: GET simple ----
  String urlGet = String(TS_WRITE_URL) + "?api_key=" + TS_WRITE_KEY +
                  "&field1=" + String(lat, 6) +
                  "&field2=" + String(lon, 6) +
                  "&field3=" + String(vel_kmh, 1) +
                  "&field4=" + String(alt_m, 1) +
                  "&field5=" + String(sats);

  HTTPClient http2;
  http2.setReuse(false);
  http2.setTimeout(10000);

  if (!http2.begin(client, urlGet)) {
    Serial.println("[TS GET ] begin() fallo");
    return false;
  }

  int code2 = http2.GET();
  String resp2 = http2.getString();
  http2.end();

  Serial.printf("[TS GET ] code=%d resp=%s\n", code2, resp2.c_str());
  display.setCursor(60, 54);
  display.printf(" G:%d", code2);
  display.display();

  return (code2 == 200 && resp2.toInt() > 0);
}

/* ================== Simulador GPS ================== */
void generarSimulados(double &lat, double &lon, float &vel, float &alt, int &sats) {
  const double baseLat = 4.633;     // ciudad base (cámbiala si quieres)
  const double baseLon = -74.068;
  int dx = random(-40, 40);
  int dy = random(-40, 40);
  lat = baseLat + dx / 10000.0;
  lon = baseLon + dy / 10000.0;
  vel = random(0, 40);              // km/h
  alt = 2550 + random(-10, 10);     // m
  sats = random(5, 12);
}

/* ================== SETUP / LOOP ================== */
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LED_PIN, OUTPUT);
  setLed(false); // apagado al inicio

  // OLED
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("ERROR: No se detecta SSD1306 en 0x3C.");
    while (true) { setLed(true); delay(120); setLed(false); delay(120); }
  }
  showText("ESP32 + Simulador GPS", "Inicializando...");

  // WiFi
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    showText("Sin WiFi", "Reconectando...");
    connectWifi();
    delay(1500);
    return;
  }

  unsigned long now = millis();
  if (now - lastSend >= TS_INTERVAL) {
    lastSend = now;

    double lat, lon; float vel, alt; int sats;
    generarSimulados(lat, lon, vel, alt, sats);

    // OLED (datos)
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);  display.println("Simulador GPS");
    display.setCursor(0,12); display.print("Lat: "); display.println(lat, 6);
    display.setCursor(0,22); display.print("Lon: "); display.println(lon, 6);
    display.setCursor(0,32); display.print("Vel: "); display.print(vel, 1); display.println(" km/h");
    display.setCursor(0,42); display.print("Alt: "); display.print(alt, 1); display.println(" m");
    display.display();

    // Diagnóstico extra por Serial
    Serial.print("IP ");   Serial.println(WiFi.localIP());
    Serial.print("RSSI "); Serial.println(WiFi.RSSI());

    // Envío
    bool ok = sendToThingSpeak(lat, lon, vel, alt, sats);
    if (ok) {
      showText("Enviado a ThingSpeak", "HTTP 200 OK", "Revisa tu canal");
      setLed(false); delay(70); setLed(true);  // tick de confirmación
    } else {
      showText("Error enviando", "Revisa WiFi o WriteKey");
      setLed(false); delay(250); setLed(true); delay(250);
    }
  }
}
