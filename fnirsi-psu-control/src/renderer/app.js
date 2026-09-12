'use strict';

/* global StripChart */

const $ = (id) => document.getElementById(id);
const api = window.psu;

/* -------------------------------------------------------------------------- */
/* State                                                                      */
/* -------------------------------------------------------------------------- */

const state = {
  connected: false,
  model: '',
  firmware: '',
  maxVoltage: 24,
  maxCurrent: 5,
  outputVoltage: 0,
  outputCurrent: 0,
  outputPower: 0,
  inputVoltage: 0,
  temperature: 0,
  setVoltage: 5,
  setCurrent: 1,
  outputEnabled: false,
  metering: false,
  ampHours: 0,
  wattHours: 0,
  brightness: 7,
  regulationMode: 'CV',
  protection: 'normal',
  groups: [],
  ovp: 0,
  ocp: 0,
  opp: 0,
  otp: 0,
  lvp: 0,
};

/** Guards every long-running routine: only one of program/V-scan/I-scan runs. */
let busyTask = null;
/** Recording buffer: rows destined for CSV. */
let recording = false;
let recordRows = [];
let chart = null;
let t = window.I18N.makeTranslator('en');

const DEFAULT_STEP_ROWS = 10;

/* -------------------------------------------------------------------------- */
/* Small helpers                                                              */
/* -------------------------------------------------------------------------- */

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const clamp = (v, lo, hi) => Math.min(hi, Math.max(lo, v));
const num = (el, fallback = 0) => {
  const v = parseFloat(el.value);
  return Number.isFinite(v) ? v : fallback;
};

let toastTimer = null;
function toast(message, kind = '') {
  const el = $('toast');
  el.textContent = message;
  el.className = `toast show ${kind}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => {
    el.className = 'toast';
  }, 2600);
}

/** Send a device command; surfaces failures instead of swallowing them. */
async function send(name, ...args) {
  const res = await api.command(name, ...args);
  if (!res.ok && res.error && res.error !== 'not connected') {
    toast(res.error, 'bad');
  }
  return res.ok;
}

function requireConnection() {
  if (state.connected) return true;
  toast(t('unconnectedDevice'), 'bad');
  return false;
}

/* -------------------------------------------------------------------------- */
/* Rendering                                                                  */
/* -------------------------------------------------------------------------- */

function applyLanguage(code) {
  t = window.I18N.makeTranslator(code);
  document.querySelectorAll('[data-i18n]').forEach((el) => {
    const value = t(el.dataset.i18n);
    if (value) el.textContent = value;
  });
  localStorage.setItem('lang', code);
  render();
}

function render() {
  // Header meters
  $('m-vout').innerHTML = `${state.outputVoltage.toFixed(2)}<small>V</small>`;
  $('m-iout').innerHTML = `${state.outputCurrent.toFixed(3)}<small>A</small>`;
  $('m-pout').innerHTML = `${state.outputPower.toFixed(2)}<small>W</small>`;
  $('m-vin').innerHTML = `${state.inputVoltage.toFixed(2)}<small>V</small>`;
  const f = state.temperature * 1.8 + 32;
  $('m-temp').innerHTML =
    `${Math.round(state.temperature)}<small>°C / ${Math.round(f)}°F</small>`;

  // Mode and protection badges
  const mode = $('badge-mode');
  mode.textContent = state.regulationMode === 'CV' ? t('cv') : t('cc');
  mode.className = `badge ${state.regulationMode === 'CV' ? 'cv' : 'cc'}`;

  const protect = $('badge-protect');
  const tripped = state.protection !== 'normal';
  protect.textContent = tripped ? t(state.protection) : t('normal');
  protect.className = `badge ${tripped ? 'trip' : 'ok'}`;

  // Output controls. The header button and the switch on the Basic page are
  // two views of one piece of state; both are driven from here so they can
  // never disagree.
  const label = state.outputEnabled ? t('outputOn') : t('outputOff');

  const out = $('btn-output');
  out.disabled = !state.connected;
  out.className = `power${state.outputEnabled ? ' on' : ''}`;
  $('output-state').textContent = label;

  const chk = $('chk-output');
  chk.disabled = !state.connected;
  chk.checked = state.outputEnabled;
  $('basic-output-state').textContent = label;
  $('basic-output-state').parentElement.parentElement.className =
    `output-row mt${state.outputEnabled ? ' live' : ''}`;

  // Connection panel
  $('link-dot').className = `dot${state.connected ? ' on' : ''}`;
  $('device-id').textContent = state.connected
    ? `${state.model || '—'} · ${state.firmware || '—'}`
    : t('notConnected');
  $('info-model').textContent = state.model || '—';
  $('info-firmware').textContent = state.firmware || '—';
  $('info-vin').textContent = `${state.inputVoltage.toFixed(2)} V`;
  $('info-vout').textContent = `${state.outputVoltage.toFixed(2)} V`;
  $('info-iout').textContent = `${state.outputCurrent.toFixed(3)} A`;
  $('info-pout').textContent = `${state.outputPower.toFixed(2)} W`;
  $('info-temp').textContent = `${Math.round(state.temperature)} °C`;
  $('info-mode').textContent = state.regulationMode === 'CV' ? t('cv') : t('cc');
  $('info-protect').textContent = tripped ? t(state.protection) : t('normal');
  $('btn-connect').textContent = state.connected ? t('disconnect') : t('online');

  // Limits and bars
  $('lim-v').textContent = `${state.maxVoltage.toFixed(2)} V`;
  $('lim-a').textContent = `${state.maxCurrent.toFixed(3)} A`;
  $('set-v').max = state.maxVoltage;
  $('set-a').max = state.maxCurrent;
  $('bar-v').style.width =
    `${clamp((num($('set-v')) / state.maxVoltage) * 100, 0, 100)}%`;
  $('bar-a').style.width =
    `${clamp((num($('set-a')) / state.maxCurrent) * 100, 0, 100)}%`;

  // Metering and brightness
  $('stat-ah').textContent = `${state.ampHours.toFixed(3)} Ah`;
  $('stat-wh').textContent = `${state.wattHours.toFixed(3)} Wh`;
  $('chk-metering').checked = state.metering;
  $('rng-bright').value = state.brightness;
  $('val-bright').textContent = state.brightness;

  renderGroups();
  $('rec-count').textContent = recordRows.length;
  $('btn-rec').textContent = recording ? t('recordStop') : t('recordStart');
}

function renderGroups() {
  const host = $('groups');
  if (host.childElementCount !== 6) {
    host.innerHTML = '';
    for (let i = 1; i <= 6; i += 1) {
      const b = document.createElement('button');
      b.className = 'group';
      b.dataset.group = String(i);
      b.innerHTML =
        `<span class="g-name">M${i}</span><span class="g-val">—</span>`;
      b.title = 'Click to recall · Shift-click to store the current setpoints';
      host.appendChild(b);
    }
  }
  state.groups.forEach((g) => {
    const el = host.querySelector(`[data-group="${g.id}"] .g-val`);
    if (el) el.textContent = `${g.voltage.toFixed(2)}V ${g.current.toFixed(3)}A`;
  });
}

/* -------------------------------------------------------------------------- */
/* Device events                                                              */
/* -------------------------------------------------------------------------- */

api.onState((patch) => {
  Object.assign(state, patch);

  // The setpoint inputs are user-editable; only sync them when the user is
  // not typing, otherwise the device echo fights the keyboard.
  if (patch.setVoltage !== undefined && document.activeElement !== $('set-v')) {
    $('set-v').value = patch.setVoltage.toFixed(2);
  }
  if (patch.setCurrent !== undefined && document.activeElement !== $('set-a')) {
    $('set-a').value = patch.setCurrent.toFixed(3);
  }
  for (const [key, id] of Object.entries({
    ovp: 'lim-ovp',
    ocp: 'lim-ocp',
    opp: 'lim-opp',
    otp: 'lim-otp',
    lvp: 'lim-lvp',
  })) {
    if (patch[key] !== undefined && document.activeElement !== $(id)) {
      $(id).value = key === 'otp' ? Math.round(patch[key]) : patch[key];
    }
  }

  // Feed the chart on every fresh measurement, exactly like the vendor app,
  // which only plots while the output is on.
  if (
    (patch.outputVoltage !== undefined || patch.outputCurrent !== undefined) &&
    state.outputEnabled
  ) {
    chart.push(state.outputVoltage, state.outputCurrent);
    if (recording) captureRow();
  }

  render();
});

api.onStatus((status) => {
  state.connected = status.connected;
  $('nav-status').textContent = status.message || '';
  if (status.message) $('connect-hint').textContent = status.message;
  render();
});

api.onProtection((code) => {
  if (code === 'normal') {
    toast(t('outputNormal'), 'good');
    return;
  }
  const messages = {
    ovp: 'ovpTripped',
    ocp: 'ocpTripped',
    opp: 'oppTripped',
    otp: 'otpTripped',
    lvp: 'lvpTripped',
  };
  toast(t(messages[code] || 'prompt'), 'bad');
  stopAllTasks();
});

api.onError((message) => toast(message, 'bad'));

/* -------------------------------------------------------------------------- */
/* Connection                                                                 */
/* -------------------------------------------------------------------------- */

async function refreshPorts() {
  const ports = await api.listPorts();
  const sel = $('sel-port');
  const previous = sel.value;
  sel.innerHTML = '';
  for (const p of ports) {
    const opt = document.createElement('option');
    opt.value = p.path;
    opt.textContent = p.manufacturer ? `${p.path} — ${p.manufacturer}` : p.path;
    sel.appendChild(opt);
  }
  if (previous && ports.some((p) => p.path === previous)) sel.value = previous;
  if (!ports.length) {
    const opt = document.createElement('option');
    opt.textContent = 'No serial ports found';
    opt.value = '';
    sel.appendChild(opt);
  }
}

async function toggleConnection() {
  const btn = $('btn-connect');
  btn.disabled = true;
  try {
    if (state.connected) {
      stopAllTasks();
      await api.disconnect();
      return;
    }
    const port = $('sel-port').value;
    if (!port) {
      toast('Select a serial port', 'bad');
      return;
    }
    // null tells the driver to accept whatever address the supply reports.
    const address = $('chk-auto-address').checked
      ? null
      : clamp(parseInt($("inp-address").value, 10) || 1, 1, 255);

    const res = await api.connect(port, parseInt($('sel-baud').value, 10), address);
    if (!res.ok) {
      toast(res.error || t('connectionFailed'), 'bad');
    } else {
      toast(t('connectionSuccessful'), 'good');
    }
  } finally {
    btn.disabled = false;
  }
}

/* -------------------------------------------------------------------------- */
/* Basic control                                                              */
/* -------------------------------------------------------------------------- */

async function applySetpoints() {
  if (!requireConnection()) return;
  const v = clamp(num($('set-v')), 0, state.maxVoltage);
  const a = clamp(num($('set-a')), 0, state.maxCurrent);
  $('set-v').value = v.toFixed(2);
  $('set-a').value = a.toFixed(3);
  await send('setVoltage', v);
  await send('setCurrent', a);
  render();
}

/**
 * Turn the output on or off.
 *
 * @param {boolean} [desired] explicit target state; defaults to inverting the
 *   current one. The switch passes its own checked value so a rejected command
 *   cannot leave the control out of step with the supply.
 */
async function toggleOutput(desired) {
  if (!requireConnection()) {
    render(); // put the switch back where the device actually is
    return;
  }
  const turningOn = desired === undefined ? !state.outputEnabled : desired;
  if (turningOn === state.outputEnabled) return;
  // Vendor behaviour: enabling the output re-sends both setpoints first.
  await send(
    'setOutput',
    turningOn,
    turningOn
      ? {
          voltage: clamp(num($('set-v')), 0, state.maxVoltage),
          current: clamp(num($('set-a')), 0, state.maxCurrent),
        }
      : null,
  );
}

/* -------------------------------------------------------------------------- */
/* Program (list) mode                                                        */
/* -------------------------------------------------------------------------- */

function buildStepRows(count = DEFAULT_STEP_ROWS) {
  const body = $('steps-body');
  body.innerHTML = '';
  for (let i = 1; i <= count; i += 1) addStepRow(i);
}

function addStepRow(index) {
  const body = $('steps-body');
  const n = index ?? body.childElementCount + 1;
  const tr = document.createElement('tr');
  tr.innerHTML = `
    <td>${n}</td>
    <td><input type="number" step="0.01" value="${n.toFixed(2)}" data-col="v" /></td>
    <td><input type="number" step="0.001" value="1.000" data-col="a" /></td>
    <td><input type="number" step="1" value="1" data-col="d" /></td>
    <td class="st"></td>`;
  body.appendChild(tr);
}

function readStep(rowIndex) {
  const tr = $('steps-body').children[rowIndex];
  if (!tr) return null;
  const get = (col) => parseFloat(tr.querySelector(`[data-col="${col}"]`).value);
  return {
    tr,
    voltage: clamp(get('v') || 0, 0, state.maxVoltage),
    current: clamp(get('a') || 0, 0, state.maxCurrent),
    // The vendor software caps the dwell at 24 hours.
    delay: clamp(get('d') || 0, 0, 86400),
  };
}

function markSteps(from, to, label) {
  const rows = $('steps-body').children;
  for (let i = 0; i < rows.length; i += 1) {
    rows[i].classList.remove('running');
    if (i + 1 >= from && i + 1 <= to) rows[i].querySelector('.st').textContent = label;
    else rows[i].querySelector('.st').textContent = '';
  }
}

async function runStep(rowIndex) {
  const step = readStep(rowIndex);
  if (!step) return null;
  for (const row of $('steps-body').children) row.classList.remove('running');
  step.tr.classList.add('running');
  step.tr.scrollIntoView({ block: 'nearest' });
  await send('setVoltage', step.voltage);
  await send('setCurrent', step.current);
  step.tr.querySelector('.st').textContent = t('run');
  $('prog-current').textContent = String(rowIndex + 1);
  return step;
}

/** Auto mode: run start..stop for N cycles, honouring pause and stop. */
async function startProgram() {
  if (!requireConnection()) return;
  if (busyTask) return;

  const rows = $('steps-body').childElementCount;
  const cycles = clamp(parseInt($('prog-cycles').value, 10) || 1, 1, 999);
  const from = clamp(parseInt($('prog-start').value, 10) || 1, 1, rows);
  const to = clamp(parseInt($('prog-stop').value, 10) || rows, 1, rows);
  if (to < from) {
    toast('Stop number must not be below the start number', 'bad');
    return;
  }

  const task = { kind: 'program', cancelled: false, paused: false };
  busyTask = task;
  $('nav-status').textContent = t('autoScanRunning');

  // Ensure the output is live before stepping.
  if (!state.outputEnabled) await toggleOutput(true);

  try {
    for (let cycle = 1; cycle <= cycles && !task.cancelled; cycle += 1) {
      $('prog-cycle').textContent = String(cycle);
      markSteps(from, to, '…');
      for (let i = from; i <= to && !task.cancelled; i += 1) {
        const step = await runStep(i - 1);
        if (!step) break;
        const until = Date.now() + step.delay * 1000;
        while (Date.now() < until || task.paused) {
          if (task.cancelled) break;
          await sleep(100);
        }
      }
    }
  } finally {
    if (busyTask === task) busyTask = null;
    for (const row of $('steps-body').children) row.classList.remove('running');
    $('nav-status').textContent = '';
  }
}

/** Manual mode: one step per click, wrapping through the cycle count. */
const manual = { index: null, from: 1, to: 1, cycle: 0 };

async function manualStep() {
  if (!requireConnection()) return;
  if (busyTask) return;

  const rows = $('steps-body').childElementCount;
  const cycles = clamp(parseInt($('prog-cycles').value, 10) || 1, 1, 999);

  if (manual.index === null) {
    manual.from = clamp(parseInt($('prog-start').value, 10) || 1, 1, rows);
    manual.to = clamp(parseInt($('prog-stop').value, 10) || rows, 1, rows);
    manual.index = manual.from;
    manual.cycle = 0;
    markSteps(manual.from, manual.to, '…');
  }

  if (manual.index > manual.to) {
    manual.index = manual.from;
  }
  if (manual.index === manual.from) {
    if (manual.cycle >= cycles) {
      toast('Program complete');
      return;
    }
    manual.cycle += 1;
    $('prog-cycle').textContent = String(manual.cycle);
  }

  if (!state.outputEnabled) await toggleOutput(true);
  await runStep(manual.index - 1);
  manual.index += 1;
}

function resetManual() {
  manual.index = null;
  manual.cycle = 0;
  $('prog-current').textContent = '—';
  $('prog-cycle').textContent = '—';
  for (const row of $('steps-body').children) {
    row.classList.remove('running');
    row.querySelector('.st').textContent = '';
  }
}

/* -------------------------------------------------------------------------- */
/* Voltage / current sweeps                                                   */
/* -------------------------------------------------------------------------- */

/**
 * Shared sweep engine. Mirrors the vendor algorithm: clamp stop above start,
 * clamp the step to the span, set the fixed axis once, then walk from start to
 * stop dwelling `delay` seconds at each point.
 *
 * @param {'voltage'|'current'} axis the swept axis
 */
async function runSweep(axis) {
  if (!requireConnection()) return;
  if (busyTask) return;

  const isVoltage = axis === 'voltage';
  const ids = isVoltage
    ? { fixed: 'vs-a', start: 'vs-start', stop: 'vs-stop', step: 'vs-step', delay: 'vs-delay' }
    : { fixed: 'as-v', start: 'as-start', stop: 'as-stop', step: 'as-step', delay: 'as-delay' };

  const limit = isVoltage ? state.maxVoltage : state.maxCurrent;
  const epsilon = isVoltage ? 0.01 : 0.001;
  const decimals = isVoltage ? 2 : 3;

  const start = clamp(num($(ids.start)), 0, limit);
  let stop = num($(ids.stop));
  if (stop <= start) stop = start + epsilon;
  stop = clamp(stop, 0, limit);
  const span = stop - start;
  let step = num($(ids.step));
  if (step > span || step <= 0) step = span;
  const delay = clamp(parseInt($(ids.delay).value, 10) || 1, 1, 86400) * 1000;

  $(ids.stop).value = stop.toFixed(decimals);
  $(ids.step).value = step.toFixed(decimals);

  const task = { kind: axis, cancelled: false };
  busyTask = task;
  $('nav-status').textContent = isVoltage
    ? t('voltageScanRunning')
    : t('currentScanRunning');

  try {
    // Fix the other axis first, then enable the output.
    const fixed = num($(ids.fixed));
    await send(isVoltage ? 'setCurrent' : 'setVoltage', fixed);
    if (!state.outputEnabled) await toggleOutput(true);

    let value = start;
    await send(isVoltage ? 'setVoltage' : 'setCurrent', value);
    await sleep(delay);

    while (!task.cancelled && value < stop) {
      value += value + step > stop ? stop - value : step;
      await send(isVoltage ? 'setVoltage' : 'setCurrent', value);
      if (task.cancelled) break;
      await sleep(delay);
    }
  } finally {
    if (busyTask === task) busyTask = null;
    $('nav-status').textContent = '';
  }
}

function stopAllTasks() {
  if (busyTask) busyTask.cancelled = true;
  busyTask = null;
  $('nav-status').textContent = '';
}

/* -------------------------------------------------------------------------- */
/* Recording                                                                  */
/* -------------------------------------------------------------------------- */

function captureRow() {
  const now = new Date();
  const pad = (n) => String(n).padStart(2, '0');
  const stamp =
    `${now.getFullYear()}/${pad(now.getMonth() + 1)}/${pad(now.getDate())} ` +
    `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;

  recordRows.push([
    stamp,
    `${state.outputVoltage.toFixed(2)}V`,
    `${state.outputCurrent.toFixed(3)}A`,
    state.outputEnabled ? t('labelOpen') : t('labelClose'),
    state.regulationMode === 'CV' ? t('cv') : t('cc'),
    state.protection === 'normal' ? t('normal') : t(state.protection),
  ]);

  // Keep the on-screen table light; the CSV export uses the full buffer.
  const body = $('rec-body');
  const tr = document.createElement('tr');
  tr.innerHTML = recordRows[recordRows.length - 1]
    .map((c) => `<td>${c}</td>`)
    .join('');
  body.prepend(tr);
  while (body.childElementCount > 200) body.lastElementChild.remove();

  $('rec-count').textContent = recordRows.length;
}

async function exportCsv() {
  if (!recordRows.length) {
    toast('Nothing recorded yet', 'bad');
    return;
  }
  const header = [
    t('recordTime'),
    t('outputVoltage'),
    t('outputCurrent'),
    t('switchStatus'),
    t('outputMode'),
    t('operatingStatus'),
  ];
  const escape = (cell) =>
    /[",\n]/.test(cell) ? `"${cell.replace(/"/g, '""')}"` : cell;
  const csv = [header, ...recordRows]
    .map((row) => row.map(escape).join(','))
    .join('\r\n');

  const stamp = new Date().toISOString().slice(0, 19).replace(/[:T]/g, '-');
  const res = await api.exportCsv(`psu-recording-${stamp}.csv`, csv);
  if (res.ok) toast(t('saveSuccessful'), 'good');
  else if (!res.canceled) toast(t('saveFailed'), 'bad');
}

/* -------------------------------------------------------------------------- */
/* Wiring                                                                     */
/* -------------------------------------------------------------------------- */

function wire() {
  // Navigation
  document.querySelectorAll('.nav-item').forEach((btn) => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.nav-item').forEach((b) => b.classList.remove('active'));
      document.querySelectorAll('.view').forEach((v) => v.classList.remove('active'));
      btn.classList.add('active');
      $(`view-${btn.dataset.view}`).classList.add('active');
      if (btn.dataset.view === 'recording') chart.draw();
    });
  });

  // Connection
  $('btn-refresh').addEventListener('click', refreshPorts);
  $('btn-connect').addEventListener('click', toggleConnection);
  $('chk-auto-address').addEventListener('change', (e) => {
    $('inp-address').disabled = e.target.checked;
  });

  // Output and setpoints
  $('btn-output').addEventListener('click', () => toggleOutput());
  $('chk-output').addEventListener('change', (e) => toggleOutput(e.target.checked));
  $('btn-apply').addEventListener('click', applySetpoints);
  $('set-v').addEventListener('change', applySetpoints);
  $('set-a').addEventListener('change', applySetpoints);
  $('set-v').addEventListener('input', render);
  $('set-a').addEventListener('input', render);

  document.querySelectorAll('[data-step]').forEach((btn) => {
    btn.addEventListener('click', () => {
      const [axis, dir] = [btn.dataset.step[0], btn.dataset.step[1]];
      const input = axis === 'v' ? $('set-v') : $('set-a');
      const delta = (axis === 'v' ? 0.01 : 0.001) * (dir === '+' ? 1 : -1);
      const limit = axis === 'v' ? state.maxVoltage : state.maxCurrent;
      const next = clamp(num(input) + delta, 0, limit);
      input.value = next.toFixed(axis === 'v' ? 2 : 3);
      applySetpoints();
    });
  });

  // Memory groups: click recalls, shift-click stores.
  $('groups').addEventListener('click', async (e) => {
    const btn = e.target.closest('.group');
    if (!btn || !requireConnection()) return;
    const id = parseInt(btn.dataset.group, 10);

    if (e.shiftKey) {
      const v = clamp(num($('set-v')), 0, state.maxVoltage);
      const a = clamp(num($('set-a')), 0, state.maxCurrent);
      await send('saveGroup', id, v, a);
      toast(`M${id} ← ${v.toFixed(2)}V ${a.toFixed(3)}A`, 'good');
      return;
    }

    const group = state.groups.find((g) => g.id === id);
    if (!group) return;
    $('set-v').value = group.voltage.toFixed(2);
    $('set-a').value = group.current.toFixed(3);
    await applySetpoints();
    toast(`M${id} → ${group.voltage.toFixed(2)}V ${group.current.toFixed(3)}A`);
  });

  // Metering and brightness
  $('chk-metering').addEventListener('change', (e) => {
    if (!requireConnection()) {
      e.target.checked = state.metering;
      return;
    }
    send('setMetering', e.target.checked);
  });

  $('rng-bright').addEventListener('input', (e) => {
    $('val-bright').textContent = e.target.value;
  });
  $('rng-bright').addEventListener('change', (e) => {
    if (requireConnection()) send('setBrightness', parseInt(e.target.value, 10));
  });

  // Program mode
  $('btn-prog-start').addEventListener('click', startProgram);
  $('btn-prog-pause').addEventListener('click', () => {
    if (busyTask?.kind === 'program') busyTask.paused = true;
  });
  $('btn-prog-resume').addEventListener('click', () => {
    if (busyTask?.kind === 'program') busyTask.paused = false;
  });
  $('btn-prog-stop').addEventListener('click', stopAllTasks);
  $('btn-rows-add').addEventListener('click', () => addStepRow());
  $('btn-rows-del').addEventListener('click', () => {
    const body = $('steps-body');
    if (body.childElementCount > 1) body.lastElementChild.remove();
  });
  $('btn-step-next').addEventListener('click', manualStep);
  $('btn-step-stop').addEventListener('click', () => {
    stopAllTasks();
    resetManual();
  });

  document.querySelectorAll('.seg').forEach((btn) => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.seg').forEach((b) => b.classList.remove('active'));
      btn.classList.add('active');
      const isAuto = btn.dataset.mode === 'auto';
      $('auto-controls').classList.toggle('hidden', !isAuto);
      $('manual-controls').classList.toggle('hidden', isAuto);
      resetManual();
    });
  });

  // Sweeps
  $('btn-vs-run').addEventListener('click', () => runSweep('voltage'));
  $('btn-as-run').addEventListener('click', () => runSweep('current'));
  $('btn-vs-stop').addEventListener('click', stopAllTasks);
  $('btn-as-stop').addEventListener('click', stopAllTasks);

  // Recording
  $('btn-rec').addEventListener('click', () => {
    if (!requireConnection()) return;
    recording = !recording;
    if (recording) {
      recordRows = [];
      $('rec-body').innerHTML = '';
    }
    render();
  });
  $('btn-rec-clear').addEventListener('click', () => {
    recordRows = [];
    $('rec-body').innerHTML = '';
    chart.clear();
    render();
  });
  $('btn-rec-export').addEventListener('click', exportCsv);
  $('chk-series-v').addEventListener('change', (e) => {
    chart.showV = e.target.checked;
    chart.draw();
  });
  $('chk-series-a').addEventListener('change', (e) => {
    chart.showA = e.target.checked;
    chart.draw();
  });

  // Settings
  $('btn-limits').addEventListener('click', () => {
    if (!requireConnection()) return;
    send('setProtections', {
      ovp: num($('lim-ovp')),
      ocp: num($('lim-ocp')),
      opp: num($('lim-opp')),
      otp: Math.round(num($('lim-otp'))),
      lvp: num($('lim-lvp')),
    }).then((ok) => toast(t(ok ? 'saveSuccessful' : 'saveFailed'), ok ? 'good' : 'bad'));
  });

  $('sel-lang').addEventListener('change', (e) => applyLanguage(e.target.value));

  wireFirmware();
}

/**
 * Firmware update, deliberately staged so the bootloader command can never be
 * a stray click: choose a file, then arm the update, then copy. The main
 * process enforces the same ordering, so the disabled buttons are a courtesy
 * rather than the actual safety mechanism.
 */
function wireFirmware() {
  $('btn-fw-choose').addEventListener('click', async () => {
    const res = await api.firmware.choose();
    if (!res.ok) {
      if (!res.canceled) toast(res.error, 'bad');
      return;
    }
    const kb = (res.size / 1024).toFixed(1);
    $('fw-file').textContent = `${res.name} (${kb} KiB)`;
    $('fw-file').className = 'fw-file set';
    $('btn-fw-enter').disabled = false;
    $('btn-fw-copy').disabled = false;
  });

  $('btn-fw-enter').addEventListener('click', async () => {
    if (!requireConnection()) return;
    const ok = window.confirm(
      'Put the power supply into firmware update mode?\n\n' +
        'It will disconnect from this application and appear as a USB drive. ' +
        'Nothing is written until you copy the firmware onto it.\n\n' +
        'If you change your mind, power-cycle the supply and it returns to ' +
        'normal operation.',
    );
    if (!ok) return;

    const res = await api.firmware.enterBootloader();
    if (!res.ok) {
      toast(res.error, 'bad');
      return;
    }
    toast('Supply is in update mode — copy the firmware to its drive', 'good');
  });

  $('btn-fw-copy').addEventListener('click', async () => {
    const res = await api.firmware.copy();
    if (!res.ok) {
      if (!res.canceled) toast(res.error, 'bad');
      return;
    }
    toast(`Copied to ${res.path} — now power-cycle the supply`, 'good');
  });
}

/* -------------------------------------------------------------------------- */
/* Boot                                                                       */
/* -------------------------------------------------------------------------- */

async function init() {
  chart = new StripChart($('chart'));

  const info = await api.appInfo();
  $('about-version').textContent = info.version;
  $('about-electron').textContent = info.electron;

  const baud = $('sel-baud');
  for (const rate of info.baudRates) {
    const opt = document.createElement('option');
    opt.value = String(rate);
    opt.textContent = String(rate);
    baud.appendChild(opt);
  }
  baud.value = String(info.baudRates[0]); // vendor default: 9600

  const langSel = $('sel-lang');
  for (const lang of window.I18N.LANGUAGES) {
    const opt = document.createElement('option');
    opt.value = lang.code;
    opt.textContent = lang.label;
    langSel.appendChild(opt);
  }
  const saved = localStorage.getItem('lang') || 'en';
  langSel.value = saved;

  buildStepRows();
  wire();
  applyLanguage(saved);
  await refreshPorts();
  render();
}

init();
