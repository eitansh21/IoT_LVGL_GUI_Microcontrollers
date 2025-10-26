# LovyanGFX Migration Guide

## The Problem

TFT_eSPI performance on ESP32 with ILI9488 displays is poor. Initial research suggested LovyanGFX wouldn't help due to ILI9488's 18-bit color mode, but actual migration proved this wrong.

## Why This Migration Matters

**Theoretical concern:** "ILI9488 requires 16→18 bit conversion, negating DMA benefits"
**Reality:** LovyanGFX has many optimizations beyond just DMA

**Results:** 70% performance improvement (500ms → 141ms average render time)

## Migration Steps

### 1. Library Changes

**platformio.ini:**
```ini
# Before
lib_deps = bodmer/TFT_eSPI@^2.5.43

# After
lib_deps = lovyan03/LovyanGFX@^1.1.16

# Remove all TFT_eSPI build flags
```

### 2. Hardware Configuration

**Create `lib/tft/lovyangfx_config.hpp`:**
```cpp
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void) {
    // SPI Bus config
    auto cfg = _bus_instance.config();
    cfg.spi_host = HSPI_HOST;
    cfg.freq_write = 40000000;    // 40MHz
    cfg.freq_read = 20000000;     // 20MHz
    cfg.dma_channel = SPI_DMA_CH_AUTO;
    cfg.pin_sclk = 18;
    cfg.pin_mosi = 23;
    cfg.pin_miso = 19;
    cfg.pin_dc = 2;
    _bus_instance.config(cfg);

    // Panel config
    auto panel_cfg = _panel_instance.config();
    panel_cfg.pin_cs = 15;
    panel_cfg.pin_rst = 4;
    panel_cfg.panel_width = 320;
    panel_cfg.panel_height = 480;
    panel_cfg.bus_shared = true;
    _panel_instance.config(panel_cfg);

    // Touch config
    auto touch_cfg = _touch_instance.config();
    touch_cfg.pin_cs = 21;
    touch_cfg.pin_int = 27;
    touch_cfg.bus_shared = true;
    touch_cfg.freq = 2500000;
    _touch_instance.config(touch_cfg);

    setPanel(&_panel_instance);
  }
};

extern LGFX gfx;
```

### 3. The Big Change: Double Buffering

**Before (TFT_eSPI):**
```cpp
// Single buffer - blocking transfers
static uint8_t *draw_buf;
draw_buf = new uint8_t[DRAW_BUF_SIZE];
disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);
```

**After (LovyanGFX):**
```cpp
// Double buffering with DMA memory
static lv_color_t *draw_buf1, *draw_buf2;

void lovyangfx_flush_cb(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx.startWrite();
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    gfx.writePixels((lgfx::rgb565_t*)px_map, w * h);
    gfx.endWrite();

    lv_display_flush_ready(disp_drv);
}

void init_lvgl_display() {
    uint32_t buf_size = TFT_HOR_RES * TFT_VER_RES / 10;

    // DMA-capable memory allocation
    draw_buf1 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    draw_buf2 = (lv_color_t*)heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);

    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, lovyangfx_flush_cb);
    lv_display_set_buffers(disp, draw_buf1, draw_buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}
```

**Why double buffering works:**
- Buffer 1: LVGL draws new frame
- Buffer 2: DMA sends previous frame to display
- **No waiting** - CPU and DMA work in parallel

### 4. Touch Calibration Changes

**Major difference:** LovyanGFX uses 8 calibration values vs TFT_eSPI's 5

```cpp
// Before (TFT_eSPI)
uint16_t cal_data[5];
tft.setTouch(cal_data);

// After (LovyanGFX)
uint16_t cal_data[8];
gfx.setTouchCalibrate(cal_data);
```

### 5. API Changes

```cpp
// Initialization
tft.begin();          → gfx.init();

// Colors
TFT_BLACK             → 0x0000
TFT_WHITE             → 0xFFFF

// Touch (remove timeout parameter)
tft.getTouch(&x, &y, 200);  → gfx.getTouch(&x, &y);

// Backlight (now built-in PWM)
digitalWrite(32, HIGH);      → gfx.setBrightness(255);
```

## Migration Issues and Solutions

### 1. Touch Calibration Problems

**Issue:** Calibration upside down or wrong resolution
**Solution:** Remove rotation changes during calibration
```cpp
void touch_calibrate() {
    uint16_t cal_data[8];
    // Don't change rotation during calibration
    gfx.calibrateTouch(cal_data, 0xFFFFFFU, 0x000000U, 15);
    save_touch_calibration(cal_data);
}
```

### 2. Single Clicks Not Working

**Issue:** Drags work but single clicks don't
**Solution:** Delete old calibration files and recalibrate
```cpp
// Clear calibration via CLI
pio run --target uploadfs

// Or in code
void force_recalibration() {
    SPIFFS.remove("/touch_cal_lgfx.dat");
    ESP.restart();
}
```

### 3. Variable Naming Consistency

**Issue:** Using `tft` variable name with LovyanGFX
**Solution:** Use `gfx` but maintain compatibility
```cpp
LGFX gfx;              // Main instance
LGFX& tft = gfx;       // Reference for backward compatibility
```

## Performance Results

**Before migration:**
```
Render: 500000+ us (avg), frequent stuttering
Memory: High fragmentation
Touch: 200-300ms lag
```

**After migration:**
```
Memory - ESP32: 120/299 KB (40.3%), LVGL: 31/60 KB (52%, frag: 3%),
Render: 138947 us (avg: 141467, max: 146221)
```

**Key improvements:**
- **70% faster rendering** (500ms → 141ms)
- **3% memory fragmentation** (was much higher)
- **5ms render consistency** (max - avg = 5ms)
- **<50ms touch response** (was 200-300ms)

## Lessons Learned

### 1. Theory vs Practice

**Don't dismiss optimizations based on single constraints.** LovyanGFX's benefits came from:
- Better memory management
- Superior DMA coordination
- Double buffering architecture
- ESP32-specific optimizations

The 18-bit conversion overhead was real but overwhelmed by other improvements.

### 2. Double Buffering is Critical

The biggest single improvement was moving from single to double buffering with DMA memory allocation.

### 3. Touch Calibration Requires Attention

LovyanGFX touch calibration is more precise but requires different handling than TFT_eSPI.

## Recommendation

**For any ESP32 + ILI9488 project using LVGL: migrate to LovyanGFX.** The performance improvement is substantial and migration effort is manageable.