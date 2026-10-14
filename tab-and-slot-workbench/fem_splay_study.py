"""FEM sweep of the leg splay -- run headless, never inside the GUI.

Builds the whole bench as assembled, bonds every joint (the fit clearance
is set to zero in this throwaway copy, so mating faces coincide, as if the
glue filled them), fixes the feet, and loads the worktop with a vertical
load plus a sideways push along both the length and the depth. Solves
with CalculiX for each splay value and writes one CSV row per run.

    freecadcmd fem_splay_study.py

The bench is symmetric about both vertical mid-planes, so the push along
the length produces no average sway across the depth and vice versa: one
run gives the sway in each direction.

Settings (environment variables, all optional):

    FEM_MODEL   model file             (default: tab_slot_workbench.FCStd here)
    FEM_SPLAYS  splays in inches       (default: 0,1,2,3,4,5,6,8)
    FEM_PARAM   which splay to sweep   (default: splay_W; the model's own value
                is kept for the other one)
    FEM_V       worktop load, N        (default: 4000, about 900 lbf)
    FEM_H       push each way, N       (default: 900, about 200 lbf)
    FEM_MESH    max element size, mm   (default: 20)
    FEM_OUT     CSV path               (default: fem_splay.csv here)

The model file is opened read-only: nothing is saved back to it.
"""
import csv
import math
import os
import time

import FreeCAD as App
import ObjectsFem
from femmesh.gmshtools import GmshTools
from femtools import ccxtools

HERE = os.path.dirname(os.path.abspath(globals().get("__file__", "fem_splay_study.py")))
MODEL = os.environ.get("FEM_MODEL", os.path.join(HERE, "tab_slot_workbench.FCStd"))
SPLAYS = [float(s) for s in os.environ.get("FEM_SPLAYS", "0,1,2,3,4,5,6,8").split(",")]
PARAMS = os.environ.get("FEM_PARAM", "splay_W").split(",")
V_N = float(os.environ.get("FEM_V", "4000"))
H_N = float(os.environ.get("FEM_H", "900"))
MESH = float(os.environ.get("FEM_MESH", "20"))
ALGOS = os.environ.get("FEM_ALGO", "HXT,Delaunay").split(",")   # Frontal leaves slivers on this geometry
OUT = os.environ.get("FEM_OUT", os.path.join(HERE, "fem_splay.csv"))

# MDF, typical published values. Bending strength (MOR) is the yardstick.
MDF = {"Name": "MDF", "YoungsModulus": "3500 MPa", "PoissonRatio": "0.25", "Density": "750 kg/m^3"}
MOR_MPA = 28.0


def q(sheet, alias):
    v = sheet.get(alias)
    return float(getattr(v, "Value", v))


def horizontal(f):
    try:
        return abs(abs(f.normalAt(0, 0).z) - 1) < 1e-6
    except Exception:
        return False


def faces_where(shape, test):
    return ["Face%d" % (i + 1) for i, f in enumerate(shape.Faces) if test(f)]


def straight_edge(shape, axis):
    """An edge parallel to a world axis, as a force direction reference."""
    for i, e in enumerate(shape.Edges):
        if e.Curve.TypeId == "Part::GeomLine" and abs(abs(e.Curve.Direction[axis]) - 1) < 1e-9:
            return "Edge%d" % (i + 1), e.Curve.Direction[axis] > 0
    raise RuntimeError("no straight edge along axis %d" % axis)


def bench_solid(src):
    """Every placed part of the assembly, fused into one bonded solid."""
    asm = src.getObject("Assembly")
    shapes = [o.Shape for o in asm.Group if hasattr(o, "Shape") and not o.Shape.isNull()]
    fused = shapes[0].fuse(shapes[1:]).removeSplitter()
    solids = fused.Solids
    if len(solids) != 1:
        raise RuntimeError("bench fused into %d solids, expected 1 -- a joint is not bonded" % len(solids))
    return solids[0]


def solve(shape, bench_h, box_z0):
    """Solve, re-meshing with another Gmsh algorithm or size if the mesh has a sliver."""
    for algo in ALGOS:
        for size in (MESH, MESH * 1.08, MESH * 0.93):
            r = _solve(shape, bench_h, box_z0, size, algo)
            if r is not None:
                r["mesh_mm"], r["mesh_algo"] = size, algo
                return r
            print("mesh %s %.1f mm gave no result (distorted element); retrying" % (algo, size), flush=True)
    raise RuntimeError("no mesh gave a valid solve")


def _solve(shape, bench_h, box_z0, size, algo):
    fem = App.newDocument("fem_run")
    part = fem.addObject("Part::Feature", "Bench")
    part.Shape = shape
    an = ObjectsFem.makeAnalysis(fem, "Analysis")
    solver = ObjectsFem.makeSolverCalculiXCcxTools(fem, "CCX")
    an.addObject(solver)
    mat = ObjectsFem.makeMaterialSolid(fem, "MDF")
    m = mat.Material
    m.update(MDF)
    mat.Material = m
    an.addObject(mat)

    feet = faces_where(shape, lambda f: horizontal(f) and f.BoundBox.ZMax < 1e-3)
    top = faces_where(shape, lambda f: horizontal(f) and f.BoundBox.ZMin > bench_h - 1e-3)
    fix = ObjectsFem.makeConstraintFixed(fem, "Feet")
    fix.References = [(part, feet)]
    an.addObject(fix)
    for name, axis, load, want_positive in (("Vertical", 2, V_N, False),
                                            ("PushX", 0, H_N, True), ("PushY", 1, H_N, True)):
        edge, positive = straight_edge(shape, axis)
        f = ObjectsFem.makeConstraintForce(fem, name)
        f.References = [(part, top)]
        f.Force = "%f N" % load
        f.Direction = (part, [edge])
        f.Reversed = positive != want_positive
        an.addObject(f)

    mesh = ObjectsFem.makeMeshGmsh(fem, "Mesh")
    mesh.Shape = part
    mesh.CharacteristicLengthMax = "%f mm" % size
    mesh.ElementOrder = "2nd"
    mesh.Algorithm3D = algo
    mesh.SecondOrderLinear = True   # curved mid-nodes on the arches give negative jacobians
    an.addObject(mesh)
    fem.recompute()
    err = GmshTools(mesh).create_mesh()
    if err:
        raise RuntimeError("gmsh: %s" % err)

    fea = ccxtools.FemToolsCcx(an, solver)
    fea.update_objects()
    fea.setup_working_dir()
    fea.setup_ccx()
    msg = fea.check_prerequisites()
    if msg:
        raise RuntimeError(msg)
    fea.write_inp_file()
    fea.ccx_run()
    fea.load_results()
    results = [o for o in fem.Objects if o.isDerivedFrom("Fem::FemResultObject")]
    if not results or results[0].Mesh is None:
        App.closeDocument(fem.Name)
        return None
    res = results[0]

    nodes = res.Mesh.FemMesh.Nodes
    vm = dict(zip(res.NodeNumbers, res.vonMises))
    disp = dict(zip(res.NodeNumbers, res.DisplacementVectors))
    legs = sorted(vm[n] for n, p in nodes.items() if p.z < box_z0 - 25)
    topn = [disp[n] for n, p in nodes.items() if p.z > bench_h - 1]
    out = {
        "nodes": len(nodes),
        "sway_x_mm": sum(v.x for v in topn) / len(topn),
        "sway_y_mm": sum(v.y for v in topn) / len(topn),
        "sag_mm": -sum(v.z for v in topn) / len(topn),
        "vm_leg_p99": legs[int(0.99 * (len(legs) - 1))],
        "vm_leg_p95": legs[int(0.95 * (len(legs) - 1))],
        "vm_max": max(res.vonMises),
    }
    App.closeDocument(fem.Name)
    return out


def main():
    src = App.openDocument(MODEL)
    # drawing pages would re-project on every recompute; this copy is never saved
    # Drawing pages are left alone: they would re-project on every recompute,
    # and deleting one headless segfaults FreeCAD (TechDraw::DrawPage::unsetupObject).
    model = [o for o in src.Objects if not o.TypeId.startswith("TechDraw::")]
    sh = src.getObject("Design")
    sh.set(sh.getCellFromAlias("fit"), "0 mm")      # bonded joints
    rows = []
    for s in SPLAYS:
        t0 = time.time()
        for alias in PARAMS:
            sh.set(sh.getCellFromAlias(alias), "%g in" % s)
        for o in model:
            o.touch()
        src.recompute(model)
        bench_h, box_z0, ov = q(sh, "bench_h"), q(sh, "box_z0"), q(sh, "ov")
        r = solve(bench_solid(src), bench_h, box_z0)
        r.update({
            "splay_in": s,
            "angle_deg": math.degrees(math.atan2(q(sh, "splay_W"), box_z0)),
            # sideways push at the worktop that tips the bench, as a fraction of its weight
            "tip_x": (q(sh, "frame_L") / 2 + q(sh, "splay_L")) / bench_h,
            "tip_y": (q(sh, "frame_W") / 2 + q(sh, "splay_W")) / bench_h,
            "toe_past_top_in": (q(sh, "splay_W") - ov) / 25.4,
            "safety_p99": MOR_MPA / r["vm_leg_p99"],
            "seconds": time.time() - t0,
        })
        rows.append(r)
        print("splay %4.1f in (%4.1f deg)  sway x %.3f  y %.3f mm  sag %.3f mm  leg p99 %.2f MPa  max %.2f  (%d nodes, %.0fs)"
              % (s, r["angle_deg"], r["sway_x_mm"], r["sway_y_mm"], r["sag_mm"], r["vm_leg_p99"],
                 r["vm_max"], r["nodes"], r["seconds"]), flush=True)
    App.closeDocument(src.Name)
    with open(OUT, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)
    print("wrote", OUT)


try:
    main()
except Exception:
    # freecadcmd exits 0 and prints nothing on an uncaught script error
    import traceback
    traceback.print_exc()
    raise SystemExit(1)
