// Status bar: clock, screen title, network, volume.
//
// No battery indicator. The PocketTerm35 gives the Pi no way to read charge
// state, and a gauge stuck at "--" reads as a flat battery. See docs/hardware.md.

import { api } from "./api.js";
import { clear, el } from "./dom.js";

export class StatusBar {
  constructor(node) {
    this.node = node;
    this.title = "LinuxTouch";
    this.net = { connected: false, ssid: null, signal: 0, type: null };
    this.audio = { volume: 0, muted: false, available: false };
    this.hour24 = true;

    api.addEventListener("net.status", (e) => { this.net = e.detail; this.render(); });
    api.addEventListener("audio.changed", (e) => { this.audio = e.detail; this.render(); });

    // Tick on the minute boundary rather than every second: the clock shows no
    // seconds, so a per-second repaint is 60x the work for no visible change.
    const tick = () => {
      this.render();
      setTimeout(tick, 60000 - (Date.now() % 60000));
    };
    tick();
  }

  setTitle(title) {
    this.title = title;
    this.render();
  }

  async refresh() {
    try {
      this.net = await api.call("net.status");
      this.audio = await api.call("audio.get");
      this.hour24 = (await api.call("settings.get", { key: "24_hour_clock" })) !== false;
    } catch { /* offline; the bar still shows the clock */ }
    this.render();
  }

  render() {
    const now = new Date();
    const time = now.toLocaleTimeString([], {
      hour: "2-digit", minute: "2-digit", hour12: !this.hour24,
    });

    clear(this.node);
    this.node.append(
      el("span", { class: "sb-clock", text: time }),
      el("span", { class: "sb-title", text: this.title }),
      el("div", { class: "sb-right" },
        el("span", {
          class: `sb-net${this.net.connected ? "" : " offline"}`,
          text: netLabel(this.net),
        }),
        this.audio.available
          ? el("span", { class: "sb-vol", text: volLabel(this.audio) })
          : null,
      ),
    );
  }
}

function netLabel(net) {
  if (!net.connected) return "offline";
  if (net.type === "ethernet") return "eth";
  // Four bars of signal as blocks. Text rather than an SVG icon so it scales
  // with the font and needs no asset.
  const bars = net.signal >= 75 ? "▁▃▅▇"
             : net.signal >= 50 ? "▁▃▅"
             : net.signal >= 25 ? "▁▃"
             : "▁";
  return bars;
}

function volLabel(audio) {
  if (audio.muted) return "🔇";
  return `${audio.volume}%`;
}
