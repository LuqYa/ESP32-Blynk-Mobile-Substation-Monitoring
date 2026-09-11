# Blynk Dashboard Setup

This guide matches `firmware/mobile_substation_monitoring.ino` and the direct Render bridge integration.

## 1. Create the Device Template

In Blynk Console, enable Developer Mode and create a template with:

- Template Name: `Mobile Substation Monitoring`
- Hardware: `ESP32`
- Connection Type: `WiFi`

Keep the generated Template ID local for `firmware/secrets.h`.

## 2. Create Virtual Pin Datastreams

Create these Virtual Pin datastreams exactly:

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
| V11 | Advisory Summary | String | - | - | - |
| V12 | Recommended Check | String | - | - | - |

V0-V10 are written directly by the ESP32 monitoring logic.
V11-V12 are written by the ESP32 after it requests an advisory from the Render bridge.

## 3. Recommended Dashboard Layout

### System Status

- Overall Condition -> V4
- Trend Anomaly Indicator -> V9
- Anomaly Message -> V8
- Condition Reason -> V10

### Environmental Monitoring

- Temperature -> V0
- Humidity -> V1
- SuperChart -> V0 and V1

### Electrical Monitoring

- Voltage -> V2
- Current -> V3
- Power -> V5
- Frequency -> V6
- Power Factor -> V7
- SuperChart -> V2 and V3

### Advisory Section

- Advisory Summary -> V11
- Recommended Check -> V12

Keep V11/V12 visually separate from V4/V8/V9/V10. The core condition classification is still performed locally by the ESP32 threshold/trend/rule-based logic.

## 4. Create the Device

Create a device from the template:

- Template: `Mobile Substation Monitoring`
- Device Name: `FYP Mobile Substation Prototype`

Open Device Info and copy the Auth Token only to your local `firmware/secrets.h`.

Never commit the Auth Token to GitHub.

## 5. Default Integration - No Blynk Webhook Required

The default integration is:

```text
ESP32 -> Blynk V0-V10
   |
   +-> Render /esp32-analyse when an abnormal condition is active
           |
           v
      SUMMARY / ACTION
           |
           v
       ESP32 -> Blynk V11/V12
```

Render endpoint:

`https://openai-blynk-bridge.onrender.com/esp32-analyse`

The firmware already contains this public URL.

Render advisory may be requested when the ESP32 detects:

- threshold WARNING
- threshold CRITICAL
- DHT11 or PZEM read/communication fault
- trend anomaly

V9 remains specifically the trend-anomaly flag. A threshold WARNING or CRITICAL can therefore request an advisory even when V9 remains `0`.

For the normal FYP demonstration you do NOT need:

- a Blynk webhook
- a Blynk Device Token stored on Render

## 6. Optional AI Advisory

The Render bridge uses rule-based advisory output by default.

If OpenAI advisory is enabled later, keep the OpenAI API key in Render only and use a private shared secret between the ESP32 and Render. Do not place the OpenAI API key in ESP32 firmware or Blynk.

## 7. Test Sequence

1. Compile and upload the latest firmware.
2. Confirm ESP32 connects to Wi-Fi and Blynk.
3. Confirm V0-V10 update.
4. Confirm the prototype reaches a stable NORMAL condition.
5. Run a controlled threshold WARNING test, for example temperature above 35 °C, and confirm V4 becomes `WARNING`.
6. Confirm Serial Monitor shows the Render response and V11/V12 update, even if V9 remains `0`.
7. Run a controlled trend-anomaly test after the history buffer has enough samples and confirm V9 becomes `1`.
8. Confirm V11 shows the advisory summary and V12 shows the recommended laboratory check.
9. Run a controlled CRITICAL test only within safe laboratory limits and confirm V4 becomes `CRITICAL` and V11/V12 update.
10. Return the prototype to a normal condition and confirm the advisory status resets.
11. Record only real measurements and observed results in `testing/test_results.csv`.

## 8. Automated Verification Already Completed

GitHub Actions has verified the live Render bridge for:

- health endpoint
- temperature warning
- humidity warning
- voltage warning
- current warning
- sensor fault
- critical condition

The cloud tests verify that Render returns the exact response contract expected by the ESP32:

```text
SUMMARY:<short interpretation>
ACTION:<safe laboratory check>
MODE:RULE
```

## Important Notes

- Virtual Pins are Blynk software channels, not ESP32 GPIO pins.
- The PZEM-004T values are AC measurements; do not describe them as direct DC battery voltage/current measurements.
- Prototype thresholds are laboratory values and require baseline-test justification.
- Do not connect this prototype directly to energized 33 kV or 11 kV equipment.
- The Render/OpenAI advisory layer is not a protection relay, interlock, switching controller, or automatic operating authority.
