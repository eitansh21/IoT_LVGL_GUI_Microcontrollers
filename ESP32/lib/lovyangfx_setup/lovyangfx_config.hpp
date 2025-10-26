#ifndef LOVYANGFX_CONFIG_HPP
#define LOVYANGFX_CONFIG_HPP

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

/**
 * LovyanGFX configuration for ILI9488 3.5" 320x480 display
 * Hardware: ESP32 with resistive touch
 */
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9488     _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void)
  {
    { // Configure SPI bus
      auto cfg = _bus_instance.config();
      cfg.spi_host = HSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;    // 40MHz for writing
      cfg.freq_read  = 20000000;    // 20MHz for reading
      cfg.spi_3wire  = false;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;

      cfg.pin_sclk = 18;            // SPI SCLK
      cfg.pin_mosi = 23;            // SPI MOSI
      cfg.pin_miso = 19;            // SPI MISO
      cfg.pin_dc   = 2;             // SPI D/C

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    { // Configure display panel
      auto cfg = _panel_instance.config();

      cfg.pin_cs           = 15;    // Chip Select
      cfg.pin_rst          = 4;     // Reset
      cfg.pin_busy         = -1;    // Busy (not used)

      cfg.panel_width      = 320;   // Actual panel width
      cfg.panel_height     = 480;   // Actual panel height
      cfg.offset_x         = 0;     // Panel offset in X direction
      cfg.offset_y         = 0;     // Panel offset in Y direction
      cfg.offset_rotation  = 0;     // Rotation offset (0~7)
      cfg.dummy_read_pixel = 8;     // Number of dummy read bits before pixel read
      cfg.dummy_read_bits  = 1;     // Number of dummy read bits before non-pixel read
      cfg.readable         = false; // Set to true if data can be read
      cfg.invert           = false; // Set to true if panel light/dark is inverted
      cfg.rgb_order        = false; // Set to true if panel red and blue are swapped
      cfg.dlen_16bit       = false; // Set to true for panels that transmit data length in 16 bit units
      cfg.bus_shared       = true;  // Set to true when sharing the bus with SD card (use transaction control with drawJpgFile etc.)

      _panel_instance.config(cfg);
    }

    { // Configure backlight PWM
      auto cfg = _light_instance.config();
      cfg.pin_bl = 32;              // Backlight pin
      cfg.invert = false;           // true to invert brightness
      cfg.freq   = 44100;           // PWM frequency
      cfg.pwm_channel = 7;          // PWM channel

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    { // Configure touch screen
      auto cfg = _touch_instance.config();
      cfg.x_min      = 0;           // Minimum X value obtained from touch screen
      cfg.x_max      = 319;         // Maximum X value obtained from touch screen
      cfg.y_min      = 0;           // Minimum Y value obtained from touch screen
      cfg.y_max      = 479;         // Maximum Y value obtained from touch screen
      cfg.pin_int    = 27;          // Touch interrupt pin
      cfg.bus_shared = true;        // Set true when sharing SPI bus with screen
      cfg.offset_rotation = 0;      // Adjustment when touch orientation doesn't match display

      // SPI bus settings for touch - try slower frequency for better stability
      cfg.spi_host = HSPI_HOST;
      cfg.freq = 2500000;           // Reduced from 2.5MHz to 1MHz for stability
      cfg.pin_sclk = 18;            // Same as display SCLK
      cfg.pin_mosi = 23;            // Same as display MOSI
      cfg.pin_miso = 19;            // Same as display MISO
      cfg.pin_cs   = 21;            // Touch chip select

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

#endif // LOVYANGFX_CONFIG_HPP
