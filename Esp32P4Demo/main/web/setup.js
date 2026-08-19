/*
 * Wi-Fi provisioning UI.
 *
 * Flow: scan → pick a network → enter password → the board verifies the
 * connection before saving, then reboots into normal operation.
 */
'use strict';

const $ = (id) => document.getElementById(id);

const els = {
  banner:     $('banner'),
  scanBody:   $('scan-body'),
  rescan:     $('rescan'),
  stepScan:   $('step-scan'),
  stepAuth:   $('step-auth'),
  stepDone:   $('step-done'),
  chosenSsid: $('chosen-ssid'),
  form:       $('connect-form'),
  password:   $('password'),
  pwToggle:   $('pw-toggle'),
  back:       $('back'),
  join:       $('join'),
  doneBody:   $('done-body'),
};

let chosen = null;

/* ---- helpers ---- */
function banner(kind, text) {
  if (!kind) { els.banner.innerHTML = ''; return; }
  const div = document.createElement('div');
  div.className = `banner banner-${kind}`;
  div.textContent = text;
  els.banner.innerHTML = '';
  els.banner.appendChild(div);
}

/* Map RSSI to four bars. -55 or better is full, -85 or worse is one. */
function barsFor(rssi) {
  const level = rssi >= -55 ? 4 : rssi >= -65 ? 3 : rssi >= -75 ? 2 : 1;
  const bars = document.createElement('span');
  bars.className = 'bars';
  for (let i = 1; i <= 4; i++) {
    const bar = document.createElement('i');
    if (i <= level) bar.className = 'on';
    bars.appendChild(bar);
  }
  return bars;
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

/* ---- step 1: scan ---- */
function renderNetworks(networks) {
  if (!networks.length) {
    els.scanBody.innerHTML =
      '<p class="empty">No networks found. Move closer to your router and rescan.</p>';
    return;
  }

  const list = document.createElement('ul');
  list.className = 'net-list';

  for (const net of networks) {
    const li = document.createElement('li');
    const btn = document.createElement('button');
    btn.type = 'button';
    btn.className = 'net';
    btn.setAttribute('aria-selected', 'false');

    btn.appendChild(barsFor(net.rssi));

    const name = document.createElement('span');
    name.className = 'net-name';
    name.textContent = net.ssid;
    const meta = document.createElement('span');
    meta.className = 'net-meta';
    meta.textContent = `${net.auth} · ch ${net.channel} · ${net.rssi} dBm`;
    name.appendChild(document.createElement('br'));
    name.appendChild(meta);
    btn.appendChild(name);

    if (!net.open) {
      const lock = document.createElement('span');
      lock.className = 'lock';
      lock.textContent = '🔒';
      lock.setAttribute('aria-label', 'secured');
      btn.appendChild(lock);
    }

    btn.addEventListener('click', () => choose(net));
    li.appendChild(btn);
    list.appendChild(li);
  }

  els.scanBody.innerHTML = '';
  els.scanBody.appendChild(list);
}

async function scan() {
  banner(null);
  els.rescan.disabled = true;
  els.scanBody.innerHTML =
    '<div class="empty center-row"><span class="spinner"></span> Scanning…</div>';
  try {
    const { networks } = await api('/api/scan');
    renderNetworks(networks);
  } catch (err) {
    els.scanBody.innerHTML = '<p class="empty">Scan failed.</p>';
    banner('error', err.message);
  } finally {
    els.rescan.disabled = false;
  }
}

/* ---- step 2: password ---- */
function choose(net) {
  chosen = net;
  els.chosenSsid.textContent = net.ssid;
  els.stepScan.hidden = true;
  els.stepAuth.hidden = false;
  banner(null);

  // An open network needs no password, so skip straight to the join button.
  els.password.parentElement.parentElement.hidden = net.open;
  els.password.value = '';
  if (!net.open) els.password.focus();
}

els.back.addEventListener('click', () => {
  els.stepAuth.hidden = true;
  els.stepScan.hidden = false;
  banner(null);
});

els.pwToggle.addEventListener('click', () => {
  const showing = els.password.type === 'text';
  els.password.type = showing ? 'password' : 'text';
  els.pwToggle.textContent = showing ? 'Show' : 'Hide';
});

els.form.addEventListener('submit', async (event) => {
  event.preventDefault();
  if (!chosen) return;

  els.join.disabled = true;
  els.join.innerHTML = '<span class="spinner"></span> Checking…';
  banner('info', `Testing the connection to ${chosen.ssid}. This can take a few seconds.`);

  try {
    const result = await api('/api/connect', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ ssid: chosen.ssid, password: els.password.value }),
    });

    els.stepAuth.hidden = true;
    els.stepDone.hidden = false;
    banner('success', `Connected to ${result.ssid}.`);

    els.doneBody.innerHTML = '';
    for (const [label, value] of [
      ['Network', result.ssid],
      ['IP address', result.ip],
      ['Signal', `${result.rssi} dBm`],
    ]) {
      const row = document.createElement('div');
      row.className = 'kv';
      const dt = document.createElement('dt');
      dt.textContent = label;
      const dd = document.createElement('dd');
      dd.textContent = value;
      row.append(dt, dd);
      els.doneBody.appendChild(row);
    }
  } catch (err) {
    banner('error', err.message);
  } finally {
    els.join.disabled = false;
    els.join.textContent = 'Join network';
  }
});

els.rescan.addEventListener('click', scan);
scan();
