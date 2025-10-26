#include <lvgl.h>
#include "esp_log.h"
#include "ui_helpers.h"


void create_welcome_tab(lv_obj_t *tab) {
    // Set RTL base direction for the tab
    lv_obj_set_style_base_dir(tab, LV_BASE_DIR_RTL, 0);

    // Create standard Hebrew tab container
    lv_obj_t *container = ui_create_tab_container(tab, 15);

    // Create title using helper
    lv_obj_t *title = ui_create_title_label(container, "ברוכים הבאים לפרויקט ב-IOT קיץ 2025");

    // Project description
    lv_obj_t *project_desc = lv_label_create(container);
    lv_label_set_text(project_desc, "פרויקט זה מדגים אפשרויות שימוש שונות בשפה העברית כחלק מממשק LVGL. \n בפרויקט יצרנו דמוי אתר חדשות, אשר משלב כמה פיצ'רים בLVGL, ונועד בכדי לעזור למפתחים בעתיד להשתמש בפיצ'רים דומים.");
    lv_label_set_long_mode(project_desc, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(project_desc, LV_PCT(100));

    // Features section
    lv_obj_t *features_title = ui_create_title_label(container, "מה כלול בפרויקט:");

    lv_obj_t *features_list = lv_label_create(container);
    lv_label_set_text(features_list,
        "- טקסט מימין לשמאל (RTL)\n"
        "- פונטים עבריים בגדלים שונים\n"
        "- ממשק תומך בשפה העברית ובניקוד\n"
        "- כרטיסיות טקסט שמתרחבות ומתכווצות\n"
        "- טקסט מנוקד עברי בגלילה\n"
        "- משיכה לרענון עם טקסט אקראי\n"
        "- גלריית תמונות אינטראקטיבית\n"
        "- תמיכה בערכות נושא (בהיר/כהה)\n"
        );
    lv_label_set_long_mode(features_list, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(features_list, LV_PCT(100));

    // Navigation instructions
    lv_obj_t *navigation = lv_label_create(container);
    lv_label_set_text(navigation, "עברו בין הכרטיסיות כדי לראות את היכולות השונות.");
    lv_label_set_long_mode(navigation, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_width(navigation, LV_PCT(100));
}
