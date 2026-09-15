#pragma once
#include <cmath>
#include <cstdint>
#include <string>

namespace monitoring {
enum Level { NORMAL = 0, WARNING = 1, CRITICAL = 2, SYSTEM_FAULT = 3 };
inline const char* levelName(Level level) {
  const char* names[] = {"NORMAL", "WARNING", "CRITICAL", "SYSTEM FAULT"};
  return names[level];
}
inline const char* levelColor(Level level) {
  const char* colors[] = {"#23C48E", "#FFC107", "#D3435C", "#6C757D"};
  return colors[level];
}
struct Sample {
  float temperature, humidity, voltage, current;
  bool dhtValid, pzemValid;
};
struct Result {
  Level level = NORMAL;
  std::string cause = "No abnormal condition";
  std::string action = "No inspection required";
  std::string trend = "STABLE";
  unsigned activeAnomalies = 0;
  unsigned recoveryCount = 0;
  bool dhtHealthy = false, pzemHealthy = false;
};
inline void append(std::string& text, const std::string& item) {
  if (!text.empty()) text += " + ";
  text += item;
}

// Four consecutive valid readings, including the newest reading.
class History {
  float values[4] = {};
  unsigned count = 0;
public:
  void add(float value, bool valid) {
    if (!valid || !std::isfinite(value)) { count = 0; return; }
    if (count < 4) values[count++] = value;
    else {
      for (unsigned i = 0; i < 3; ++i) values[i] = values[i + 1];
      values[3] = value;
    }
  }
  // Tiny float-rounding tolerance preserves decimal delta boundaries (e.g. 1.3-1.0).
  bool rising(float delta) const {
    return count == 4 && values[0] < values[1] && values[1] < values[2]
      && values[2] < values[3] && values[3] - values[0] + 0.00001f >= delta;
  }
  bool falling(float delta) const {
    return count == 4 && values[0] > values[1] && values[1] > values[2]
      && values[2] > values[3] && values[0] - values[3] + 0.00001f >= delta;
  }
};

class Monitor {
  History history[4];
  Level heldLevel = NORMAL;
  unsigned normalCount = 0;
public:
  Result update(const Sample& sample) {
    Result result;
    result.dhtHealthy = sample.dhtValid && std::isfinite(sample.temperature)
      && std::isfinite(sample.humidity);
    result.pzemHealthy = sample.pzemValid && std::isfinite(sample.voltage)
      && std::isfinite(sample.current);
    const float values[] = {sample.temperature, sample.humidity, sample.voltage, sample.current};
    const bool valid[] = {result.dhtHealthy, result.dhtHealthy, result.pzemHealthy, result.pzemHealthy};
    for (unsigned i = 0; i < 4; ++i) history[i].add(values[i], valid[i]);

    // Demonstration thresholds only; validate against baseline/equipment data.
    const bool critical[] = {
      values[0] >= 40, values[1] >= 85,
      values[2] <= 200 || values[2] >= 260, values[3] >= 5
    };
    const bool warning[] = {
      values[0] >= 35, values[1] >= 75,
      values[2] <= 210 || values[2] >= 250, values[3] >= 3
    };
    const bool voltageFalling = history[2].falling(5.0f);
    const bool trends[] = {history[0].rising(1.5f), history[1].rising(5.0f),
      voltageFalling || history[2].rising(5.0f), history[3].rising(0.3f)};
    const char* names[] = {"Temperature", "Humidity", "Voltage", "Current"};
    const char* trendNames[] = {"TEMPERATURE RISING", "HUMIDITY RISING",
      voltageFalling ? "VOLTAGE DECREASING" : "VOLTAGE INCREASING", "CURRENT RISING"};
    const char* actions[] = {"Inspect equipment environment and ventilation",
      "Inspect moisture and environmental condition", "Inspect auxiliary AC supply",
      "Inspect connected electrical load"};
    std::string causes, trendText, actionText;
    bool anyCritical = false;
    for (unsigned i = 0; i < 4; ++i) {
      if (!valid[i]) continue;
      if (trends[i]) append(trendText, trendNames[i]);
      if (!warning[i] && !trends[i]) continue;
      ++result.activeAnomalies; // Count each parameter once, including trend-only anomalies.
      anyCritical = anyCritical || critical[i];
      if (warning[i]) {
        std::string detail = critical[i] ? "Critical " : "High ";
        if (i == 2) detail = critical[i] ? "Critical " : "";
        detail += i == 2 ? (values[2] <= 210 ? "Undervoltage" : "Overvoltage") : names[i];
        append(causes, detail);
      } else append(causes, std::string("Persistent trend: ") + trendNames[i]);
      append(actionText, actions[i]);
    }
    result.level = anyCritical || result.activeAnomalies >= 2 ? CRITICAL
      : result.activeAnomalies ? WARNING : NORMAL;
    if (!result.dhtHealthy) {
      append(causes, "DHT11 Sensor Failure");
      append(actionText, "Inspect DHT sensor and connection");
      result.level = SYSTEM_FAULT;
    }
    if (!result.pzemHealthy) {
      append(causes, "PZEM Communication Failure");
      append(actionText, "Inspect PZEM communication and connection");
      result.level = SYSTEM_FAULT;
    }
    if (!causes.empty()) result.cause = causes;
    if (!actionText.empty()) result.action = actionText;
    if (!trendText.empty()) result.trend = trendText;

    if (result.level != NORMAL) { heldLevel = result.level; normalCount = 0; }
    else if (heldLevel != NORMAL) {
      if (++normalCount >= 3) { heldLevel = NORMAL; normalCount = 0; }
      else {
        result.level = heldLevel;
        result.recoveryCount = normalCount;
        result.cause = "Recovery pending: " + std::to_string(normalCount) + "/3 normal readings";
        result.action = "Continue monitoring until three consecutive normal readings";
      }
    }
    return result;
  }
};

// Coalesce disconnected/cooldown transitions to the latest state. No repeated
// event while severity is unchanged. Device Timeline remains the delivery record.
class Events {
  Level lastLevel = NORMAL;
  uint32_t lastSent[4] = {};
  bool sent[4] = {};
public:
  bool due(Level level, uint32_t now, bool connected) const {
    return connected && level != lastLevel
      && (!sent[level] || uint32_t(now - lastSent[level]) >= 60000UL);
  }
  void markSent(Level level, uint32_t now) {
    lastLevel = level; lastSent[level] = now; sent[level] = true;
  }
  static const char* code(Level level) {
    const char* codes[] = {"condition_recovered", "condition_warning", "critical_condition", "system_fault"};
    return codes[level];
  }
};
} // namespace monitoring
