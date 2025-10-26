#include <lvgl.h>
#include "hebrew_tabs.h"
#include "hebrew_fonts.h"
#include "lv_expandable_card.h"
#include "../ui_config/hebrew_widget_config.h"
#include "ui_helpers.h"

// Layout constants
#define NEWS_TAB_PADDING 15
#define NEWS_TAB_ROW_PADDING 15

// Article data - must be static/global for widget lifetime
// Examples of theme-aware palette colors vs custom colors vs theme default
static lv_card_data_t news_articles[] = {
    {
        .title = "פיתוח ממשק משתמש עברי עם LVGL",
        .content = "מדריך מקיף לפיתוח ממשקי משתמש עבריים באמצעות ספריית LVGL. "
                   "המדריך כולל הסבר על התמיכה בכיוון RTL, יצירת פונטים עבריים מותאמים, "
                   "והטמעת טקסט דו-כיווני. בנוסף, נסקור את הכלים הזמינים ליצירת "
                   "ממשקים אינטראקטיביים ומותאמים לשפה העברית. המדריך מתאים גם למפתחים חדשים וגם למקצועות מנוסים :).",
    },
    {
        .title = "טכנולוגיות חדשות בעולם הפיתוח",
        .content = "סקירה של הטכנולוגיות החדשות ביותר בתחום פיתוח התוכנה. "
                   "בכתבה מדובר על הטרנדים החמים כמו בינה מלאכותית, פיתוח ענן, ומיקרו-שירותים. "
                   "כמו כן בחנו את ההשפעה של טכנולוגיות אלו על השוק הישראלי ועל "
                   "הזדמנויות התעסוקה בתחום. המאמר כולל המלצות למפתחים המעוניינים להתמחות בתחומים החדשים.",
    },
    {
        .title = "עדכוני אבטחת מידע לשנת 2024",
        .content = "חדשות ועדכונים חשובים בתחום אבטחת המידע והסייבר לשנת 2024. "
                   "סקרנו את האיומים החדשים, שיטות ההגנה המתקדמות, ואת החקיקה החדשה "
                   "בתחום הפרטיות וההגנה על מידע. המאמר כולל המלצות מעשיות לארגונים ולמפתחים "
                   "על יישום אמצעי אבטחה מתקדמים ועל הכנה לאתגרי העתיד בעולם הסייבר.",
    },
    {
        .title = "LVGL Multi-Language Support Demo",
        .content = "This English card demonstrates that the expandable card widget supports both RTL and LTR text layouts. "
                   "The widget automatically handles text direction based on configuration. This makes it perfect for "
                   "international applications that need to support multiple languages with different reading directions. "
                   "Notice how this card flows left-to-right while Hebrew cards flow right-to-left seamlessly.",
    }
};

void create_news_tab(lv_obj_t *tab) {
    // Set RTL base direction for the tab
    lv_obj_set_style_base_dir(tab, LV_BASE_DIR_RTL, 0);

    // Create standard Hebrew tab container (eliminates 15+ lines of repetitive code)
    lv_obj_t *container = ui_create_tab_container(tab, NEWS_TAB_PADDING);

    // Create title using helper (eliminates style object repetition)
    lv_obj_t *title = ui_create_title_label(container, "חדשות וכתבות \"החמות ביותר\"");

    // Create article cards using the reusable widget
    const int num_articles = sizeof(news_articles) / sizeof(news_articles[0]);
    for (int i = 0; i < num_articles; i++) {
        // Default to Hebrew configuration
        static lv_card_config_t hebrew_config;
        hebrew_config = hebrew_get_expandable_card_config();
        lv_card_config_t* config = &hebrew_config;

        // Special config for English card (last one) - demonstrate LTR mode
        static lv_card_config_t english_config;
        if (i == num_articles - 1) {
            english_config = lv_expandable_card_get_default_config();
            english_config.style.base_dir = LV_BASE_DIR_LTR;     // Left-to-right for English
            english_config.expand_text = NULL;   // Auto-set to "Expand" based on rtl_mode
            english_config.collapse_text = NULL; // Auto-set to "Collapse" based on rtl_mode
            english_config.max_content_height = 200;  // Demonstrate height limiting
            english_config.title_style = ui_get_title_style();   // Use theme-aware title style
            english_config.button_style = ui_get_button_style(); // Use theme-aware button style
            config = &english_config;
        }

        lv_obj_t* card = lv_expandable_card_create(
            container,
            &news_articles[i],
            config  // Use custom config for English, NULL for Hebrew cards
        );

        if (!card) {
            // Handle error - could log or continue with next article
            continue;
        }
    }
}