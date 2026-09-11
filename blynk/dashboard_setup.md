# Blynk Dashboard Setup

This guide matches `firmware/mobile_substation_monitoring.ino` and the current Blynk Console workflow.

## 1. Enable Developer Mode

In Blynk.Console, open your profile and make sure **Developer Mode** is enabled. Template creation and Web Dashboard editing require Developer Mode.

## 2. Create the Device Template

Go to **Developer Zone -> My Templates -> New Template**.

Use:

- Template Name: `Mobile Substation Monitoring`
- Hardware: `ESP32` (choose the closest ESP32 option available; use Other only if your board is not listed)
- Connection Type: `WiFi`

After creation, Blynk generates a **Template ID**. Keep this value private/local for use in `firmware/secrets.h`.

## 3. Create Virtual Pin Datastreams

Open the Template -> **Datastreams** -> **Add Datastream** -> **Virtual Pin**.

Create these datastreams exactly so they match the firmware:

| Pin | Name | Data Type | Unit | Suggested Min | Suggested Max |
|---|---|---|---|---:|---:|
| V0 | Temperature | Double | °C | 0 | 60 |
| V1 | Humidity | Double | %RH | 0 | 100 |
| V2 | Voltage | Double | V | 0 | 300 |
| V3 | Current | Double | A | 0 | 10 |
| V4 | Overall Condition | String | - | - | - |
| V5 | Power | Double | W | 0 | 2500 |
| V6 | Frequency | Double | Hz | 40 | 60 |
| V7 | Power Factor | Double | - | 0 | 1 |
| V8 | Anomaly Message | String | - | - | - |
| V9 | Anomaly Flag | Integer | - | 0 | 1 |
| V10 | Condition Reason | String | - | - | - |

Enable history for numerical datastreams when available so charts can show recorded trends.

## 4. Create the Web Dashboard

Open the Template -> **Web Dashboard** -> **Edit**. Add and configure widgets by assigning each one to its datastream.

Recommended layout:

### System Status

- Overall Condition -> V4
- Anomaly Indicator -> V9
- Anomaly Message -> V8
- Condition Reason -> V10

### Environmental Monitoring

- Temperature Gauge / Value -> V0
- Humidity Gauge / Value -> V1
- Chart -> V0 and V1

### Electrical Monitoring

- Voltage Gauge / Value -> V2
- Current Gauge / Value -> V3
- Power Value -> V5
- Frequency Value -> V6
- Power Factor Value -> V7
- Chart -> V2 and V3

Save the dashboard. The template dashboard defines the interface; live values are viewed from the actual device dashboard after a device is created from the template.

## 5. Create the Device

For this FYP prototype, manual device creation with an Auth Token is suitable.

Go to **Devices / Search -> New Device -> From Template** (wording may vary slightly in the Console).

- Template: `Mobile Substation Monitoring`
- Device Name: `FYP Mobile Substation Prototype`

Create the device, open **Device Info**, and copy its **Auth Token** locally. Do not commit the Auth Token to GitHub.

## 6. Create `secrets.h` Locally

Copy:

`firmware/secrets.example.h`

and rename the copy to:

`firmware/secrets.h`

Fill in your own values locally:

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BLYNK_AUTH_TOKEN "YOUR_DEVICE_AUTH_TOKEN"
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Mobile Substation Monitoring"
```

The repository `.gitignore` excludes `firmware/secrets.h`. Never upload this file manually to a public repository.

## 7. Mobile Dashboard

Blynk Web Dashboard and Blynk mobile dashboard are configured separately but can use the same datastreams.

In the Blynk mobile app:

1. Enable Developer Mode.
2. Open the `Mobile Substation Monitoring` template.
3. Add widgets for Temperature V0, Humidity V1, Voltage V2, Current V3 and Overall Condition V4.
4. Add labeled values for Power V5, Frequency V6 and Power Factor V7.
5. Add a SuperChart using V0/V1 and another using V2/V3 where your plan/widget availability permits.
6. Add an indicator/value for Anomaly Flag V9 and labels for V8/V10.

## 8. Firmware Test Sequence

1. Install the Blynk, DHT and PZEM004Tv30 libraries in Arduino IDE.
2. Compile `firmware/mobile_substation_monitoring.ino`.
3. Upload to the ESP32.
4. Open Serial Monitor at 115200 baud.
5. Confirm the ESP32 connects to Wi-Fi and Blynk.
6. Confirm V0/V1 update from DHT11.
7. With the PZEM-004T connected using an appropriate safe laboratory setup, confirm V2/V3/V5/V6/V7 update.
8. Confirm V4 reports NORMAL/WARNING/CRITICAL as expected.
9. Create controlled abnormal conditions and check V8/V9/V10.
10. Record actual results in `testing/test_results.csv`.

## Important Notes

- Virtual Pins such as V0 are Blynk software channels, not ESP32 physical GPIO pins.
- Web and mobile dashboards must be configured independently even though they use the same datastreams.
- The values in this repository are laboratory prototype thresholds and must be reviewed using actual baseline/test data.
- Do not connect the laboratory prototype directly to energized 33 kV or 11 kV equipment.
