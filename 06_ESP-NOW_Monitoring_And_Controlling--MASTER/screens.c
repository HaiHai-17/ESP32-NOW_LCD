#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffdedede), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 40);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff006468), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffbebebf), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_header
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_header = obj;
            lv_obj_set_pos(obj, 61, 4);
            lv_obj_set_size(obj, 199, 32);
            lv_label_set_text(obj, "ESP32 ESP-NOW \nNHIET DO  DO AM - RELAY");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_monitoring
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_monitoring = obj;
            lv_obj_set_pos(obj, 8, 45);
            lv_obj_set_size(obj, 304, 129);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_pad_top(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, -2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff006468), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xffb7b7b7), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_controlling
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_controlling = obj;
            lv_obj_set_pos(obj, 8, 180);
            lv_obj_set_size(obj, 304, 56);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff006468), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 95, -10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "DIEU KHIEN");
                }
            }
        }
        {
            // panel_slave_info_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.panel_slave_info_1 = obj;
            lv_obj_set_pos(obj, 112, 48);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "TRANG THAI");
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_slave_info
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_slave_info = obj;
            lv_obj_set_pos(obj, 16, 64);
            lv_obj_set_size(obj, 288, 23);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_slave_info
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_slave_info = obj;
                    lv_obj_set_pos(obj, -9, -11);
                    lv_obj_set_size(obj, 277, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Trang thai Salve : NN");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // panel_mntr_temperature
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_mntr_temperature = obj;
            lv_obj_set_pos(obj, 16, 95);
            lv_obj_set_size(obj, 140, 44);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_temperature
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_temperature = obj;
                    lv_obj_set_pos(obj, -9, 5);
                    lv_obj_set_size(obj, 129, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "NN.NN °C");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0603e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffe0603e), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_CHECKED);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 23, -13);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Nhiet Do");
                }
            }
        }
        {
            // panel_mntr_humidity
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_mntr_humidity = obj;
            lv_obj_set_pos(obj, 165, 95);
            lv_obj_set_size(obj, 140, 44);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 31, -13);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Do Am");
                }
            }
        }
        {
            // panel_mntr_lb_a
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_mntr_lb_a = obj;
            lv_obj_set_pos(obj, 16, 145);
            lv_obj_set_size(obj, 140, 23);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mntr_lb_a
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mntr_lb_a = obj;
                    lv_obj_set_pos(obj, 19, -11);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Relay 1 : NN");
                }
            }
        }
        {
            // panel_mntr_lb_b
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_mntr_lb_b = obj;
            lv_obj_set_pos(obj, 166, 145);
            lv_obj_set_size(obj, 140, 23);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mntr_lb_b
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mntr_lb_b = obj;
                    lv_obj_set_pos(obj, 14, -11);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Relay 2 : NN");
                }
            }
        }
        {
            // panel_ctr_light_bulb_a
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_ctr_light_bulb_a = obj;
            lv_obj_set_pos(obj, 17, 205);
            lv_obj_set_size(obj, 140, 25);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mntr_lb_a_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mntr_lb_a_1 = obj;
                    lv_obj_set_pos(obj, 6, -10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Relay 1");
                }
                {
                    // switch_ctr_light_bulb_a
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.switch_ctr_light_bulb_a = obj;
                    lv_obj_set_pos(obj, 64, -10);
                    lv_obj_set_size(obj, 33, 15);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbdbdbd), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffdd00), LV_PART_INDICATOR | LV_STATE_CHECKED);
                }
            }
        }
        {
            // panel_ctr_light_bulb_b
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_ctr_light_bulb_b = obj;
            lv_obj_set_pos(obj, 165, 205);
            lv_obj_set_size(obj, 140, 25);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_mntr_lb_a_2
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_mntr_lb_a_2 = obj;
                    lv_obj_set_pos(obj, 4, -10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Relay 2");
                }
                {
                    // switch_ctr_light_bulb_b
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.switch_ctr_light_bulb_b = obj;
                    lv_obj_set_pos(obj, 65, -10);
                    lv_obj_set_size(obj, 33, 15);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffdd00), LV_PART_INDICATOR | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffbdbdbd), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // label_humidity
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_humidity = obj;
            lv_obj_set_pos(obj, 172, 117);
            lv_obj_set_size(obj, 129, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "NNN %");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff3598db), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
