# Blynk Dashboard

## Suggested Datastreams

| Virtual Pin | Parameter | Unit / Type |
|---|---|---|
| V0 | Temperature | °C |
| V1 | Relative Humidity | % |
| V2 | AC Voltage | V |
| V3 | AC Current | A |
| V4 | Overall Condition | String |
| V5 | Active Power | W |
| V6 | Frequency | Hz |
| V7 | Power Factor | Number |
| V8 | Trend / Anomaly Message | String |
| V9 | Trend Anomaly Flag | 0 or 1 |
| V10 | Condition Reason | String |

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
- LED indicator for anomaly flag (V9)
- Text display for condition reason (V10)
- SuperChart for historical temperature, humidity, voltage and current trends

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

## Setup Notes

1. Create a Blynk Template for the FYP prototype.
2. Create datastreams V0 to V10 using the table above.
3. Add dashboard widgets and map each widget to its matching virtual pin.
4. Copy `firmware/secrets.example.h` to `firmware/secrets.h` on your computer.
5. Add your Blynk Template ID, Template Name, Auth Token, Wi-Fi SSID and Wi-Fi password to `secrets.h`.
6. Never upload your real token or Wi-Fi password to this public repository.

Threshold and trend values should be adjusted using actual prototype test results and the operating requirements represented by the laboratory model.
