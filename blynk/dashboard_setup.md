# Blynk Dashboard Setup

This guide matches `firmware/mobile_substation_monitoring.ino` and the optional OpenAI bridge in `integration/openai-blynk-bridge/`.

## 1. Enable Developer Mode

In Blynk.Console, open your profile and make sure **Developer Mode** is enabled. Template creation and Web Dashboard editing require Developer Mode.

## 2. Create the Device Template

Go to **Developer Zone -> My Templates -> New Template**.

Use:

- Template Name: `Mobile Substation Monitoring`
- Hardware: `ESP32`
- Connection Type: `WiFi`

After creation, Blynk generates a **Template ID**. Keep this value local for use in `firmware/secrets.h`.

## 3. Create Virtual Pin Datastreams

Open the Template -> **Datastreams** -> **Add Datastream** -> **Virtual Pin**.

Create these datastreams exactly:

| Pin | Name | Data Type | Unit | Suggested Min | Suggested Max |
|---|---|---|---|---:|---:|
| V0 | Temperature | Double | °C | 0 | 60 |
| V1 | Humidity | Double | %RH | 0 | 100 |
| V2 | Voltage | Double | V | 0 | 300 |
| V3 | Current | Double | A | 0 | 10 |
| V4 | Overall Condition | String | - | - | - |
| V5 | Power | Double | W | 0 | 2500 |
| V6 | Frequency | Double | Hz | 40 | 60 |
| V7 | Power Factor | Double | - | 0 | 1 |
| V8 | Anomaly Message | String | - | - | - |
| V9 | Anomaly Flag | Integer | - | 0 | 1 |
| V10 | Condition Reason | String | - | - | - |
| V11 | AI Summary | String | - | - | - |
| V12 | AI Recommended Check | String | - | - | - |

V0-V10 are produced by the ESP32 firmware. V11-V12 are written by the optional OpenAI-Blynk bridge.

Enable history for numerical datastreams when available so charts can show recorded trends.

## 4. Create the Web Dashboard

Open the Template -> **Web Dashboard** -> **Edit**.

### System Status

- Overall Condition -> V4
- Anomaly Indicator -> V9
- Anomaly Message -> V8
- Condition Reason -> V10

### Environmental Monitoring

- Temperature Gauge / Value -> V0
- Humidity Gauge / Value -> V1
- Chart -> V0 and V1

### Electrical Monitoring

- Voltage Gauge / Value -> V2
- Current Gauge / Value -> V3
- Power Value -> V5
- Frequency Value -> V6
- Power Factor Value -> V7
- Chart -> V2 and V3

### AI Advisory

- AI Summary -> V11
- AI Recommended Check -> V12

Keep this section visually separate from the ESP32 condition state so the dashboard makes clear that AI is advisory only.

## 5. Create the Device

Go to **Devices / Search -> New Device -> From Template**.

- Template: `Mobile Substation Monitoring`
- Device Name: `FYP Mobile Substation Prototype`

Create the device, open **Device Info**, and copy its **Auth Token** locally. Do not commit the Auth Token to GitHub.

## 6. Create `secrets.h` Locally

Copy `firmware/secrets.example.h` to `firmware/secrets.h` and fill in your own values locally.

The repository `.gitignore` excludes `firmware/secrets.h`.

## 7. Mobile Dashboard

The mobile dashboard can use the same datastreams. Add the main sensor/status widgets first, then add text/value widgets for V11 and V12 if your plan supports them.

## 8. OpenAI Bridge

The optional bridge is located at:

`integration/openai-blynk-bridge/`

Architecture:

```text
ESP32 -> Blynk -> V9 anomaly update -> Blynk Webhook -> Node.js bridge -> OpenAI API -> V11/V12
```

The bridge is intentionally separate from the ESP32 firmware. The core FYP remains based on threshold, trend and rule-based anomaly detection even if the AI bridge is not running.

Follow `integration/openai-blynk-bridge/README.md` for deployment and webhook configuration.

## 9. Firmware and Integration Test Sequence

1. Install the required Arduino libraries.
2. Compile and upload `firmware/mobile_substation_monitoring.ino`.
3. Confirm V0-V10 work in Serial Monitor and Blynk.
4. Deploy the OpenAI-Blynk bridge to an HTTPS Node.js host.
5. Configure the Blynk V9 webhook.
6. Trigger a controlled anomaly.
7. Confirm V9 changes to 1.
8. Confirm the bridge receives the webhook.
9. Confirm V11 receives an AI summary.
10. Confirm V12 receives a safe recommended laboratory check.
11. Record the ESP32 result separately from the AI advisory result.

## Important Notes

- Virtual Pins are Blynk software channels, not ESP32 physical GPIO pins.
- Do not expose Wi-Fi passwords, Blynk tokens, OpenAI API keys or webhook secrets in GitHub.
- The AI layer must not be used to operate protection, interlocking or switching functions.
- The prototype must not be connected directly to energized 33 kV or 11 kV equipment.
