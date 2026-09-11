# Data Collection Plan

## Purpose

Collect repeatable laboratory data to evaluate sensor acquisition, Blynk transmission, threshold classification, trend-based anomaly detection, sensor-fault handling, and Render advisory response.

## Sampling

- Firmware sampling interval: approximately 2 seconds.
- Record stable baseline data before abnormal-condition tests.
- Repeat important tests at least three times where practical.
- Use the `run_no` field in `testing/test_results.csv` to distinguish repetitions.
- Record reference-instrument readings for electrical measurements when available.
- Do not replace missing measurements with assumed values.

## Baseline Test

Operate the prototype under normal laboratory conditions and record:

- Temperature (°C)
- Relative humidity (%RH)
- AC voltage (V)
- AC current (A)
- Power (W)
- Frequency (Hz)
- Power factor
- Overall condition (V4)
- Trend message (V8)
- Trend anomaly flag (V9)
- Condition reason (V10)
- Advisory summary (V11), when applicable
- Recommended check (V12), when applicable

The baseline should be used to check whether the initial thresholds are reasonable before finalising them.

## Controlled Test Scenarios

| Scenario | Controlled Change | Expected System Response |
|---|---|---|
| Normal | Stable room/environmental and electrical conditions | V4 = NORMAL, V9 = 0 |
| Temperature warning | Raise temperature into warning region and allow trend to stabilise | V4 = WARNING, V9 = 0 for threshold-only record |
| Temperature critical | Raise temperature into critical region and allow trend to stabilise | V4 = CRITICAL, V9 = 0 for threshold-only record |
| Humidity warning | Raise humidity into warning region and allow trend to stabilise | V4 = WARNING, V9 = 0 for threshold-only record |
| Humidity critical | Raise humidity into critical region and allow trend to stabilise | V4 = CRITICAL, V9 = 0 for threshold-only record |
| Voltage warning | Adjust safe laboratory voltage condition into warning region | V4 = WARNING, V9 = 0 after stabilisation |
| Voltage critical | Adjust safe laboratory voltage condition into critical region | V4 = CRITICAL, V9 = 0 after stabilisation |
| Current warning | Increase laboratory load into warning region | V4 = WARNING, V9 = 0 after stabilisation |
| Current critical | Increase laboratory load into critical region | V4 = CRITICAL, V9 = 0 after stabilisation |
| Rapid temperature rise | Increase temperature by approximately 3 °C or more within trend window | V9 = 1, V8 = Rapid temperature rise, V4 ≥ WARNING |
| Rapid humidity rise | Increase humidity by approximately 10 %RH or more within trend window | V9 = 1, V8 = Rapid humidity rise, V4 ≥ WARNING |
| Rapid voltage change | Change voltage by approximately 15 V or more within trend window where safely possible | V9 = 1, V8 = Rapid voltage change, V4 ≥ WARNING |
| Rapid current rise | Increase current by approximately 1.5 A or more within trend window | V9 = 1, V8 = Rapid current increase, V4 ≥ WARNING |
| DHT11 sensor fault | Safely interrupt DHT11 communication | V4 = WARNING, V9 normally 0, V10 identifies DHT11 fault |
| PZEM sensor fault | Safely interrupt UART/PZEM communication | V4 = WARNING, V9 normally 0, V10 identifies PZEM fault |
| Multiple abnormal parameters | Combine two or more controlled threshold conditions | Highest severity retained; CRITICAL if any critical rule is active |
| Wi-Fi interruption | Temporarily disable network | Local ESP32 monitoring continues; Blynk updates resume after reconnect |

## Advisory Evidence

For every abnormal test where Wi-Fi/Render are available, record:

- V11 Advisory Summary;
- V12 Recommended Check;
- advisory mode reported in Serial Monitor (`RULE` or `AI` when applicable);
- whether the advisory matched the parameter or fault identified by V10.

The advisory output is supporting information only. The ESP32 remains responsible for threshold/trend/rule-based condition classification.

## Electrical Measurement Verification

Where a suitable reference instrument is available, record both prototype and reference values for AC voltage/current.

Calculate:

`Voltage Error (%) = |PZEM Voltage - Reference Voltage| / Reference Voltage × 100`

`Current Error (%) = |PZEM Current - Reference Current| / Reference Current × 100`

Do not describe this as calibration unless a proper calibration procedure and traceable reference are used.

## Data Recording

Use `testing/test_results.csv` for every test run. Record actual measured/observed values only. If a value is unavailable, leave it blank and explain why in `remarks` rather than inventing a value.

## Evaluation

Evaluate:

1. Whether sensor values update correctly.
2. Whether Blynk V0–V10 receive the same intended values/state information shown in Serial Monitor.
3. Whether threshold transitions occur at the intended prototype values.
4. Whether V9/V8 identify rapid changes only during trend anomalies.
5. Whether sensor faults produce the intended WARNING state and V10 reason.
6. Whether the most severe abnormal condition determines V4.
7. Whether Render responds to abnormal conditions and ESP32 writes V11/V12.
8. Whether electrical measurement error is acceptable for the intended laboratory demonstration.
9. Whether the system recovers after temporary communication interruption.

## Threshold Revision

After baseline and controlled testing, revise the placeholder thresholds only when supported by measured data, equipment requirements, or the selected laboratory test range. Document every final threshold and its justification in `testing/thresholds.md`.
