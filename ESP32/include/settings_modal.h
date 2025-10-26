#ifndef SETTINGS_MODAL_H
#define SETTINGS_MODAL_H

#include <lvgl.h>

typedef void (*theme_toggle_callback_t)(void);

void create_settings_modal(lv_obj_t *parent, theme_toggle_callback_t theme_cb);

#endif // SETTINGS_MODAL_H
