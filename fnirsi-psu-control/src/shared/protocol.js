'use strict';

/**
 * FNIRSI DC power supply serial protocol.
 *
 * Frame layout, the same in both directions apart from the start byte:
 *
 *   START | TYPE | REG | LEN | PAYLOAD[LEN] | CKS
 *
 * where CKS = (REG + LEN + sum(PAYLOAD)) & 0xFF. The start byte and TYPE are
 * deliberately excluded from the checksum. See docs/PROTOCOL.md.
 *
 * The host sends 0xF1 and the device answers with 0xF0. The vendor software
 * never checked the start byte at all — it walked the buffer using the length
 * field alone — so the distinction is undocumented there but is plainly
 * visible on the wire.
 *
 * This module is pure: no I/O, no Electron. It is exercised directly by
 * test/protocol.test.js.
 */

/** Start byte on frames the host sends. */
const START = 0xf1;
/** Start byte on frames the device sends. */
const START_DEVICE = 0xf0;

/** True for either direction's start byte. */
const isStartByte = (b) => b === START || b === START_DEVICE;

/** Frame type byte (second byte of every frame). */
const TYPE = {
  SESSION: 0xc1, // connect / disconnect
  BOOTLOADER: 0xc0, // enter firmware upgrade mode
  BAUD: 0xb0, // set baud rate
  READ: 0xa1, // read register
  WRITE: 0xb1, // write register
};

/** Register / command ids. */
const REG = {
  SESSION: 0x00,
  INPUT_VOLTAGE: 0xc0,
  SET_VOLTAGE: 0xc1,
  SET_CURRENT: 0xc2,
  OUTPUT_VIP: 0xc3, // V, I, P together
  TEMPERATURE: 0xc4,
  GROUP_BASE: 0xc3, // memory groups start here; see groupRegister()
  OVP: 0xd1,
  OCP: 0xd2,
  OPP: 0xd3,
  OTP: 0xd4,
  LVP: 0xd5,
  BRIGHTNESS: 0xd6,
  METERING: 0xd8,
  AMP_HOURS: 0xd9,
  WATT_HOURS: 0xda,
  OUTPUT_ENABLE: 0xdb,
  PROTECTION_STATUS: 0xdc,
  REGULATION_MODE: 0xdd,
  MODEL: 0xde,
  HARDWARE: 0xdf,
  FIRMWARE: 0xe0,
  DEVICE_ADDRESS: 0xe1,
  MAX_VOLTAGE: 0xe2,
  MAX_CURRENT: 0xe3,
  ALL: 0xff,
};

/** Baud rates the device accepts, in the order the protocol indexes them. */
const BAUD_RATES = [9600, 19200, 38400, 57600, 115200];

/** Protection status codes reported in register 0xDC and blob offset 108. */
const PROTECTION = {
  0: 'normal',
  1: 'ovp',
  2: 'ocp',
  3: 'opp',
  4: 'otp',
  5: 'lvp',
};

/** Number of memory (quick-recall) groups. */
const GROUP_COUNT = 6;

/** Display brightness range accepted by register 0xD6. */
const BRIGHTNESS_MIN = 1;
const BRIGHTNESS_MAX = 14;

/** Limits the device reports before it has told us its real ones. */
const DEFAULT_MAX_VOLTAGE = 24;
const DEFAULT_MAX_CURRENT = 5;

/**
 * Register holding one half of a memory group.
 * Mirrors `0xC3 + 2 * id + (isVoltage ? 0 : 1)` from the vendor software.
 *
 * @param {number} id 1-based group number, 1..6
 * @param {boolean} isVoltage true for the voltage half, false for current
 */
function groupRegister(id, isVoltage) {
  if (!Number.isInteger(id) || id < 1 || id > GROUP_COUNT) {
    throw new RangeError(`memory group must be 1..${GROUP_COUNT}, got ${id}`);
  }
  return (REG.GROUP_BASE + 2 * id + (isVoltage ? 0 : 1)) & 0xff;
}

/** Checksum over REG, LEN and the payload. */
function checksum(reg, payload) {
  let sum = reg + payload.length;
  for (const b of payload) sum += b;
  return sum & 0xff;
}

/**
 * Build a complete frame.
 *
 * @param {number} type one of TYPE.*
 * @param {number} reg register id
 * @param {Buffer|number[]|number} payload payload bytes, or a single byte
 * @returns {Buffer}
 */
function buildFrame(type, reg, payload) {
  const body = Buffer.isBuffer(payload)
    ? payload
    : Buffer.from(typeof payload === 'number' ? [payload] : payload);
  if (body.length > 0xff) {
    throw new RangeError(`payload too long: ${body.length} bytes`);
  }
  return Buffer.concat([
    Buffer.from([START, type, reg, body.length]),
    body,
    Buffer.from([checksum(reg, body)]),
  ]);
}

/** Little-endian float32 as a 4-byte buffer. */
function float32(value) {
  const b = Buffer.allocUnsafe(4);
  b.writeFloatLE(value, 0);
  return b;
}

/* -------------------------------------------------------------------------- */
/* Host -> device commands                                                    */
/* -------------------------------------------------------------------------- */

const cmd = {
  /** Hand control to the PC. Sent immediately after opening the port. */
  connect: () => buildFrame(TYPE.SESSION, REG.SESSION, 1),

  /** Return control to the front panel. Sent before closing the port. */
  disconnect: () => buildFrame(TYPE.SESSION, REG.SESSION, 0),

  /** Enter the bootloader for a firmware upgrade. */
  enterBootloader: () => buildFrame(TYPE.BOOTLOADER, REG.SESSION, 1),

  /**
   * Tell the device which baud rate to use.
   * @param {number} baud one of BAUD_RATES
   */
  setBaudRate(baud) {
    const index = BAUD_RATES.indexOf(baud);
    if (index < 0) throw new RangeError(`unsupported baud rate ${baud}`);
    return buildFrame(TYPE.BAUD, REG.SESSION, index + 1);
  },

  /**
   * Read a register. Note the payload is a single zero byte, not empty —
   * the device rejects zero-length read requests.
   */
  read: (reg) => buildFrame(TYPE.READ, reg, 0),

  readAll: () => cmd.read(REG.ALL),
  readModel: () => cmd.read(REG.MODEL),
  readFirmware: () => cmd.read(REG.FIRMWARE),
  readHardware: () => cmd.read(REG.HARDWARE),
  readDeviceAddress: () => cmd.read(REG.DEVICE_ADDRESS),
  readBrightness: () => cmd.read(REG.BRIGHTNESS),
  readInputVoltage: () => cmd.read(REG.INPUT_VOLTAGE),
  readOutput: () => cmd.read(REG.OUTPUT_VIP),

  setVoltage: (volts) => buildFrame(TYPE.WRITE, REG.SET_VOLTAGE, float32(volts)),
  setCurrent: (amps) => buildFrame(TYPE.WRITE, REG.SET_CURRENT, float32(amps)),

  /** @param {boolean} on */
  setOutput: (on) => buildFrame(TYPE.WRITE, REG.OUTPUT_ENABLE, on ? 1 : 0),

  /** Ah/Wh accumulation. @param {boolean} on */
  setMetering: (on) => buildFrame(TYPE.WRITE, REG.METERING, on ? 1 : 0),

  /** @param {number} level 1..14 */
  setBrightness(level) {
    const clamped = Math.min(
      BRIGHTNESS_MAX,
      Math.max(BRIGHTNESS_MIN, Math.round(level)),
    );
    return buildFrame(TYPE.WRITE, REG.BRIGHTNESS, clamped);
  },

  setOvp: (volts) => buildFrame(TYPE.WRITE, REG.OVP, float32(volts)),
  setOcp: (amps) => buildFrame(TYPE.WRITE, REG.OCP, float32(amps)),
  setOpp: (watts) => buildFrame(TYPE.WRITE, REG.OPP, float32(watts)),
  /** Transmitted as a float even though the UI edits whole degrees. */
  setOtp: (celsius) => buildFrame(TYPE.WRITE, REG.OTP, float32(celsius)),
  setLvp: (volts) => buildFrame(TYPE.WRITE, REG.LVP, float32(volts)),

  /**
   * Store one half of a memory group.
   * @param {number} id 1..6
   * @param {boolean} isVoltage
   * @param {number} value
   */
  setGroup: (id, isVoltage, value) =>
    buildFrame(TYPE.WRITE, groupRegister(id, isVoltage), float32(value)),
};

/* -------------------------------------------------------------------------- */
/* Device -> host parsing                                                     */
/* -------------------------------------------------------------------------- */

/**
 * Split a raw read into whole frames.
 *
 * The device may pack several frames into one USB transfer, and a transfer may
 * end mid-frame. Returns the frames it could fully decode plus whatever tail
 * bytes remain, which the caller must prepend to the next read.
 *
 * Resynchronises by discarding bytes until a plausible start byte is found, so
 * a corrupted frame costs one frame rather than the whole stream.
 *
 * @param {Buffer} buffer
 * @returns {{frames: Buffer[], rest: Buffer}}
 */
function splitFrames(buffer) {
  const frames = [];
  let i = 0;

  while (i < buffer.length) {
    if (!isStartByte(buffer[i])) {
      i += 1; // resynchronise
      continue;
    }
    if (i + 4 > buffer.length) break; // header incomplete
    const total = buffer[i + 3] + 5;
    if (i + total > buffer.length) break; // payload incomplete
    frames.push(buffer.subarray(i, i + total));
    i += total;
  }

  return { frames, rest: buffer.subarray(i) };
}

/** True when a frame's trailing checksum byte matches its contents. */
function verifyChecksum(frame) {
  const payload = frame.subarray(4, frame.length - 1);
  return checksum(frame[2], payload) === frame[frame.length - 1];
}

/** Trim at the first NUL and decode as ASCII. */
function decodeString(buf) {
  const end = buf.indexOf(0);
  return buf.subarray(0, end === -1 ? buf.length : end).toString('ascii').trim();
}

/**
 * Decode the 0xFF full-status blob into a flat state object.
 * Offsets are transcribed from the vendor `SerialData.setAllData`.
 *
 * @param {Buffer} p payload (>= 139 bytes)
 */
function parseFullStatus(p) {
  const f = (offset) => p.readFloatLE(offset);
  const groups = [];
  for (let i = 0; i < GROUP_COUNT; i += 1) {
    groups.push({ id: i + 1, voltage: f(28 + i * 8), current: f(32 + i * 8) });
  }
  return {
    inputVoltage: f(0),
    setVoltage: f(4),
    setCurrent: f(8),
    outputVoltage: f(12),
    outputCurrent: f(16),
    outputPower: f(20),
    temperature: f(24),
    groups,
    ovp: f(76),
    ocp: f(80),
    opp: f(84),
    otp: f(88),
    lvp: f(92),
    brightness: p[96],
    metering: p[98] !== 0,
    ampHours: f(99),
    wattHours: f(103),
    outputEnabled: p[107] !== 0,
    protection: PROTECTION[p[108]] ?? 'unknown',
    regulationMode: p[109] === 1 ? 'CV' : 'CC',
    maxVoltage: f(111),
    maxCurrent: f(115),
    ovpMax: f(119),
    ocpMax: f(123),
    oppMax: f(127),
    otpMax: f(131),
    lvpMax: f(135),
  };
}

/**
 * Decode one frame into a partial state object.
 *
 * The vendor software dispatches purely on the register byte and ignores the
 * type byte on receive, so this does the same. Returns null for registers we
 * do not model; returns `{}` for a frame that is valid but carries nothing.
 *
 * @param {Buffer} frame a complete frame as returned by splitFrames()
 * @returns {object|null}
 */
function parseFrame(frame) {
  if (frame.length < 5 || !isStartByte(frame[0])) return null;
  if (!verifyChecksum(frame)) return null;

  const reg = frame[2];
  const len = frame[3];
  const p = frame.subarray(4, 4 + len);

  switch (reg) {
    case REG.INPUT_VOLTAGE:
      return len >= 4 ? { inputVoltage: p.readFloatLE(0) } : null;

    case REG.SET_VOLTAGE:
      return len >= 4 ? { setVoltage: p.readFloatLE(0) } : null;

    case REG.SET_CURRENT:
      return len >= 4 ? { setCurrent: p.readFloatLE(0) } : null;

    case REG.OUTPUT_VIP:
      return len >= 12
        ? {
            outputVoltage: p.readFloatLE(0),
            outputCurrent: p.readFloatLE(4),
            outputPower: p.readFloatLE(8),
          }
        : null;

    case REG.TEMPERATURE:
      return len >= 4 ? { temperature: p.readFloatLE(0) } : null;

    case REG.AMP_HOURS:
      return len >= 4 ? { ampHours: p.readFloatLE(0) } : null;

    case REG.WATT_HOURS:
      return len >= 4 ? { wattHours: p.readFloatLE(0) } : null;

    case REG.OUTPUT_ENABLE:
      return len >= 1 ? { outputEnabled: p[0] !== 0 } : null;

    case REG.METERING:
      return len >= 1 ? { metering: p[0] !== 0 } : null;

    case REG.BRIGHTNESS:
      return len >= 1 ? { brightness: p[0] } : null;

    case REG.PROTECTION_STATUS:
      return len >= 1 ? { protection: PROTECTION[p[0]] ?? 'unknown' } : null;

    case REG.REGULATION_MODE:
      return len >= 1 ? { regulationMode: p[0] === 1 ? 'CV' : 'CC' } : null;

    case REG.MODEL:
      return { model: decodeString(p) };

    case REG.HARDWARE:
      return { hardware: decodeString(p) };

    case REG.FIRMWARE:
      return { firmware: decodeString(p) };

    case REG.DEVICE_ADDRESS:
      return len >= 1 ? { deviceAddress: p[0] } : null;

    case REG.MAX_VOLTAGE:
      return len >= 4 ? { maxVoltage: p.readFloatLE(0) } : null;

    case REG.MAX_CURRENT:
      return len >= 4 ? { maxCurrent: p.readFloatLE(0) } : null;

    case REG.ALL:
      return len >= 139 ? parseFullStatus(p) : null;

    default:
      return null;
  }
}

module.exports = {
  START,
  START_DEVICE,
  isStartByte,
  TYPE,
  REG,
  BAUD_RATES,
  PROTECTION,
  GROUP_COUNT,
  BRIGHTNESS_MIN,
  BRIGHTNESS_MAX,
  DEFAULT_MAX_VOLTAGE,
  DEFAULT_MAX_CURRENT,
  buildFrame,
  checksum,
  groupRegister,
  splitFrames,
  verifyChecksum,
  parseFrame,
  parseFullStatus,
  cmd,
};
