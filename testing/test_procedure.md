# Prototype Test Procedure

## Purpose

Verify sensor acquisition, Blynk transmission, threshold classification, trend-based anomaly detection, sensor-fault handling, Render advisory response, and overall system behaviour under controlled laboratory conditions.

## General Test Method

1. Power the ESP32 and all prototype modules using the intended laboratory supply arrangement.
2. Open the Arduino Serial Monitor at 115200 baud.
3. Confirm the ESP32 connects to Wi-Fi and Blynk.
4. Confirm DHT11 and PZEM-004T measurements are valid before starting each test.
5. Record the initial stable values for at least 30 seconds.
6. Apply only one controlled change at a time unless the test specifically requires multiple abnormal parameters.
7. Record the values displayed in both the Serial Monitor and Blynk dashboard.
8. Record the expected state, actual state, trend-anomaly flag/message, condition reason, V11/V12 advisory output, and pass/fail result in `test_results.csv`.

Important distinction:

- V9 is specifically the **trend-anomaly flag**.
- Threshold WARNING/CRITICAL conditions and sensor faults can request a Render advisory even when V9 remains `0`.
- V10 carries the ESP32 condition reason.
- V11 carries the advisory summary returned through Render.
- V12 carries the recommended safe laboratory check.

## Test Cases

### T01 — Normal Operating Condition

- Operate the prototype under normal room conditions and a normal laboratory load.
- Confirm all values are displayed in Blynk.
- Expected state: `NORMAL`.
- Expected V9 trend-anomaly flag: `0`.
- V11/V12 may retain/reset to the normal monitoring message after recovery from a previous abnormal event.

### T02/T03 — High Temperature Threshold

- Increase the temperature around the DHT11 using a safe controlled heat source.
- Do not directly heat the ESP32 or PZEM module.
- Increase temperature gradually through the warning threshold and then, if safe for the prototype, through the critical threshold.
- To evaluate the threshold rule independently from the trend rule, make the change gradually or hold the final temperature stable until the short trend window no longer reports a rapid rise before recording the threshold-only result.
- Expected warning state: temperature ≥ 35 °C and < 40 °C.
- Expected critical state: temperature ≥ 40 °C.
- Expected V9 for a threshold-only observation: `0`.
- Confirm V10 reports the temperature threshold reason and V11/V12 receive an advisory for the abnormal state.

### T04/T05 — High Humidity Threshold

- Increase humidity around the DHT11 in a controlled manner without allowing condensation or moisture to contact the electronics.
- To evaluate the threshold rule independently, hold the final condition stable before recording if a rapid humidity trend was triggered during the transition.
- Expected warning state: humidity ≥ 70 %RH and < 80 %RH.
- Expected critical state: humidity ≥ 80 %RH.
- Expected V9 for a threshold-only observation: `0`.
- Confirm V10 identifies the humidity threshold condition and V11/V12 update.

### T06–T09 — Voltage Threshold Tests

- Use only an appropriate safe laboratory AC source or arrangement supervised according to laboratory safety procedures.
- Verify the PZEM-004T voltage reading using a suitable reference instrument.
- Test the warning and critical low/high voltage regions defined in `thresholds.md` where the laboratory setup safely allows this.
- Do not create unsafe mains conditions merely to reach a test threshold. Simulation or controlled low-risk alternatives should be used when necessary.
- Where a rapid voltage-change trend is triggered while moving to the target value, wait until the target condition is stable before recording the threshold-only result.
- Expected V9 for the final threshold-only observation: `0`.
- Record reference voltage and calculate percentage error where a suitable reference instrument is available.

### T10/T11 — Current Threshold Tests

- Use controlled loads within the rating of the laboratory setup, PZEM-004T, CT, conductors, and protection devices.
- Increase load current gradually.
- Compare the PZEM reading with a suitable reference instrument.
- Verify warning and critical classification according to the configured prototype threshold values.
- Where a rapid current-rise trend is triggered during the transition, hold the final current stable before recording the threshold-only result.
- Expected V9 for the final threshold-only observation: `0`.
- Record reference current and percentage error where a suitable reference instrument is available.

### T12 — Temperature Trend Anomaly

- Start below the temperature warning threshold.
- Cause a controlled temperature rise of approximately 3 °C or more within the firmware history window.
- Expected V9: `1`.
- Expected V8 message: `Rapid temperature rise`.
- Expected result: `WARNING` may be produced by the trend rule even if the absolute temperature has not reached the warning threshold.
- Confirm V11/V12 receive the Render advisory.

### T13 — Humidity Trend Anomaly

- Start below the humidity warning threshold.
- Cause an increase of approximately 10 %RH or more within the history window.
- Expected V9: `1`.
- Expected V8 message: `Rapid humidity rise`.
- Expected state: `WARNING`.

### T14 — Voltage Change Anomaly

- Apply only a safe controlled voltage change permitted by the laboratory setup.
- Expected result: an absolute change of approximately 15 V or more across the trend window triggers a warning-level anomaly.
- Expected V9: `1`.
- Expected V8 message: `Rapid voltage change`.

### T15 — Current Rise Anomaly

- Increase a controlled load so current rises approximately 1.5 A or more within the history window while remaining within safe equipment ratings.
- Expected result: warning-level current trend anomaly.
- Expected V9: `1`.
- Expected V8 message: `Rapid current increase`.

### T16 — DHT11 Read Failure

- Simulate a DHT11 read/communication failure using a safe low-voltage method, such as disconnecting the DHT11 signal while the prototype is de-energized and then restarting as appropriate.
- Expected state: `WARNING`.
- Expected V9: `0` unless an unrelated trend anomaly is also active.
- Expected V10 reason: `DHT11 communication/read failure`.
- Confirm V11/V12 receive an advisory for the sensor fault.

### T17 — PZEM Read Failure

- Simulate loss of communication between ESP32 and the PZEM module using a safe low-voltage UART-side method where possible.
- Expected state: `WARNING`.
- Expected V9: `0` unless an unrelated trend anomaly is also active.
- Expected V10 reason: `PZEM-004T communication/read failure`.
- Confirm V11/V12 receive an advisory for the sensor fault.

### T18 — Multiple Abnormal Parameters

- Simulate two or more abnormal threshold parameters under controlled laboratory conditions.
- Verify the overall condition follows the highest detected severity.
- Expected state: `CRITICAL` when at least one critical rule is active.
- For the threshold-only version of this test, allow the values to stabilise before recording so V9 can remain `0`.
- Verify V10 preserves the relevant highest-severity reason(s) and V11/V12 update.

### T19 — Wi-Fi Interruption and Reconnection

- Temporarily disable the Wi-Fi connection or access point.
- Observe ESP32/Blynk behaviour.
- Restore Wi-Fi.
- Confirm the system reconnects and dashboard updates resume.
- Do not fabricate missing cloud data during the outage; record exactly what was observed before, during, and after reconnection.

## Repeatability

Where practical, repeat each principal test at least three times. Record every run rather than only the best result. Use the `run_no` column in `test_results.csv`. This allows the final report to discuss repeatability and consistency.

## Reference Measurement

For electrical tests, record the PZEM-004T value and a suitable reference instrument value where available. Calculate measurement error using:

`Error (%) = |Prototype Reading - Reference Reading| / Reference Reading × 100`

Do not claim calibration unless a proper calibration method and traceable reference have been used. The comparison can instead be described as a measurement verification or accuracy check.

## Data Recording

Use `test_results.csv` for raw results. At minimum, record:

- date/time and run number;
- test case;
- temperature;
- humidity;
- voltage;
- current;
- power;
- frequency;
- power factor;
- reference voltage/current and error percentage where available;
- V9 trend-anomaly flag;
- V8 trend message;
- expected and actual condition state;
- V10 condition reason;
- V11 advisory summary;
- V12 recommended check;
- advisory mode shown in Serial Monitor (`RULE` or `AI` when applicable);
- confirmation that Blynk V0–V10 and V11/V12 updated;
- pass/fail;
- remarks.

## Pass Criteria

A test passes when:

1. valid sensor values are acquired when the sensor is operating normally;
2. Blynk receives the intended monitored values;
3. the actual condition matches the expected threshold, sensor-validity, or trend rule;
4. V9/V8 match the expected trend behaviour for trend tests;
5. V10 correctly identifies the active threshold/sensor/trend condition;
6. an abnormal state requests a Render advisory and V11/V12 update where network connectivity is available; and
7. the prototype remains stable without unintended reset or unsafe operation.

## Safety Note

The project is a laboratory-scale monitoring prototype. Do not connect the prototype directly to energized 33 kV or 11 kV equipment. All mains-voltage work must follow laboratory rules, suitable isolation/protection practices, equipment ratings, and competent supervision.
