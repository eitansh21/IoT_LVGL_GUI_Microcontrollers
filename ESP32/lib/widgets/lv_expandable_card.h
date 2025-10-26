/**
 * @file lv_expandable_card.h
 * @brief A reusable LVGL widget for displaying expandable article/content cards
 *
 * This widget creates a card with a title, collapsible content, and an expand/collapse button.
 * It's theme-aware and language-agnostic, supporting any text direction and font.
 *
 * Features:
 * - Expandable content with smooth transitions
 * - Theme-aware styling (respects current LVGL theme)
 * - Language agnostic (configurable text direction and fonts)
 * - Memory-efficient with proper cleanup
 * - Single-threaded design following LVGL patterns
 *
 * @author ESP32 LVGL Project
 * @date 2025
 */

#ifndef LV_EXPANDABLE_CARD_H
#define LV_EXPANDABLE_CARD_H

#include <lvgl.h>
#include "widget_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold card content data
 *
 * Contains the actual content to be displayed in the card.
 * All strings are stored by reference - caller must ensure persistence.
 */
typedef struct {
    const char* title;           /**< Card title text */
    const char* content;         /**< Full content text (auto-truncated when collapsed) */
} lv_card_data_t;

/**
 * @brief Expandable card configuration structure
 *
 * Contains all configuration options for the expandable card widget.
 * Uses theme-aware defaults that work with any language or theme.
 */
typedef struct {
    // Text configuration
    const char* expand_text;         /**< Text for expand button */
    const char* collapse_text;       /**< Text for collapse button */
    int truncate_length;             /**< Character limit for collapsed text */
    const char* truncate_suffix;     /**< Suffix for truncated text */

    // Layout configuration
    int max_content_height;          /**< Max height for expanded content (0 = no limit) */
    int button_width;                /**< Width of expand/collapse button */
    int button_height;               /**< Height of expand/collapse button */

    // Styling (theme-aware)
    widget_style_t style;            /**< Common styling configuration */
    lv_style_t* title_style;         /**< Custom title style (optional, NULL for default) */
    lv_style_t* button_style;        /**< Custom button style (optional, NULL for default) */

    // Callbacks
    void (*on_expand)(void* user_data);  /**< Called when card expands */
    void (*on_collapse)(void* user_data); /**< Called when card collapses */
    void* user_data;                 /**< User data passed to callbacks */
} lv_card_config_t;

/**
 * @brief Create an expandable article card widget
 *
 * Creates a complete expandable card with title, collapsible content,
 * and an expand/collapse button with customizable appearance.
 *
 * @param parent Parent LVGL object (typically a container or tab)
 * @param card_data Structure containing title and content data
 * @param config Optional configuration (pass NULL for defaults)
 *
 * @return Pointer to the created card container object, or NULL on failure
 *
 * @note The card_data structure is stored by REFERENCE - caller must ensure
 *       it remains valid for the lifetime of the card widget.
 *       Content is automatically truncated based on truncate_length setting.
 *
 * @warning This widget is NOT thread-safe. All operations must be performed
 *          on the main thread where lv_timer_handler() runs. This follows
 *          LVGL's single-threaded architecture requirements.
 *          See: https://docs.lvgl.io/master/details/integration/overview/threading.html
 *
 * Example usage:
 * @code
 * // Create card data (must be static/global for widget lifetime)
 * static lv_card_data_t my_article = {
 *     .title = "Article Title",
 *     .content = "Full article content that will be automatically truncated when collapsed..."
 * };
 *
 * // Create card with defaults
 * lv_obj_t* card = lv_expandable_card_create(
 *     parent_container,
 *     &my_article,
 *     NULL  // Use default config
 * );
 *
 * // Or with custom config
 * lv_card_config_t config = lv_expandable_card_get_default_config();
 * config.style.base_dir = LV_BASE_DIR_RTL;  // Use RTL for Hebrew/Arabic
 * config.truncate_length = 80;              // Show only 80 chars when collapsed
 * config.expand_text = "Show More";         // Custom text
 * config.collapse_text = "Show Less";
 *
 * lv_obj_t* card2 = lv_expandable_card_create(
 *     parent_container, &my_article, &config
 * );
 * @endcode
 */
lv_obj_t* lv_expandable_card_create(
    lv_obj_t* parent,
    const lv_card_data_t* card_data,
    const lv_card_config_t* config
);

/**
 * @brief Get the default card configuration
 *
 * Returns a configuration structure with sensible defaults that work
 * with any language and theme.
 *
 * @return lv_card_config_t structure with default values
 */
lv_card_config_t lv_expandable_card_get_default_config(void);

/**
 * @brief Update card styles when theme changes
 *
 * Call this function after changing the display theme to update
 * all card colors to match the new theme.
 *
 * @param ref_obj Any widget to use as reference for theme colors
 */
void lv_expandable_card_update_theme(lv_obj_t* ref_obj);

/**
 * @brief Set the expansion state of the card
 *
 * @param card Card object returned by lv_expandable_card_create()
 * @param expanded true to expand, false to collapse
 *
 * @return true if successful, false on error
 */
bool lv_expandable_card_set_expanded(lv_obj_t* card, bool expanded);

/**
 * @brief Get the current expansion state
 *
 * @param card Card object returned by lv_expandable_card_create()
 *
 * @return true if expanded, false if collapsed or on error
 */
bool lv_expandable_card_is_expanded(lv_obj_t* card);

/**
 * @brief Toggle the expansion state
 *
 * Toggles between expanded and collapsed states.
 *
 * @param card Card object returned by lv_expandable_card_create()
 */
void lv_expandable_card_toggle(lv_obj_t* card);

#ifdef __cplusplus
}
#endif

#endif // LV_EXPANDABLE_CARD_H