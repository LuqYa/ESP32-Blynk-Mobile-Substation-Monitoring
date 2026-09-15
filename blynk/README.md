# Blynk Dashboard

Use [dashboard_setup.md](dashboard_setup.md) for the authoritative V0–V11 map, colors, four event codes and notification setup.

The ESP32 computes NORMAL / WARNING / CRITICAL / SYSTEM FAULT locally. V5 is Anomaly Cause, V6 Recommended Action, V7/V8 sensor LED brightness with runtime health colors, V9 Trend Status, V10 numeric Alarm Level and V11 Active Anomalies.

See [thresholds and persistence](../testing/thresholds.md) and [tests](../testing/test_procedure.md). Render's optional advisory endpoint returns text only; it never overwrites these datastreams.
