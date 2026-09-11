# OpenAI ↔ Blynk Bridge

This optional integration connects Blynk monitoring data to the OpenAI API through a small Node.js server.

## Architecture

```text
ESP32 sensors
   |
   v
Blynk Cloud
   |
   | V9 anomaly update
   v
Blynk Webhook
   |
   v
Node.js bridge
   |
   v
OpenAI Responses API
   |
   v
Blynk V11 / V12
```

The AI is advisory only. It does not control, switch, trip, energise, isolate, or operate electrical equipment.

## New Blynk Datastreams

Create two additional Virtual Pin datastreams:

| Pin | Name | Type | Purpose |
|---|---|---|---|
| V11 | AI Summary | String | Short AI interpretation of the current measurements |
| V12 | AI Recommended Check | String | Safe laboratory inspection/check recommendation |

## 1. Local Installation

Use Node.js 22–24.

```bash
cd integration/openai-blynk-bridge
npm install
cp .env.example .env
```

## 2. Configure `.env`

Fill these locally on the server:

```text
OPENAI_API_KEY=...
OPENAI_MODEL=gpt-5.6-luna
BLYNK_SERVER=https://your-region.blynk.cloud
BLYNK_DEVICE_TOKEN=...
WEBHOOK_SECRET=...
PORT=3000
```

Do not commit `.env`.

Use the exact Blynk server address shown in your Blynk Console. The Device Auth Token is available from Device Info.

## 3. Run Locally

```bash
npm start
```

Health check:

```text
GET /health
```

Expected result:

```json
{"ok":true,"service":"openai-blynk-bridge"}
```

## 4. Deploy with Render

The repository root contains `render.yaml`, so Render can create the service from a Blueprint.

Recommended service settings are already defined:

```text
Service type: Web Service
Runtime: Node
Plan: Free (prototype/testing)
Root Directory: integration/openai-blynk-bridge
Build Command: npm install
Start Command: npm start
Health Check Path: /health
```

During Blueprint creation, enter these environment variables when Render asks for values:

```text
OPENAI_API_KEY       = your OpenAI API key
BLYNK_DEVICE_TOKEN   = your Blynk device Auth Token
BLYNK_SERVER         = exact Blynk regional server URL shown in Blynk Console
WEBHOOK_SECRET       = a long random secret you create
```

The Blueprint sets:

```text
OPENAI_MODEL=gpt-5.6-luna
NODE_ENV=production
```

Do not put API keys or tokens directly into `render.yaml` or any committed file.

After a successful deploy, Render provides an HTTPS URL similar to:

```text
https://openai-blynk-bridge.onrender.com
```

Test:

```text
https://YOUR-RENDER-URL/health
```

You should receive:

```json
{"ok":true,"service":"openai-blynk-bridge"}
```

The Blynk webhook destination is then:

```text
https://YOUR-RENDER-URL/blynk-webhook
```

## 5. Configure the Blynk Webhook

In Blynk Console, go to:

```text
Settings → Developers → Webhooks → Create New Webhook
```

Recommended configuration:

- Trigger Event: `Device Datastream Update`
- Device: your FYP prototype device
- Datastream: `Anomaly Flag (V9)`
- URL: `https://YOUR-RENDER-URL/blynk-webhook`
- Request Type: `POST`
- Content Type: `Custom JSON`

Custom JSON:

```json
{
  "pin": "{device_pin}",
  "value": "{device_pinValue}",
  "device": "{device_name}",
  "timestamp": "{timestamp_iso8601}"
}
```

Add this HTTP header:

```text
X-Webhook-Secret: SAME_VALUE_AS_WEBHOOK_SECRET
```

Use the same `WEBHOOK_SECRET` value that you entered in Render.

Then use **Test Webhook** before creating/enabling it.

## 6. Behaviour

When V9 = 0:
- The bridge does not call OpenAI.
- V11 is updated with a normal-monitoring message.
- V12 recommends continuing baseline monitoring.

When V9 = 1:
- The bridge reads current Blynk datastream values through the Blynk HTTPS API.
- It sends the measurements to the OpenAI Responses API.
- It writes a short interpretation to V11.
- It writes a safe laboratory check recommendation to V12.

## 7. Blynk Values Used by the AI Layer

```text
V0  Temperature
V1  Humidity
V2  Voltage
V3  Current
V4  Overall Condition
V5  Power
V6  Frequency
V7  Power Factor
V8  Anomaly Message
V9  Anomaly Flag
V10 Condition Reason
V11 AI Summary
V12 AI Recommended Check
```

## Security

- Never put the OpenAI API key in ESP32 firmware.
- Never put the OpenAI API key in Blynk datastreams.
- Never commit `.env`.
- Keep the Blynk device token in server environment variables only.
- Protect the webhook with `X-Webhook-Secret`.
- This AI layer must not be used as a protection relay, interlock, switching controller, or automatic operating authority.

## FYP Positioning

This AI bridge is optional and separate from the core ESP32 threshold/trend/rule-based anomaly detection. The base FYP remains functional without AI. If included, describe it as an advisory interpretation layer rather than the primary condition-classification algorithm.
