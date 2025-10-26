#ifndef LVGL_SETUP_HPP
#define LVGL_SETUP_HPP

#include <lvgl.h>

#define DRAW_BUF_SIZE (320 * 480 / 5 * (LV_COLOR_DEPTH / 8))  // Increased from /10 to /5 for better FPS
#define LV_TICK_PERIOD_MS 5    // Optimized from 2ms to 5ms for better performance
#define TASK_SLEEP_PERIOD_MS 5 // Optimized from 10ms to 5ms for better responsivity

// Initialize LVGL components
void init_lvgl_display();
void init_lvgl_input_device();
void init_lvgl_timer();

// Touch callback
void touch_read_callback(lv_indev_t *indev_driver, lv_indev_data_t *data);

// Get global LVGL objects
extern lv_display_t *disp;
extern lv_indev_t *indev;

#endif // LVGL_SETUP_HPP
