# Prototype Thresholds and Anomaly Rules

Demonstration values only. Validate against collected baseline data and equipment requirements before adopting final FYP thresholds. These are not utility protection settings.

| Parameter | Normal threshold region | Warning | Critical |
|---|---|---|---|
| Temperature | <35 °C | ≥35 and <40 °C | ≥40 °C |
| Humidity | <75 % | ≥75 and <85 % | ≥85 % |
| AC voltage | >210 and <250 V | >200 to ≤210 V or ≥250 to <260 V | ≤200 or ≥260 V |
| AC current | <3 A | ≥3 and <5 A | ≥5 A |

Thresholds are inclusive at warning/critical boundaries. Invalid measurements are excluded.

## Four-reading trend persistence

At a nominal two-second interval, four readings span approximately six seconds. Every adjacent reading must strictly increase (or strictly decrease for falling voltage), and total change must meet:

- Temperature rise: ≥1.5 °C
- Humidity rise: ≥5 %
- Voltage rise or fall: ≥5 V
- Current rise: ≥0.3 A

A plateau or reversal fails the trend rule. Invalid sensor readings reset that sensor's histories; the healthy sensor continues independently. All simultaneous trends appear in V9. STABLE means no confirmed trend, including during the initial four-reading collection period.

## Severity and multiple anomalies

Each abnormal parameter is counted once in V11, whether flagged by threshold, trend, or both. Trend-only anomalies count. V11 ranges 0–4 and excludes sensor-fault counts.

1. Any sensor fault -> SYSTEM FAULT (3), while retaining causes/counts from healthy sensors.
2. Any critical threshold, or at least two abnormal parameters -> CRITICAL (2).
3. One abnormal parameter -> WARNING (1).
4. No abnormal parameters or sensor faults -> NORMAL (0), subject to recovery.

Abnormal classifications update immediately. Returning to NORMAL requires three consecutive normal evaluation cycles. Any new abnormality resets that count. During recovery, V4/V10 retain the previous alarm, V5 explicitly shows recovery progress, V6 requests continued monitoring, and V11 reports current abnormalities (zero). Other abnormal-state transitions are immediate.

Implementation: [monitoring_logic.h](../firmware/monitoring_logic.h).
