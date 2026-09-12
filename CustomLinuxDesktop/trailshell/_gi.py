"""Pins the GObject-Introspection versions the shell needs.

Import this *before* anything from ``gi.repository`` in every module. Without
it, whichever module happens to import first decides the version and PyGObject
warns (or worse, silently binds GTK 4 alongside GTK 3).
"""
import gi

gi.require_version("Gtk", "3.0")
gi.require_version("Gdk", "3.0")
gi.require_version("GdkPixbuf", "2.0")
gi.require_version("Pango", "1.0")
gi.require_version("PangoCairo", "1.0")
