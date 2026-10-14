# Crack the Safe main component.
#
# GENERATED UI: main/ui/ is written by EEZ Studio's Build (Ctrl+B) from
# GUI/CrackTheSafe.eez-project and is entirely disposable -- delete the whole
# folder at any time and re-export to recreate it. Nothing hand-written lives
# there, which is why actions.c and vars.c sit here in main/ instead.
#
# With main/ui/ absent the project still compiles: it builds a placeholder
# firmware that asks for an export.

set(UI_DIR "ui")

# CONFIGURE_DEPENDS re-checks the globs when the file set changes, so a fresh
# export (or a deleted one) is picked up without a manual reconfigure. It is
# illegal in the CMake script mode ESP-IDF uses for its first pass over every
# component, hence the guard.
if(CMAKE_SCRIPT_MODE_FILE)
    set(UI_GLOB_MODE "")
else()
    set(UI_GLOB_MODE CONFIGURE_DEPENDS)
endif()

file(GLOB UI_CORE   ${UI_GLOB_MODE} "${UI_DIR}/screens.c" "${UI_DIR}/styles.c"
                                    "${UI_DIR}/images.c"  "${UI_DIR}/ui.c")
file(GLOB UI_FONTS  ${UI_GLOB_MODE} "${UI_DIR}/ui_font_*.c")
file(GLOB UI_IMAGES ${UI_GLOB_MODE} "${UI_DIR}/ui_image_*.c")

# A stale glob links a firmware with NO UI and still reports success. Fail
# loudly instead.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${UI_DIR}/ui.h" AND NOT UI_CORE)
    message(FATAL_ERROR
        "main/ui/ui.h exists but no generated sources were found -- the glob "
        "is stale. Run `idf.py reconfigure` after an EEZ Studio export.")
endif()

# Whether the export exists is decided HERE and passed as a define, not with
# `__has_include` in C: that records no dependency on an absent file, so a
# build made before the first export would never notice the export appear.
if(UI_CORE)
    set(APP_HAVE_UI 1)
    set(APP_SRCS "game.c" "app.c" "ui_game.c" "actions.c" "bt_link.c"
                 "sound.c" "sfx.c")
    # Only registered when present: ESP-IDF rejects an include directory that
    # does not exist, which would break the build the moment main/ui/ is
    # deleted.
    set(UI_INCLUDE "${UI_DIR}")
else()
    set(APP_HAVE_UI 0)
    set(APP_SRCS "")
    set(UI_INCLUDE "")
    message(WARNING
        "No generated UI in main/ui -- building a placeholder firmware. Open "
        "GUI/CrackTheSafe.eez-project in EEZ Studio and press Ctrl+B.")
endif()

idf_component_register(
    SRCS
        "main.c"
        ${APP_SRCS}
        ${UI_CORE}
        ${UI_FONTS}
        ${UI_IMAGES}
    INCLUDE_DIRS
        "."
        ${UI_INCLUDE}
    REQUIRES
        knob_board
        esp_timer
        esp_hw_support
        mbedtls
        nvs_flash
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE APP_HAVE_UI=${APP_HAVE_UI})
