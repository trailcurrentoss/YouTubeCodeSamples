'use strict';

const { contextBridge, ipcRenderer } = require('electron');

/**
 * The renderer has no Node access. Everything it can do to the device goes
 * through this fixed surface, and every device command is name-checked again
 * in the main process.
 */
contextBridge.exposeInMainWorld('psu', {
  listPorts: () => ipcRenderer.invoke('ports:list'),
  connect: (port, baudRate, address) =>
    ipcRenderer.invoke('device:connect', { port, baudRate, address }),
  disconnect: () => ipcRenderer.invoke('device:disconnect'),
  snapshot: () => ipcRenderer.invoke('device:snapshot'),
  command: (name, ...args) => ipcRenderer.invoke('device:command', { name, args }),
  exportCsv: (suggestedName, csv) =>
    ipcRenderer.invoke('export:csv', { suggestedName, csv }),
  appInfo: () => ipcRenderer.invoke('app:info'),

  // Firmware update. enterBootloader is gated in the main process on a
  // firmware file having been chosen first — the renderer cannot bypass it.
  firmware: {
    choose: () => ipcRenderer.invoke('firmware:choose'),
    clear: () => ipcRenderer.invoke('firmware:clear'),
    enterBootloader: () => ipcRenderer.invoke('firmware:enter-bootloader'),
    copy: () => ipcRenderer.invoke('firmware:copy'),
  },

  onState: (fn) => ipcRenderer.on('device:state', (_e, patch) => fn(patch)),
  onStatus: (fn) => ipcRenderer.on('device:status', (_e, s) => fn(s)),
  onProtection: (fn) => ipcRenderer.on('device:protection', (_e, c) => fn(c)),
  onError: (fn) => ipcRenderer.on('device:error', (_e, m) => fn(m)),
});
