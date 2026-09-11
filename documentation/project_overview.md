# Project Overview

## Title

Development of an IoT-Based Condition Monitoring System for a 33/11 kV Mobile Substation

## Overview

This Final Year Project develops a laboratory-scale IoT monitoring prototype for selected mobile-substation parameters. The system uses an ESP32 microcontroller, DHT11 sensor, PZEM-004T electrical monitoring module, Wi-Fi connectivity, and the Blynk platform.

## Monitoring Scope

The prototype focuses on selected parameters that can indicate environmental and electrical operating condition, including temperature, relative humidity, AC voltage, AC current, and selected status/alarm signals.

## Condition Analysis

The software is intended to use simple and explainable methods suitable for the FYP prototype:

- predefined threshold checking;
- trend observation;
- rule-based anomaly detection; and
- Normal, Warning, and Critical classification.

Machine Learning, Node-RED, and MQTT are not required in the current project architecture.

## Safety Boundary

The prototype is not intended to directly instrument energized 33 kV or 11 kV primary equipment. Safe isolated signals, laboratory supplies, and simulated equipment conditions should be used to demonstrate the monitoring concept.

## Intended Outputs

- functioning ESP32-based prototype;
- real-time Blynk dashboard;
- recorded sensor and electrical measurements;
- condition/alarm indication;
- prototype testing results; and
- documented system architecture and implementation.
