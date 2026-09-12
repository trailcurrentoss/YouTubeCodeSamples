'use strict';

const path = require('node:path');
const fs = require('node:fs/promises');
const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const { PowerSupply } = require('./device');
const proto = require('../shared/protocol');

/** @type {BrowserWindow|null} */
let win = null;
const psu = new PowerSupply();

function createWindow() {
  win = new BrowserWindow({
    width: 1180,
    height: 820,
    minWidth: 940,
    minHeight: 680,
    backgroundColor: '#11151c',
    title: 'FNIRSI PSU Control',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
      sandbox: false,
    },
  });

  win.setMenuBarVisibility(false);
  win.loadFile(path.join(__dirname, '..', 'renderer', 'index.html'));
}

/** Forward a device event to the renderer, if it is still there. */
function forward(channel, payload) {
  if (win && !win.isDestroyed()) {
    win.webContents.send(channel, payload);
  }
}

psu.on('state', (patch) => forward('device:state', patch));
psu.on('status', (status) => forward('device:status', status));
psu.on('protection', (code) => forward('device:protection', code));
psu.on('error', (err) => forward('device:error', String(err.message || err)));

app.whenReady().then(() => {
  createWindow();
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', async () => {
  await psu.disconnect();
  if (process.platform !== 'darwin') app.quit();
});

app.on('before-quit', async () => {
  await psu.disconnect();
});

/* -------------------------------------------------------------------------- */
/* IPC                                                                        */
/* -------------------------------------------------------------------------- */

ipcMain.handle('ports:list', () => PowerSupply.listPorts());

ipcMain.handle('device:connect', async (_e, { port, baudRate, address }) => {
  try {
    const ok = await psu.connect(port, baudRate, address);
    return { ok };
  } catch (err) {
    return { ok: false, error: String(err.message || err) };
  }
});

ipcMain.handle('device:disconnect', async () => {
  await psu.disconnect();
  return { ok: true };
});

ipcMain.handle('device:snapshot', () => ({
  connected: psu.connected,
  state: psu.state,
}));

ipcMain.handle('device:command', (_e, { name, args = [] }) => {
  const allowed = {
    setVoltage: (v) => psu.setVoltage(v),
    setCurrent: (a) => psu.setCurrent(a),
    setOutput: (on, setpoints) => psu.setOutput(on, setpoints),
    setMetering: (on) => psu.setMetering(on),
    setBrightness: (n) => psu.setBrightness(n),
    setProtections: (limits) => psu.setProtections(limits),
    saveGroup: (id, v, a) => psu.saveGroup(id, v, a),
    readAll: () => psu.readAll(),
    // enterBootloader is deliberately NOT exposed over IPC. See device.js.
    startPolling: (ms) => psu.startPolling(ms),
    stopPolling: () => psu.stopPolling(),
  };
  const fn = allowed[name];
  if (!fn) return { ok: false, error: `unknown command ${name}` };
  if (!psu.connected && name !== 'stopPolling') {
    return { ok: false, error: 'not connected' };
  }
  try {
    fn(...args);
    return { ok: true };
  } catch (err) {
    return { ok: false, error: String(err.message || err) };
  }
});

ipcMain.handle('export:csv', async (_e, { suggestedName, csv }) => {
  const result = await dialog.showSaveDialog(win, {
    title: 'Export recording',
    defaultPath: suggestedName,
    filters: [{ name: 'CSV', extensions: ['csv'] }],
  });
  if (result.canceled || !result.filePath) return { ok: false, canceled: true };
  try {
    // BOM so Excel reads the UTF-8 degree and unit symbols correctly.
    await fs.writeFile(result.filePath, '﻿' + csv, 'utf8');
    return { ok: true, path: result.filePath };
  } catch (err) {
    return { ok: false, error: String(err.message || err) };
  }
});

/* -------------------------------------------------------------------------- */
/* Firmware update                                                            */
/* -------------------------------------------------------------------------- */

/**
 * Firmware file the user has explicitly chosen this session.
 *
 * This is the hard gate on the bootloader command: `firmware:enter-bootloader`
 * refuses to do anything while this is null, so entering BOOT mode always
 * requires a deliberate file selection first. Clearing it after use means a
 * second update needs a second selection.
 */
let selectedFirmware = null;

ipcMain.handle('firmware:choose', async () => {
  const result = await dialog.showOpenDialog(win, {
    title: 'Select firmware file',
    properties: ['openFile'],
    filters: [
      { name: 'Firmware', extensions: ['bin', 'hex', 'dfu', 'fw'] },
      { name: 'All files', extensions: ['*'] },
    ],
  });
  if (result.canceled || !result.filePaths.length) {
    return { ok: false, canceled: true };
  }
  const filePath = result.filePaths[0];
  try {
    const stat = await fs.stat(filePath);
    if (!stat.isFile() || stat.size === 0) {
      return { ok: false, error: 'That is not a readable firmware file.' };
    }
    selectedFirmware = filePath;
    return {
      ok: true,
      path: filePath,
      name: path.basename(filePath),
      size: stat.size,
    };
  } catch (err) {
    return { ok: false, error: String(err.message || err) };
  }
});

ipcMain.handle('firmware:clear', () => {
  selectedFirmware = null;
  return { ok: true };
});

ipcMain.handle('firmware:enter-bootloader', async () => {
  // The gate: no chosen firmware file, no bootloader command. Ever.
  if (!selectedFirmware) {
    return { ok: false, error: 'Select a firmware file first.' };
  }
  if (!psu.connected) {
    return { ok: false, error: 'not connected' };
  }
  try {
    await fs.access(selectedFirmware);
  } catch {
    selectedFirmware = null;
    return { ok: false, error: 'The selected firmware file is no longer readable.' };
  }

  psu.enterBootloader();
  // The supply drops off the serial bus immediately; close the port cleanly
  // rather than leaving a dead handle open.
  setTimeout(() => psu.disconnect(/* notifyDevice */ false), 400);
  return { ok: true };
});

/**
 * Copy the chosen firmware onto the volume the supply exposes in BOOT mode.
 * The destination is picked by the user — this never guesses at a mount point,
 * because writing a firmware image to the wrong drive is not recoverable by
 * an undo button.
 */
ipcMain.handle('firmware:copy', async () => {
  if (!selectedFirmware) {
    return { ok: false, error: 'Select a firmware file first.' };
  }
  const result = await dialog.showOpenDialog(win, {
    title: 'Select the drive the power supply is showing',
    properties: ['openDirectory'],
  });
  if (result.canceled || !result.filePaths.length) {
    return { ok: false, canceled: true };
  }
  const target = path.join(result.filePaths[0], path.basename(selectedFirmware));
  try {
    await fs.copyFile(selectedFirmware, target);
    return { ok: true, path: target };
  } catch (err) {
    return { ok: false, error: String(err.message || err) };
  }
});

ipcMain.handle('app:info', () => ({
  version: app.getVersion(),
  electron: process.versions.electron,
  platform: process.platform,
  baudRates: proto.BAUD_RATES,
  groupCount: proto.GROUP_COUNT,
  brightnessMin: proto.BRIGHTNESS_MIN,
  brightnessMax: proto.BRIGHTNESS_MAX,
}));
