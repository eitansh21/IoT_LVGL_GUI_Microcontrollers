/**
 * @file lv_pull_refresh.h
 * @brief A reusable LVGL widget for pull-to-refresh functionality
 *
 * This widget creates a scrollable container with pull-to-refresh capability.
 * It supports customizable callbacks, visual feedback, and smooth animations.
 * Theme-aware and language-agnostic design.
 *
 * Features:
 * - Callback-driven refresh behavior
 * - Visual feedback with animations and indicators
 * - Configurable pull threshold and sensitivity
 * - Theme-aware styling and language-agnostic text
 * - Memory-efficient with proper cleanup
 * - Single-threaded design following LVGL patterns
 *
 * @author ESP32 LVGL Project
 * @date 2025
 */

#ifndef LV_PULL_REFRESH_H
#define LV_PULL_REFRESH_H

#include <lvgl.h>
#include "widget_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pull-to-refresh callback function type
 *
 * This callback is invoked when a refresh is triggered by pulling.
 *
 * @param container The pull-refresh container object
 * @param user_data User data passed during widget creation
 */
typedef void (*lv_pull_refresh_cb_t)(lv_obj_t* container, void* user_data);

/**
 * @brief Pull state change callback function type
 *
 * This callback is invoked when the pull state changes (pulling, ready to refresh, etc.)
 *
 * @param container The pull-refresh container object
 * @param pull_distance Current pull distance in pixels
 * @param threshold_reached true if pull threshold has been reached
 * @param user_data User data passed during widget creation
 */
typedef void (*lv_pull_state_cb_t)(lv_obj_t* container, int pull_distance, bool threshold_reached, void* user_data);

/**
 * @brief Configuration structure for pull-to-refresh widget
 */
typedef struct {
    // Text configuration
    const char* pull_text;               /**< Text shown when pulling */
    const char* release_text;            /**< Text shown when ready to refresh */
    const char* refreshing_text;         /**< Text shown while refreshing */

    // Layout configuration
    int pull_threshold;                  /**< Pull distance to trigger refresh in pixels */
    int indicator_size;                  /**< Size of visual indicator */
    bool show_indicator;                 /**< Show visual pull indicator */

    // Styling (theme-aware)
    widget_style_t style;                /**< Common styling configuration */

    // Callbacks
    lv_pull_refresh_cb_t refresh_cb;     /**< Callback for refresh events (required) */
    lv_pull_state_cb_t state_cb;         /**< Callback for pull state changes (optional) */
    void* user_data;                     /**< User data passed to callbacks */
} lv_pull_refresh_config_t;

/**
 * @brief Create a pull-to-refresh container widget
 *
 * Creates a scrollable container with pull-to-refresh functionality.
 * The container automatically handles scroll events and triggers callbacks
 * based on user interactions.
 *
 * @param parent Parent LVGL object (typically a tab or screen)
 * @param config Configuration structure (pass NULL for defaults)
 *
 * @return Pointer to the created container object, or NULL on failure
 *
 * @note The refresh_cb in config is required and must be provided.
 *       The container supports adding child objects normally.
 *
 * @warning This widget is NOT thread-safe. All operations must be performed
 *          on the main thread where lv_timer_handler() runs. This follows
 *          LVGL's single-threaded architecture requirements.
 *          See: https://docs.lvgl.io/master/details/integration/overview/threading.html
 *
 * Example usage:
 * @code
 * // Define refresh callback
 * void my_refresh_callback(lv_obj_t* container, void* user_data) {
 *     // Perform refresh operation here
 *     ESP_LOGI("App", "Refreshing content...");
 *
 *     // Signal completion (optional)
 *     lv_pull_refresh_complete(container);
 * }
 *
 * // Create pull-refresh container with defaults
 * lv_pull_refresh_config_t config = lv_pull_refresh_get_default_config();
 * config.refresh_cb = my_refresh_callback;
 * config.user_data = my_data;
 *
 * lv_obj_t* container = lv_pull_refresh_create(parent_tab, &config);
 *
 * // Add content to the container
 * lv_obj_t* label = lv_label_create(container);
 * lv_label_set_text(label, "Content that can be refreshed");
 *
 * // Or with custom configuration
 * config.pull_threshold = 100;
 * config.style.accent_palette = LV_PALETTE_GREEN;
 * config.pull_text = "Pull down to refresh";
 * config.release_text = "Release to refresh";
 * @endcode
 */
lv_obj_t* lv_pull_refresh_create(
    lv_obj_t* parent,
    const lv_pull_refresh_config_t* config
);

/**
 * @brief Get the default pull-refresh configuration
 *
 * Returns a configuration structure with default values suitable for
 * Hebrew RTL interfaces.
 *
 * @return lv_pull_refresh_config_t structure with default values
 */
lv_pull_refresh_config_t lv_pull_refresh_get_default_config(void);

/**
 * @brief Signal that refresh operation is complete
 *
 * Call this function when your refresh operation is finished to
 * reset the visual state and allow new refresh operations.
 *
 * @param container Container object returned by lv_pull_refresh_create()
 */
void lv_pull_refresh_complete(lv_obj_t* container);

/**
 * @brief Check if container is currently refreshing
 *
 * @param container Container object returned by lv_pull_refresh_create()
 *
 * @return true if refresh is in progress, false otherwise
 */
bool lv_pull_refresh_is_refreshing(lv_obj_t* container);

/**
 * @brief Set refreshing state manually
 *
 * Manually trigger or stop the refreshing state. Useful for
 * programmatic refresh operations.
 *
 * @param container Container object returned by lv_pull_refresh_create()
 * @param refreshing true to start refreshing, false to stop
 */
void lv_pull_refresh_set_refreshing(lv_obj_t* container, bool refreshing);

/**
 * @brief Get the current pull distance
 *
 * @param container Container object returned by lv_pull_refresh_create()
 *
 * @return Current pull distance in pixels, or 0 if not pulling
 */
int lv_pull_refresh_get_pull_distance(lv_obj_t* container);

/**
 * @brief Enable or disable the pull-to-refresh functionality
 *
 * @param container Container object returned by lv_pull_refresh_create()
 * @param enabled true to enable, false to disable
 */
void lv_pull_refresh_set_enabled(lv_obj_t* container, bool enabled);

#ifdef __cplusplus
}
#endif

#endif // LV_PULL_REFRESH_H