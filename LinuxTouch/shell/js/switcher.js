// App switcher: one card per running window.
//
// The window list comes from touchd, which gets it from the compositor along
// with a PID per window — that is what makes a card correspond to something the
// user actually launched. See touchd/wm/base.py.

import { api } from "./api.js";
import { clear, el, toast } from "./dom.js";

export class Switcher {
  constructor(node, shell) {
    this.node = node;
    this.shell = shell;
    this.windows = [];
    this.index = 0;

    api.addEventListener("wm.windows", (e) => {
      this.windows = e.detail || [];
      if (!this.node.hidden) this.render();
    });
  }

  title() { return "Running apps"; }

  hints() {
    return [
      ["Start", "Switch to"],
      ["Select", "Home"],
      ["D-pad", "Move"],
    ];
  }

  async load() {
    try {
      this.windows = await api.call("wm.windows");
    } catch (err) {
      this.windows = [];
      toast(err.message);
    }
    this.index = 0;
    this.render();
  }

  render() {
    clear(this.node);
    if (!this.windows.length) {
      this.node.append(el("div", { class: "empty" },
        "Nothing is running. Press Select to go back."));
      return;
    }

    this.cardsNode = el("div", { class: "cards" },
      this.windows.map((win, i) => this.card(win, i)));
    this.node.append(this.cardsNode);
    this.select(0);
  }

  card(win, index) {
    const node = el("button", {
      class: "card",
      "data-index": index,
      onclick: () => this.focus(index),
    },
      el("div", { class: "card-title", text: win.title || win.app_id }),
      el("div", { class: "card-app", text: win.app_id }),
      el("button", {
        class: "close",
        // The card is a button and so is this; without stopPropagation a tap on
        // the X would also fire the card's own click and switch to the app it
        // was closing.
        onclick: (e) => { e.stopPropagation(); this.close(index); },
      }, "✕"),
    );

    this.attachSwipe(node, index);
    return node;
  }

  // Swipe a card up to close it, matching the platform gesture. Pointer events
  // rather than touch events so this also works with a mouse in `make mock`.
  attachSwipe(node, index) {
    let startY = null;
    node.addEventListener("pointerdown", (e) => { startY = e.clientY; });
    node.addEventListener("pointerup", (e) => {
      if (startY === null) return;
      const travel = startY - e.clientY;
      startY = null;
      // 60px, well past a tap's jitter but reachable inside a 190px card.
      if (travel > 60) this.close(index);
    });
    node.addEventListener("pointercancel", () => { startY = null; });
  }

  select(index) {
    if (!this.windows.length) return;
    this.index = Math.max(0, Math.min(this.windows.length - 1, index));
    for (const card of this.node.querySelectorAll(".card")) {
      const on = Number(card.dataset.index) === this.index;
      card.classList.toggle("selected", on);
      if (on) card.scrollIntoView({ behavior: "smooth", inline: "center", block: "nearest" });
    }
  }

  async focus(index) {
    const win = this.windows[index];
    if (!win) return;
    try {
      await api.call("wm.focus", { wid: win.wid });
    } catch (err) {
      toast(err.message);
    }
  }

  async close(index) {
    const win = this.windows[index];
    if (!win) return;
    const card = this.node.querySelector(`.card[data-index="${index}"]`);
    if (card) card.classList.add("closing");
    try {
      await api.call("wm.close", { wid: win.wid });
      // No optimistic removal from this.windows: the app may refuse to close
      // (an editor with unsaved changes puts up its own dialog), and the
      // compositor's window event is the only thing that knows whether it
      // actually went away. If it did not, the card slides back into place.
      setTimeout(() => { if (card) card.classList.remove("closing"); }, 400);
    } catch (err) {
      if (card) card.classList.remove("closing");
      toast(err.message);
    }
  }

  key(name) {
    switch (name) {
      case "ArrowLeft":  this.select(this.index - 1); return true;
      case "ArrowRight": this.select(this.index + 1); return true;
      case "ArrowUp":    this.close(this.index); return true;
      case "confirm":    this.focus(this.index); return true;
      default: return false;
    }
  }
}
