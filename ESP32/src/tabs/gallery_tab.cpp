#include <lvgl.h>
#include "esp_log.h"
#include "lv_image_gallery.h"
#include "../ui_config/hebrew_widget_config.h"

static const char* TAG = "GALLERY_TEST";

// External declarations for the image arrays (defined in separate C files)
LV_IMG_DECLARE(image_1);
LV_IMG_DECLARE(image_2);
LV_IMG_DECLARE(image_3);

// Gallery images MUST be static/global since widget stores pointer
static lv_gallery_image_t gallery_images[] = {
    {&image_1, "תמונה 1", 0x2196F3}, // Blue
    {&image_2, "תמונה 2", 0x4CAF50}, // Green
    {&image_3, "תמונה 3", 0xFF5722}, // Orange
};

void create_gallery_tab(lv_obj_t* tab) {
    ESP_LOGI(TAG, "Creating gallery test tab with reusable widget");

    // Get Hebrew configuration for proper Hebrew fonts and RTL layout
    lv_gallery_config_t hebrew_config = hebrew_get_image_gallery_config();

    // Create the gallery widget with Hebrew configuration
    lv_obj_t* gallery = lv_image_gallery_create(
        tab,
        gallery_images,
        3,
        "גלריית תמונות אינטראקטיבית",
        &hebrew_config
    );

    if (gallery) {
        ESP_LOGI(TAG, "Gallery widget created successfully");
    } else {
        ESP_LOGE(TAG, "Failed to create gallery widget");
    }
}