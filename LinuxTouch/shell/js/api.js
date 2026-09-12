// touchd client.
//
// One WebSocket, request/response by id, plus server-pushed events. The shell
// never runs a command and never parses command output — if something needs a
// subprocess, it belongs in a touchd module. See docs/architecture.md.

const PORT = location.port || "8720";

export class Api extends EventTarget {
  constructor() {
    super();
    this.ws = null;
    this.pending = new Map();
    this.nextId = 1;
    this.connected = false;
    // Reconnect delay grows to a ceiling. `make deploy` restarts touchd while
    // the shell stays open, so reconnecting has to be automatic — but a tight
    // retry loop against a daemon that is down for good would spin a CPU core
    // on a passively-cooled handheld.
    this.backoff = 250;
  }

  connect() {
    this.ws = new WebSocket(`ws://127.0.0.1:${PORT}/ws`);

    this.ws.addEventListener("open", () => {
      this.connected = true;
      this.backoff = 250;
      this.dispatchEvent(new CustomEvent("open"));
    });

    this.ws.addEventListener("message", (event) => {
      let msg;
      try { msg = JSON.parse(event.data); } catch { return; }

      if (msg.event) {
        this.dispatchEvent(new CustomEvent(msg.event, { detail: msg.data }));
        return;
      }
      const entry = this.pending.get(msg.id);
      if (!entry) return;
      this.pending.delete(msg.id);
      msg.ok ? entry.resolve(msg.result) : entry.reject(new Error(msg.error));
    });

    this.ws.addEventListener("close", () => {
      this.connected = false;
      // Fail every in-flight call rather than leaving callers awaiting a reply
      // that can never arrive. A spinner that never stops is worse than an
      // error, because it gives the user nothing to act on.
      for (const { reject } of this.pending.values()) {
        reject(new Error("touchd disconnected"));
      }
      this.pending.clear();
      this.dispatchEvent(new CustomEvent("close"));
      setTimeout(() => this.connect(), this.backoff);
      this.backoff = Math.min(this.backoff * 2, 5000);
    });
  }

  call(method, params = {}) {
    if (!this.connected) return Promise.reject(new Error("touchd disconnected"));
    const id = this.nextId++;
    return new Promise((resolve, reject) => {
      this.pending.set(id, { resolve, reject });
      this.ws.send(JSON.stringify({ id, method, params }));
      // Every call is bounded. A module that hangs (a wifi scan on a saturated
      // band, say) must not leave the UI element that triggered it disabled
      // forever.
      setTimeout(() => {
        if (this.pending.has(id)) {
          this.pending.delete(id);
          reject(new Error(`${method} timed out`));
        }
      }, 30000);
    });
  }
}

export const api = new Api();
