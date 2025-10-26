#include <lvgl.h>
#include "esp_log.h"
#include "hebrew_fonts.h"
#include "ui_helpers.h"

// Structure to track article state
typedef struct {
    lv_obj_t *content_label;
    lv_obj_t *expand_btn;
    lv_obj_t *expand_label;
    bool expanded;
    const char *short_text;
    const char *full_text;
} sport_article_data_t;

void create_niqqud_demo_tab(lv_obj_t *tab) {
    // Set RTL base direction for the tab
    lv_obj_set_style_base_dir(tab, LV_BASE_DIR_RTL, 0);

    // Create standard Hebrew tab container
    lv_obj_t *container = ui_create_tab_container(tab, 15);

    // Create title using helper
    lv_obj_t *title = ui_create_title_label(container, "טקסט מנוקד לדוגמה");

    // Scrollable text demonstration section
    lv_obj_t *scroll_demo_title = ui_create_title_label(container, "הדגמת גלילה בתוך אזור טקסט:");

    // Scrollable text container with proper styling
    lv_obj_t *scroll_container = lv_obj_create(container);
    lv_obj_set_size(scroll_container, LV_PCT(100), 250); // Taller for better reading

    // Improved styling for niqqud display
    lv_obj_set_style_radius(scroll_container, 8, 0);
    lv_obj_set_style_border_width(scroll_container, 1, 0);
    lv_obj_set_style_bg_opa(scroll_container, LV_OPA_10, 0);
    lv_obj_set_style_pad_all(scroll_container, 15, 0);

    // Set RTL direction for proper Hebrew display
    lv_obj_set_style_base_dir(scroll_container, LV_BASE_DIR_RTL, 0);

    // Enable scrolling with better scrollbar styling
    lv_obj_set_scroll_dir(scroll_container, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLLABLE);

    // Enable elastic scroll only (momentum disabled for better control)
    lv_obj_remove_flag(scroll_container, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Demo text for scrolling
    lv_obj_t *scroll_text = lv_label_create(scroll_container);
    lv_label_set_text(scroll_text,
        "א וַתָּשַׁר דְּבוֹרָה, וּבָרָק בֶּן-אֲבִינֹעַם, בַּיּוֹם הַהוּא לֵאמֹר. \n"
        "ב בִּפְרֹעַ פְּרָעוֹת בְּיִשְׂרָאֵל, בְּהִתְנַדֵּב עָם, בָּרְכוּ, יְהוָה. \n"
        "ג שִׁמְעוּ מְלָכִים, הַאֲזִינוּ רֹזְנִים: אָנֹכִי, לַיהוָה אָנֹכִי אָשִׁירָה, אֲזַמֵּר, לַיהוָה אֱלֹהֵי יִשְׂרָאֵל.\n"
        "ד יְהוָה, בְּצֵאתְךָ מִשֵּׂעִיר בְּצַעְדְּךָ מִשְּׂדֵה אֱדוֹם, אֶרֶץ רָעָשָׁה, גַּם-שָׁמַיִם נָטָפוּ; גַּם-עָבִים, נָטְפוּ מָיִם.\n"
        "ה הָרִים נָזְלוּ, מִפְּנֵי יְהוָה: זֶה סִינַי--מִפְּנֵי, יְהוָה אֱלֹהֵי יִשְׂרָאֵל.\n"
        "ו בִּימֵי שַׁמְגַּר בֶּן עֲנָת, בִּימֵי יָעֵל, חָדְלוּ, אֳרָחוֹת; וְהֹלְכֵי נְתִיבוֹת--יֵלְכוּ, אֳרָחוֹת עֲקַלְקַלּוֹת.\n"
        "ז חָדְלוּ פְרָזוֹן בְּיִשְׂרָאֵל חָדֵלּוּ עַד שַׁקַּמְתִּי דְּבוֹרָה, שַׁקַּמְתִּי אֵם בְּיִשְׂרָאֵל.\n"
        "ח יִבְחַר אֱלֹהִים חֲדָשִׁים, אָז לָחֶם שְׁעָרִים; מָגֵן אִם-יֵרָאֶה וָרֹמַח, בְּאַרְבָּעִים אֶלֶף בְּיִשְׂרָאֵל.\n"
        "ט לִבִּי לְחוֹקְקֵי יִשְׂרָאֵל, הַמִּתְנַדְּבִים בָּעָם; בָּרְכוּ יְהוָה.\n"
        "י רֹכְבֵי אֲתֹנוֹת צְחֹרוֹת יֹשְׁבֵי עַל-מִדִּין, וְהֹלְכֵי עַל-דֶּרֶךְ--שִׂיחוּ.\n"
        "יא מִקּוֹל מְחַצְצִים, בֵּין מַשְׁאַבִּים שָׁם יְתַנּוּ צִדְקוֹת יְהוָה, צִדְקֹת פִּרְזוֹנוֹ בְּיִשְׂרָאֵל; אָז יָרְדוּ לַשְּׁעָרִים, עַם יְהוָה.\n"
        "יב עוּרִי עוּרִי דְּבוֹרָה, עוּרִי עוּרִי דַּבְּרִי-שִׁיר; קוּם בָּרָק וּשְׁבֵה שֶׁבְיְךָ, בֶּן-אֲבִינֹעַם.\n"
        "יג אָז יְרַד שָׂרִיד, לְאַדִּירִים עָם; יְהוָה יְרַד-לִי בַּגִּבּוֹרִים.\n"
        "יד מִנִּי אֶפְרַיִם, שָׁרְשָׁם בַּעֲמָלֵק, אַחֲרֶיךָ בִנְיָמִין, בַּעֲמָמֶיךָ; מִנִּי מָכִיר, יָרְדוּ מְחֹקְקִים, וּמִזְּבוּלֻן, מֹשְׁכִים בְּשֵׁבֶט סֹפֵר.\n"
        "טו וְשָׂרַי בְּיִשָּׂשכָר, עִם-דְּבֹרָה, וְיִשָּׂשכָר כֵּן בָּרָק, בָּעֵמֶק שֻׁלַּח בְּרַגְלָיו; בִּפְלַגּוֹת רְאוּבֵן, גְּדֹלִים חִקְקֵי-לֵב.\n"
        "טז לָמָּה יָשַׁבְתָּ, בֵּין הַמִּשְׁפְּתַיִם, לִשְׁמֹעַ שׁרִקוֹת עֲדָרִים; לִפְלַגּוֹת רְאוּבֵן, גְּדוֹלִים חִקְרֵי-לֵב.  \n"
        "יז גִּלְעָד, בְּעֵבֶר הַיַּרְדֵּן שָׁכֵן, וְדָן, לָמָּה יָגוּר אֳנִיּוֹת; אָשֵׁר, יָשַׁב לְחוֹף יַמִּים, וְעַל מִפְרָצָיו, יִשְׁכּוֹן.\n"
        "יח זְבֻלוּן עַם חֵרֵף נַפְשׁוֹ לָמוּת וְנַפְתָּלִי: עַל, מְרוֹמֵי שָׂדֶה.\n"
        "יט בָּאוּ מְלָכִים, נִלְחָמוּ, אָז נִלְחֲמוּ מַלְכֵי כְנַעַן, בְּתַעְנַךְ עַל-מֵי מְגִדּוֹ; בֶּצַע כֶּסֶף, לֹא לָקָחוּ.\n"
        "כ מִן-שָׁמַיִם, נִלְחָמוּ; הַכּוֹכָבִים, מִמְּסִלּוֹתָם, נִלְחֲמוּ, עִם-סִיסְרָא.\n"
        "כא נַחַל קִישׁוֹן גְּרָפָם, נַחַל קְדוּמִים נַחַל קִישׁוֹן; תִּדְרְכִי נַפְשִׁי עֹז.\n"
        "כב אָז הָלְמוּ, עִקְּבֵי-סוּס, מִדַּהֲרוֹת, דַּהֲרוֹת אַבִּירָיו.\n"
        "כג אוֹרוּ מֵרוֹז, אָמַר מַלְאַךְ יְהוָה--אֹרוּ אָרוֹר יֹשְׁבֶיהָ: כִּי לֹא-בָאוּ לְעֶזְרַת יְהוָה, לְעֶזְרַת יְהוָה בַּגִּבּוֹרִים.\n"
        "כד תְּבֹרַךְ, מִנָּשִׁים--יָעֵל, אֵשֶׁת חֶבֶר הַקֵּינִי: מִנָּשִׁים בָּאֹהֶל, תְּבֹרָךְ.\n"
        "כה מַיִם שָׁאַל, חָלָב נָתָנָה; בְּסֵפֶל אַדִּירִים, הִקְרִיבָה חֶמְאָה. \n"
        "כו יָדָהּ לַיָּתֵד תִּשְׁלַחְנָה, וִימִינָהּ לְהַלְמוּת עֲמֵלִים; וְהָלְמָה סִיסְרָא מָחֲקָה רֹאשׁוֹ, וּמָחֲצָה וְחָלְפָה רַקָּתוֹ.\n"
        "כז בֵּין רַגְלֶיהָ, כָּרַע נָפַל שָׁכָב: בֵּין רַגְלֶיהָ, כָּרַע נָפָל, בַּאֲשֶׁר כָּרַע, שָׁם נָפַל שָׁדוּד.\n"
        "כח בְּעַד הַחַלּוֹן נִשְׁקְפָה וַתְּיַבֵּב אֵם סִיסְרָא, בְּעַד הָאֶשְׁנָב: מַדּוּעַ, בֹּשֵׁשׁ רִכְבּוֹ לָבוֹא מַדּוּעַ אֶחֱרוּ, פַּעֲמֵי מַרְכְּבוֹתָיו.\n"
        "כט חַכְמוֹת שָׂרוֹתֶיהָ, תַּעֲנֶינָּה; אַף-הִיא, תָּשִׁיב אֲמָרֶיהָ לָהּ.\n"
        "ל הֲלֹא יִמְצְאוּ יְחַלְּקוּ שָׁלָל, רַחַם רַחֲמָתַיִם לְרֹאשׁ גֶּבֶר שְׁלַל צְבָעִים לְסִיסְרָא, שְׁלַל צְבָעִים רִקְמָה: צֶבַע רִקְמָתַיִם, לְצַוְּארֵי שָׁלָל.\n"
        "לא כֵּן יֹאבְדוּ כָל-אוֹיְבֶיךָ, יְהוָה, וְאֹהֲבָיו, כְּצֵאת הַשֶּׁמֶשׁ בִּגְבֻרָתוֹ; וַתִּשְׁקֹט הָאָרֶץ, אַרְבָּעִים שָׁנָה.");

    // Improved text styling for niqqud
    lv_obj_set_style_base_dir(scroll_text, LV_BASE_DIR_RTL, 0);
    lv_obj_set_width(scroll_text, LV_PCT(100)); // Use full container width
    lv_label_set_long_mode(scroll_text, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(scroll_text, LV_TEXT_ALIGN_RIGHT, 0);

    // Apply OpenSans Hebrew font for proper niqqud display (LVGL 9 direct styling)
    lv_obj_set_style_text_font(scroll_text, &opensans_hebrew_16, LV_PART_MAIN);
    lv_obj_set_style_text_line_space(scroll_text, 8, LV_PART_MAIN); // More line spacing for niqqud
    lv_obj_set_style_text_letter_space(scroll_text, 1, LV_PART_MAIN); // Slight letter spacing
}
