# Main component — Rotary HID.
#
# GENERATED UI: main/ui/ is written by EEZ Studio's Build (Ctrl+B) from
# GUI/RotaryHid.eez-project and is entirely disposable -- delete the whole
# folder at any time and re-export to recreate it. Nothing hand-written lives
# there, which is why actions.c and vars.c sit here in main/ instead.
#
# With main/ui/ absent the project still compiles: it builds a placeholder
# firmware that asks for an export.

# CONFIGURE_DEPENDS makes CMake re-check the globs when the file set changes,
# so a fresh export (or a deleted one) is picked up without a manual
# reconfigure. It is illegal in the script-mode pass ESP-IDF runs first to
# collect REQUIRES, hence the guard.
if(CMAKE_SCRIPT_MODE_FILE)
    set(UI_GLOB_MODE "")
else()
    set(UI_GLOB_MODE CONFIGURE_DEPENDS)
endif()

file(GLOB UI_GENERATED ${UI_GLOB_MODE}
     "ui/screens.c" "ui/styles.c" "ui/images.c" "ui/ui.c")
file(GLOB UI_FONTS  ${UI_GLOB_MODE} "ui/ui_font_*.c")
file(GLOB UI_IMAGES ${UI_GLOB_MODE} "ui/ui_image_*.c")

# A stale glob would link a firmware with NO UI in it and still report
# success. Fail loudly instead.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ui/ui.h" AND NOT UI_GENERATED)
    message(FATAL_ERROR
        "main/ui/ui.h exists but no generated sources were found. "
        "Run `idf.py reconfigure` after an EEZ Studio export.")
endif()

# Whether the export is present is decided HERE and passed to the compiler as
# a define, not with `#if __has_include("ui.h")` in C. __has_include records no
# dependency on a file that does not exist, so a main.c compiled before the
# first export would bake in "no UI" and never be rebuilt when the export
# appeared. A define that changes with the glob forces the rebuild.
if(UI_GENERATED)
    set(APP_HAVE_UI 1)
    # actions.c / vars.c / ui_glue.c use symbols the export declares, so
    # they only compile once it exists.
    set(UI_HANDWRITTEN "actions.c" "vars.c" "ui_glue.c")
    # Only registered when present: ESP-IDF rejects an include directory that
    # does not exist, which would break the build the moment main/ui/ is
    # deleted.
    set(UI_INCLUDE_DIR "ui")
else()
    set(APP_HAVE_UI 0)
    set(UI_HANDWRITTEN "")
    set(UI_INCLUDE_DIR "")
    message(WARNING
        "No generated UI in main/ui/. Building a placeholder firmware. Open "
        "GUI/RotaryHid.eez-project in EEZ Studio and press Ctrl+B.")
endif()

idf_component_register(
    SRCS
        "main.c"
        "app_model.c"
        "clock.c"
        ${UI_HANDWRITTEN}
        ${UI_GENERATED}
        ${UI_FONTS}
        ${UI_IMAGES}
    INCLUDE_DIRS
        "."
        ${UI_INCLUDE_DIR}
    REQUIRES
        matouch_board
        rotary_usb
        espressif__tinyusb
        nvs_flash
        esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE APP_HAVE_UI=${APP_HAVE_UI})
