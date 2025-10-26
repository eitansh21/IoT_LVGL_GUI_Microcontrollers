/**
 * @file lv_pull_refresh.cpp
 * Implementation of the LVGL pull-to-refresh widget
 */

#include "lv_pull_refresh.h"
#include "widget_common.h"
#include "esp_log.h"
#include <string.h>
#include <stdbool.h>

static const char* TAG = "PULL_REFRESH";

// Layout and sizing constants
#define PULL_DEFAULT_THRESHOLD 25  // Much more responsive threshold
#define PULL_DEFAULT_PADDING 20
#define PULL_DEFAULT_INDICATOR_SIZE 24
#define PULL_ANIMATION_DURATION 300
#define PULL_REFRESH_COOLDOWN 1000  // Minimum time between refreshes (ms)

/**
 * @brief Pull state enumeration
 */
typedef enum {
    PULL_STATE_IDLE,        /**< Not pulling */
    PULL_STATE_PULLING,     /**< Pulling but below threshold */
    PULL_STATE_READY,       /**< Pulled past threshold, ready to refresh */
    PULL_STATE_REFRESHING   /**< Currently refreshing */
} lv_pull_state_t;

/**
 * Internal pull-refresh state stored in object user data
 */
typedef struct {
    lv_pull_refresh_config_t config;     /**< Widget configuration */
    lv_obj_t* indicator_label;           /**< Pull indicator label */
    lv_obj_t* indicator_icon;            /**< Pull indicator icon/spinner */
    lv_pull_state_t state;               /**< Current pull state */
    int current_pull_distance;          /**< Current pull distance in pixels */
    bool refresh_triggered;              /**< Flag to prevent multiple triggers */
    uint32_t last_refresh_time;          /**< Last refresh timestamp for cooldown */
    bool enabled;                        /**< Whether pull-to-refresh is enabled */
} lv_pull_refresh_data_t;

// Forward declarations
static void update_indicator_display(lv_pull_refresh_data_t* data);
static void scroll_event_cb(lv_event_t* e);
static lv_pull_refresh_data_t* get_pull_refresh_data(lv_obj_t* container);
static void cleanup_pull_refresh_data(lv_pull_refresh_data_t* data);
static void animate_indicator(lv_obj_t* indicator, bool show);
static void set_pull_state(lv_pull_refresh_data_t* data, lv_pull_state_t new_state, lv_obj_t* container);

/**
 * Get default configuration with sensible defaults
 */
lv_pull_refresh_config_t lv_pull_refresh_get_default_config(void) {
    lv_pull_refresh_config_t config = {
        // Text configuration - language agnostic
        .pull_text = "Pull to refresh",
        .release_text = "Release to refresh",
        .refreshing_text = "Refreshing...",

        // Layout configuration
        .pull_threshold = PULL_DEFAULT_THRESHOLD,
        .indicator_size = PULL_DEFAULT_INDICATOR_SIZE,
        .show_indicator = true,

        // Theme-aware styling
        .style = widget_get_default_style(),

        // Callbacks
        .refresh_cb = NULL,
        .state_cb = NULL,
        .user_data = NULL
    };
    return config;
}

/**
 * Safely retrieve pull-refresh data with validation
 */
static lv_pull_refresh_data_t* get_pull_refresh_data(lv_obj_t* container) {
    if (!container) {
        ESP_LOGW(TAG, "Container object is NULL");
        return NULL;
    }

    lv_pull_refresh_data_t* data = (lv_pull_refresh_data_t*)lv_obj_get_user_data(container);
    if (!data) {
        ESP_LOGW(TAG, "Pull-refresh data is NULL");
        return NULL;
    }

    return data;
}

/**
 * Clean up pull-refresh data safely
 */
static void cleanup_pull_refresh_data(lv_pull_refresh_data_t* data) {
    if (!data) return;

    lv_free(data);
    ESP_LOGD(TAG, "Pull-refresh data freed");
}

/**
 * Simple indicator visibility (no complex animations to avoid issues)
 */
static void set_indicator_visible(lv_obj_t* indicator, bool visible) {
    if (!indicator) return;

    if (visible) {
        lv_obj_clear_flag(indicator, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_opa(indicator, LV_OPA_COVER, 0);
    } else {
        lv_obj_add_flag(indicator, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * Set pull state and trigger callbacks
 */
static void set_pull_state(lv_pull_refresh_data_t* data, lv_pull_state_t new_state, lv_obj_t* container) {
    if (!data || data->state == new_state) return;

    lv_pull_state_t old_state = data->state;
    data->state = new_state;

    // Update visual indicators
    update_indicator_display(data);

    // Trigger state callback if provided
    if (data->config.state_cb) {
        bool threshold_reached = (new_state == PULL_STATE_READY || new_state == PULL_STATE_REFRESHING);
        data->config.state_cb(
            container,
            data->current_pull_distance,
            threshold_reached,
            data->config.user_data
        );
    }

    ESP_LOGD(TAG, "Pull state changed: %d -> %d", old_state, new_state);
}

/**
 * Update the indicator display based on current state
 */
static void update_indicator_display(lv_pull_refresh_data_t* data) {
    if (!data || !data->config.show_indicator) return;

    const char* text = NULL;
    lv_color_t color;
    bool show_indicator = false;

    switch (data->state) {
        case PULL_STATE_IDLE:
            text = NULL;  // Hide indicator
            show_indicator = false;
            break;
        case PULL_STATE_PULLING:
            text = data->config.pull_text;
            color = widget_get_theme_color(data->indicator_label, WIDGET_COLOR_PRIMARY);
            show_indicator = true;
            break;
        case PULL_STATE_READY:
            text = data->config.release_text;
            color = widget_get_theme_color(data->indicator_label, WIDGET_COLOR_SECONDARY);
            show_indicator = true;
            break;
        case PULL_STATE_REFRESHING:
            text = data->config.refreshing_text;
            color = widget_get_theme_color(data->indicator_label, WIDGET_COLOR_PRIMARY);
            show_indicator = true;
            break;
    }

    // Update indicator text and visibility
    if (data->indicator_label) {
        if (show_indicator && text) {
            lv_label_set_text(data->indicator_label, text);

            // Use secondary theme color for release state for better visibility
            if (data->state == PULL_STATE_READY) {
                lv_color_t release_color = widget_get_theme_color(data->indicator_label, WIDGET_COLOR_SECONDARY);
                lv_obj_set_style_text_color(data->indicator_label, release_color, 0);
            } else {
                lv_obj_set_style_text_color(data->indicator_label, color, 0);
            }

            set_indicator_visible(data->indicator_label, true);
        } else {
            set_indicator_visible(data->indicator_label, false);
        }
    }

    // Update icon/spinner visibility
    if (data->indicator_icon) {
        if (show_indicator) {
            lv_obj_set_style_text_color(data->indicator_icon, color, 0);
            set_indicator_visible(data->indicator_icon, true);

            // Simple visual change for refreshing state
            if (data->state == PULL_STATE_REFRESHING) {
                // Use refresh symbol for refreshing
                lv_label_set_text(data->indicator_icon, LV_SYMBOL_REFRESH);
            } else {
                lv_label_set_text(data->indicator_icon, LV_SYMBOL_DOWN);
            }
        } else {
            set_indicator_visible(data->indicator_icon, false);
        }
    }
}

/**
 * Scroll event handler for pull-to-refresh logic
 */
static void scroll_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* container = (lv_obj_t*)lv_event_get_target(e);
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);

    if (!data || !data->enabled) return;

    if (code == LV_EVENT_SCROLL) {
        lv_coord_t scroll_y = lv_obj_get_scroll_y(container);
        data->current_pull_distance = -scroll_y;  // Convert to positive pull distance

        ESP_LOGD(TAG, "Scroll Y: %d, Pull distance: %d, State: %d", scroll_y, data->current_pull_distance, data->state);

        // Don't change state if currently refreshing
        if (data->state == PULL_STATE_REFRESHING) {
            return;
        }

        // Determine state based on pull distance
        if (data->current_pull_distance <= 0) {
            set_pull_state(data, PULL_STATE_IDLE, container);
        } else if (data->current_pull_distance < data->config.pull_threshold) {
            set_pull_state(data, PULL_STATE_PULLING, container);
        } else {
            set_pull_state(data, PULL_STATE_READY, container);
        }

    } else if (code == LV_EVENT_SCROLL_END) {
        ESP_LOGD(TAG, "Scroll end - State: %d, Pull distance: %d", data->state, data->current_pull_distance);

        // Check for refresh trigger
        if (data->state == PULL_STATE_READY && !data->refresh_triggered) {
            // Check cooldown
            uint32_t current_time = lv_tick_get();
            if (current_time - data->last_refresh_time >= PULL_REFRESH_COOLDOWN) {
                data->refresh_triggered = true;
                data->last_refresh_time = current_time;
                set_pull_state(data, PULL_STATE_REFRESHING, container);

                // Trigger refresh callback
                if (data->config.refresh_cb) {
                    data->config.refresh_cb(container, data->config.user_data);
                }

                ESP_LOGI(TAG, "Pull-to-refresh triggered!");
                return;  // Don't scroll back yet, wait for completion
            }
        }

        // Reset to top with animation only if not refreshing
        if (data->state != PULL_STATE_REFRESHING) {
            lv_obj_scroll_to_y(container, 0, LV_ANIM_ON);
            set_pull_state(data, PULL_STATE_IDLE, container);
        }
    }
}

/**
 * Delete event handler to free allocated memory
 */
static void pull_refresh_delete_event_cb(lv_event_t* e) {
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    lv_pull_refresh_data_t* data = (lv_pull_refresh_data_t*)lv_obj_get_user_data(obj);
    if (data) {
        cleanup_pull_refresh_data(data);
    }
}

/**
 * Create the pull-to-refresh container widget
 */
lv_obj_t* lv_pull_refresh_create(
    lv_obj_t* parent,
    const lv_pull_refresh_config_t* config
) {
    // Validate inputs
    if (!parent) {
        ESP_LOGE(TAG, "Parent object is NULL");
        return NULL;
    }

    if (!config || !config->refresh_cb) {
        ESP_LOGE(TAG, "Config or refresh callback is NULL");
        return NULL;
    }

    ESP_LOGI(TAG, "Creating pull-to-refresh container");

    // Allocate and initialize data
    lv_pull_refresh_data_t* data = (lv_pull_refresh_data_t*)lv_malloc(sizeof(lv_pull_refresh_data_t));
    if (!data) {
        ESP_LOGE(TAG, "Failed to allocate pull-refresh data");
        return NULL;
    }

    data->config = *config;
    data->indicator_label = NULL;
    data->indicator_icon = NULL;
    data->state = PULL_STATE_IDLE;
    data->current_pull_distance = 0;
    data->refresh_triggered = false;
    data->last_refresh_time = 0;
    data->enabled = true;

    // Create main container
    lv_obj_t* container = lv_obj_create(parent);
    if (!container) {
        ESP_LOGE(TAG, "Failed to create container");
        cleanup_pull_refresh_data(data);
        return NULL;
    }

    // Configure container properties
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(container, data->config.style.padding, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);

    // Set text direction based on style configuration
    lv_obj_set_style_base_dir(container, data->config.style.base_dir, 0);

    // Configure scrolling
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    // CRITICAL: Pull-to-refresh REQUIRES elastic scrolling to function
    // This is not negotiable and overrides any style configuration
    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLL_ELASTIC);

    if (data->config.style.enable_momentum_scroll) {
        lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    } else {
        lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    }

    // Set flex layout
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    // Create pull indicator if enabled
    if (data->config.show_indicator) {
        data->indicator_label = lv_label_create(container);
        lv_obj_set_width(data->indicator_label, LV_PCT(100));
        lv_obj_set_style_text_align(data->indicator_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_all(data->indicator_label, 10, 0);
        lv_obj_add_flag(data->indicator_label, LV_OBJ_FLAG_HIDDEN);  // Start hidden

        // Apply theme-aware font for indicator text
        const lv_font_t* indicator_font = widget_get_theme_font(&data->config.style, data->config.style.content_font, WIDGET_FONT_SIZE_SMALL);
        lv_obj_set_style_text_font(data->indicator_label, indicator_font, 0);

        // Create simple icon/spinner (using a unicode symbol)
        data->indicator_icon = lv_label_create(container);
        lv_label_set_text(data->indicator_icon, LV_SYMBOL_REFRESH);
        lv_obj_set_style_text_font(data->indicator_icon, &lv_font_montserrat_14, 0);
        lv_color_t icon_color = widget_get_theme_color(data->indicator_icon, WIDGET_COLOR_PRIMARY);
        lv_obj_set_style_text_color(data->indicator_icon, icon_color, 0);
        lv_obj_set_style_text_align(data->indicator_icon, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_add_flag(data->indicator_icon, LV_OBJ_FLAG_HIDDEN);  // Start hidden
        lv_obj_set_width(data->indicator_icon, LV_PCT(100));
    }

    // Store data in container and set up cleanup
    lv_obj_set_user_data(container, data);
    lv_obj_add_event_cb(container, pull_refresh_delete_event_cb, LV_EVENT_DELETE, NULL);

    // Add scroll event handlers
    lv_obj_add_event_cb(container, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_add_event_cb(container, scroll_event_cb, LV_EVENT_SCROLL_END, NULL);

    ESP_LOGI(TAG, "Pull-to-refresh container created successfully");

    return container;
}

/**
 * Signal that refresh operation is complete
 */
void lv_pull_refresh_complete(lv_obj_t* container) {
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);
    if (!data) return;

    ESP_LOGI(TAG, "Pull-to-refresh completed");

    data->refresh_triggered = false;
    data->current_pull_distance = 0;

    // Reset state and scroll to top
    set_pull_state(data, PULL_STATE_IDLE, container);
    lv_obj_scroll_to_y(container, 0, LV_ANIM_ON);
}

/**
 * Check if container is currently refreshing
 */
bool lv_pull_refresh_is_refreshing(lv_obj_t* container) {
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);
    if (!data) return false;

    return data->state == PULL_STATE_REFRESHING;
}

/**
 * Set refreshing state manually
 */
void lv_pull_refresh_set_refreshing(lv_obj_t* container, bool refreshing) {
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);
    if (!data) return;

    if (refreshing) {
        data->refresh_triggered = true;
        set_pull_state(data, PULL_STATE_REFRESHING, container);
    } else {
        lv_pull_refresh_complete(container);
    }
}

/**
 * Get the current pull distance
 */
int lv_pull_refresh_get_pull_distance(lv_obj_t* container) {
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);
    if (!data) return 0;

    return data->current_pull_distance;
}

/**
 * Enable or disable the pull-to-refresh functionality
 */
void lv_pull_refresh_set_enabled(lv_obj_t* container, bool enabled) {
    lv_pull_refresh_data_t* data = get_pull_refresh_data(container);
    if (!data) return;

    data->enabled = enabled;

    if (!enabled && data->state != PULL_STATE_IDLE) {
        lv_pull_refresh_complete(container);
    }
}