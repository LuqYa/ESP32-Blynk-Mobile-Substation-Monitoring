# Render Advisory Compatibility Test

The V0–V11 firmware computes and publishes locally. It no longer automatically calls Render or publishes advisory text to V11/V12. V11 is an integer anomaly count.

1. GET [health](https://openai-blynk-bridge.onrender.com/health): expect ok=true, architecture=V0-V11, decisionOwner=ESP32, blynkWritesEnabled=false.
2. POST /esp32-analyse with `{"v4":"WARNING","v5":"High Current","v9":"STABLE","v10":1,"v11":1}`: expect SUMMARY/ACTION/MODE lines and a current-related advisory.
3. Test NORMAL and SYSTEM FAULT explicitly.
4. POST /blynk-webhook: expect HTTP 410; confirm it cannot overwrite V11.
5. Verify actual V0–V11 dashboard behavior separately with [test_procedure.md](test_procedure.md).

No Blynk device token is required on Render. Optional AI mode uses the existing server-side key and shared-secret header. No secrets are needed for RULE tests.
