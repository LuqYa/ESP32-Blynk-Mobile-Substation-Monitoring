# Blynk V0-V11 Dashboard Setup

ESP32 owns all measurement, condition, trend, recovery and event decisions.

## Template and datastreams

Template: Mobile Substation Monitoring; hardware: ESP32; connection: WiFi.
Keep Template ID and device Auth Token in local `firmware/secrets.h`.

| Pin | Name | Type | Range / default |
|---|---|---|---|
| V0 | Temperature | Double | 0–60 °C |
| V1 | Humidity | Integer | 0–100 % |
| V2 | Voltage | Double | 0–300 V |
| V3 | Current | Double | 0–100 A |
| V4 | Overall Condition | String | NORMAL |
| V5 | Anomaly Cause | String | No abnormal condition |
| V6 | Recommended Action | String | No inspection required |
| V7 | DHT Status | Integer | 0–1; default 0 |
| V8 | PZEM Status | Integer | 0–1; default 0 |
| V9 | Trend Status | String | STABLE |
| V10 | Alarm Level | Integer | 0–3; default 0 |
| V11 | Active Anomalies | Integer | 0–4; default 0 |

Firmware truncates V1 humidity to an integer. A gauge may use a smaller display range for the actual laboratory load without changing the datastream mapping.

**Migration:** reconfigure old V5–V11 datastream types, remove obsolete V12 widgets, and disable the old V9 webhook before using this firmware. The Render webhook now returns HTTP 410 and never writes to Blynk. Historical values from the old mapping must not be interpreted as new measurements.

## Runtime colors

V7 and V8 remain at 1 (LED on) for both healthy and faulty sensors. Firmware changes `color` using `Blynk.setProperty`: healthy #23C48E, fault #D3435C. Their numeric values are brightness, not sensor-health flags. Both fault causes appear in V5 if both sensors fail.

V4 color: NORMAL #23C48E; WARNING #FFC107; CRITICAL #D3435C; SYSTEM FAULT #6C757D. V10 encodes these as 0, 1, 2, 3. Reconnection restores colors. When the device is offline, last-known dashboard values/colors can remain visible; use Blynk's device-online indication to determine freshness.

## Four events and notifications

Create these exact event codes in the template, enable Timeline and notifications for the intended recipients, and apply template changes to the active device.

| Event code | Trigger |
|---|---|
| condition_warning | Enter WARNING |
| critical_condition | Enter CRITICAL |
| system_fault | Enter SYSTEM FAULT |
| condition_recovered | Return to NORMAL after three consecutive normal readings |

Keep datastream Automation and Event Automation OFF. Event notifications are configured separately and should be ON where alerts are wanted.

Firmware logs severity transitions only, with a 60-second cooldown per event code. Disconnected/cooldown transitions are coalesced to the latest state, not replayed as a historical queue. Same-severity cause changes still update V5 but do not generate another event. A condition that starts and ends entirely offline is not replayed. A reconnect after a previously sent abnormal event can emit recovery once the local recovery condition is met. Cloud delivery is verified in Device Timeline; `logEvent` is not a delivery acknowledgement.

Blynk applies a daily event quota; rapid repeated tests can reach it even with this cooldown. See [Blynk events](https://docs.blynk.io/en/getting-started/events-tutorial) and [LED properties](https://docs.blynk.io/en/blynk.apps/widgets-displays/led).

## Layout

1. Large Overall Condition V4.
2. Temperature V0, Humidity V1, Voltage V2, Current V3 in one row.
3. Anomaly Cause V5 beside Trend Status V9.
4. Recommended Action V6 beside Active Anomalies V11.
5. DHT Status V7 and PZEM Status V8 LED widgets.
6. One historical graph containing V0–V3. V10 can remain internal.

## Verification

Upload the firmware, confirm the device is online, then run [the test procedure](../testing/test_procedure.md). Verify actual LED colors and event/phone delivery on the physical device; repository/cloud tests cannot establish those results.
