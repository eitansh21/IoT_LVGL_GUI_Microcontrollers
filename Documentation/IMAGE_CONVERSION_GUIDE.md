# Image Conversion and Integration Guide

## Overview

This guide explains how to convert images to C arrays for LVGL and integrate them efficiently into your ESP32 project.

## Image Preparation

### 1. Optimize Your Images First

```bash
# Resize to appropriate dimensions
convert original.jpg -resize 128x128 optimized.jpg

# Reduce quality for smaller file size
convert original.jpg -resize 128x128 -quality 85 optimized.jpg
```

**Recommended sizes for ESP32:**
- Small icons: 32x32 or 48x48
- Medium images: 64x64 or 96x96
- Large images: 128x128 (max recommended)

## Image Conversion

### Using LVGL Image Converter (Recommended)

**URL:** https://lvgl.io/tools/imageconverter

**Steps:**
1. Upload your optimized image
2. Select color format:
   - **RGB565:** Good balance of quality/memory (recommended)
   - **True color:** Best quality, most memory
3. Select output format: **C Array**
4. Download the generated `.c` file

**Generated file example:**
```c
#include "lvgl.h"

const LV_ATTRIBUTE_MEM_ALIGN lv_img_dsc_t my_image = {
  .header.cf = LV_IMG_CF_RGB565,
  .header.w = 128,
  .header.h = 128,
  .data_size = 32768,
  .data = my_image_map,
};

const LV_ATTRIBUTE_MEM_ALIGN uint8_t my_image_map[] = {
  // Image data bytes...
};
```

## Project Integration

### 1. File Organization

```
src/
├── images/
│   ├── settings_icon.c
│   ├── home_icon.c
│   └── gallery_image1.c
└── images.h
```

### 2. Header File

**File: `include/images.h`**
```c
#ifndef IMAGES_H
#define IMAGES_H

#include <lvgl.h>

LV_IMG_DECLARE(settings_icon);
LV_IMG_DECLARE(home_icon);
LV_IMG_DECLARE(gallery_image1);

#endif // IMAGES_H
```

### 3. Usage in Code

```cpp
#include "images.h"

void create_image_display(lv_obj_t* parent) {
    lv_obj_t* img = lv_img_create(parent);
    lv_img_set_src(img, &gallery_image1);
    lv_obj_center(img);
}

void create_icon_button(lv_obj_t* parent) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_t* icon = lv_img_create(btn);
    lv_img_set_src(icon, &settings_icon);
    lv_obj_center(icon);
}
```

## Memory Optimization

### 1. Store Images in Flash Memory

**Reduce RAM usage:**
```c
// In your image .c file, add __attribute__((section(".rodata")))
const LV_ATTRIBUTE_MEM_ALIGN lv_img_dsc_t settings_icon __attribute__((section(".rodata"))) = {
    .header.cf = LV_IMG_CF_RGB565,
    .header.w = 32,
    .header.h = 32,
    .data_size = 2048,
    .data = settings_icon_map,
};

const LV_ATTRIBUTE_MEM_ALIGN uint8_t settings_icon_map[] __attribute__((section(".rodata"))) = {
    // Image data...
};
```

### 2. Choose Appropriate Color Format

| Format | Memory Usage | Quality | Use Case |
|--------|-------------|---------|----------|
| `LV_IMG_CF_RGB565` | 2 bytes/pixel | Good | **Recommended** |
| `LV_IMG_CF_TRUE_COLOR` | 3 bytes/pixel | Best | High-quality images |
| `LV_IMG_CF_RGB332` | 1 byte/pixel | Poor | Simple icons only |

### 3. Memory Usage Calculation

```cpp
// RGB565: 128x128 image = 128 * 128 * 2 = 32KB
// True color: 128x128 image = 128 * 128 * 3 = 48KB
```

## Example: Gallery Implementation

### Gallery Widget from Project

```cpp
// From the project's image gallery
void create_gallery_tab(lv_obj_t *tab) {
    lv_obj_t *container = ui_create_tab_container(tab, 15);

    // Create image gallery widget
    lv_gallery_config_t config = lv_gallery_get_default_config();
    config.style.base_dir = LV_BASE_DIR_RTL;

    const lv_img_dsc_t* images[] = {
        &gallery_image1,
        &gallery_image2,
        &gallery_image3
    };

    lv_obj_t* gallery = lv_gallery_create(container, images, 3, &config);
}
```

## Best Practices

### 1. Image Sizing Guidelines

```cpp
// ✅ Good - reasonable sizes
32x32   // Small icons (2KB in RGB565)
64x64   // Medium icons (8KB in RGB565)
128x128 // Large images (32KB in RGB565)

// ❌ Avoid - too large for ESP32
256x256 // 128KB in RGB565!
512x512 // 512KB in RGB565!
```

### 2. Memory Monitoring

```cpp
void check_image_memory(void) {
    uint32_t free_heap = ESP.getFreeHeap();
    ESP_LOGI(TAG, "Free heap: %lu KB", free_heap / 1024);

    // Each 128x128 RGB565 image uses ~32KB
    // Plan accordingly!
}
```

### 3. Dynamic Loading (Optional)

```cpp
// Only load images when needed
const lv_img_dsc_t* get_gallery_image(int index) {
    static const lv_img_dsc_t* images[] = {
        &gallery_image1,
        &gallery_image2,
        &gallery_image3
    };

    if (index >= 0 && index < sizeof(images)/sizeof(images[0])) {
        return images[index];
    }
    return NULL;
}
```

## Troubleshooting

### Issue: Image Not Displaying
- ✅ Check `LV_IMG_DECLARE()` in header
- ✅ Verify `.c` file is compiled
- ✅ Use correct variable name

### Issue: Out of Memory
- ✅ Use RGB565 instead of True Color
- ✅ Add `__attribute__((section(".rodata")))`
- ✅ Reduce image dimensions
- ✅ Load images dynamically

### Issue: Poor Image Quality
- ✅ Use higher quality source image
- ✅ Use True Color format (if memory allows)
- ✅ Avoid over-compression during optimization

## Summary

1. **Optimize images** before conversion (resize, compress)
2. **Use LVGL Image Converter** at https://lvgl.io/tools/imageconverter
3. **Choose RGB565** for best balance of quality/memory
4. **Store in flash** with `__attribute__((section(".rodata")))`
5. **Monitor memory usage** - each 128x128 RGB565 image = ~32KB
6. **Keep images reasonably sized** for ESP32 constraints