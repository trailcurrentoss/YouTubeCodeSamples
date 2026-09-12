// Settings: network, sound, display, apps, about, power.
//
// One screen with an internal view stack, so Back means "up one level" here and
// only leaves Settings from the root. Every value is owned by touchd; this file
// renders and calls, and never shells out.

import { api } from "./api.js";
import { bytes, clear, confirmSheet, duration, el, toast } from "./dom.js";

export class Settings {
  constructor(node, shell) {
    this.node = node;
    this.shell = shell;
    this.view = "root";
    this.index = 0;
    this.rows = [];          // the currently rendered, selectable rows
  }

  title() {
    return {
      root: "Settings", wifi: "Wi-Fi", sound: "Sound",
      display: "Display", apps: "Apps", about: "About",
    }[this.view] || "Settings";
  }

  hints() {
    if (this.view === "wifi-join") {
      // Inside a text field the letter keys type, so the legend must not
      // advertise them. Enter and Esc are real keys on this keyboard.
      return [["Enter", "Join"], ["Esc", "Cancel"], ["Select", "Back"]];
    }
    return [["Start", "Open"], ["Select", "Back"], ["D-pad", "Move"]];
  }

  async load() {
    this.view = "root";
    await this.renderRoot();
  }

  // Back: up one level inside Settings, or out to home from the root.
  // Returns true if it was handled here.
  back() {
    if (this.view === "root") return false;
    this.view = "root";
    this.renderRoot();
    return true;
  }

  key(name) {
    if (this.view === "wifi-join") {
      // Arrows move the caret inside the field; nothing here should intercept.
      return false;
    }
    switch (name) {
      case "ArrowUp":   this.select(this.index - 1); return true;
      case "ArrowDown": this.select(this.index + 1); return true;
      case "confirm":   this.rows[this.index]?.click(); return true;
      default: return false;
    }
  }

  select(index) {
    if (!this.rows.length) return;
    this.index = Math.max(0, Math.min(this.rows.length - 1, index));
    this.rows.forEach((row, i) => row.classList.toggle("selected", i === this.index));
    this.rows[this.index].scrollIntoView({ block: "nearest" });
  }

  // Render a list and register its rows as the selectable set.
  paint(children) {
    clear(this.node);
    const list = el("div", { class: "list" }, children);
    this.node.append(list);
    this.rows = [...list.querySelectorAll(".row")];
    this.index = 0;
    this.select(0);
    this.shell.refreshChrome();
  }

  row({ title, sub, value, onclick, chevron = true }) {
    return el("button", { class: "row", onclick },
      el("div", { class: "row-main" },
        el("div", { class: "row-title", text: title }),
        sub ? el("div", { class: "row-sub", text: sub }) : null),
      value ? el("div", { class: "row-value", text: value }) : null,
      chevron ? el("div", { class: "chev", text: "›" }) : null,
    );
  }

  // ── root ────────────────────────────────────────────────────────

  async renderRoot() {
    const [net, audio] = await Promise.all([
      api.call("net.status").catch(() => ({ connected: false })),
      api.call("audio.get").catch(() => ({ volume: 0, available: false })),
    ]);

    this.paint([
      this.row({
        title: "Wi-Fi",
        value: net.connected ? (net.ssid || "connected") : "off",
        onclick: () => this.openWifi(),
      }),
      this.row({
        title: "Sound",
        value: audio.available ? `${audio.volume}%` : "unavailable",
        onclick: () => this.openSound(),
      }),
      this.row({ title: "Display", onclick: () => this.openDisplay() }),
      this.row({ title: "Apps", sub: "Hide apps from the home screen",
                 onclick: () => this.openApps() }),
      this.row({ title: "About", onclick: () => this.openAbout() }),
      el("div", { class: "section-title", text: "Power" }),
      this.row({ title: "Restart", chevron: false, onclick: () => this.power("reboot") }),
      this.row({ title: "Shut down", chevron: false, onclick: () => this.power("shutdown") }),
    ]);
  }

  async power(action) {
    const shutdown = action === "shutdown";
    const ok = await confirmSheet({
      title: shutdown ? "Shut down?" : "Restart?",
      body: "Any unsaved work in running apps will be lost.",
      confirmLabel: shutdown ? "Shut down" : "Restart",
      danger: true,
    });
    if (!ok) return;
    try {
      // confirm:true is required by touchd as well; the daemon refuses without
      // it, so a mis-wired button cannot power the device off. See power.py.
      await api.call(`power.${action}`, { confirm: true });
    } catch (err) {
      toast(err.message);
    }
  }

  // ── wifi ────────────────────────────────────────────────────────

  async openWifi() {
    this.view = "wifi";
    this.paint([el("div", { class: "empty", text: "Scanning…" })]);
    let networks = [];
    try {
      networks = await api.call("net.scan");
    } catch (err) {
      this.paint([el("div", { class: "empty", text: `Scan failed: ${err.message}` })]);
      return;
    }
    if (!networks.length) {
      this.paint([el("div", { class: "empty", text: "No networks found." })]);
      return;
    }
    this.paint(networks.map((net) => this.row({
      title: net.ssid,
      sub: [net.active ? "connected" : null,
            net.known ? "saved" : null,
            net.secure ? net.security : "open"].filter(Boolean).join(" · "),
      value: `${net.signal}%`,
      onclick: () => this.joinWifi(net),
    })));
  }

  async joinWifi(net) {
    // A saved or open network needs no password, so join immediately rather
    // than showing an empty password box the user has to dismiss.
    if (net.known || !net.secure) {
      toast(`Connecting to ${net.ssid}…`);
      const result = await api.call("net.connect", { ssid: net.ssid }).catch((e) => ({ ok: false, error: e.message }));
      toast(result.ok ? `Connected to ${net.ssid}` : `Failed: ${result.error}`);
      if (result.ok) this.openWifi();
      return;
    }

    this.view = "wifi-join";
    const field = el("input", { type: "password", autocomplete: "off",
                                autocapitalize: "off", spellcheck: "false" });

    const submit = async () => {
      const password = field.value;
      if (!password) { toast("Enter the password"); return; }
      this.view = "wifi";
      this.paint([el("div", { class: "empty", text: `Connecting to ${net.ssid}…` })]);
      const result = await api
        .call("net.connect", { ssid: net.ssid, password })
        .catch((e) => ({ ok: false, error: e.message }));
      if (result.ok) {
        toast(`Connected to ${net.ssid}`);
        this.openWifi();
      } else {
        toast(`Failed: ${result.error}`);
        this.openWifi();
      }
    };

    field.addEventListener("keydown", (e) => {
      // Handled here rather than in the global router because the global router
      // must not see letters at all while a field has focus.
      if (e.key === "Enter") { e.preventDefault(); submit(); }
      if (e.key === "Escape") { e.preventDefault(); this.openWifi(); }
      e.stopPropagation();
    });

    clear(this.node);
    this.node.append(el("div", { class: "list" },
      el("div", { class: "section-title", text: `Password for ${net.ssid}` }),
      el("div", { style: "padding:10px 12px" }, field),
      el("div", { class: "field-note", text:
        "Type on the keyboard. Enter joins, Esc cancels. There is no on-screen "
        + "keyboard by design." }),
      el("div", { style: "padding:4px 12px", class: "sheet-actions" },
        el("button", { class: "btn", onclick: () => this.openWifi() }, "Cancel"),
        el("button", { class: "btn btn-primary", onclick: submit }, "Join")),
    ));
    this.rows = [];
    this.shell.refreshChrome();
    field.focus();
  }

  // ── sound ───────────────────────────────────────────────────────

  async openSound() {
    this.view = "sound";
    const audio = await api.call("audio.get").catch(() => ({ available: false, volume: 0 }));
    if (!audio.available) {
      this.paint([el("div", { class: "empty", text:
        "No audio sink. Is PipeWire running?" })]);
      return;
    }

    const slider = el("input", {
      type: "range", min: "0", max: "100", value: String(audio.volume),
      oninput: (e) => { label.textContent = `${e.target.value}%`; },
      // Commit on release, not on every input event: each change is a wpctl
      // process, and a dragged slider would spawn dozens.
      onchange: (e) => api.call("audio.set", { volume: Number(e.target.value) })
        .catch((err) => toast(err.message)),
    });
    const label = el("div", { class: "row-value", text: `${audio.volume}%` });

    this.paint([
      el("div", { class: "section-title", text: "Volume" }),
      el("div", { style: "padding:0 12px" }, slider, label),
      this.row({
        title: audio.muted ? "Unmute" : "Mute", chevron: false,
        onclick: async () => {
          await api.call("audio.setMuted", { muted: !audio.muted }).catch((e) => toast(e.message));
          this.openSound();
        },
      }),
    ]);
  }

  // ── display ─────────────────────────────────────────────────────

  async openDisplay() {
    this.view = "display";
    const state = await api.call("brightness.get").catch(() => null);
    if (!state || !state.available) {
      this.paint([el("div", { class: "empty", text:
        "Brightness control needs gammastep, which is not installed." })]);
      return;
    }

    const percent = Math.round(state.level * 100);
    const label = el("div", { class: "row-value", text: `${percent}% (dim)` });
    const slider = el("input", {
      type: "range",
      min: String(Math.round(state.minimum * 100)), max: "100",
      value: String(percent),
      oninput: (e) => { label.textContent = `${e.target.value}% (dim)`; },
      onchange: (e) => api.call("brightness.set", { level: Number(e.target.value) / 100 })
        .catch((err) => toast(err.message)),
    });

    this.paint([
      el("div", { class: "section-title", text: "Brightness" }),
      el("div", { style: "padding:0 12px" }, slider, label),
      // The wording comes from the daemon so there is exactly one copy of it,
      // and so it cannot drift into implying hardware backlight control.
      el("div", { class: "field-note", text: state.note }),
      el("div", { class: "section-title", text: "Panel" }),
      this.row({ title: "Resolution", value: "640 × 480", chevron: false, onclick: () => {} }),
      this.row({ title: "Connection", value: "HDMI", chevron: false, onclick: () => {} }),
    ]);
  }

  // ── apps ────────────────────────────────────────────────────────

  async openApps() {
    this.view = "apps";
    const apps = await api.call("apps.list").catch(() => []);
    this.paint([
      el("div", { class: "section-title", text: "Tap to show or hide on the home screen" }),
      ...apps.map((app) => this.row({
        title: app.name,
        sub: app.id,
        value: app.hidden ? "hidden" : "shown",
        chevron: false,
        onclick: async () => {
          await api.call("apps.setHidden", { id: app.id, hidden: !app.hidden })
            .catch((e) => toast(e.message));
          this.openApps();
        },
      })),
    ]);
  }

  // ── about ───────────────────────────────────────────────────────

  async openAbout() {
    this.view = "about";
    const info = await api.call("system.info").catch(() => null);
    if (!info) {
      this.paint([el("div", { class: "empty", text: "touchd is not answering." })]);
      return;
    }
    this.paint([
      this.row({ title: "Device", value: info.model, chevron: false, onclick: () => {} }),
      this.row({ title: "Hostname", value: info.hostname, chevron: false, onclick: () => {} }),
      this.row({ title: "OS", value: info.os, chevron: false, onclick: () => {} }),
      this.row({ title: "Kernel", value: info.kernel, chevron: false, onclick: () => {} }),
      this.row({ title: "Uptime", value: duration(info.uptime), chevron: false, onclick: () => {} }),
      this.row({ title: "Temperature",
                 value: info.temperature != null ? `${info.temperature} °C` : "—",
                 chevron: false, onclick: () => {} }),
      this.row({ title: "Memory",
                 value: `${bytes(info.memory.used)} / ${bytes(info.memory.total)}`,
                 chevron: false, onclick: () => {} }),
      this.row({ title: "Storage",
                 value: `${bytes(info.disk.free)} free`,
                 chevron: false, onclick: () => {} }),
      // Stated rather than omitted, so the absence reads as a hardware fact
      // instead of a missing feature.
      this.row({ title: "Battery", value: "not readable on this hardware",
                 chevron: false, onclick: () => {} }),
    ]);
  }
}
