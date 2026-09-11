import 'dotenv/config';
import express from 'express';
import OpenAI from 'openai';

const app = express();
app.use(express.json({ limit: '64kb' }));

const required = [
  'OPENAI_API_KEY',
  'BLYNK_DEVICE_TOKEN',
  'WEBHOOK_SECRET'
];

for (const key of required) {
  if (!process.env[key]) {
    console.warn(`Missing environment variable: ${key}`);
  }
}

const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });
const BLYNK_SERVER = (process.env.BLYNK_SERVER || 'https://blynk.cloud').replace(/\/$/, '');
const MODEL = process.env.OPENAI_MODEL || 'gpt-5.6-luna';
const PORT = Number(process.env.PORT || 3000);

function clip(value, max = 220) {
  return String(value ?? '').replace(/\s+/g, ' ').trim().slice(0, max);
}

async function getBlynkValues() {
  const url = new URL(`${BLYNK_SERVER}/external/api/getAll`);
  url.searchParams.set('token', process.env.BLYNK_DEVICE_TOKEN);

  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`Blynk getAll failed: ${response.status} ${await response.text()}`);
  }
  return response.json();
}

async function updateBlynk(pin, value) {
  const url = new URL(`${BLYNK_SERVER}/external/api/update`);
  url.searchParams.set('token', process.env.BLYNK_DEVICE_TOKEN);
  url.searchParams.set(pin.toLowerCase(), clip(value));

  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`Blynk update ${pin} failed: ${response.status} ${await response.text()}`);
  }
}

function extractLine(text, label) {
  const regex = new RegExp(`^${label}:\\s*(.+)$`, 'im');
  const match = text.match(regex);
  return match ? match[1].trim() : '';
}

async function analysePrototype(values) {
  const input = `
You are assisting with a university laboratory prototype for condition monitoring of a 33/11 kV mobile substation.

Important constraints:
- This is a laboratory prototype, not a live high-voltage protection or control system.
- Do not tell the system to operate, trip, energise, isolate, switch, or control high-voltage equipment.
- Give only monitoring interpretation and safe inspection/check recommendations.
- Thresholds and anomaly flags come from the ESP32 firmware and should be treated as prototype indicators.

Current Blynk values:
Temperature V0: ${values.v0 ?? 'N/A'} °C
Humidity V1: ${values.v1 ?? 'N/A'} %RH
Voltage V2: ${values.v2 ?? 'N/A'} V
Current V3: ${values.v3 ?? 'N/A'} A
Overall condition V4: ${values.v4 ?? 'N/A'}
Power V5: ${values.v5 ?? 'N/A'} W
Frequency V6: ${values.v6 ?? 'N/A'} Hz
Power factor V7: ${values.v7 ?? 'N/A'}
Anomaly message V8: ${values.v8 ?? 'N/A'}
Anomaly flag V9: ${values.v9 ?? 'N/A'}
Condition reason V10: ${values.v10 ?? 'N/A'}

Return exactly two short lines, each under 180 characters:
SUMMARY: <what the measurements indicate>
ACTION: <safe next inspection/check for the laboratory prototype>
`;

  const response = await openai.responses.create({
    model: MODEL,
    input
  });

  const text = response.output_text || '';
  return {
    summary: extractLine(text, 'SUMMARY') || clip(text),
    action: extractLine(text, 'ACTION') || 'Review the flagged parameter and verify the sensor reading against the laboratory reference instrument.'
  };
}

app.get('/health', (req, res) => {
  res.json({ ok: true, service: 'openai-blynk-bridge' });
});

app.post('/blynk-webhook', async (req, res) => {
  try {
    const secret = req.get('x-webhook-secret');
    if (!secret || secret !== process.env.WEBHOOK_SECRET) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    // Recommended webhook trigger: V9 (Anomaly Flag) update.
    const pin = String(req.body.pin || '').toUpperCase();
    const value = String(req.body.value ?? '');

    if (pin && pin !== 'V9') {
      return res.json({ ok: true, skipped: true, reason: 'Webhook is intended for V9.' });
    }

    // Avoid an OpenAI call when the firmware reports no anomaly.
    if (value === '0') {
      await Promise.all([
        updateBlynk('V11', 'No active anomaly. ESP32 threshold and trend checks are within the current prototype rules.'),
        updateBlynk('V12', 'Continue monitoring and record baseline data for threshold validation.')
      ]);
      return res.json({ ok: true, analysed: false, condition: 'normal' });
    }

    const values = await getBlynkValues();
    const ai = await analysePrototype(values);

    await Promise.all([
      updateBlynk('V11', ai.summary),
      updateBlynk('V12', ai.action)
    ]);

    res.json({ ok: true, analysed: true });
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Bridge processing failed' });
  }
});

app.listen(PORT, () => {
  console.log(`OpenAI-Blynk bridge listening on port ${PORT}`);
});
