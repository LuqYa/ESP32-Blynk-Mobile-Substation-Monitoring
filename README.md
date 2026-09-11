# ESP32-Blynk Mobile Substation Monitoring

[![ESP32 Firmware Build](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/firmware-build.yml/badge.svg)](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/firmware-build.yml)
[![Render Bridge Integration Test](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/render-bridge-test.yml/badge.svg)](https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring/actions/workflows/render-bridge-test.yml)

Laboratory-scale IoT condition monitoring prototype for a 33/11 kV mobile substation using ESP32, Blynk, DHT11 and PZEM-004T.

## Connected Architecture

```text
DHT11 + PZEM-004T
        |
        v
      ESP32
        |
        +---------------------> Blynk V0-V10
        |
        | abnormal condition
        v
      Render
 /esp32-analyse
        |
        | SUMMARY / ACTION
        v
      ESP32
        |
        v
   Blynk V11/V12

GitHub main branch
        |
        v
   Render deployment
```

The ESP32 remains responsible for threshold checking, trend analysis, rule-based anomaly detection, and Normal/Warning/Critical classification. Render is an advisory layer only.

OpenAI is optional. Without an OpenAI API key, Render uses deterministic rule-based advisory output, so the default FYP demonstration remains functional.

## Verified Automation Status

- ESP32 firmware compile: PASS
- Render health test: PASS
- Temperature warning advisory test: PASS
- Humidity warning advisory test: PASS
- Voltage warning advisory test: PASS
- Current warning advisory test: PASS
- Sensor-fault advisory test: PASS
- Critical-condition advisory test: PASS

## Current Render Service

```text
Service: openai-blynk-bridge
URL: https://openai-blynk-bridge.onrender.com
Direct ESP32 endpoint: https://openai-blynk-bridge.onrender.com/esp32-analyse
Health: https://openai-blynk-bridge.onrender.com/health
Ready: https://openai-blynk-bridge.onrender.com/ready
```

Legacy optional webhook endpoint:

`https://openai-blynk-bridge.onrender.com/blynk-webhook`

## Blynk Datastream Map

| Pin | Parameter | Written By |
|---|---|---|
| V0 | Temperature | ESP32 |
| V1 | Humidity | ESP32 |
| V2 | AC Voltage | ESP32 |
| V3 | AC Current | ESP32 |
| V4 | Overall Condition | ESP32 |
| V5 | Active Power | ESP32 |
| V6 | Frequency | ESP32 |
| V7 | Power Factor | ESP32 |
| V8 | Anomaly Message | ESP32 |
| V9 | Trend Anomaly Flag | ESP32 |
| V10 | Condition Reason | ESP32 |
| V11 | Advisory Summary | ESP32 after Render response |
| V12 | Recommended Check | ESP32 after Render response |

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

## Default Integration

No Blynk webhook and no Blynk Device Token on Render are required for the default setup.

The ESP32 requests a Render advisory when any monitored abnormal condition is active, including:

- threshold WARNING
- threshold CRITICAL
- sensor communication/read fault
- trend anomaly

V9 remains specifically the trend-anomaly flag, so a threshold WARNING/CRITICAL may request Render advisory even when V9 is 0.

The ESP32 sends the current prototype measurements and condition result to:

`https://openai-blynk-bridge.onrender.com/esp32-analyse`

Render returns:

```text
SUMMARY:<short interpretation>
ACTION:<safe laboratory check>
MODE:RULE or AI
```

The ESP32 then writes SUMMARY to V11 and ACTION to V12.

## Optional OpenAI Upgrade

Rule mode works without OpenAI.

To enable optional AI advisory later, configure:

```text
OPENAI_API_KEY in Render
BRIDGE_SHARED_SECRET in Render
```

and define the same `BRIDGE_SHARED_SECRET` locally in `firmware/secrets.h`.

The OpenAI API key must never be placed in the ESP32 firmware or Blynk.

## Legacy Optional Blynk Webhook

The previous V9 webhook integration is retained only as an optional alternative. It requires `BLYNK_DEVICE_TOKEN`, `WEBHOOK_SECRET`, and Blynk webhook configuration. It is not required for the normal FYP demonstration.

## Main Hardware

- ESP32
- DHT11 temperature and humidity sensor
- PZEM-004T AC monitoring module
- Current transformer (CT)
- Laboratory test load and supporting components

## Main Files

- `firmware/firmware.ino` — Arduino sketch entry point
- `firmware/mobile_substation_monitoring.ino` — ESP32 firmware and direct Render integration
- `firmware/secrets.example.h` — local credential template
- `blynk/dashboard_setup.md` — Blynk datastream/dashboard setup
- `integration/openai-blynk-bridge/server.js` — Render bridge
- `integration/openai-blynk-bridge/README.md` — integration guide
- `render.yaml` — Render deployment definition
- `testing/test_procedure.md` — formal test procedure
- `testing/direct_render_blynk_test.md` — direct integration test sequence
- `testing/test_results.csv` — real test-result template
- `INTEGRATION_STATUS.md` — integration checklist

## Safety

This is a laboratory-scale prototype. Do not connect prototype wiring directly to energized 33 kV or 11 kV equipment. Use appropriate isolation, protection, supervision and safe laboratory procedures. The advisory layer must not be used as a protection relay, interlock or switching authority.

## Academic Context

Final Year Project — Bachelor of Technology in Electrical System Maintenance with Honours, Universiti Tun Hussein Onn Malaysia (UTHM).
