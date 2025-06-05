#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *label_header;
    lv_obj_t *panel_monitoring;
    lv_obj_t *panel_controlling;
    lv_obj_t *panel_slave_info_1;
    lv_obj_t *panel_slave_info;
    lv_obj_t *label_slave_info;
    lv_obj_t *panel_mntr_temperature;
    lv_obj_t *label_temperature;
    lv_obj_t *panel_mntr_humidity;
    lv_obj_t *panel_mntr_lb_a;
    lv_obj_t *label_mntr_lb_a;
    lv_obj_t *panel_mntr_lb_b;
    lv_obj_t *label_mntr_lb_b;
    lv_obj_t *panel_ctr_light_bulb_a;
    lv_obj_t *label_mntr_lb_a_1;
    lv_obj_t *switch_ctr_light_bulb_a;
    lv_obj_t *panel_ctr_light_bulb_b;
    lv_obj_t *label_mntr_lb_a_2;
    lv_obj_t *switch_ctr_light_bulb_b;
    lv_obj_t *label_humidity;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/