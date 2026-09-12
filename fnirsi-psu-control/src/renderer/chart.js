'use strict';

/**
 * Minimal dual-axis strip chart on a canvas. No dependencies.
 *
 * Replaces the vendor software's System.Windows.Forms.DataVisualization chart,
 * including its wheel-zoom and drag-pan behaviour and its habit of following
 * the live edge until the user interacts.
 */
class StripChart {
  /** @param {HTMLCanvasElement} canvas */
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');

    /** @type {{v:number, a:number}[]} */
    this.points = [];
    this.showV = true;
    this.showA = true;

    /** Number of samples visible. */
    this.window = 120;
    /** Index of the left edge; null means "follow the live edge". */
    this.offset = null;

    this.dragging = false;
    this.dragX = 0;
    this.dragOffset = 0;

    this._bindEvents();
    this._resize();
    new ResizeObserver(() => {
      this._resize();
      this.draw();
    }).observe(canvas);
  }

  _bindEvents() {
    const c = this.canvas;

    c.addEventListener(
      'wheel',
      (e) => {
        e.preventDefault();
        const factor = e.deltaY > 0 ? 1.25 : 0.8;
        const next = Math.round(this.window * factor);
        this.window = Math.max(10, Math.min(20000, next));
        if (this.offset !== null) {
          this.offset = Math.min(this.offset, this._maxOffset());
        }
        this.draw();
      },
      { passive: false },
    );

    c.addEventListener('mousedown', (e) => {
      this.dragging = true;
      this.dragX = e.clientX;
      this.dragOffset = this.offset === null ? this._maxOffset() : this.offset;
    });

    window.addEventListener('mousemove', (e) => {
      if (!this.dragging) return;
      const perPixel = this.window / Math.max(1, this._plotWidth());
      const moved = Math.round((e.clientX - this.dragX) * perPixel);
      this.offset = Math.max(
        0,
        Math.min(this._maxOffset(), this.dragOffset - moved),
      );
      this.draw();
    });

    window.addEventListener('mouseup', () => {
      this.dragging = false;
    });

    // Double-click snaps back to following the live edge.
    c.addEventListener('dblclick', () => {
      this.offset = null;
      this.window = 120;
      this.draw();
    });
  }

  _resize() {
    const dpr = window.devicePixelRatio || 1;
    const rect = this.canvas.getBoundingClientRect();
    this.canvas.width = Math.max(1, Math.round(rect.width * dpr));
    this.canvas.height = Math.max(1, Math.round(rect.height * dpr));
    this.ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    this.w = rect.width;
    this.h = rect.height;
  }

  _plotWidth() {
    return Math.max(1, this.w - 54);
  }

  _maxOffset() {
    return Math.max(0, this.points.length - this.window);
  }

  /** Append a sample and redraw. */
  push(v, a) {
    this.points.push({ v, a });
    if (this.points.length > 200000) this.points.splice(0, 50000);
    this.draw();
  }

  clear() {
    this.points = [];
    this.offset = null;
    this.draw();
  }

  draw() {
    const { ctx } = this;
    const padL = 46;
    const padR = 8;
    const padT = 10;
    const padB = 22;
    const w = this.w;
    const h = this.h;

    ctx.clearRect(0, 0, w, h);
    ctx.fillStyle = '#0e141b';
    ctx.fillRect(0, 0, w, h);

    const start = this.offset === null ? this._maxOffset() : this.offset;
    const slice = this.points.slice(start, start + this.window);

    // Scale to the visible slice, never below a small floor so a flat trace
    // at zero does not divide by zero.
    let max = 0;
    for (const p of slice) {
      if (this.showV) max = Math.max(max, p.v);
      if (this.showA) max = Math.max(max, p.a);
    }
    max = max > 0 ? max * 1.15 : 1;

    const plotW = w - padL - padR;
    const plotH = h - padT - padB;
    const xOf = (i) =>
      padL + (slice.length <= 1 ? 0 : (i / (slice.length - 1)) * plotW);
    const yOf = (val) => padT + plotH - (val / max) * plotH;

    // Grid and Y labels
    ctx.strokeStyle = '#1b2530';
    ctx.fillStyle = '#93a0b0';
    ctx.font = '10px ui-monospace, Menlo, Consolas, monospace';
    ctx.textAlign = 'right';
    ctx.textBaseline = 'middle';
    ctx.lineWidth = 1;
    for (let i = 0; i <= 4; i += 1) {
      const y = padT + (plotH * i) / 4;
      ctx.beginPath();
      ctx.moveTo(padL, y);
      ctx.lineTo(w - padR, y);
      ctx.stroke();
      ctx.fillText((max * (1 - i / 4)).toFixed(2), padL - 6, y);
    }

    // X label: sample index range
    ctx.textAlign = 'left';
    ctx.textBaseline = 'alphabetic';
    ctx.fillText(String(start), padL, h - 6);
    ctx.textAlign = 'right';
    ctx.fillText(String(start + slice.length), w - padR, h - 6);
    ctx.textAlign = 'center';
    ctx.fillText(
      this.offset === null ? 'live' : 'paused — double-click to resume',
      padL + plotW / 2,
      h - 6,
    );

    if (slice.length < 2) return;

    const line = (key, color) => {
      ctx.strokeStyle = color;
      ctx.lineWidth = 2;
      ctx.lineJoin = 'round';
      ctx.beginPath();
      slice.forEach((p, i) => {
        const x = xOf(i);
        const y = yOf(p[key]);
        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      });
      ctx.stroke();
    };

    if (this.showV) line('v', '#23c55e');
    if (this.showA) line('a', '#f0a63a');
  }
}

window.StripChart = StripChart;
