# Expandable Card Widget

## Purpose
Displays article content with expand/collapse functionality. Automatically truncates long Hebrew text when collapsed, with optional scrollable content when expanded.

## Key Features
- UTF-8 aware text truncation
- Max height with scrolling for long content
- Theme-aware styling
- RTL/LTR layout support
- Memory-efficient caching
- Auto-scroll to view when collapsed

## How It Works

### Two Display Modes

**Collapsed Mode:**
- Shows truncated text with suffix (e.g., "...")
- Fixed height based on content
- No scrolling

**Expanded Mode:**
- Shows full content
- If `max_content_height` set: enables vertical scrolling
- If no max height: grows to fit all content

### Content Container Logic
```cpp
if (data->expanded) {
    if (data->config.max_content_height > 0) {
        lv_obj_set_height(data->content_container, data->config.max_content_height);
        lv_obj_add_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scroll_dir(data->content_container, LV_DIR_VER);
    } else {
        lv_obj_set_height(data->content_container, LV_SIZE_CONTENT);
        lv_obj_clear_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);
    }
} else {
    // Collapsed: always content size, no scrolling
    lv_obj_set_height(data->content_container, LV_SIZE_CONTENT);
    lv_obj_clear_flag(data->content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_scroll_to(data->content_container, 0, 0, LV_ANIM_OFF);  // Reset scroll
}
```

### UTF-8 Text Truncation
```cpp
static char* create_truncated_text(const char* full_text, int max_chars, const char* suffix) {
    int full_char_count = utf8_char_count(full_text);  // Character count, not bytes
    int truncate_char_count = max_chars - utf8_char_count(suffix);
    int truncate_byte_pos = utf8_char_to_byte_pos(full_text, truncate_char_count);
    // Safe UTF-8 boundary truncation
}
```

### Smart Scrolling
- When expanding: shows content where it is
- When collapsing: `lv_obj_scroll_to_view(card, LV_ANIM_ON)` ensures card stays visible

### State Management
- Caches truncated text (created once, reused)
- Stores expansion state in widget user data
- Automatic cleanup on widget deletion

### Usage Pattern
```cpp
// Data must be static/global (widget stores reference)
static lv_card_data_t article = {
    .title = "כותרת המאמר",
    .content = "תוכן ארוך מאוד שיכול להיות ארוך מאוד..."
};

lv_card_config_t config = hebrew_get_expandable_card_config();
config.max_content_height = 200;  // Enable scrolling for long content
config.truncate_length = 80;      // Show 80 characters when collapsed

lv_obj_t* card = lv_expandable_card_create(parent, &article, &config);
```

## Real Usage
- **news_tab.cpp:76** - Creates Hebrew article cards with auto-truncation
- English card uses `max_content_height = 200` to demonstrate scrolling
- Hebrew cards use unlimited height to show full content
- Demonstrates both Hebrew RTL and English LTR modes in same tab