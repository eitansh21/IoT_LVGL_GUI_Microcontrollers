# Image Gallery Widget

## Purpose
Carousel-style image navigation with Previous/Next buttons and image counter. Supports custom border colors per image.

## Key Features
- Button-based navigation (no swipe)
- Automatic sizing based on largest image
- Custom border colors per image
- RTL layout support

## How It Works

### Navigation Logic
```cpp
// Wraps around at boundaries
if (is_next) {
    data->current_index = (data->current_index + 1) % data->image_count;
} else {
    data->current_index = (data->current_index - 1 + data->image_count) % data->image_count;
}
```

### Image Display
- Updates image source, label text, and border color
- Shows "current / total" counter
- Automatically sizes container to fit largest image

### Usage Pattern
```cpp
// Images must be declared and data must be static
LV_IMG_DECLARE(image_1);
static lv_gallery_image_t gallery_images[] = {
    {&image_1, "תמונה ראשונה", 0x2196F3},  // Blue border
};

lv_gallery_config_t config = hebrew_get_image_gallery_config();
lv_obj_t* gallery = lv_image_gallery_create(tab, gallery_images, 1, "גלריה", &config);
```

## Real Usage
- **gallery_tab.cpp:27** - Shows 3 images with Hebrew labels
- Each image has different border color
- Uses Hebrew RTL configuration