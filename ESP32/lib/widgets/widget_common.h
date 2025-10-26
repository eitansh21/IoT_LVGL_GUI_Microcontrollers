/**
 * @file widget_common.h
 * @brief Common styling and configuration shared across all custom widgets
 *
 * This file provides shared styling configurations that can be used by all widgets,
 * while keeping widget-specific configurations separate.
 *
 * @author ESP32 LVGL Project
 * @date 2025
 */

#ifndef WIDGET_COMMON_H
#define WIDGET_COMMON_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Common style configuration shared across widgets
 *
 * Simplified to focus on essential layout and font configuration.
 * All theme colors are now handled automatically via widget_get_theme_color().
 */
typedef struct {
    // Text and layout
    lv_base_dir_t base_dir;              /**< LV_BASE_DIR_RTL or LV_BASE_DIR_LTR */
    const lv_font_t* title_font;         /**< Override font for titles (NULL = use theme) */
    const lv_font_t* content_font;       /**< Override font for content (NULL = use theme) */
    const lv_font_t* button_font;        /**< Override font for buttons (NULL = use theme) */

    // Layout spacing
    int32_t padding;                     /**< General padding in pixels */
    int32_t margin;                      /**< General margin in pixels */
    int32_t border_radius;               /**< Border radius in pixels */
    int32_t border_width;                /**< Border width in pixels */

    // Performance settings
    bool enable_elastic_scroll;          /**< Enable elastic scrolling */
    bool enable_momentum_scroll;         /**< Enable momentum scrolling */
} widget_style_t;

/**
 * @brief Common text strings that might be used across widgets
 */
typedef struct {
    const char* loading_text;            /**< Text for loading state */
    const char* error_text;              /**< Text for error state */
    const char* empty_text;              /**< Text for empty state */
    const char* truncate_suffix;         /**< Suffix for truncated text */
} widget_common_text_t;

/**
 * @brief Get default style configuration
 *
 * Returns sensible defaults for LTR languages with standard LVGL fonts.
 * Applications can modify these defaults as needed.
 *
 * @return Default widget style configuration
 */
widget_style_t widget_get_default_style(void);

/**
 * @brief Get default common text configuration for English
 *
 * Returns default English text strings for common widget text elements.
 *
 * @return Default English common text configuration
 */
widget_common_text_t widget_get_default_common_text_en(void);

/**
 * @brief Theme color roles (since LVGL doesn't define these)
 */
typedef enum {
    WIDGET_COLOR_PRIMARY,
    WIDGET_COLOR_SECONDARY
} widget_color_role_t;

/**
 * @brief Font size roles (since LVGL doesn't define these)
 */
typedef enum {
    WIDGET_FONT_SIZE_SMALL,
    WIDGET_FONT_SIZE_NORMAL,
    WIDGET_FONT_SIZE_LARGE
} widget_font_size_t;

/**
 * @brief Get theme-aware color using proper LVGL theme API
 *
 * @param obj LVGL object to get theme colors for (required)
 * @param color_role The type of color needed (primary, secondary, etc.)
 * @return lv_color_t color value from current theme
 */
lv_color_t widget_get_theme_color(lv_obj_t* obj, widget_color_role_t color_role);

/**
 * @brief Get theme-aware font for widget styling
 *
 * @param style Widget style configuration
 * @param font_override Override font (NULL to use theme font)
 * @param font_size Theme font size to use if no override
 * @return const lv_font_t* font from theme or override
 */
const lv_font_t* widget_get_theme_font(const widget_style_t* style, const lv_font_t* font_override, widget_font_size_t font_size);

#ifdef __cplusplus
}
#endif

#endif // WIDGET_COMMON_H
