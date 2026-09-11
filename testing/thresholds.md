# Prototype Thresholds and Anomaly Rules

These values are initial laboratory settings for the FYP prototype. They are not utility protection settings and must not be treated as operating limits for energized 33/11 kV equipment.

## Threshold Classification

| Parameter | Normal | Warning | Critical |
|---|---|---|---|
| Temperature | < 35 °C | 35 to < 40 °C | ≥ 40 °C |
| Relative humidity | < 70 %RH | 70 to < 80 %RH | ≥ 80 %RH |
| AC voltage | > 210 V and < 250 V | 200–210 V or 250–260 V | ≤ 200 V or ≥ 260 V |
| AC current | < 4 A | 4 to < 5 A | ≥ 5 A |

## Trend-Based Anomaly Rules

The ESP32 stores six samples. With a 2-second sampling interval, the oldest and newest usable samples provide an approximately 10-second comparison window.

A WARNING-level anomaly is generated when one of the following is detected:

- Temperature rise ≥ 3 °C over the history window.
- Relative humidity rise ≥ 10 %RH over the history window.
- Absolute voltage change ≥ 15 V over the history window.
- Current rise ≥ 1.5 A over the history window.

## Rule-Based Anomaly Detection

The system combines three types of checks:

1. **Threshold checking** — compares the latest value against predefined warning and critical ranges.
2. **Trend checking** — compares recent samples to detect rapid change before a critical threshold is reached.
3. **Sensor validity checking** — marks invalid DHT11 or PZEM measurements as an abnormal condition.

The final operating state follows the highest severity detected: `NORMAL < WARNING < CRITICAL`.

## Calibration of Final Values

The final FYP report should distinguish between:

- initial prototype thresholds used to demonstrate system operation;
- measured baseline values obtained during laboratory tests; and
- any final threshold values adopted after testing.

Any final values should be justified using collected data, sensor capability, prototype operating conditions, and applicable equipment requirements.
