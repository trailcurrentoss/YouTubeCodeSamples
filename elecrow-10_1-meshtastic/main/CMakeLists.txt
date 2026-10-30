# Main component — Meshtastic messenger UI.
#
# main/ui/ is EEZ Studio's export (open GUI/elecrow-10_1-meshtastic/*.eez-project,
# then Build / Ctrl+B). The globs re-check on every build so a fresh export is
# picked up; CONFIGURE_DEPENDS is illegal in ESP-IDF's script-mode pass.
if(CMAKE_SCRIPT_MODE_FILE)
    set(UI_GLOB_MODE "")
else()
    set(UI_GLOB_MODE CONFIGURE_DEPENDS)
endif()

file(GLOB UI_GENERATED ${UI_GLOB_MODE} "ui/screens.c" "ui/styles.c" "ui/images.c" "ui/ui.c")
file(GLOB UI_FONTS ${UI_GLOB_MODE} "ui/ui_font_*.c")
file(GLOB UI_IMAGES ${UI_GLOB_MODE} "ui/ui_image_*.c")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ui/ui.h" AND NOT UI_GENERATED)
    message(FATAL_ERROR "main/ui/ui.h exists but no generated sources were found. "
                        "Run `idf.py reconfigure` after an EEZ Studio export.")
endif()

set(UI_INCLUDE "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ui")
    set(UI_INCLUDE "ui")
endif()

idf_component_register(
    SRCS
        main.c
        mesh_ui.c
        actions.c
        ${UI_GENERATED}
        ${UI_FONTS}
        ${UI_IMAGES}
    INCLUDE_DIRS
        .
        ${UI_INCLUDE}
    REQUIRES
        bsp_i2c bsp_display bsp_illuminate
        lvgl__lvgl
        espressif__esp_lvgl_port
        meshtastic_node
        nvs_flash esp_timer esp_hw_support
)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
if(LVGL_LIB)
    target_compile_options(${LVGL_LIB} PRIVATE -DLV_LVGL_H_INCLUDE_SIMPLE)
endif()
