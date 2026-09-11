# Physical Bring-Up Checklist

Use this checklist when moving from the verified GitHub/Render software setup to the physical laboratory prototype.

The sequence intentionally starts with low-voltage functions before any PZEM/mains test.

## Stage 1 — ESP32 Only

- [ ] Use a known-good USB cable that supports data, not charge-only.
- [ ] Connect the ESP32 to the computer by USB only.
- [ ] Open `firmware/firmware.ino` in Arduino IDE.
- [ ] Confirm the required libraries are installed: Blynk, DHT sensor library, Adafruit Unified Sensor, and PZEM004Tv30.
- [ ] Select the correct ESP32 board type.
- [ ] Select the correct COM port.
- [ ] Create local `firmware/secrets.h` from `firmware/secrets.example.h`.
- [ ] Enter Wi-Fi and Blynk credentials locally only.
- [ ] Do not commit `secrets.h` to GitHub.
- [ ] Compile the sketch locally.
- [ ] Upload the sketch to the ESP32.
- [ ] Open Serial Monitor at 115200 baud.
- [ ] Confirm the ESP32 boots without repeated resets.

Expected software note: GitHub Actions already verifies that the repository firmware compiles with the documented ESP32/Blynk/DHT/PZEM library stack. The local compile confirms your Arduino IDE and selected board environment also work.

## Stage 2 — Wi-Fi and Blynk

Before connecting sensors, verify the cloud path.

- [ ] Confirm the ESP32 connects to the intended Wi-Fi network.
- [ ] Confirm the Blynk device shows Online.
- [ ] Confirm the Blynk template contains V0 through V12 exactly as documented in `blynk/dashboard_setup.md`.
- [ ] Confirm no real Wi-Fi password or Blynk Auth Token is stored in GitHub.

If the Blynk device remains offline, check the local Template ID, Template Name, Auth Token, Wi-Fi SSID, Wi-Fi password, and whether the network allows the ESP32 to reach the internet.

## Stage 3 — DHT11 Low-Voltage Test

Keep the PZEM/mains side disconnected for this stage.

Planned DHT11 connection:

| ESP32 | DHT11 |
|---|---|
| 3V3 | VCC |
| GND | GND |
| GPIO 2 | DOUT |

Checklist:

- [ ] Power off before changing connections.
- [ ] Connect DHT11 according to the planned pin map.
- [ ] Recheck VCC, GND and DOUT before powering.
- [ ] Power the ESP32.
- [ ] Confirm Serial Monitor reports temperature and humidity rather than `READ ERROR`.
- [ ] Confirm Blynk V0 updates with temperature.
- [ ] Confirm Blynk V1 updates with humidity.
- [ ] Leave the prototype stable for at least 30 seconds and record baseline values.

Do not apply condensation or allow water/moisture to contact the electronics during humidity testing.

## Stage 4 — Render Advisory Path

The Render service is already cloud-tested. This stage confirms the physical ESP32 can use it.

- [ ] With Wi-Fi connected, create a safe abnormal DHT11 test condition.
- [ ] Confirm Serial Monitor shows the local ESP32 condition result.
- [ ] Confirm an abnormal condition causes a request to `https://openai-blynk-bridge.onrender.com/esp32-analyse`.
- [ ] Confirm Serial Monitor shows `Render mode : RULE` unless optional AI mode has been deliberately enabled.
- [ ] Confirm Blynk V11 receives the advisory summary.
- [ ] Confirm Blynk V12 receives the recommended check.
- [ ] Return the sensor to a normal condition.
- [ ] Confirm V11/V12 return to the normal-monitoring message after recovery.

## Stage 5 — PZEM Communication Side Only

Before any mains-voltage measurement, identify the exact PZEM-004T hardware version and confirm its manufacturer/module interface requirements.

Planned UART mapping in the firmware:

| ESP32 | Function |
|---|---|
| GPIO 12 | RX from PZEM TX |
| GPIO 13 | TX to PZEM RX |

- [ ] Keep the mains measurement terminals de-energized while first checking the communication-side wiring.
- [ ] Verify the exact PZEM-004T version and pin labels physically match the project documentation.
- [ ] Verify the required PZEM communication-side supply voltage.
- [ ] Verify that the PZEM TX logic level is safe for the ESP32 RX input; use appropriate interface/level shifting if required by the exact module version.
- [ ] Confirm ESP32 RX is connected to PZEM TX and ESP32 TX to PZEM RX.
- [ ] Confirm common reference/ground arrangement only where required by the exact module interface.
- [ ] Do not energize the AC measurement side merely to troubleshoot the UART wiring.

## Stage 6 — PZEM AC Measurement Test

This stage involves hazardous voltage and must be performed only under suitable laboratory supervision, protection and equipment ratings.

- [ ] Confirm the exact PZEM-004T/CT wiring from the module documentation before energizing.
- [ ] Use a suitable protected laboratory AC setup.
- [ ] Keep all low-voltage ESP32/user-accessible parts safely separated from hazardous conductors.
- [ ] Verify voltage using a suitable reference instrument.
- [ ] Verify current using a suitable reference instrument where available.
- [ ] Confirm Serial Monitor reports voltage/current/power/frequency/power factor.
- [ ] Confirm Blynk V2, V3, V5, V6 and V7 update.
- [ ] Record PZEM and reference readings in `testing/test_results.csv`.
- [ ] Calculate voltage/current percentage error using the equations in `testing/data_collection_plan.md`.

Do not connect this prototype directly to energized 33 kV or 11 kV equipment.

## Stage 7 — Formal FYP Tests

After all individual functions are stable:

- [ ] Run T01 Normal.
- [ ] Run T02/T03 Temperature threshold tests.
- [ ] Run T04/T05 Humidity threshold tests.
- [ ] Run T06–T09 Voltage threshold tests only where safely achievable.
- [ ] Run T10/T11 Current threshold tests only within laboratory equipment ratings.
- [ ] Run T12–T15 trend-anomaly tests.
- [ ] Run T16/T17 sensor-fault tests using safe low-voltage communication-side methods.
- [ ] Run T18 multiple-abnormal-parameter test.
- [ ] Run T19 Wi-Fi interruption/reconnection test.
- [ ] Repeat principal tests at least three times where practical.
- [ ] Record actual values only in `testing/test_results.csv`.

## Evidence to Capture

For the final report/presentation, capture evidence only after the system is operating correctly:

- [ ] Full prototype photograph.
- [ ] ESP32 + DHT11 connection photograph.
- [ ] PZEM/CT laboratory setup photograph taken from a safe viewpoint.
- [ ] Serial Monitor normal-condition screenshot.
- [ ] Serial Monitor abnormal-condition + Render response screenshot.
- [ ] Blynk normal dashboard screenshot.
- [ ] Blynk WARNING screenshot.
- [ ] Blynk CRITICAL screenshot.
- [ ] Blynk trend anomaly screenshot showing V9/V8.
- [ ] Blynk V11/V12 advisory screenshot.
- [ ] Reference-instrument comparison photograph/readings where applicable.

## Stop Conditions

Stop the test and de-energize the relevant setup if any of the following occurs:

- unexpected heating, smell, smoke or damaged insulation;
- repeated ESP32 resets after connecting a module;
- uncertain PZEM terminal identification;
- uncertain mains/CT wiring;
- exposed hazardous conductors;
- readings are grossly inconsistent with the reference instrument;
- the planned test would require exceeding the rating of the PZEM, CT, load, conductor, protection device or laboratory supply.
