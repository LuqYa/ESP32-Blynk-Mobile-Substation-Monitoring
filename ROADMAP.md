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
- [x] Add direct ESP32-to-Render advisory integration
- [x] Add GitHub Actions firmware compile verification
- [x] Verify firmware compiles with ESP32 + Blynk + DHT + PZEM libraries
- [x] Add repository secret-safety check

## Phase 2 — Render / Cloud Integration

- [x] Deploy Render bridge
- [x] Add `/health` endpoint
- [x] Add `/esp32-analyse` endpoint
- [x] Implement rule-based advisory fallback
- [x] Verify `SUMMARY / ACTION / MODE` response contract
- [x] Add automated live Render integration tests
- [x] Test temperature warning advisory
- [x] Test humidity warning advisory
- [x] Test voltage warning advisory
- [x] Test current warning advisory
- [x] Test sensor fault advisory
- [x] Test critical-condition advisory

## Phase 3 — Blynk Setup

- [ ] Create Blynk template
- [ ] Configure V0–V12 datastreams
- [ ] Build phone/web dashboard
- [ ] Add advisory widgets for V11 and V12
- [ ] Verify ESP32 connection to Blynk
- [ ] Verify V0–V10 update correctly
- [ ] Verify V11/V12 update after Render response

## Phase 4 — Physical Bring-Up and Hardware Validation

Follow `testing/physical_bringup_checklist.md` in sequence.

- [ ] Upload the verified firmware to the physical ESP32
- [ ] Confirm stable ESP32 boot and Serial Monitor at 115200 baud
- [ ] Confirm Wi-Fi and Blynk connection before attaching measurement hardware
- [ ] Assemble ESP32 + DHT11
- [ ] Verify V0/V1 temperature and humidity readings
- [ ] Verify the physical ESP32 can obtain a Render advisory and update V11/V12
- [ ] Identify the exact PZEM-004T hardware version and verify its communication-interface requirements
- [ ] Verify PZEM UART-side wiring before energising the AC measurement side
- [ ] Assemble PZEM-004T + CT under safe laboratory conditions
- [ ] Verify voltage/current/power/frequency/power-factor readings
- [ ] Compare voltage/current measurements with a suitable reference instrument

## Phase 5 — Condition Monitoring Tests

Use `testing/test_procedure.md` and record every run in `testing/test_results.csv`.

- [ ] Normal-condition baseline
- [ ] Temperature warning test
- [ ] Temperature critical test
- [ ] Humidity warning test
- [ ] Humidity critical test
- [ ] Voltage warning/critical test where safely achievable
- [ ] Current warning/critical test within equipment ratings
- [ ] Temperature trend anomaly test
- [ ] Humidity trend anomaly test
- [ ] Voltage-change trend anomaly test where safely achievable
- [ ] Current-rise trend anomaly test
- [ ] DHT11 sensor-fault test
- [ ] PZEM communication-fault test
- [ ] Multiple-abnormal-parameter test
- [ ] Wi-Fi reconnection test
- [ ] Confirm V11/V12 advisory reset after returning to NORMAL

## Phase 6 — Data and Threshold Review

- [x] Align `testing/test_results.csv` with V0–V12 and current firmware behaviour
- [x] Add reference voltage/current and percentage-error fields
- [x] Add V10 condition reason and V11/V12 advisory fields
- [ ] Complete `testing/test_results.csv` using actual measurements
- [ ] Repeat principal tests for consistency
- [ ] Review initial threshold values
- [ ] Document final threshold justification
- [ ] Identify limitations and sources of measurement error

## Phase 7 — FYP Evidence and Documentation

- [ ] Capture hardware photograph
- [ ] Capture Serial Monitor output
- [ ] Capture Blynk normal-state screenshot
- [ ] Capture Blynk warning-state screenshot
- [ ] Capture Blynk critical-state screenshot
- [ ] Capture anomaly/trend screenshot showing V8/V9
- [ ] Capture V11/V12 advisory screenshot
- [ ] Capture reference-instrument comparison evidence where applicable
- [ ] Update Chapter 3 implementation text
- [ ] Prepare results/discussion material
- [ ] Prepare final presentation figures

## Final Milestone

A complete laboratory-scale prototype that measures selected environmental and AC electrical parameters, processes threshold/trend/rule-based condition logic locally on the ESP32, transmits data to Blynk, classifies conditions as NORMAL/WARNING/CRITICAL, and obtains a non-protection advisory response from the Render bridge for abnormal laboratory conditions. Machine Learning, MQTT and Node-RED are not required for the core FYP method.
