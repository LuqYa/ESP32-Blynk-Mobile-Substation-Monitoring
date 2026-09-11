#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include "secrets.h"

#define DHTPIN 2
#define DHTTYPE DHT11

// PZEM UART pins based on current prototype plan
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Example thresholds. Update these after prototype testing.
const float TEMP_WARNING = 35.0;
const float TEMP_CRITICAL = 40.0;
const float HUM_WARNING = 70.0;
const float HUM_CRITICAL = 80.0;

String classifyCondition(float temperature, float humidity) {
  if (temperature >= TEMP_CRITICAL || humidity >= HUM_CRITICAL) {
    return "CRITICAL";
  }

  if (temperature >= TEMP_WARNING || humidity >= HUM_WARNING) {
    return "WARNING";
  }

  return "NORMAL";
}

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read DHT11 sensor.");
    return;
  }

  String condition = classifyCondition(temperature, humidity);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" % | Condition: ");
  Serial.println(condition);

  // Suggested Blynk virtual pins
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V4, condition);

  // TODO: Add PZEM-004T voltage/current readings to V2 and V3.
  // TODO: Add trend/rule-based anomaly logic after baseline data collection.
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
