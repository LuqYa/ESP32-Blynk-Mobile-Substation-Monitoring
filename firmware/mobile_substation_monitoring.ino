#include "secrets.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <PZEM004Tv30.h>
#include "monitoring_logic.h"

#define DHTPIN 2
#define DHTTYPE DHT11
// Latest wiring: ESP32 RX <- PZEM TX; ESP32 TX -> PZEM RX.
#define PZEM_RX_PIN 13
#define PZEM_TX_PIN 12
DHT dht(DHTPIN, DHTTYPE);
PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);
BlynkTimer timer;
monitoring::Monitor monitor;
monitoring::Events events;
monitoring::Sample latestSample = {};
monitoring::Result latestResult;
bool haveSample = false;
bool refreshProperties = true;
bool displayedDht = false, displayedPzem = false;
int displayedLevel = -1;
unsigned long lastConnectAttempt = 0;

bool wasConnected = false;

void publishStatus() {
  if (!Blynk.connected() || !haveSample) return;
  if (latestResult.dhtHealthy) {
    Blynk.virtualWrite(V0, latestSample.temperature);
    Blynk.virtualWrite(V1, static_cast<int>(latestSample.humidity));
  }
  if (latestResult.pzemHealthy) {
    Blynk.virtualWrite(V2, latestSample.voltage);
    Blynk.virtualWrite(V3, latestSample.current);
  }
  Blynk.virtualWrite(V4, monitoring::levelName(latestResult.level));
  Blynk.virtualWrite(V5, latestResult.cause.c_str());
  Blynk.virtualWrite(V6, latestResult.action.c_str());
  // V7/V8 are brightness (0..1), NOT boolean health values. Color shows health.
  Blynk.virtualWrite(V7, 1);
  Blynk.virtualWrite(V8, 1);
  Blynk.virtualWrite(V9, latestResult.trend.c_str());
  Blynk.virtualWrite(V10, static_cast<int>(latestResult.level));
  Blynk.virtualWrite(V11, latestResult.activeAnomalies);
  if (refreshProperties || displayedDht != latestResult.dhtHealthy)
    Blynk.setProperty(V7, "color", latestResult.dhtHealthy ? "#23C48E" : "#D3435C");
  if (refreshProperties || displayedPzem != latestResult.pzemHealthy)
    Blynk.setProperty(V8, "color", latestResult.pzemHealthy ? "#23C48E" : "#D3435C");
  if (refreshProperties || displayedLevel != latestResult.level)
    Blynk.setProperty(V4, "color", monitoring::levelColor(latestResult.level));
  displayedDht = latestResult.dhtHealthy;
  displayedPzem = latestResult.pzemHealthy;
  displayedLevel = latestResult.level;
  refreshProperties = false;

  const uint32_t now = millis();
  if (events.due(latestResult.level, now, Blynk.connected())) {
    std::string description = latestResult.level == monitoring::NORMAL
      ? "System condition returned to NORMAL after three consecutive normal readings"
      : latestResult.cause;
    description = description.substr(0, 300);
    Blynk.logEvent(monitoring::Events::code(latestResult.level), description.c_str());
    events.markSent(latestResult.level, now);
  }
}

void sendSensorData() {
  latestSample.temperature = dht.readTemperature();
  latestSample.humidity = dht.readHumidity();
  latestSample.voltage = pzem.voltage();
  latestSample.current = pzem.current();
  latestSample.dhtValid = std::isfinite(latestSample.temperature) && std::isfinite(latestSample.humidity);
  latestSample.pzemValid = std::isfinite(latestSample.voltage) && std::isfinite(latestSample.current);
  latestResult = monitor.update(latestSample);
  haveSample = true;
  Serial.printf("%s | %s | %s | active=%u | DHT=%d PZEM=%d\n",
    monitoring::levelName(latestResult.level), latestResult.cause.c_str(),
    latestResult.trend.c_str(), latestResult.activeAnomalies,
    latestResult.dhtHealthy, latestResult.pzemHealthy);
  publishStatus();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Blynk.config(BLYNK_AUTH_TOKEN);
  // Monitoring starts even when Wi-Fi/Blynk is unavailable.
  timer.setInterval(2000L, sendSensorData);
  Serial.println("MOBIGUARD-MS: ESP32 owns Blynk V0-V11 decisions and events");
}

void loop() {
  const bool connected = Blynk.connected();
  // Restore colors after reconnect without syncing old cloud decisions back.
  if (connected && !wasConnected) refreshProperties = true;
  wasConnected = connected;
  timer.run();
  if (Blynk.connected()) Blynk.run();
  else if (millis() - lastConnectAttempt >= 10000UL) {
    lastConnectAttempt = millis();
    if (WiFi.status() == WL_CONNECTED) Blynk.connect(250);
    else WiFi.reconnect();
  }
}
