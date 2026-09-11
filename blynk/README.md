# Blynk Dashboard

## Datastreams

Create these Virtual Pin datastreams in the Blynk template:

| Virtual Pin | Parameter | Unit / Type | Written By |
|---|---|---|---|
| V0 | Temperature | °C | ESP32 |
| V1 | Relative Humidity | %RH | ESP32 |
| V2 | AC Voltage | V | ESP32 |
| V3 | AC Current | A | ESP32 |
| V4 | Overall Condition | String | ESP32 |
| V5 | Active Power | W | ESP32 |
| V6 | Frequency | Hz | ESP32 |
| V7 | Power Factor | Number | ESP32 |
| V8 | Trend / Anomaly Message | String | ESP32 |
| V9 | Trend Anomaly Flag | 0 or 1 | ESP32 |
| V10 | Condition Reason | String | ESP32 |
| V11 | Advisory Summary | String | ESP32 after Render response |
| V12 | Recommended Check | String | ESP32 after Render response |

## Suggested Dashboard Widgets

- Gauge or value display for temperature (V0)
- Gauge or value display for humidity (V1)
- Gauge for AC voltage (V2)
- Gauge for AC current (V3)
- Label/value display for overall condition (V4)
- Value display for active power (V5)
- Value display for frequency (V6)
- Value display for power factor (V7)
- Text display for trend/anomaly information (V8)
- LED indicator for trend anomaly flag (V9)
- Text display for condition reason (V10)
- Text display for advisory summary (V11)
- Text display for recommended laboratory check (V12)
- SuperChart for historical temperature, humidity, voltage and current trends

Keep V11 and V12 visually separate from V4, V8, V9 and V10. The ESP32 remains responsible for the core condition classification.

## Condition States

- `NORMAL` — measurements are within the selected acceptable range and no abnormal trend is detected.
- `WARNING` — one or more parameters reach a warning threshold, a sensor reading fails, or an abnormal trend is detected.
- `CRITICAL` — one or more parameters exceed a selected critical threshold.

## Current Prototype Thresholds

These are initial laboratory values in the firmware and must be reviewed after baseline testing.

| Parameter | Warning | Critical |
|---|---|---|
| Temperature | >= 35 °C | >= 40 °C |
| Humidity | >= 70 %RH | >= 80 %RH |
| AC Voltage | <= 210 V or >= 250 V | <= 200 V or >= 260 V |
| AC Current | >= 4 A | >= 5 A |

The voltage and current values are prototype settings, not protection settings for an actual substation.

## Trend Detection

The firmware stores a short history of measurements and compares the newest reading with an older reading. Current example anomaly rules are:

- Temperature rise >= 3 °C within the history window
- Humidity rise >= 10 %RH within the history window
- Voltage change >= 15 V within the history window
- Current increase >= 1.5 A within the history window

With six samples and a two-second sampling interval, the comparison represents approximately ten seconds between the oldest and newest usable samples.

## Direct Render Advisory Integration

The default architecture does not require a Blynk webhook.

```text
ESP32
  -> Blynk V0-V10
  -> Render /esp32-analyse when an abnormal condition is active
  -> Render returns SUMMARY / ACTION / MODE
  -> ESP32 writes SUMMARY to V11 and ACTION to V12
```

Render advisory can be requested for:

- threshold WARNING
- threshold CRITICAL
- DHT11 or PZEM sensor/read fault
- trend anomaly

V9 remains specifically the trend-anomaly flag. A threshold WARNING or CRITICAL may therefore request a Render advisory even when V9 is `0`.

## Setup Notes

1. Create a Blynk Template for the FYP prototype.
2. Create datastreams V0 to V12 using the table above.
3. Add dashboard widgets and map each widget to its matching virtual pin.
4. Create a device from the template.
5. Copy `firmware/secrets.example.h` to `firmware/secrets.h` on your computer.
6. Add your Blynk Template ID, Template Name, Auth Token, Wi-Fi SSID and Wi-Fi password to `secrets.h`.
7. Open `firmware/firmware.ino` in Arduino IDE, select the actual ESP32 board and COM port, and upload.
8. Confirm V0-V10 update first, then run a controlled abnormal-condition test and confirm V11/V12 update after the Render response.
9. Never upload your real token or Wi-Fi password to this public repository.

## Verified Cloud Tests

The live Render bridge is tested automatically from GitHub Actions for:

- health endpoint
- temperature warning
- humidity warning
- voltage warning
- current warning
- sensor fault
- critical condition

These tests verify the response contract used by the ESP32 firmware:

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
