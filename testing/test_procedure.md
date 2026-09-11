# Prototype Test Procedure

## Purpose

Verify sensor acquisition, Blynk transmission, condition classification, and alarm behaviour under controlled laboratory conditions.

## Test Cases

1. **Normal temperature and humidity**
   - Operate the prototype under normal room conditions.
   - Confirm temperature and humidity are displayed in Blynk.
   - Expected condition: NORMAL.

2. **High temperature**
   - Increase the temperature around the DHT11 using a safe controlled heat source.
   - Observe the measured value and condition state.
   - Expected condition: WARNING or CRITICAL depending on the configured threshold.

3. **High humidity**
   - Increase humidity in a controlled and safe manner without wetting the electronics.
   - Observe the Blynk reading and condition state.
   - Expected condition: WARNING or CRITICAL depending on the configured threshold.

4. **Voltage and current monitoring**
   - Connect the PZEM-004T only through an appropriate safe laboratory arrangement and according to the module documentation.
   - Record voltage and current readings at several controlled load conditions.
   - Compare readings with a suitable reference instrument.

5. **Communication interruption**
   - Temporarily disconnect Wi-Fi.
   - Observe ESP32/Blynk behaviour and reconnection.

6. **Multiple abnormal parameters**
   - Simulate more than one abnormal parameter.
   - Verify the overall condition prioritises the most severe state.

## Data to Record

| Test | Temperature | Humidity | Voltage | Current | Expected State | Actual State | Remarks |
|---|---:|---:|---:|---:|---|---|---|
| Normal | | | | | NORMAL | | |
| High temperature | | | | | WARNING/CRITICAL | | |
| High humidity | | | | | WARNING/CRITICAL | | |
| Electrical load test | | | | | As configured | | |
| Multiple abnormal | | | | | CRITICAL | | |

## Note

Final threshold values should be justified using prototype baseline data, relevant equipment requirements, and the intended FYP scope rather than relying only on the placeholder values in the starter firmware.
