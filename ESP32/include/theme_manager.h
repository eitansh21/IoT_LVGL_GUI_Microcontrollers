#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <lvgl.h>
#include <stdbool.h>

/**
 * @brief Initialize the theme manager
 *
 * Sets up the theme manager with the given display and applies
 * the initial light theme.
 *
 * @param disp Display to manage themes for
 */
void theme_manager_init(lv_display_t* disp);

/**
 * @brief Get current dark mode state
 *
 * @return true if dark mode is active, false if light mode
 */
bool theme_manager_is_dark_mode(void);

/**
 * @brief Set dark mode state
 *
 * Changes the theme to dark or light mode and updates all
 * custom styles accordingly.
 *
 * @param dark_mode true for dark mode, false for light mode
 */
void theme_manager_set_dark_mode(bool dark_mode);

/**
 * @brief Toggle between light and dark mode
 *
 * Switches from light to dark mode or vice versa.
 */
void theme_manager_toggle_mode(void);

/**
 * @brief Apply current theme to display
 *
 * Re-applies the current theme and updates all custom styles.
 * Useful for refreshing after style changes.
 */
void theme_manager_apply_theme(void);

#endif // THEME_MANAGER_H