/**
 * @file widget_common.c
 * @brief Implementation of theme-aware widget styling and configuration
 */

#include "widget_common.h"

/**
 * Get default style configuration that respects LVGL themes
 */
widget_style_t widget_get_default_style(void) {
    widget_style_t style = {
        // Text and layout - LTR defaults, but apps can override
        .base_dir = LV_BASE_DIR_LTR,
        .title_font = NULL,                 // NULL = use theme font
        .content_font = NULL,               // NULL = use theme font
        .button_font = NULL,                // NULL = use theme font

        // Spacing and dimensions - reasonable defaults
        .padding = 15,
        .margin = 8,
        .border_radius = 8,
        .border_width = 1,

        // Performance settings - elastic enabled, momentum disabled for better control
        .enable_elastic_scroll = true,
        .enable_momentum_scroll = false
    };
    return style;
}

/**
 * Get default English common text configuration
 */
widget_common_text_t widget_get_default_common_text_en(void) {
    widget_common_text_t text = {
        .loading_text = "Loading...",
        .error_text = "Error occurred",
        .empty_text = "No items",
        .truncate_suffix = "..."
    };
    return text;
}

/**
 * Get theme-aware color using proper LVGL theme API
 */
lv_color_t widget_get_theme_color(lv_obj_t* obj, widget_color_role_t color_role) {
    if (!obj) {
        // Fallback to a neutral color if no object provided
        return lv_color_white();  // Simple fallback, theme will override when available
    }

    // Use proper LVGL theme API
    switch (color_role) {
        case WIDGET_COLOR_PRIMARY:
            return lv_theme_get_color_primary(obj);
        case WIDGET_COLOR_SECONDARY:
            return lv_theme_get_color_secondary(obj);
        default:
            return lv_theme_get_color_primary(obj);
    }
}

/**
 * Get theme-aware font respecting current theme
 */
const lv_font_t* widget_get_theme_font(const widget_style_t* style, const lv_font_t* font_override, widget_font_size_t font_size) {
    // If specific font override is provided, use it
    if (font_override) {
        return font_override;
    }

    // If no style provided, return default LVGL font
    if (!style) {
        return LV_FONT_DEFAULT;
    }

    // Use configured fonts from style, fallback to LVGL default font
    switch (font_size) {
        case WIDGET_FONT_SIZE_SMALL:
            return style->button_font ? style->button_font : LV_FONT_DEFAULT;
        case WIDGET_FONT_SIZE_NORMAL:
            return style->content_font ? style->content_font : LV_FONT_DEFAULT;
        case WIDGET_FONT_SIZE_LARGE:
            return style->title_font ? style->title_font : LV_FONT_DEFAULT;
        default:
            return style->content_font ? style->content_font : LV_FONT_DEFAULT;
    }
}