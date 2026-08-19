/*
 * Dashboard UI: board details, microSD capacity, and file management.
 */
'use strict';

const $ = (id) => document.getElementById(id);

const els = {
  banner:      $('banner'),
  connPill:    $('conn-pill'),
  boardBody:   $('board-body'),
  netBody:     $('net-body'),
  sdBody:      $('sd-body'),
  sdPill:      $('sd-pill'),
  sdMeter:     $('sd-meter'),
  crumb:       $('crumb'),
  filesBody:   $('files-body'),
  refreshFiles: $('refresh-files'),
  forget:      $('forget'),
  micPill:     $('mic-pill'),
  recTimer:    $('rec-timer'),
  camPill:     $('cam-pill'),
  camToggle:   $('cam-toggle'),
  camView:     $('cam-view'),
  camVideo:    $('cam-video'),
  camStats:    $('cam-stats'),
  camNote:     $('cam-note'),
  aiPill:      $('ai-pill'),
  aiToggle:    $('ai-toggle'),
  aiView:      $('ai-view'),
  aiImg:       $('ai-img'),
  aiOverlay:   $('ai-overlay'),
  aiStats:     $('ai-stats'),
  aiList:      $('ai-list'),
  record:      $('record'),
  recResult:   $('rec-result'),
};

let cwd = '/';
/* Recording blocks the server for its whole duration, so the periodic board
 * poll is suspended while one is running to avoid queueing requests behind it. */
let recording = false;

/* ---- bottom navigation ---- */
const PAGES = ['home', 'audio', 'cam', 'ai', 'files', 'settings'];

function showPage(name) {
  if (!PAGES.includes(name)) name = 'home';
  for (const p of PAGES) {
    document.getElementById(`page-${p}`).classList.toggle('active', p === name);
  }
  document.querySelectorAll('.nav-item').forEach((b) =>
    b.classList.toggle('active', b.dataset.page === name));
  // Survive a reload on the same page.
  if (location.hash !== `#${name}`) history.replaceState(null, '', `#${name}`);

  // Refresh the content the user is now looking at (never mid-recording:
  // each HTTP request is a radio burst that can mark the audio).
  if (!recording) {
    if (name === 'files') loadFiles(cwd);
    if (name === 'settings') loadBoard();
  }
  if (name === 'ai') {
    aiPageEnter();
  } else {
    aiPageLeave();
  }
  if (name === 'cam') {
    camPageEnter();
  } else {
    camPageLeave();
  }
}

document.querySelectorAll('.nav-item').forEach((b) =>
  b.addEventListener('click', () => showPage(b.dataset.page)));

/* ---- helpers ---- */
function banner(kind, text) {
  if (!kind) { els.banner.innerHTML = ''; return; }
  const div = document.createElement('div');
  div.className = `banner banner-${kind}`;
  div.textContent = text;
  els.banner.innerHTML = '';
  els.banner.appendChild(div);
}

/* Decimal units, matching how cards are sold and how the firmware logs them. */
function bytes(n) {
  if (!n && n !== 0) return '—';
  const units = ['B', 'KB', 'MB', 'GB', 'TB'];
  let i = 0;
  let v = Number(n);
  while (v >= 1000 && i < units.length - 1) { v /= 1000; i++; }
  return `${v.toFixed(i === 0 ? 0 : v >= 100 ? 0 : 1)} ${units[i]}`;
}

function duration(seconds) {
  const s = Math.floor(seconds % 60);
  const m = Math.floor((seconds / 60) % 60);
  const h = Math.floor((seconds / 3600) % 24);
  const d = Math.floor(seconds / 86400);
  if (d) return `${d}d ${h}h ${m}m`;
  if (h) return `${h}h ${m}m`;
  if (m) return `${m}m ${s}s`;
  return `${s}s`;
}

function rows(target, pairs) {
  target.innerHTML = '';
  for (const [label, value] of pairs) {
    const row = document.createElement('div');
    row.className = 'kv';
    const dt = document.createElement('dt');
    dt.textContent = label;
    const dd = document.createElement('dd');
    dd.textContent = value;
    row.append(dt, dd);
    target.appendChild(row);
  }
}

async function api(path, options) {
  const res = await fetch(path, options);
  let body = {};
  try { body = await res.json(); } catch { /* non-JSON error page */ }
  if (!res.ok || body.ok === false) {
    throw new Error(body.error || `Request failed (${res.status})`);
  }
  return body;
}

/* ---- board + network + sd ---- */
async function loadBoard() {
  let info;
  try {
    info = await api('/api/board');
  } catch (err) {
    els.boardBody.innerHTML = '<p class="empty">Could not read board details.</p>';
    banner('error', err.message);
    return;
  }

  rows(els.boardBody, [
    ['Board',        info.board],
    ['Chip',         `${info.chip} · ${info.cores} cores · rev ${info.revision}`],
    ['Flash',        bytes(info.flashBytes)],
    ['PSRAM',        `${bytes(info.psramBytes)} (${bytes(info.psramFree)} free)`],
    ['Internal RAM', `${bytes(info.internalFree)} free`],
    ['ESP-IDF',      info.idf],
    ['Firmware',     `${info.app} · ${info.built}`],
    ['Uptime',       duration(info.uptimeSeconds)],
  ]);

  const net = info.network;
  rows(els.netBody, [
    ['Network',     net.ssid || '—'],
    ['IP address',  net.ip],
    ['MAC',         net.mac],
    ['Signal',      net.connected ? `${net.rssi} dBm` : 'not connected'],
    ['Channel',     net.channel || '—'],
  ]);

  els.connPill.innerHTML = '';
  const pill = document.createElement('span');
  pill.className = `pill ${net.connected ? 'pill-ok' : 'pill-off'}`;
  const dot = document.createElement('span');
  dot.className = 'dot';
  pill.append(dot, document.createTextNode(net.connected ? 'Online' : 'Offline'));
  els.connPill.appendChild(pill);

  renderSd(info.sd);
  renderMic(info.mic);
}

function renderMic(mic) {
  els.micPill.innerHTML = '';
  const pill = document.createElement('span');
  pill.className = `pill ${mic && mic.ready ? 'pill-ok' : 'pill-off'}`;
  pill.textContent = mic && mic.ready ? 'Ready' : 'Unavailable';
  els.micPill.appendChild(pill);

  // Nothing to record with, so do not offer a button that can only fail.
  // Leave the button alone mid-recording; the poller owns it then.
  if (!recording) {
    els.record.disabled = !(mic && mic.ready);
  }
}

function renderSd(sd) {
  els.sdPill.innerHTML = '';
  const pill = document.createElement('span');
  pill.className = `pill ${sd.present ? 'pill-ok' : 'pill-off'}`;
  pill.textContent = sd.present ? 'Mounted' : 'No card';
  els.sdPill.appendChild(pill);

  if (!sd.present) {
    els.sdBody.innerHTML = '<p class="empty">No microSD card is mounted.</p>';
    els.sdMeter.innerHTML = '';
    return;
  }

  rows(els.sdBody, [
    ['Card',     `${sd.name} (${sd.type})`],
    ['Capacity', bytes(sd.capacityBytes)],
    ['Bus',      `${sd.busWidth}-bit @ ${(sd.speedKhz / 1000).toFixed(0)} MHz`],
  ]);

  updateMeter(sd.fsUsedBytes, sd.fsTotalBytes);
}

function updateMeter(used, total) {
  const pct = total ? Math.min(100, (used / total) * 100) : 0;
  els.sdMeter.innerHTML = `
    <div class="meter"><div class="meter-fill" style="width:${pct.toFixed(1)}%"></div></div>
    <div class="meter-legend">
      <span>${bytes(used)} used</span>
      <span>${bytes(total - used)} free of ${bytes(total)}</span>
    </div>`;
}

/* ---- files ---- */
function renderCrumb() {
  els.crumb.innerHTML = '';
  const parts = cwd.split('/').filter(Boolean);

  const root = document.createElement('button');
  root.type = 'button';
  root.textContent = 'sdcard';
  root.addEventListener('click', () => loadFiles('/'));
  els.crumb.appendChild(root);

  let path = '';
  for (const part of parts) {
    path += `/${part}`;
    const target = path;
    els.crumb.appendChild(document.createTextNode(' / '));
    const btn = document.createElement('button');
    btn.type = 'button';
    btn.textContent = part;
    btn.addEventListener('click', () => loadFiles(target));
    els.crumb.appendChild(btn);
  }
}

async function loadFiles(path) {
  cwd = path || '/';
  renderCrumb();
  els.filesBody.innerHTML =
    '<div class="empty center-row"><span class="spinner"></span> Loading…</div>';

  let data;
  try {
    data = await api(`/api/sd/list?path=${encodeURIComponent(cwd)}`);
  } catch (err) {
    els.filesBody.innerHTML = '<p class="empty">Could not list this folder.</p>';
    banner('error', err.message);
    return;
  }

  const entries = data.entries;
  // Folders first, then files, each alphabetical — matches what a file manager does.
  entries.sort((a, b) =>
    a.dir !== b.dir ? (a.dir ? -1 : 1) : a.name.localeCompare(b.name));

  if (!entries.length) {
    els.filesBody.innerHTML = '<p class="empty">This folder is empty.</p>';
    return;
  }

  const list = document.createElement('ul');
  list.className = 'file-list';

  for (const entry of entries) {
    const li = document.createElement('li');
    li.className = 'file';

    const icon = document.createElement('span');
    icon.className = 'file-icon';
    icon.textContent = entry.dir ? '📁' : '📄';

    const name = document.createElement('span');
    name.className = `file-name${entry.dir ? ' dir' : ''}`;
    name.textContent = entry.name;
    if (entry.dir) {
      name.setAttribute('role', 'button');
      name.addEventListener('click', () =>
        loadFiles(cwd === '/' ? `/${entry.name}` : `${cwd}/${entry.name}`));
    }

    const size = document.createElement('span');
    size.className = 'file-size';
    size.textContent = entry.dir ? '' : bytes(entry.size);

    const actions = document.createElement('span');
    actions.className = 'file-actions';

    // Playback only makes sense for audio; offering it on a text file would
    // just be a button that fails.
    if (!entry.dir && /\.wav$/i.test(entry.name)) {
      const path = cwd === '/' ? `/${entry.name}` : `${cwd}/${entry.name}`;

      const webBtn = document.createElement('button');
      webBtn.type = 'button';
      webBtn.className = 'btn btn-ghost btn-sm';
      webBtn.innerHTML = '▶ Web';
      webBtn.title = 'Play in this browser';
      webBtn.addEventListener('click', () => playWeb(path, li));

      const boardBtn = document.createElement('button');
      boardBtn.type = 'button';
      boardBtn.className = 'btn btn-ghost btn-sm board-play';
      boardBtn.dataset.path = path;
      boardBtn.innerHTML = '🔈 Board';
      boardBtn.title = 'Play through the speaker attached to the board';
      boardBtn.addEventListener('click', () => playBoard(path));

      actions.append(webBtn, boardBtn);
    }

    if (!entry.dir) {
      const path = cwd === '/' ? `/${entry.name}` : `${cwd}/${entry.name}`;
      // A real anchor, not a fetch: the browser streams straight to disk, so
      // a large file never has to fit in page memory.
      const dl = document.createElement('a');
      dl.className = 'btn btn-ghost btn-sm';
      dl.textContent = '⬇';
      dl.title = `Download ${entry.name}`;
      dl.setAttribute('aria-label', `Download ${entry.name}`);
      dl.href = `/api/sd/download?path=${encodeURIComponent(path)}&dl=1`;
      dl.download = entry.name;
      actions.appendChild(dl);
    }

    const del = document.createElement('button');
    del.type = 'button';
    del.className = 'btn btn-danger btn-sm';
    del.textContent = 'Delete';
    del.addEventListener('click', () => remove(entry));
    actions.appendChild(del);

    li.append(icon, name, size, actions);
    list.appendChild(li);
  }

  els.filesBody.innerHTML = '';
  els.filesBody.appendChild(list);
}

/* ---- playback: in this browser ---- */
function playWeb(path, row) {
  // One player at a time, reused, so a list of files doesn't accumulate
  // half-loaded <audio> elements.
  document.querySelectorAll('.inline-player').forEach((p) => p.remove());

  const wrap = document.createElement('div');
  wrap.className = 'inline-player';
  const audio = document.createElement('audio');
  audio.controls = true;
  audio.autoplay = true;
  audio.src = `/api/sd/download?path=${encodeURIComponent(path)}`;
  wrap.appendChild(audio);
  row.insertAdjacentElement('afterend', wrap);
}

/* ---- playback: through the speaker on the board ---- */
let boardPlayPoll = null;

async function playBoard(path) {
  try {
    await api('/api/audio/play', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ path }),
    });
    banner('info', `Playing ${path} through the board's speaker.`);
    markBoardPlaying(path);
    if (!boardPlayPoll) boardPlayPoll = setInterval(pollBoardPlayback, 700);
  } catch (err) {
    banner('error', err.message);
  }
}

async function stopBoard() {
  try {
    await api('/api/audio/playstop', { method: 'POST' });
  } catch (err) {
    banner('error', err.message);
  }
}

function markBoardPlaying(path) {
  document.querySelectorAll('.board-play').forEach((b) => {
    const active = b.dataset.path === path;
    b.classList.toggle('btn-stop', active);
    b.classList.toggle('btn-ghost', !active);
    b.innerHTML = active ? '■ Stop' : '🔈 Board';
    b.onclick = active ? stopBoard : () => playBoard(b.dataset.path);
  });
}

async function pollBoardPlayback() {
  try {
    const st = await api('/api/audio/state');
    if (st.playing) {
      markBoardPlaying(st.playingPath);
    } else {
      clearInterval(boardPlayPoll);
      boardPlayPoll = null;
      markBoardPlaying(null);
      banner(null);
    }
  } catch {
    /* transient — try again on the next tick */
  }
}

async function remove(entry) {
  const what = entry.dir ? 'folder' : 'file';
  if (!confirm(`Delete the ${what} "${entry.name}"? This cannot be undone.`)) {
    return;
  }

  const path = cwd === '/' ? `/${entry.name}` : `${cwd}/${entry.name}`;
  try {
    const result = await api('/api/sd/delete', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ path }),
    });
    banner('success', `Deleted ${entry.name}.`);
    updateMeter(result.fsUsedBytes, result.fsUsedBytes + result.fsFreeBytes);
    loadFiles(cwd);
  } catch (err) {
    banner('error', err.message);
  }
}

/* ---- microphone ---- */
let recTimer = null;
let maxSeconds = 0;

function setButton(mode) {
  // mode: 'record' | 'stop' | 'busy'
  els.record.classList.toggle('btn-stop', mode === 'stop');
  els.record.classList.toggle('btn-primary', mode !== 'stop');
  els.record.classList.toggle('recording', mode === 'stop');
  els.record.disabled = (mode === 'busy');
  els.record.innerHTML = mode === 'stop'
    ? '<span class="rec-dot"></span> Stop'
    : '<span class="rec-dot"></span> Record';
}

function showElapsed(seconds) {
  const m = Math.floor(seconds / 60);
  const s = Math.floor(seconds % 60);
  els.recTimer.hidden = false;
  els.recTimer.textContent = `${m}:${String(s).padStart(2, '0')}`;
}

/*
 * While recording, every HTTP request makes the ESP32-C6 transmit, and those
 * radio bursts couple into the microphone's analog path as faint bird-like
 * chirps (measured: 10 chirps in a flooded 18 s window vs 1 in a radio-quiet
 * one). So the elapsed timer runs LOCALLY, and the board is only asked for its
 * state every 10 s — just often enough to notice a card-full auto-stop.
 */
let recStartedAt = 0;
let statePoll = null;

function tickElapsed() {
  showElapsed((Date.now() - recStartedAt) / 1000);
}

async function pollRecording() {
  try {
    const st = await api('/api/audio/state');
    // The board stops itself if the card fills; follow it rather than leaving
    // the button showing Stop for a recording that already ended.
    if (!st.recording) {
      await finishRecording();
    }
  } catch {
    /* transient — the next tick will retry */
  }
}

async function startRecording() {
  banner(null);
  els.recResult.innerHTML = '';
  setButton('busy');
  try {
    const r = await api('/api/audio/start', { method: 'POST' });
    maxSeconds = r.maxSeconds;
    recording = true;
    recStartedAt = Date.now();
    setButton('stop');
    showElapsed(0);
    recTimer = setInterval(tickElapsed, 250);     // local, no radio traffic
    statePoll = setInterval(pollRecording, 10000); // rare, to catch auto-stop
  } catch (err) {
    setButton('record');
    banner('error', err.message);
  }
}

async function finishRecording() {
  if (recTimer) { clearInterval(recTimer); recTimer = null; }
  if (statePoll) { clearInterval(statePoll); statePoll = null; }
  setButton('busy');
  try {
    const result = await api('/api/audio/stop', { method: 'POST' });
    renderRecording(result);
    updateMeter(result.fsUsedBytes, result.fsUsedBytes + result.fsFreeBytes);
    loadFiles(cwd);
  } catch (err) {
    banner('error', err.message);
  } finally {
    recording = false;
    els.recTimer.hidden = true;
    setButton('record');
  }
}

els.record.addEventListener('click', () => {
  if (recording) {
    finishRecording();
  } else {
    startRecording();
  }
});

function renderRecording(r) {
  els.recResult.innerHTML = '';

  // Peak is reported in dBFS, where 0 is full scale. Map -60..0 onto the bar.
  const pct = Math.max(0, Math.min(100, ((r.peakDbfs + 60) / 60) * 100));

  const level = document.createElement('div');
  level.className = 'level';
  const fill = document.createElement('div');
  fill.className = `level-fill ${r.silent ? 'low' : 'ok'}`;
  fill.style.width = `${pct.toFixed(1)}%`;
  level.appendChild(fill);

  const legend = document.createElement('div');
  legend.className = 'meter-legend';
  legend.innerHTML =
    `<span>peak ${r.peakDbfs.toFixed(1)} dBFS</span>` +
    `<span>rms ${r.rmsDbfs.toFixed(1)} dBFS</span>`;

  els.recResult.append(level, legend);

  // A dead microphone yields a flat near-zero stream, which is otherwise
  // indistinguishable from a good recording of a quiet room — so say so.
  banner(r.silent ? 'error' : 'success',
    r.silent
      ? 'Recording saved, but no sound was detected. The microphone may not be ' +
        'working — try speaking closer, or raise the mic gain in menuconfig.'
      : `Recorded ${r.seconds.toFixed(1)} s to ${r.path}.`);

  const rows2 = document.createElement('dl');
  rows(rows2, [
    ['File', r.path],
    ['Size', bytes(r.bytes)],
    ['Format', `${(r.sampleRate / 1000).toFixed(0)} kHz mono 16-bit WAV`],
  ]);
  els.recResult.appendChild(rows2);

  const audio = document.createElement('audio');
  audio.controls = true;
  audio.preload = 'none';
  audio.src = `/api/sd/download?path=${encodeURIComponent(r.path)}`;
  els.recResult.appendChild(audio);
}

/* ---- AI camera ---- */
let aiRunning = false;
let aiPoll = null;

function aiPill(on) {
  els.aiPill.innerHTML = '';
  const pill = document.createElement('span');
  pill.className = `pill ${on ? 'pill-ok' : 'pill-off'}`;
  pill.textContent = on ? 'Running' : 'Stopped';
  els.aiPill.appendChild(pill);
}

function aiAttachStream() {
  // The MJPEG stream lives on its own HTTP server (port 81) so it cannot
  // starve the UI's API server. Attach only while the AI page is visible.
  els.aiImg.src = `http://${location.hostname}:81/stream?t=${Date.now()}`;
  els.aiView.hidden = false;
  if (!aiPoll) aiPoll = setInterval(aiFetchDetections, 400);
}

function aiDetachStream() {
  if (aiPoll) { clearInterval(aiPoll); aiPoll = null; }
  els.aiImg.src = '';
}

async function aiPageEnter() {
  try {
    const st = await api('/api/ai/status');
    aiRunning = st.running;
    aiPill(aiRunning);
    els.aiToggle.textContent = aiRunning ? 'Stop camera' : 'Start camera';
    if (aiRunning) aiAttachStream();
  } catch { /* board unreachable; button stays as-is */ }
}

function aiPageLeave() {
  // Leaving the page stops the stream + polling but NOT the pipeline —
  // stopping the camera is an explicit button press.
  aiDetachStream();
}

async function aiFetchDetections() {
  try {
    const d = await api('/api/ai/detections');
    drawDetections(d);
    els.aiStats.innerHTML =
      `<span>${d.width}×${d.height} · ${d.fps.toFixed(1)} fps</span>` +
      `<span>inference ${d.inferMs} ms</span>`;
    els.aiList.innerHTML = '';
    for (const det of d.detections) {
      const li = document.createElement('li');
      const name = document.createElement('span');
      name.textContent = det.label;
      const score = document.createElement('span');
      score.className = 'score';
      score.textContent = `${(det.score * 100).toFixed(0)}%`;
      li.append(name, score);
      els.aiList.appendChild(li);
    }
  } catch { /* transient */ }
}

function drawDetections(d) {
  const cv = els.aiOverlay;
  if (cv.width !== d.width || cv.height !== d.height) {
    cv.width = d.width;
    cv.height = d.height;
  }
  const ctx = cv.getContext('2d');
  ctx.clearRect(0, 0, cv.width, cv.height);
  ctx.lineWidth = Math.max(2, d.width / 320);
  ctx.font = `${Math.max(12, d.width / 40)}px sans-serif`;
  for (const det of d.detections) {
    ctx.strokeStyle = '#52a441';
    ctx.fillStyle = '#52a441';
    ctx.strokeRect(det.x0, det.y0, det.x1 - det.x0, det.y1 - det.y0);
    const label = `${det.label} ${(det.score * 100).toFixed(0)}%`;
    const tw = ctx.measureText(label).width + 8;
    const th = Math.max(16, d.width / 36);
    const ty = det.y0 > th ? det.y0 - th : det.y0;
    ctx.fillRect(det.x0, ty, tw, th);
    ctx.fillStyle = '#fff';
    ctx.fillText(label, det.x0 + 4, ty + th * 0.75);
  }
}

els.aiToggle.addEventListener('click', async () => {
  els.aiToggle.disabled = true;
  try {
    if (!aiRunning) {
      await api('/api/ai/start', { method: 'POST' });
      aiRunning = true;
      aiAttachStream();
    } else {
      aiDetachStream();
      await api('/api/ai/stop', { method: 'POST' });
      aiRunning = false;
      els.aiView.hidden = true;
      els.aiList.innerHTML = '';
      els.aiStats.innerHTML = '';
    }
    aiPill(aiRunning);
    els.aiToggle.textContent = aiRunning ? 'Stop camera' : 'Start camera';
  } catch (err) {
    banner('error', err.message);
  } finally {
    els.aiToggle.disabled = false;
  }
});

/* ---- webcam (hardware H.264 → fragmented MP4 via MSE) ----
 *
 * The board sends a bare stream of packets on port 81: an 8-byte header
 * (uint32 LE payload length, uint32 LE PTS in ms) followed by one H.264
 * Annex-B access unit straight from the P4's hardware encoder. Each access
 * unit is remuxed here into a one-sample fragmented-MP4 segment and fed to
 * a <video> element through Media Source Extensions.
 *
 * MSE rather than WebCodecs on purpose: WebCodecs is restricted to secure
 * contexts, and this dashboard is served over plain http:// on the LAN, so
 * VideoDecoder simply does not exist here. MSE works on insecure origins
 * and still hands decoding to the browser's hardware decoder.
 */
let camRunning = false;
let camPoll = null;
let camAbort = null;
let camSeq = 0;

const CAM_SAMPLE_MS = 40;   // nominal frame duration (board encodes at 25 fps)

function camPill(on) {
  els.camPill.innerHTML = '';
  const pill = document.createElement('span');
  pill.className = `pill ${on ? 'pill-ok' : 'pill-off'}`;
  pill.textContent = on ? 'Running' : 'Stopped';
  els.camPill.appendChild(pill);
}

/* -- minimal fMP4 writer -- */
const camStr = (s) => [...s].map((c) => c.charCodeAt(0));
const camU16 = (v) => [(v >> 8) & 255, v & 255];
const camU32 = (v) => [(v >>> 24) & 255, (v >>> 16) & 255, (v >>> 8) & 255, v & 255];

function camBox(type, ...parts) {
  let size = 8;
  for (const p of parts) size += p.length;
  const out = new Uint8Array(size);
  out.set(camU32(size), 0);
  out.set(camStr(type), 4);
  let o = 8;
  for (const p of parts) { out.set(p, o); o += p.length; }
  return out;
}

/* Split an Annex-B access unit into NAL units (start codes stripped). */
function camParseNALs(au) {
  const nals = [];
  let start = -1;
  for (let i = 0; i + 2 < au.length;) {
    if (au[i] === 0 && au[i + 1] === 0 &&
        (au[i + 2] === 1 || (au[i + 2] === 0 && i + 3 < au.length && au[i + 3] === 1))) {
      const sc = au[i + 2] === 1 ? 3 : 4;
      if (start >= 0) nals.push(au.subarray(start, i));
      start = i + sc;
      i += sc;
    } else {
      i++;
    }
  }
  if (start >= 0) nals.push(au.subarray(start));
  return nals;
}

/* Same NALs with 4-byte big-endian length prefixes (AVCC), as MP4 wants. */
function camToAvcc(nals) {
  let size = 0;
  for (const n of nals) size += 4 + n.length;
  const out = new Uint8Array(size);
  let o = 0;
  for (const n of nals) {
    out.set(camU32(n.length), o);
    out.set(n, o + 4);
    o += 4 + n.length;
  }
  return out;
}

function camInitSegment(sps, pps, width, height) {
  const matrix = [
    ...camU32(0x10000), ...camU32(0), ...camU32(0),
    ...camU32(0), ...camU32(0x10000), ...camU32(0),
    ...camU32(0), ...camU32(0), ...camU32(0x40000000),
  ];
  const ftyp = camBox('ftyp', camStr('isom'), camU32(0x200),
                      camStr('isom'), camStr('iso2'), camStr('avc1'), camStr('mp41'));
  const mvhd = camBox('mvhd', camU32(0), camU32(0), camU32(0),
                      camU32(1000) /* timescale: ms */, camU32(0),
                      camU32(0x10000), camU16(0x100), camU16(0),
                      camU32(0), camU32(0), matrix, new Uint8Array(24), camU32(2));
  const tkhd = camBox('tkhd', camU32(7), camU32(0), camU32(0), camU32(1),
                      camU32(0), camU32(0), camU32(0), camU32(0),
                      camU16(0), camU16(0), camU16(0), camU16(0), matrix,
                      camU32(width << 16), camU32(height << 16));
  const mdhd = camBox('mdhd', camU32(0), camU32(0), camU32(0),
                      camU32(1000), camU32(0), camU16(0x55c4), camU16(0));
  const hdlr = camBox('hdlr', camU32(0), camU32(0), camStr('vide'),
                      camU32(0), camU32(0), camU32(0), [...camStr('VideoHandler'), 0]);
  const avcC = camBox('avcC', [1, sps[1], sps[2], sps[3], 0xff, 0xe1],
                      camU16(sps.length), sps, [1], camU16(pps.length), pps);
  const avc1 = camBox('avc1', new Uint8Array(6), camU16(1), new Uint8Array(16),
                      camU16(width), camU16(height),
                      camU32(0x480000), camU32(0x480000), camU32(0), camU16(1),
                      new Uint8Array(32), camU16(0x18), camU16(0xffff), avcC);
  const stbl = camBox('stbl',
                      camBox('stsd', camU32(0), camU32(1), avc1),
                      camBox('stts', camU32(0), camU32(0)),
                      camBox('stsc', camU32(0), camU32(0)),
                      camBox('stsz', camU32(0), camU32(0), camU32(0)),
                      camBox('stco', camU32(0), camU32(0)));
  const dinf = camBox('dinf', camBox('dref', camU32(0), camU32(1),
                                     camBox('url ', camU32(1))));
  const minf = camBox('minf',
                      camBox('vmhd', camU32(1), camU16(0), camU16(0), camU16(0), camU16(0)),
                      dinf, stbl);
  const mdia = camBox('mdia', mdhd, hdlr, minf);
  const trak = camBox('trak', tkhd, mdia);
  const mvex = camBox('mvex',
                      camBox('trex', camU32(0), camU32(1), camU32(1),
                             camU32(CAM_SAMPLE_MS), camU32(0), camU32(0x01010000)));
  const moov = camBox('moov', mvhd, trak, mvex);

  const out = new Uint8Array(ftyp.length + moov.length);
  out.set(ftyp);
  out.set(moov, ftyp.length);
  return out;
}

function camMediaSegment(sample, ptsMs, key) {
  const mfhd = camBox('mfhd', camU32(0), camU32(++camSeq));
  const tfhd = camBox('tfhd', camU32(0x020000) /* base-is-moof */, camU32(1));
  const tfdt = camBox('tfdt', camU32(0), camU32(ptsMs));
  // trun is fixed-size (one sample: duration + size + flags), so the moof
  // size — and with it the data offset — is known before assembly.
  const moofSize = 8 + mfhd.length + 8 + tfhd.length + tfdt.length + 32;
  const trun = camBox('trun', camU32(0x701), camU32(1), camU32(moofSize + 8),
                      camU32(CAM_SAMPLE_MS), camU32(sample.length),
                      camU32(key ? 0x02000000 : 0x01010000));
  const moof = camBox('moof', mfhd, camBox('traf', tfhd, tfdt, trun));
  const mdat = camBox('mdat', sample);

  const out = new Uint8Array(moof.length + mdat.length);
  out.set(moof);
  out.set(mdat, moof.length);
  return out;
}

async function camAttach() {
  if (!('MediaSource' in window) ||
      !MediaSource.isTypeSupported('video/mp4; codecs="avc1.42E01E"')) {
    els.camNote.innerHTML =
      '<p class="empty">This browser cannot play H.264 video via MSE.</p>';
    return;
  }
  els.camNote.innerHTML = '';
  els.camView.hidden = false;
  if (!camPoll) camPoll = setInterval(camFetchStats, 2000);

  camAbort = new AbortController();
  camSeq = 0;

  const video = els.camVideo;
  let ms = null;
  let sb = null;
  let started = false;
  let dropUntilKey = false;
  // The MP4 timeline is synthetic and continuous (one tick per APPENDED
  // frame) rather than copied from board timestamps: a frame dropped on the
  // board or here must not leave a hole that stalls MSE playback.
  let nextDts = 0;
  const queue = [];

  const pump = () => {
    if (sb && !sb.updating && ms && ms.readyState === 'open' && queue.length) {
      try { sb.appendBuffer(queue.shift()); } catch { /* torn down mid-append */ }
    }
  };

  try {
    // Frame dimensions for the MP4 headers (the decoder itself trusts SPS).
    const st = await api('/api/cam/status');

    const res = await fetch(`http://${location.hostname}:81/h264`,
                            { signal: camAbort.signal });
    if (!res.ok) throw new Error(`Stream failed (${res.status})`);
    const reader = res.body.getReader();

    let pending = new Uint8Array(0);
    for (;;) {
      const { value, done } = await reader.read();
      if (done) break;

      // Append the network chunk, then peel off complete packets.
      const merged = new Uint8Array(pending.length + value.length);
      merged.set(pending);
      merged.set(value, pending.length);
      pending = merged;

      while (pending.length >= 8) {
        const view = new DataView(pending.buffer, pending.byteOffset);
        const len = view.getUint32(0, true);
        const pts = view.getUint32(4, true);
        if (pending.length < 8 + len) break;
        const au = pending.slice(8, 8 + len);
        pending = pending.subarray(8 + len);

        const nals = camParseNALs(au);
        const key = nals.some((n) => (n[0] & 0x1f) === 5);

        if (!started) {
          // The first IDR carries SPS+PPS — everything the init segment needs.
          const sps = nals.find((n) => (n[0] & 0x1f) === 7);
          const pps = nals.find((n) => (n[0] & 0x1f) === 8);
          if (!key || !sps || !pps) continue;

          const hex = (b) => b.toString(16).padStart(2, '0');
          const codec = `avc1.${hex(sps[1])}${hex(sps[2])}${hex(sps[3])}`;
          ms = new MediaSource();
          video.src = URL.createObjectURL(ms);
          await new Promise((r) => ms.addEventListener('sourceopen', r, { once: true }));
          URL.revokeObjectURL(video.src);
          sb = ms.addSourceBuffer(`video/mp4; codecs="${codec}"`);
          sb.mode = 'segments';
          sb.addEventListener('updateend', pump);
          queue.push(camInitSegment(sps, pps, st.width, st.height));
          started = true;
          video.play().catch(() => { /* autoplay is allowed: muted */ });
        }

        // If appends fall behind, drop WHOLE GOPs: a skipped delta frame
        // would break every later frame's references and smear the picture
        // until the next keyframe, so once one frame is dropped the rest of
        // its GOP must go too.
        if (queue.length > 30) dropUntilKey = true;
        if (dropUntilKey) {
          if (!key) continue;
          dropUntilKey = false;
        }
        queue.push(camMediaSegment(camToAvcc(nals), nextDts, key));
        nextDts += CAM_SAMPLE_MS;
        pump();

        // Bound buffered history so a long-running stream cannot chew
        // through browser memory.
        if (sb && !sb.updating && !queue.length && video.buffered.length) {
          const start = video.buffered.start(0);
          const end = video.buffered.end(video.buffered.length - 1);
          if (end - start > 30) {
            try { sb.remove(start, end - 10); } catch { /* busy — next pass */ }
          }
        }

        // Live-edge chase: never let the playhead drift behind the stream.
        if (video.buffered.length) {
          const end = video.buffered.end(video.buffered.length - 1);
          if (end - video.currentTime > 0.7) video.currentTime = end - 0.15;
        }
      }
    }
  } catch (err) {
    if (err.name !== 'AbortError') banner('error', err.message);
  } finally {
    try {
      if (ms && ms.readyState === 'open') ms.endOfStream();
    } catch { /* already closed */ }
    video.removeAttribute('src');
    video.load();
  }
}

function camDetach() {
  if (camPoll) { clearInterval(camPoll); camPoll = null; }
  if (camAbort) { camAbort.abort(); camAbort = null; }
}

async function camFetchStats() {
  try {
    const st = await api('/api/cam/status');
    els.camStats.innerHTML =
      `<span>${st.width}×${st.height} · ${st.fps.toFixed(1)} fps</span>` +
      `<span>H.264 ${(st.bitrate / 1000000).toFixed(1)} Mbit/s</span>`;
  } catch { /* transient */ }
}

async function camPageEnter() {
  try {
    const st = await api('/api/cam/status');
    camRunning = st.running;
    camPill(camRunning);
    els.camToggle.textContent = camRunning ? 'Stop camera' : 'Start camera';
    if (camRunning) camAttach();
  } catch { /* board unreachable; button stays as-is */ }
}

function camPageLeave() {
  // Leaving the page stops the stream + polling but NOT the pipeline —
  // stopping the camera is an explicit button press.
  camDetach();
}

els.camToggle.addEventListener('click', async () => {
  els.camToggle.disabled = true;
  try {
    if (!camRunning) {
      await api('/api/cam/start', { method: 'POST' });
      camRunning = true;
      camAttach();
    } else {
      camDetach();
      await api('/api/cam/stop', { method: 'POST' });
      camRunning = false;
      els.camView.hidden = true;
      els.camStats.innerHTML = '';
    }
    camPill(camRunning);
    els.camToggle.textContent = camRunning ? 'Stop camera' : 'Start camera';
  } catch (err) {
    banner('error', err.message);
  } finally {
    els.camToggle.disabled = false;
  }
});

/* ---- forget network ---- */
els.forget.addEventListener('click', async () => {
  if (!confirm('Forget the saved Wi-Fi network and restart into setup mode?')) {
    return;
  }
  try {
    await api('/api/forget', { method: 'POST' });
    banner('info', 'Network forgotten. The board is restarting into setup mode — ' +
                   'look for its setup Wi-Fi network in a few seconds.');
    els.forget.disabled = true;
  } catch (err) {
    banner('error', err.message);
  }
});

els.refreshFiles.addEventListener('click', () => loadFiles(cwd));

/* If the page is opened while the board is already recording (reload, second
 * device), pick the session up instead of showing a Record button that can
 * only answer "already running". */
async function resumeIfRecording() {
  try {
    const st = await api('/api/audio/state');
    if (st.recording) {
      recording = true;
      recStartedAt = Date.now() - st.elapsed * 1000;
      setButton('stop');
      showElapsed(st.elapsed);
      recTimer = setInterval(tickElapsed, 250);
      statePoll = setInterval(pollRecording, 10000);
      showPage('audio');   // land where the Stop button is
    }
  } catch { /* board unreachable; normal load path continues */ }
}

/* loadBoard always runs at startup: it feeds the header pill and the record
 * button's enabled state, which matter on every page. The Files listing loads
 * lazily when its page is opened (showPage), not up front. */
loadBoard();
resumeIfRecording();
showPage(location.hash.slice(1) || 'home');
setInterval(() => { if (!recording) loadBoard(); }, 10000);
