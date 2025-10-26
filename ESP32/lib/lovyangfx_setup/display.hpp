#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "lovyangfx_config.hpp"

// TFT Configuration
#define TFT_HOR_RES 320
#define TFT_VER_RES 480

// Initialize display and touch
void init_display();
void init_touch();
void touch_calibrate();

// Get global LovyanGFX instance
extern LGFX gfx;

#endif // DISPLAY_HPP
