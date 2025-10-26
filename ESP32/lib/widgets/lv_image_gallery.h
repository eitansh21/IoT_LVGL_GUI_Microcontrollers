/**
 * @file lv_image_gallery.h
 * @brief A reusable LVGL widget for displaying an interactive image gallery
 *
 * This widget creates a carousel-style image gallery with navigation controls.
 * It accepts an array of LVGL image descriptors (C array images) and allows
 * users to navigate through them using Previous/Next buttons.
 *
 * Features:
 * - Automatic sizing based on largest image
 * - Theme-aware navigation buttons and styling
 * - Image counter display
 * - Language-agnostic design
 * - Non-scrollable gallery behavior (button navigation only)
 * - Single-threaded design following LVGL patterns
 *
 * @author ESP32 LVGL Project
 * @date 2025
 */

#ifndef LV_IMAGE_GALLERY_H
#define LV_IMAGE_GALLERY_H

#include <lvgl.h>
#include "widget_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold gallery image data
 * 
 * Each image in the gallery requires:
 * - img_src: Pointer to LVGL image descriptor (e.g., &image_1)
 * - label_text: Text label to display for this image (RTL Hebrew supported)
 * - border_color: RGB color for image border (e.g., 0x2196F3)
 */
typedef struct {
    const lv_img_dsc_t* img_src;     /**< LVGL image descriptor pointer */
    const char* label_text;          /**< Label text for image (supports RTL) */
    uint32_t border_color;           /**< Border color in RGB hex (e.g., 0xFF5722) */
} lv_gallery_image_t;

/**
 * @brief Configuration structure for gallery widget
 */
typedef struct {
    // Navigation button text
    const char* prev_text;           /**< Text for previous button */
    const char* next_text;           /**< Text for next button */

    // Styling (theme-aware)
    widget_style_t style;            /**< Common styling configuration */
    lv_style_t* title_style;         /**< Custom title style (optional, NULL for default) */
    lv_style_t* button_style;        /**< Custom button style (optional, NULL for default) */
} lv_gallery_config_t;

/**
 * @brief Create an image gallery widget
 * 
 * Creates a complete interactive image gallery with navigation controls,
 * image counter, and customizable appearance.
 * 
 * @param parent Parent LVGL object (typically a tab or screen)
 * @param images Array of lv_gallery_image_t structures containing image data
 * @param image_count Number of images in the array
 * @param title Gallery title text (RTL Hebrew supported, can be NULL)
 * @param config Optional configuration (pass NULL for defaults)
 * 
 * @return Pointer to the created gallery container object, or NULL on failure
 * 
 * @note The parent object should have sufficient space to display the gallery.
 *       The widget will size itself based on the largest image in the array.
 *       The images array is stored by REFERENCE - caller must ensure it remains valid
 *       for the lifetime of the gallery widget.
 *
 * @warning This widget is NOT thread-safe. All operations must be performed
 *          on the main thread where lv_timer_handler() runs. This follows
 *          LVGL's single-threaded architecture requirements.
 *          See: https://docs.lvgl.io/master/details/integration/overview/threading.html
 * 
 * Example usage:
 * @code
 * // Declare images (in your C files)
 * LV_IMG_DECLARE(image_1);
 * LV_IMG_DECLARE(image_2);
 * LV_IMG_DECLARE(image_3);
 *
 * // Create gallery data (must be static/global for widget lifetime)
 * static lv_gallery_image_t my_images[] = {
 *     {&image_1, "תמונה ראשונה", 0x2196F3},
 *     {&image_2, "תמונה שנייה", 0x4CAF50},
 *     {&image_3, "תמונה שלישית", 0xFF5722}
 * };
 *
 * // Create gallery with default RTL Hebrew mode
 * lv_obj_t* gallery = lv_image_gallery_create(
 *     parent_tab,
 *     my_images,
 *     3,
 *     "גלריית התמונות שלי",
 *     NULL  // Use default config (RTL Hebrew)
 * );
 *
 * // Or with custom config for LTR mode
 * lv_gallery_config_t config = lv_image_gallery_get_default_config();
 * config.rtl_mode = false;  // Use LTR mode for English
 *
 * lv_obj_t* gallery2 = lv_image_gallery_create(
 *     parent_tab, my_images, 3, "My Gallery", &config
 * );
 * @endcode
 */
lv_obj_t* lv_image_gallery_create(
    lv_obj_t* parent,
    const lv_gallery_image_t* images,
    int image_count,
    const char* title,
    const lv_gallery_config_t* config
);

/**
 * @brief Get the default gallery configuration
 *
 * Returns a configuration structure with sensible defaults that work
 * with any language and theme.
 *
 * @return lv_gallery_config_t structure with default values
 */
lv_gallery_config_t lv_image_gallery_get_default_config(void);

/**
 * @brief Navigate to a specific image in the gallery
 * 
 * @param gallery Gallery object returned by lv_image_gallery_create()
 * @param index Zero-based index of image to display
 * 
 * @return true if successful, false if index is out of range
 */
bool lv_image_gallery_set_index(lv_obj_t* gallery, int index);

/**
 * @brief Get the current image index
 * 
 * @param gallery Gallery object returned by lv_image_gallery_create()
 * 
 * @return Current zero-based image index, or -1 on error
 */
int lv_image_gallery_get_index(lv_obj_t* gallery);

/**
 * @brief Navigate to the next image
 * 
 * Wraps around to the first image if at the end.
 * 
 * @param gallery Gallery object returned by lv_image_gallery_create()
 */
void lv_image_gallery_next(lv_obj_t* gallery);

/**
 * @brief Navigate to the previous image
 * 
 * Wraps around to the last image if at the beginning.
 * 
 * @param gallery Gallery object returned by lv_image_gallery_create()
 */
void lv_image_gallery_prev(lv_obj_t* gallery);

#ifdef __cplusplus
}
#endif

#endif // LV_IMAGE_GALLERY_H
