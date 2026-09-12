'use strict';

const test = require('node:test');
const assert = require('node:assert');
const p = require('../src/shared/protocol');

const hex = (buf) => Buffer.from(buf).toString('hex').toUpperCase();

/**
 * Every expected byte string below was derived from the vendor software's own
 * `Command.cs` constants (CmdMoudle(c1, c2, c3, c5)) with the checksum
 * recomputed by hand, so these tests pin the port to the original behaviour.
 */
test('command frames match the vendor software byte for byte', () => {
  const cases = [
    ['CMD_1  connect', p.cmd.connect(), 'F1C100010102'],
    ['CMD_2  disconnect', p.cmd.disconnect(), 'F1C100010001'],
    ['CMD_3  read all', p.cmd.readAll(), 'F1A1FF010000'],
    ['CMD_4  read input voltage', p.cmd.readInputVoltage(), 'F1A1C00100C1'],
    ['CMD_5  read model', p.cmd.readModel(), 'F1A1DE0100DF'],
    ['CMD_6  read firmware', p.cmd.readFirmware(), 'F1A1E00100E1'],
    ['CMD_7  read device address', p.cmd.readDeviceAddress(), 'F1A1E10100E2'],
    ['CMD_8  output off', p.cmd.setOutput(false), 'F1B1DB0100DC'],
    ['CMD_9  output on', p.cmd.setOutput(true), 'F1B1DB0101DD'],
    ['CMD_10 metering on', p.cmd.setMetering(true), 'F1B1D80101DA'],
    ['CMD_11 metering off', p.cmd.setMetering(false), 'F1B1D80100D9'],
    ['CMD_12 read brightness', p.cmd.readBrightness(), 'F1A1D60100D7'],
    ['CMD_14 read output', p.cmd.readOutput(), 'F1A1C30100C4'],
    ['CMD_15 bootloader', p.cmd.enterBootloader(), 'F1C000010102'],
    ['CMD_16 read hardware', p.cmd.readHardware(), 'F1A1DF0100E0'],
  ];
  for (const [name, actual, expected] of cases) {
    assert.strictEqual(hex(actual), expected, name);
  }
});

test('connect and bootloader frames are one bit apart with equal checksums', () => {
  // This is a hazard in the vendor protocol, not a bug in this port: the
  // checksum covers REG, LEN and payload but NOT the type byte, so a single
  // flipped bit turns "connect" into "enter bootloader" undetectably.
  // Documented here so nobody "simplifies" the two into one code path.
  const connect = hex(p.cmd.connect());
  const boot = hex(p.cmd.enterBootloader());
  assert.strictEqual(connect, 'F1C100010102');
  assert.strictEqual(boot, 'F1C000010102');
  assert.strictEqual(
    connect.slice(-2),
    boot.slice(-2),
    'identical checksums — the type byte is unprotected',
  );
  assert.strictEqual(p.TYPE.SESSION ^ p.TYPE.BOOTLOADER, 0x01);
});

test('CMD_13 encodes the baud rate as a 1-based index', () => {
  // Vendor: Command.CMD_13.setContent(indexOf(baud) + 1)
  // REG is 0x00, so the checksum is 0x00 + LEN(1) + n = n + 1.
  assert.strictEqual(hex(p.cmd.setBaudRate(9600)), 'F1B000010102');
  assert.strictEqual(hex(p.cmd.setBaudRate(19200)), 'F1B000010203');
  assert.strictEqual(hex(p.cmd.setBaudRate(115200)), 'F1B000010506');
  assert.throws(() => p.cmd.setBaudRate(230400), RangeError);
});

test('checksum covers REG, LEN and payload but not the start or type bytes', () => {
  // 5.0f little-endian is 00 00 A0 40.
  // 0xC1 + 0x04 + 0x00 + 0x00 + 0xA0 + 0x40 = 0x1A5 -> 0xA5
  const frame = p.cmd.setVoltage(5);
  assert.strictEqual(hex(frame), 'F1B1C1040000A040A5');
  assert.strictEqual(frame[frame.length - 1], p.checksum(0xc1, frame.subarray(4, 8)));
});

test('checksum wraps at 8 bits', () => {
  // readAll: 0xFF + 1 + 0 = 0x100 -> 0x00
  assert.strictEqual(p.cmd.readAll()[5], 0x00);
});

test('setpoints are little-endian float32', () => {
  // 1.5f little-endian is 00 00 C0 3F; 0xC2 + 0x04 + 0xC0 + 0x3F = 0x1C5 -> 0xC5
  assert.strictEqual(hex(p.cmd.setCurrent(1.5)), 'F1B1C2040000C03FC5');
  const f = p.cmd.setVoltage(12.34);
  assert.ok(Math.abs(f.subarray(4, 8).readFloatLE(0) - 12.34) < 1e-5);
});

test('memory group registers follow 0xC3 + 2*id + (isVoltage ? 0 : 1)', () => {
  const expected = [
    [1, 0xc5, 0xc6],
    [2, 0xc7, 0xc8],
    [3, 0xc9, 0xca],
    [4, 0xcb, 0xcc],
    [5, 0xcd, 0xce],
    [6, 0xcf, 0xd0],
  ];
  for (const [id, vReg, aReg] of expected) {
    assert.strictEqual(p.groupRegister(id, true), vReg, `M${id} voltage`);
    assert.strictEqual(p.groupRegister(id, false), aReg, `M${id} current`);
  }
  assert.throws(() => p.groupRegister(0, true), RangeError);
  assert.throws(() => p.groupRegister(7, true), RangeError);
});

test('brightness is clamped to the device range', () => {
  assert.strictEqual(p.cmd.setBrightness(0)[4], 1);
  assert.strictEqual(p.cmd.setBrightness(99)[4], 14);
  assert.strictEqual(p.cmd.setBrightness(7)[4], 7);
});

test('protection setters use float32 even for whole-degree OTP', () => {
  const frame = p.cmd.setOtp(80);
  assert.strictEqual(frame[2], p.REG.OTP);
  assert.strictEqual(frame[3], 4);
  assert.strictEqual(frame.subarray(4, 8).readFloatLE(0), 80);
});

/* -------------------------------------------------------------------------- */
/* Framing                                                                    */
/* -------------------------------------------------------------------------- */

test('splitFrames separates several frames packed into one read', () => {
  const joined = Buffer.concat([
    p.cmd.setOutput(true),
    p.cmd.readAll(),
    p.cmd.setVoltage(3.3),
  ]);
  const { frames, rest } = p.splitFrames(joined);
  assert.strictEqual(frames.length, 3);
  assert.strictEqual(rest.length, 0);
  assert.strictEqual(hex(frames[0]), hex(p.cmd.setOutput(true)));
  assert.strictEqual(hex(frames[2]), hex(p.cmd.setVoltage(3.3)));
});

test('splitFrames holds back a partial trailing frame', () => {
  const whole = p.cmd.setVoltage(5);
  const truncated = Buffer.concat([p.cmd.readAll(), whole.subarray(0, 5)]);
  const { frames, rest } = p.splitFrames(truncated);
  assert.strictEqual(frames.length, 1);
  assert.strictEqual(rest.length, 5);

  // Feeding the tail plus the remainder yields the complete second frame.
  const next = p.splitFrames(Buffer.concat([rest, whole.subarray(5)]));
  assert.strictEqual(next.frames.length, 1);
  assert.strictEqual(hex(next.frames[0]), hex(whole));
  assert.strictEqual(next.rest.length, 0);
});

test('splitFrames resynchronises after leading garbage', () => {
  const noisy = Buffer.concat([Buffer.from([0x00, 0xaa, 0x55]), p.cmd.readAll()]);
  const { frames } = p.splitFrames(noisy);
  assert.strictEqual(frames.length, 1);
  assert.strictEqual(hex(frames[0]), hex(p.cmd.readAll()));
});

test('parseFrame rejects a frame with a bad checksum', () => {
  const frame = Buffer.from(p.cmd.setOutput(true));
  frame[frame.length - 1] ^= 0xff;
  assert.strictEqual(p.parseFrame(frame), null);
});

/* -------------------------------------------------------------------------- */
/* Response decoding                                                          */
/* -------------------------------------------------------------------------- */

/** Build a device->host response frame for testing. */
function response(reg, payload) {
  return p.buildFrame(0xf0, reg, payload);
}

/**
 * Captured live from a DPS-150 (firmware V1.2). The device answers with start
 * byte 0xF0, not the 0xF1 the host sends — an earlier framer required 0xF1 and
 * silently discarded every reply, so the handshake never completed.
 */
const CAPTURE = Buffer.from(
  'f0a1c0044e15a14109f0a1e204b47b9f41f5f0a1e3043333a34030' +
    'f0a1c4048ebcd44127' +
    'f0a1d90400000000ddf0a1da0400000000de' +
    'f0a1dd0101df' +
    'f0a1dc0100dd' +
    'f0a1c30c000000000000000000000000cf',
  'hex',
);

test('device replies start with 0xF0 and decode from a live capture', () => {
  const { frames, rest } = p.splitFrames(CAPTURE);
  assert.strictEqual(rest.length, 0, 'capture should contain only whole frames');
  assert.strictEqual(frames.length, 9);
  assert.ok(
    frames.every((f) => f[0] === p.START_DEVICE),
    'every captured frame starts with 0xF0',
  );

  const state = frames.reduce((acc, f) => Object.assign(acc, p.parseFrame(f)), {});
  assert.ok(Math.abs(state.inputVoltage - 20.135) < 0.01);
  assert.ok(Math.abs(state.maxVoltage - 19.936) < 0.01);
  assert.ok(Math.abs(state.maxCurrent - 5.1) < 0.001);
  assert.ok(Math.abs(state.temperature - 26.59) < 0.05);
  assert.strictEqual(state.ampHours, 0);
  assert.strictEqual(state.wattHours, 0);
  assert.strictEqual(state.regulationMode, 'CV');
  assert.strictEqual(state.protection, 'normal');
  assert.strictEqual(state.outputVoltage, 0);
});

test('both start bytes are accepted, anything else resynchronises', () => {
  assert.ok(p.isStartByte(0xf0));
  assert.ok(p.isStartByte(0xf1));
  assert.ok(!p.isStartByte(0xf2));
});

test('a live 0xE1 address reply decodes to the front-panel value', () => {
  // Captured with the supply's display showing 023.
  const frame = Buffer.from('f0a1e10117f9', 'hex');
  assert.ok(p.verifyChecksum(frame));
  assert.deepStrictEqual(p.parseFrame(frame), { deviceAddress: 23 });
});

test('a live 0xFF blob from a DPS-150 decodes to plausible values', () => {
  const blob = Buffer.from(
    'f0a1ff8b2d18a141000090413333a3400000000000000000000000000043d641' +
      '0000a0400000803fd73253400000803f000040410000a040000090413333a340' +
      '0000c0413333a3400000a0400000803f0000f8413333a340000016430000a042' +
      '0000a0400b0f00000000000000000000000117937e9f413333a3400000f04133' +
      '33a340000016430000c6420000f04198',
    'hex',
  );
  const state = p.parseFrame(blob);
  assert.ok(state, 'live blob should parse');
  assert.strictEqual(state.model, undefined, 'blob carries no model string');
  assert.ok(Math.abs(state.inputVoltage - 20.138) < 0.01);
  assert.strictEqual(state.setVoltage, 18);
  assert.ok(Math.abs(state.setCurrent - 5.1) < 0.001);
  assert.strictEqual(state.groups.length, 6);
  assert.strictEqual(state.groups[0].voltage, 5);
  assert.strictEqual(state.groups[0].current, 1);
  assert.strictEqual(state.groups[2].voltage, 12);
  assert.strictEqual(state.groups[2].current, 5);
  assert.strictEqual(state.ovp, 31);
  assert.strictEqual(state.opp, 150);
  assert.strictEqual(state.otp, 80);
  assert.strictEqual(state.lvp, 5);
  assert.strictEqual(state.brightness, 11);
  assert.strictEqual(state.metering, false);
  assert.strictEqual(state.outputEnabled, false);
  assert.strictEqual(state.protection, 'normal');
  assert.strictEqual(state.regulationMode, 'CV');
  assert.strictEqual(state.ovpMax, 30);
  assert.strictEqual(state.otpMax, 99);
  assert.strictEqual(state.lvpMax, 30);
});

test('parseFrame decodes scalar registers', () => {
  const vip = Buffer.alloc(12);
  vip.writeFloatLE(12.5, 0);
  vip.writeFloatLE(1.25, 4);
  vip.writeFloatLE(15.625, 8);

  assert.deepStrictEqual(p.parseFrame(response(p.REG.OUTPUT_VIP, vip)), {
    outputVoltage: 12.5,
    outputCurrent: 1.25,
    outputPower: 15.625,
  });

  const temp = Buffer.alloc(4);
  temp.writeFloatLE(42, 0);
  assert.deepStrictEqual(p.parseFrame(response(p.REG.TEMPERATURE, temp)), {
    temperature: 42,
  });

  assert.deepStrictEqual(p.parseFrame(response(p.REG.OUTPUT_ENABLE, [1])), {
    outputEnabled: true,
  });
  assert.deepStrictEqual(p.parseFrame(response(p.REG.DEVICE_ADDRESS, [7])), {
    deviceAddress: 7,
  });
});

test('regulation mode: 1 is CV, 0 is CC', () => {
  // The vendor getData32String maps 1 -> "Constant Voltage", 0 -> "Constant Current".
  assert.deepStrictEqual(p.parseFrame(response(p.REG.REGULATION_MODE, [1])), {
    regulationMode: 'CV',
  });
  assert.deepStrictEqual(p.parseFrame(response(p.REG.REGULATION_MODE, [0])), {
    regulationMode: 'CC',
  });
});

test('protection status codes map to the vendor labels', () => {
  const expected = ['normal', 'ovp', 'ocp', 'opp', 'otp', 'lvp'];
  expected.forEach((label, code) => {
    assert.deepStrictEqual(p.parseFrame(response(p.REG.PROTECTION_STATUS, [code])), {
      protection: label,
    });
  });
  assert.deepStrictEqual(p.parseFrame(response(p.REG.PROTECTION_STATUS, [9])), {
    protection: 'unknown',
  });
});

test('string registers decode as NUL-trimmed ASCII', () => {
  const payload = Buffer.concat([Buffer.from('DPS-150', 'ascii'), Buffer.from([0, 0])]);
  assert.deepStrictEqual(p.parseFrame(response(p.REG.MODEL, payload)), {
    model: 'DPS-150',
  });
});

test('full-status blob decodes every documented offset', () => {
  const blob = Buffer.alloc(140);
  const put = (offset, value) => blob.writeFloatLE(value, offset);

  put(0, 19.8); // input voltage
  put(4, 5.0); // V setpoint
  put(8, 2.0); // I setpoint
  put(12, 4.99); // V out
  put(16, 1.234); // I out
  put(20, 6.16); // P out
  put(24, 31); // temperature
  for (let i = 0; i < 6; i += 1) {
    put(28 + i * 8, i + 1); // group voltage
    put(32 + i * 8, (i + 1) / 10); // group current
  }
  put(76, 30); // OVP
  put(80, 5.1); // OCP
  put(84, 155); // OPP
  put(88, 80); // OTP
  put(92, 1.5); // LVP
  blob[96] = 9; // brightness
  blob[98] = 1; // metering on
  put(99, 0.125); // Ah
  put(103, 0.75); // Wh
  blob[107] = 1; // output on
  blob[108] = 2; // OCP tripped
  blob[109] = 0; // CC
  put(111, 30); // max voltage
  put(115, 5); // max current
  put(119, 30); // OVP max
  put(123, 5); // OCP max
  put(127, 160); // OPP max
  put(131, 100); // OTP max
  put(135, 30); // LVP max

  const state = p.parseFrame(response(p.REG.ALL, blob));
  assert.ok(state, 'blob should parse');
  assert.ok(Math.abs(state.inputVoltage - 19.8) < 1e-4);
  assert.strictEqual(state.setVoltage, 5);
  assert.strictEqual(state.setCurrent, 2);
  assert.ok(Math.abs(state.outputCurrent - 1.234) < 1e-5);
  assert.strictEqual(state.temperature, 31);
  assert.strictEqual(state.groups.length, 6);
  assert.strictEqual(state.groups[0].voltage, 1);
  assert.ok(Math.abs(state.groups[5].current - 0.6) < 1e-6);
  assert.strictEqual(state.ovp, 30);
  assert.strictEqual(state.otp, 80);
  assert.strictEqual(state.brightness, 9);
  assert.strictEqual(state.metering, true);
  assert.strictEqual(state.ampHours, 0.125);
  assert.strictEqual(state.wattHours, 0.75);
  assert.strictEqual(state.outputEnabled, true);
  assert.strictEqual(state.protection, 'ocp');
  assert.strictEqual(state.regulationMode, 'CC');
  assert.strictEqual(state.maxVoltage, 30);
  assert.strictEqual(state.maxCurrent, 5);
  assert.strictEqual(state.oppMax, 160);
});

test('a truncated full-status blob is rejected rather than misread', () => {
  assert.strictEqual(p.parseFrame(response(p.REG.ALL, Buffer.alloc(100))), null);
});

test('unknown registers are ignored', () => {
  assert.strictEqual(p.parseFrame(response(0x99, [1, 2, 3])), null);
});

test('a round trip through splitFrames and parseFrame reconstructs state', () => {
  const stream = Buffer.concat([
    response(p.REG.OUTPUT_ENABLE, [1]),
    response(p.REG.REGULATION_MODE, [1]),
    response(p.REG.PROTECTION_STATUS, [0]),
  ]);
  const { frames } = p.splitFrames(stream);
  const state = frames.reduce((acc, f) => Object.assign(acc, p.parseFrame(f)), {});
  assert.deepStrictEqual(state, {
    outputEnabled: true,
    regulationMode: 'CV',
    protection: 'normal',
  });
});
