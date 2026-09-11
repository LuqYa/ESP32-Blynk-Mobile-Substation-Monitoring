# Chapter 3 Methodology Alignment

This note maps the GitHub implementation work to the current FYP Chapter 3 structure.

## 3.1 Introduction

Describe the development of the laboratory-scale ESP32 and Blynk condition-monitoring prototype, including sensor acquisition, wireless transmission, dashboard display, threshold checking, trend monitoring, and rule-based anomaly detection.

## 3.2 Project Design

Use the project architecture represented in this repository:

```text
Sensors / Monitoring Inputs
        |
        v
      ESP32
        |
      Wi-Fi
        |
        v
    Blynk Cloud
        |
        v
  Blynk Dashboard
        |
        v
Threshold + Trend + Rule-Based Analysis
        |
 Normal / Warning / Critical
```

The main hardware references are in `hardware/connection_summary.md` and the main firmware is in `firmware/mobile_substation_monitoring.ino`.

## 3.3 Data Collection

Follow `testing/data_collection_plan.md` and record actual measurements in `testing/test_results.csv`.

Collected parameters include:

- Temperature
- Relative humidity
- AC voltage
- AC current
- Power
- Frequency
- Power factor
- Overall condition
- Anomaly status

Data should be collected under normal and controlled abnormal conditions.

## 3.4 Project Implementation / Installation

Implementation consists of:

1. Connecting the DHT11 and PZEM-004T to the ESP32 according to the laboratory connection plan.
2. Installing the required Arduino libraries.
3. Configuring Wi-Fi and Blynk credentials locally using `secrets.h`.
4. Creating the Blynk datastreams and widgets described in `blynk/dashboard_setup.md`.
5. Uploading the firmware to the ESP32.
6. Verifying values in Serial Monitor and Blynk.
7. Running threshold, trend and anomaly tests.
8. Recording and reviewing the results.

The condition-monitoring algorithm does not use Machine Learning, MQTT, or Node-RED. It uses direct ESP32-to-Blynk communication and rule-based analysis.

## 3.5 Economic Analysis

The economic analysis should focus on the prototype development cost rather than inventing operational savings that have not been measured. A component-cost table may include:

- ESP32
- DHT11
- PZEM-004T
- Current transformer
- Power supply
- Prototype board/enclosure
- Wiring/connectors
- Supporting laboratory components

Actual purchase prices should be entered from receipts or current supplier quotations when available.

## Evidence to Capture for the FYP Report

Keep the following evidence during implementation:

- Photograph of assembled hardware
- Screenshot of Arduino IDE / Serial Monitor
- Screenshot of Blynk dashboard under normal condition
- Screenshot of Warning condition
- Screenshot of Critical condition
- Screenshot of trend/anomaly indication
- Completed test-results table
- Final threshold table with justification

These items can later be used in the methodology, implementation, testing and presentation sections of the FYP.
