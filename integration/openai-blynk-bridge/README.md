# ESP32 ↔ Render ↔ Blynk Advisory Bridge

This service supports the FYP laboratory prototype without requiring a Blynk webhook.

## Default Architecture

```text
DHT11 / PZEM-004T
        |
        v
      ESP32
        |
        +-----------------> Blynk V0-V10
        |
        | anomaly detected
        v
Render /esp32-analyse
        |
        v
Rule-based advisory
(optional OpenAI enhancement)
        |
        v
SUMMARY / ACTION
        |
        v
      ESP32
        |
        v
 Blynk V11 / V12
```

The ESP32 remains responsible for threshold, trend, and rule-based Normal / Warning / Critical logic. Render provides only an advisory layer.

## Public Endpoints

Health:

`GET https://openai-blynk-bridge.onrender.com/health`

Readiness/status:

`GET https://openai-blynk-bridge.onrender.com/ready`

Default ESP32 advisory endpoint:

`POST https://openai-blynk-bridge.onrender.com/esp32-analyse`

Legacy optional Blynk webhook endpoint:

`POST https://openai-blynk-bridge.onrender.com/blynk-webhook`

## Default ESP32 Request

Example body:

```json
{
  "temperature": 36.5,
  "humidity": 65.0,
  "voltage": 230.0,
  "current": 2.1,
  "condition": "WARNING",
  "trend": "Rapid temperature rise",
  "reason": "Rapid temperature rise"
}
```

The service returns plain text that is easy for the ESP32 to parse:

```text
SUMMARY:Temperature-related abnormal trend detected by the ESP32 prototype logic.
ACTION:Verify DHT11 temperature with a reference thermometer and inspect the laboratory heat source and ventilation.
MODE:RULE
```

The firmware writes SUMMARY to Blynk V11 and ACTION to Blynk V12.

## Rule Mode

Rule mode is the default and requires no OpenAI API key and no Blynk Device Token on Render.

This keeps the FYP demonstration functional even if the optional AI layer is unavailable.

## Optional OpenAI Mode

OpenAI is used only when all of these are configured:

- `OPENAI_API_KEY` in Render
- `BRIDGE_SHARED_SECRET` in Render
- the same `BRIDGE_SHARED_SECRET` is defined locally in `firmware/secrets.h`

The ESP32 then sends that secret in the `X-Bridge-Secret` header.

If the key/secret is missing or the OpenAI request fails, the service automatically falls back to RULE mode.

Never commit the OpenAI API key or private shared secret to GitHub.

## Legacy Optional Blynk Webhook Mode

The previous Blynk webhook mode is retained for experiments but is not needed for the normal FYP demonstration.

It requires:

- `BLYNK_DEVICE_TOKEN` in Render
- `WEBHOOK_SECRET` in Render
- Blynk webhook configured on V9

Endpoint:

`https://openai-blynk-bridge.onrender.com/blynk-webhook`

## Render Deployment

Repository:

`https://github.com/LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring`

Service:

`openai-blynk-bridge`

Region:

`Singapore`

Build command:

`cd integration/openai-blynk-bridge && npm install`

Start command:

`cd integration/openai-blynk-bridge && npm start`

The repository also contains a `render.yaml` Blueprint definition.

## Safety

This bridge is for a laboratory monitoring prototype only. It must not be used as a protection relay, interlock, switching controller, or automatic operating authority for energized substation equipment.
