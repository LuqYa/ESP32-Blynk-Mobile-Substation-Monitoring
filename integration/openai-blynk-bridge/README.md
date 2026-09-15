# Render Advisory Bridge — V0-V11

The ESP32 owns all V0–V11 decisions, events and recommended actions. The current firmware does not call Render automatically. This existing service remains available for optional advisory requests; responses never write to Blynk.

## Endpoints

- GET /health: service health and V0-V11 architecture metadata.
- GET /ready: readiness/configuration status.
- POST /esp32-analyse: SUMMARY, ACTION and MODE plain-text response.
- POST /blynk-webhook: HTTP 410 (retired). The previous V9/V11/V12 routing conflicts with the new datastream meanings.

Public service: https://openai-blynk-bridge.onrender.com

## Request

Accepts named fields temperature, humidity, voltage, current, condition, reason, trend, or corresponding v0, v1, v2, v3, v4, v5, v9 fields. V8 is LED brightness; V10 is numeric severity; neither supplies reason/trend text.

Example: `{"condition":"SYSTEM FAULT","reason":"DHT11 Sensor Failure","trend":"STABLE"}`.

RULE mode needs no private credentials. Optional AI behavior remains available when OPENAI_API_KEY and BRIDGE_SHARED_SECRET are configured server-side and the caller supplies X-Bridge-Secret. Keep credentials private. This service does not control the monitoring classification.

## Deployment and tests

Existing Render service: openai-blynk-bridge, Singapore, main branch.
Build: `cd integration/openai-blynk-bridge && npm install`.
Start: `cd integration/openai-blynk-bridge && npm start`.

Run `npm test` for local HTTP contract tests. GitHub Actions tests checked-out code rather than a possibly older live deployment; after deployment verify /health identifies V0-V11.

This is laboratory monitoring/advisory software, not a protection or switching controller.
