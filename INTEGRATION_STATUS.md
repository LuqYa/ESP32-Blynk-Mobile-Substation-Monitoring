# Integration Status

## 1. GitHub

Status: CONNECTED

Repository:

`LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring`

Branch:

`main`

GitHub is the source of truth for the ESP32 firmware, Blynk setup documentation, and Render bridge code.

## 2. Render

Status: DEPLOYED

Service:

`openai-blynk-bridge`

Public URL:

`https://openai-blynk-bridge.onrender.com`

Default ESP32 advisory endpoint:

`https://openai-blynk-bridge.onrender.com/esp32-analyse`

Health endpoint:

`https://openai-blynk-bridge.onrender.com/health`

The default direct integration does NOT require a Blynk Device Token or a Blynk webhook on Render.

## 3. Blynk

Status: ESP32 CONNECTION REQUIRED

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

V0-V10 are written by the ESP32 monitoring firmware.
V11-V12 are also written by the ESP32 after it receives an advisory response from Render.

No Blynk webhook is required for the default integration.

## 4. Default End-to-End Flow

```text
Sensors
  -> ESP32 threshold/trend/rule analysis
  -> Blynk V0-V10
  -> Render /esp32-analyse when an anomaly occurs
  -> Render rule-based advisory (or optional OpenAI advisory)
  -> ESP32 receives SUMMARY/ACTION
  -> Blynk V11/V12
```

GitHub provides the source code used by Render and the ESP32 firmware.

## 5. Optional OpenAI Mode

The Render bridge works without OpenAI using deterministic rule-based advisory output.

To enable OpenAI advisory later, configure both:

- `OPENAI_API_KEY` in Render
- `BRIDGE_SHARED_SECRET` in Render and the same value locally in `firmware/secrets.h`

If those values are not configured, the direct integration remains functional in `RULE` mode.

## 6. Legacy Optional Blynk Webhook Mode

The old webhook path remains available at:

`https://openai-blynk-bridge.onrender.com/blynk-webhook`

It requires:

- `BLYNK_DEVICE_TOKEN`
- `WEBHOOK_SECRET`
- a Blynk V9 webhook

This mode is optional and is no longer required for the normal FYP demonstration.

## 7. Remaining Physical/Account Actions

The code and Render bridge are configured. To operate the physical prototype, the user still must:

1. Ensure V0-V12 exist in the Blynk template.
2. Keep Wi-Fi and Blynk credentials only in local `firmware/secrets.h`.
3. Compile and upload the latest firmware to the physical ESP32.
4. Confirm V0-V10 update in Blynk.
5. Trigger a controlled anomaly and confirm V11/V12 update after the Render response.

Never commit Wi-Fi passwords, Blynk Auth Tokens, OpenAI API keys, or private shared secrets to GitHub.
