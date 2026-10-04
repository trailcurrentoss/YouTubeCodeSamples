# FreeCAD runs InitGui.py from every folder in its Mod directory at GUI
# start-up. This add-on has no workbench; it only registers the Rotary Macro
# Pad's ring keys (see rotaryhid.py). Deferred to the event loop so the main
# window is fully up first.

from PySide import QtCore

import rotaryhid

QtCore.QTimer.singleShot(0, rotaryhid.install)
