"""Hand calculation of how much load the workbench can carry.

Reads every dimension from the model's Design spreadsheet, checks each way
the bench can fail with textbook beam and plate formulas, and prints the
load that would break it and a working load with safety factors applied.

    freecadcmd load_capacity.py                # uses tab_slot_workbench.FCStd here
    LC_MODEL=/path/to/model.FCStd freecadcmd load_capacity.py

Or in FreeCAD's Python console with the model open:

    exec(open("/path/to/load_capacity.py").read())

Material values are typical published figures for industrial MDF. Your
sheet may differ; override them with environment variables:

    LC_MOR    bending strength, MPa          (default 24, ~3500 psi)
    LC_E      stiffness, MPa                 (default 2800, ~400 ksi)
    LC_SHEAR  edgewise shear strength, MPa   (default 5)
    LC_COMP   edgewise crushing strength, MPa (default 12)
    LC_SF     safety factor, short-term loads (default 3)
    LC_CREEP  extra factor for loads left in place for weeks (default 2)
"""
import math
import os

import FreeCAD as App

MOR = float(os.environ.get("LC_MOR", "24"))
E = float(os.environ.get("LC_E", "2800"))
TAU = float(os.environ.get("LC_SHEAR", "5"))
COMP = float(os.environ.get("LC_COMP", "12"))
SF = float(os.environ.get("LC_SF", "3"))
CREEP = float(os.environ.get("LC_CREEP", "2"))
NU = 0.25
LBF = 4.44822   # N per lbf

# Roark's Formulas for Stress and Strain, Table 11.4: rectangular plate,
# all edges simply supported. b = short side, a = long side.
ROARK_UNIFORM_BETA = [(1.0, 0.2874), (1.2, 0.3762), (1.4, 0.4530), (1.6, 0.5172),
                      (1.8, 0.5688), (2.0, 0.6102), (3.0, 0.7134), (4.0, 0.7410), (99, 0.7500)]
ROARK_PATCH_BETA = [(1.0, 0.435), (1.2, 0.650), (1.4, 0.789), (1.6, 0.875),
                    (1.8, 0.927), (2.0, 0.958), (99, 1.000)]


def interp(table, x):
    for (x0, y0), (x1, y1) in zip(table, table[1:]):
        if x <= x1:
            return y0 + (y1 - y0) * (x - x0) / (x1 - x0)
    return table[-1][1]


def model_values():
    here = os.path.dirname(os.path.abspath(globals().get("__file__", "load_capacity.py")))
    doc = App.ActiveDocument if App.ActiveDocument and App.ActiveDocument.getObject("Design") else None
    opened = None
    if doc is None:
        opened = doc = App.openDocument(os.environ.get("LC_MODEL", os.path.join(here, "tab_slot_workbench.FCStd")))
    sh = doc.getObject("Design")
    names = ("t_s t_r t_l rib_h box_h nx ny px py rx0 ry0 in_L in_W top_L top_W panel_h "
             "box_z0 shelf_z foot_w").split()
    v = {}
    for n in names:
        x = sh.get(n)
        v[n] = float(getattr(x, "Value", x))
    if opened is not None:
        App.closeDocument(opened.Name)
    return v


def lb(newtons):
    return newtons / LBF


def main():
    v = model_values()
    ts, tr, tl, H = v["t_s"], v["t_r"], v["t_l"], v["rib_h"]
    results = []

    # 1. A heavy object on one cell of the top skin (vise foot, anvil, jack stand).
    #    The skin between ribs is a plate, simply supported on the ribs (the glue
    #    makes the real joint stiffer, so this is on the safe side). Load spread
    #    over a 2 in circle.
    a = max(v["px"], v["py"]) - tr
    b = min(v["px"], v["py"]) - tr
    r0 = 25.4
    rp = r0 if r0 >= 0.5 * ts else math.sqrt(1.6 * r0 ** 2 + ts ** 2) - 0.675 * ts
    k_point = 3 / (2 * math.pi * ts ** 2) * ((1 + NU) * math.log(2 * b / (math.pi * rp))
                                             + interp(ROARK_PATCH_BETA, a / b))
    results.append(("Point load on one skin cell (2 in footprint)", MOR / k_point,
                    "cell %.1f x %.1f in, skin %.2f in" % (a / 25.4, b / 25.4, ts / 25.4)))

    # 2. Load spread evenly over the whole top, failing one skin cell in bending.
    beta = interp(ROARK_UNIFORM_BETA, a / b)
    q = MOR * ts ** 2 / (beta * b ** 2)                      # MPa on one cell
    area = v["top_L"] * v["top_W"]
    results.append(("Even load: skin bending between ribs", q * area, "%.1f psi on the skin" % (q * 145.04)))

    # 3. The box as a beam spanning front to back between the side panels.
    span = -2 * v["ry0"]
    bf = v["in_L"]
    d = H + ts
    webs = (v["nx"] - 1) + 2                                  # inner cross ribs + both end panels
    I = 2 * (bf * ts ** 3 / 12 + bf * ts * (d / 2) ** 2) + webs * tr * H ** 3 / 12
    W_bend = MOR * I / (v["box_h"] / 2) * 8 / span           # M = W L / 8
    results.append(("Even load: box bending, front to back", W_bend, "span %.1f in" % (span / 25.4)))

    # 4. Same beam, the ribs failing in shear next to the side panels.
    Qs = bf * ts * d / 2 + webs * tr * (H / 2) * (H / 4)
    W_shear = 2 * TAU * I * webs * tr / Qs                   # V = W / 2
    results.append(("Even load: rib shear at the supports", W_shear, "%d webs" % webs))

    # 5. Side panels buckling between the shelf and the bottom skin. Each side
    #    panel carries half the load (the box spans front to back onto them).
    h = v["box_z0"] - (v["shelf_z"] + tl)
    L_free = v["in_L"]
    sig_cr = math.pi ** 2 * E * tl ** 2 / (12 * (1 - NU ** 2) * h ** 2) * (1 + (h / L_free) ** 2) ** 2
    W_buckle = 2 * sig_cr * L_free * tl
    results.append(("Even load: side panels buckling", W_buckle,
                    "%.1f in unbraced between shelf and bottom skin" % (h / 25.4)))

    # 6. The feet of the side panels crushing on the floor.
    W_feet = 2 * 2 * v["foot_w"] * tl * COMP
    results.append(("Even load: feet crushing", W_feet, "4 feet, %.1f x %.2f in each"
                    % (v["foot_w"] / 25.4, tl / 25.4)))

    print("Bench top %.0f x %.0f in, MDF skins %.2f in, ribs %.2f in, frame %.2f in"
          % (v["top_L"] / 25.4, v["top_W"] / 25.4, ts / 25.4, tr / 25.4, tl / 25.4))
    print("MDF: MOR %.0f MPa, E %.0f MPa, shear %.0f MPa, crushing %.0f MPa; safety factor %.1f, creep x%.1f\n"
          % (MOR, E, TAU, COMP, SF, CREEP))
    print("%-48s %10s %10s %10s" % ("Failure mode", "Breaks at", "Working", "Left on"))
    for name, w, note in results:
        print("%-48s %8.0f lb %8.0f lb %8.0f lb   %s" % (name, lb(w), lb(w) / SF, lb(w) / SF / CREEP, note))
    point = results[0][1]
    even = min(r[1] for r in results[1:])
    gov = min(results[1:], key=lambda r: r[1])[0]
    print("\nConcentrated load on one cell: works to %.0f lb, breaks near %.0f lb."
          % (lb(point) / SF, lb(point)))
    print("Load spread over the top:      works to %.0f lb (%.0f lb left on for weeks), limited by %s."
          % (lb(even) / SF, lb(even) / SF / CREEP, gov.split(": ")[-1]))
    return results


main()
