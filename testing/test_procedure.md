# Prototype Test Procedure

## Purpose

Verify sensor acquisition, Blynk transmission, threshold classification, trend-based anomaly detection, sensor-fault handling, and overall system response under controlled laboratory conditions.

## General Test Method

1. Power the ESP32 and all prototype modules using the intended laboratory supply arrangement.
2. Open the Arduino Serial Monitor at 115200 baud.
3. Confirm the ESP32 connects to Wi-Fi and Blynk.
4. Confirm DHT11 and PZEM-004T measurements are valid before starting each test.
5. Record the initial stable values for at least 30 seconds.
6. Apply only one controlled change at a time unless the test specifically requires multiple abnormal parameters.
7. Record the values displayed in both the Serial Monitor and Blynk dashboard.
8. Record the expected state, actual state, anomaly message, and pass/fail result in `test_results.csv`.

## Test Cases

### T01 — Normal Operating Condition

- Operate the prototype under normal room conditions and a normal laboratory load.
- Confirm all values are displayed in Blynk.
- Expected state: `NORMAL`.
- Expected anomaly flag: `0`.

### T02/T03 — High Temperature

- Increase the temperature around the DHT11 using a safe controlled heat source.
- Do not directly heat the ESP32 or PZEM module.
- Increase temperature gradually through the warning threshold and then, if safe for the prototype, through the critical threshold.
- Expected warning state: temperature ≥ 35 °C and < 40 °C.
- Expected critical state: temperature ≥ 40 °C.

### T04/T05 — High Humidity

- Increase humidity around the DHT11 in a controlled manner without allowing condensation or moisture to contact the electronics.
- Expected warning state: humidity ≥ 70 %RH and < 80 %RH.
- Expected critical state: humidity ≥ 80 %RH.

### T06–T09 — Voltage Threshold Tests

- Use only an appropriate safe laboratory AC source or arrangement supervised according to laboratory safety procedures.
- Verify the PZEM-004T voltage reading using a suitable reference instrument.
- Test the warning and critical low/high voltage regions defined in `thresholds.md` where the laboratory setup safely allows this.
- Do not create unsafe mains conditions merely to reach a test threshold. Simulation or controlled low-risk alternatives should be used when necessary.

### T10/T11 — Current Threshold Tests

- Use controlled loads within the rating of the laboratory setup, PZEM-004T, CT, conductors, and protection devices.
- Increase load current gradually.
- Compare the PZEM reading with a suitable reference instrument.
- Verify warning and critical classification according to the configured prototype threshold values.

### T12 — Temperature Trend Anomaly

- Start below the temperature warning threshold.
- Cause a controlled temperature rise of approximately 3 °C or more within the firmware history window.
- Expected result: `WARNING` may be produced by the trend rule even if the absolute temperature has not reached the warning threshold.
- Verify the anomaly message describes the temperature trend.

### T13 — Humidity Trend Anomaly

- Start below the humidity warning threshold.
- Cause an increase of approximately 10 %RH or more within the history window.
- Expected result: trend anomaly detected and `WARNING` generated.

### T14 — Voltage Change Anomaly

- Apply only a safe controlled voltage change permitted by the laboratory setup.
- Expected result: an absolute change of approximately 15 V or more across the trend window triggers a warning-level anomaly.

### T15 — Current Rise Anomaly

- Increase a controlled load so current rises approximately 1.5 A or more within the history window while remaining within safe equipment ratings.
- Expected result: warning-level current trend anomaly.

### T16 — DHT11 Read Failure

- Simulate a DHT11 read/communication failure using a safe low-voltage method, such as disconnecting the DHT11 signal while the prototype is de-energized and then restarting as appropriate.
- Expected result: sensor fault identified and the system reports the configured abnormal state/reason.

### T17 — PZEM Read Failure

- Simulate loss of communication between ESP32 and the PZEM module using a safe low-voltage UART-side method where possible.
- Expected result: PZEM communication/read failure is identified.

### T18 — Multiple Abnormal Parameters

- Simulate two or more abnormal parameters under controlled laboratory conditions.
- Verify the overall condition follows the highest detected severity.
- Expected state: `CRITICAL` when at least one critical rule is active.

### T19 — Wi-Fi Interruption and Reconnection

- Temporarily disable the Wi-Fi connection or access point.
- Observe ESP32/Blynk behaviour.
- Restore Wi-Fi.
- Confirm the system reconnects and dashboard updates resume.

## Repeatability

Where practical, repeat each principal test at least three times. Record every run rather than only the best result. This allows the final report to discuss repeatability and consistency.

## Reference Measurement

For electrical tests, record the PZEM-004T value and a suitable reference instrument value where available. Calculate measurement error using:

`Error (%) = |Prototype Reading - Reference Reading| / Reference Reading × 100`

Do not claim calibration unless a proper calibration method and traceable reference have been used. The comparison can instead be described as a measurement verification or accuracy check.

## Data Recording

Use `test_results.csv` for raw results. At minimum, record:

- date/time;
- test case;
- temperature;
- humidity;
- voltage;
- current;
- power;
- frequency;
- power factor;
- anomaly flag/message;
- expected state;
- actual state;
- pass/fail;
- remarks.

## Pass Criteria

A test passes when:

1. valid sensor values are acquired when the sensor is operating normally;
2. Blynk receives the intended monitored values;
3. the actual condition matches the expected threshold/trend rule;
4. the anomaly message correctly identifies the relevant abnormal behaviour; and
5. the prototype remains stable without unintended reset or unsafe operation.

## Safety Note

The project is a laboratory-scale monitoring prototype. Do not connect the prototype directly to energized 33 kV or 11 kV equipment. All mains-voltage work must follow laboratory rules, suitable isolation/protection practices, equipment ratings, and competent supervision.
