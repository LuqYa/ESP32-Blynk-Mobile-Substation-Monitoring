# V0-V11 Test Procedure

Use a supervised laboratory prototype and suitable reference instruments. Simulated/software tests can verify logic without creating hazardous electrical conditions. Do not connect this prototype directly to energized 33 kV or 11 kV equipment.

## Setup

1. Follow [firmware setup](../firmware/README.md) and [Blynk setup](../blynk/dashboard_setup.md).
2. Verify GPIO 2 DHT, GPIO 13 ESP32 RX from PZEM TX, GPIO 12 ESP32 TX to PZEM RX.
3. Confirm device online, V0–V3 readings and V7/V8 green LEDs.
4. Use [thresholds.md](thresholds.md) for exact boundaries and four-reading trend deltas.

## Cases

| Case | Stimulus | Expected |
|---|---|---|
| Normal | Stable 28 °C, 55 %, 230 V, 1 A | NORMAL; V10=0, V11=0, V9=STABLE |
| Temperature | 35 / 40 °C | WARNING / CRITICAL |
| Humidity | 75 / 85 % | WARNING / CRITICAL |
| Low voltage | 210 / 200 V | WARNING / CRITICAL |
| High voltage | 250 / 260 V | WARNING / CRITICAL |
| Current | 3 / 5 A | WARNING / CRITICAL |
| Temperature trend | 28, 28.5, 29, 29.5 °C | WARNING on fourth sample; V11=1 |
| Humidity trend | 55, 57, 59, 61 % | WARNING; HUMIDITY RISING |
| Voltage trends | 230, 232, 234, 236 V; separately reverse | WARNING; rising/decreasing text |
| Current trend | 1, 1.2, 1.4, 1.6 A | WARNING |
| Non-monotonic | Temperature 28, 29, 28.5, 30 °C | No trend warning |
| Multiple anomalies | Temperature 35 °C plus current 3 A | CRITICAL; V11=2; both causes |
| Threshold and trend same parameter | Temperature 35,36,37,38 °C | WARNING; V11=1 |
| DHT fault | Invalid DHT readings | SYSTEM FAULT; V7 red; V8 reflects PZEM |
| PZEM fault | Invalid PZEM readings | SYSTEM FAULT; V8 red; V7 reflects DHT |
| Both faults | Both invalid | Both causes; both LEDs red; V11=0 |
| Fault plus healthy anomaly | DHT invalid; current 5 A | SYSTEM FAULT; current cause retained; V11=1 |
| Recovery | Three normal evaluations after abnormality | Previous alarm for first two; NORMAL on third |
| Interrupted recovery | Normal, abnormal, normal | Recovery count restarts at 1 |
| Wi-Fi loss | Disconnect/reconnect network | Local monitoring continues; latest values/colors restored |

For threshold-only tests, hold other parameters stable and allow any earlier trend to leave the four-reading history. Invalid readings must break history so trends cannot bridge a fault.

## Events and phone notifications

Verify WARNING -> condition_warning, CRITICAL -> critical_condition, SYSTEM FAULT -> system_fault, confirmed NORMAL -> condition_recovered in Blynk Device Timeline and on the configured phone. Confirm steady severity produces no repeated event. Each code has a 60-second firmware cooldown; account notification limits also apply. Record event timestamps and any cooldown/offline suppression.

## Automated checks

- C++ behavior tests: `g++ -std=c++17 -Wall -Wextra -Werror testing/monitoring_logic_test.cpp -o monitoring-test && ./monitoring-test`
- Arduino compile: existing firmware-build workflow.
- Bridge tests: `npm test` inside integration/openai-blynk-bridge after installing dependencies.
- Physical LEDs, sensor acquisition and actual push notification delivery require hardware/account tests.

Record actual readings only. The older [test_results.csv](test_results.csv) is retained unchanged as a legacy template; use [test_results_v0_v11.csv](test_results_v0_v11.csv) for this architecture.
