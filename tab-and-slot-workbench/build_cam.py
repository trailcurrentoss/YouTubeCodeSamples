"""CAM jobs that cut the workbench from 49 x 97 in sheets -- FreeCAD generator.

Run inside FreeCAD (GUI) with tab_slot_workbench.FCStd open, one step at a
time so the GUI never freezes for long:

    import sys; sys.path.insert(0, "<folder containing this file>")
    import build_cam as bc
    bc.build_sheets()            # CAM and Nest spreadsheets
    for name in bc.job_names():
        bc.build_job(name)       # one job per sheet
    bc.check_all()               # every toolpath inside the machine's reach
    for name in bc.job_names():
        bc.post_job(name)        # G-code into cam/

What is parametric, and what is not:

* Every part in a job is a clone of the real Body, so it follows the Design
  sheet. Every position is an expression in the Nest sheet, and every depth,
  feed and speed is an expression in the CAM sheet. The profile operations
  cut all of a job's models without naming faces, so geometry changes never
  leave them pointing at faces that no longer exist.
* The NUMBER of rib clones on each sheet is fixed when the job is built.
  Change nx or ny and the Nest sheet's chk_counts cell says so; run
  rebuild_job() for the affected sheets.

Machine conventions follow the LR4 torsion table CAM (TorsionTableCAD):
machine X across the sheet, Y along it, Z0 on the top of the material, grbl
post, and SCORE -> STOP (drive hold-down screws) -> CUT on every sheet.
Corners are modelled square; every cutting operation gets a dogbone dressup.
"""
import os
import re

import FreeCAD as App

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, "cam")

# (alias, value, note). A row with alias None is a section heading.
CAM_PARAMS = [
    (None, "SHEET AND MACHINE", ""),
    ("sheet_x", "49 in", "Sheet width, along machine X"),
    ("sheet_y", "97 in", "Sheet length, along machine Y"),
    ("reach_x", "48 in", "Furthest the cutter centre can travel in X"),
    ("reach_y", "96 in", "Furthest the cutter centre can travel in Y"),
    ("edge_m", "0.25 in", "Gap between a cut part and the sheet's origin edges"),
    (None, "TOOL", ""),
    ("tool_d", "0.25 in", "End mill diameter"),
    ("tool_r", "=tool_d / 2", "End mill radius"),
    ("flutes", "1", "Flutes"),
    ("rpm", "12000", "Spindle speed"),
    ("feed_xy", "3000 mm/min", "Cutting feed"),
    ("feed_z", "600 mm/min", "Plunge feed"),
    ("step_down", "2 mm", "Depth per pass (the tested maximum on the LR4)"),
    ("chipload", "=feed_xy * 60 s / (rpm * flutes)", "Chip load per tooth"),
    (None, "DEPTHS (Z0 = top of the material at its highest point)", ""),
    ("surf_var", "14 mm", "How far the lowest spot of the material sits below Z0. RE-MEASURE ON YOUR MACHINE."),
    ("cut_over", "2 mm", "Extra depth below the part"),
    ("mark_depth", "=surf_var + 0.5 mm", "Scoring pass depth: just marks the lowest spot"),
    ("safe_z", "5 mm", "Safe height"),
    ("clear_z", "20 mm", "Clearance height"),
]

NEST_PARAMS = [
    (None, "LAYOUT (follows Design and CAM)", ""),
    ("gap", "=CAM.tool_d + 0.25 in", "Gap between parts on a sheet"),
    ("panel_x", "=Design.panel_h + Design.t_s", "Frame panel height, laid across the sheet"),
    ("long_len", "=-2 * Design.rx0 + Design.t_l", "Inner long rib length"),
    ("cross_len", "=-2 * Design.ry0 + Design.t_l", "Inner cross rib length"),
    ("rib_w", "=Design.box_h", "Rib width including its tabs"),
    ("strip_x0", "=CAM.edge_m + panel_x + gap", "Where the rib strip starts beside a frame panel"),
    ("strip_cols", "=floor((CAM.reach_x - CAM.tool_r - strip_x0 + gap) / (rib_w + gap))",
     "Ribs side by side in that strip"),
    ("cross_rows", "=floor((CAM.reach_y - CAM.tool_r - CAM.edge_m + gap) / (cross_len + gap))",
     "Cross ribs end to end in that strip"),
    ("long_cap", "=strip_cols", "Long ribs that fit beside a side panel"),
    ("cross_cap", "=strip_cols * cross_rows", "Cross ribs that fit beside one panel"),
    (None, "RIB COUNTS (set when the jobs were built)", ""),
    ("built_long", "0", "Long ribs on Sheet 2"),
    ("built_cross3", "0", "Cross ribs on Sheet 3"),
    ("built_cross4", "0", "Cross ribs on Sheet 4"),
    (None, "CHECKS (all should read OK)", ""),
]

NEST_CHECKS = [
    ("chk_counts", ["built_long == Design.ny - 1", "built_cross3 + built_cross4 == Design.nx - 1"],
     "rib counts changed: run build_cam.rebuild_job() for Sheets 2-4"),
    ("chk_long", ["Design.ny - 1 <= long_cap"], "too many long ribs for Sheet 2"),
    ("chk_cross", ["Design.nx - 1 <= 2 * cross_cap"], "too many cross ribs for Sheets 3 and 4"),
    ("chk_thick", ["abs(Design.t_r - Design.t_l) < 0.01 mm"],
     "ribs and frame panels differ in thickness but share Sheets 2-4"),
    ("chk_side", ["CAM.edge_m + Design.foot_L + CAM.tool_r <= CAM.reach_y",
                  "CAM.edge_m + panel_x + CAM.tool_r <= CAM.reach_x"],
     "a side panel is beyond the machine's reach"),
    ("chk_end", ["CAM.edge_m + Design.foot_W + CAM.tool_r <= CAM.reach_y"], "an end panel is beyond reach"),
    ("chk_plate", ["CAM.edge_m + Design.in_L + CAM.tool_r <= CAM.reach_y",
                   "CAM.edge_m + Design.in_W + 2 * Design.t_l + CAM.tool_r <= CAM.reach_x"],
     "the bottom skin or shelf is beyond reach"),
    ("chk_top", ["abs(Design.top_L - CAM.sheet_y) < 0.1 mm", "abs(Design.top_W - CAM.sheet_x) < 0.1 mm"],
     "the top skin is no longer the full sheet: its outline needs cutting (see README)"),
]

# How each body lies on the sheet. Sheet X = across, sheet Y = along.
#   side: body X -> sheet Y, body Z -> sheet X   (side panels, long ribs)
#   end:  body Y -> sheet Y, body Z -> sheet X   (end panels, cross ribs)
#   flat: body X -> sheet Y, body Y -> sheet X   (skins, shelf)
ROT = {
    "side": App.Rotation(App.Matrix(0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1)),
    "end": App.Rotation(App.Matrix(0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1)),
    "flat": App.Rotation(App.Vector(0, 0, 1), 90),
}
ORIENT = {"SideFront": "side", "SideBack": "side", "LongInner": "side",
          "EndPanel": "end", "CrossInner": "end",
          "TopSkin": "flat", "BottomSkin": "flat", "Shelf": "flat"}
# Distance from a part's lower-left corner on the sheet to its body origin.
HALF_Y = {"SideFront": "Design.foot_L / 2", "SideBack": "Design.foot_L / 2",
          "EndPanel": "Design.foot_W / 2", "LongInner": "Nest.long_len / 2",
          "CrossInner": "Nest.cross_len / 2", "TopSkin": "Design.top_L / 2",
          "BottomSkin": "Design.in_L / 2", "Shelf": "Design.in_L / 2"}
HALF_X = {"TopSkin": "Design.top_W / 2", "BottomSkin": "Design.in_W / 2 + Design.t_l",
          "Shelf": "Design.in_W / 2 + Design.t_l"}
THICK = {"SideFront": "Design.t_l", "SideBack": "Design.t_l", "EndPanel": "Design.t_l",
         "LongInner": "Design.t_r", "CrossInner": "Design.t_r", "TopSkin": "Design.t_s",
         "BottomSkin": "Design.t_s", "Shelf": "Design.t_l"}

# (job, label, main part, ribs in the strip beside it)
SHEETS = [
    ("Sheet1", "Sheet 1 - top skin (factory edges)", "TopSkin", None),
    ("Sheet2", "Sheet 2 - front side panel and long ribs", "SideFront", "long"),
    ("Sheet3", "Sheet 3 - back side panel and cross ribs", "SideBack", "cross3"),
    ("Sheet4", "Sheet 4 - end panel and cross ribs", "EndPanel", "cross4"),
    ("Sheet5", "Sheet 5 - end panel", "EndPanel", None),
    ("Sheet6", "Sheet 6 - bottom skin", "BottomSkin", None),
    ("Sheet7", "Sheet 7 - shelf", "Shelf", None),
]
FACTORY_EDGES = {"Sheet1"}     # the part IS the sheet: cut only its inside features


def doc():
    return App.ActiveDocument


def job_names():
    return [s[0] for s in SHEETS]


def _q(v):
    return float(getattr(v, "Value", v))


def _check_expr(conds, msg):
    """Nested ternaries: FreeCAD expressions have no && operator."""
    if not conds:
        return "<<OK>>"
    return "(%s ? %s : <<%s>>)" % (conds[0], _check_expr(conds[1:], msg), msg)


def _fill(sheet, rows, r):
    for alias, value, note in rows:
        if alias is None:
            r += 1
            sheet.set("A%d" % r, value)
            sheet.setStyle("A%d" % r, "bold")
        else:
            sheet.set("A%d" % r, alias)
            sheet.set("B%d" % r, value)
            sheet.setAlias("B%d" % r, alias)
            if note:
                sheet.set("C%d" % r, note)
        r += 1
    return r


def _inch_display(sheet, last_row):
    for r in range(1, last_row):
        try:
            v = sheet.get("B%d" % r)
        except Exception:
            continue
        if hasattr(v, "Unit") and "[Length]" in str(v.Unit) and "mm/min" not in sheet.getContents("B%d" % r):
            sheet.setDisplayUnit("B%d" % r, "in")


def build_sheets():
    """The CAM sheet (machine, tool, depths) and the Nest sheet (layout)."""
    d = doc()
    cam = d.addObject("Spreadsheet::Sheet", "CAM")
    cam.Label = "CAM parameters"
    cam.set("A1", "CAM parameters: machine X across the sheet, Y along it, Z0 on top of the material")
    cam.setStyle("A1", "bold")
    clast = _fill(cam, CAM_PARAMS, 2)
    nest = d.addObject("Spreadsheet::Sheet", "Nest")
    nest.Label = "Nest"
    nest.set("A1", "Nesting: positions follow Design; rib counts are set when the jobs are built")
    nest.setStyle("A1", "bold")
    nlast = _fill(nest, NEST_PARAMS + [(a, "=" + _check_expr(c, m), "") for a, c, m in NEST_CHECKS], 2)
    for s in (cam, nest):
        s.setColumnWidth("A", 110)
        s.setColumnWidth("B", 110)
        s.setColumnWidth("C", 480)
    d.recompute([cam, nest])
    _inch_display(cam, clast)
    _inch_display(nest, nlast)
    d.recompute([cam, nest])
    return cam, nest


# ------------------------------------------------------------ nesting plan
def plan(job):
    """[(clone label, body, x corner expr, y corner expr)] for one sheet."""
    d = doc()
    ds, ns = d.getObject("Design"), d.getObject("Nest")
    spec = dict((s[0], s) for s in SHEETS)[job]
    main, ribs = spec[2], spec[3]
    out = [("%s_%s" % (job, main), main, "0 mm" if job in FACTORY_EDGES else "CAM.edge_m",
            "0 mm" if job in FACTORY_EDGES else "CAM.edge_m")]
    if ribs is None:
        return out
    cols = int(_q(ns.get("strip_cols")))
    if ribs == "long":
        n = int(_q(ds.get("ny"))) - 1
        for i in range(min(n, cols)):
            out.append(("%s_LongInner_%d" % (job, i + 1), "LongInner",
                        "Nest.strip_x0 + %d * (Nest.rib_w + Nest.gap)" % i,
                        "CAM.edge_m + (Design.foot_L - Nest.long_len) / 2"))
        return out
    total = int(_q(ds.get("nx"))) - 1
    cap = int(_q(ns.get("cross_cap")))
    n = min(total, cap) if ribs == "cross3" else max(0, min(total - cap, cap))
    rows = max(1, int(_q(ns.get("cross_rows"))))
    for i in range(n):
        c, r = i // rows, i % rows
        out.append(("%s_CrossInner_%d" % (job, i + 1), "CrossInner",
                    "Nest.strip_x0 + %d * (Nest.rib_w + Nest.gap)" % c,
                    "CAM.edge_m + %d * (Nest.cross_len + Nest.gap)" % r))
    return out


def _place(clone, body, x, y):
    """Bind a clone's placement: fixed orientation, expression-driven position."""
    o = ORIENT[body]
    clone.Placement = App.Placement(App.Vector(0, 0, 0), ROT[o])
    bx = "(%s) + %s" % (x, HALF_X[body]) if o == "flat" else x
    by = "(%s) + %s" % (y, HALF_Y[body])
    bz = "-(%s)" % THICK[body] if o == "flat" else "-(%s) / 2" % THICK[body]
    for c, e in (("x", bx), ("y", by), ("z", bz)):
        clone.setExpression("Placement.Base." + c, e)


# ----------------------------------------------------------------- jobs
def _op_view(op, gui_module):
    import Path.Op.Gui.Base as PathOpGui
    if App.GuiUp:
        op.ViewObject.Proxy = PathOpGui.ViewProvider(op.ViewObject, gui_module.Command.res)


def top_face(clone):
    """The clone's top face (at Z0): its outer wire is the outline, its inner wires the holes."""
    faces = [(i, f) for i, f in enumerate(clone.Shape.Faces)
             if abs(f.BoundBox.ZMin) < 1e-4 and abs(f.BoundBox.ZMax) < 1e-4]
    i, _ = max(faces, key=lambda t: t[1].Area)
    return "Face%d" % (i + 1)


def _bases(job):
    return [(c, (top_face(c),)) for c in job.Model.Group]


def _profile(job, name, label, perimeter, holes, final_depth):
    from Path.Op import Profile as PProfile
    import Path.Op.Gui.Profile as ProfileGui
    op = PProfile.Create(name, parentJob=job)
    _op_view(op, ProfileGui)
    op.Label = label
    # Each model's top face. With no faces at all, Profile cuts only the outer
    # envelope and silently skips every slot and opening.
    op.Base = _bases(job)
    op.processPerimeter = perimeter
    op.processHoles = holes
    op.processCircles = False
    op.Side = "Outside"
    op.Direction = "CCW"
    op.UseComp = True
    op.setExpression("StartDepth", "0 mm")
    op.setExpression("FinalDepth", final_depth)
    op.setExpression("StepDown", "CAM.step_down")
    op.setExpression("SafeHeight", "CAM.safe_z")
    op.setExpression("ClearanceHeight", "CAM.clear_z")
    return op


def _stop(job, name, label):
    import Path.Op.Gui.Stop as StopGui
    op = doc().addObject("Path::FeaturePython", name)
    StopGui.Stop(op)
    op.Stop = "Mandatory"
    if App.GuiUp:
        StopGui._ViewProviderStop(op.ViewObject)
    job.Proxy.addOperation(op)
    op.Label = label
    return op


def _dogbone(op, name, side):
    import Path.Dressup.Gui.DogboneII as DB     # Gui variant: attaches the view provider, no task panel
    db = DB.Create(op, name)
    db.Style = "Dogbone"
    db.Incision = "adaptive"
    db.Side = side
    db.Label = op.Label + " + dogbones"
    return db


def _model_clone(job, body):
    """A job model clone, made the way the Job makes its own (Path.Main.Job.
    createResourceClone) but without its whole-document recompute, which
    freezes the GUI for tens of seconds per clone."""
    import Draft
    clone = Draft.clone(body)
    clone.Label = "Model-%s" % body.Label
    clone.addProperty("App::PropertyString", "PathResource")
    clone.PathResource = "Model"
    if App.GuiUp:
        import Path.Base.Gui.IconViewProvider
        Path.Base.Gui.IconViewProvider.Attach(clone.ViewObject, "BaseGeometry")
        clone.ViewObject.Visibility = False
        clone.ViewObject.Transparency = 80
    job.Model.addObject(clone)
    return clone


def build_job(job_name):
    """One sheet: clones of its parts, a sheet-sized stock, the tool, and the ops."""
    import Draft
    import Path.Main.Gui.Job as PathJobGui
    import Path.Main.Stock as PathStock
    d = doc()
    spec = dict((s[0], s) for s in SHEETS)[job_name]
    parts = plan(job_name)
    bodies = [d.getObject(b) for _, b, _, _ in parts]

    job = PathJobGui.Create([bodies[0]], None, openTaskPanel=False)   # NOT the App-layer Create
    job.Label = spec[1]
    clones = list(job.Model.Group)
    for label, body, _, _ in parts[1:]:
        clones.append(_model_clone(job, d.getObject(body)))
    for c, (label, body, x, y) in zip(clones, parts):
        c.Label = label
        _place(c, body, x, y)
    d.recompute(clones)               # placements are expressions: apply them before finding top faces

    thick = THICK[parts[0][1]]
    old = job.Stock
    stock = PathStock.CreateBox(job)
    stock.setExpression("Length", "CAM.sheet_x")
    stock.setExpression("Width", "CAM.sheet_y")
    stock.setExpression("Height", thick)
    stock.setExpression("Placement.Base.x", "0 mm")   # sheet corner at the machine origin
    stock.setExpression("Placement.Base.y", "0 mm")
    stock.setExpression("Placement.Base.z", "-(%s)" % thick)
    job.Stock = stock                 # assign BEFORE removing the old one
    d.removeObject(old.Name)

    tc = job.Tools.Group[0]           # retune the job's own controller; never share a ToolBit
    tc.Label = "T1 1/4 in end mill (%s)" % job_name
    tc.Tool.setExpression("Diameter", "CAM.tool_d")
    if hasattr(tc.Tool, "Flutes"):
        tc.Tool.setExpression("Flutes", "CAM.flutes")
    tc.setExpression("SpindleSpeed", "CAM.rpm")
    tc.setExpression("HorizFeed", "CAM.feed_xy")
    tc.setExpression("VertFeed", "CAM.feed_z")

    job.PostProcessor = "grbl"
    job.PostProcessorArgs = "--precision=3 --no-line-numbers"
    job.PostProcessorOutputFile = "%D/cam/" + job_name + ".nc"

    depth = "-(%s + CAM.cut_over + CAM.surf_var)" % thick
    factory = job_name in FACTORY_EDGES
    score = _profile(job, "OP_%s_score" % job_name, "1. SCORE outlines - %s" % job_name,
                     not factory, factory, "-CAM.mark_depth")
    stop = _stop(job, "STOP_%s" % job_name, "2. STOP - drive hold-down screws - %s" % job_name)
    ops = [score, stop]
    holes = _profile(job, "OP_%s_holes" % job_name, "3. CUT slots and openings - %s" % job_name,
                     False, True, depth)
    ops.append(_dogbone(holes, "Dogbone_%s_holes" % job_name, HOLE_SIDE))
    if not factory:
        outline = _profile(job, "OP_%s_outline" % job_name, "4. CUT outlines - %s" % job_name,
                           True, False, depth)
        ops.append(_dogbone(outline, "Dogbone_%s_outline" % job_name, OUTLINE_SIDE))
    job.Operations.Group = ops        # a dressup reorders the group: put it back
    _record_counts(job_name, parts)
    d.recompute()
    _relabel(job_name)
    return job


# Labels end in a bracket: FreeCAD renumbers a label that ends in a digit when
# it clashes (..Sheet4 -> ..Sheet001), and the G-code comments carry the label.
OP_LABELS = {"OP_%s_score": "1. SCORE outlines [%s]", "STOP_%s": "2. STOP - drive hold-down screws [%s]",
             "OP_%s_holes": "3. CUT slots and openings [%s]", "Dogbone_%s_holes": "3. CUT slots and openings + dogbones [%s]",
             "OP_%s_outline": "4. CUT outlines [%s]", "Dogbone_%s_outline": "4. CUT outlines + dogbones [%s]"}


def _relabel(job_name):
    """Labels as intended: while a rebuilt job's predecessor still exists, FreeCAD
    renames clashing labels (..Sheet4 -> ..Sheet001), and the G-code comments carry them."""
    job = find_job(job_name)
    if job is not None:
        job.Label = dict((s[0], s[1]) for s in SHEETS)[job_name]
    for name, label in OP_LABELS.items():
        o = doc().getObject(name % job_name)
        if o is not None:
            o.Label = label % job_name.replace("Sheet", "Sheet ")


# Which side of the toolpath the dogbones go: verified against slot and lap
# corners by check_corners().
OUTLINE_SIDE = "Left"
HOLE_SIDE = "Left"


def _record_counts(job_name, parts):
    ns = doc().getObject("Nest")
    n = sum(1 for _, b, _, _ in parts if b in ("LongInner", "CrossInner"))
    cell = {"Sheet2": "built_long", "Sheet3": "built_cross3", "Sheet4": "built_cross4"}.get(job_name)
    if cell:
        ns.set(ns.getCellFromAlias(cell), str(n))


def find_job(job_name):
    import Path.Main.Job as PathJob
    prefix = dict((s[0], s[1]) for s in SHEETS)[job_name].split(" - ")[0] + " -"
    for j in PathJob.Instances():
        if j.Label.startswith(prefix):
            return j
    return None


def _owned(obj, acc, via_bit=False):
    """Everything a CAM object owns: groups, stock, setup sheet, tool controllers,
    tool bits and the PartDesign Body each bit carries, ops and dressup bases.

    Never descends into an App::Link (a link forwards attribute lookups to the
    object it links, so link.Group is the linked Body's feature list) or into a
    model Body.
    """
    if obj is None or not hasattr(obj, "Name") or obj.Name in acc:
        return acc
    acc.add(obj.Name)
    if obj.TypeId == "App::Link" or (obj.TypeId == "PartDesign::Body" and not via_bit):
        return acc
    for p in ("Group", "Model", "Tools", "Operations", "OriginFeatures"):
        v = getattr(obj, p, None)
        for c in (v if isinstance(v, list) else [v]):
            if hasattr(c, "Name"):
                _owned(c, acc, via_bit)
    for p in ("Stock", "SetupSheet", "Tool", "Origin"):
        v = getattr(obj, p, None)
        if hasattr(v, "Name"):
            _owned(v, acc, via_bit)
    base = getattr(obj, "Base", None)
    if hasattr(base, "Name") and base.TypeId.startswith("Path::"):
        _owned(base, acc)                      # a dressup's wrapped op
    bit_body = getattr(obj, "BitBody", None)    # a ToolBit carries a whole PartDesign Body
    if hasattr(bit_body, "Name"):
        _owned(bit_body, acc, True)
    return acc


def protected():
    """Objects CAM cleanup must never delete: the model Bodies and everything in
    them, the assembly, every spreadsheet, every drawing."""
    d = doc()
    bit_bodies = set()
    for o in d.Objects:
        bb = getattr(o, "BitBody", None)
        if hasattr(bb, "Name"):
            bit_bodies.add(bb.Name)
    keep = set()

    def add(o):
        if o is None or not hasattr(o, "Name") or o.Name in keep:
            return
        keep.add(o.Name)
        if o.TypeId == "App::Link":
            return
        for c in (getattr(o, "Group", None) or []):
            add(c)
        org = getattr(o, "Origin", None)
        if hasattr(org, "Name"):
            add(org)
            for f in org.OriginFeatures:
                add(f)
    for o in d.Objects:
        if (o.TypeId == "PartDesign::Body" and o.Name not in bit_bodies) or o.TypeId.startswith(
                ("Assembly::", "Spreadsheet::", "TechDraw::")):
            add(o)
    return keep


def _delete(names):
    d = doc()
    names = set(names)
    blocked = names & protected()
    if blocked:
        raise RuntimeError("refusing to delete model objects: %s" % sorted(blocked)[:10])
    for n in sorted(names, key=lambda n: -len(d.getObject(n).InList) if d.getObject(n) else 0):
        if d.getObject(n) is not None:
            try:
                d.removeObject(n)
            except Exception:
                pass


def remove_job(job_name):
    """Delete a job and everything it owns. The model Bodies are never touched."""
    job = find_job(job_name)
    if job is not None:
        _delete(_owned(job, set()))


def remove_orphans():
    """CAM objects that no job owns any more (left by an interrupted build)."""
    import Path.Main.Job as PathJob
    d = doc()
    live = set()
    for j in PathJob.Instances():
        _owned(j, live)
    orphans = set()
    for o in d.Objects:
        if o.Name in live:
            continue
        is_cam = (o.TypeId.startswith("Path::") or hasattr(o, "BitBody")
                  or o.Name.startswith(("TC_", "SetupSheet", "Stock"))
                  # a job's Model / Tools / Operations groups, left with their clones
                  or (o.TypeId == "App::DocumentObjectGroup" and not o.InList
                      and o.Name.startswith(("Model", "Tools", "Operations"))))
        if is_cam:
            _owned(o, orphans)
    _delete(orphans - live)
    return sorted(orphans - live)


def stale_faces():
    """Face references that no longer resolve, per op (should be empty)."""
    import Path.Main.Job as PathJob
    out = []
    for j in PathJob.Instances():
        for op in j.Operations.Group:
            inner = op.Base if hasattr(op, "Base") and hasattr(op.Base, "Path") else op   # inside a dressup
            o = getattr(inner, "Base", None)
            o = o if isinstance(o, list) else []
            for base, subs in o:
                out += ["%s: %s %s" % (inner.Name, base.Label, s) for s in subs if s.startswith("?")]
    return out


def rebind_faces():
    """Point every profile op at its models' current top faces."""
    import Path.Main.Job as PathJob
    n = 0
    for j in PathJob.Instances():
        for op in j.Operations.Group:
            inner = op.Base if hasattr(op, "Base") and hasattr(op.Base, "Path") else op
            if isinstance(getattr(inner, "Base", None), list):
                inner.Base = []
                inner.Base = _bases(j)
                inner.Side = "Outside"     # reassigning Base can flip it, which cuts parts undersize
                n += 1
    return n


def built_counts():
    ns = doc().getObject("Nest")
    return {"Sheet2": int(_q(ns.get("built_long"))), "Sheet3": int(_q(ns.get("built_cross3"))),
            "Sheet4": int(_q(ns.get("built_cross4")))}


def update_cam():
    """After a design change: re-lay sheets whose rib count changed, re-point
    every op at its parts' current top faces, recompute and check everything."""
    d = doc()
    rebuilt = []
    have = built_counts()
    for job_name in ("Sheet2", "Sheet3", "Sheet4"):
        want = sum(1 for _, b, _, _ in plan(job_name) if b in ("LongInner", "CrossInner"))
        if want != have[job_name] or find_job(job_name) is None:
            rebuild_job(job_name)
            rebuilt.append(job_name)
    remove_orphans()
    rebound = rebind_faces()
    d.recompute()
    report = {"rebuilt": rebuilt, "ops re-pointed": rebound, "stale faces": stale_faces(),
              "reach": check_all(), "corners": check_all_corners(), "outline offset error mm": check_sizes()}
    return report


def check_sizes():
    """Each outline toolpath must sit one cutter radius outside its parts:
    [(job, worst error mm)]. Catches a profile cut on the wrong side."""
    d = doc()
    r = _q(d.getObject("CAM").get("tool_r"))
    out = []
    for n in job_names():
        op = d.getObject("OP_%s_outline" % n)
        if op is None:
            continue
        job = find_job(n)
        bb = None
        for c in job.Model.Group:
            bb = c.Shape.BoundBox if bb is None else bb.united(c.Shape.BoundBox)
        xs = [k.Parameters["X"] for k in op.Path.Commands if "X" in k.Parameters]
        ys = [k.Parameters["Y"] for k in op.Path.Commands if "Y" in k.Parameters]
        err = max(abs((bb.XMin - r) - min(xs)), abs((bb.XMax + r) - max(xs)),
                  abs((bb.YMin - r) - min(ys)), abs((bb.YMax + r) - max(ys)))
        out.append((n, round(err, 3)))
    return out


def check_all_corners():
    """Dogbone check on every cutting op: [(job, kind, corners, not cleared, worst gap mm)]."""
    out = []
    for n in job_names():
        for kind in ("holes", "outline"):
            if doc().getObject("Dogbone_%s_%s" % (n, kind)):
                out.append((n, kind) + check_corners(n, "Dogbone_%s_%s" % (n, kind), kind))
    return out


def rebuild_job(job_name):
    remove_job(job_name)
    return build_job(job_name)


# --------------------------------------------------------------- checks
def path_extents(job):
    """(xmin, xmax, ymin, ymax, zmin) of the cutter centre over every op."""
    xs, ys, zs = [], [], []
    for op in job.Operations.Group:
        if not hasattr(op, "Path"):
            continue
        for cmd in op.Path.Commands:
            p = cmd.Parameters
            if cmd.Name in ("G0", "G00", "G1", "G01", "G2", "G02", "G3", "G03"):
                if "X" in p:
                    xs.append(p["X"])
                if "Y" in p:
                    ys.append(p["Y"])
                if "Z" in p:
                    zs.append(p["Z"])
    if not xs:
        return None
    return min(xs), max(xs), min(ys), max(ys), min(zs)


def check_all():
    """Every job: has cutting moves, stays inside the reach, cuts deep enough."""
    d = doc()
    cam = d.getObject("CAM")
    rx, ry = _q(cam.get("reach_x")), _q(cam.get("reach_y"))
    report = []
    for name in job_names():
        job = find_job(name)
        ext = path_extents(job)
        if ext is None:
            report.append((name, "NO TOOLPATH"))
            continue
        x0, x1, y0, y1, z0 = ext
        ok = x0 >= -1e-6 and y0 >= -1e-6 and x1 <= rx + 1e-6 and y1 <= ry + 1e-6
        report.append((name, "%s  X %.3f..%.3f in  Y %.3f..%.3f in  min Z %.2f mm" % (
            "OK  " if ok else "OUT OF REACH", x0 / 25.4, x1 / 25.4, y0 / 25.4, y1 / 25.4, z0)))
    return report


# ----------------------------------------------------------------- post
def post_job(job_name):
    """Write the job's G-code to cam/<job>.nc and verify the file.

    Refuses if any face reference is stale, the toolpath leaves the machine's
    reach, or a dogbone misses a corner: run update_cam() first.
    """
    from Path.Post.Processor import PostProcessorFactory
    job = find_job(job_name)
    problems = [s for s in stale_faces() if ("_%s_" % job_name) in s]
    problems += [r[1] for r in check_all() if r[0] == job_name and not r[1].startswith("OK")]
    problems += ["%s %s: %d corners not cleared" % (c[0], c[1], c[3]) for c in check_all_corners()
                 if c[0] == job_name and c[3]]
    problems += ["%s outline is %.2f mm off one cutter radius outside the parts" % (n, e)
                 for n, e in check_sizes() if n == job_name and e > 0.05]
    if problems:
        raise RuntimeError("not posting %s: %s" % (job_name, "; ".join(problems)))
    os.makedirs(OUT_DIR, exist_ok=True)
    args = job.PostProcessorArgs
    job.PostProcessorArgs = args + " --no-show-editor"   # the editor is modal: it would hang the MCP
    try:
        sections = PostProcessorFactory.get_post_processor(job, job.PostProcessor).export()
    finally:
        job.PostProcessorArgs = args                      # so the GUI Post Process button shows the editor
    gcode = "\n".join(g for _, g in sections)
    path = os.path.join(OUT_DIR, job_name + ".nc")
    with open(path, "w") as fh:
        fh.write(gcode)
    order = re.findall(r"\(Begin operation: (.*?)\)", gcode)
    return path, len(gcode.splitlines()), "G21" in gcode and "G20" not in gcode, order


# ------------------------------------------------------- corner clearance
def _concave_corners(face, which):
    """Concave corners a square-ended mating part must reach: inside corners of
    the outline (notches, slots, tab shoulders), or every sharp corner of a hole.
    Only corners between two straight edges count; arcs have none."""
    pts = []
    wires = [face.OuterWire] if which == "outline" else [w for w in face.Wires if not w.isSame(face.OuterWire)]
    for w in wires:
        es = w.OrderedEdges
        vs = [v.Point for v in w.OrderedVertexes]
        n = len(es)
        if n < 3:
            continue
        # winding from the vertex polygon (arcs become chords, which is enough for the sign)
        area = sum(vs[i].x * vs[(i + 1) % n].y - vs[(i + 1) % n].x * vs[i].y for i in range(n)) / 2
        for i in range(n):
            e0, e1 = es[i - 1], es[i]
            if e0.Curve.TypeId != "Part::GeomLine" or e1.Curve.TypeId != "Part::GeomLine":
                continue
            a, b, c = vs[i - 1], vs[i], vs[(i + 1) % n]
            cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x)
            if abs(cross) < 1e-9:
                continue              # straight through, not a corner
            # a hole's material is outside it, so its convex-in-the-wire corners are concave in the part
            if (which == "holes" and cross * area > 0) or (which == "outline" and cross * area < 0):
                pts.append(b)
    return pts


def _path_segments(op, z_max):
    segs, last = [], None
    for cmd in op.Path.Commands:
        p = cmd.Parameters
        if cmd.Name not in ("G1", "G01", "G2", "G02", "G3", "G03", "G0", "G00"):
            continue
        cur = App.Vector(p.get("X", last.x if last else 0), p.get("Y", last.y if last else 0),
                         p.get("Z", last.z if last else 0))
        if last is not None and cmd.Name not in ("G0", "G00") and cur.z <= z_max and last.z <= z_max:
            segs.append((last, cur))
        last = cur
    return segs


def _dist(p, a, b):
    ab = b - a
    L2 = ab.x ** 2 + ab.y ** 2
    t = 0 if L2 == 0 else max(0, min(1, ((p.x - a.x) * ab.x + (p.y - a.y) * ab.y) / L2))
    return ((a.x + t * ab.x - p.x) ** 2 + (a.y + t * ab.y - p.y) ** 2) ** 0.5


def check_corners(job_name, op_name, which):
    """(corners, not cleared, worst gap mm) for one op's final pass."""
    d = doc()
    job, op = find_job(job_name), d.getObject(op_name)
    r = _q(d.getObject("CAM").get("tool_r"))
    zs = [c.Parameters["Z"] for c in op.Path.Commands if "Z" in c.Parameters]
    segs = _path_segments(op, min(zs) + 1e-6)
    # grid index so each corner only checks nearby segments
    cell, grid = 20.0, {}
    for s in segs:
        for gx in range(int(min(s[0].x, s[1].x) // cell), int(max(s[0].x, s[1].x) // cell) + 1):
            for gy in range(int(min(s[0].y, s[1].y) // cell), int(max(s[0].y, s[1].y) // cell) + 1):
                grid.setdefault((gx, gy), []).append(s)
    n = bad = 0
    worst = 0.0
    for c in job.Model.Group:
        face = c.Shape.getElement(top_face(c))
        for p in _concave_corners(face, which):
            near = [s for gx in (-1, 0, 1) for gy in (-1, 0, 1)
                    for s in grid.get((int(p.x // cell) + gx, int(p.y // cell) + gy), [])]
            dist = min([_dist(p, a, b) for a, b in near] or [1e9])
            n += 1
            if dist > r + 0.01:
                bad += 1
                worst = max(worst, dist - r)
    return n, bad, round(worst, 3)
