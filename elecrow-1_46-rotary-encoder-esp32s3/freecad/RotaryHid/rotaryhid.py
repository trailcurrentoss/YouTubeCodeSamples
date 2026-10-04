# Rotary Macro Pad -- FreeCAD view rotation from the pad's ring.
#
# The pad sends F14 / F15 / F16 for the X / Y / Z axis, with Shift for the
# other direction. On X11 those arrive as XF86Launch5-7, which Qt reports as
# Key_Launch5-7 (verified in FreeCAD 1.1 by logging the key events). Windows
# and macOS report them as Key_F14-16. Both are registered, so the add-on
# works unchanged on any OS.
#
# The keys are registered as Qt shortcuts on FreeCAD's main window rather
# than as FreeCAD command shortcuts, for two reasons found the hard way:
#
#   - FreeCAD normalises shortcut strings by stripping spaces, which turns
#     Qt's "Launch (5)" into "Launch(5)" -- a string Qt cannot parse -- so
#     these keys cannot be bound through Tools -> Customize at all.
#   - Macro commands created from Python get no action (and so no working
#     shortcut) until something else builds one.
#
# QShortcut takes the Qt key code directly, so neither applies.

import math

import FreeCAD as App
import FreeCADGui as Gui
from PySide import QtCore, QtGui

try:
    QShortcut = QtGui.QShortcut          # Qt 6
except AttributeError:                   # Qt 5
    from PySide import QtWidgets
    QShortcut = QtWidgets.QShortcut

PARAM = "User parameter:BaseApp/Preferences/Mod/RotaryHid"

AXES = {
    #      world axis   keys it may arrive as: X11,            other OSes
    "X": ((1, 0, 0), (QtCore.Qt.Key_Launch5, QtCore.Qt.Key_F14)),
    "Y": ((0, 1, 0), (QtCore.Qt.Key_Launch6, QtCore.Qt.Key_F15)),
    "Z": ((0, 0, 1), (QtCore.Qt.Key_Launch7, QtCore.Qt.Key_F16)),
}


def step_degrees():
    """Degrees per ring click. Match the firmware's CONFIG_ROTARY_STEP_DEG."""
    return App.ParamGet(PARAM).GetFloat("StepDeg", 5.0)


def rotate(axis, sign):
    """Orbit the camera about a world axis through the focal point, so the
    model appears to turn in place by +/- step_degrees()."""
    from pivy import coin

    doc = Gui.ActiveDocument
    if doc is None:
        return
    view = doc.ActiveView
    if not hasattr(view, "getCameraNode"):
        return   # not a 3D view (e.g. a spreadsheet or TechDraw page)

    cam = view.getCameraNode()
    vec = AXES[axis][0]
    # Rotating the camera by -angle turns the model by +angle.
    rot = coin.SbRotation(coin.SbVec3f(*vec), -math.radians(sign * step_degrees()))
    ori = cam.orientation.getValue()
    dist = cam.focalDistance.getValue()
    look = ori.multVec(coin.SbVec3f(0, 0, -1))
    focal = cam.position.getValue() + look * dist

    new_ori = ori * rot          # camera's own rotation, then the world one
    new_look = new_ori.multVec(coin.SbVec3f(0, 0, -1))
    cam.orientation.setValue(new_ori)
    cam.position.setValue(focal - new_look * dist)


_shortcuts = []


def install():
    """Register the ring shortcuts on the main window. Idempotent."""
    mw = Gui.getMainWindow()
    if mw is None or _shortcuts:
        return len(_shortcuts)
    for axis, (_, keys) in AXES.items():
        for key in keys:
            name = QtGui.QKeySequence(key).toString()
            for sign, seq in ((+1, name), (-1, "Shift+" + name)):
                sc = QShortcut(QtGui.QKeySequence(seq), mw)
                sc.setContext(QtCore.Qt.ApplicationShortcut)
                sc.activated.connect(lambda a=axis, s=sign: rotate(a, s))
                _shortcuts.append(sc)
    App.Console.PrintLog("RotaryHid: ring shortcuts active\n")
    return len(_shortcuts)


def uninstall():
    while _shortcuts:
        sc = _shortcuts.pop()
        sc.setEnabled(False)
        sc.setParent(None)
        sc.deleteLater()
