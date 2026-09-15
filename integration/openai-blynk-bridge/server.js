import 'dotenv/config';
import express from 'express';
import OpenAI from 'openai';

const app = express();
app.disable('x-powered-by');
app.use(express.json({ limit: '64kb' }));

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
  const trend = String(values.trend || values.v9 || '');
  const reason = String(values.reason || values.v5 || '');
  const combined = `${trend} ${reason}`.toLowerCase();

  if (condition === 'NORMAL') {
    return { summary: 'No abnormal condition', action: 'No inspection required', mode: 'RULE' };
  }
  if (condition === 'SYSTEM FAULT') {
    return {
      summary: clip(reason || 'Sensor monitoring system fault', 180),
      action: 'Inspect the reported sensor connections and verify readings before relying on the monitoring result.',
      mode: 'RULE'
    };
  }

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
Trend: ${values.trend ?? values.v9 ?? 'N/A'}
Reason: ${values.reason ?? values.v5 ?? 'N/A'}

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

app.get('/health', (req, res) => {
  res.set('Cache-Control', 'no-store');
  res.json({
    ok: true,
    service: 'blynk-render-bridge',
    directEsp32Mode: true,
    architecture: 'V0-V11',
    decisionOwner: 'ESP32',
    blynkWritesEnabled: false,
    legacyWebhookMode: false,
    aiModeAvailable: Boolean(process.env.OPENAI_API_KEY && process.env.BRIDGE_SHARED_SECRET)
  });
});

app.get('/ready', (req, res) => {
  res.set('Cache-Control', 'no-store');
  res.json({
    ready: true,
    directEsp32Mode: true,
    model: MODEL,
    legacyWebhookReady: false
  });
});

// Optional advisory endpoint. The current firmware computes V0-V11 locally.
// This endpoint never writes to Blynk or changes the ESP32 decision.
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

// Old V9/V11/V12 routing conflicts with V9 trend text and V11 anomaly count.
// Retain the route with an explicit migration response; perform no cloud writes.
app.post('/blynk-webhook', (req, res) => {
  res.status(410).json({
    error: 'Legacy webhook retired for V0-V11. Use /esp32-analyse for advisory responses only.'
  });
});

export { app };
if (process.env.NODE_ENV !== 'test') app.listen(PORT, () => {
  console.log(`Blynk-Render bridge listening on port ${PORT}`);
  console.log('V0-V11: ESP32 owns all decisions; /esp32-analyse is advisory-only.');
  if (!process.env.OPENAI_API_KEY) {
    console.log('OPENAI_API_KEY not set: rule-based advisory fallback is active.');
  }

});
