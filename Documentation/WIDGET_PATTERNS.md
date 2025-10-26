# Widget Patterns

## The Problem

Creating consistent, theme-aware widgets that work with Hebrew RTL and handle memory properly.

## Configuration Pattern

All widgets in this project use this structure:

```cpp
typedef struct {
    widget_style_t style;        // Common styling (padding, RTL, etc.)
    lv_style_t* title_style;     // Reference to global theme style
    lv_style_t* button_style;    // Reference to global theme style

    // Widget-specific config
    int max_chars;
    const char* truncate_suffix;

    // Callbacks
    void (*on_expand)(lv_obj_t* widget);
    void* user_data;
} lv_widget_config_t;
```

## Memory Management Pattern

```cpp
// 1. Create widget with internal state
typedef struct {
    lv_widget_config_t config;
    bool expanded;
    char* cached_text;           // Dynamically allocated
} widget_internal_data_t;

// 2. Store in widget user data
widget_internal_data_t* data = malloc(sizeof(widget_internal_data_t));
lv_obj_set_user_data(widget, data);

// 3. Register automatic cleanup
static void cleanup_cb(lv_event_t* e) {
    widget_internal_data_t* data = lv_obj_get_user_data(lv_event_get_target(e));
    if (data) {
        if (data->cached_text) free(data->cached_text);
        free(data);
    }
}

lv_obj_add_event_cb(widget, cleanup_cb, LV_EVENT_DELETE, NULL);
```

## Theme Integration Pattern

```cpp
// Widget gets references to global styles (don't create new ones)
lv_widget_config_t config = get_default_config();
config.title_style = ui_get_title_style();     // Global style reference
config.button_style = ui_get_button_style();   // Global style reference

// Apply styles
if (config.title_style) {
    lv_obj_add_style(title_label, config.title_style, 0);
}
```

## Example: Expandable Card Creation

```cpp
lv_obj_t* lv_card_create(lv_obj_t* parent, const lv_card_data_t* data, const lv_card_config_t* cfg) {
    // Validate inputs
    if (!parent || !data) return NULL;

    // Use default config if none provided
    lv_card_config_t config = cfg ? *cfg : lv_card_get_default_config();

    // Create internal state
    lv_card_internal_data_t* internal = malloc(sizeof(lv_card_internal_data_t));
    internal->card_data = data;  // Reference, not copy
    internal->config = config;   // Copy config
    internal->expanded = false;
    internal->cached_text = NULL;

    // Create container
    lv_obj_t* container = lv_obj_create(parent);
    lv_obj_set_user_data(container, internal);

    // Register cleanup
    lv_obj_add_event_cb(container, cleanup_cb, LV_EVENT_DELETE, NULL);

    // Apply styling and create children...

    return container;
}
```

## Safe Data Access

```cpp
static widget_internal_data_t* get_widget_data(lv_obj_t* widget) {
    if (!widget) {
        ESP_LOGW(TAG, "Widget is NULL");
        return NULL;
    }

    widget_internal_data_t* data = lv_obj_get_user_data(widget);
    if (!data) {
        ESP_LOGW(TAG, "Widget data is NULL");
        return NULL;
    }

    return data;
}

// Use in event handlers
static void button_event_cb(lv_event_t* e) {
    lv_obj_t* widget = lv_event_get_user_data(e);
    widget_internal_data_t* data = get_widget_data(widget);
    if (!data) return;

    // Safe to use data now
}
```

## Common Mistakes

**Memory leaks:** Not registering `LV_EVENT_DELETE` cleanup callback

**Theme inconsistency:** Creating new styles instead of referencing global ones

**Null pointers:** Not validating widget data before use

**UTF-8 issues:** Using `strlen()` instead of `utf8_char_count()` for Hebrew text