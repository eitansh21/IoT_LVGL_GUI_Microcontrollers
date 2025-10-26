#ifndef HEBREW_TABS_H
#define HEBREW_TABS_H

#include <lvgl.h>

void create_welcome_tab(lv_obj_t *tab);
void create_pull_refresh_tab(lv_obj_t *tab);
void create_niqqud_demo_tab(lv_obj_t *tab);
void create_news_tab(lv_obj_t *tab);
void create_gallery_tab(lv_obj_t *tab);

// Tabview creation function
lv_obj_t* create_hebrew_tabview(lv_obj_t *parent);

#endif // HEBREW_TABS_H