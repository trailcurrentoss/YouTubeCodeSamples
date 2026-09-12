'use strict';

const { EventEmitter } = require('node:events');
const { SerialPort } = require('serialport');
const proto = require('../shared/protocol');

/** How long to wait for each device-address probe during the handshake. */
const PROBE_INTERVAL_MS = 1000;
/** How many probes before giving up. Matches the vendor software. */
const PROBE_ATTEMPTS = 2;

/**
 * Minimum gap between outgoing frames.
 *
 * The supply drops commands that arrive back-to-back — it is a small MCU that
 * is already busy streaming measurements. Sending the five init reads in a
 * burst reliably loses all but the first, leaving the model, firmware and
 * memory groups blank. Spacing them out fixes it. 120 ms was the smallest gap
 * that read back cleanly on a DPS-150 with firmware V1.2.
 */
const MIN_FRAME_GAP_MS = 120;

/**
 * Quiet period after the set-baud-rate command.
 *
 * The supply reinitialises its serial handling when it receives this and
 * ignores everything for a few hundred milliseconds afterwards. The vendor
 * software sends the five init commands back-to-back and simply loses the four
 * that follow the baud command — which is why its Product Model and Firmware
 * Version fields are so often blank. Waiting here is what makes them populate.
 */
const BAUD_SETTLE_MS = 600;

/** How many times to re-ask for identity/state if the replies do not arrive. */
const INIT_ATTEMPTS = 3;

/**
 * Owns the serial port and speaks the protocol.
 *
 * Emits:
 *   'state'       — partial state patch decoded from the device
 *   'status'      — {connected, port, baudRate, message}
 *   'protection'  — protection code string, when it changes to a tripped state
 *   'error'       — Error
 *
 * The device pushes measurement frames on its own once connected; the vendor
 * software never polls on a timer and neither do we. A slow keepalive read is
 * available via `startPolling()` for setups where the stream stalls.
 */
class PowerSupply extends EventEmitter {
  constructor() {
    super();
    this.port = null;
    this.rxBuffer = Buffer.alloc(0);
    this.connected = false;
    this.deviceAddress = 1;
    /** Address the device actually reported, whether or not it matched. */
    this.observedAddress = null;
    this.pollTimer = null;
    /** Outgoing frames awaiting their turn; see MIN_FRAME_GAP_MS. */
    this.txQueue = [];
    this.draining = false;
    /** Timestamp of the last frame written, so gaps span separate _write calls. */
    this.lastWriteAt = 0;
    /** Last known full state, so a late-joining renderer can be caught up. */
    this.state = {
      model: '',
      firmware: '',
      hardware: '',
      maxVoltage: proto.DEFAULT_MAX_VOLTAGE,
      maxCurrent: proto.DEFAULT_MAX_CURRENT,
      protection: 'normal',
      regulationMode: 'CV',
      outputEnabled: false,
      metering: false,
    };
  }

  /** List candidate serial ports, most-likely-first. */
  static async listPorts() {
    const ports = await SerialPort.list();
    return ports.map((p) => ({
      path: p.path,
      manufacturer: p.manufacturer || '',
      serialNumber: p.serialNumber || '',
      vendorId: p.vendorId || '',
      productId: p.productId || '',
    }));
  }

  get isOpen() {
    return Boolean(this.port && this.port.isOpen);
  }

  /**
   * Open the port and run the vendor handshake.
   *
   * @param {string} path serial port path
   * @param {number} baudRate one of proto.BAUD_RATES
   * @param {number} deviceAddress expected device address, 1..255. Pass null
   *   to accept whatever address the supply reports (auto-detect).
   * @returns {Promise<boolean>} true once the device has answered with a
   *   matching address; false if it never did (the port is closed again).
   */
  async connect(path, baudRate, deviceAddress = 1) {
    await this.disconnect();

    this.deviceAddress = deviceAddress;
    this.observedAddress = null;
    this.rxBuffer = Buffer.alloc(0);

    // Clear identity so a reconnect re-reads it instead of showing stale or
    // half-filled values from the previous session.
    this.state.model = '';
    this.state.firmware = '';
    this.state.hardware = '';
    delete this.state.groups;

    await new Promise((resolve, reject) => {
      this.port = new SerialPort({
        path,
        baudRate,
        dataBits: 8,
        parity: 'none',
        stopBits: 1,
        rtscts: false,
        // Assert the modem lines as part of opening rather than toggling them
        // afterwards. The vendor software sets RtsEnable = true; doing it here
        // avoids a post-open DTR/RTS transition, which on USB-CDC supplies can
        // be interpreted as a bootloader-entry signal.
        dtr: true,
        rts: true,
        autoOpen: false,
      });
      this.port.open((err) => (err ? reject(err) : resolve()));
    });

    this.port.on('data', (chunk) => this._onData(chunk));
    this.port.on('error', (err) => this.emit('error', err));
    this.port.on('close', () => {
      if (this.connected) {
        this.connected = false;
        this._emitStatus('Port closed');
      }
    });

    this.emit('status', {
      connected: false,
      port: path,
      baudRate,
      message: 'Connecting…',
    });

    this._write(proto.cmd.connect());

    const matched = await this._probeForDevice();
    if (!matched) {
      const seen = this.observedAddress;
      await this.disconnect(/* notifyDevice */ false);
      // A mismatch is the common failure and the vendor software gave no clue
      // about it. If the supply answered at all, say which address it used.
      this._emitStatus(
        seen === null
          ? 'Connection failed — no response from the device'
          : `Connection failed — device address is ${seen}, not ${deviceAddress}`,
      );
      return false;
    }

    this.connected = true;
    this._emitStatus('Connection successful');

    await this._runInitSequence(baudRate);
    return true;
  }

  /**
   * Ask the supply to identify itself and report its full state.
   *
   * Vendor order is baud, model, firmware, hardware string, read-all — but the
   * baud command deafens the device briefly, so we wait it out and then retry
   * until the answers actually arrive rather than assuming they did.
   */
  async _runInitSequence(baudRate) {
    this._write(proto.cmd.setBaudRate(baudRate));
    await this._flush();
    await new Promise((r) => setTimeout(r, BAUD_SETTLE_MS));

    for (let attempt = 0; attempt < INIT_ATTEMPTS; attempt += 1) {
      if (!this.isOpen) return;

      // Only re-ask for what is still missing.
      if (!this.state.model) this._write(proto.cmd.readModel());
      if (!this.state.firmware) this._write(proto.cmd.readFirmware());
      if (!this.state.hardware) this._write(proto.cmd.readHardware());
      if (!this.state.groups) this._write(proto.cmd.readAll());

      await this._flush();
      await new Promise((r) => setTimeout(r, 400));

      if (this.state.model && this.state.firmware && this.state.groups) return;
    }
  }

  /**
   * Send the address probe up to PROBE_ATTEMPTS times and wait for a matching
   * 0xE1 response. Resolves true on a match, false on timeout.
   */
  _probeForDevice() {
    return new Promise((resolve) => {
      let attempts = 0;
      let settled = false;

      const onState = (patch) => {
        if (patch.deviceAddress === undefined) return;
        this.observedAddress = patch.deviceAddress;
        // A null expected address means "accept whatever answers".
        if (this.deviceAddress === null) {
          this.deviceAddress = patch.deviceAddress;
          finish(true);
          return;
        }
        // The supply's address range is 1..255, matching the single wire byte.
        if (patch.deviceAddress !== this.deviceAddress) return;
        finish(true);
      };

      const finish = (result) => {
        if (settled) return;
        settled = true;
        clearInterval(timer);
        this.off('state', onState);
        resolve(result);
      };

      this.on('state', onState);

      const probe = () => {
        if (settled) return;
        if (attempts >= PROBE_ATTEMPTS) {
          finish(false);
          return;
        }
        attempts += 1;
        this._write(proto.cmd.readDeviceAddress());
      };

      probe();
      const timer = setInterval(probe, PROBE_INTERVAL_MS);
    });
  }

  /**
   * Close the port, handing control back to the front panel first.
   * @param {boolean} notifyDevice send the disconnect command before closing
   */
  async disconnect(notifyDevice = true) {
    this.stopPolling();
    if (!this.isOpen) {
      this.port = null;
      this.connected = false;
      return;
    }
    if (notifyDevice) {
      try {
        // Drop anything still queued — we are leaving — then hand control back
        // to the front panel before the port goes away.
        this.txQueue.length = 0;
        const wait = this.lastWriteAt + MIN_FRAME_GAP_MS - Date.now();
        if (wait > 0) await new Promise((r) => setTimeout(r, wait));
        this.port.write(proto.cmd.disconnect());
        await new Promise((resolve) => this.port.drain(() => resolve()));
      } catch {
        // The port may already be gone; closing is what matters.
      }
    } else {
      this.txQueue.length = 0;
    }
    await new Promise((resolve) => this.port.close(() => resolve()));
    this.port = null;
    this.connected = false;
    this._emitStatus('Disconnected');
  }

  /**
   * Ask for a full state refresh on an interval. Off by default — the device
   * streams updates unprompted.
   * @param {number} intervalMs
   */
  startPolling(intervalMs = 1000) {
    this.stopPolling();
    this.pollTimer = setInterval(() => {
      if (this.connected) this._write(proto.cmd.readAll());
    }, intervalMs);
  }

  stopPolling() {
    if (this.pollTimer) {
      clearInterval(this.pollTimer);
      this.pollTimer = null;
    }
  }

  /* ---------------------------------------------------------------------- */
  /* Commands                                                               */
  /* ---------------------------------------------------------------------- */

  setVoltage(volts) {
    this._write(proto.cmd.setVoltage(volts));
  }

  setCurrent(amps) {
    this._write(proto.cmd.setCurrent(amps));
  }

  /**
   * Enable or disable the output. Turning it on re-sends both setpoints
   * first, which is what the vendor software does.
   */
  setOutput(on, setpoints) {
    if (on && setpoints) {
      this._write(proto.cmd.setVoltage(setpoints.voltage));
      this._write(proto.cmd.setCurrent(setpoints.current));
    }
    this._write(proto.cmd.setOutput(on));
  }

  setMetering(on) {
    this._write(proto.cmd.setMetering(on));
  }

  setBrightness(level) {
    this._write(proto.cmd.setBrightness(level));
  }

  /** @param {{ovp?:number, ocp?:number, opp?:number, otp?:number, lvp?:number}} limits */
  setProtections(limits) {
    const map = {
      ovp: proto.cmd.setOvp,
      ocp: proto.cmd.setOcp,
      opp: proto.cmd.setOpp,
      otp: proto.cmd.setOtp,
      lvp: proto.cmd.setLvp,
    };
    for (const [key, build] of Object.entries(map)) {
      if (limits[key] !== undefined && limits[key] !== null) {
        this._write(build(limits[key]));
      }
    }
  }

  /** Store a memory group and read the state back, as the vendor UI does. */
  saveGroup(id, voltage, current) {
    this._write(proto.cmd.setGroup(id, true, voltage));
    this._write(proto.cmd.setGroup(id, false, current));
    this._write(proto.cmd.readAll());
  }

  readAll() {
    this._write(proto.cmd.readAll());
  }

  /**
   * Put the device into its USB bootloader, where it enumerates as a mass
   * storage volume and waits for a firmware file. It stops responding to this
   * protocol until it is power-cycled.
   *
   * This is never sent as a side effect of anything. The IPC layer refuses to
   * call it unless the user has already picked a firmware file, so the command
   * cannot be reached by a stray click.
   *
   * Note for anyone editing the codec: this frame is one bit away from the
   * connect frame, and the protocol cannot tell them apart:
   *
   *   connect     F1 C1 00 01 01 02
   *   bootloader  F1 C0 00 01 01 02
   *
   * The checksum covers only REG, LEN and payload — not the type byte — so
   * both carry the identical checksum 0x02. Keep the two paths distinct.
   */
  enterBootloader() {
    this._write(proto.cmd.enterBootloader());
  }

  /* ---------------------------------------------------------------------- */
  /* Internals                                                              */
  /* ---------------------------------------------------------------------- */

  /**
   * Queue a frame for transmission.
   *
   * Everything goes through the queue so that no caller can accidentally
   * burst-write and lose commands. See MIN_FRAME_GAP_MS.
   */
  _write(frame) {
    if (!this.isOpen) return false;
    this.txQueue.push(frame);
    this._drainQueue();
    return true;
  }

  /**
   * Write queued frames, never closer together than MIN_FRAME_GAP_MS.
   *
   * The gap is measured from the last frame actually written, not from the
   * start of this pass. An earlier version waited only between items within a
   * single drain, which did nothing: `_write` drains synchronously, so the
   * queue was always emptied one frame at a time and three calls in a row went
   * out in the same millisecond. The supply dropped all but the first — so
   * "set voltage, set current, output on" silently became "set voltage".
   */
  async _drainQueue() {
    if (this.draining) return;
    this.draining = true;
    try {
      while (this.txQueue.length) {
        const wait = this.lastWriteAt + MIN_FRAME_GAP_MS - Date.now();
        if (wait > 0) await new Promise((r) => setTimeout(r, wait));

        if (!this.isOpen) {
          this.txQueue.length = 0;
          break;
        }
        const frame = this.txQueue.shift();
        try {
          this.port.write(frame);
          this.lastWriteAt = Date.now();
        } catch (err) {
          this.emit('error', err);
        }
      }
    } finally {
      this.draining = false;
    }
  }

  /** Resolve once every queued frame has been written. */
  async _flush(timeoutMs = 3000) {
    const deadline = Date.now() + timeoutMs;
    while ((this.txQueue.length || this.draining) && Date.now() < deadline) {
      await new Promise((r) => setTimeout(r, 20));
    }
  }

  _onData(chunk) {
    this.rxBuffer = Buffer.concat([this.rxBuffer, chunk]);
    const { frames, rest } = proto.splitFrames(this.rxBuffer);
    this.rxBuffer = rest;

    // A stuck partial frame must not grow without bound.
    if (this.rxBuffer.length > 4096) this.rxBuffer = Buffer.alloc(0);

    for (const frame of frames) {
      const patch = proto.parseFrame(frame);
      if (!patch) continue;

      const previousProtection = this.state.protection;
      Object.assign(this.state, patch);

      if (
        patch.protection !== undefined &&
        patch.protection !== previousProtection
      ) {
        this.emit('protection', patch.protection);
      }

      this.emit('state', patch);
    }
  }

  _emitStatus(message) {
    this.emit('status', {
      connected: this.connected,
      port: this.port ? this.port.path : null,
      baudRate: this.port ? this.port.baudRate : null,
      message,
    });
  }
}

module.exports = {
  PowerSupply,
  PROBE_INTERVAL_MS,
  PROBE_ATTEMPTS,
  MIN_FRAME_GAP_MS,
  BAUD_SETTLE_MS,
};
