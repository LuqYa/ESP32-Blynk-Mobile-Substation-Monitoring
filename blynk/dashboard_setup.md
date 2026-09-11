# Blynk Dashboard Setup

Use this guide with the firmware in `firmware/mobile_substation_monitoring.ino`.

## Template

Create a Blynk Template named:

`Mobile Substation Monitoring`

Select ESP32 as the hardware and Wi-Fi as the connection type.

## Datastreams

Create the following Virtual Pin datastreams.

| Pin | Name | Data Type | Unit | Suggested Widget |
|---|---|---|---|---|
| V0 | Temperature | Double | °C | Gauge + Chart |
| V1 | Humidity | Double | %RH | Gauge + Chart |
| V2 | Voltage | Double | V | Gauge + Chart |
| V3 | Current | Double | A | Gauge + Chart |
| V4 | Overall Condition | String | - | Label / Value Display |
| V5 | Power | Double | W | Value Display |
| V6 | Frequency | Double | Hz | Value Display |
| V7 | Power Factor | Double | - | Value Display |
| V8 | Anomaly Message | String | - | Label / Value Display |
| V9 | Anomaly Flag | Integer | 0/1 | LED / Indicator |
| V10 | Condition Reason | String | - | Label / Value Display |

## Suggested Dashboard Layout

Top section:
- Overall Condition (V4)
- Anomaly Indicator (V9)
- Anomaly Message (V8)

Environmental section:
- Temperature gauge (V0)
- Humidity gauge (V1)
- SuperChart with V0 and V1

Electrical section:
- Voltage gauge (V2)
- Current gauge (V3)
- Power display (V5)
- Frequency display (V6)
- Power factor display (V7)
- SuperChart with V2 and V3

Diagnostic section:
- Condition Reason (V10)

## Recommended Display Ranges

These ranges are for the laboratory prototype display only:

- Temperature: 0–60 °C
- Humidity: 0–100 %RH
- Voltage: 180–270 V
- Current: 0–10 A
- Power: choose a range suitable for the laboratory load
- Frequency: 45–55 Hz
- Power factor: 0–1

## Condition Meaning

- `NORMAL`: all valid measurements are within the selected normal range and no rapid abnormal trend is detected.
- `WARNING`: one or more values reach a warning threshold or a rapid trend is detected.
- `CRITICAL`: a critical threshold is exceeded or a critical sensor/read condition is detected.

## Credential Setup

Copy `firmware/secrets.example.h` to `firmware/secrets.h` locally and insert the Blynk Template ID, Template Name, Auth Token, Wi-Fi SSID and Wi-Fi password.

Never upload `secrets.h` to GitHub.

## First Dashboard Test

1. Upload the firmware to the ESP32.
2. Open Serial Monitor at 115200 baud.
3. Confirm ESP32 connects to Wi-Fi/Blynk.
4. Confirm V0 and V1 update from the DHT11.
5. Confirm V2–V7 update after the PZEM-004T is connected correctly.
6. Confirm V4 shows the operating state.
7. Trigger a controlled abnormal condition and verify V8/V9/V10 change.
8. Compare dashboard data with the Serial Monitor before recording formal test results.
