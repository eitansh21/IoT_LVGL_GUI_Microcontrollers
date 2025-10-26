#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_DEV_VERSION 1
#define LV_COLOR_DEPTH 16
//#define LV_MEM_SIZE (128 * 1024U)

#define LV_COLOR_16_SWAP 0


/* Disable TFT_eSPI driver - using LovyanGFX instead */
#define LV_USE_TFT_ESPI 0

/* Include Hebrew support configuration */
#include "lv_conf_hebrew_support.h"


#ifdef ENABLE_HEBREW_SUPPORT
/* Set built-in Hebrew font as default when Hebrew support is enabled */
#define LV_FONT_DEFAULT &lv_font_dejavu_16_persian_hebrew
#endif


#endif /*LV_CONF_H*/
