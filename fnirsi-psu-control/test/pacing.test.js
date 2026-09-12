'use strict';

const test = require('node:test');
const assert = require('node:assert');
const { PowerSupply, MIN_FRAME_GAP_MS } = require('../src/main/device');

/**
 * Build a PowerSupply with a fake port that records write timestamps, so the
 * transmit pacing can be checked without hardware.
 */
function withFakePort() {
  const psu = new PowerSupply();
  const writes = [];
  psu.port = {
    isOpen: true,
    write(buf) {
      writes.push({ at: Date.now(), hex: Buffer.from(buf).toString('hex') });
    },
    drain(cb) {
      cb();
    },
    close(cb) {
      this.isOpen = false;
      cb();
    },
  };
  return { psu, writes };
}

/**
 * Regression test for the bug that made "output on" silently do nothing.
 *
 * The supply drops frames that arrive back-to-back. The original queue applied
 * its delay only *between items within a single drain pass* — but `_write`
 * drained synchronously, so the queue never held more than one frame and three
 * consecutive calls went out in the same millisecond. On hardware that turned
 * "set voltage, set current, output on" into "set voltage": the output stayed
 * off and the app reported it as off, with no error anywhere.
 */
test('consecutive _write calls are spaced, not bursted', async () => {
  const { psu, writes } = withFakePort();

  // Three separate synchronous calls — exactly what setOutput(true, …) does.
  psu.setVoltage(3.3);
  psu.setCurrent(0.5);
  psu.setOutput(true);

  await psu._flush();

  assert.strictEqual(writes.length, 3, 'all three frames should be written');

  for (let i = 1; i < writes.length; i += 1) {
    const gap = writes[i].at - writes[i - 1].at;
    assert.ok(
      gap >= MIN_FRAME_GAP_MS - 15, // timer granularity
      `frame ${i} came ${gap}ms after the previous, expected >= ${MIN_FRAME_GAP_MS}`,
    );
  }
});

test('setOutput(true, setpoints) sends voltage and current before enabling', async () => {
  const { psu, writes } = withFakePort();

  psu.setOutput(true, { voltage: 5, current: 1.5 });
  await psu._flush();

  assert.strictEqual(writes.length, 3);
  // 0xC1 set voltage, 0xC2 set current, 0xDB output enable — in that order.
  assert.ok(writes[0].hex.startsWith('f1b1c1'), `first was ${writes[0].hex}`);
  assert.ok(writes[1].hex.startsWith('f1b1c2'), `second was ${writes[1].hex}`);
  assert.strictEqual(writes[2].hex, 'f1b1db0101dd');
});

test('setOutput(false) does not resend setpoints', async () => {
  const { psu, writes } = withFakePort();

  psu.setOutput(false);
  await psu._flush();

  assert.strictEqual(writes.length, 1);
  assert.strictEqual(writes[0].hex, 'f1b1db0100dc');
});

test('a burst of protection writes is paced, and all of them go out', async () => {
  const { psu, writes } = withFakePort();

  psu.setProtections({ ovp: 30, ocp: 5, opp: 150, otp: 80, lvp: 1 });
  await psu._flush();

  assert.strictEqual(writes.length, 5, 'every limit should be written');
  const regs = writes.map((w) => w.hex.slice(4, 6));
  assert.deepStrictEqual(regs, ['d1', 'd2', 'd3', 'd4', 'd5']);
});

test('nothing is written once the port is closed', async () => {
  const { psu, writes } = withFakePort();

  psu.port.isOpen = false;
  psu.setVoltage(5);
  await psu._flush();

  assert.strictEqual(writes.length, 0);
});
