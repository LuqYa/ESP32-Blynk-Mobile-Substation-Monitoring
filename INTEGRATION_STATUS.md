# Integration Status — V0-V11

Repository: LuqYa/ESP32-Blynk-Mobile-Substation-Monitoring.
Connected Render service: openai-blynk-bridge (Singapore), auto-deploys main.
Service URL: https://openai-blynk-bridge.onrender.com

## Architecture

ESP32 -> Blynk V0–V11. All thresholds, four-reading trends, multi-anomaly counting, sensor faults, three-reading recovery and four events execute on ESP32. Render's optional /esp32-analyse returns advisory text only. Legacy /blynk-webhook returns 410 and cannot write to Blynk.

## Verification

See the commit-specific GitHub Actions results for firmware compilation, monitoring behavior tests, bridge contract tests and repository safety checks. Check live /health for architecture=V0-V11 after deployment. Previous PASS statements applied to the former architecture and are not hardware evidence for this version.

## Physical/account steps

1. Apply the datastream types, colors and four event notification settings in [Blynk setup](blynk/dashboard_setup.md).
2. Create local secrets.h, compile and upload firmware/firmware.ino to the actual ESP32.
3. Verify the latest UART mapping (RX=13, TX=12) against the physical wiring.
4. Run [tests](testing/test_procedure.md) and record actual measurements, LED colors and phone notifications.

A Render deploy does not flash the ESP32 or change the user's Blynk template.
