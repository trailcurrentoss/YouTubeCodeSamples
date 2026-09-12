// Home screen: a paged grid of every application on the system.
//
// The grid is built from .desktop files discovered by touchd, so installing a
// package puts it here with no change to this file.

import { api } from "./api.js";
import { clear, el, toast } from "./dom.js";

const COLUMNS = 4;
const ROWS = 3;
const PER_PAGE = COLUMNS * ROWS;

export class Home {
  constructor(node, shell) {
    this.node = node;
    this.shell = shell;
    this.apps = [];
    this.index = 0;      // D-pad selection, into the flat app list
    this.pagesNode = null;
  }

  title() { return "LinuxTouch"; }

  hints() {
    return [
      ["Start", "Open"],
      ["Select", "Switcher"],
      ["D-pad", "Move"],
    ];
  }

  async load() {
    try {
      const all = await api.call("apps.list");
      // Settings is a LinuxTouch screen rather than a .desktop entry, so it is
      // injected here. It sorts first deliberately: it is the one thing a user
      // needs to find on a device they have not set up yet.
      this.apps = [
        { id: "__settings__", name: "Settings", builtin: true },
        ...all.filter((a) => !a.hidden),
      ];
    } catch (err) {
      this.apps = [];
      toast(`Could not list apps: ${err.message}`);
    }
    this.render();
  }

  render() {
    clear(this.node);
    if (!this.apps.length) {
      this.node.append(el("div", { class: "empty" },
        "No applications found. Is touchd running?"));
      return;
    }

    const pageCount = Math.ceil(this.apps.length / PER_PAGE);
    this.pagesNode = el("div", { class: "pages", onscroll: () => this.updateDots() });

    for (let page = 0; page < pageCount; page++) {
      const slice = this.apps.slice(page * PER_PAGE, (page + 1) * PER_PAGE);
      this.pagesNode.append(
        el("div", { class: "page" }, slice.map((app, i) => this.tile(app, page * PER_PAGE + i))),
      );
    }

    this.dotsNode = el("div", { class: "dots" },
      Array.from({ length: pageCount }, (_, i) => el("i", { class: i === 0 ? "on" : "" })));

    this.node.append(this.pagesNode, this.dotsNode);
    this.select(this.index, { scroll: false });
  }

  tile(app, index) {
    // An icon may be missing entirely — plenty of Debian packages ship a
    // .desktop entry with an Icon= name no installed theme has. Falling back to
    // the first letter keeps the grid regular instead of leaving a hole.
    const art = app.builtin
      ? el("span", { class: "fallback", text: "⚙" })
      : app.icon_path
        ? el("img", {
            src: `/icon/${encodeURIComponent(app.id)}`,
            alt: "",
            onerror: (e) => e.target.replaceWith(
              el("span", { class: "fallback", text: app.name.slice(0, 1).toUpperCase() })),
          })
        : el("span", { class: "fallback", text: app.name.slice(0, 1).toUpperCase() });

    return el("button", {
      class: "tile",
      "data-index": index,
      onclick: () => { this.index = index; this.open(); },
    }, art, el("span", { class: "label", text: app.name }));
  }

  select(index, { scroll = true } = {}) {
    if (!this.apps.length) return;
    // Clamp rather than wrap. Wrapping from the last app back to the first is
    // disorienting when the grid is several pages long.
    this.index = Math.max(0, Math.min(this.apps.length - 1, index));
    for (const tile of this.node.querySelectorAll(".tile")) {
      tile.classList.toggle("selected", Number(tile.dataset.index) === this.index);
    }
    if (scroll && this.pagesNode) {
      const page = Math.floor(this.index / PER_PAGE);
      this.pagesNode.scrollTo({ left: page * 640, behavior: "smooth" });
    }
    this.updateDots();
  }

  updateDots() {
    if (!this.pagesNode || !this.dotsNode) return;
    const page = Math.round(this.pagesNode.scrollLeft / 640);
    [...this.dotsNode.children].forEach((dot, i) => dot.classList.toggle("on", i === page));
  }

  async open() {
    const app = this.apps[this.index];
    if (!app) return;
    if (app.builtin) {
      this.shell.show("settings");
      return;
    }
    try {
      await api.call("apps.launch", { id: app.id });
      // No "launching…" spinner. sway raises the window fullscreen the moment
      // it maps, so any progress UI here would be covered before it painted.
    } catch (err) {
      toast(`${app.name} did not start: ${err.message}`);
    }
  }

  // Returns true if the key was handled.
  key(name) {
    switch (name) {
      case "ArrowLeft":  this.select(this.index - 1); return true;
      case "ArrowRight": this.select(this.index + 1); return true;
      case "ArrowUp":    this.select(this.index - COLUMNS); return true;
      case "ArrowDown":  this.select(this.index + COLUMNS); return true;
      case "confirm":    this.open(); return true;
      default: return false;
    }
  }
}
