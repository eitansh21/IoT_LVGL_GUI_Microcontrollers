# Theme Awareness and Dark Mode

## The Problem

Widgets in this project need to respond to dark/light mode changes automatically. Without proper theme awareness, colors become inconsistent when switching modes.

## How It Works Here

### 1. Centralized Theme Manager

The project uses `theme_manager.h` with these key functions:
```cpp
theme_manager_init(disp);           // Initialize with display
theme_manager_toggle_mode();        // Switch dark/light
theme_manager_is_dark_mode();       // Check current state
```

### 2. Getting Theme Colors

**Always use LVGL's theme API:**
```cpp
// ✅ Correct - updates automatically
lv_color_t primary = lv_theme_get_color_primary(obj);

// ❌ Wrong - stays the same color
lv_color_t primary = lv_color_hex(0x1976D2);
```

### 3. Global Styles Pattern

The project creates global styles that get updated on theme changes:
```cpp
// In ui_helpers.cpp
static lv_style_t title_style;
static bool title_style_initialized = false;

// Initialize once
if (!title_style_initialized) {
    lv_style_init(&title_style);
    lv_style_set_text_color(&title_style, lv_theme_get_color_primary(obj));
    title_style_initialized = true;
}

// Update when theme changes
void ui_update_title_style(lv_obj_t *ref_obj) {
    if (title_style_initialized) {
        lv_style_set_text_color(&title_style, lv_theme_get_color_primary(ref_obj));
    }
}
```

### 4. Widget Integration

Widgets reference global styles instead of creating their own:
```cpp
// Widget gets reference to global style
config.title_style = ui_get_title_style();
config.button_style = ui_get_button_style();
```

## Implementing Dark Mode Switch

### Settings Modal Pattern

```cpp
// Event handler
static void dark_mode_switch_event_cb(lv_event_t *e) {
    if (g_theme_toggle_cb) {
        g_theme_toggle_cb();  // Calls theme_manager_toggle_mode()
        lv_obj_invalidate(lv_event_get_target(e));
    }
}

// Switch creation
create_setting_row_with_switch(content,
                               "מצב לילה",
                               "הפעל ערכת נושא כהה",
                               theme_manager_is_dark_mode(),  // Current state
                               dark_mode_switch_event_cb);
```

## Common Issues

**Widgets don't update colors:** Check that all custom styles are updated in `theme_manager_apply_theme()`

**Inconsistent colors:** Some widget is using hardcoded colors instead of `lv_theme_get_color_primary()`

**Performance issues:** Creating new styles instead of reusing global ones