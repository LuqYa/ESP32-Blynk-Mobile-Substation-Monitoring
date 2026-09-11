# Integration Status

## 1. GitHub

Status: CONNECTED

Repository:

`LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring`

Branch:

`main`

GitHub is the source of truth for firmware, Blynk setup documentation and the Render bridge code.

## 2. Render

Status: DEPLOYED / LIVE

Service:

`openai-blynk-bridge`

Public URL:

`https://openai-blynk-bridge.onrender.com`

Latest rebuilt GitHub commit has been deployed successfully.

Render service configuration shows `autoDeploy=yes`, but the latest GitHub commits did not trigger a deploy automatically. The latest version was therefore deployed manually through the Render integration. Native GitHub authorization in Render should be checked if automatic deploys are required for future commits.

Required Render environment variables:

- `BLYNK_SERVER`
- `BLYNK_DEVICE_TOKEN`
- `WEBHOOK_SECRET`

Optional:

- `OPENAI_API_KEY`
- `OPENAI_MODEL`

Current bridge mode without an OpenAI API key: rule-based advisory fallback.

## 3. Blynk

Status: REQUIRES ACCOUNT-SIDE PRIVATE CONFIGURATION

Required datastreams:

- V0 Temperature
- V1 Humidity
- V2 Voltage
- V3 Current
- V4 Overall Condition
- V5 Power
- V6 Frequency
- V7 Power Factor
- V8 Anomaly Message
- V9 Anomaly Flag
- V10 Condition Reason
- V11 Advisory Summary
- V12 Recommended Check

Required webhook:

```text
Trigger: Device Datastream Update
Datastream: V9
Method: POST
URL: https://openai-blynk-bridge.onrender.com/blynk-webhook
Header: X-Webhook-Secret = same value as WEBHOOK_SECRET in Render
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

## 4. End-to-End Flow

```text
ESP32 -> Blynk V0-V10 -> V9 webhook -> Render -> V11/V12 -> Blynk dashboard
GitHub repository -> Render deployment
```

## 5. Remaining Private Inputs

The following values must never be committed to GitHub:

- Wi-Fi SSID/password
- Blynk Template ID/Auth Token
- Blynk Device Token for Render
- Webhook secret
- OpenAI API key, if AI advisory is enabled

Current Render logs show the only required missing value is `BLYNK_DEVICE_TOKEN`.

Once `BLYNK_DEVICE_TOKEN` is present in Render and the V9 webhook is created in Blynk, the GitHub-Blynk-Render chain is complete.
