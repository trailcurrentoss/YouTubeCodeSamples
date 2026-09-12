// Shell entry point: screen stack, global navigation, key routing.

import { api } from "./api.js";
import { cancelSheet, clear, el, sheetOpen, toast } from "./dom.js";
import { Home } from "./home.js";
import { Settings } from "./settings.js";
import { StatusBar } from "./statusbar.js";
import { Switcher } from "./switcher.js";

class Shell {
  constructor() {
    this.statusbar = new StatusBar(document.getElementById("statusbar"));
    this.hintsNode = document.getElementById("hints");

    this.screens = {
      home: new Home(document.getElementById("screen-home"), this),
      switcher: new Switcher(document.getElementById("screen-switcher"), this),
      settings: new Settings(document.getElementById("screen-settings"), this),
    };
    this.current = "home";

    this.wireNav();
    this.wireKeys();
  }

  async start() {
    // ?mock=1 swaps touchd for fixtures so the UI can be developed on a
    // workstation. Loaded dynamically so the module is never fetched on device.
    if (new URLSearchParams(location.search).has("mock")) {
      const mock = await import("./mock.js");
      mock.install(api);
    }

    api.addEventListener("open", async () => {
      await this.statusbar.refresh();
      await this.screens.home.load();
      this.refreshChrome();
    });
    api.addEventListener("close", () => {
      // Say so rather than freezing. Every call rejects while disconnected, so
      // without this the UI would look alive and do nothing.
      this.statusbar.setTitle("reconnecting…");
    });
    api.connect();
  }

  show(name) {
    for (const [key, screen] of Object.entries(this.screens)) {
      screen.node.hidden = key !== name;
    }
    this.current = name;
    this.screens[name].load?.();
    this.refreshChrome();
  }

  refreshChrome() {
    const screen = this.screens[this.current];
    this.statusbar.setTitle(screen.title());
    clear(this.hintsNode);
    for (const [key, label] of screen.hints()) {
      this.hintsNode.append(el("span", {}, el("b", { text: key }), label));
    }
  }

  // ── global navigation ───────────────────────────────────────────
  //
  // These arrive from touchd, which is POSTed by bin/linuxtouch-nav, which sway
  // runs from a keybind. The compositor is the only thing that ever sees the
  // raw key, which is why letters stay available to applications.

  wireNav() {
    api.addEventListener("nav", (event) => {
      const action = event.detail?.action;
      // shell_focused is sampled by touchd BEFORE it raises us, so it answers
      // "was the user looking at the shell, or at an app?" — which this page
      // cannot determine for itself.
      const wasFocused = event.detail?.shell_focused !== false;
      if (action === "home") this.onHome(wasFocused);
      else if (action === "back") this.onBack();
      else if (action === "switcher") this.show("switcher");
    });
  }

  onHome(wasFocused = true) {
    if (sheetOpen()) { cancelSheet(); return; }
    // Coming out of an app, Start always means home — never the switcher, even
    // if home happened to be the last screen rendered underneath.
    if (!wasFocused) { this.show("home"); return; }
    // Already looking at the shell: Start from the home screen opens the
    // switcher. That gives one button both roles without a hold gesture, which
    // a compositor keybind cannot detect anyway.
    if (this.current === "home") this.show("switcher");
    else this.show("home");
  }

  onBack() {
    if (sheetOpen()) { cancelSheet(); return; }
    // A screen may consume Back to go up one level inside itself; Settings does.
    if (this.screens[this.current].back?.()) {
      this.refreshChrome();
      return;
    }
    this.show("home");
  }

  // ── keys ────────────────────────────────────────────────────────

  wireKeys() {
    window.addEventListener("keydown", (event) => {
      // A focused text field owns every key, including the arrows. Nothing here
      // may intercept, or the Wi-Fi password box would lose its caret keys.
      const tag = document.activeElement?.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA") return;

      let name = event.key;

      // Pause and Print are normally consumed by the compositor and never reach
      // this page. They are handled anyway so the shell also works when run
      // nested inside another compositor for testing, and in `make mock`.
      if (name === "Pause") { event.preventDefault(); this.onHome(); return; }
      if (name === "PrintScreen") { event.preventDefault(); this.onBack(); return; }
      if (name === "Escape") { event.preventDefault(); this.onBack(); return; }

      // Enter is a real key on this keyboard and carries no letter, so it is
      // safe as a confirm alias. A/B/X/Y deliberately are NOT bound anywhere:
      // they are the literal letters and belong to whatever is typing.
      if (name === "Enter") name = "confirm";

      if (this.screens[this.current].key?.(name)) event.preventDefault();
    });
  }
}

const shell = new Shell();
shell.start().catch((err) => toast(String(err)));
