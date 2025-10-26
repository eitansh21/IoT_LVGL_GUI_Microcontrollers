#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <Arduino.h>
#include "esp_log.h"
#include "lv_conf_hebrew_support.h"
#include "hebrew_tabs.h"
#include "display.hpp"
#include "lvgl_setup.hpp"
#include "hebrew_fonts.h"

static const char* TAG = "MAIN";

// LVGL FPS label (stays on top)
static lv_obj_t* fps_label = NULL;
static bool fps_display_enabled = true;

void create_fps_label() {
    if (fps_label) return; // Already created

    // Create FPS label once during setup
    fps_label = lv_label_create(lv_screen_active());
    lv_obj_align(fps_label, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_text_color(fps_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(fps_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_bg_opa(fps_label, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(fps_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_pad_all(fps_label, 3, 0);
    lv_obj_set_style_radius(fps_label, 3, 0);

    // Make sure it stays on top
    lv_obj_move_to_index(fps_label, -1);

    ESP_LOGI(TAG, "FPS label created");
}

void create_ui() {
    ESP_LOGI(TAG, "Creating Hebrew UI...");
    lv_obj_t *screen = lv_display_get_screen_active(disp);

    // Set RTL direction (background uses theme color automatically)
    lv_obj_set_style_base_dir(screen, LV_BASE_DIR_RTL, 0);

    // Create the Hebrew tabview
    create_hebrew_tabview(screen);

    // Create FPS label
    create_fps_label();

    ESP_LOGI(TAG, "Hebrew UI created successfully");
}

void setup() {
    delay(2000);
    ESP_LOGI(TAG, "Starting LVGL Hebrew demo...");

    init_display();
    init_touch();
    init_lvgl_display();
    init_lvgl_input_device();
    init_lvgl_timer();
    create_ui();

    ESP_LOGI(TAG, "Setup complete");
}

unsigned long last_frame_time = 0;
int frame_count = 0;
float current_FPS = 0.0;


// Function to toggle FPS display (can be called from settings)
void toggle_fps_display() {
    fps_display_enabled = !fps_display_enabled;
    ESP_LOGI(TAG, "FPS display %s", fps_display_enabled ? "enabled" : "disabled");
}

// Function to get current FPS display state
bool is_fps_display_enabled() {
    return fps_display_enabled;
}

// Performance tracking
unsigned long max_render_time = 0;
unsigned long total_render_time = 0;
int render_samples = 0;
unsigned long last_benchmark_reset = 0;

// Responsivity tracking
int frames_under_100ms = 0;    // Good performance
int frames_under_450ms = 0;    // Acceptable performance
int frames_under_550ms = 0;    // Poor but usable
int frames_above_550ms = 0;    // Severe lag
unsigned long lastTouchTime = 0;
bool touchActive = false;

void loop() {

  unsigned long current_time = millis();
  unsigned long elapsedTime = current_time - last_frame_time;

  // Only update FPS every second or so to avoid flickering and excessive calculations
  if (elapsedTime >= 1000) {
    current_FPS = (float)frame_count / (elapsedTime / 1000.0);
    frame_count = 0;
    last_frame_time = current_time;

    // Get LVGL memory info for display
    lv_mem_monitor_t mem_display;
    lv_mem_monitor(&mem_display);
    size_t used_kb = (mem_display.total_size - mem_display.free_size) / 1024;
    size_t total_kb = mem_display.total_size / 1024;

    // Update FPS label if it exists

    if (fps_label) {
      if (fps_display_enabled) {
        char fps_text[64];
        snprintf(fps_text, sizeof(fps_text), "FPS: %.1f | LVGL: %zu/%zuKB", current_FPS, used_kb, total_kb);
        lv_label_set_text(fps_label, fps_text);
        lv_obj_clear_flag(fps_label, LV_OBJ_FLAG_HIDDEN);
      } else {
        lv_obj_add_flag(fps_label, LV_OBJ_FLAG_HIDDEN);
      }
    }
  }

  unsigned long render_start = micros();
  // LVGL handler
  lv_timer_handler();
  unsigned long render_end = micros();

  // Track render performance
  unsigned long render_time = render_end - render_start;
  total_render_time += render_time;
  render_samples++;
  if (render_time > max_render_time) {
    max_render_time = render_time;
  }

  // Track frame performance buckets
  if (render_time < 100000) frames_under_100ms++;        // <100ms (good)
  else if (render_time < 450000) frames_under_450ms++;   // 100-450ms (acceptable)
  else if (render_time < 550000) frames_under_550ms++;   // 450-550ms (poor)
  else frames_above_550ms++;                             // >550ms (severe lag)

  // Log memory usage and performance stats
  if (elapsedTime >= 100) {
    // ESP32 heap info
    uint32_t free_heap = ESP.getFreeHeap();
    uint32_t total_heap = ESP.getHeapSize();
    uint32_t used_heap = total_heap - free_heap;

    // LVGL memory info
    lv_mem_monitor_t mem_mon;
    lv_mem_monitor(&mem_mon);

    // Calculate average render time
    unsigned long avgRenderTime = render_samples > 0 ? total_render_time / render_samples : 0;

    ESP_LOGI(TAG, "Memory - ESP32: %lu/%lu KB (%.1f%%), LVGL: %zu/%zu KB (%d%%, frag: %d%%), Render: %lu us (avg: %lu, max: %lu)",
             used_heap / 1024, total_heap / 1024, (float)used_heap * 100.0 / total_heap,
             (mem_mon.total_size - mem_mon.free_size) / 1024, mem_mon.total_size / 1024,
             mem_mon.used_pct, mem_mon.frag_pct,
             render_time, avgRenderTime, max_render_time);

    // Reset benchmark every 10 seconds
    if (current_time - last_benchmark_reset >= 10000) {
      // Calculate responsivity percentages
      float good = render_samples > 0 ? (float)frames_under_100ms * 100.0 / render_samples : 0;
      float acceptable = render_samples > 0 ? (float)frames_under_450ms * 100.0 / render_samples : 0;
      float poor = render_samples > 0 ? (float)frames_under_550ms * 100.0 / render_samples : 0;
      float severe = render_samples > 0 ? (float)frames_above_550ms * 100.0 / render_samples : 0;

      ESP_LOGI(TAG, "RESPONSIVITY - Good(<100ms): %.1f%% OK(100-450ms): %.1f%% Poor(450-550ms): %.1f%% Severe(>550ms): %.1f%% (Samples: %d)",
               good, acceptable, poor, severe, render_samples);

      max_render_time = 0;
      total_render_time = 0;
      render_samples = 0;
      frames_under_100ms = 0;
      frames_under_450ms = 0;
      frames_under_550ms = 0;
      frames_above_550ms = 0;
      last_benchmark_reset = current_time;
    }
  }

  vTaskDelay(pdMS_TO_TICKS(TASK_SLEEP_PERIOD_MS));
  
  frame_count++;
}

