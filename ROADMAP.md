# FYP Implementation Roadmap

## Phase 1 — Repository and Firmware Foundation

- [x] Create GitHub repository
- [x] Add project README
- [x] Add secure credential handling with `secrets.h`
- [x] Add DHT11 support
- [x] Add PZEM-004T support
- [x] Add Blynk virtual-pin mapping
- [x] Add threshold classification
- [x] Add trend/rule-based anomaly logic

## Phase 2 — Blynk Setup

- [ ] Create Blynk template
- [ ] Configure V0–V10 datastreams
- [ ] Build phone/web dashboard
- [ ] Verify ESP32 connection to Blynk
- [ ] Verify all widgets update correctly

## Phase 3 — Hardware Validation

- [ ] Assemble ESP32 + DHT11
- [ ] Verify temperature and humidity readings
- [ ] Assemble ESP32 + PZEM-004T + CT under safe laboratory conditions
- [ ] Verify voltage/current readings
- [ ] Compare measurements with a suitable reference instrument

## Phase 4 — Condition Monitoring Tests

- [ ] Normal-condition baseline
- [ ] Temperature warning test
- [ ] Temperature critical test
- [ ] Humidity warning test
- [ ] Humidity critical test
- [ ] Voltage warning/critical test
- [ ] Current warning/critical test
- [ ] Trend anomaly test
- [ ] Sensor fault test
- [ ] Multiple-abnormal-parameter test
- [ ] Wi-Fi reconnection test

## Phase 5 — Data and Threshold Review

- [ ] Complete `testing/test_results.csv`
- [ ] Repeat principal tests for consistency
- [ ] Review initial threshold values
- [ ] Document final threshold justification
- [ ] Identify limitations and sources of measurement error

## Phase 6 — FYP Evidence and Documentation

- [ ] Capture hardware photograph
- [ ] Capture Serial Monitor output
- [ ] Capture Blynk normal-state screenshot
- [ ] Capture Blynk warning-state screenshot
- [ ] Capture Blynk critical-state screenshot
- [ ] Capture anomaly/trend screenshot
- [ ] Update Chapter 3 implementation text
- [ ] Prepare results/discussion material
- [ ] Prepare final presentation figures

## Final Milestone

A complete laboratory-scale prototype that measures selected parameters, transmits them through ESP32 Wi-Fi to Blynk, classifies conditions as NORMAL/WARNING/CRITICAL, and flags simple threshold/trend/rule-based anomalies without Machine Learning, MQTT, or Node-RED.
