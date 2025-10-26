#include "display.hpp"
#include "esp_log.h"
#include "SPIFFS.h"
#include "FS.h"
#include <algorithm>

static const char* TAG = "TFT";

// Global LovyanGFX instance
LGFX gfx;

void init_display() {
    ESP_LOGI(TAG, "Initializing LovyanGFX display...");
    gfx.init();
    gfx.setRotation(0);  // Portrait mode: 320x480
    gfx.setBrightness(255);  // Full brightness
    gfx.fillScreen(0x0000);  // Clear screen to black
    ESP_LOGI(TAG, "LovyanGFX display initialized");
}

void touch_calibrate() {
    ESP_LOGI(TAG, "Initializing touchscreen...");

    // Check if touch is available
    if (!gfx.touch()) {
        ESP_LOGE(TAG, "Touch controller not detected!");
        return;
    }

    // Initialize SPIFFS for calibration file
    if (!SPIFFS.begin()) {
        ESP_LOGE(TAG, "SPIFFS mount failed, formatting...");
        SPIFFS.format();
        SPIFFS.begin();
    }

    uint16_t cal_data[8];  // LovyanGFX uses 8 values
    bool cal_data_ok = false;

    // Try to read existing calibration
    fs::File cal_file = SPIFFS.open("/touch_cal_lgfx.dat", "r");
    if (cal_file) {
        if (cal_file.readBytes((char*)cal_data, 16) == 16) {
            cal_data_ok = true;
            ESP_LOGI(TAG, "Loaded touch calibration from file");
            ESP_LOGI(TAG, "Cal data: [%d, %d, %d, %d, %d, %d, %d, %d]",
                     cal_data[0], cal_data[1], cal_data[2], cal_data[3],
                     cal_data[4], cal_data[5], cal_data[6], cal_data[7]);
        }
        cal_file.close();
    }

    if (cal_data_ok) {
        gfx.setTouchCalibrate(cal_data);
        ESP_LOGI(TAG, "Applied existing calibration");
    } else {
        ESP_LOGI(TAG, "No calibration found, running calibration...");

        gfx.fillScreen(0x0000);  // Black
        gfx.setTextColor(0xFFFF);  // White
        gfx.setTextSize(2);
        gfx.drawString("TOUCH THE ARROW MARKER", 30, 100);

        // Run calibration in current portrait mode (320x480)
        // Size parameter: reasonable marker size for our screen
        gfx.calibrateTouch(cal_data, 0xFFFFFFU, 0x000000U, 15);

        // Log the calibration values
        ESP_LOGI(TAG, "Calibration complete! Values: [%d, %d, %d, %d, %d, %d, %d, %d]",
                 cal_data[0], cal_data[1], cal_data[2], cal_data[3],
                 cal_data[4], cal_data[5], cal_data[6], cal_data[7]);

        gfx.fillScreen(0x0000);  // Black
        gfx.drawString("CALIBRATION DONE!", 50, 200);
        gfx.drawString("TOUCH TO CONTINUE", 50, 230);

        // Save calibration to file
        fs::File save_file = SPIFFS.open("/touch_cal_lgfx.dat", "w");
        if (save_file) {
            save_file.write((uint8_t*)cal_data, 16);
            save_file.close();
            ESP_LOGI(TAG, "Calibration saved to file");
        } else {
            ESP_LOGE(TAG, "Failed to save calibration");
        }

        // Wait for touch to continue
        uint16_t x, y;
        while (!gfx.getTouch(&x, &y)) {
            delay(100);
        }
    }

    gfx.fillScreen(0x0000);  // Black
    ESP_LOGI(TAG, "Touch initialization complete");
}

void init_touch() {
    touch_calibrate();
}
