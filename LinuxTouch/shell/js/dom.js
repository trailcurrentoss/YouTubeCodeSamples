// Minimal DOM helpers. No framework: the whole shell is a handful of screens,
// and a framework's runtime would cost more than the code it replaced.

export function el(tag, attrs = {}, ...children) {
  const node = document.createElement(tag);
  for (const [key, value] of Object.entries(attrs)) {
    if (key === "class") node.className = value;
    else if (key === "text") node.textContent = value;
    else if (key.startsWith("on")) node.addEventListener(key.slice(2), value);
    else if (value === true) node.setAttribute(key, "");
    else if (value !== false && value != null) node.setAttribute(key, value);
  }
  for (const child of children.flat()) {
    if (child == null) continue;
    node.append(child instanceof Node ? child : document.createTextNode(child));
  }
  return node;
}

export function clear(node) {
  while (node.firstChild) node.removeChild(node.firstChild);
}

let toastTimer = null;

export function toast(message) {
  const node = document.getElementById("toast");
  node.textContent = message;
  node.hidden = false;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => { node.hidden = true; }, 2600);
}

// A confirmation sheet. Resolves true/false, and resolves false on Back so a
// sheet can never trap the user.
export function confirmSheet({ title, body, confirmLabel = "Confirm", danger = false }) {
  const sheet = document.getElementById("sheet");
  clear(sheet);
  sheet.hidden = false;

  return new Promise((resolve) => {
    const finish = (value) => {
      sheet.hidden = true;
      clear(sheet);
      sheet.dispatchEvent(new CustomEvent("resolved"));
      resolve(value);
    };
    sheet.__cancel = () => finish(false);

    sheet.append(el("div", { class: "sheet-card" },
      el("h2", { text: title }),
      body ? el("p", { text: body }) : null,
      el("div", { class: "sheet-actions" },
        el("button", { class: "btn", onclick: () => finish(false) }, "Cancel"),
        el("button", {
          class: danger ? "btn btn-danger" : "btn btn-primary",
          onclick: () => finish(true),
        }, confirmLabel),
      ),
    ));
  });
}

export function sheetOpen() {
  return !document.getElementById("sheet").hidden;
}

export function cancelSheet() {
  const sheet = document.getElementById("sheet");
  if (!sheet.hidden && sheet.__cancel) sheet.__cancel();
}

export function bytes(value) {
  if (!value) return "0 B";
  const units = ["B", "KB", "MB", "GB", "TB"];
  const index = Math.min(Math.floor(Math.log(value) / Math.log(1024)), units.length - 1);
  return `${(value / 1024 ** index).toFixed(index === 0 ? 0 : 1)} ${units[index]}`;
}

export function duration(seconds) {
  const days = Math.floor(seconds / 86400);
  const hours = Math.floor((seconds % 86400) / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  if (days) return `${days}d ${hours}h`;
  if (hours) return `${hours}h ${minutes}m`;
  return `${minutes}m`;
}
