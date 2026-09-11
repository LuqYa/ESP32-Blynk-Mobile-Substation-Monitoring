# Blynk Dashboard

## Suggested Datastreams

| Virtual Pin | Parameter | Unit |
|---|---|---|
| V0 | Temperature | °C |
| V1 | Relative Humidity | % |
| V2 | AC Voltage | V |
| V3 | AC Current | A |
| V4 | Overall Condition | Text |

## Suggested Widgets

- Temperature gauge
- Humidity gauge
- Voltage display/gauge
- Current display/gauge
- Historical chart / SuperChart
- Overall condition indicator

## Condition States

- NORMAL — measurements are within the selected acceptable range.
- WARNING — one or more parameters reach a warning threshold or show an abnormal trend.
- CRITICAL — one or more parameters exceed a critical threshold or satisfy a critical rule.

## Setup Notes

1. Create a Blynk Template for the FYP prototype.
2. Create the datastreams above.
3. Add dashboard widgets and map each widget to the correct virtual pin.
4. Copy your Template ID, Template Name and Auth Token into a local `firmware/secrets.h` file.
5. Never upload your real token or Wi-Fi password to this public repository.

Threshold values in the firmware are initial examples and should be adjusted using the actual prototype test results and the requirements of the monitored equipment.
