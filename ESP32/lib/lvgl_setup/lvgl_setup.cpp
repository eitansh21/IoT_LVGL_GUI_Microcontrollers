#include "lvgl_setup.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "display.hpp"

static const char* TAG = "LVGL";

lv_display_t *disp;
lv_indev_t *indev;
static lv_color_t *draw_buf1;
static lv_color_t *draw_buf2;

// LVGL tick task callback
void lv_tick_task(void *arg) {
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

// LovyanGFX display flush callback
void lovyangfx_flush_cb(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.writePixels((lgfx::rgb565_t*)px_map, w * h);
    gfx.endWrite();

    lv_display_flush_ready(disp_drv);
}

// LVGL touch input callback
void touch_read_callback(lv_indev_t *indev_driver, lv_indev_data_t *data) {
    static bool was_pressed = false;
    uint16_t touch_x, touch_y;
    bool touched = gfx.getTouch(&touch_x, &touch_y);

    if (!touched) {
        if (was_pressed) {
            ESP_LOGI(TAG, "Touch RELEASED");
            was_pressed = false;
        }
        data->state = LV_INDEV_STATE_REL;
    } else {
        if (!was_pressed) {
            ESP_LOGI(TAG, "Touch PRESSED at x=%d, y=%d (settings area: 10-55, 430-475)", touch_x, touch_y);
            was_pressed = true;
        }
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_x;
        data->point.y = touch_y;
    }
}

void init_lvgl_display() {
    ESP_LOGI(TAG, "Initializing LVGL display...");
    lv_init();

    // Create display buffer for LovyanGFX
    uint32_t buf_size = TFT_HOR_RES * TFT_VER_RES / 10;  // 1/10 screen buffer
    draw_buf1 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    draw_buf2 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);

    if (!draw_buf1 || !draw_buf2) {
        ESP_LOGE(TAG, "Failed to allocate display buffers");
        return;
    }

    // Create LVGL display
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, lovyangfx_flush_cb);
    lv_display_set_buffers(disp, draw_buf1, draw_buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);  // Portrait mode

    ESP_LOGI(TAG, "LVGL display created with LovyanGFX integration");
}

void init_lvgl_input_device() {
    ESP_LOGI(TAG, "Initializing LVGL input device...");
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_callback);
    lv_indev_enable(indev, true);
    ESP_LOGI(TAG, "LVGL input device created and enabled");
}

void init_lvgl_timer() {
    ESP_LOGI(TAG, "Initializing LVGL timer...");
    // Create and start a periodic timer for LVGL tick
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    ESP_LOGI(TAG, "LVGL timer started");
}
