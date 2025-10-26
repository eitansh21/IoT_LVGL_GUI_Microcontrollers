/**
 * @file lv_expandable_card.cpp
 * Implementation of the LVGL expandable card widget
 */

#include "lv_expandable_card.h"
#include "widget_common.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static const char* TAG = "EXPANDABLE_CARD";

// Layout and sizing constants
#define CARD_DEFAULT_PADDING 15
#define CARD_DEFAULT_ROW_PADDING 8
#define CARD_DEFAULT_RADIUS 8
#define CARD_DEFAULT_BORDER_WIDTH 1
#define CARD_DEFAULT_BUTTON_WIDTH 100
#define CARD_DEFAULT_BUTTON_HEIGHT 30
#define CARD_DEFAULT_BUTTON_RADIUS 5
#define CARD_DEFAULT_TRUNCATE_LENGTH 60
#define CARD_TRUNCATE_BUFFER_SIZE 256

/**
 * Internal card state stored in object user data
 */
typedef struct {
    const lv_card_data_t* card_data;     /**< Reference to card data (not copied for performance) */
    lv_obj_t* content_container;         /**< Content container (for scrolling) */
    lv_obj_t* content_label;             /**< Content display label */
    lv_obj_t* expand_btn;                /**< Expand/collapse button */
    lv_obj_t* expand_label;              /**< Button label */
    lv_card_config_t config;             /**< Widget configuration */
    bool expanded;                       /**< Current expansion state */
    char* truncated_text;                /**< Cached truncated text */
} lv_card_widget_data_t;

// Theme-aware styles
static lv_style_t card_title_style;
static lv_style_t card_button_style;
static lv_style_t card_button_text_style;
static lv_style_t card_container_style;
static bool styles_initialized = false;

// Forward declarations
static void update_card_display(lv_card_widget_data_t* data);
static void expand_button_event_cb(lv_event_t* e);
static lv_card_widget_data_t* get_card_data(lv_obj_t* card);
static bool validate_card_data(const lv_card_data_t* card_data);
static void cleanup_card_data(lv_card_widget_data_t* data);
static char* create_truncated_text(const char* full_text, int max_length, const char* suffix);
static void init_card_styles(lv_obj_t* ref_obj);
static void update_card_styles(lv_obj_t* ref_obj);

/**
 * Initialize theme-aware styles for cards
 */
static void init_card_styles(lv_obj_t* ref_obj) {
    if (styles_initialized) return;

    // Initialize title style
    lv_style_init(&card_title_style);
    lv_style_set_text_color(&card_title_style, lv_theme_get_color_primary(ref_obj));

    // Initialize button style - let theme handle colors
    lv_style_init(&card_button_style);

    // Initialize button text style - let theme handle colors
    lv_style_init(&card_button_text_style);

    // Initialize container style
    lv_style_init(&card_container_style);
    lv_style_set_border_color(&card_container_style, lv_theme_get_color_secondary(ref_obj));

    styles_initialized = true;
}

/**
 * Update theme-aware styles when theme changes
 */
static void update_card_styles(lv_obj_t* ref_obj) {
    if (!styles_initialized) return;

    // Update colors to match current theme
    lv_style_set_text_color(&card_title_style, lv_theme_get_color_primary(ref_obj));
    lv_style_set_border_color(&card_container_style, lv_theme_get_color_secondary(ref_obj));
}

/**
 * Get default configuration with theme-aware sensible defaults
 */
lv_card_config_t lv_expandable_card_get_default_config(void) {
    lv_card_config_t config = {
        // Text configuration - language agnostic
        .expand_text = "Show More",
        .collapse_text = "Show Less",
        .truncate_length = CARD_DEFAULT_TRUNCATE_LENGTH,
        .truncate_suffix = "...",

        // Layout configuration
        .max_content_height = 0,  // No height limit by default
        .button_width = CARD_DEFAULT_BUTTON_WIDTH,
        .button_height = CARD_DEFAULT_BUTTON_HEIGHT,

        // Theme-aware styling
        .style = widget_get_default_style(),
        .title_style = NULL,  // Use default theme colors
        .button_style = NULL, // Use default theme colors

        // Callbacks
        .on_expand = NULL,
        .on_collapse = NULL,
        .user_data = NULL
    };
    return config;
}

/**
 * Safely retrieve card data with validation
 */
static lv_card_widget_data_t* get_card_data(lv_obj_t* card) {
    if (!card) {
        ESP_LOGW(TAG, "Card object is NULL");
        return NULL;
    }

    lv_card_widget_data_t* data = (lv_card_widget_data_t*)lv_obj_get_user_data(card);
    if (!data) {
        ESP_LOGW(TAG, "Card data is NULL");
        return NULL;
    }

    return data;
}

/**
 * Validate card data for common issues
 */
static bool validate_card_data(const lv_card_data_t* card_data) {
    if (!card_data) {
        ESP_LOGE(TAG, "Card data is NULL");
        return false;
    }

    if (!card_data->title) {
        ESP_LOGW(TAG, "Card title is NULL");
        return false;
    }

    if (!card_data->content) {
        ESP_LOGW(TAG, "Card content is NULL");
        return false;
    }

    return true;
}

/**
 * Clean up card data safely
 */
/**
 * UTF-8 aware function to count characters (not bytes)
 */
static int utf8_char_count(const char* text) {
    if (!text) return 0;

    int char_count = 0;
    const unsigned char* p = (const unsigned char*)text;

    while (*p) {
        // Skip UTF-8 continuation bytes (10xxxxxx)
        if ((*p & 0xC0) != 0x80) {
            char_count++;
        }
        p++;
    }
    return char_count;
}

/**
 * UTF-8 aware function to find byte position of nth character
 */
static int utf8_char_to_byte_pos(const char* text, int char_pos) {
    if (!text || char_pos <= 0) return 0;

    int char_count = 0;
    int byte_pos = 0;
    const unsigned char* p = (const unsigned char*)text;

    while (*p && char_count < char_pos) {
        // Count only non-continuation bytes
        if ((*p & 0xC0) != 0x80) {
            char_count++;
        }
        if (char_count < char_pos) {
            byte_pos++;
        }
        p++;
    }
    return byte_pos;
}

/**
 * Create truncated version of text with suffix (UTF-8 aware for Hebrew)
 */
static char* create_truncated_text(const char* full_text, int max_chars, const char* suffix) {
    if (!full_text) return NULL;

    int full_char_count = utf8_char_count(full_text);
    int suffix_char_count = suffix ? utf8_char_count(suffix) : 0;

    // If text is already short enough, return copy of original
    if (full_char_count <= max_chars) {
        size_t full_len = strlen(full_text);
        char* result = (char*)lv_malloc(full_len + 1);
        if (result) {
            strcpy(result, full_text);
        }
        return result;
    }

    // Calculate truncation point in characters (leave room for suffix)
    int truncate_char_count = max_chars - suffix_char_count;
    if (truncate_char_count < 0) truncate_char_count = 0;

    // Find byte position for truncation
    int truncate_byte_pos = utf8_char_to_byte_pos(full_text, truncate_char_count);

    // Calculate total size needed
    size_t suffix_byte_len = suffix ? strlen(suffix) : 0;
    size_t result_size = truncate_byte_pos + suffix_byte_len + 1;

    char* result = (char*)lv_malloc(result_size);
    if (!result) {
        ESP_LOGE(TAG, "Failed to allocate truncated text buffer");
        return NULL;
    }

    // Copy truncated text (safe byte-wise copy up to character boundary)
    strncpy(result, full_text, truncate_byte_pos);
    result[truncate_byte_pos] = '\0';

    // Append suffix if provided
    if (suffix && suffix_byte_len > 0) {
        strcat(result, suffix);
    }

    return result;
}

/**
 * Clean up card data safely
 */
static void cleanup_card_data(lv_card_widget_data_t* data) {
    if (!data) return;

    if (data->truncated_text) {
        lv_free(data->truncated_text);
        data->truncated_text = NULL;
    }

    lv_free(data);
    ESP_LOGD(TAG, "Card data freed");
}

/**
 * Update the displayed content based on expansion state
 */
static void update_card_display(lv_card_widget_data_t* data) {
    if (!data || !data->card_data) {
        ESP_LOGW(TAG, "Invalid data for display update");
        return;
    }

    const lv_card_data_t* card_data = data->card_data;

    // Update content label
    if (data->content_label) {
        const char* content;
        if (data->expanded) {
            content = card_data->content;

            // Apply max height for expanded content if specified
            if (data->config.max_content_height > 0) {
                lv_obj_set_height(data->content_container, data->config.max_content_height);
                lv_obj_add_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);
                lv_obj_set_scroll_dir(data->content_container, LV_DIR_VER);
            } else {
                lv_obj_set_height(data->content_container, LV_SIZE_CONTENT);
                lv_obj_clear_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);
            }
        } else {
            // Use cached truncated text or create it if needed
            if (!data->truncated_text) {
                data->truncated_text = create_truncated_text(
                    card_data->content,
                    data->config.truncate_length,
                    data->config.truncate_suffix
                );
            }
            content = data->truncated_text ? data->truncated_text : card_data->content;

            // Always use content size for collapsed state
            lv_obj_set_height(data->content_container, LV_SIZE_CONTENT);
            lv_obj_clear_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);

            // Reset scroll position to top when collapsing
            lv_obj_scroll_to(data->content_container, 0, 0, LV_ANIM_OFF);
        }
        lv_label_set_text(data->content_label, content);
    }

    // Update button text
    if (data->expand_label) {
        const char* btn_text = data->expanded ? data->config.collapse_text : data->config.expand_text;
        lv_label_set_text(data->expand_label, btn_text);
    }

    ESP_LOGD(TAG, "Card updated - expanded: %s", data->expanded ? "true" : "false");
}

/**
 * Expand button event handler
 */
static void expand_button_event_cb(lv_event_t* e) {
    lv_obj_t* card = (lv_obj_t*)lv_event_get_user_data(e);
    lv_card_widget_data_t* data = get_card_data(card);
    if (!data) return;

    // Toggle expansion state
    bool was_expanded = data->expanded;
    data->expanded = !data->expanded;
    update_card_display(data);

    // Call callbacks if provided
    if (data->expanded && data->config.on_expand) {
        data->config.on_expand(data->config.user_data);
    } else if (!data->expanded && data->config.on_collapse) {
        data->config.on_collapse(data->config.user_data);
    }

    // When collapsing, scroll the card into view to prevent it from being "lost"
    if (!data->expanded) {
        lv_obj_scroll_to_view(card, LV_ANIM_ON);
    }

    ESP_LOGI(TAG, "Card %s", data->expanded ? "expanded" : "collapsed");
}

/**
 * Delete event handler to free allocated memory
 */
static void card_delete_event_cb(lv_event_t* e) {
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    lv_card_widget_data_t* data = (lv_card_widget_data_t*)lv_obj_get_user_data(obj);
    if (data) {
        cleanup_card_data(data);
    }
}

/**
 * Create the expandable card widget
 */
lv_obj_t* lv_expandable_card_create(
    lv_obj_t* parent,
    const lv_card_data_t* card_data,
    const lv_card_config_t* config
) {
    // Validate inputs
    if (!parent) {
        ESP_LOGE(TAG, "Parent object is NULL");
        return NULL;
    }

    if (!validate_card_data(card_data)) {
        return NULL;
    }

    ESP_LOGI(TAG, "Creating expandable card: %s", card_data->title);

    // Use default config if none provided
    lv_card_config_t cfg = config ? *config : lv_expandable_card_get_default_config();

    // Ensure button text is set (defaults are provided in get_default_config)
    if (!cfg.expand_text) {
        cfg.expand_text = "Show More";
    }
    if (!cfg.collapse_text) {
        cfg.collapse_text = "Show Less";
    }

    // Allocate and initialize card data
    lv_card_widget_data_t* data = (lv_card_widget_data_t*)lv_malloc(sizeof(lv_card_widget_data_t));
    if (!data) {
        ESP_LOGE(TAG, "Failed to allocate card data");
        return NULL;
    }

    // Store reference to card data (caller must ensure lifetime)
    data->card_data = card_data;
    data->content_container = NULL;
    data->content_label = NULL;
    data->expand_btn = NULL;
    data->expand_label = NULL;
    data->config = cfg;
    data->expanded = false;
    data->truncated_text = NULL;

    // Create main card container
    lv_obj_t* card_container = lv_obj_create(parent);
    if (!card_container) {
        ESP_LOGE(TAG, "Failed to create card container");
        cleanup_card_data(data);
        return NULL;
    }

    lv_obj_set_size(card_container, LV_PCT(100), LV_SIZE_CONTENT);

    // Initialize theme-aware styles
    init_card_styles(card_container);

    // Apply theme-aware styling
    lv_obj_set_style_pad_all(card_container, cfg.style.padding, 0);
    lv_obj_set_style_pad_row(card_container, cfg.style.margin, 0);
    lv_obj_set_style_border_width(card_container, cfg.style.border_width, 0);
    lv_obj_set_style_radius(card_container, cfg.style.border_radius, 0);
    lv_obj_add_style(card_container, &card_container_style, 0);
    lv_obj_set_flex_flow(card_container, LV_FLEX_FLOW_COLUMN);

    // Only allow vertical scrolling, disable horizontal scrolling
    lv_obj_set_scroll_dir(card_container, LV_DIR_VER);

    // Set text direction and alignment based on configuration
    lv_obj_set_style_base_dir(card_container, cfg.style.base_dir, 0);
    if (cfg.style.base_dir == LV_BASE_DIR_RTL) {
        lv_obj_set_flex_align(card_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);
    } else {
        lv_obj_set_flex_align(card_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    }

    // Store data in container
    lv_obj_set_user_data(card_container, data);
    lv_obj_add_event_cb(card_container, card_delete_event_cb, LV_EVENT_DELETE, NULL);

    // Create title label
    lv_obj_t* title_label = lv_label_create(card_container);
    lv_label_set_text(title_label, card_data->title);
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(title_label, LV_PCT(100));

    // Apply theme-aware styling for title
    const lv_font_t* title_font = widget_get_theme_font(&cfg.style, cfg.style.title_font, WIDGET_FONT_SIZE_LARGE);
    lv_obj_set_style_text_font(title_label, title_font, 0);

    // Apply custom title style if provided
    if (cfg.title_style) {
        lv_obj_add_style(title_label, cfg.title_style, 0);
    }

    // Set text direction for title label
    lv_obj_set_style_base_dir(title_label, cfg.style.base_dir, 0);
    if (cfg.style.base_dir == LV_BASE_DIR_RTL) {
        lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_RIGHT, 0);
    } else {
        lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_LEFT, 0);
    }

    // Create content container (for potential scrolling)
    lv_obj_t* content_container = lv_obj_create(card_container);
    lv_obj_set_width(content_container, LV_PCT(100));
    lv_obj_set_height(content_container, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(content_container, 0, 0);
    lv_obj_set_style_bg_opa(content_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(content_container, 0, 0);

    // Create content label inside the container
    data->content_label = lv_label_create(content_container);
    lv_label_set_long_mode(data->content_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(data->content_label, LV_PCT(100));

    // Apply theme-aware font for content
    const lv_font_t* content_font = widget_get_theme_font(&cfg.style, cfg.style.content_font, WIDGET_FONT_SIZE_NORMAL);
    lv_obj_set_style_text_font(data->content_label, content_font, 0);

    // Store content container reference
    data->content_container = content_container;

    // Set text direction for content label
    lv_obj_set_style_base_dir(data->content_label, cfg.style.base_dir, 0);
    if (cfg.style.base_dir == LV_BASE_DIR_RTL) {
        lv_obj_set_style_text_align(data->content_label, LV_TEXT_ALIGN_RIGHT, 0);
    } else {
        lv_obj_set_style_base_dir(data->content_label, LV_BASE_DIR_LTR, 0);
        lv_obj_set_style_text_align(data->content_label, LV_TEXT_ALIGN_LEFT, 0);
    }

    // Create expand button
    data->expand_btn = lv_btn_create(card_container);
    lv_obj_set_size(data->expand_btn, cfg.button_width, cfg.button_height);
    lv_obj_set_style_radius(data->expand_btn, cfg.style.border_radius, 0);

    // Apply custom button style if provided
    if (cfg.button_style) {
        lv_obj_add_style(data->expand_btn, cfg.button_style, 0);
    }

    data->expand_label = lv_label_create(data->expand_btn);
    const lv_font_t* button_font = widget_get_theme_font(&cfg.style, cfg.style.button_font, WIDGET_FONT_SIZE_SMALL);
    lv_obj_set_style_text_font(data->expand_label, button_font, 0);
    lv_obj_center(data->expand_label);

    // Set up button event
    lv_obj_add_event_cb(data->expand_btn, expand_button_event_cb, LV_EVENT_CLICKED, card_container);

    // Initialize display with collapsed state
    update_card_display(data);

    ESP_LOGI(TAG, "Card widget created successfully");

    return card_container;
}

/**
 * Update card styles when theme changes
 */
void lv_expandable_card_update_theme(lv_obj_t* ref_obj) {
    update_card_styles(ref_obj);
}

/**
 * Set the expansion state of the card
 */
bool lv_expandable_card_set_expanded(lv_obj_t* card, bool expanded) {
    lv_card_widget_data_t* data = get_card_data(card);
    if (!data) return false;

    data->expanded = expanded;
    update_card_display(data);
    return true;
}

/**
 * Get the current expansion state
 */
bool lv_expandable_card_is_expanded(lv_obj_t* card) {
    lv_card_widget_data_t* data = get_card_data(card);
    if (!data) return false;

    return data->expanded;
}

/**
 * Toggle the expansion state
 */
void lv_expandable_card_toggle(lv_obj_t* card) {
    lv_card_widget_data_t* data = get_card_data(card);
    if (!data) return;

    data->expanded = !data->expanded;
    update_card_display(data);
}