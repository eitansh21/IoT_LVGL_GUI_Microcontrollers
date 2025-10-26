# Pull-to-Refresh Widget

## Purpose
Scrollable container with pull-down refresh capability. Triggers callbacks when user pulls beyond threshold.

## Key Features
- Gesture-based refresh activation
- Visual feedback during pull
- Configurable pull threshold
- Async operation support

## How It Works

### Scroll Detection
```cpp
// Monitors scroll events to detect upward pulls
static void scroll_event_cb(lv_event_t* e) {
    lv_coord_t scroll_y = lv_obj_get_scroll_y(container);
    if (scroll_y < 0) {  // Pulled beyond top
        int pull_distance = abs(scroll_y);
        // Trigger refresh if threshold reached
    }
}
```

### Refresh Flow
1. User pulls container upward
2. Visual feedback shows pull distance
3. Callback fired when threshold exceeded
4. `lv_pull_refresh_complete()` resets state

### Usage Pattern
```cpp
void my_refresh_callback(lv_obj_t* container, void* user_data) {
    update_content();
    lv_pull_refresh_complete(container);  // Reset UI
}

lv_pull_refresh_config_t config = hebrew_get_pull_refresh_config(my_refresh_callback, NULL);
lv_obj_t* container = lv_pull_refresh_create(tab, &config);
```

## Real Usage
- **pull_refresh_tab.cpp:77** - Updates random Hebrew text on pull
- Container fills entire tab space
- Uses timer to simulate async completion