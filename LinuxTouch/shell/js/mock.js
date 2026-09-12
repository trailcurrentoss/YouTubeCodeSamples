// Fake daemon, for developing the shell on a workstation.
//
// Activated by ?mock=1 — `make mock` opens that URL. Nothing imports this file
// on the device, and it is never loaded unless the query parameter is present.
//
// The point is to make most UI work possible without hardware. It is NOT a
// simulator: it returns plausible fixtures and does not model window lifetimes
// or network state machines. Anything that depends on real behaviour still has
// to be checked on the device.

const APPS = [
  "Files", "Chromium", "Thunderbird", "Geany", "Terminal", "Calculator",
  "Text Editor", "Image Viewer", "Music", "Archive Manager", "PDF Viewer",
  "FreeCAD Viewer", "Task Manager", "Bluetooth", "Printers", "Screenshot",
].map((name, i) => ({
  id: `mock-${i}.desktop`,
  name,
  comment: `${name} (mock)`,
  categories: ["Utility"],
  terminal: false,
  icon: "",
  // No icon_path, so every tile exercises the letter fallback — which is the
  // path most likely to look wrong and least likely to be tested otherwise.
  icon_path: null,
  hidden: false,
}));

const WINDOWS = [
  { wid: "11", app_id: "thunderbird", title: "Inbox — 3 unread", pid: 4111, focused: false },
  { wid: "12", app_id: "chromium", title: "Raspberry Pi OS documentation", pid: 4222, focused: true },
  { wid: "13", app_id: "geany", title: "shell.css", pid: 4333, focused: false },
];

const NETWORKS = [
  { ssid: "workshop-5g", signal: 88, secure: true, security: "WPA2", active: true, known: true },
  { ssid: "workshop", signal: 71, secure: true, security: "WPA2", active: false, known: true },
  { ssid: "neighbour-2g", signal: 42, secure: true, security: "WPA2", active: false, known: false },
  { ssid: "guest-open", signal: 25, secure: false, security: "", active: false, known: false },
];

const state = {
  audio: { available: true, volume: 45, muted: false },
  brightness: {
    available: true, level: 1.0, minimum: 0.35,
    note: "Dims what is drawn. This panel has no backlight, so it reduces "
        + "glare but not battery use.",
  },
  settings: { hidden_apps: [], favorites: [], brightness: 1.0, "24_hour_clock": true },
};

const HANDLERS = {
  "apps.list": () => APPS.filter((a) => !state.settings.hidden_apps.includes(a.id))
                         .map((a) => ({ ...a, hidden: false })),
  "apps.launch": ({ id }) => ({ id, pid: 9999 }),
  "apps.setHidden": ({ id, hidden }) => {
    const set = new Set(state.settings.hidden_apps);
    hidden ? set.add(id) : set.delete(id);
    state.settings.hidden_apps = [...set];
    return { ok: true };
  },
  "wm.windows": () => WINDOWS,
  "wm.focus": () => ({ ok: true }),
  "wm.close": () => ({ ok: true }),
  "wm.home": () => ({ ok: true }),
  "net.status": () => ({
    connected: true, ssid: "workshop-5g", signal: 88, ip: "10.0.0.42",
    type: "wifi", radio: true,
  }),
  "net.scan": () => NETWORKS,
  "net.connect": ({ ssid }) => ({ ok: true, status: { connected: true, ssid } }),
  "net.forget": () => ({ ok: true }),
  "audio.get": () => state.audio,
  "audio.set": ({ volume }) => { state.audio.volume = volume; return state.audio; },
  "audio.setMuted": ({ muted }) => { state.audio.muted = muted; return state.audio; },
  "brightness.get": () => state.brightness,
  "brightness.set": ({ level }) => { state.brightness.level = level; return state.brightness; },
  "power.shutdown": () => ({ ok: false, error: "not powering off a mock device" }),
  "power.reboot": () => ({ ok: false, error: "not rebooting a mock device" }),
  "system.info": () => ({
    hostname: "mock", model: "Raspberry Pi 5 Model B Rev 1.0",
    os: "Debian GNU/Linux 13 (trixie)", kernel: "6.12.47+rpt-rpi-2712",
    uptime: 8130, temperature: 51.2,
    memory: { total: 8455716864, available: 7000000000, used: 1455716864 },
    disk: { total: 7301444608, free: 729444352, used: 6572000256 },
    battery: null,
  }),
  "settings.all": () => state.settings,
  "settings.get": ({ key }) => state.settings[key],
  "settings.set": ({ key, value }) => { state.settings[key] = value; return { ok: true }; },
};

export function install(api) {
  api.connect = () => {
    api.connected = true;
    // Deferred so listeners registered after connect() still see the event,
    // matching the real WebSocket's asynchronous open.
    setTimeout(() => api.dispatchEvent(new CustomEvent("open")), 0);
  };

  api.call = async (method, params = {}) => {
    const handler = HANDLERS[method];
    if (!handler) throw new Error(`mock: unknown method ${method}`);
    // A little latency, so loading states are visible during development
    // instead of resolving before the first paint.
    await new Promise((resolve) => setTimeout(resolve, 60));
    return handler(params);
  };

  // Start and Select do not exist on a laptop keyboard, and the real ones are
  // consumed by the compositor anyway. Home/End stand in for them here.
  window.addEventListener("keydown", (event) => {
    if (event.key === "Home") api.dispatchEvent(new CustomEvent("nav", { detail: { action: "home" } }));
    if (event.key === "End") api.dispatchEvent(new CustomEvent("nav", { detail: { action: "back" } }));
  });

  console.info("LinuxTouch: mock daemon installed. Home = Start, End = Select.");
}
