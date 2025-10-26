#include <lvgl.h>
#include "esp_log.h"
#include "hebrew_tabs.h"
#include "hebrew_fonts.h"
#include "settings_modal.h"
#include "ui_helpers.h"
#include "theme_manager.h"

static const char* TAG = "TABVIEW";

// Global tabview references
static lv_obj_t *global_tabview = NULL;
static lv_obj_t *global_tabs[5] = {NULL}; // Store tab references

// Function to toggle between dark and light mode
static void toggle_theme_internal(void) {
    theme_manager_toggle_mode();
}

// Settings button event callback
static void settings_btn_event_cb(lv_event_t *e) {
    // Stop event propagation to prevent affecting tab selection
    lv_event_stop_processing(e);

    lv_obj_t *screen = lv_screen_active();
    create_settings_modal(screen, toggle_theme_internal);

    ESP_LOGI(TAG, "Settings modal opened");
}

lv_obj_t* create_hebrew_tabview(lv_obj_t *parent) {
    ESP_LOGW(TAG, "FUNCTION CALLED: create_hebrew_tabview");
    ESP_LOGI(TAG, "Creating Hebrew tabview...");

    // Create tabview
    lv_obj_t *tabview = lv_tabview_create(parent);
    ESP_LOGW(TAG, "Tabview created successfully");
    
    // Store tabview globally for theme switching
    global_tabview = tabview;

    // Initialize theme manager with this display
    lv_display_t *disp = lv_obj_get_display(tabview);
    theme_manager_init(disp);
    ESP_LOGI(TAG, "Theme manager initialized");

    // Enable elastic scroll only (momentum disabled for better control)
    lv_obj_remove_flag(tabview, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_flag(tabview, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Add tabs with Hebrew names

    // Main App Page Tab
    lv_obj_t *main_app_page_tab = lv_tabview_add_tab(tabview, "דף ראשי");
    global_tabs[0] = main_app_page_tab;

    // News Tab
    lv_obj_t *news_tab = lv_tabview_add_tab(tabview, "כרטיסיות נפתחות");
    global_tabs[1] = news_tab;

    // Niqqud Tab
    lv_obj_t *niqqud_tab = lv_tabview_add_tab(tabview, "טקסט מנוקד");
    global_tabs[2] = niqqud_tab;

    // Random Tab
    lv_obj_t *pull_refresh_tab = lv_tabview_add_tab(tabview, "משיכה לרענון");
    global_tabs[3] = pull_refresh_tab;

    // Gallery Tab
    lv_obj_t *gallery_tab = lv_tabview_add_tab(tabview, "גלריה");
    global_tabs[4] = gallery_tab;

    // Apply Hebrew font and styling to individual tab buttons (LVGL 9 direct styling)
    lv_obj_t * tab_buttons = lv_tabview_get_tab_bar(tabview);
    lv_obj_add_flag(tab_buttons, LV_OBJ_FLAG_SCROLLABLE);

    // Set RTL direction for Hebrew tab layout
    lv_obj_set_style_base_dir(tab_buttons, LV_BASE_DIR_RTL, 0);

    // Set flex flow to allow dynamic widths
    lv_obj_set_flex_flow(tab_buttons, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(tab_buttons, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    uint32_t tab_count = lv_tabview_get_tab_count(tabview);
    for(uint32_t i = 0; i < tab_count; i++) {
        lv_obj_t * button = lv_obj_get_child(tab_buttons, i);

        // Apply styling directly (more efficient than static style objects)
        lv_obj_set_style_text_font(button, &opensans_hebrew_16, LV_PART_MAIN);
        lv_obj_set_style_pad_top(button, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_bottom(button, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_left(button, 15, LV_PART_MAIN);
        lv_obj_set_style_pad_right(button, 15, LV_PART_MAIN);

        // Set width to content size and prevent flex grow
        lv_obj_set_width(button, LV_SIZE_CONTENT);
        lv_obj_set_flex_grow(button, 0);
    }

    // Add settings icon button positioned fixed in bottom-left corner (small, circular)
    lv_obj_t *settings_btn = lv_btn_create(parent); // Create on parent, not tab_buttons
    lv_obj_set_size(settings_btn, 45, 45);
    lv_obj_set_style_radius(settings_btn, 22, 0);
    lv_obj_align(settings_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10); // Fixed position bottom-left

    // Make sure it stays on top of everything
    lv_obj_move_to_index(settings_btn, -1);

    // Settings icon
    lv_obj_t *settings_icon = lv_label_create(settings_btn);
    lv_label_set_text(settings_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(settings_icon, &lv_font_montserrat_14, 0);
    lv_obj_center(settings_icon);

    // Add event callback
    lv_obj_add_event_cb(settings_btn, settings_btn_event_cb, LV_EVENT_CLICKED, NULL);

    // Disable horizontal swiping on tab content (keep only tab bar swipeable)
    lv_obj_t* tab_content = lv_tabview_get_content(tabview);
    if (tab_content) {
        // Disable horizontal scrolling on content area only
        lv_obj_set_scroll_dir(tab_content, LV_DIR_VER);
        ESP_LOGI(TAG, "Disabled horizontal swiping on tab content");
    }

    // Also disable horizontal scrolling on each individual tab
    for(uint32_t i = 0; i < tab_count; i++) {
        if (global_tabs[i]) {
            lv_obj_set_scroll_dir(global_tabs[i], LV_DIR_VER);
        }
    }

    // Add content to the tabs
    create_welcome_tab(main_app_page_tab);
    create_pull_refresh_tab(pull_refresh_tab);
    create_niqqud_demo_tab(niqqud_tab);
    create_news_tab(news_tab);
    create_gallery_tab(gallery_tab);

    ESP_LOGI(TAG, "Hebrew tabview created successfully");
    return tabview;
}
