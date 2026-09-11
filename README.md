# ESP32-Blynk Mobile Substation Monitoring

Laboratory-scale IoT condition monitoring prototype for a 33/11 kV mobile substation using ESP32, Blynk, DHT11, and PZEM-004T.

## Project Purpose

The project demonstrates real-time monitoring of selected environmental and electrical parameters without directly connecting sensors to energized 33 kV or 11 kV equipment. Safe isolated measurements and simulated operating conditions are used for prototype testing.

## Main Hardware

- ESP32
- DHT11 temperature and humidity sensor
- PZEM-004T AC voltage/current monitoring module
- Current transformer (CT)
- Laboratory power supply and supporting prototype components

## Software

- Arduino IDE
- Blynk IoT
- GitHub

## Monitoring Functions

- Temperature monitoring
- Relative humidity monitoring
- AC voltage monitoring
- AC current monitoring
- Power, frequency and power-factor monitoring
- Normal / Warning / Critical condition classification
- Threshold checking
- Trend monitoring
- Rule-based anomaly detection
- Blynk real-time dashboard

## System Architecture

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
Threshold / Trend / Rule-Based Analysis
          |
   +------+------+ 
   |      |      |
 Normal Warning Critical
```

## Quick Start

1. Install the ESP32 board package and required Arduino libraries.
2. Copy `firmware/secrets.example.h` to `firmware/secrets.h` locally.
3. Insert your own Wi-Fi and Blynk credentials in `secrets.h`.
4. Configure the Blynk datastreams using `blynk/dashboard_setup.md`.
5. Review `hardware/connection_summary.md` before assembling the prototype.
6. Compile and upload `firmware/mobile_substation_monitoring.ino`.
7. Verify sensor values in Serial Monitor and Blynk.
8. Follow `testing/data_collection_plan.md` and `testing/test_procedure.md`.
9. Record only actual measurements in `testing/test_results.csv`.

## Key Project Documents

- `ROADMAP.md` — implementation progress and remaining FYP tasks.
- `hardware/connection_summary.md` — planned laboratory hardware connections and safety scope.
- `blynk/dashboard_setup.md` — Blynk datastream and dashboard configuration.
- `testing/data_collection_plan.md` — baseline and controlled-test data collection plan.
- `testing/test_procedure.md` — formal prototype testing procedure.
- `testing/test_results.csv` — actual test-data recording template.
- `testing/thresholds.md` — threshold and trend-rule documentation.
- `documentation/chapter3_alignment.md` — mapping of repository work to Chapter 3 methodology.
- `documentation/project_overview.md` — project summary.

## Repository Structure

```text
.
├── README.md
├── ROADMAP.md
├── .gitignore
├── firmware/
│   ├── mobile_substation_monitoring.ino
│   └── secrets.example.h
├── hardware/
│   ├── README.md
│   └── connection_summary.md
├── blynk/
│   ├── README.md
│   └── dashboard_setup.md
├── testing/
│   ├── data_collection_plan.md
│   ├── test_procedure.md
│   ├── test_results.csv
│   └── thresholds.md
└── documentation/
    ├── project_overview.md
    └── chapter3_alignment.md
```

## Current Development Status

The repository currently contains the firmware and documentation framework for the prototype. The next evidence must come from the physical implementation: Blynk configuration, hardware assembly, actual sensor readings, baseline data, controlled abnormal-condition tests, and screenshots/photographs of the working system.

## Security

Wi-Fi passwords, Blynk authentication tokens, and other credentials must not be committed to this public repository. Copy `firmware/secrets.example.h` to `firmware/secrets.h` on your computer and insert your real credentials there. The `.gitignore` file excludes `secrets.h` from Git.

## Safety Note

This repository represents a laboratory-scale prototype. It is not intended for direct connection to energized 33 kV or 11 kV equipment. Appropriate isolation, protection, supervision, and engineering procedures are required for any real substation application.

## Academic Context

Final Year Project — Bachelor of Technology in Electrical System Maintenance with Honours, Universiti Tun Hussein Onn Malaysia (UTHM).
