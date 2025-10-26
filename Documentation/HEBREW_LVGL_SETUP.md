# Hebrew LVGL Setup

## LVGL Configuration

### 1. Enable Hebrew Support

**File: `include/lv_conf_hebrew_support.h`**
```c
#ifdef ENABLE_HEBREW_SUPPORT
#define LV_USE_BIDI 1                          // Essential for Hebrew
#define LV_USE_ARABIC_PERSIAN_CHARS 1          // Related script support
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 1     // Built-in Hebrew font
#endif
```

**File: `platformio.ini`**
```ini
build_flags = -D ENABLE_HEBREW_SUPPORT=1
```

## Font Generation

### 1. Install Font Converter
```bash
npm install -g lv_font_conv
```

### 2. Unicode Ranges for Hebrew

| Range | Description | Characters |
|-------|-------------|------------|
| `0x20-0x7F` | Basic Latin | Space, A-Z, 0-9, punctuation |
| `0x5D0-0x5EA` | Hebrew Letters | א ב ג ד ... ת |
| `0x0591-0x05BD` | Hebrew Accents | ◌ַ ◌ָ ◌ֶ ◌ֵ ◌ִ (niqqud) |
| `0x05BF-0x05C7` | Hebrew Points | ◌ֿ ◌ׁ ◌ׂ ◌ׄ (additional niqqud) |

### 3. Generate Hebrew Fonts

**Basic Hebrew Font:**
```bash
lv_font_conv --size 16 --bpp 4 --format lvgl \
  --font OpenSans-VariableFont_wdth,wght.ttf \
  -r 0x20-0x7F -r 0x5d0-0x5ea \
  --no-compress --lv-include lvgl.h \
  -o opensans_hebrew_16.c
```

**Hebrew Font with Niqqud:**
```bash
lv_font_conv --size 16 --bpp 4 --format lvgl \
  --font OpenSans-VariableFont_wdth,wght.ttf \
  -r 0x20-0x7F -r 0x5d0-0x5ea -r 0x0591-0x05BD -r 0x05BF-0x05C7 \
  --no-compress --lv-include lvgl.h \
  -o opensans_hebrew_16_niqqud.c
```

**Parameters:**
- `--size 16`: Font size in pixels
- `--bpp 4`: 4 bits per pixel for anti-aliasing
- `--no-compress`: Required for Hebrew fonts
- `-r`: Unicode range specification

## Font Integration

### 1. Font Declaration

**File: `include/hebrew_fonts.h`**
```c
#ifndef HEBREW_FONTS_H
#define HEBREW_FONTS_H

#include <lvgl.h>

LV_FONT_DECLARE(opensans_hebrew_10);
LV_FONT_DECLARE(opensans_hebrew_12);
LV_FONT_DECLARE(opensans_hebrew_16);

#endif
```

### 2. Using Fonts in Code

```cpp
#include "hebrew_fonts.h"

// Set as default font
#undef LV_FONT_DEFAULT
#define LV_FONT_DEFAULT &opensans_hebrew_16

// Apply to specific objects
lv_obj_set_style_text_font(label, &opensans_hebrew_16, 0);
```

## RTL and BIDI Setup

### 1. Understanding the Difference

**BIDI (Bidirectional Text):** Character reordering within text
- Example: "Hello שלום" becomes "שלום Hello"
- Handled automatically by LVGL when enabled

**RTL (Right-to-Left Layout):** Container and layout direction
- Affects flex child ordering and alignment
- Must be set explicitly on containers

### 2. Setting Text Direction

```cpp
// Automatic BIDI detection (recommended)
lv_obj_set_style_base_dir(label, LV_BASE_DIR_AUTO, 0);

// Force Hebrew RTL
lv_obj_set_style_base_dir(label, LV_BASE_DIR_RTL, 0);
lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);

// Force English LTR
lv_obj_set_style_base_dir(label, LV_BASE_DIR_LTR, 0);
lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
```

### 3. Setting Layout Direction

```cpp
// Hebrew container layout
lv_obj_set_style_base_dir(container, LV_BASE_DIR_RTL, 0);
lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(container,
    LV_FLEX_ALIGN_START,    // Top to bottom
    LV_FLEX_ALIGN_END,      // Align to right (RTL)
    LV_FLEX_ALIGN_START
);
```

## Common Issues and Solutions

### 1. Hebrew Text Shows as Rectangles
**Cause:** Font doesn't include Hebrew characters
**Solution:** Regenerate font with `-r 0x5d0-0x5ea` range

### 2. Symbols Don't Work with Hebrew Fonts
**Problem:** Hebrew fonts don't include `LV_SYMBOL_*` characters
**Solution:** Use separate fonts
```cpp
// Hebrew text
lv_obj_set_style_text_font(text_label, &opensans_hebrew_16, 0);
lv_label_set_text(text_label, "הגדרות");

// Symbol
lv_obj_set_style_text_font(symbol_label, &lv_font_montserrat_14, 0);
lv_label_set_text(symbol_label, LV_SYMBOL_SETTINGS);
```

### 3. Layout Appears Backwards
**Cause:** Not setting RTL base direction
**Solution:** Set `LV_BASE_DIR_RTL` on containers

### 4. Text Direction Wrong
**Cause:** BIDI not enabled or wrong base direction
**Solution:**
- Enable `LV_USE_BIDI 1` in lv_conf.h
- Use `LV_BASE_DIR_AUTO` for automatic detection

## Helper Functions from This Project

### 1. Standard Hebrew Container
```cpp
lv_obj_t* ui_create_tab_container(lv_obj_t *parent, int padding) {
    lv_obj_t *container = lv_obj_create(parent);

    // Apply Hebrew RTL styling
    lv_obj_set_style_base_dir(container, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container,
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START);

    return container;
}
```

### 2. Standard Hebrew Title
```cpp
lv_obj_t* ui_create_title_label(lv_obj_t *parent, const char *text) {
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, text);

    lv_obj_set_style_base_dir(title, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_font(title, &opensans_hebrew_16, 0);

    return title;
}
```

### 3. Apply Hebrew Text Style
```cpp
void ui_apply_hebrew_text_style(lv_obj_t *label, const lv_font_t *font, bool rtl_mode) {
    lv_obj_set_style_text_font(label, font, 0);

    if (rtl_mode) {
        lv_obj_set_style_base_dir(label, LV_BASE_DIR_RTL, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    } else {
        lv_obj_set_style_base_dir(label, LV_BASE_DIR_LTR, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    }
}
```

## Testing Your Setup

Test with various Hebrew content:
```cpp
const char* test_strings[] = {
    "שלום עולם",                    // Simple Hebrew
    "Hello שלום World",             // Mixed Hebrew/English
    "מספר 123 בעברית",              // Hebrew with numbers
    "וַתָּשַׁר דְּבוֹרָה",            // Biblical Hebrew with niqqud
};
```
