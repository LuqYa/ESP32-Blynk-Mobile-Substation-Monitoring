# Data Collection Plan

## Purpose

Collect repeatable laboratory data to evaluate sensor acquisition, Blynk transmission, threshold classification, and rule-based anomaly detection.

## Sampling

- Firmware sampling interval: approximately 2 seconds.
- Record stable baseline data before abnormal-condition tests.
- Repeat important tests at least three times where practical.
- Record reference-instrument readings for electrical measurements when available.

## Baseline Test

Operate the prototype under normal laboratory conditions and record:

- Temperature (°C)
- Relative humidity (%RH)
- AC voltage (V)
- AC current (A)
- Power (W)
- Frequency (Hz)
- Power factor
- Overall condition
- Anomaly flag/message

The baseline should be used to check whether the initial thresholds are reasonable before finalising them.

## Controlled Test Scenarios

| Scenario | Controlled Change | Expected System Response |
|---|---|---|
| Normal | Stable room/environmental and electrical conditions | NORMAL |
| Temperature warning | Raise temperature above warning limit | WARNING |
| Temperature critical | Raise temperature above critical limit | CRITICAL |
| Humidity warning | Raise humidity above warning limit | WARNING |
| Humidity critical | Raise humidity above critical limit | CRITICAL |
| Voltage warning | Adjust safe laboratory voltage condition into warning region | WARNING |
| Voltage critical | Adjust safe laboratory voltage condition into critical region | CRITICAL |
| Current warning | Increase laboratory load into warning region | WARNING |
| Current critical | Increase laboratory load into critical region | CRITICAL |
| Rapid temperature rise | Increase temperature fast enough to trigger trend rule | Anomaly detected |
| Rapid humidity rise | Increase humidity fast enough to trigger trend rule | Anomaly detected |
| Rapid current rise | Change load quickly | Anomaly detected |
| Sensor fault | Disconnect/interrupt one sensor safely | Sensor fault/anomaly indication |
| Multiple abnormal parameters | Combine two or more abnormal conditions | Highest severity retained |
| Wi-Fi interruption | Temporarily disable network | Local ESP32 continues; Blynk reconnects when network returns |

## Data Recording

Use `testing/test_results.csv` for each test run. Do not replace measurements with assumed values. Record the actual measured result and add remarks when the observed state differs from the expected state.

## Evaluation

Evaluate:

1. Whether sensor values update correctly.
2. Whether Blynk receives the same values shown in Serial Monitor.
3. Whether threshold transitions occur at the intended values.
4. Whether trend rules identify rapid changes.
5. Whether the most severe abnormal condition determines the overall state.
6. Whether the system recovers after temporary communication interruption.

## Threshold Revision

After baseline and controlled testing, revise the placeholder thresholds only when supported by measured data, equipment requirements, or the selected laboratory test range. Document every final threshold and its justification in `testing/thresholds.md`.
