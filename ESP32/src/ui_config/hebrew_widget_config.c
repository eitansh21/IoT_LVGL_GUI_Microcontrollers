/**
 * @file hebrew_widget_config.c
 * @brief Implementation of Hebrew/RTL-specific widget configurations
 */

#include "hebrew_widget_config.h"
#include "ui_helpers.h"

/**
 * Get Hebrew-optimized style configuration
 */
widget_style_t hebrew_get_widget_style(void) {
    widget_style_t style = widget_get_default_style();

    // Override for Hebrew/RTL
    style.base_dir = LV_BASE_DIR_RTL;
    style.title_font = &opensans_hebrew_16;
    style.content_font = &opensans_hebrew_16;
    style.button_font = &opensans_hebrew_12;

    return style;
}

/**
 * Get Hebrew text configuration
 */
widget_common_text_t hebrew_get_common_text(void) {
    widget_common_text_t text = {
        .loading_text = "טוען...",
        .error_text = "אירעה שגיאה",
        .empty_text = "אין פריטים",
        .truncate_suffix = "..."
    };
    return text;
}

/**
 * Get Hebrew-configured expandable card configuration
 */
lv_card_config_t hebrew_get_expandable_card_config(void) {
    lv_card_config_t config = lv_expandable_card_get_default_config();

    // Override with Hebrew-specific settings
    config.expand_text = "הרחב";
    config.collapse_text = "כווץ";
    config.style = hebrew_get_widget_style();
    config.title_style = ui_get_title_style();   // Use theme-aware title style
    config.button_style = ui_get_button_style(); // Use theme-aware button style

    // Hebrew-optimized truncation
    config.truncate_length = 80;  // Hebrew text might need more characters
    config.truncate_suffix = "...";

    return config;
}

/**
 * Get Hebrew-configured expandable card configuration with callbacks
 */
lv_card_config_t hebrew_get_expandable_card_config_with_callbacks(
    void (*on_expand)(void* user_data),
    void (*on_collapse)(void* user_data),
    void* user_data
) {
    lv_card_config_t config = hebrew_get_expandable_card_config();

    // Set callbacks
    config.on_expand = on_expand;
    config.on_collapse = on_collapse;
    config.user_data = user_data;

    return config;
}

/**
 * Get Hebrew-configured pull refresh configuration
 */
lv_pull_refresh_config_t hebrew_get_pull_refresh_config(
    lv_pull_refresh_cb_t refresh_cb,
    void* user_data
) {
    lv_pull_refresh_config_t config = lv_pull_refresh_get_default_config();

    // Override with Hebrew-specific settings
    config.pull_text = "משוך לרענון";
    config.release_text = "שחרר לרענון";
    config.refreshing_text = "מרענן...";
    config.style = hebrew_get_widget_style();

    // Set callbacks
    config.refresh_cb = refresh_cb;
    config.user_data = user_data;

    return config;
}

/**
 * Get Hebrew-configured image gallery configuration
 */
lv_gallery_config_t hebrew_get_image_gallery_config(void) {
    lv_gallery_config_t config = lv_image_gallery_get_default_config();

    // Override with Hebrew-specific settings (symbols removed - will be handled separately)
    config.prev_text = "הקודם";
    config.next_text = "הבא";
    config.style = hebrew_get_widget_style();
    config.title_style = ui_get_title_style();   // Use theme-aware title style
    config.button_style = ui_get_button_style(); // Use theme-aware button style

    return config;
}