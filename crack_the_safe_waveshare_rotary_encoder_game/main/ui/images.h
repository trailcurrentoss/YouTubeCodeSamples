#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_prize_1;
extern const lv_img_dsc_t img_prize_2;
extern const lv_img_dsc_t img_prize_3;
extern const lv_img_dsc_t img_prize_4;
extern const lv_img_dsc_t img_prize_5;
extern const lv_img_dsc_t img_prize_6;
extern const lv_img_dsc_t img_prize_7;
extern const lv_img_dsc_t img_prize_8;
extern const lv_img_dsc_t img_gold_glow;
extern const lv_img_dsc_t img_police_red;
extern const lv_img_dsc_t img_police_blue;
extern const lv_img_dsc_t img_door_00;
extern const lv_img_dsc_t img_door_01;
extern const lv_img_dsc_t img_door_02;
extern const lv_img_dsc_t img_door_03;
extern const lv_img_dsc_t img_door_04;
extern const lv_img_dsc_t img_door_05;
extern const lv_img_dsc_t img_door_06;
extern const lv_img_dsc_t img_door_07;
extern const lv_img_dsc_t img_door_08;
extern const lv_img_dsc_t img_door_09;
extern const lv_img_dsc_t img_door_10;
extern const lv_img_dsc_t img_door_11;
extern const lv_img_dsc_t img_door_12;
extern const lv_img_dsc_t img_door_13;
extern const lv_img_dsc_t img_door_14;
extern const lv_img_dsc_t img_door_15;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[27];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/