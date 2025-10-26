# LVGL Custom Widgets Library

A collection of theme-aware, language-agnostic LVGL widgets for embedded applications.

## Features

- **Theme-Aware**: Automatically adapts to LVGL light/dark themes
- **Language-Agnostic**: Works with any language and text direction
- **Performance-Optimized**: Configurable scroll behavior for ESP32
- **Flexible Configuration**: Easy customization through config structs
- **Production-Ready**: Used in Hebrew LVGL applications

## Widgets Included

### Expandable Card (`lv_expandable_card`)
Interactive cards with collapsible content, suitable for articles or detailed information.

### Pull Refresh (`lv_pull_refresh`)
Pull-to-refresh container with customizable callbacks and visual feedback.

### Image Gallery (`lv_image_gallery`)
Simple image gallery with navigation controls and image counter.

## Quick Start

```c
#include "widget_common.h"
#include "lv_expandable_card.h"

// Create with defaults (English/LTR)
lv_card_config_t config = lv_expandable_card_get_default_config();
lv_obj_t* card = lv_expandable_card_create(parent, &card_data, &config);

// Theme-aware styling is automatic
```

## Dependencies

- LVGL 9.3.0+
- ESP32 Arduino/ESP-IDF framework

## Architecture

- `widget_common.h/c` - Shared theme-aware infrastructure
- `lv_*.h/c` - Individual widget implementations
- All widgets respect LVGL single-threaded architecture

## Thread Safety

**Important**: All widgets must be used on the main thread where `lv_timer_handler()` runs, following LVGL's architecture requirements.