#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <PZEM004Tv30.h>

// ----------------------------
// Hardware configuration
// ----------------------------
#define DHTPIN 2
#define DHTTYPE DHT11

// ESP32 RX connects to PZEM TX.
// ESP32 TX connects to PZEM RX.
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#define PZEM_SERIAL Serial2

DHT dht(DHTPIN, DHTTYPE);
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
BlynkTimer timer;

// ----------------------------
// Render bridge configuration
// ----------------------------
// Public service URL is safe to keep in source control.
const char *BRIDGE_URL = "https://openai-blynk-bridge.onrender.com/esp32-analyse";
const unsigned long BRIDGE_REFRESH_MS = 60000UL;
unsigned long lastBridgeCall = 0;
String lastBridgeTrigger = "";
bool lastAbnormalState = false;

// ----------------------------
// Prototype thresholds
// ----------------------------
// These are initial laboratory values only. Adjust them after collecting
// baseline data and according to the actual equipment requirements.
const float TEMP_WARNING = 35.0;
const float TEMP_CRITICAL = 40.0;
const float HUM_WARNING = 70.0;
const float HUM_CRITICAL = 80.0;

const float VOLT_WARNING_LOW = 210.0;
const float VOLT_WARNING_HIGH = 250.0;
const float VOLT_CRITICAL_LOW = 200.0;
const float VOLT_CRITICAL_HIGH = 260.0;

const float CURRENT_WARNING = 4.0;
const float CURRENT_CRITICAL = 5.0;

// ----------------------------
// Trend detection configuration
// ----------------------------
// Six samples at a 2-second interval provide an approximately 10-second
// comparison window between the oldest and newest usable sample.
const uint8_t HISTORY_SIZE = 6;
const float TEMP_RISE_ANOMALY = 3.0;       // deg C over history window
const float HUM_RISE_ANOMALY = 10.0;       // %RH over history window
const float VOLT_CHANGE_ANOMALY = 15.0;    // V over history window
const float CURRENT_RISE_ANOMALY = 1.5;    // A over history window

struct Sample {
  float temperature;
  float humidity;
  float voltage;
  float current;
  bool dhtValid;
  bool pzemValid;
};

Sample historyBuffer[HISTORY_SIZE];
uint8_t historyIndex = 0;
uint8_t historyCount = 0;

struct TrendResult {
  bool anomaly;
  String message;
};

struct ConditionResult {
  String level;
  String reason;
};

int severityRank(const String &level) {
  if (level == "CRITICAL") return 2;
  if (level == "WARNING") return 1;
  return 0;
}

void raiseCondition(ConditionResult &result, const String &newLevel, const String &newReason) {
  if (severityRank(newLevel) > severityRank(result.level)) {
    result.level = newLevel;
    result.reason = newReason;
  } else if (severityRank(newLevel) == severityRank(result.level) && newLevel != "NORMAL") {
    if (result.reason.length() > 0) result.reason += "; ";
    result.reason += newReason;
  }
}

TrendResult detectTrend(const Sample &currentSample) {
  TrendResult result = {false, "No abnormal trend"};

  if (historyCount < HISTORY_SIZE) {
    result.message = "Collecting baseline samples";
    return result;
  }

  // historyIndex points to the oldest sample once the circular buffer is full.
  const Sample &oldest = historyBuffer[historyIndex];
  String messages = "";

  if (currentSample.dhtValid && oldest.dhtValid) {
    float tempRise = currentSample.temperature - oldest.temperature;
    float humRise = currentSample.humidity - oldest.humidity;

    if (tempRise >= TEMP_RISE_ANOMALY) {
      result.anomaly = true;
      messages += "Rapid temperature rise";
    }

    if (humRise >= HUM_RISE_ANOMALY) {
      if (messages.length() > 0) messages += "; ";
      result.anomaly = true;
      messages += "Rapid humidity rise";
    }
  }

  if (currentSample.pzemValid && oldest.pzemValid) {
    float voltageChange = fabs(currentSample.voltage - oldest.voltage);
    float currentRise = currentSample.current - oldest.current;

    if (voltageChange >= VOLT_CHANGE_ANOMALY) {
      if (messages.length() > 0) messages += "; ";
      result.anomaly = true;
      messages += "Rapid voltage change";
    }

    if (currentRise >= CURRENT_RISE_ANOMALY) {
      if (messages.length() > 0) messages += "; ";
      result.anomaly = true;
      messages += "Rapid current increase";
    }
  }

  if (result.anomaly) result.message = messages;
  return result;
}

ConditionResult evaluateCondition(const Sample &sample, const TrendResult &trend) {
  ConditionResult result = {"NORMAL", "All monitored parameters within selected limits"};

  if (!sample.dhtValid) {
    raiseCondition(result, "WARNING", "DHT11 communication/read failure");
  } else {
    if (sample.temperature >= TEMP_CRITICAL) {
      raiseCondition(result, "CRITICAL", "Temperature above critical threshold");
    } else if (sample.temperature >= TEMP_WARNING) {
      raiseCondition(result, "WARNING", "Temperature above warning threshold");
    }

    if (sample.humidity >= HUM_CRITICAL) {
      raiseCondition(result, "CRITICAL", "Humidity above critical threshold");
    } else if (sample.humidity >= HUM_WARNING) {
      raiseCondition(result, "WARNING", "Humidity above warning threshold");
    }
  }

  if (!sample.pzemValid) {
    raiseCondition(result, "WARNING", "PZEM-004T communication/read failure");
  } else {
    if (sample.voltage <= VOLT_CRITICAL_LOW || sample.voltage >= VOLT_CRITICAL_HIGH) {
      raiseCondition(result, "CRITICAL", "Voltage outside critical range");
    } else if (sample.voltage <= VOLT_WARNING_LOW || sample.voltage >= VOLT_WARNING_HIGH) {
      raiseCondition(result, "WARNING", "Voltage outside warning range");
    }

    if (sample.current >= CURRENT_CRITICAL) {
      raiseCondition(result, "CRITICAL", "Current above critical threshold");
    } else if (sample.current >= CURRENT_WARNING) {
      raiseCondition(result, "WARNING", "Current above warning threshold");
    }
  }

  if (trend.anomaly) {
    raiseCondition(result, "WARNING", trend.message);
  }

  return result;
}

void storeSample(const Sample &sample) {
  historyBuffer[historyIndex] = sample;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
  if (historyCount < HISTORY_SIZE) historyCount++;
}

String jsonEscape(const String &input) {
  String out = "";
  out.reserve(input.length() + 8);
  for (size_t i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    if (c == '\\' || c == '"') out += '\\';
    if (c == '\n' || c == '\r') {
      out += ' ';
    } else {
      out += c;
    }
  }
  return out;
}

String jsonNumber(float value, bool valid, uint8_t decimals) {
  if (!valid || isnan(value)) return "null";
  return String(value, decimals);
}

String responseLine(const String &body, const String &label) {
  int start = body.indexOf(label);
  if (start < 0) return "";
  start += label.length();
  int end = body.indexOf('\n', start);
  if (end < 0) end = body.length();
  String value = body.substring(start, end);
  value.trim();
  return value;
}

void requestRenderAdvisory(const Sample &sample, const TrendResult &trend, const ConditionResult &condition) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Render bridge: Wi-Fi unavailable");
    Blynk.virtualWrite(V11, "Advisory unavailable: Wi-Fi disconnected");
    Blynk.virtualWrite(V12, "Use the ESP32 local condition result and verify the flagged parameter manually.");
    return;
  }

  WiFiClientSecure client;
  // Laboratory prototype shortcut. For production, pin/validate the server certificate.
  client.setInsecure();

  HTTPClient https;
  https.setTimeout(10000);

  if (!https.begin(client, BRIDGE_URL)) {
    Serial.println("Render bridge: HTTPS initialization failed");
    return;
  }

  https.addHeader("Content-Type", "application/json");
#ifdef BRIDGE_SHARED_SECRET
  https.addHeader("X-Bridge-Secret", BRIDGE_SHARED_SECRET);
#endif

  String payload = "{";
  payload += "\"temperature\":" + jsonNumber(sample.temperature, sample.dhtValid, 1) + ",";
  payload += "\"humidity\":" + jsonNumber(sample.humidity, sample.dhtValid, 1) + ",";
  payload += "\"voltage\":" + jsonNumber(sample.voltage, sample.pzemValid, 1) + ",";
  payload += "\"current\":" + jsonNumber(sample.current, sample.pzemValid, 3) + ",";
  payload += "\"condition\":\"" + jsonEscape(condition.level) + "\",";
  payload += "\"trend\":\"" + jsonEscape(trend.message) + "\",";
  payload += "\"reason\":\"" + jsonEscape(condition.reason) + "\"";
  payload += "}";

  int httpCode = https.POST(payload);

  if (httpCode == HTTP_CODE_OK) {
    String body = https.getString();
    String summary = responseLine(body, "SUMMARY:");
    String action = responseLine(body, "ACTION:");
    String mode = responseLine(body, "MODE:");

    if (summary.length() == 0) summary = "Render returned an empty advisory summary.";
    if (action.length() == 0) action = "Verify the flagged parameter using a laboratory reference instrument.";

    Blynk.virtualWrite(V11, summary);
    Blynk.virtualWrite(V12, action);

    Serial.print("Render mode  : ");
    Serial.println(mode);
    Serial.print("AI/Advisory : ");
    Serial.println(summary);
    Serial.print("Next check   : ");
    Serial.println(action);
  } else {
    Serial.printf("Render bridge HTTP error: %d\n", httpCode);
    Blynk.virtualWrite(V11, "Render advisory unavailable; ESP32 local condition result remains active.");
    Blynk.virtualWrite(V12, "Verify the flagged parameter manually with the laboratory reference instrument.");
  }

  https.end();
}

void handleBridgeAdvisory(const Sample &sample, const TrendResult &trend, const ConditionResult &condition) {
  bool abnormal = trend.anomaly || condition.level != "NORMAL";
  String triggerKey = trend.message + "|" + condition.level + "|" + condition.reason;
  unsigned long now = millis();

  if (abnormal) {
    bool newEvent = !lastAbnormalState || triggerKey != lastBridgeTrigger;
    bool refreshDue = (now - lastBridgeCall) >= BRIDGE_REFRESH_MS;

    if (newEvent || refreshDue) {
      requestRenderAdvisory(sample, trend, condition);
      lastBridgeCall = now;
      lastBridgeTrigger = triggerKey;
    }
  } else if (lastAbnormalState) {
    Blynk.virtualWrite(V11, "No active abnormal condition. ESP32 threshold and trend monitoring continue locally.");
    Blynk.virtualWrite(V12, "Continue monitoring and record baseline data for threshold validation.");
    lastBridgeTrigger = "";
  }

  lastAbnormalState = abnormal;
}

void sendSensorData() {
  Sample sample;

  // DHT11 readings
  sample.humidity = dht.readHumidity();
  sample.temperature = dht.readTemperature();
  sample.dhtValid = !isnan(sample.humidity) && !isnan(sample.temperature);

  // PZEM-004T v3 readings
  sample.voltage = pzem.voltage();
  sample.current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float powerFactor = pzem.pf();

  sample.pzemValid = !isnan(sample.voltage) && !isnan(sample.current);

  TrendResult trend = detectTrend(sample);
  ConditionResult condition = evaluateCondition(sample, trend);

  // Serial Monitor output
  Serial.println("----------------------------------------");

  if (sample.dhtValid) {
    Serial.printf("Temperature : %.1f C\n", sample.temperature);
    Serial.printf("Humidity    : %.1f %%\n", sample.humidity);
  } else {
    Serial.println("DHT11       : READ ERROR");
  }

  if (sample.pzemValid) {
    Serial.printf("Voltage     : %.1f V\n", sample.voltage);
    Serial.printf("Current     : %.3f A\n", sample.current);
    if (!isnan(power)) Serial.printf("Power       : %.1f W\n", power);
    if (!isnan(energy)) Serial.printf("Energy      : %.3f kWh\n", energy);
    if (!isnan(frequency)) Serial.printf("Frequency   : %.1f Hz\n", frequency);
    if (!isnan(powerFactor)) Serial.printf("Power Factor: %.2f\n", powerFactor);
  } else {
    Serial.println("PZEM-004T   : READ ERROR");
  }

  Serial.print("Trend       : ");
  Serial.println(trend.message);
  Serial.print("Condition   : ");
  Serial.println(condition.level);
  Serial.print("Reason      : ");
  Serial.println(condition.reason);

  // Blynk datastreams from ESP32
  if (sample.dhtValid) {
    Blynk.virtualWrite(V0, sample.temperature);
    Blynk.virtualWrite(V1, sample.humidity);
  }

  if (sample.pzemValid) {
    Blynk.virtualWrite(V2, sample.voltage);
    Blynk.virtualWrite(V3, sample.current);
  }

  Blynk.virtualWrite(V4, condition.level);
  if (!isnan(power)) Blynk.virtualWrite(V5, power);
  if (!isnan(frequency)) Blynk.virtualWrite(V6, frequency);
  if (!isnan(powerFactor)) Blynk.virtualWrite(V7, powerFactor);
  Blynk.virtualWrite(V8, trend.message);
  Blynk.virtualWrite(V9, trend.anomaly ? 1 : 0);
  Blynk.virtualWrite(V10, condition.reason);

  // Direct integration: ESP32 -> Render -> ESP32 -> Blynk V11/V12.
  // Triggered for WARNING, CRITICAL, sensor faults, or trend anomalies.
  handleBridgeAdvisory(sample, trend, condition);

  // Store only after trend comparison so the current reading is not compared with itself.
  storeSample(sample);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println();
  Serial.println("ESP32 Mobile Substation Monitoring Prototype");
  Serial.println("Starting Wi-Fi and Blynk connection...");

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  // DHT11 should not be read too frequently. A 2-second monitoring interval
  // is appropriate for this laboratory prototype.
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
