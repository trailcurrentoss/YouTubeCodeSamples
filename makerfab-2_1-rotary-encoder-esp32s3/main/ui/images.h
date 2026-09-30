#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_logo_freecad;
extern const lv_img_dsc_t img_logo_blender;
extern const lv_img_dsc_t img_logo_kdenlive;
extern const lv_img_dsc_t img_logo_vscodium;
extern const lv_img_dsc_t img_logo_gimp;
extern const lv_img_dsc_t img_logo_inkscape;
extern const lv_img_dsc_t img_logo_libreoffice;
extern const lv_img_dsc_t img_logo_firefox;
extern const lv_img_dsc_t img_logo_chromium;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[9];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/