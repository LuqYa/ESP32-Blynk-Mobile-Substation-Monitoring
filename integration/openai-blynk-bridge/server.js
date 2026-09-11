import 'dotenv/config';
import express from 'express';
import OpenAI from 'openai';

const app = express();
app.disable('x-powered-by');
app.use(express.json({ limit: '64kb' }));

const BLYNK_SERVER = (process.env.BLYNK_SERVER || 'https://blynk.cloud').replace(/\/$/, '');
const MODEL = process.env.OPENAI_MODEL || 'gpt-5.6-luna';
const PORT = Number(process.env.PORT || 3000);

function clip(value, max = 220) {
  return String(value ?? '').replace(/\s+/g, ' ').trim().slice(0, max);
}

function extractLine(text, label) {
  const regex = new RegExp(`^${label}:\\s*(.+)$`, 'im');
  const match = text.match(regex);
  return match ? clip(match[1], 180) : '';
}

function ruleBasedAdvisory(values = {}) {
  const condition = String(values.condition || values.v4 || '').toUpperCase();
  const trend = String(values.trend || values.v8 || '');
  const reason = String(values.reason || values.v10 || '');
  const combined = `${trend} ${reason}`.toLowerCase();

  if (condition === 'CRITICAL') {
    return {
      summary: `Critical prototype condition reported: ${clip(reason || trend || 'critical threshold exceeded', 145)}.`,
      action: 'Stop the laboratory test stimulus safely and verify the flagged sensor value with a reference instrument before continuing.',
      mode: 'RULE'
    };
  }

  if (combined.includes('temperature')) {
    return {
      summary: 'Temperature-related abnormal trend detected by the ESP32 prototype logic.',
      action: 'Verify DHT11 temperature with a reference thermometer and inspect the laboratory heat source and ventilation.',
      mode: 'RULE'
    };
  }

  if (combined.includes('humidity')) {
    return {
      summary: 'Humidity-related abnormal trend detected by the ESP32 prototype logic.',
      action: 'Verify DHT11 humidity against a reference meter and inspect the controlled laboratory environment.',
      mode: 'RULE'
    };
  }

  if (combined.includes('voltage')) {
    return {
      summary: 'Voltage-related abnormal condition detected by the ESP32 prototype logic.',
      action: 'Check the safe laboratory supply and compare the PZEM voltage reading with a suitable reference meter.',
      mode: 'RULE'
    };
  }

  if (combined.includes('current')) {
    return {
      summary: 'Current-related abnormal condition detected by the ESP32 prototype logic.',
      action: 'Check the laboratory load and CT arrangement, then compare the current reading with a suitable reference instrument.',
      mode: 'RULE'
    };
  }

  return {
    summary: clip(reason || trend || 'Abnormal prototype condition reported by the ESP32 monitoring logic.', 180),
    action: 'Verify the flagged parameter using the laboratory reference instrument and record the result before continuing the test.',
    mode: 'RULE'
  };
}

async function aiAdvisory(values) {
  const fallback = ruleBasedAdvisory(values);
  const expectedSecret = process.env.BRIDGE_SHARED_SECRET;
  const suppliedSecret = String(values.__bridgeSecret || '');

  // OpenAI is used only when both a server-side shared secret and API key are configured.
  // Otherwise the bridge deliberately stays in rule-based fallback mode.
  if (!process.env.OPENAI_API_KEY || !expectedSecret || suppliedSecret !== expectedSecret) {
    return fallback;
  }

  const input = `
You are assisting with a university laboratory prototype for condition monitoring of a 33/11 kV mobile substation.
This is NOT a live protection or switching system. Give monitoring interpretation and safe laboratory checks only.
Do not tell the system to trip, energise, isolate, switch, or control high-voltage equipment.

Measurements and ESP32 results:
Temperature: ${values.temperature ?? values.v0 ?? 'N/A'} C
Humidity: ${values.humidity ?? values.v1 ?? 'N/A'} %RH
Voltage: ${values.voltage ?? values.v2 ?? 'N/A'} V
Current: ${values.current ?? values.v3 ?? 'N/A'} A
Condition: ${values.condition ?? values.v4 ?? 'N/A'}
Trend: ${values.trend ?? values.v8 ?? 'N/A'}
Reason: ${values.reason ?? values.v10 ?? 'N/A'}

Return exactly two short lines, each under 180 characters:
SUMMARY: <interpretation>
ACTION: <safe laboratory check>
`;

  try {
    const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });
    const response = await openai.responses.create({
      model: MODEL,
      input,
      max_output_tokens: 160
    });

    const text = response.output_text || '';
    return {
      summary: extractLine(text, 'SUMMARY') || fallback.summary,
      action: extractLine(text, 'ACTION') || fallback.action,
      mode: 'AI'
    };
  } catch (error) {
    console.error('OpenAI advisory failed; using rule fallback:', error.message);
    return fallback;
  }
}

async function getBlynkValues() {
  if (!process.env.BLYNK_DEVICE_TOKEN) {
    throw new Error('BLYNK_DEVICE_TOKEN is not configured');
  }

  const url = new URL(`${BLYNK_SERVER}/external/api/getAll`);
  url.searchParams.set('token', process.env.BLYNK_DEVICE_TOKEN);

  const response = await fetch(url, { headers: { accept: 'application/json' } });
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

app.get('/health', (req, res) => {
  res.set('Cache-Control', 'no-store');
  res.json({
    ok: true,
    service: 'blynk-render-bridge',
    directEsp32Mode: true,
    legacyWebhookMode: Boolean(process.env.BLYNK_DEVICE_TOKEN && process.env.WEBHOOK_SECRET),
    aiModeAvailable: Boolean(process.env.OPENAI_API_KEY && process.env.BRIDGE_SHARED_SECRET)
  });
});

app.get('/ready', (req, res) => {
  res.set('Cache-Control', 'no-store');
  res.json({
    ready: true,
    directEsp32Mode: true,
    model: MODEL,
    legacyWebhookReady: Boolean(process.env.BLYNK_DEVICE_TOKEN && process.env.WEBHOOK_SECRET)
  });
});

// Default integration path: ESP32 -> Render -> ESP32 -> Blynk V11/V12.
// No Blynk Device Token is required on Render for this mode.
app.post('/esp32-analyse', async (req, res) => {
  try {
    const body = req.body || {};
    const suppliedSecret = req.get('x-bridge-secret') || '';
    const advisory = await aiAdvisory({ ...body, __bridgeSecret: suppliedSecret });

    res.type('text/plain').send(
      `SUMMARY:${clip(advisory.summary, 180)}\n` +
      `ACTION:${clip(advisory.action, 180)}\n` +
      `MODE:${advisory.mode}`
    );
  } catch (error) {
    console.error(error);
    res.status(500).type('text/plain').send(
      'SUMMARY:Bridge processing error.\n' +
      'ACTION:Continue using the ESP32 local condition result and verify the prototype manually.\n' +
      'MODE:ERROR'
    );
  }
});

// Legacy optional path: Blynk V9 webhook -> Render -> Blynk V11/V12.
app.post('/blynk-webhook', async (req, res) => {
  try {
    if (!process.env.BLYNK_DEVICE_TOKEN || !process.env.WEBHOOK_SECRET) {
      return res.status(503).json({
        error: 'Legacy webhook mode is not configured. Use /esp32-analyse for the default integration.'
      });
    }

    const secret = req.get('x-webhook-secret');
    if (!secret || secret !== process.env.WEBHOOK_SECRET) {
      return res.status(401).json({ error: 'Unauthorized' });
    }

    const pin = String(req.body.pin || '').toUpperCase();
    const value = String(req.body.value ?? '');

    if (pin && pin !== 'V9') {
      return res.json({ ok: true, skipped: true, reason: 'Webhook is intended for V9.' });
    }

    if (value === '0') {
      await Promise.all([
        updateBlynk('V11', 'No active anomaly. ESP32 threshold and trend checks are within the current prototype rules.'),
        updateBlynk('V12', 'Continue monitoring and record baseline data for threshold validation.')
      ]);
      return res.json({ ok: true, analysed: false, condition: 'normal' });
    }

    const values = await getBlynkValues();
    const advisory = await aiAdvisory({
      ...values,
      __bridgeSecret: req.get('x-bridge-secret') || ''
    });

    await Promise.all([
      updateBlynk('V11', advisory.summary),
      updateBlynk('V12', advisory.action)
    ]);

    res.json({ ok: true, analysed: true, mode: advisory.mode });
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Bridge processing failed' });
  }
});

app.listen(PORT, () => {
  console.log(`Blynk-Render bridge listening on port ${PORT}`);
  console.log('Default integration: ESP32 -> /esp32-analyse -> ESP32 -> Blynk V11/V12');
  if (!process.env.OPENAI_API_KEY) {
    console.log('OPENAI_API_KEY not set: rule-based advisory fallback is active.');
  }
  if (!process.env.BLYNK_DEVICE_TOKEN) {
    console.log('BLYNK_DEVICE_TOKEN not set: legacy Blynk webhook mode is disabled; direct ESP32 mode remains active.');
  }
});
