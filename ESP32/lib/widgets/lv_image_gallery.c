/**
 * @file lv_image_gallery.cpp
 * Implementation of the LVGL image gallery widget
 */

#include "lv_image_gallery.h"
#include "widget_common.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdbool.h>

static const char* TAG = "GALLERY_WIDGET";

// Layout and sizing constants
#define GALLERY_CONTAINER_PADDING 15
#define GALLERY_IMAGES_PADDING 10
#define GALLERY_BUTTONS_PADDING 10
#define GALLERY_BUTTON_WIDTH 120
#define GALLERY_BUTTON_HEIGHT 50
#define GALLERY_CONTAINER_HEIGHT_FACTOR 1.3f
#define GALLERY_COUNTER_BUFFER_SIZE 128

/**
 * Internal gallery state stored in object user data
 */
typedef struct {
    const lv_gallery_image_t* images;        /**< Reference to images array (not copied for performance) */
    int                       image_count;   /**< Total number of images */
    int                       current_index; /**< Current displayed image index */
    lv_obj_t*                 img_obj;       /**< Image display object */
    lv_obj_t*                 counter_label; /**< Counter label object */
    lv_gallery_config_t       config;        /**< Widget configuration */
    char*                     counter_format; /**< Cached counter text format */
} lv_gallery_data_t;

// Forward declarations
static void update_gallery_display(lv_gallery_data_t* data);
static void prev_button_event_cb(lv_event_t* e);
static void next_button_event_cb(lv_event_t* e);
static lv_gallery_data_t* get_gallery_data(lv_obj_t* gallery);
static lv_obj_t* create_nav_button(lv_obj_t* parent, const char* text, lv_color_t color,
                                  lv_event_cb_t callback, void* user_data, lv_style_t* button_style);
static bool validate_images_array(const lv_gallery_image_t* images, int image_count);
static void cleanup_gallery_data(lv_gallery_data_t* data);

/**
 * Get default configuration with sensible defaults
 */
lv_gallery_config_t lv_image_gallery_get_default_config(void) {
    lv_gallery_config_t config = {
        .prev_text = "< Previous",
        .next_text = "Next >",
        .style = widget_get_default_style(),
        .title_style = NULL,  // Use default theme colors
        .button_style = NULL  // Use default theme colors
    };
    return config;
}

/**
 * Safely retrieve gallery data with validation
 */
static lv_gallery_data_t* get_gallery_data(lv_obj_t* gallery) {
    if (!gallery) {
        ESP_LOGW(TAG, "Gallery object is NULL");
        return NULL;
    }

    lv_gallery_data_t* data = (lv_gallery_data_t*)lv_obj_get_user_data(gallery);
    if (!data) {
        ESP_LOGW(TAG, "Gallery data is NULL");
        return NULL;
    }

    return data;
}

/**
 * Create a navigation button with consistent styling
 */
static lv_obj_t* create_nav_button(lv_obj_t* parent, const char* text, lv_color_t color,
                                  lv_event_cb_t callback, void* user_data, lv_style_t* button_style) {
    if (!parent || !text || !callback) {
        ESP_LOGE(TAG, "Invalid parameters for button creation");
        return NULL;
    }

    lv_obj_t* btn = lv_btn_create(parent);
    if (!btn) {
        ESP_LOGE(TAG, "Failed to create button");
        return NULL;
    }

    lv_obj_set_size(btn, GALLERY_BUTTON_WIDTH, GALLERY_BUTTON_HEIGHT);

    // Apply custom button style if provided
    if (button_style) {
        lv_obj_add_style(btn, button_style, 0);
    }

    lv_obj_t* label = lv_label_create(btn);
    if (label) {
        lv_label_set_text(label, text);
        lv_obj_center(label);
    }

    lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, user_data);

    return btn;
}

/**
 * Validate images array for common issues
 */
static bool validate_images_array(const lv_gallery_image_t* images, int image_count) {
    if (!images) {
        ESP_LOGE(TAG, "Images array is NULL");
        return false;
    }

    if (image_count <= 0) {
        ESP_LOGE(TAG, "Invalid image count: %d", image_count);
        return false;
    }

    for (int i = 0; i < image_count; i++) {
        if (!images[i].img_src) {
            ESP_LOGW(TAG, "Image %d has NULL source", i);
            return false;
        }

        if (!images[i].label_text) {
            ESP_LOGW(TAG, "Image %d has NULL label text", i);
        }
    }

    return true;
}

/**
 * Clean up gallery data safely
 */
static void cleanup_gallery_data(lv_gallery_data_t* data) {
    if (!data) return;

    if (data->counter_format) {
        lv_free(data->counter_format);
        data->counter_format = NULL;
    }

    lv_free(data);
    ESP_LOGD(TAG, "Gallery data freed");
}

/**
 * Update the displayed image and counter
 */
static void update_gallery_display(lv_gallery_data_t* data) {
    if (!data || !data->images) {
        ESP_LOGW(TAG, "Invalid data for display update");
        return;
    }

    if (data->current_index < 0 || data->current_index >= data->image_count) {
        ESP_LOGE(TAG, "Invalid current index: %d (count: %d)", data->current_index, data->image_count);
        return;
    }

    const lv_gallery_image_t* current_image = &data->images[data->current_index];

    // Update image with validation
    if (data->img_obj && current_image->img_src) {
        lv_img_set_src(data->img_obj, current_image->img_src);
    }

    // Update counter label with cached format
    if (data->counter_label) {
        char counter_text[GALLERY_COUNTER_BUFFER_SIZE];
        const char* label_text = current_image->label_text ? current_image->label_text : "";

        snprintf(counter_text, sizeof(counter_text), "%d / %d - %s",
                 data->current_index + 1,
                 data->image_count,
                 label_text);

        lv_label_set_text(data->counter_label, counter_text);
    }

    ESP_LOGD(TAG, "Gallery updated to image %d/%d",
             data->current_index + 1, data->image_count);
}

/**
 * Previous button event handler
 */
static void prev_button_event_cb(lv_event_t* e) {
    lv_obj_t* gallery = (lv_obj_t*)lv_event_get_user_data(e);
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return;

    data->current_index = (data->current_index - 1 + data->image_count) % data->image_count;
    update_gallery_display(data);

    ESP_LOGI(TAG, "Previous button clicked - showing image %d", data->current_index + 1);
}

/**
 * Next button event handler
 */
static void next_button_event_cb(lv_event_t* e) {
    lv_obj_t* gallery = (lv_obj_t*)lv_event_get_user_data(e);
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return;

    data->current_index = (data->current_index + 1) % data->image_count;
    update_gallery_display(data);

    ESP_LOGI(TAG, "Next button clicked - showing image %d", data->current_index + 1);
}

/**
 * Delete event handler to free allocated memory
 */
static void gallery_delete_event_cb(lv_event_t* e) {
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    lv_gallery_data_t* data = (lv_gallery_data_t*)lv_obj_get_user_data(obj);
    if (data) {
        cleanup_gallery_data(data);
    }
}

/**
 * Create the gallery widget
 */
lv_obj_t* lv_image_gallery_create(
    lv_obj_t* parent,
    const lv_gallery_image_t* images,
    int image_count,
    const char* title,
    const lv_gallery_config_t* config
) {
    // Validate inputs
    if (!parent) {
        ESP_LOGE(TAG, "Parent object is NULL");
        return NULL;
    }

    if (!validate_images_array(images, image_count)) {
        return NULL;
    }
    
    ESP_LOGI(TAG, "Creating image gallery with %d images", image_count);
    
    // Use default config if none provided
    lv_gallery_config_t cfg = config ? *config : lv_image_gallery_get_default_config();
    
    // Allocate and initialize gallery data
    lv_gallery_data_t* data = (lv_gallery_data_t*)lv_malloc(sizeof(lv_gallery_data_t));
    if (!data) {
        ESP_LOGE(TAG, "Failed to allocate gallery data");
        return NULL;
    }

    // Store reference to images array (caller must ensure lifetime)
    // This avoids copying and improves performance
    data->images = images;
    
    data->image_count = image_count;
    data->current_index = 0;
    data->img_obj = NULL;
    data->counter_label = NULL;
    data->config = cfg;
    data->counter_format = NULL;
    
    // Create main container
    lv_obj_t* container = lv_obj_create(parent);
    if (!container) {
        ESP_LOGE(TAG, "Failed to create main container");
        cleanup_gallery_data(data);
        return NULL;
    }

    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(container, GALLERY_CONTAINER_PADDING, 0);
    lv_obj_set_style_pad_row(container, GALLERY_CONTAINER_PADDING, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    
    // Set base direction from style
    lv_obj_set_style_base_dir(container, cfg.style.base_dir, 0);
    
    // Disable scrolling - image gallery should use button navigation only
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Store data in container
    lv_obj_set_user_data(container, data);
    lv_obj_add_event_cb(container, gallery_delete_event_cb, LV_EVENT_DELETE, NULL);
    
    // Create title if provided
    if (title && title[0] != '\0') {
        lv_obj_t* title_label = lv_label_create(container);
        lv_label_set_text(title_label, title);
        const lv_font_t* title_font = widget_get_theme_font(&cfg.style, cfg.style.title_font, WIDGET_FONT_SIZE_LARGE);
        lv_obj_set_style_text_font(title_label, title_font, 0);

        // Apply custom title style if provided
        if (cfg.title_style) {
            lv_obj_add_style(title_label, cfg.title_style, 0);
        }
    }
    
    // Create counter label
    data->counter_label = lv_label_create(container);
    lv_label_set_long_mode(data->counter_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(data->counter_label, LV_PCT(100));
    
    // Calculate max height for images container
    int max_height = 0;
    for (int i = 0; i < image_count; i++) {
        if (images[i].img_src && images[i].img_src->header.h > max_height) {
            max_height = images[i].img_src->header.h;
        }
    }
    const int container_height = (int)(max_height * GALLERY_CONTAINER_HEIGHT_FACTOR);
    
    // Create images container
    lv_obj_t* images_container = lv_obj_create(container);
    lv_obj_set_size(images_container, LV_PCT(100), container_height);
    lv_obj_set_style_bg_opa(images_container, LV_OPA_10, 0);
    lv_obj_set_style_border_width(images_container, 1, 0);
    lv_obj_set_style_radius(images_container, 8, 0);
    lv_obj_set_style_pad_all(images_container, GALLERY_IMAGES_PADDING, 0);
    lv_obj_set_flex_flow(images_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(images_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create image display object
    data->img_obj = lv_img_create(images_container);
    lv_obj_set_style_border_width(data->img_obj, 2, 0);
    
    // Create navigation buttons container
    lv_obj_t* buttons_container = lv_obj_create(container);
    lv_obj_set_size(buttons_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(buttons_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(buttons_container, 0, 0);
    lv_obj_set_style_pad_all(buttons_container, GALLERY_BUTTONS_PADDING, 0);
    lv_obj_set_flex_flow(buttons_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(buttons_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Disable scrolling on button container - buttons should be fixed in place
    lv_obj_clear_flag(buttons_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create navigation buttons using config text and theme colors
    const char* prev_text = cfg.prev_text ? cfg.prev_text : "< Previous";
    const char* next_text = cfg.next_text ? cfg.next_text : "Next >";

    lv_color_t primary_color = widget_get_theme_color(buttons_container, WIDGET_COLOR_PRIMARY);
    lv_color_t secondary_color = widget_get_theme_color(buttons_container, WIDGET_COLOR_SECONDARY);

    lv_obj_t* prev_btn = create_nav_button(buttons_container, prev_text, primary_color,
                                          prev_button_event_cb, container, cfg.button_style);
    lv_obj_t* next_btn = create_nav_button(buttons_container, next_text, secondary_color,
                                          next_button_event_cb, container, cfg.button_style);

    if (!prev_btn || !next_btn) {
        ESP_LOGE(TAG, "Failed to create navigation buttons");
        cleanup_gallery_data(data);
        return NULL;
    }
    
    // Initialize display with first image
    update_gallery_display(data);
    
    ESP_LOGI(TAG, "Gallery widget created successfully");
    
    return container;
}

/**
 * Set the current image index
 */
bool lv_image_gallery_set_index(lv_obj_t* gallery, int index) {
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return false;

    if (index < 0 || index >= data->image_count) {
        ESP_LOGW(TAG, "Index %d out of range [0, %d)", index, data->image_count);
        return false;
    }

    data->current_index = index;
    update_gallery_display(data);
    return true;
}

/**
 * Get the current image index
 */
int lv_image_gallery_get_index(lv_obj_t* gallery) {
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return -1;

    return data->current_index;
}

/**
 * Navigate to next image
 */
void lv_image_gallery_next(lv_obj_t* gallery) {
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return;

    data->current_index = (data->current_index + 1) % data->image_count;
    update_gallery_display(data);
}

/**
 * Navigate to previous image
 */
void lv_image_gallery_prev(lv_obj_t* gallery) {
    lv_gallery_data_t* data = get_gallery_data(gallery);
    if (!data) return;

    data->current_index = (data->current_index - 1 + data->image_count) % data->image_count;
    update_gallery_display(data);
}
