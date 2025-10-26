# Widget Design Overview

This folder contains design documentation for the custom LVGL widgets in this project.

## Widgets

- **[expandable_card.md](expandable_card.md)** - Article cards with expand/collapse
- **[pull_refresh.md](pull_refresh.md)** - Pull-to-refresh container
- **[image_gallery.md](image_gallery.md)** - Image carousel with navigation

## Common Design Patterns

All widgets follow these patterns:

### Memory Management
- Store internal state in widget user data
- Register `LV_EVENT_DELETE` cleanup callbacks
- Reference external data (don't copy)

### Theme Integration
- Use `lv_theme_get_color_*()` functions
- Reference global styles from `ui_helpers.h`
- Update colors when theme changes

### Hebrew Support
- UTF-8 aware text handling
- RTL layout configuration
- Hebrew text defaults in `hebrew_widget_config.h`

### Usage
Each widget has Hebrew configuration helpers:
```cpp
config = hebrew_get_widget_config();
widget = lv_widget_create(parent, data, &config);
```