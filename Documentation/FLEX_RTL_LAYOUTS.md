# Flex Layouts with Hebrew RTL

## The Problem

Hebrew interfaces need RTL (right-to-left) layout, but LVGL's flex system can be confusing when visual order doesn't match logical order.

## Key Insight

**In RTL flex rows, first child appears on the RIGHT, second on LEFT.**

```cpp
// Hebrew settings row: [Switch] ←space→ [Text]
lv_obj_set_style_base_dir(row, LV_BASE_DIR_RTL, 0);
lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

// Add in logical order:
lv_obj_t *text = lv_label_create(row);    // First → appears on RIGHT
lv_obj_t *switch = lv_switch_create(row); // Second → appears on LEFT
```

## Common Patterns from This Project

### Tab Container (Column)
```cpp
lv_obj_t* ui_create_tab_container(lv_obj_t *parent, int padding) {
    lv_obj_t *container = lv_obj_create(parent);

    lv_obj_set_style_base_dir(container, LV_BASE_DIR_RTL, 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container,
        LV_FLEX_ALIGN_START,    // Top to bottom
        LV_FLEX_ALIGN_END,      // Align to right (RTL)
        LV_FLEX_ALIGN_START
    );
}
```

### Settings Row (Row with spacing)
```cpp
lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
lv_obj_set_flex_align(row,
    LV_FLEX_ALIGN_SPACE_BETWEEN,  // Text and switch at opposite ends
    LV_FLEX_ALIGN_CENTER,         // Center vertically
    LV_FLEX_ALIGN_START
);
```

### Header (Title + Close Button)
```cpp
// Want: [×] ←space→ [הגדרות]
lv_obj_set_style_base_dir(header, LV_BASE_DIR_RTL, 0);
lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);

lv_obj_t *title = lv_label_create(header);  // First → right side
lv_obj_t *close = lv_btn_create(header);    // Second → left side
```

## Flex Alignment Quick Reference

### Main Axis (direction of flow)
```cpp
LV_FLEX_ALIGN_START          // Beginning
LV_FLEX_ALIGN_END            // End
LV_FLEX_ALIGN_CENTER         // Center
LV_FLEX_ALIGN_SPACE_BETWEEN  // Spread apart
LV_FLEX_ALIGN_SPACE_EVENLY   // Equal spacing
```

### Cross Axis (perpendicular)
```cpp
LV_FLEX_ALIGN_START    // Top (column) / Left (row)
LV_FLEX_ALIGN_END      // Bottom (column) / Right (row)
LV_FLEX_ALIGN_CENTER   // Middle
LV_FLEX_ALIGN_STRETCH  // Fill space
```

### Advanced Pattern: Settings Row
```cpp
// Want: [Text Container] ←space→ [Switch]
lv_obj_set_style_base_dir(row, LV_BASE_DIR_RTL, 0);
lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
lv_obj_set_flex_align(row,
    LV_FLEX_ALIGN_SPACE_BETWEEN,  // Spread items apart
    LV_FLEX_ALIGN_CENTER,         // Center vertically
    LV_FLEX_ALIGN_START
);

// Add in logical order:
lv_obj_t *text = lv_label_create(row);    // First → appears on RIGHT
lv_obj_t *switch = lv_switch_create(row); // Second → appears on LEFT
```

## Flex Alignment Complete Reference

### Main Axis (flow direction)
```cpp
LV_FLEX_ALIGN_START          // Beginning of flow
LV_FLEX_ALIGN_END            // End of flow
LV_FLEX_ALIGN_CENTER         // Center
LV_FLEX_ALIGN_SPACE_BETWEEN  // Items spread apart
LV_FLEX_ALIGN_SPACE_EVENLY   // Equal spacing including edges
LV_FLEX_ALIGN_SPACE_AROUND   // Equal space around items
```

### Cross Axis (perpendicular to flow)
```cpp
LV_FLEX_ALIGN_START    // Top (column) / Start side (row)
LV_FLEX_ALIGN_END      // Bottom (column) / End side (row)
LV_FLEX_ALIGN_CENTER   // Middle
LV_FLEX_ALIGN_STRETCH  // Fill available space
```

## Common Mistakes

**Visual vs Logical thinking:** Don't think "put switch on left" - think "switch comes second in RTL row"

**Forgetting base direction:** RTL only works if you set `LV_BASE_DIR_RTL` on the container

**Wrong alignment:** Use `LV_FLEX_ALIGN_END` for right-alignment in RTL, not manual positioning