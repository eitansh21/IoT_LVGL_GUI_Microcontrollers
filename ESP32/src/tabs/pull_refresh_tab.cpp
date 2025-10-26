#include <lvgl.h>
#include <cstdlib>
#include <ctime>
#include "esp_log.h"
#include "hebrew_fonts.h"
#include "lv_pull_refresh.h"
#include "../ui_config/hebrew_widget_config.h"
#include "ui_helpers.h"

// Array of random Hebrew texts
static const char* random_hebrew_texts[] = {
    "השמש זורחת מעל הרים גבוהים. הציפורים שרות בשמיים הכחולים. הרוח נושבת בעדינות בין העצים הירוקים.",
    
    "בעיר הגדולה יש הרבה בניינים גבוהים. האנשים הולכים במהירות ברחובות העמוסים. המכוניות נוסעות בכל כיוון.",
    
    "הילדים משחקים בפארק הגדול. הם רצים על הדשא הירוק ונהנים מהשמש החמה. ההורים יושבים על הספסלים וצופים בהם.",
    
    "הכלב הקטן רץ בגינה ומחפש את הכדור האדום. החתול יושב על העץ וצופה בו מלמעלה. הפרפרים עפים בין הפרחים הצבעוניים.",
};

static const int num_texts = sizeof(random_hebrew_texts) / sizeof(random_hebrew_texts[0]);
static lv_obj_t* global_text_label = NULL;  // Global reference for callbacks

// Initialize random seed once
static void init_random_seed() {
    static bool initialized = false;
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }
}

// Generate and display random text
static void update_random_text() {
    if (!global_text_label) return;

    init_random_seed();
    int random_index = rand() % num_texts;
    lv_label_set_text(global_text_label, random_hebrew_texts[random_index]);
    ESP_LOGI("RandomTab", "Random text updated: index %d", random_index);
}

// Timer callback for completing refresh
static void refresh_complete_timer_cb(lv_timer_t* timer) {
    lv_obj_t* container = (lv_obj_t*)lv_timer_get_user_data(timer);
    lv_pull_refresh_complete(container);
    lv_timer_delete(timer);
}

// Pull-to-refresh callback
static void pull_refresh_callback(lv_obj_t* container, void* user_data) {
    ESP_LOGI("RandomTab", "Pull-to-refresh triggered!");

    // Update random text
    update_random_text();

    // Simulate async operation with a small delay
    lv_timer_t* complete_timer = lv_timer_create(refresh_complete_timer_cb, 300, container);
    lv_timer_set_repeat_count(complete_timer, 1);
}

// Pull state change callback for visual feedback
static void pull_state_callback(lv_obj_t* container, int pull_distance, bool threshold_reached, void* user_data) {
    // Optional: Add visual feedback based on pull distance
    ESP_LOGD("RandomTab", "Pull distance: %d, threshold reached: %s",
             pull_distance, threshold_reached ? "yes" : "no");
}

// No longer needed - removed manual refresh button

void create_pull_refresh_tab(lv_obj_t *tab) {
    // Use Hebrew configuration for pull-to-refresh
    lv_pull_refresh_config_t config = hebrew_get_pull_refresh_config(pull_refresh_callback, NULL);
    config.state_cb = pull_state_callback;

    // Create pull-to-refresh as the main container filling the entire tab
    lv_obj_t *container = lv_pull_refresh_create(tab, &config);
    if (!container) {
        ESP_LOGE("RandomTab", "Failed to create pull-refresh container");
        return;
    }

    // Make it fill the parent tab completely
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_base_dir(container, LV_BASE_DIR_RTL, 0);

    // Create title using helper
    lv_obj_t *title = ui_create_title_label(container, "טקסט אקראי");
    lv_obj_set_style_pad_bottom(title, 20, 0);

    // Instructions - focus on pull-to-refresh only
    lv_obj_t *instructions = lv_label_create(container);
    lv_label_set_text(instructions, "משוך את המסך למעלה כדי לקבל טקסט עברי אקראי חדש");
    lv_label_set_long_mode(instructions, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(instructions, LV_PCT(100));
    lv_obj_set_style_pad_bottom(instructions, 25, 0);

    // Random text display - more prominent styling
    global_text_label = lv_label_create(container);
    lv_label_set_text(global_text_label, "משוך למעלה כדי לקבל טקסט עברי אקראי...");
    lv_label_set_long_mode(global_text_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(global_text_label, LV_PCT(100));
    lv_obj_set_style_pad_all(global_text_label, 20, 0);
    lv_obj_set_style_bg_opa(global_text_label, LV_OPA_10, 0);
    lv_obj_set_style_radius(global_text_label, 10, 0);
    lv_obj_set_style_border_width(global_text_label, 1, 0);
    lv_obj_set_style_border_opa(global_text_label, LV_OPA_30, 0);

    // Add spacer at bottom to ensure scrollable content
    lv_obj_t *spacer = lv_obj_create(container);
    lv_obj_set_size(spacer, LV_PCT(100), 150);
    lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spacer, 0, 0);

    ESP_LOGI("RandomTab", "Random tab created with pull-to-refresh as main container");
}
