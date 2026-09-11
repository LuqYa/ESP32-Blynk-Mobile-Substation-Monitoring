# GitHub ↔ Render ↔ Blynk Bridge

This integration connects the FYP repository, Render web service and Blynk monitoring dashboard.

## Architecture

```text
GitHub repository
      |
      | auto-deploy
      v
Render Node.js bridge
      ^
      | HTTPS webhook from V9
      |
Blynk Cloud <---- ESP32 sensors
      |
      +---- V0-V10 from ESP32
      +---- V11-V12 from Render bridge
```

OpenAI is optional. If `OPENAI_API_KEY` is configured, the bridge uses AI for a short advisory. If it is not configured or the AI call fails, the bridge uses a rule-based advisory fallback.

The advisory layer is monitoring-only. It must not operate, switch, trip, energise or isolate electrical equipment.

## Blynk Datastreams

| Pin | Name | Type | Source |
|---|---|---|---|
| V0 | Temperature | Double | ESP32 |
| V1 | Humidity | Double | ESP32 |
| V2 | Voltage | Double | ESP32 |
| V3 | Current | Double | ESP32 |
| V4 | Overall Condition | String | ESP32 |
| V5 | Power | Double | ESP32 |
| V6 | Frequency | Double | ESP32 |
| V7 | Power Factor | Double | ESP32 |
| V8 | Anomaly Message | String | ESP32 |
| V9 | Anomaly Flag | Integer | ESP32 |
| V10 | Condition Reason | String | ESP32 |
| V11 | Advisory Summary | String | Render bridge |
| V12 | Recommended Check | String | Render bridge |

## Render Service

Current service name:

`openai-blynk-bridge`

Current public URL:

`https://openai-blynk-bridge.onrender.com`

Endpoints:

```text
GET  /
GET  /health
GET  /ready
POST /blynk-webhook
```

## Required Render Environment Variables

```text
BLYNK_SERVER=https://blynk.cloud
BLYNK_DEVICE_TOKEN=<private Blynk device Auth Token>
WEBHOOK_SECRET=<private random secret>
```

Optional:

```text
OPENAI_API_KEY=<private OpenAI API key>
OPENAI_MODEL=gpt-5.6-luna
```

Do not commit any real token, API key or webhook secret to GitHub.

## Blynk Webhook

In Blynk Console create a webhook with:

```text
Trigger: Device Datastream Update
Device: FYP Mobile Substation Prototype
Datastream: V9 - Anomaly Flag
Method: POST
URL: https://openai-blynk-bridge.onrender.com/blynk-webhook
```

Header:

```text
X-Webhook-Secret: <same value as WEBHOOK_SECRET in Render>
```

Custom JSON body:

```json
{
  "pin": "{device_pin}",
  "value": "{device_pinValue}",
  "device": "{device_name}",
  "timestamp": "{timestamp_iso8601}"
}
```

## End-to-End Test

1. Confirm ESP32 updates Blynk V0-V10.
2. Open `https://openai-blynk-bridge.onrender.com/health`.
3. Confirm `configured` becomes `true` after `BLYNK_DEVICE_TOKEN` and `WEBHOOK_SECRET` are set.
4. Create the V9 webhook in Blynk.
5. Trigger a controlled anomaly in the laboratory prototype.
6. Confirm Blynk changes V9 to `1`.
7. Confirm Render receives `POST /blynk-webhook`.
8. Confirm V11 and V12 are updated in Blynk.
9. Record the result separately from the ESP32 Normal/Warning/Critical classification.

## FYP Positioning

The core FYP remains ESP32 + Blynk + threshold checking + trend analysis + rule-based anomaly detection. Render provides the integration/advisory layer. OpenAI is optional and is not the primary detection algorithm.
