import test from 'node:test';
import assert from 'node:assert/strict';
process.env.NODE_ENV = 'test';
delete process.env.OPENAI_API_KEY;
delete process.env.BRIDGE_SHARED_SECRET;
const { app } = await import('./server.js');
const server = app.listen(0, '127.0.0.1');
await new Promise(resolve => server.once('listening', resolve));
const base = 'http://127.0.0.1:' + server.address().port;
test.after(() => new Promise(resolve => server.close(resolve)));
async function advisory(body) {
  const response = await fetch(base + '/esp32-analyse', {
    method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(body)
  });
  assert.equal(response.status,200);
  const text = await response.text();
  assert.match(text,/^SUMMARY:.*\nACTION:.*\nMODE:RULE$/);
  return text;
}
test('health identifies architecture and disabled cloud writes', async () => {
  const data = await (await fetch(base+'/health')).json();
  assert.equal(data.architecture,'V0-V11');
  assert.equal(data.decisionOwner,'ESP32');
  assert.equal(data.blynkWritesEnabled,false);
  assert.equal(data.legacyWebhookMode,false);
});
test('normal, system fault, critical and four warning parameters', async () => {
  assert.match(await advisory({v4:'NORMAL',v5:'No abnormal condition',v9:'STABLE'}),/No inspection required/);
  assert.match(await advisory({v4:'SYSTEM FAULT',v5:'DHT11 Sensor Failure',v9:'STABLE'}),/DHT11 Sensor Failure/);
  assert.match(await advisory({v4:'CRITICAL',v5:'High Temperature + High Current'}),/Critical/);
  for (const parameter of ['Temperature','Humidity','Voltage','Current'])
    assert.match(await advisory({condition:'WARNING',reason:parameter}),new RegExp(parameter,'i'));
});
test('V9 supplies trend; V5 supplies cause; V8 and V10 are not interpreted as text', async () => {
  assert.match(await advisory({v4:'WARNING',v9:'HUMIDITY RISING',v8:1,v10:1}),/Humidity-related/);
  assert.match(await advisory({v4:'WARNING',v5:'High Current',v8:1,v10:1}),/Current-related/);
});
test('legacy webhook cannot overwrite V11 even with legacy configuration', async () => {
  process.env.BLYNK_DEVICE_TOKEN='TEST_ONLY';
  process.env.WEBHOOK_SECRET='TEST_ONLY';
  const response=await fetch(base+'/blynk-webhook',{method:'POST',
    headers:{'Content-Type':'application/json','x-webhook-secret':'TEST_ONLY'},
    body:JSON.stringify({pin:'V9',value:0})});
  assert.equal(response.status,410);
  assert.match((await response.json()).error,/retired/);
});
