import 'dotenv/config';
import express from 'express';
import OpenAI from 'openai';

const app = express();
app.disable('x-powered-by');
app.use(express.json({ limit: '64kb' }));

const REQUIRED_ENV = [
  'OPENAI_API_KEY',
  'BLYNK_DEVICE_TOKEN',
  'WEBHOOK_SECRET'
];

const BLYNK_SERVER = (process.env.BLYNK_SERVER || 'https://blynk.cloud').replace(/\/$/, '');
const MODEL = process.env.OPENAI_MODEL || 'gpt-5.6-luna';
const PORT = Number(process.env.PORT || 3000);

function missingEnvironment() {
  return REQUIRED_ENV.filter((key) => !process.env[key]);
}

function clip(value, max = 220) {
  return String(value ?? '').replace(/\s+/g, ' ').trim().slice(0, max);
}

function getOpenAIClient() {
  if (!process.env.OPENAI_API_KEY) {
    throw new Error('OPENAI_API_KEY is not configured');
  }
  return new OpenAI({ apiKey: process.env.OPENAI_API_KEY });
}

async function getBlynkValues() {
  if (!process.env.BLYNK_DEVICE_TOKEN) {
    throw new Error('BLYNK_DEVICE_TOKEN is not configured');
  }

  const url = new URL(`${BLYNK_SERVER}/external/api/getAll`);
  url.searchParams.set('token', process.env.BLYNK_DEVICE_TOKEN);

  const response = await fetch(url, {
    headers: { accept: 'application/json' }
  });

  if (!response.ok) {
    throw new Error(`Blynk getAll failed: ${response.status} ${await response.text()}`);
  }

  return response.json();
}

async function updateBlynk(pin, value) {
  if (!process.env.BLYNK_DEVICE_TOKEN) {
    throw new Error('BLYNK_DEVICE_TOKEN is not configured');
  }

  const url = new URL(`${BLYNK_SERVER}/external/api/update`);
  url.searchParams.set('token', process.env.BLYNK_DEVICE_TOKEN);
  url.searchParams.set(String(pin).toLowerCase(), clip(value));

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

  const openai = getOpenAIClient();
  const response = await openai.responses.create({
    model: MODEL,
    input,
    max_output_tokens: 160
  });

  const text = response.output_text || '';
  return {
    summary: extractLine(text, 'SUMMARY') || clip(text),
    action: extractLine(text, 'ACTION') || 'Review the flagged parameter and verify the sensor reading against the laboratory reference instrument.'
  };
}

app.get('/health', (req, res) => {
  const missing = missingEnvironment();
  res.set('Cache-Control', 'no-store');
  res.json({
    ok: true,
    service: 'openai-blynk-bridge',
    configured: missing.length === 0,
    missing
  });
});

app.get('/ready', (req, res) => {
  const missing = missingEnvironment();
  res.set('Cache-Control', 'no-store');

  if (missing.length > 0) {
    return res.status(503).json({
      ready: false,
      missing
    });
  }

  res.json({
    ready: true,
    model: MODEL,
    blynkServer: BLYNK_SERVER
  });
});

app.post('/blynk-webhook', async (req, res) => {
  try {
    const missing = missingEnvironment();
    if (missing.length > 0) {
      return res.status(503).json({
        error: 'Bridge is not fully configured',
        missing
      });
    }

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
  const missing = missingEnvironment();
  console.log(`OpenAI-Blynk bridge listening on port ${PORT}`);
  if (missing.length > 0) {
    console.warn(`Bridge is not ready. Missing: ${missing.join(', ')}`);
  }
});
