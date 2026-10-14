"""Parametric tab-and-slot torsion box workbench -- FreeCAD generator.

Builds every part as a PartDesign Body driven by the "Design" spreadsheet,
an Assembly that places them, a "CutList" sheet, and TechDraw pages.

Run from FreeCAD's Python console (or a macro), one step at a time so the
GUI never freezes for long:

    import sys; sys.path.insert(0, "<folder containing this file>")
    import build_workbench as bw
    bw.new_document()
    for step in bw.PART_STEPS:
        step()
    bw.build_assembly()
    bw.build_cutlist()
    bw.build_drawings()
    bw.tidy()

After that the generator is no longer needed: change a value in the
"Design" spreadsheet, press Recompute, and the parts, assembly, cut list
and drawings follow.

Coordinates: X = bench length, Y = bench depth (front is -Y), Z = up,
origin at floor level under the centre of the bench. Corners are modelled
square; add a dogbone dressup in CAM.
"""
import os
import re

import FreeCAD as App
import Part
import Sketcher

DOC_NAME = "TabSlotWorkbench"
SHEET = "Design"
HERE = os.path.dirname(os.path.abspath(__file__))

# (alias, value, note). A row with alias None is a section heading.
# Values ending in a length unit, or formulas producing one, are displayed
# in inches; the model itself is unit-agnostic.
PARAMS = [
    (None, "MATERIAL", ""),
    ("mdf_size", "0.5", "Nominal MDF thickness in inches: 0.25, 0.5 or 0.75"),
    ("t_adjust", "0 mm", "Measured thickness minus nominal (negative if the sheet runs thin)"),
    ("t_nom", "=mdf_size * 25.4 mm + t_adjust", "Actual sheet thickness used below"),
    ("t_s", "=t_nom", "Skin thickness, top and bottom (type a value to override)"),
    ("t_r", "=t_nom", "Inner rib thickness (type a value to override)"),
    ("t_l", "=t_nom", "Frame panel (sides, ends) and shelf thickness (type a value to override)"),
    ("fit", "0.3 mm", "Clearance added to every slot, total (width and length)"),
    (None, "TORSION BOX", ""),
    ("top_L", "72 in", "Bench top length (X)"),
    ("top_W", "30 in", "Bench top depth (Y)"),
    ("ov", "0.5 in", "Top skin overhang past the frame panels, each side (0 = flush)"),
    ("rib_h", "4 in", "Rib height between the skins (storage cubby height)"),
    ("nx", "6", "Cells along the length (2 or more)"),
    ("ny", "3", "Cells across the depth (2 or more)"),
    ("tab_len", "2 in", "Length of each tab through a skin"),
    (None, "STORAGE OPENINGS (1 = cut, 0 = solid)", ""),
    ("open_front", "1", "Openings in the front side panel"),
    ("open_back", "0", "Openings in the back side panel"),
    ("open_ends", "1", "Openings in both end panels"),
    ("open_inner", "0", "Openings in the inner long ribs (full-depth tunnels)"),
    ("open_mx", "1 in", "Solid panel left each side of an opening"),
    ("open_mz", "0.5 in", "Solid panel left above and below an opening"),
    ("open_r", "0.5 in", "Opening corner radius"),
    (None, "FRAME (sides and ends run floor to worktop)", ""),
    ("bench_h", "36 in", "Finished height, floor to top surface"),
    ("corner_proud", "1 in", "End panels sit in from the side-panel ends, and stand out past the side panels, by this much"),
    ("splay_L", "=ov", "Side panel ends flare out by this much at the floor (at most ov: the feet must stay under the worktop ends)"),
    ("splay_W", "3.5 in", "End panel edges flare out by this much at the floor, each side (FEM optimum: see README)"),
    ("foot_w", "4 in", "Width of each foot left by the arches"),
    ("arch_h", "4 in", "Height of the arch cut between the feet"),
    ("shelf_z", "10 in", "Shelf underside above the floor"),
    ("shelf_tab_len", "4 in", "Length of each shelf tab"),
    ("end_window", "1", "Lightening window in the end panels (1 = cut)"),
    ("win_side", "4 in", "End panel left each side of the window"),
    ("win_margin", "3 in", "End panel left above and below the window"),
    ("win_r", "1.5 in", "Window corner radius"),
    (None, "DERIVED (do not edit)", ""),
    ("frame_L", "=top_L - 2 * ov", "Side panel length"),
    ("frame_W", "=top_W - 2 * ov", "End panel width"),
    ("foot_L", "=frame_L + 2 * splay_L", "Footprint length at the floor"),
    ("foot_W", "=frame_W + 2 * splay_W", "Footprint depth at the floor"),
    ("rx0", "=-frame_L / 2 + corner_proud + t_l / 2", "End panel centre (X), left"),
    ("ry0", "=-frame_W / 2 + corner_proud + t_l / 2", "Side panel centre (Y), front"),
    ("px", "=-2 * rx0 / nx", "Cross rib pitch, centre to centre"),
    ("py", "=-2 * ry0 / ny", "Long rib pitch, centre to centre"),
    ("cx0", "=rx0 + px / 2", "First cell centre (X)"),
    ("cy0", "=ry0 + py / 2", "First cell centre (Y)"),
    ("box_h", "=rib_h + 2 * t_s", "Torsion box thickness"),
    ("box_z0", "=bench_h - box_h", "Underside of the bottom skin"),
    ("panel_h", "=bench_h - t_s", "Frame panel height below its top tabs"),
    ("lap_z", "=box_z0 + t_s + rib_h / 2", "Half-lap line inside the box"),
    ("in_L", "=-2 * rx0 - t_l", "Inside length between the end panels"),
    ("in_W", "=-2 * ry0 - t_l", "Inside depth between the side panels"),
    ("open_w", "=px - t_r - 2 * open_mx", "Opening width, side panels"),
    ("open_wy", "=py - t_r - 2 * open_mx", "Opening width, end panels"),
    ("open_h", "=rib_h - 2 * open_mz", "Opening height"),
    ("arch_aL", "=frame_L / 2 + splay_L - foot_w", "Half chord, side panel arch"),
    ("arch_RL", "=(arch_aL * arch_aL + arch_h * arch_h) / (2 * arch_h)", "Side panel arch radius"),
    ("arch_czL", "=arch_h - arch_RL", "Side panel arch centre height"),
    ("arch_aW", "=frame_W / 2 + splay_W - foot_w", "Half chord, end panel arch"),
    ("arch_RW", "=(arch_aW * arch_aW + arch_h * arch_h) / (2 * arch_h)", "End panel arch radius"),
    ("arch_czW", "=arch_h - arch_RW", "End panel arch centre height"),
    ("win_w", "=frame_W - 2 * win_side", "Window width"),
    ("win_z0", "=shelf_z + t_l + win_margin", "Window bottom"),
    ("win_h", "=box_z0 - win_margin - win_z0", "Window height"),
    ("win_cz", "=win_z0 + win_h / 2", "Window centre height"),
    (None, "CHECKS (all should read OK)", ""),
]

# (alias, [conditions that must all hold], message when one fails)
CHECKS = [
    ("chk_cells", ["nx >= 2", "ny >= 2"], "nx and ny must be 2 or more"),
    ("chk_tab", ["tab_len + fit < px - t_r", "tab_len + fit < py - t_r"], "tab_len too long for the cell"),
    ("chk_open", ["open_w > 2 * open_r", "open_wy > 2 * open_r", "open_h > 2 * open_r"],
     "openings too small for open_r"),
    ("chk_shelf", ["shelf_tab_len + fit < px - t_r", "shelf_z + t_l + 1 in < panel_h / 2"],
     "shelf tabs too long, or shelf above the corner half-laps"),
    ("chk_arch", ["arch_h + 1 in < shelf_z", "arch_aW > 0 mm"], "arch reaches the shelf, or feet too wide"),
    ("chk_window", ["win_h > 2 * win_r", "win_w > 2 * win_r", "win_side > corner_proud + t_l + 0.5 in"],
     "window too small or into the corner slots - set end_window to 0"),
    ("chk_ends", ["splay_L <= ov + 0.01 mm"], "splay_L puts the feet past the ends of the worktop"),
    ("chk_mdf", ["abs(mdf_size - 0.25) < 0.001 || abs(mdf_size - 0.5) < 0.001 || abs(mdf_size - 0.75) < 0.001"],
     "non-standard MDF size"),
]


def _check_expr(conds, msg):
    """Nested ternaries: FreeCAD expressions have no && operator."""
    if conds == [] :
        return "<<OK>>"
    c = conds[0]
    if "||" in c:   # any-of: chain the alternatives
        alts = [a.strip() for a in c.split("||")]
        inner = _check_expr(conds[1:], msg)
        e = "<<%s>>" % msg
        for a in reversed(alts):
            e = "(%s ? %s : %s)" % (a, inner, e)
        return e
    return "(%s ? %s : <<%s>>)" % (c, _check_expr(conds[1:], msg), msg)


PARAMS += [(a, "=" + _check_expr(c, m), "") for a, c, m in CHECKS]

ALIASES = [p[0] for p in PARAMS if p[0]]
_ALIAS_RE = re.compile(r"\b(" + "|".join(sorted(ALIASES, key=len, reverse=True)) + r")\b")

COLORS = {
    "skin": (0.80, 0.68, 0.50),
    "rib": (0.70, 0.55, 0.38),
    "edge": (0.60, 0.45, 0.30),
    "leg": (0.45, 0.55, 0.62),
}


# ---------------------------------------------------------------- helpers
def doc():
    return App.getDocument(DOC_NAME)


def sheet():
    return doc().getObject(SHEET)


def q(expr):
    """Qualify bare aliases with the sheet name, for property bindings."""
    return _ALIAS_RE.sub(lambda m: SHEET + "." + m.group(1), expr)


def ev(expr):
    """Evaluate an expression against the sheet, in mm."""
    v = sheet().evalExpression(expr)
    return float(getattr(v, "Value", v))


def bind(obj, prop, expr):
    obj.setExpression(prop, q(expr))


def plane(body, role):
    return [f for f in body.Origin.OriginFeatures if f.Role == role][0]


def new_body(name, label, color):
    d = doc()
    remove_body(name)
    b = d.addObject("PartDesign::Body", name)
    b.Label = label
    if App.GuiUp:
        b.ViewObject.ShapeColor = color
    return b


def remove_body(name):
    """Delete a Body and everything in it, so a part can be rebuilt."""
    d = doc()
    b = d.getObject(name)
    if b is None:
        return
    for o in reversed(b.Group):
        d.removeObject(o.Name)
    origin = b.Origin.Name if b.Origin else None
    d.removeObject(b.Name)
    o = d.getObject(origin) if origin else None
    if o is not None:          # FreeCAD normally takes the Origin with the Body
        for f in o.OriginFeatures:
            d.removeObject(f.Name)
        d.removeObject(o.Name)


def new_sketch(body, role, name):
    sk = doc().addObject("Sketcher::SketchObject", name)
    body.addObject(sk)
    sk.AttachmentSupport = [(plane(body, role), "")]
    sk.MapMode = "FlatFace"
    return sk


def _named(sk, cons, name, expr):
    i = sk.addConstraint(cons)
    sk.renameConstraint(i, name)
    bind(sk, "Constraints." + name, expr)


def _place_point(sk, tag, geo, pos, x, y):
    """Fix a sketch point to the origin by two expression-driven distances."""
    for axis, expr, ref in (("X", x, -2), ("Y", y, -1)):
        if expr.strip() == "0":
            sk.addConstraint(Sketcher.Constraint("PointOnObject", geo, pos, ref))
        else:
            c = Sketcher.Constraint("Distance" + axis, -1, 1, geo, pos, ev(expr))
            _named(sk, c, "%s_%s" % (tag, axis.lower()), expr)


def rect(sk, tag, x0, y0, w, h):
    """Rectangle from lower-left corner (x0, y0); all four args are expressions."""
    X, Y, Wd, Ht = ev(x0), ev(y0), ev(w), ev(h)
    if Wd <= 0 or Ht <= 0:
        raise ValueError("%s: rectangle %s has non-positive size %.2f x %.2f" % (sk.Name, tag, Wd, Ht))
    pts = [(X, Y), (X + Wd, Y), (X + Wd, Y + Ht), (X, Y + Ht)]
    g = sk.GeometryCount
    for i in range(4):
        a, c = pts[i], pts[(i + 1) % 4]
        sk.addGeometry(Part.LineSegment(App.Vector(a[0], a[1], 0), App.Vector(c[0], c[1], 0)))
    for i in range(4):
        sk.addConstraint(Sketcher.Constraint("Coincident", g + i, 2, g + (i + 1) % 4, 1))
    sk.addConstraint(Sketcher.Constraint("Horizontal", g))
    sk.addConstraint(Sketcher.Constraint("Horizontal", g + 2))
    sk.addConstraint(Sketcher.Constraint("Vertical", g + 1))
    sk.addConstraint(Sketcher.Constraint("Vertical", g + 3))
    _place_point(sk, tag, g, 1, x0, y0)
    _named(sk, Sketcher.Constraint("DistanceX", g, 1, g, 2, Wd), tag + "_w", w)
    _named(sk, Sketcher.Constraint("DistanceY", g + 1, 1, g + 1, 2, Ht), tag + "_h", h)


def polygon(sk, tag, pts):
    """Closed polygon; every vertex is placed by expressions, so it is fully constrained."""
    g = sk.GeometryCount
    P = [(ev(x), ev(y)) for x, y in pts]
    n = len(P)
    for i in range(n):
        a, c = P[i], P[(i + 1) % n]
        sk.addGeometry(Part.LineSegment(App.Vector(a[0], a[1], 0), App.Vector(c[0], c[1], 0)))
    for i in range(n):
        sk.addConstraint(Sketcher.Constraint("Coincident", g + i, 2, g + (i + 1) % n, 1))
    for i, (x, y) in enumerate(pts):
        _place_point(sk, "%s%d" % (tag, i), g + i, 1, x, y)


def rounded_rect(sk, tag, cx, cy, w, h, r):
    """Rounded rectangle centred on (cx, cy)."""
    CX, CY, Wd, Ht, R = ev(cx), ev(cy), ev(w), ev(h), ev(r)
    if Wd <= 2 * R or Ht <= 2 * R:
        raise ValueError("%s: %s too small for its corner radius" % (sk.Name, tag))
    xl, xr = CX - Wd / 2 + R, CX + Wd / 2 - R
    yb, yt = CY - Ht / 2 + R, CY + Ht / 2 - R
    import math
    z = App.Vector(0, 0, 1)

    def arc(x, y, a1, a2):
        return Part.ArcOfCircle(Part.Circle(App.Vector(x, y, 0), z, R), math.radians(a1), math.radians(a2))

    g = sk.GeometryCount
    # arcs g..g+3: BL, BR, TR, TL ; lines g+4..g+7: bottom, right, top, left
    sk.addGeometry(arc(xl, yb, 180, 270))
    sk.addGeometry(arc(xr, yb, 270, 360))
    sk.addGeometry(arc(xr, yt, 0, 90))
    sk.addGeometry(arc(xl, yt, 90, 180))
    V = App.Vector
    sk.addGeometry(Part.LineSegment(V(xl, yb - R, 0), V(xr, yb - R, 0)))
    sk.addGeometry(Part.LineSegment(V(xr + R, yb, 0), V(xr + R, yt, 0)))
    sk.addGeometry(Part.LineSegment(V(xr, yt + R, 0), V(xl, yt + R, 0)))
    sk.addGeometry(Part.LineSegment(V(xl - R, yt, 0), V(xl - R, yb, 0)))
    BL, BR, TR, TL, B, Rt, T, Lf = range(g, g + 8)
    C = Sketcher.Constraint
    for line, a_start, a_end in ((B, BL, BR), (Rt, BR, TR), (T, TR, TL), (Lf, TL, BL)):
        sk.addConstraint(C("Tangent", a_start, 2, line, 1))
        sk.addConstraint(C("Tangent", line, 2, a_end, 1))
    sk.addConstraint(C("Horizontal", B))
    sk.addConstraint(C("Horizontal", T))
    sk.addConstraint(C("Vertical", Rt))
    sk.addConstraint(C("Vertical", Lf))
    for a in (BR, TR, TL):
        sk.addConstraint(C("Equal", BL, a))
    _named(sk, C("Radius", BL, R), tag + "_r", r)
    _place_point(sk, tag, BL, 3, "(%s) - (%s) / 2 + (%s)" % (cx, w, r), "(%s) - (%s) / 2 + (%s)" % (cy, h, r))
    _named(sk, C("DistanceX", BL, 3, BR, 3, Wd - 2 * R), tag + "_w", "(%s) - 2 * (%s)" % (w, r))
    _named(sk, C("DistanceY", BL, 3, TL, 3, Ht - 2 * R), tag + "_h", "(%s) - 2 * (%s)" % (h, r))


def circle(sk, tag, cx, cy, r):
    g = sk.addGeometry(Part.Circle(App.Vector(ev(cx), ev(cy), 0), App.Vector(0, 0, 1), ev(r)))
    _place_point(sk, tag, g, 3, cx, cy)
    _named(sk, Sketcher.Constraint("Radius", g, ev(r)), tag + "_r", r)


def _finish(body, feat):
    body.Tip = feat
    if App.GuiUp:   # links draw the Tip's own colour, not the Body's
        feat.ViewObject.ShapeColor = body.ViewObject.ShapeColor
    return feat


def pad(body, sk, name, length, symmetric):
    p = doc().addObject("PartDesign::Pad", name)
    body.addObject(p)
    p.Profile = sk
    bind(p, "Length", length)
    p.SideType = "Symmetric" if symmetric else "One side"
    return _finish(body, p)


def pocket(body, sk, name):
    p = doc().addObject("PartDesign::Pocket", name)
    body.addObject(p)
    p.Profile = sk
    p.Type = "ThroughAll"
    p.SideType = "Symmetric"
    return _finish(body, p)


def pattern(body, feat, name, sk, n, length, n2=None, length2=None):
    """Linear pattern along the sketch H axis (and V axis if n2 is given)."""
    lp = doc().addObject("PartDesign::LinearPattern", name)
    body.addObject(lp)
    lp.Originals = [feat]
    lp.Direction = (sk, ["H_Axis"])
    lp.Mode = "Extent"
    bind(lp, "Occurrences", n)
    bind(lp, "Length", "max(%s; 1 mm)" % length)
    if n2:
        lp.Direction2 = (sk, ["V_Axis"])
        lp.Mode2 = "Extent"
        bind(lp, "Occurrences2", n2)
        bind(lp, "Length2", "max(%s; 1 mm)" % length2)
    return _finish(body, lp)


def suppress_unless(flag, *feats):
    for f in feats:
        f.setExpression("Suppressed", "%s.%s == 0" % (SHEET, flag))


# --------------------------------------------------------------- document
def new_document(path=None):
    if DOC_NAME in App.listDocuments():
        App.closeDocument(DOC_NAME)
    d = App.newDocument(DOC_NAME)
    sh = d.addObject("Spreadsheet::Sheet", SHEET)
    sh.Label = "Design"
    sh.set("A1", "Parameter")
    sh.set("B1", "Value")
    sh.set("C1", "Notes")
    sh.setStyle("A1:C1", "bold")
    row = 2
    for alias, value, note in PARAMS:
        if alias is None:
            row += 1
            sh.set("A%d" % row, value)
            sh.setStyle("A%d" % row, "bold")
        else:
            sh.set("A%d" % row, alias)
            sh.set("B%d" % row, value)
            sh.setAlias("B%d" % row, alias)
            if note:
                sh.set("C%d" % row, note)
        row += 1
    sh.setColumnWidth("A", 120)
    sh.setColumnWidth("B", 110)
    sh.setColumnWidth("C", 460)
    d.recompute()
    # Show lengths in inches.
    for r in range(2, row):
        a = "B%d" % r
        try:
            v = sh.get(a)
        except Exception:
            continue
        if hasattr(v, "Unit") and str(v.Unit) == "Unit: mm (1,0,0,0,0,0,0,0) [Length]":
            sh.setDisplayUnit(a, "in")
    d.recompute()
    if path:
        d.saveAs(path)
    return d


# ------------------------------------------------------------------ parts
def _tabs(b, plane, name, c0, n, pitch, top, bottom):
    """Tabs centred on each cell, at the given heights (each a (z0, h) pair)."""
    sk = new_sketch(b, plane, "sk_%s_tabs" % name)
    for tag, (z0, h) in (("tab_t", top), ("tab_b", bottom)):
        if z0 is not None:
            rect(sk, tag, "%s - tab_len / 2" % c0, z0, "tab_len", h)
    f = pad(b, sk, "tab_" + name, "t_l" if name in FRAME else "t_r", True)
    pattern(b, f, "tabs_" + name, sk, n, "(%s - 1) * %s" % (n, pitch))


def _slots(b, plane, name, tag, x0, z0, w, h, n, length, n2=None, length2=None):
    """One pocketed rectangle, patterned."""
    sk = new_sketch(b, plane, "sk_%s_%s" % (name, tag))
    rect(sk, tag, x0, z0, w, h)
    f = pocket(b, sk, "%s_%s" % (tag, name))
    return pattern(b, f, "%ss_%s" % (tag, name), sk, n, length, n2, length2)


def _openings(b, plane, name, c0, w, n, pitch, flag):
    sk = new_sketch(b, plane, "sk_%s_open" % name)
    rounded_rect(sk, "open", c0, "lap_z", w, "open_h", "open_r")
    f = pocket(b, sk, "open_" + name)
    p = pattern(b, f, "opens_" + name, sk, n, "(%s - 1) * %s" % (n, pitch))
    suppress_unless(flag, f, p)
    b.Tip = p


FRAME = ("SideFront", "SideBack", "EndPanel")


def _splayed(width, splay):
    """Frame panel outline: square through the torsion box, flaring out below it."""
    w = "%s / 2" % width
    return [("-(%s) - %s" % (w, splay), "0"), ("%s + %s" % (w, splay), "0"),
            (w, "box_z0"), (w, "panel_h"), ("-(%s)" % w, "panel_h"), ("-(%s)" % w, "box_z0")]


def _side(name, label, flag):
    """Side panel: floor to worktop, slotted from the top."""
    P = "XZ_Plane"
    b = new_body(name, label, COLORS["edge"])
    sk = new_sketch(b, P, "sk_%s_core" % name)
    polygon(sk, "core", _splayed("frame_L", "splay_L"))
    pad(b, sk, "core_" + name, "t_l", True)
    _tabs(b, P, name, "cx0", "nx", "px", ("panel_h", "t_s"), (None, None))
    # corner half-laps (end panels drop over these), then inner cross-rib laps
    _slots(b, P, name, "corner", "rx0 - (t_l + fit) / 2", "panel_h / 2", "t_l + fit", "panel_h / 2 + t_s",
           "2", "-2 * rx0")
    _slots(b, P, name, "lap", "rx0 + px - (t_r + fit) / 2", "lap_z", "t_r + fit", "panel_h + t_s - lap_z",
           "nx - 1", "(nx - 2) * px")
    # mortises for the bottom skin and shelf tabs
    sk = new_sketch(b, P, "sk_%s_mortise" % name)
    rect(sk, "mskin", "cx0 - (tab_len + fit) / 2", "box_z0 - fit / 2", "tab_len + fit", "t_s + fit")
    rect(sk, "mshelf", "cx0 - (shelf_tab_len + fit) / 2", "shelf_z - fit / 2", "shelf_tab_len + fit", "t_l + fit")
    f = pocket(b, sk, "mortise_" + name)
    pattern(b, f, "mortises_" + name, sk, "nx", "(nx - 1) * px")
    sk = new_sketch(b, P, "sk_%s_arch" % name)
    circle(sk, "arch", "0", "arch_czL", "arch_RL")
    pocket(b, sk, "arch_" + name)
    _openings(b, P, name, "cx0", "open_w", "nx", "px", flag)
    doc().recompute()
    return b


def build_side_front():
    return _side("SideFront", "Side panel - front", "open_front")


def build_side_back():
    return _side("SideBack", "Side panel - back", "open_back")


def build_end_panel():
    """End panel: floor to worktop, corner slots from the bottom."""
    P, name = "YZ_Plane", "EndPanel"
    b = new_body(name, "End panel", COLORS["leg"])
    sk = new_sketch(b, P, "sk_%s_core" % name)
    polygon(sk, "core", _splayed("frame_W", "splay_W"))
    pad(b, sk, "core_" + name, "t_l", True)
    _tabs(b, P, name, "cy0", "ny", "py", ("panel_h", "t_s"), (None, None))
    _slots(b, P, name, "corner", "ry0 - (t_l + fit) / 2", "-1 mm", "t_l + fit", "panel_h / 2 + 1 mm",
           "2", "-2 * ry0")
    _slots(b, P, name, "lap", "ry0 + py - (t_r + fit) / 2", "lap_z", "t_r + fit", "panel_h + t_s - lap_z",
           "ny - 1", "(ny - 2) * py")
    sk = new_sketch(b, P, "sk_%s_arch" % name)
    circle(sk, "arch", "0", "arch_czW", "arch_RW")
    pocket(b, sk, "arch_" + name)
    sk = new_sketch(b, P, "sk_%s_window" % name)
    rounded_rect(sk, "win", "0 mm", "win_cz", "win_w", "win_h", "win_r")
    f = pocket(b, sk, "window_" + name)
    suppress_unless("end_window", f)
    _openings(b, P, name, "cy0", "open_wy", "ny", "py", "open_ends")
    doc().recompute()
    return b


def build_long_inner():
    """Inner long rib (box coordinates): ends drop into the end panels."""
    P, name = "XZ_Plane", "LongInner"
    b = new_body(name, "Long rib - inner", COLORS["rib"])
    sk = new_sketch(b, P, "sk_%s_core" % name)
    rect(sk, "core", "rx0 - t_l / 2", "t_s", "-2 * rx0 + t_l", "rib_h")
    pad(b, sk, "core_" + name, "t_r", True)
    _tabs(b, P, name, "cx0", "nx", "px", ("t_s + rib_h", "t_s"), ("0", "t_s"))
    _slots(b, P, name, "endlap", "rx0 - (t_l + fit) / 2", "0", "t_l + fit", "t_s + rib_h / 2", "2", "-2 * rx0")
    _slots(b, P, name, "lap", "rx0 + px - (t_r + fit) / 2", "t_s + rib_h / 2", "t_r + fit", "rib_h / 2 + t_s",
           "nx - 1", "(nx - 2) * px")
    sk = new_sketch(b, P, "sk_%s_open" % name)
    rounded_rect(sk, "open", "cx0", "t_s + rib_h / 2", "open_w", "open_h", "open_r")
    f = pocket(b, sk, "open_" + name)
    p = pattern(b, f, "opens_" + name, sk, "nx", "(nx - 1) * px")
    suppress_unless("open_inner", f, p)
    b.Tip = p
    doc().recompute()
    return b


def build_cross_inner():
    """Inner cross rib (box coordinates): slotted from the bottom throughout."""
    P, name = "YZ_Plane", "CrossInner"
    b = new_body(name, "Cross rib - inner", COLORS["rib"])
    sk = new_sketch(b, P, "sk_%s_core" % name)
    rect(sk, "core", "ry0 - t_l / 2", "t_s", "-2 * ry0 + t_l", "rib_h")
    pad(b, sk, "core_" + name, "t_r", True)
    _tabs(b, P, name, "cy0", "ny", "py", ("t_s + rib_h", "t_s"), ("0", "t_s"))
    _slots(b, P, name, "endlap", "ry0 - (t_l + fit) / 2", "0", "t_l + fit", "t_s + rib_h / 2", "2", "-2 * ry0")
    _slots(b, P, name, "lap", "ry0 + py - (t_r + fit) / 2", "0", "t_r + fit", "t_s + rib_h / 2",
           "ny - 1", "(ny - 2) * py")
    doc().recompute()
    return b


def build_top_skin():
    P, name = "XY_Plane", "TopSkin"
    b = new_body(name, "Top skin", COLORS["skin"])
    sk = new_sketch(b, P, "sk_" + name)
    rect(sk, "skin", "-top_L / 2", "-top_W / 2", "top_L", "top_W")
    pad(b, sk, "plate_" + name, "t_s", False)
    sl, w = "tab_len + fit", "(tab_len + fit) / 2"
    _slots(b, P, name, "sideslot", "cx0 - " + w, "ry0 - (t_l + fit) / 2", sl, "t_l + fit",
           "nx", "(nx - 1) * px", "2", "-2 * ry0")
    _slots(b, P, name, "longslot", "cx0 - " + w, "ry0 + py - (t_r + fit) / 2", sl, "t_r + fit",
           "nx", "(nx - 1) * px", "ny - 1", "(ny - 2) * py")
    _slots(b, P, name, "endslot", "rx0 - (t_l + fit) / 2", "cy0 - " + w, "t_l + fit", sl,
           "2", "-2 * rx0", "ny", "(ny - 1) * py")
    _slots(b, P, name, "crossslot", "rx0 + px - (t_r + fit) / 2", "cy0 - " + w, "t_r + fit", sl,
           "nx - 1", "(nx - 2) * px", "ny", "(ny - 1) * py")
    doc().recompute()
    return b


def _inside_plate(name, label, thick, tab_len, color):
    """A plate filling the frame, with tabs into mortises in both side panels."""
    P = "XY_Plane"
    b = new_body(name, label, color)
    sk = new_sketch(b, P, "sk_" + name)
    rect(sk, "plate", "rx0 + t_l / 2", "ry0 + t_l / 2", "in_L", "in_W")
    pad(b, sk, "plate_" + name, thick, False)
    sk = new_sketch(b, P, "sk_%s_tabs" % name)
    rect(sk, "tabF", "cx0 - %s / 2" % tab_len, "ry0 - t_l / 2", tab_len, "t_l + 1 mm")
    rect(sk, "tabB", "cx0 - %s / 2" % tab_len, "-ry0 - t_l / 2 - 1 mm", tab_len, "t_l + 1 mm")
    f = pad(b, sk, "tab_" + name, thick, False)
    pattern(b, f, "tabs_" + name, sk, "nx", "(nx - 1) * px")
    return b


def build_bottom_skin():
    P, name = "XY_Plane", "BottomSkin"
    b = _inside_plate(name, "Bottom skin", "t_s", "tab_len", COLORS["skin"])
    sl, w = "tab_len + fit", "(tab_len + fit) / 2"
    _slots(b, P, name, "longslot", "cx0 - " + w, "ry0 + py - (t_r + fit) / 2", sl, "t_r + fit",
           "nx", "(nx - 1) * px", "ny - 1", "(ny - 2) * py")
    _slots(b, P, name, "crossslot", "rx0 + px - (t_r + fit) / 2", "cy0 - " + w, "t_r + fit", sl,
           "nx - 1", "(nx - 2) * px", "ny", "(ny - 1) * py")
    doc().recompute()
    return b


def build_shelf():
    b = _inside_plate("Shelf", "Shelf", "t_l", "shelf_tab_len", COLORS["leg"])
    doc().recompute()
    return b


PART_STEPS = [build_side_front, build_side_back, build_end_panel, build_long_inner,
              build_cross_inner, build_top_skin, build_bottom_skin, build_shelf]

# Assembly instances: (link name, body, x, y, z expressions)
INSTANCES = [
    ("A_SideFront", "SideFront", "0", "ry0", "0"),
    ("A_SideBack", "SideBack", "0", "-ry0", "0"),
    ("A_EndPanel_L", "EndPanel", "rx0", "0", "0"),
    ("A_EndPanel_R", "EndPanel", "-rx0", "0", "0"),
    ("A_BottomSkin", "BottomSkin", "0", "0", "box_z0"),
    ("A_Shelf", "Shelf", "0", "0", "shelf_z"),
    ("A_TopSkin", "TopSkin", "0", "0", "bench_h - t_s"),
]
# Variable-count inner ribs: (array name, body, axis, count, interval, x, y, z)
ARRAYS = [
    ("A_LongInner", "LongInner", "Y", "ny - 1", "py", "0", "ry0 + py", "box_z0"),
    ("A_CrossInner", "CrossInner", "X", "nx - 1", "px", "rx0 + px", "0", "box_z0"),
]
GROUND = "A_SideFront"
ALL_PARTS = [n for n, *_ in INSTANCES] + [n for n, *_ in ARRAYS]


def _bind_xyz(obj, x, y, z):
    for c, e in (("x", x), ("y", y), ("z", z)):
        bind(obj, "Placement.Base." + c, e)


def build_assembly():
    import Draft
    d = doc()
    asm = d.addObject("Assembly::AssemblyObject", "Assembly")
    asm.Label = "Workbench assembly"
    asm.newObject("Assembly::JointGroup", "Joints")
    for name, body, x, y, z in INSTANCES:
        ln = d.addObject("App::Link", name)
        ln.LinkedObject = d.getObject(body)
        ln.Label = name
        _bind_xyz(ln, x, y, z)
        asm.addObject(ln)
    for name, body, axis, n, step, x, y, z in ARRAYS:
        arr = Draft.make_ortho_array(d.getObject(body), v_x=App.Vector(1, 0, 0), v_y=App.Vector(0, 1, 0),
                                     v_z=App.Vector(0, 0, 1), n_x=1, n_y=1, n_z=1, use_link=True)
        arr.Label = name
        bind(arr, "Number" + axis, n)
        bind(arr, "Interval%s.%s" % (axis, axis.lower()), step)
        _bind_xyz(arr, x, y, z)
        asm.addObject(arr)
    d.recompute()
    ground_assembly()
    return asm


def ground_assembly():
    """Ground one frame panel so the assembly has a fixed reference."""
    import JointObject
    d = doc()
    j = d.Joints.newObject("App::FeaturePython", "GroundedJoint")
    JointObject.GroundedJoint(j, d.getObject(GROUND))
    if App.GuiUp:
        JointObject.ViewProviderGroundedJoint(j.ViewObject)
        j.ViewObject.Visibility = False   # its marker otherwise floats away from the model
    d.recompute()
    return j


# --------------------------------------------------------------- cut list
CUTS = [
    # part, qty, length, width, thickness
    ("Top skin", "1", "top_L", "top_W", "t_s"),
    ("Side panel - front", "1", "foot_L", "panel_h + t_s", "t_l"),
    ("Side panel - back", "1", "foot_L", "panel_h + t_s", "t_l"),
    ("End panel", "2", "panel_h + t_s", "foot_W", "t_l"),
    ("Bottom skin", "1", "in_L", "in_W + 2 * t_l", "t_s"),
    ("Long rib - inner", "=Design.ny - 1", "-2 * rx0 + t_l", "box_h", "t_r"),
    ("Cross rib - inner", "=Design.nx - 1", "-2 * ry0 + t_l", "box_h", "t_r"),
    ("Shelf", "1", "in_L", "in_W + 2 * t_l", "t_l"),
]


def build_cutlist():
    d = doc()
    cl = d.addObject("Spreadsheet::Sheet", "CutList")
    cl.Label = "Cut list"
    for col, h in zip("ABCDE", ("Part", "Qty", "Length", "Width", "Thickness")):
        cl.set(col + "1", h)
    cl.setStyle("A1:E1", "bold")
    for i, (part, qty, l, w, t) in enumerate(CUTS, start=2):
        cl.set("A%d" % i, part)
        cl.set("B%d" % i, qty)
        for col, e in (("C", l), ("D", w), ("E", t)):
            cl.set("%s%d" % (col, i), "=" + q(e))
            cl.setDisplayUnit("%s%d" % (col, i), "in")
    n = len(CUTS) + 2
    cl.set("A%d" % n, "Total parts")
    cl.set("B%d" % n, "=sum(B2:B%d)" % (n - 1))
    cl.setStyle("A%d:B%d" % (n, n), "bold")
    cl.setColumnWidth("A", 150)
    cl.setColumnWidth("B", 40)
    for col in "CDE":
        cl.setColumnWidth(col, 75)
    for r in range(1, n + 1):
        cl.setRowHeight(str(r), TABLE_ROW)
    d.recompute()
    return cl


# --------------------------------------------------------------- drawings
TEMPLATE = "Mod/TechDraw/Templates/ASME/ANSIB_Landscape.svg"   # 17 x 11 in
UNITS = "Imperial decimal (in, lb)"
# DrawViewSpreadsheet draws text ~2.6x smaller than TextSize says; 8 prints at ~3 mm.
TABLE_TEXT = 8.0
TABLE_ROW = 20   # spreadsheet row height in px (~5.3 mm on the page)

# Title block text, per page: (name, title line 2, title line 3)
PAGES = [
    ("Sheet1_Overview", "General arrangement and cut list", "1"),
    ("Sheet2_TorsionBox", "Torsion box parts", "2"),
    ("Sheet3_Frame", "Frame parts (sides, ends, shelf)", "3"),
    ("Sheet4_Assembly", "Assembly instructions", "4"),
]

NOTES = [
    "NOTES",
    "1. Material: MDF. Thickness is set by Design.mdf_size (1/4, 1/2 or 3/4 in);",
    "   skins, inner ribs and frame panels can each be overridden (t_s, t_r, t_l).",
    "2. All dimensions in inches. Every size follows the Design spreadsheet.",
    "3. Slots are drawn with the fit clearance (Design.fit) already added.",
    "4. Corners are drawn square: add a dogbone dressup to every CAM profile.",
    "5. Cut a test corner and one rib pair first and check every fit.",
    "6. Seal MDF edges (shellac or thinned glue) before finishing.",
    "7. Overall sizes are the footprint at the floor. Default leg splay (3.5 in)",
    "   comes from the FEM study in fem_splay_study.py; see README.",
]

STEPS = [
    ("Step 1 - Sides, bottom skin, shelf",
     ["A_SideFront", "A_SideBack", "A_BottomSkin", "A_Shelf"],
     ["Stand both side panels up, corner slots",
      "facing UP. Fit the bottom-skin and shelf tabs",
      "into their mortises in one panel, then bring",
      "the other panel in over the far tabs."]),
    ("Step 2 - Drop in the end panels",
     ["A_SideFront", "A_SideBack", "A_BottomSkin", "A_Shelf", "A_EndPanel_L", "A_EndPanel_R"],
     ["Lower each end panel, corner slots facing",
      "DOWN, over the side panels until the tops",
      "line up. The frame now stands on its own.",
      "Check it is square before the glue grabs."]),
    ("Step 3 - Inner long ribs",
     ["A_SideFront", "A_SideBack", "A_BottomSkin", "A_Shelf", "A_EndPanel_L", "A_EndPanel_R",
      "A_LongInner"],
     ["Drop each inner long rib into the slots at",
      "the top of the end panels, half-lap slots",
      "facing UP, until its bottom tabs seat in",
      "the bottom skin."]),
    ("Step 4 - Inner cross ribs",
     ["A_SideFront", "A_SideBack", "A_BottomSkin", "A_Shelf", "A_EndPanel_L", "A_EndPanel_R",
      "A_LongInner", "A_CrossInner"],
     ["Lower each cross rib, slots facing DOWN,",
      "over the long ribs and into the slots at",
      "the top of the side panels, until its tabs",
      "seat in the bottom skin."]),
    ("Step 5 - Close with the top skin",
     ALL_PARTS,
     ["Glue every top edge and press the top skin",
      "down over all the tabs. Weight it evenly and",
      "let it cure with the bench on a level floor:",
      "the top takes its flatness from the frame."]),
]

DIRS = {
    "front": App.Vector(0, -1, 0),
    "top": App.Vector(0, 0, 1),
    "right": App.Vector(1, 0, 0),
    "iso": App.Vector(1, -1, 1),
}


def _objs(names):
    """Assembly member names -> objects (array labels resolve to the array)."""
    d = doc()
    out = []
    for n in names:
        o = d.getObject(n)
        if o is None:
            o = d.getObjectsByLabel(n)[0]
        out.append(o)
    return out


def _page(name, title2, sheet_no):
    d = doc()
    page = d.addObject("TechDraw::DrawPage", name)
    tpl = d.addObject("TechDraw::DrawSVGTemplate", name + "_Template")
    tpl.Template = App.getResourceDir() + TEMPLATE
    page.Template = tpl
    texts = dict(tpl.EditableTexts)
    texts.update({
        "DrawingTitle1": "Tab & Slot Torsion Box Workbench",
        "DrawingTitle2": title2,
        "DrawingTitle3": "Parametric - see Design spreadsheet",
        "Sheet": "%s of %d" % (sheet_no, len(PAGES)),
        "drawing_number": "TSW-%s" % sheet_no,
        "scale": "AS NOTED",
        "Weight": "",
        "CompanyName": "",
        "CompanyAddress": "",
        "DrawnBy": "",
        "CheckedBy": "",
        "Approved1": "",
        "Approved2": "",
        "Code": "",
        "revision_index": "A",
    })
    tpl.EditableTexts = texts
    return page


# Standard drawing scales, largest first. Each view takes the largest that
# fits its box on the sheet, so the drawings follow any size typed into Design.
SCALES = [2, 4, 5, 8, 10, 12.5, 15, 20, 25, 30, 40, 50, 60, 80, 100]

ISO_W = "(Design.foot_L + Design.foot_W) * 0.7071"
ISO_H = "(Design.foot_L + Design.foot_W) * 0.4082 + Design.bench_h * 0.8165"
ASM = "ALL"

# name: (page, sources, direction, x, y, box_w, box_h, title, extent_w, extent_h)
# extent_* are the view's size in model units, as expressions on Design.
VIEWS = {
    "V_Front": ("Sheet1_Overview", ASM, "front", 117, 210, 158, 75, "FRONT (sizes at floor)",
                "Design.foot_L", "Design.bench_h"),
    "V_Right": ("Sheet1_Overview", ASM, "right", 228, 210, 72, 75, "END", "Design.foot_W", "Design.bench_h"),
    "V_Top": ("Sheet1_Overview", ASM, "top", 117, 128, 158, 62, "TOP", "Design.foot_L", "Design.foot_W"),
    "V_Iso": ("Sheet1_Overview", ASM, "iso", 342, 205, 140, 90, "", ISO_W, ISO_H),
    "V_LongInner": ("Sheet2_TorsionBox", ["LongInner"], "front", 140, 225, 235, 30, "LONG RIB - INNER",
                    "-2 * Design.rx0 + Design.t_l", "Design.box_h"),
    "V_CrossInner": ("Sheet2_TorsionBox", ["CrossInner"], "right", 340, 225, 115, 30, "CROSS RIB - INNER",
                     "-2 * Design.ry0 + Design.t_l", "Design.box_h"),
    "V_TopSkin": ("Sheet2_TorsionBox", ["TopSkin"], "top", 100, 115, 150, 80, "TOP SKIN",
                  "Design.top_L", "Design.top_W"),
    "V_BottomSkin": ("Sheet2_TorsionBox", ["BottomSkin"], "top", 258, 115, 150, 80, "BOTTOM SKIN",
                     "Design.in_L", "Design.in_W + 2 * Design.t_l"),
    "V_SideFront": ("Sheet3_Frame", ["SideFront"], "front", 120, 205, 160, 75, "SIDE PANEL - FRONT",
                    "Design.foot_L", "Design.bench_h"),
    "V_SideBack": ("Sheet3_Frame", ["SideBack"], "front", 120, 100, 160, 75, "SIDE PANEL - BACK",
                   "Design.foot_L", "Design.bench_h"),
    "V_EndPanel": ("Sheet3_Frame", ["EndPanel"], "right", 262, 205, 85, 75, "END PANEL (2 req'd)",
                   "Design.foot_W", "Design.bench_h"),
    "V_Shelf": ("Sheet3_Frame", ["Shelf"], "top", 300, 120, 150, 62, "SHELF",
                "Design.in_L", "Design.in_W + 2 * Design.t_l"),
}
STEP_SLOTS = [(75, 220), (215, 220), (355, 220), (75, 110), (215, 110)]   # view centres
STEP_BOX = (125, 60)

# Box sizes leave room beside each view for its dimensions.

# Overall dimensions: (view, kind, [(object, axis, "min"/"max"), ...], side).
# They attach to model vertices, which FreeCAD keeps track of through
# parameter changes; TechDraw's own vertex numbers are reshuffled instead.
DIMS = [
    ("V_Front", "DistanceX", [("A_SideFront", "x", "min"), ("A_SideFront", "x", "max")], "above"),
    ("V_Front", "DistanceY", [("A_SideFront", "z", "min"), ("A_TopSkin", "z", "max")], "left"),
    ("V_Right", "DistanceX", [("A_EndPanel_R", "y", "min"), ("A_EndPanel_R", "y", "max")], "above"),
    ("V_Top", "DistanceY", [("A_EndPanel_L", "y", "min"), ("A_EndPanel_L", "y", "max")], "left"),
    ("V_LongInner", "DistanceX", [("LongInner", "x", "min"), ("LongInner", "x", "max")], "above"),
    ("V_LongInner", "DistanceY", [("LongInner", "z", "min"), ("LongInner", "z", "max")], "right"),
    ("V_CrossInner", "DistanceX", [("CrossInner", "y", "min"), ("CrossInner", "y", "max")], "above"),
    ("V_CrossInner", "DistanceY", [("CrossInner", "z", "min"), ("CrossInner", "z", "max")], "right"),
    ("V_TopSkin", "DistanceX", [("TopSkin", "x", "min"), ("TopSkin", "x", "max")], "above"),
    ("V_TopSkin", "DistanceY", [("TopSkin", "y", "min"), ("TopSkin", "y", "max")], "left"),
    ("V_BottomSkin", "DistanceX", [("BottomSkin", "x", "min"), ("BottomSkin", "x", "max")], "above"),
    ("V_SideFront", "DistanceX", [("SideFront", "x", "min"), ("SideFront", "x", "max")], "above"),
    ("V_SideFront", "DistanceY", [("SideFront", "z", "min"), ("SideFront", "z", "max")], "left"),
    ("V_EndPanel", "DistanceX", [("EndPanel", "y", "min"), ("EndPanel", "y", "max")], "above"),
    ("V_EndPanel", "DistanceY", [("EndPanel", "z", "min"), ("EndPanel", "z", "max")], "right"),
    ("V_Shelf", "DistanceX", [("Shelf", "x", "min"), ("Shelf", "x", "max")], "above"),
]


def _scale_exprs(box_w, box_h, ext_w, ext_h, title):
    """Expressions for (fit, standard scale, caption) of one view."""
    fit = "min(%g mm / (%s); %g mm / (%s))" % (box_w, ext_w, box_h, ext_h)
    sc, cap = "1 / %g" % SCALES[-1], "<<%s  1:%g>>" % (title, SCALES[-1])
    for n in reversed(SCALES[:-1]):
        sc = "(fit >= 1 / %g ? 1 / %g : %s)" % (n, n, sc)
        cap = "(sc >= 1 / %g - 1e-9 ? <<%s  1:%g>> : %s)" % (n, title, n, cap)
    return fit, sc, cap


def build_layout():
    """Spreadsheet of view scales and captions, all derived from Design."""
    d = doc()
    lay = d.addObject("Spreadsheet::Sheet", "Layout")
    lay.Label = "Layout"
    for col, h in zip("ABCD", ("View", "Fit", "Scale", "Caption")):
        lay.set(col + "1", h)
    lay.setStyle("A1:D1", "bold")
    specs = [(n, v[5], v[6], v[8], v[9], v[7]) for n, v in VIEWS.items()]
    specs += [("V_Step%d" % (i + 1), STEP_BOX[0], STEP_BOX[1], ISO_W, ISO_H, "") for i in range(len(STEPS))]
    for r, (name, bw_, bh, ew, eh, title) in enumerate(specs, start=2):
        fit, sc, cap = _scale_exprs(bw_, bh, ew, eh, title)
        key = name[2:]
        lay.set("A%d" % r, name)
        lay.set("B%d" % r, "=" + fit)
        lay.setAlias("B%d" % r, "fit_" + key)
        lay.set("C%d" % r, "=" + sc.replace("fit", "fit_" + key))
        lay.setAlias("C%d" % r, "sc_" + key)
        lay.set("D%d" % r, "=" + cap.replace("sc ", "sc_%s " % key))
        lay.setAlias("D%d" % r, "cap_" + key)
    lay.setColumnWidth("A", 110)
    lay.setColumnWidth("D", 260)
    d.recompute()
    return lay


def apply_layout():
    """Push VIEWS box sizes and positions into an existing document."""
    d = doc()
    lay = d.getObject("Layout")
    for r in range(2, 200):
        name = lay.getContents("A%d" % r).lstrip("'")   # text cells are stored with a leading quote
        if not name:
            break
        if name in VIEWS:
            _pg, _src, _dir, x, y, bw_, bh, title, ew, eh = VIEWS[name]
            fit, _sc, _cap = _scale_exprs(bw_, bh, ew, eh, title)
            lay.set("B%d" % r, "=" + fit)
            v = d.getObject(name)
            v.X, v.Y = x, y
    # only the layout and the drawings depend on this; leave the model alone
    objs = [lay] + [o for o in d.Objects if o.TypeId.startswith("TechDraw::")]
    for o in objs:
        o.touch()
    d.recompute(objs)


def _view(page, name, sources, direction, x, y, xdir=None):
    d = doc()
    v = d.addObject("TechDraw::DrawViewPart", name)
    page.addView(v)
    v.Source = sources
    v.Direction = DIRS[direction]
    if xdir is None and direction == "right":
        xdir = App.Vector(0, 1, 0)   # default X would be parallel to the view axis
    if xdir is not None:
        v.XDirection = xdir
    v.ScaleType = "Custom"
    v.setExpression("Scale", "Layout.sc_" + name[2:])
    v.X, v.Y = x, y
    if direction == "iso":
        v.CoarseView = False
    return v


def _note(page, name, lines, x, y, size=3.5):
    a = doc().addObject("TechDraw::DrawViewAnnotation", name)
    page.addView(a)
    a.Text = lines
    a.TextSize = size
    a.X, a.Y = x, y
    return a


def build_drawings():
    """Create the four pages and their views. Dimensions follow in add_dimensions()."""
    import TechDraw  # noqa: F401  (registers the types)
    d = doc()
    d.UnitSystem = UNITS
    if d.getObject("Layout") is None:
        build_layout()
    p = {n: _page(n, t, s) for n, t, s in PAGES}
    for name, (page, src, direction, x, y, _bw, _bh, title, _ew, _eh) in VIEWS.items():
        v = _view(p[page], name, _objs(ALL_PARTS if src == ASM else src), direction, x, y)
        if title:
            v.setExpression("Caption", "Layout.cap_" + name[2:])

    pg = p["Sheet1_Overview"]
    cl = d.addObject("TechDraw::DrawViewSpreadsheet", "V_CutList")
    pg.addView(cl)
    cl.Source = d.getObject("CutList")
    cl.CellStart, cl.CellEnd = "A1", "E%d" % (len(CUTS) + 2)
    cl.TextSize = TABLE_TEXT
    cl.X, cl.Y = 330, 118
    _note(pg, "N_Notes", NOTES, 95, 52, 2.6)

    pg = p["Sheet4_Assembly"]
    for i, (title, names, text) in enumerate(STEPS):
        x, y = STEP_SLOTS[i]
        v = _view(pg, "V_Step%d" % (i + 1), _objs(names), "iso", x, y)
        v.Caption = title
        _note(pg, "N_Step%d" % (i + 1), text, x, y - 57, 3.2)
    _note(pg, "N_Glue", ["GLUE-UP", "Dry-fit every joint first.",
                         "Use PVA wood glue on all", "tabs, half-laps and mortises.",
                         "MDF swells with glue: work", "one stage at a time."], 355, 100, 3.2)
    d.recompute()
    return p


def _vertex(obj, axis, which):
    """Name of the object's extreme vertex along an axis (ties broken by the others)."""
    k = "xyz".index(axis)
    others = [i for i in range(3) if i != k]
    vs = obj.Shape.Vertexes
    sign = 1 if which == "min" else -1

    def key(i):
        p = vs[i].Point
        return (round(sign * p[k], 3), round(p[others[0]], 3), round(p[others[1]], 3))
    return "Vertex%d" % (min(range(len(vs)), key=key) + 1)


def add_dimensions():
    """Overall dimensions, attached to model vertices and offset by expression."""
    d = doc()
    made = []
    for view_name, kind, refs, side in DIMS:
        view = d.getObject(view_name)
        page_name, _src, _dir, _x, _y, _bw, _bh, _t, ew, eh = VIEWS[view_name]
        dim = d.addObject("TechDraw::DrawViewDimension", "D_%s_%s" % (view_name, kind[-1]))
        dim.Type = kind
        dim.MeasureType = "Projected"
        dim.References2D = [(view, "")]
        dim.References3D = [(d.getObject(o), _vertex(d.getObject(o), ax, w)) for o, ax, w in refs]
        d.getObject(page_name).addView(dim)
        sc = "Layout.sc_" + view_name[2:]
        if side == "above":
            dim.setExpression("Y", "%s * (%s) / 2 + 9 mm" % (sc, eh))
        else:
            sign = "-" if side == "left" else ""
            dim.setExpression("X", "%s(%s * (%s) / 2 + 10 mm)" % (sign, sc, ew))
        made.append(dim)
    d.recompute()
    return made


def refresh_dimensions():
    """Re-create the overall dimensions (only needed if one was deleted or broken)."""
    d = doc()
    for o in [o for o in d.Objects if o.TypeId == "TechDraw::DrawViewDimension"]:
        for page in o.InList:
            if page.TypeId == "TechDraw::DrawPage":
                page.removeView(o)
        d.removeObject(o.Name)
    d.recompute()
    return add_dimensions()


def key_sizes_sheet():
    """A small table of the joinery sizes, drawn on the parts sheets."""
    d = doc()
    ks = d.addObject("Spreadsheet::Sheet", "KeySizes")
    ks.Label = "Key sizes"
    box = [("Torsion box joinery", ""),
           ("Sheet thickness", "t_nom"), ("Slot width (inner rib)", "t_r + fit"),
           ("Rib tab length", "tab_len"), ("Rib slot length", "tab_len + fit"),
           ("Rib height (between skins)", "rib_h"), ("Rib half-lap depth", "rib_h / 2"),
           ("Cross rib pitch", "px"), ("Long rib pitch", "py"),
           ("Opening, side panels", "open_w"), ("Opening, end panels", "open_wy"),
           ("Opening height", "open_h"), ("Opening corner radius", "open_r")]
    legs = [("Frame joinery", ""),
            ("Slot width (frame)", "t_l + fit"), ("Corner half-lap depth", "panel_h / 2"),
            ("Bottom-skin mortise", "tab_len + fit"), ("Shelf mortise", "shelf_tab_len + fit"),
            ("Side arch radius", "arch_RL"), ("End arch radius", "arch_RW"),
            ("Window corner radius", "win_r")]
    rows = box + legs
    for i, (a, b) in enumerate(rows, start=1):
        ks.set("A%d" % i, a)
        ks.setRowHeight(str(i), TABLE_ROW)
        if b:
            ks.set("B%d" % i, "=" + q(b))
            ks.setDisplayUnit("B%d" % i, "in")
        else:
            ks.setStyle("A%d" % i, "bold")
    ks.setColumnWidth("A", 190)
    ks.setColumnWidth("B", 80)
    d.recompute()
    n = len(box)
    for page, x, y, a, b in (("Sheet2_TorsionBox", 372, 110, 1, n),
                             ("Sheet3_Frame", 372, 205, n + 1, len(rows))):
        v = d.addObject("TechDraw::DrawViewSpreadsheet", "V_KeySizes_" + page.split("_")[0])
        d.getObject(page).addView(v)
        v.Source = ks
        v.CellStart, v.CellEnd = "A%d" % a, "B%d" % b
        v.TextSize = TABLE_TEXT
        v.X, v.Y = x, y
    d.recompute()
    return ks


def export_page(name, folder=None):
    """Write one page to PDF. One page per call keeps the GUI responsive.

    A page only builds its template graphics (border, title block) once it
    has been opened in the GUI, so open it first.
    """
    import TechDrawGui
    folder = folder or os.path.join(HERE, "drawings")
    os.makedirs(folder, exist_ok=True)
    page = doc().getObject(name)
    page.ViewObject.doubleClicked()
    path = os.path.join(folder, name + ".pdf")
    TechDrawGui.exportPageAsPdf(page, path)
    return path


def export_drawings(folder=None):
    return [export_page(n, folder) for n, _, _ in PAGES]


# ------------------------------------------------------------------ tidy
def tidy():
    """Hide sketches, origins and every feature but each Body's Tip."""
    d = doc()
    for o in d.Objects:
        if o.TypeId in ("Sketcher::SketchObject", "App::Origin", "App::Plane", "App::Line", "App::Point"):
            o.ViewObject.Visibility = False
    for b in [o for o in d.Objects if o.TypeId == "PartDesign::Body"]:
        for f in b.Group:
            if f.TypeId.startswith("PartDesign::"):
                f.ViewObject.Visibility = (f is b.Tip)
        b.ViewObject.Visibility = False   # bodies are shown through the assembly links
