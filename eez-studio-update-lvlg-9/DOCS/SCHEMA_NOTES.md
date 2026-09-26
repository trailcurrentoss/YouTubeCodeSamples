# LVGL 9 widget JSON in EEZ Studio 0.29.0

Notes taken while building this sample. Everything here was read out of
EEZ Studio 0.29.0's own bundled sources rather than inferred from behaviour,
because the LVGL 9 widgets are thinly documented and several of their fields
change meaning with the project's LVGL version.

Paths below are relative to `resources/app/build/project-editor/` inside an
EEZ Studio install (`--appimage-extract` an AppImage, then extract
`resources/app.asar`). The same files are at
<https://github.com/eez-open/studio> under `packages/project-editor/`, as
TypeScript.

- [Version strings](#version-strings)
- [Grid and flex layout](#grid-and-flex-layout)
- [Scale (LVGL 9 only)](#scale-lvgl-9-only)
- [Arc angles from expressions](#arc-angles-from-expressions)
- [Spangroup](#spangroup)
- [Static label text](#static-label-text)
- [Merged fonts](#merged-fonts)
- [Expressions without flow support](#expressions-without-flow-support)
- [Themes are runtime-switchable](#themes-are-runtime-switchable)

## Version strings

`settings.general.lvglVersion` accepts exactly five values
(`project/project.js`):

```
"8.4.0"  "9.2.2"  "9.3.0"  "9.4.0"  "9.5.0"
```

Older files are migrated on load: `8.3` and `8.3.0` become `8.4.0`, `9.0` and
`9.0.0` become `9.2.2`.

The string is not cosmetic. Widget code generators branch on it via
`isLVGLVersion([...])`, and flag bits, state bits, part codes and style-property
codes are all version-keyed tables in `lvgl/lvgl-constants.js`. The Spangroup
differences are the most visible — see [Spangroup](#spangroup).

`settings.general` in 0.29 also carries fields that did not exist in the 8.4-era
files:

| Field | Default | Notes |
|---|---|---|
| `colorBpp` | `"16"` | `"16"` or `"32"` |
| `bitmapColorFormat` | `"BGR"` for LVGL | replaces the old `colorFormat`, which is migrated into it |
| `embedBitmaps` | `true` | |
| `embedFonts` | `true` | |
| `cacheFonts` | `false` | 0.27; caches font definitions to a `<project>.eez-project-fonts-cache` sidecar |
| `circularDisplay` | `false` | suppresses `displayBorderRadius` |

## Grid and flex layout

Both are ordinary style properties, so they go in a style or in a widget's
`localStyles`, not on the widget itself. `lvgl/style-catalog.js`.

Container side:

| Property | Values |
|---|---|
| `layout` | `NONE` / `FLEX` / `GRID` |
| `grid_column_dsc_array`, `grid_row_dsc_array` | track list, see below |
| `grid_column_align`, `grid_row_align` | `START` `CENTER` `END` `STRETCH` `SPACE_EVENLY` `SPACE_AROUND` `SPACE_BETWEEN` |
| `flex_flow` | `ROW` `COLUMN` `ROW_WRAP` `ROW_REVERSE` `ROW_WRAP_REVERSE` `COLUMN_WRAP` `COLUMN_REVERSE` |
| `flex_main_place` | `START` `END` `CENTER` `SPACE_EVENLY` `SPACE_AROUND` `SPACE_BETWEEN` |
| `flex_cross_place` | `START` `END` `CENTER` |
| `flex_track_place` | as `flex_main_place` |
| `pad_row`, `pad_column` | gaps between tracks / flex items |

Child side:

| Property | Values |
|---|---|
| `grid_cell_column_pos`, `grid_cell_row_pos` | 0-based track index |
| `grid_cell_column_span`, `grid_cell_row_span` | ≥ 1 |
| `grid_cell_x_align`, `grid_cell_y_align` | as `grid_column_align` |
| `flex_grow` | integer |

### Track list syntax

Comma **or** space separated, case-insensitive
(`dscArrayValueBuild` in `style-catalog.js`):

| Token | Emits |
|---|---|
| `120` | `120` — fixed pixels |
| `FR(2)` | `LV_GRID_FR(2)` — share of the leftover space |
| `CONTENT` | `LV_GRID_CONTENT` — size to the children |

`LV_GRID_TEMPLATE_LAST` is appended automatically. Anything unparseable becomes
`0` silently, so a typo like `FR1` or `1FR` costs you the track rather than
raising an error.

This project's screen root uses `"FR(1)"` by one column and
`"CONTENT, FR(1), CONTENT"` by three rows; `body` uses
`"FR(1), FR(1), FR(1)"` by `"FR(1)"`.

A `CONTENT` row still needs something to measure. The header and footer pin
`min_height` / `max_height` in their local styles so the track resolves to a
known height.

## Scale (LVGL 9 only)

`lvgl/widgets/Scale.js`. `enabledInComponentPalette` requires
`lvglVersion.startsWith("9.")`, so Scale does not appear in an 8.4 project at
all — it is the replacement for the Meter widget that LVGL 9 removed.

Widget fields:

| Field | Notes |
|---|---|
| `scaleMode` | `HORIZONTAL_TOP` `HORIZONTAL_BOTTOM` `VERTICAL_LEFT` `VERTICAL_RIGHT` `ROUND_INNER` `ROUND_OUTER` |
| `minValue`, `maxValue` (+ `…Type`) | **integers only**; literal or expression |
| `angleRange`, `rotation` (+ `rotationType`) | round modes; defaults 270 and 135 |
| `totalTickCount`, `majorTickEvery` | |
| `showLabels`, `labelTexts` | `labelTexts` is a comma-separated list |
| `postDraw`, `drawTicksOnTop` | |
| `mainLineWidth/Color/Opacity` | hidden in the property grid for round modes |
| `mainArcWidth/Color/Opacity/Rounded/ImageSrc` | round modes only |
| `minorTicksLength/Width/Color/Opacity` | |
| `majorTicksLength/Width/Color/Opacity` | |
| `labelsTextColor/Opacity/Font` | |
| `sections` | array of `LVGLScaleSection` |

**Scale has no value or needle property.** It draws the ruler; it does not
indicate a reading. LVGL's own needle API
(`lv_scale_set_line_needle_value`) is not exposed. To show a live value, overlay
something that does have one — this project puts a concentric `LVGLArcWidget`
over the same integer range.

Because the range is integer-only, a voltage gauge has to pick a scaled unit.
This one uses decivolts (100 = 10.0 V, 140 = 14.0 V).

### Sections

`LVGLScaleSection`:

| Field | Notes |
|---|---|
| `identifier` | shown as "Name"; also becomes `codeIdentifier` in snake_case |
| `minValue`, `maxValue` (+ `…Type`) | integers, literal or expression |
| `useStyle` | **must** name a style whose `forWidgetType` is `LVGLScaleWidget` |
| `mainWidth/Color/Opacity` | per-section overrides, as an alternative to `useStyle` |
| `minorTicksWidth/Color/Opacity` | |
| `majorTicksWidth/Color/Opacity` | |
| `labelsTextColor/Opacity/Font` | |

`Scale.js`'s `check` hook rejects a `useStyle` whose `forWidgetType` is anything
else, with a message naming the offending widget type. That is why this project
carries `ScaleSectionDanger` / `ScaleSectionWarn` / `ScaleSectionGood` as
`LVGLScaleWidget` styles even though they only set line and arc colours.

## Arc angles from expressions

`lvgl/widgets/Arc.js`. Eight properties accept `literal` or `expression`:
`rangeMin`, `rangeMax`, `value`, `bgStartAngle`, `bgEndAngle`, `rotation`,
`startAngle`, `endAngle`.

`useAngle` picks which set is live:

- `useAngle: false` — `lv_arc_set_range()` plus `lv_arc_set_value()`. `startAngle`
  and `endAngle` are hidden in the property grid.
- `useAngle: true` — `lv_arc_set_start_angle()` / `lv_arc_set_end_angle()` and the
  value/range path is not generated at all.

With an expression, the editor canvas draws `previewStartAngle` /
`previewEndAngle` (strings), and the generated tick code re-reads the expression,
compares against `lv_arc_get_angle_end()` and only calls the setter on a change.

The solar gauge here uses `useAngle: true` with `endAngle` bound to
`SolarSweepEnd`, which C derives from watts:

```c
end = 135 + (watts * 270) / 1200;   /* wrapped into 0..359 */
```

## Spangroup

`lvgl/widgets/Span.js`. The class is `LVGLSpanWidget`; the palette label is
"Spangroup". Its `spans` array holds `LVGLSpan` objects, which are not widgets
and have no geometry — they are styled runs inside one text block, sharing a
baseline.

`LVGLSpanWidget`:

| Field | Notes |
|---|---|
| `mode` | `FIXED` / `EXPAND` / `BREAK` — **only applied on 8.4.0 and 9.2.2** |
| `overflow` | `CLIP` / `ELLIPSIS` |
| `indent` | |
| `maxLines` | `-1` for unlimited |
| `align` | `AUTO` `LEFT` `CENTER` `RIGHT` — **only applied on 8.4.0 and 9.2.2** |
| `spans` | array of `LVGLSpan` |

Defaults are `widthUnit`/`heightUnit` of `"content"`, which is what you want:
the group sizes to its text.

`LVGLSpan`:

| Field | Notes |
|---|---|
| `text`, `textType` | `literal` / `translated-literal` / `expression` |
| `useStaticText` | default `true`; literal only |
| `textColor`, `textFont` | optional, per-span |
| `textDecor` | `NONE` / `UNDERLINE` / `STRIKETHROUGH` |
| `textLetterSpace`, `textLineSpace`, `textOpa` | optional |

### The version-dependent API

This is the one to watch when changing `lvglVersion`:

| | 8.4.0 and 9.2.2 | 9.3.0+ |
|---|---|---|
| add a span | `lv_spangroup_new_span()` | `lv_spangroup_add_span()` |
| refresh | `lv_spangroup_refr_mode()` | `lv_spangroup_refresh()` |
| line limit | `lv_spangroup_set_lines()` (8.4 only) | `lv_spangroup_set_max_lines()` |
| `mode`, `align` | emitted | **not emitted** |

So a project exported at 9.3.0 will not compile against LVGL 9.2.2 and vice
versa, and `mode`/`align` silently stop having any effect above 9.2.2.

With `useStaticText` and a literal, the generator emits
`lv_span_set_text_static()`, keeping a pointer to the string literal instead of
copying it into LVGL's heap.

## Static label text

`lvgl/widgets/Label.js` — `useStaticText`, default `true`, only offered when
`textType` is `literal` and `longMode` is not `DOT`. It selects
`lv_label_set_text_static()` over `lv_label_set_text()`, so the string stays in
flash rather than being copied into RAM per label.

`DOT` is excluded because LVGL's dot-truncation rewrites the buffer in place,
which it cannot do to a literal.

## Merged fonts

`features/font/font.js`. A font entry gets `lvglAdditionalSources`, an array of:

```jsonc
{ "filePath": "ASSETS/fa-solid-900.otf",
  "lvglRanges": "0xf0e7,0xf185,0xf240",
  "lvglSymbols": "" }
```

Each source contributes its own codepoint ranges to one output font, so a single
label can carry an icon and text. This project merges Roboto's Latin range with
nine Font Awesome glyphs into `dashicons16` / `dashicons22`.

Range syntax is parsed by `getEncodings()`: comma-separated, `from-to` for a
range, and `from=>mapped` to remap a codepoint. It uses `parseInt()` **without a
radix**, so both `0x20-0x7f` and `32-127` work. An unparseable range makes the
whole list `undefined`, which disables the font's extraction.

`removeDuplicates()` then sorts and merges overlapping ranges and drops any
`lvglSymbols` character already covered by a range — so the stored order is not
necessarily the order you wrote.

Other LVGL-only font fields worth knowing: `lvglFallbackFont` (a font name such
as `lv_font_montserrat_24`, emitted as `--lv-fallback`), and `lvglUseFreeType`
with `lvglFreeTypeRenderMode` / `lvglFreeTypeStyle` / `lvglFreeTypeFilePath`,
which switches to runtime FreeType rendering and makes the simulator preload the
TTF instead of compiling a glyph table.

`embeddedFontFile` holds the source font as base64. On save, EEZ Studio dedups
identical blobs by rewriting later ones as `embeddedFontFileIndex` and restores
them on load, so a hand-written project file can carry either form.

See [BUILD.md](BUILD.md#eez-studios-headless---build-project-emits-no-custom-fonts)
for the headless-export bug that silently drops every `ui_font_*.c`.

## Expressions without flow support

With `settings.general.flowSupport: false`, an `"expression"`-typed property is
**not** a general expression. `lvgl/expression-property.js` shows the property is
presented as a reference into `variables/globalVariables` and labelled
"Variable" rather than "Expression" in the property grid.

So `valueType: "expression"` with `value: "SolarSweepEnd"` binds the widget to
the global variable of that name. EEZ Studio then generates, in `tick_screen()`,
a call to `get_var_solar_sweep_end()` and a change check before touching the
widget.

Every global variable becomes a `get_var_*` / `set_var_*` pair in `UI/vars.h`
regardless of the `native` flag, which is a flow-only concept. Names are
converted from the project's CamelCase to snake_case, and types map as
`integer → int32_t`, `boolean → bool`, `string → const char *`.

A string getter's return value is handed straight to LVGL, so it must point at
storage that outlives the call.

## Themes are runtime-switchable

`lvgl/build.js` resolves every colour token through a generated lookup table
rather than baking one theme's hex into the styles. The export emits, in
`screens.h`:

```c
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[<themes>][<colors>];
extern uint32_t active_theme_index;
```

All themes are compiled in and `change_color_theme()` re-resolves every style
that references a token, so a day/night swap is one call — see
`action_toggle_theme()` in `main/UI/actions.c`. Theme order follows the
project's `themes` array, so index 0 is `Default` and 1 is `Dark` here.

The catch is that a theme-invariant background with a theme-varying text colour
becomes illegible in one of the two themes. Colours that stay put across themes
need text tokens that also stay put — this project keeps `ForegroundWhite` and
`ForegroundBlack` identical in both themes for exactly that, and the generator
audits for the mismatch before it will write the project file.
