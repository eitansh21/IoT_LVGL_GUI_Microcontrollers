/**
 * @file hebrew_widget_config.h
 * @brief Hebrew/RTL-specific configurations for widgets
 *
 * This file provides pre-configured widget settings optimized for Hebrew text,
 * RTL layout, and the specific fonts used in this Hebrew LVGL project.
 *
 * Applications can use these configurations as starting points and modify
 * them as needed.
 *
 * @author ESP32 LVGL Hebrew Project
 * @date 2025
 */

#ifndef HEBREW_WIDGET_CONFIG_H
#define HEBREW_WIDGET_CONFIG_H

#include "widget_common.h"
#include "lv_expandable_card.h"
#include "lv_pull_refresh.h"
#include "lv_image_gallery.h"
#include "hebrew_fonts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get Hebrew-optimized style configuration
 *
 * Returns a widget style configuration optimized for Hebrew text and RTL layout.
 * Uses the Hebrew fonts available in this project.
 *
 * @return Hebrew-optimized widget style configuration
 */
widget_style_t hebrew_get_widget_style(void);

/**
 * @brief Get Hebrew text configuration
 *
 * Returns Hebrew text strings for common widget elements.
 *
 * @return Hebrew text configuration
 */
widget_common_text_t hebrew_get_common_text(void);

/**
 * @brief Get Hebrew-configured expandable card configuration
 *
 * Returns an expandable card configuration optimized for Hebrew articles/content.
 * Includes Hebrew button text and RTL layout.
 *
 * @return Hebrew-optimized expandable card configuration
 */
lv_card_config_t hebrew_get_expandable_card_config(void);

/**
 * @brief Get Hebrew-configured expandable card configuration with callbacks
 *
 * Same as hebrew_get_expandable_card_config() but allows setting custom callbacks.
 *
 * @param on_expand Callback when card expands (can be NULL)
 * @param on_collapse Callback when card collapses (can be NULL)
 * @param user_data User data passed to callbacks (can be NULL)
 * @return Hebrew-optimized expandable card configuration with callbacks
 */
lv_card_config_t hebrew_get_expandable_card_config_with_callbacks(
    void (*on_expand)(void* user_data),
    void (*on_collapse)(void* user_data),
    void* user_data
);

/**
 * @brief Get Hebrew-configured pull refresh configuration
 *
 * Returns a pull refresh configuration optimized for Hebrew UI.
 * Includes Hebrew text and RTL layout.
 *
 * @param refresh_cb Callback for refresh events (required)
 * @param user_data User data passed to callback (can be NULL)
 * @return Hebrew-optimized pull refresh configuration
 */
lv_pull_refresh_config_t hebrew_get_pull_refresh_config(
    lv_pull_refresh_cb_t refresh_cb,
    void* user_data
);

/**
 * @brief Get Hebrew-configured image gallery configuration
 *
 * Returns an image gallery configuration optimized for Hebrew UI.
 * Includes Hebrew navigation button text and RTL layout.
 *
 * @return Hebrew-optimized image gallery configuration
 */
lv_gallery_config_t hebrew_get_image_gallery_config(void);

#ifdef __cplusplus
}
#endif

#endif // HEBREW_WIDGET_CONFIG_H