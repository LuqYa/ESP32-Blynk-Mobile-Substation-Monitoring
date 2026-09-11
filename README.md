# ESP32-Blynk Mobile Substation Monitoring

Laboratory-scale IoT condition monitoring prototype for a 33/11 kV mobile substation using ESP32, Blynk, DHT11 and PZEM-004T.

## Connected Architecture

```text
DHT11 + PZEM-004T
        |
        v
      ESP32
        |
        | Wi-Fi
        v
      Blynk
   V0 ... V10
        |
        | V9 webhook
        v
      Render
        |
        | advisory result
        v
   Blynk V11/V12

GitHub main branch
        |
        | auto-deploy
        v
      Render
```

The ESP32 remains responsible for the core monitoring logic: threshold checking, trend analysis, rule-based anomaly detection and Normal/Warning/Critical classification. Render is an integration/advisory layer. OpenAI is optional; if it is unavailable, Render uses a rule-based advisory fallback.

## Current Render Service

```text
Service: openai-blynk-bridge
URL: https://openai-blynk-bridge.onrender.com
Webhook: https://openai-blynk-bridge.onrender.com/blynk-webhook
Health: https://openai-blynk-bridge.onrender.com/health
Ready: https://openai-blynk-bridge.onrender.com/ready
```

GitHub auto-deploy is enabled for the `main` branch.

## Blynk Datastream Map

| Pin | Parameter | Source |
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
| V9 | Anomaly Flag | ESP32 |
| V10 | Condition Reason | ESP32 |
| V11 | Advisory Summary | Render |
| V12 | Recommended Check | Render |

## Required Private Configuration

### ESP32 local `firmware/secrets.h`

```text
WIFI_SSID
WIFI_PASSWORD
BLYNK_AUTH_TOKEN
BLYNK_TEMPLATE_ID
BLYNK_TEMPLATE_NAME
```

### Render Environment

Required:

```text
BLYNK_SERVER
BLYNK_DEVICE_TOKEN
WEBHOOK_SECRET
```

Optional AI upgrade:

```text
OPENAI_API_KEY
OPENAI_MODEL
```

Never commit real passwords, tokens, API keys or webhook secrets to GitHub.

## Blynk Webhook

Create a Blynk webhook using:

```text
Trigger: Device Datastream Update
Device: FYP Mobile Substation Prototype
Datastream: V9 - Anomaly Flag
Method: POST
URL: https://openai-blynk-bridge.onrender.com/blynk-webhook
```

Header:

```text
X-Webhook-Secret: <same WEBHOOK_SECRET stored in Render>
```

Body:

```json
{
  "pin": "{device_pin}",
  "value": "{device_pinValue}",
  "device": "{device_name}",
  "timestamp": "{timestamp_iso8601}"
}
```

## Main Hardware

- ESP32
- DHT11 temperature and humidity sensor
- PZEM-004T AC monitoring module
- Current transformer (CT)
- Laboratory test load and supporting components

## Main Files

- `firmware/mobile_substation_monitoring.ino` — ESP32 firmware
- `firmware/secrets.example.h` — local credential template
- `blynk/dashboard_setup.md` — Blynk datastream/dashboard setup
- `integration/openai-blynk-bridge/server.js` — Render bridge
- `integration/openai-blynk-bridge/README.md` — integration guide
- `render.yaml` — Render deployment definition
- `testing/test_procedure.md` — formal test procedure
- `testing/test_results.csv` — real test-result template
- `INTEGRATION_STATUS.md` — live integration checklist

## Safety

This is a laboratory-scale prototype. Do not connect prototype wiring directly to energized 33 kV or 11 kV equipment. Use appropriate isolation, protection, supervision and safe laboratory procedures. The advisory layer must not be used as a protection relay, interlock or switching authority.

## Academic Context

Final Year Project — Bachelor of Technology in Electrical System Maintenance with Honours, Universiti Tun Hussein Onn Malaysia (UTHM).
