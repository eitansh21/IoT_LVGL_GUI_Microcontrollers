#include "ui_helpers.h"
#include "hebrew_fonts.h"

/**
 * Create a standard Hebrew tab container with common styling
 */
lv_obj_t* ui_create_tab_container(lv_obj_t *parent, int padding) {
    // Create main scrollable container
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));

    // Apply standard Hebrew container styling
    lv_obj_set_style_base_dir(container, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_pad_all(container, padding, 0);
    lv_obj_set_style_pad_row(container, padding, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);

    // Enable elastic scroll only (momentum disabled for better control)
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Setup flex layout
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);

    return container;
}

// Global styles that get updated on theme changes
static lv_style_t title_style;
static lv_style_t button_style;
static lv_style_t switch_style;
static bool title_style_initialized = false;
static bool button_style_initialized = false;
static bool switch_style_initialized = false;

/**
 * Update title style with current theme colors
 */
void ui_update_title_style(lv_obj_t *ref_obj) {
    if (title_style_initialized) {
        lv_style_set_text_color(&title_style, lv_theme_get_color_primary(ref_obj));
    }
}

/**
 * Update button style with current theme colors
 */
void ui_update_button_style(lv_obj_t *ref_obj, bool is_dark_mode) {
    if (button_style_initialized) {
        // Set button background to primary color
        lv_style_set_bg_color(&button_style, lv_theme_get_color_primary(ref_obj));

        // Set text color for contrast: black in dark mode, white in light mode
        if (is_dark_mode) {
            lv_style_set_text_color(&button_style, lv_color_black());
        } else {
            lv_style_set_text_color(&button_style, lv_color_white());
        }
    }
}

/**
 * Update switch style with current theme colors
 */
void ui_update_switch_style(lv_obj_t *ref_obj) {
    if (switch_style_initialized) {
        lv_style_set_bg_color(&switch_style, lv_theme_get_color_primary(ref_obj));
    }
}

/**
 * Get the global title style for use in widgets
 */
lv_style_t* ui_get_title_style(void) {
    return title_style_initialized ? &title_style : NULL;
}

/**
 * Get the global button style for use in widgets
 */
lv_style_t* ui_get_button_style(void) {
    return button_style_initialized ? &button_style : NULL;
}

/**
 * Get the global switch style for use in widgets
 */
lv_style_t* ui_get_switch_style(void) {
    return switch_style_initialized ? &switch_style : NULL;
}


/**
 * Create a standard Hebrew title label with theme-aware colors
 */
lv_obj_t* ui_create_title_label(lv_obj_t *parent, const char *text) {
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, text);

    // Set RTL direction and alignment
    lv_obj_set_style_base_dir(title, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_RIGHT, 0);

    // Initialize styles with current theme colors
    if (!title_style_initialized) {
        lv_style_init(&title_style);
        lv_style_set_text_color(&title_style, lv_theme_get_color_primary(title));
        title_style_initialized = true;
    }

    if (!button_style_initialized) {
        lv_style_init(&button_style);
        lv_style_set_bg_color(&button_style, lv_theme_get_color_primary(title));
        lv_style_set_text_color(&button_style, lv_color_white()); // Default to white for light mode
        button_style_initialized = true;
    }

    if (!switch_style_initialized) {
        lv_style_init(&switch_style);
        lv_style_set_bg_color(&switch_style, lv_theme_get_color_primary(title));
        switch_style_initialized = true;
    }

    // Apply the theme-aware title style
    lv_obj_add_style(title, &title_style, 0);

    return title;
}

/**
 * Apply common Hebrew text styling to a label
 */
void ui_apply_hebrew_text_style(lv_obj_t *label, const lv_font_t *font, bool rtl_mode) {
    // Set font
    lv_obj_set_style_text_font(label, font, 0);

    // Set text direction and alignment
    if (rtl_mode) {
        lv_obj_set_style_base_dir(label, LV_BASE_DIR_RTL, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    } else {
        lv_obj_set_style_base_dir(label, LV_BASE_DIR_LTR, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    }
}