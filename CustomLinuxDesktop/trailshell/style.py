"""Builds the GTK3 stylesheet from the design tokens in :mod:`theme`.

Keeping the CSS generated (rather than a hand-maintained .css file) means the
tokens in theme.py really are the only place a colour or size is decided, so
retuning UI_SCALE on the glass reflows the entire shell.
"""
from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gdk, Gtk

from . import theme as T

C = T.C


def _css() -> str:
    return f"""
* {{
    font-family: {T.FONT_FALLBACK};
    /* Sub-pixel niceties are wasted at 228 PPI and cost fill rate. */
    outline: none;
}}

window, .ts-root {{
    background-color: {C['bg']};
    color: {C['text']};
    font-size: {T.TEXT_BASE}px;
}}

/* ---------------------------------------------------------------- type -- */
.ts-xs     {{ font-size: {T.TEXT_XS}px; }}
.ts-sm     {{ font-size: {T.TEXT_SM}px; }}
.ts-base   {{ font-size: {T.TEXT_BASE}px; }}
.ts-lg     {{ font-size: {T.TEXT_LG}px; }}
.ts-xl     {{ font-size: {T.TEXT_XL}px; }}
.ts-2xl    {{ font-size: {T.TEXT_2XL}px; }}
.ts-medium   {{ font-weight: {T.WEIGHT_MEDIUM}; }}
.ts-semibold {{ font-weight: {T.WEIGHT_SEMIBOLD}; }}
.ts-bold     {{ font-weight: {T.WEIGHT_BOLD}; }}
.ts-dim    {{ color: {C['text_dim']}; }}
.ts-mute   {{ color: {C['text_mute']}; }}
.ts-accent {{ color: {C['accent']}; }}
.ts-danger {{ color: {C['danger']}; }}

/* -------------------------------------------------------- status bar --- */
.ts-statusbar {{
    background-color: {C['bg']};
    color: {C['text']};
    font-size: {T.TEXT_XS}px;
    font-weight: {T.WEIGHT_MEDIUM};
    padding: 0 {T.SP_3}px;
    border-bottom: 1px solid {C['surface']};
}}
.ts-statusbar.ts-pulled {{ background-color: {C['surface']}; }}
.ts-status-clock {{ font-weight: {T.WEIGHT_SEMIBOLD}; }}

/* ---------------------------------------------------------- nav bar ---- */
.ts-navbar {{
    background-color: {C['bg']};
    border-top: 1px solid {C['surface']};
}}
.ts-navzone {{ background-color: transparent; }}
.ts-navzone:active {{ background-color: {C['surface_alt']}; }}

/* ------------------------------------------------------------ shade ---- */
.ts-shade {{ background-color: {C['surface']}; }}
.ts-shade-header {{
    padding: {T.SP_4}px {T.SP_4}px {T.SP_2}px {T.SP_4}px;
}}
.ts-shade-handle {{
    background-color: {C['border_strong']};
    border-radius: {T.RADIUS_FULL}px;
    min-height: {T.sp(5)}px;
    min-width: {T.sp(80)}px;
}}

/* Quick-settings tiles: big, square-ish, unmistakably on or off. */
.ts-qs-tile {{
    background-color: {C['surface_alt']};
    color: {C['text_dim']};
    border: 1px solid transparent;
    border-radius: {T.RADIUS_LG}px;
    min-height: {T.sp(68)}px;
    padding: {T.SP_2}px;
}}
.ts-qs-tile:active {{ background-color: {C['surface_hi']}; }}
.ts-qs-tile.ts-on {{
    background-color: {C['accent']};
    color: {C['on_accent']};
    border-color: {C['accent_press']};
}}
.ts-qs-label {{ font-size: {T.TEXT_XS}px; font-weight: {T.WEIGHT_MEDIUM}; }}

/* --------------------------------------------------------- controls ---- */
.ts-btn {{
    background-color: {C['surface_alt']};
    color: {C['text']};
    border: 1px solid {C['border_strong']};
    border-radius: {T.RADIUS_MD}px;
    min-height: {T.BTN_H}px;
    padding: 0 {T.SP_4}px;
    font-size: {T.TEXT_BASE}px;
    font-weight: {T.WEIGHT_MEDIUM};
}}
.ts-btn:active, .ts-btn:hover {{ background-color: {C['surface_hi']}; }}
.ts-btn.ts-primary {{
    background-color: {C['accent']};
    color: {C['on_accent']};
    border-color: {C['accent']};
    font-weight: {T.WEIGHT_SEMIBOLD};
}}
.ts-btn.ts-primary:active {{ background-color: {C['accent_press']}; }}
.ts-btn.ts-destructive {{ background-color: {C['danger']}; color: #000000; border-color: {C['danger']}; }}
.ts-btn.ts-flat {{ background-color: transparent; border-color: transparent; }}
/* Opt out of the 64 px touch floor. Only for controls that stay easy to hit
   some other way - a full-width strip, or a large adjacent target. */
.ts-btn.ts-compact {{
    min-height: 0;
    padding: 0 {T.SP_2}px;
    font-size: {T.TEXT_SM}px;
}}
.ts-btn.ts-flat:active {{ background-color: {C['surface_alt']}; }}

/* Sliders need a fat trough and a fat grabber to be usable with a thumb. */
scale trough {{
    background-color: {C['surface_hi']};
    border-radius: {T.RADIUS_FULL}px;
    min-height: {T.sp(18)}px;
}}
scale highlight {{
    background-color: {C['accent']};
    border-radius: {T.RADIUS_FULL}px;
    min-height: {T.sp(18)}px;
}}
scale slider {{
    background-color: {C['text']};
    border-radius: {T.RADIUS_FULL}px;
    min-height: {T.sp(34)}px;
    min-width: {T.sp(34)}px;
    margin: -{T.sp(9)}px;
}}
scale {{ padding: {T.SP_2}px 0; }}

/* --------------------------------------------------------- list rows --- */
.ts-row {{
    background-color: transparent;
    min-height: {T.ROW_H}px;
    padding: 0 {T.SP_4}px;
    border-bottom: 1px solid {C['surface_alt']};
}}
.ts-row:active, .ts-row:selected {{ background-color: {C['surface_alt']}; }}
list, listbox {{ background-color: transparent; }}
listbox row {{ padding: 0; }}

/* ------------------------------------------------------------ cards ---- */
.ts-card {{
    background-color: {C['surface_alt']};
    border-radius: {T.RADIUS_LG}px;
    padding: {T.SP_3}px;
}}

/* --------------------------------------------------- notifications ----- */
.ts-notif {{
    background-color: {C['surface_alt']};
    border-left: {T.sp(4)}px solid {C['accent']};
    border-radius: {T.RADIUS_MD}px;
    padding: {T.SP_2}px {T.SP_2}px {T.SP_2}px {T.SP_3}px;
}}
.ts-notif.ts-urgent {{ border-left-color: {C['danger']}; }}
.ts-notif.ts-low    {{ border-left-color: {C['text_mute']}; }}
.ts-notif-title {{ font-size: {T.TEXT_SM}px; font-weight: {T.WEIGHT_SEMIBOLD}; color: {C['text']}; }}
.ts-notif-body  {{ font-size: {T.TEXT_XS}px; color: {C['text_dim']}; }}
.ts-notif-app   {{ font-size: {T.TEXT_XS}px; color: {C['accent']}; font-weight: {T.WEIGHT_MEDIUM}; }}

/* Floating banner variant, shown over the top of whatever app is running. */
.ts-banner {{
    background-color: {C['surface_hi']};
    border-radius: {T.RADIUS_LG}px;
    border: 1px solid {C['border_strong']};
    padding: {T.SP_3}px;
}}

/* -------------------------------------------------------- launcher ----- */
.ts-launcher {{ background-color: {C['bg']}; }}
.ts-app-cell {{
    background-color: transparent;
    border-radius: {T.RADIUS_LG}px;
    padding: {T.SP_2}px 0;
}}
.ts-app-cell:active {{ background-color: {C['surface_alt']}; }}
/* The top search hit is pre-selected so Enter always has an obvious target;
   this is what makes it visible. */
.ts-app-cell:selected {{
    background-color: {C['accent_soft']};
    border: {T.sp(2)}px solid {C['accent']};
}}
.ts-app-label {{
    font-size: {T.TEXT_XS}px;
    font-weight: {T.WEIGHT_MEDIUM};
    color: {C['text']};
}}

/* The search entry is driven by the physical keyboard - there is no OSK, so
   it is styled as a passive indicator of what you have typed, not a tap
   target that implies a keyboard will appear. */
.ts-search {{
    background-color: {C['surface_alt']};
    color: {C['text']};
    caret-color: {C['accent']};
    border: 1px solid {C['border_strong']};
    border-radius: {T.RADIUS_FULL}px;
    min-height: {T.SEARCH_H}px;
    /* Left padding clears the search glyph drawn inside the field. */
    padding: 0 {T.SP_4}px 0 {T.sp(48)}px;
    font-size: {T.TEXT_BASE}px;
}}
.ts-search:focus {{ border-color: {C['accent']}; }}
.ts-page-dot {{
    background-color: {C['surface_hi']};
    border-radius: {T.RADIUS_FULL}px;
    min-width: {T.sp(8)}px;
    min-height: {T.sp(8)}px;
}}
.ts-page-dot.ts-on {{ background-color: {C['accent']}; }}

/* -------------------------------------------------------- switcher ----- */
.ts-switch-card {{
    background-color: {C['surface_alt']};
    border: 1px solid {C['border_strong']};
    border-radius: {T.RADIUS_LG}px;
}}
.ts-switch-card.ts-cursor {{ border-color: {C['accent']}; background-color: {C['surface_hi']}; }}
.ts-switch-title {{ font-size: {T.TEXT_SM}px; font-weight: {T.WEIGHT_MEDIUM}; }}

/* ------------------------------------------------------------ misc ----- */
scrollbar {{ background-color: transparent; border: none; }}
scrollbar slider {{
    background-color: {C['border_strong']};
    border-radius: {T.RADIUS_FULL}px;
    min-width: {T.sp(10)}px;
    min-height: {T.sp(40)}px;
    border: none;
    margin: 0 {T.sp(2)}px;
}}
tooltip {{ background-color: {C['surface_hi']}; color: {C['text']}; }}
separator {{ background-color: {C['surface_alt']}; min-height: 1px; min-width: 1px; }}
"""


_provider = None


def apply(screen=None) -> None:
    """Install the generated stylesheet screen-wide. Safe to call repeatedly."""
    global _provider
    screen = screen or Gdk.Screen.get_default()
    if _provider is None:
        _provider = Gtk.CssProvider()
    _provider.load_from_data(_css().encode("utf-8"))
    Gtk.StyleContext.add_provider_for_screen(
        screen, _provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
    )


def _on_theme_changed(_name: str) -> None:
    if _provider is not None:
        apply()


T.on_theme_changed(_on_theme_changed)


def dump() -> str:
    """Return the stylesheet as text (used by tools/preview.py)."""
    return _css()
