# LVGL Callbacks and Events

## The Problem

LVGL is event-driven. You register callback functions that get called when things happen. Understanding the patterns from this project helps avoid common mistakes.

## Basic Pattern

```cpp
// 1. Define callback
static void button_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    void *user_data = lv_event_get_user_data(e);

    ESP_LOGI("UI", "Button clicked");
}

// 2. Register it
lv_obj_add_event_cb(my_button, button_event_cb, LV_EVENT_CLICKED, user_data);
```

## Real Examples from This Project

### Settings Button (stops event propagation)
```cpp
static void settings_btn_event_cb(lv_event_t *e) {
    lv_event_stop_processing(e);  // Don't let tabs handle this click

    lv_obj_t *screen = lv_screen_active();
    create_settings_modal(screen, toggle_theme_internal);
}
```

### Brightness Slider (uses user data)
```cpp
static void brightness_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);  // Label to update

    int value = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d%%", value);

    ledcWrite(PWM_CHANNEL, (value * 255) / 100);
}

// Registration passes label as user data
lv_obj_add_event_cb(slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, brightness_label);
```

### Memory Cleanup (automatic)
```cpp
static void cleanup_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    widget_data_t *data = lv_obj_get_user_data(obj);

    if (data) {
        if (data->cached_text) free(data->cached_text);
        free(data);
    }
}

// Register cleanup when creating widget
lv_obj_add_event_cb(widget, cleanup_event_cb, LV_EVENT_DELETE, NULL);
```

## Passing Multiple Data

```cpp
typedef struct {
    lv_obj_t* target_label;
    int* value_storage;
} slider_context_t;

slider_context_t* ctx = malloc(sizeof(slider_context_t));
ctx->target_label = label;
ctx->value_storage = &global_brightness;

lv_obj_add_event_cb(slider, slider_cb, LV_EVENT_VALUE_CHANGED, ctx);
```

## Common Event Types

```cpp
LV_EVENT_CLICKED        // Button clicks
LV_EVENT_VALUE_CHANGED  // Sliders, switches
LV_EVENT_DELETE         // Object being deleted (for cleanup)
LV_EVENT_SCROLL         // Scrolling events
```

## Common Mistakes

**Memory leaks:** Forgetting `LV_EVENT_DELETE` cleanup for malloc'd data

**Event propagation:** Modal buttons should use `lv_event_stop_processing(e)`

**Null pointers:** Always check target and user_data aren't NULL