# Direct ESP32 -> Render -> Blynk Integration Test

This procedure verifies the default integration without using a Blynk webhook or a Blynk Device Token on Render.

## Required Blynk Datastreams

- V0 Temperature
- V1 Humidity
- V2 AC Voltage
- V3 AC Current
- V4 Overall Condition
- V5 Power
- V6 Frequency
- V7 Power Factor
- V8 Trend / Anomaly Message
- V9 Trend Anomaly Flag
- V10 Condition Reason
- V11 Advisory Summary
- V12 Recommended Check

## Before Testing

1. Open `firmware/firmware.ino` in Arduino IDE.
2. Confirm `firmware/secrets.h` contains your own Wi-Fi and Blynk values.
3. Select the correct ESP32 board and COM port.
4. Compile and upload the firmware.
5. Open Serial Monitor at 115200 baud.
6. Confirm the ESP32 connects to Wi-Fi and Blynk.
7. Confirm V0-V10 begin updating.

## Test 1 - Normal Condition

Use stable laboratory conditions below the selected prototype warning thresholds.

Expected:

- V4 = NORMAL
- V9 = 0 unless a rapid trend is intentionally produced
- V11/V12 may remain at the latest normal/recovery message
- Serial Monitor shows the measured values, condition and reason

## Test 2 - Temperature Warning

Increase the controlled laboratory temperature above the prototype warning threshold while remaining below the critical threshold.

Expected:

- V4 = WARNING
- V10 includes the temperature warning reason
- ESP32 calls `https://openai-blynk-bridge.onrender.com/esp32-analyse`
- Render returns an advisory
- V11 receives the advisory summary
- V12 receives the recommended laboratory check

This test does not require V9 to become 1 because threshold WARNING alone is sufficient to request a Render advisory.

## Test 3 - Temperature Critical

Increase the controlled laboratory temperature above the prototype critical threshold.

Expected:

- V4 = CRITICAL
- V10 includes the critical temperature reason
- V11 reports a critical prototype condition advisory
- V12 recommends safely stopping the laboratory stimulus and verifying the measurement before continuing

## Test 4 - Rapid Trend Anomaly

Create a controlled rapid change large enough to exceed one of the trend rules within the approximately 10-second comparison window.

Expected:

- V8 describes the detected rapid change
- V9 = 1
- V4 is at least WARNING
- V11/V12 update from Render

## Test 5 - Sensor Fault

Temporarily create a safe sensor communication/read failure during laboratory testing.

Expected:

- V4 = WARNING
- V10 identifies the relevant sensor read/communication failure
- Render advisory is requested because the overall condition is abnormal
- V11/V12 provide a safe verification recommendation

## Test 6 - Recovery

Return all monitored parameters to normal and restore valid sensor readings.

Expected:

- V4 returns to NORMAL
- V9 returns to 0 when no rapid trend remains
- V11 = `No active abnormal condition. ESP32 threshold and trend monitoring continue locally.`
- V12 = `Continue monitoring and record baseline data for threshold validation.`

## Serial Monitor Indicators

During an abnormal condition, a successful Render request should print:

```text
Render mode  : RULE
AI/Advisory : <summary>
Next check   : <recommended check>
```

If AI mode is configured later, `Render mode` may show `AI` instead of `RULE`.

If the Render request fails, the ESP32 local threshold/trend/condition logic remains active and V11/V12 show a fallback message.

## Data Recording

Record actual measurements and results in `testing/test_results.csv`. Do not invent or pre-fill experimental measurements.

## Safety

This procedure is for a laboratory-scale prototype only. Do not connect the ESP32, DHT11, PZEM-004T or prototype wiring directly to energized 33 kV or 11 kV equipment. Use safe isolated laboratory sources and suitable supervision.
