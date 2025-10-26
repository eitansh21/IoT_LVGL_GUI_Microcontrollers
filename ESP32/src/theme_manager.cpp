#include "theme_manager.h"
#include "hebrew_fonts.h"
#include "ui_helpers.h"
#include "esp_log.h"

static const char* TAG = "THEME_MANAGER";

// Global theme state
static bool g_is_dark_mode = false;
static lv_display_t* g_display = NULL;

/**
 * Initialize theme manager
 */
void theme_manager_init(lv_display_t* disp) {
    g_display = disp;
    g_is_dark_mode = false;

    // Apply initial light theme
    theme_manager_apply_theme();

    ESP_LOGI(TAG, "Theme manager initialized with light mode");
}

/**
 * Get current dark mode state
 */
bool theme_manager_is_dark_mode(void) {
    return g_is_dark_mode;
}

/**
 * Set dark mode state
 */
void theme_manager_set_dark_mode(bool dark_mode) {
    if (g_is_dark_mode != dark_mode) {
        g_is_dark_mode = dark_mode;
        theme_manager_apply_theme();
        ESP_LOGI(TAG, "Switched to %s mode", dark_mode ? "DARK" : "LIGHT");
    }
}

/**
 * Toggle between light and dark mode
 */
void theme_manager_toggle_mode(void) {
    theme_manager_set_dark_mode(!g_is_dark_mode);
}

/**
 * Apply current theme to display
 */
void theme_manager_apply_theme(void) {
    if (!g_display) {
        ESP_LOGE(TAG, "Display not initialized");
        return;
    }

    // Create theme with appropriate colors
    lv_theme_t *theme;
    if (g_is_dark_mode) {
        theme = lv_theme_default_init(g_display,
                                      lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 4),
                                      lv_palette_lighten(LV_PALETTE_CYAN, 4),
                                      true, &opensans_hebrew_16);
    } else {
        theme = lv_theme_default_init(g_display,
                                      lv_palette_darken(LV_PALETTE_BLUE, 1),
                                      lv_palette_darken(LV_PALETTE_GREY, 2),
                                      false, &opensans_hebrew_16);
    }

    // Apply theme to display
    lv_disp_set_theme(g_display, theme);

    // Update custom styles with new theme colors
    ui_update_title_style(lv_disp_get_scr_act(g_display));
    ui_update_button_style(lv_disp_get_scr_act(g_display), g_is_dark_mode);
    ui_update_switch_style(lv_disp_get_scr_act(g_display));

    // Refresh all widgets
    lv_obj_t *scr = lv_disp_get_scr_act(g_display);
    lv_theme_apply(scr);
    lv_obj_invalidate(scr);
}