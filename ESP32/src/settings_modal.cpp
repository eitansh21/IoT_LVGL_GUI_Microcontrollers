#include <lvgl.h>
#include "esp_log.h"
#include "hebrew_fonts.h"
#include "theme_manager.h"
#include "ui_helpers.h"
#include <Arduino.h>

// External functions from main.cpp
extern void toggle_fps_display();
extern bool is_fps_display_enabled();

static const char* TAG = "SETTINGS_MODAL";

// Backlight configuration
#define TFT_BL_PIN 32        // Backlight pin - change to your actual pin
#define PWM_CHANNEL 0        // PWM channel
#define PWM_FREQ 5000        // PWM frequency
#define PWM_RESOLUTION 8     // 8-bit resolution (0-255)

// Callback type for theme toggle
typedef void (*theme_toggle_callback_t)(void);
static theme_toggle_callback_t g_theme_toggle_cb = NULL;

// Settings state
static int brightness_level = 100;

// Event handler for dark mode switch
static void dark_mode_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = (lv_obj_t*)lv_event_get_target(e);

    if (g_theme_toggle_cb) {
        g_theme_toggle_cb();

        // Defer the switch style update to after this event is processed
        lv_obj_invalidate(sw);
    }

    ESP_LOGI(TAG, "Dark mode toggled");
}

// Event handler for FPS display switch
static void fps_display_switch_event_cb(lv_event_t *e) {
    toggle_fps_display();
    ESP_LOGI(TAG, "FPS display toggled");
}

// Event handler for brightness slider
static void brightness_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t*)lv_event_get_user_data(e);
    
    brightness_level = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d%%", brightness_level);
    
    // Actually set hardware backlight brightness via PWM
    // Map 0-100 to 0-255 for PWM
    uint8_t pwm_value = (brightness_level * 255) / 100;
    
    // Set PWM duty cycle
    ledcWrite(PWM_CHANNEL, pwm_value);
    
    ESP_LOGI(TAG, "Brightness: %d%% (PWM: %d)", brightness_level, pwm_value);
}

// Event handler for close button
static void close_btn_event_cb(lv_event_t *e) {
    lv_obj_t *modal = (lv_obj_t*)lv_event_get_user_data(e);
    lv_obj_delete(modal);
    ESP_LOGI(TAG, "Settings modal closed");
}

// Helper function to create a setting row with switch (RTL layout)
static lv_obj_t* create_setting_row_with_switch(lv_obj_t *parent, const char *title, 
                                                const char *desc, bool initial_state,
                                                lv_event_cb_t event_cb) {
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_base_dir(row, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_pad_all(row, 15, 0);
    lv_obj_set_style_pad_column(row, 10, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_radius(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    // Text container (appears first in RTL row, so on the right side visually)
    lv_obj_t *text_container = lv_obj_create(row);
    lv_obj_set_size(text_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(text_container, 0, 0);
    lv_obj_set_style_bg_opa(text_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(text_container, 0, 0);
    lv_obj_set_flex_flow(text_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(text_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
    lv_obj_set_style_base_dir(text_container, LV_BASE_DIR_RTL, 0);

    // Title
    lv_obj_t *title_label = lv_label_create(text_container);
    lv_label_set_text(title_label, title);
    lv_obj_set_style_text_font(title_label, &opensans_hebrew_16, 0);

    // Description
    lv_obj_t *desc_label = lv_label_create(text_container);
    lv_label_set_text(desc_label, desc);
    lv_obj_set_style_text_font(desc_label, &opensans_hebrew_16, 0);
    lv_obj_set_style_text_opa(desc_label, LV_OPA_60, 0);

    // Switch (appears second in RTL row, so on the left side visually)
    lv_obj_t *sw = lv_switch_create(row);
    lv_obj_set_size(sw, 50, 25);
    if (initial_state) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(sw, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Apply global switch style if available
    lv_style_t* switch_style = ui_get_switch_style();
    if (switch_style) {
        lv_obj_add_style(sw, switch_style, LV_PART_INDICATOR);
    }

    return row;
}

void create_settings_modal(lv_obj_t *parent, theme_toggle_callback_t theme_cb) {
    g_theme_toggle_cb = theme_cb;
    
    // Initialize backlight PWM if not already done
    static bool pwm_initialized = false;
    if (!pwm_initialized) {
        ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
        ledcAttachPin(TFT_BL_PIN, PWM_CHANNEL);
        // Set initial brightness
        uint8_t initial_pwm = (brightness_level * 255) / 100;
        ledcWrite(PWM_CHANNEL, initial_pwm);
        pwm_initialized = true;
        ESP_LOGI(TAG, "Backlight PWM initialized on pin %d", TFT_BL_PIN);
    }

    // Create modal background overlay
    lv_obj_t *overlay = lv_obj_create(parent);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_50, 0);
    lv_obj_set_style_border_width(overlay, 0, 0);
    lv_obj_set_style_radius(overlay, 0, 0);
    lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);

    // Create modal container
    lv_obj_t *modal = lv_obj_create(overlay);
    lv_obj_set_size(modal, LV_PCT(90), LV_PCT(80));
    lv_obj_center(modal);
    lv_obj_set_style_base_dir(modal, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_radius(modal, 12, 0);
    lv_obj_set_style_pad_all(modal, 0, 0);
    lv_obj_set_flex_flow(modal, LV_FLEX_FLOW_COLUMN);

    // Header
    lv_obj_t *header = lv_obj_create(modal);
    lv_obj_set_size(header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_base_dir(header, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_pad_all(header, 20, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    // Header title (appears first in RTL row, so on the right side visually)
    lv_obj_t *header_title = lv_label_create(header);
    lv_label_set_text(header_title, "הגדרות");
    lv_obj_set_style_text_font(header_title, &opensans_hebrew_16, 0);

    // Close button (appears second in RTL row, so on the left side visually)
    lv_obj_t *close_btn = lv_btn_create(header);
    lv_obj_set_size(close_btn, 40, 40);
    lv_obj_set_style_radius(close_btn, 20, 0);
    
    lv_obj_t *close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_14, 0);
    lv_obj_center(close_label);
    lv_obj_add_event_cb(close_btn, close_btn_event_cb, LV_EVENT_CLICKED, overlay);

    // Content container (scrollable)
    lv_obj_t *content = lv_obj_create(modal);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(content, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_pad_all(content, 0, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_radius(content, 0, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(content, 1);

    // Enable elastic scroll only (momentum disabled for better control)
    lv_obj_remove_flag(content, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_flag(content, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Section: תצוגה (Display)
    lv_obj_t *display_section = lv_label_create(content);
    lv_label_set_text(display_section, "תצוגה");
    lv_obj_set_style_text_font(display_section, &opensans_hebrew_16, 0);
    lv_obj_set_style_pad_all(display_section, 15, 0);
    lv_obj_set_style_pad_top(display_section, 10, 0);
    lv_obj_set_style_text_align(display_section, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_width(display_section, LV_PCT(100));

    // Dark mode setting - use centralized theme manager state
    create_setting_row_with_switch(content, "מצב לילה", "הפעל ערכת נושא כהה",
                                   theme_manager_is_dark_mode(),
                                   dark_mode_switch_event_cb);

    // FPS display setting
    create_setting_row_with_switch(content, "תצוגת FPS", "הצג מידע ביצועים ו-LVGL",
                                   is_fps_display_enabled(),
                                   fps_display_switch_event_cb);

    // Brightness slider
    lv_obj_t *brightness_row = lv_obj_create(content);
    lv_obj_set_size(brightness_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_base_dir(brightness_row, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_pad_all(brightness_row, 15, 0);
    lv_obj_set_style_border_width(brightness_row, 0, 0);
    lv_obj_set_style_radius(brightness_row, 0, 0);
    lv_obj_set_flex_flow(brightness_row, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(brightness_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);

    lv_obj_t *brightness_title = lv_label_create(brightness_row);
    lv_label_set_text(brightness_title, "בהירות מסך");
    lv_obj_set_style_text_font(brightness_title, &opensans_hebrew_16, 0);

    lv_obj_t *brightness_value = lv_label_create(brightness_row);
    lv_label_set_text_fmt(brightness_value, "%d%%", brightness_level);
    lv_obj_set_style_text_opa(brightness_value, LV_OPA_60, 0);

    lv_obj_t *slider = lv_slider_create(brightness_row);
    lv_obj_set_width(slider, LV_PCT(100));
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, brightness_level, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, brightness_value);

    ESP_LOGI(TAG, "Settings modal created");
}
