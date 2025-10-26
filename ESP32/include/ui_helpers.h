/**
 * @file ui_helpers.h
 * @brief Common UI helper functions to eliminate code repetition
 */

#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a standard Hebrew tab container with common styling
 *
 * This eliminates the repetitive container setup code that appears
 * in every tab file.
 *
 * @param parent Parent tab object
 * @param padding Padding for the container (use 15 for standard)
 * @return lv_obj_t* Created container, ready for content
 */
lv_obj_t* ui_create_tab_container(lv_obj_t *parent, int padding);

/**
 * @brief Update title style with current theme colors
 *
 * @param ref_obj Reference object for theme color lookup
 */
void ui_update_title_style(lv_obj_t *ref_obj);

/**
 * @brief Get the global title style for use in widgets
 *
 * @return lv_style_t* Pointer to the global title style
 */
lv_style_t* ui_get_title_style(void);

/**
 * @brief Update button style with current theme colors
 *
 * @param ref_obj Reference object for theme color lookup
 * @param is_dark_mode Whether dark mode is active
 */
void ui_update_button_style(lv_obj_t *ref_obj, bool is_dark_mode);

/**
 * @brief Get the global button style for use in widgets
 *
 * @return lv_style_t* Pointer to the global button style
 */
lv_style_t* ui_get_button_style(void);

/**
 * @brief Update switch style with current theme colors
 *
 * @param ref_obj Reference object for theme color lookup
 */
void ui_update_switch_style(lv_obj_t *ref_obj);

/**
 * @brief Get the global switch style for use in widgets
 *
 * @return lv_style_t* Pointer to the global switch style
 */
lv_style_t* ui_get_switch_style(void);


/**
 * @brief Create a standard Hebrew title label with theme-aware colors
 *
 * @param parent Parent container
 * @param text Title text
 * @return lv_obj_t* Created title label with theme primary color
 */
lv_obj_t* ui_create_title_label(lv_obj_t *parent, const char *text);

/**
 * @brief Apply common Hebrew text styling to a label
 *
 * @param label Label to style
 * @param font Font to use (e.g., &opensans_hebrew_16)
 * @param rtl_mode Whether to use RTL direction
 */
void ui_apply_hebrew_text_style(lv_obj_t *label, const lv_font_t *font, bool rtl_mode);

#ifdef __cplusplus
}
#endif

#endif // UI_HELPERS_H