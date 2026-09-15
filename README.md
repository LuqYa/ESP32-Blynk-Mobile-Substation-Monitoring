# ESP32-Blynk Mobile Substation Monitoring

[![ESP32 Firmware Build](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/firmware-build.yml/badge.svg)](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/firmware-build.yml)
[![Render Bridge Integration Test](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/render-bridge-test.yml/badge.svg)](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/render-bridge-test.yml)
[![Repository Safety Check](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/repo-safety-check.yml/badge.svg)](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/repo-safety-check.yml)

Laboratory-scale IoT condition monitoring prototype for a 33/11 kV mobile substation using ESP32, Blynk, DHT11 and PZEM-004T.

## Connected Architecture

DHT11 + PZEM-004T -> ESP32 local condition analysis -> Blynk V0–V11.

ESP32 implements thresholds, four-reading trends, three-reading recovery, multiple anomalies, sensor-health colors and four event notifications. Render remains an optional advisory endpoint and never writes to the datastreams. Current firmware does not automatically request Render advisory.

## Current Render Service

[openai-blynk-bridge](https://openai-blynk-bridge.onrender.com/health) auto-deploys from main. Its /health response identifies V0-V11 and ESP32 decision ownership. The legacy webhook returns HTTP 410.

## Blynk Datastream Map

| Pin | Parameter |
|---|---|
| V0 | Temperature |
| V1 | Humidity (Integer) |
| V2 | AC Voltage |
| V3 | AC Current |
| V4 | Overall Condition |
| V5 | Anomaly Cause |
| V6 | Recommended Action |
| V7 | DHT Status LED |
| V8 | PZEM Status LED |
| V9 | Trend Status |
| V10 | Alarm Level: 0 NORMAL, 1 WARNING, 2 CRITICAL, 3 SYSTEM FAULT |
| V11 | Active Anomalies (0–4) |

V7/V8 stay at 1 and change green/red by sensor health. See [dashboard setup and event codes](blynk/dashboard_setup.md), [thresholds](testing/thresholds.md), and [tests](testing/test_procedure.md).

## Required Private ESP32 Configuration

Create `firmware/secrets.h` locally from `firmware/secrets.example.h` and provide:

```text
WIFI_SSID
WIFI_PASSWORD
BLYNK_AUTH_TOKEN
BLYNK_TEMPLATE_ID
BLYNK_TEMPLATE_NAME
```

Never commit the real values to GitHub.

## Bring-Up and Validation

1. Configure the V0–V11 template and four notifications in Blynk.
2. Keep credentials locally, compile and upload firmware/firmware.ino.
3. Verify latest PZEM UART wiring: ESP32 RX=13, TX=12; DHT GPIO=2.
4. Run [physical bring-up](testing/physical_bringup_checklist.md) and [behavior tests](testing/test_procedure.md).
5. Record actual results in [test_results_v0_v11.csv](testing/test_results_v0_v11.csv).

GitHub Actions verifies compilation, monitoring behavior and the checked-out bridge code. Physical sensor behavior, LED colors and phone notification delivery require hardware/account verification. A Render deployment does not flash the ESP32.

## Optional Advisory

[Render integration guide](integration/openai-blynk-bridge/README.md) documents /esp32-analyse and optional AI credentials. V0–V11 always remain ESP32-owned. The old V9 webhook is retired to prevent overwriting the integer anomaly count.

## Main Hardware

- ESP32
- DHT11 temperature and humidity sensor
- PZEM-004T AC monitoring module
- Current transformer (CT)
- Laboratory test load and supporting components

## Main Files

- `firmware/firmware.ino` — Arduino sketch entry point
- `firmware/mobile_substation_monitoring.ino` — ESP32 acquisition, Blynk updates and events
- `firmware/secrets.example.h` — local credential template
- `blynk/dashboard_setup.md` — Blynk datastream/dashboard setup
- `hardware/connection_summary.md` — planned low-voltage/PZEM connection summary
- `integration/openai-blynk-bridge/server.js` — Render bridge
- `integration/openai-blynk-bridge/README.md` — integration guide
- `render.yaml` — Render deployment definition
- `testing/physical_bringup_checklist.md` — staged physical bring-up sequence
- `testing/test_procedure.md` — formal test procedure
- `testing/direct_render_blynk_test.md` — direct integration test sequence
- `testing/data_collection_plan.md` — FYP data collection plan
- `testing/test_results_v0_v11.csv` — current test-result template
- `INTEGRATION_STATUS.md` — integration checklist

## Safety

This is a laboratory-scale prototype. Do not connect prototype wiring directly to energized 33 kV or 11 kV equipment. Use appropriate isolation, protection, supervision and safe laboratory procedures. Verify the exact PZEM-004T hardware version and its interface requirements before connecting it to the ESP32 or any AC measurement circuit. The advisory layer must not be used as a protection relay, interlock or switching authority.

## Academic Context

Final Year Project — Bachelor of Technology in Electrical System Maintenance with Honours, Universiti Tun Hussein Onn Malaysia (UTHM).
