#ifndef LV_CONF_HEBREW_SUPPORT_H
#define LV_CONF_HEBREW_SUPPORT_H

/* Enable Hebrew support by defining ENABLE_HEBREW_SUPPORT */
#ifdef ENABLE_HEBREW_SUPPORT

/* Hebrew and bidirectional text support */
#define LV_USE_BIDI 1
#define LV_USE_ARABIC_PERSIAN_CHARS 1

/* Enable built-in Hebrew font support */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 1

#endif /* ENABLE_HEBREW_SUPPORT */

#endif /* LV_CONF_HEBREW_SUPPORT_H */
