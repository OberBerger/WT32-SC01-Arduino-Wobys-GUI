/**
 * @file ConfigLGFXUser.h
 * @brief User-configurable display and touch controller settings for LovyanGFX.
 *
 * This file allows integrators to configure LovyanGFX for custom display
 * hardware that differs from the default WT32-SC01-Plus integrated display.
 * Modify these parameters if using different hardware. It defines display
 * dimensions, default orientation, and detailed configuration for the
 * display panel, bus, backlight, and touch controller.
 *
 * @version 1.0.4
 * @date 2025-08-11
 * @author György Oberländer
 * @contact support@wobys.com
 *
 * @copyright (c) 2025 György Oberländer. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * IMPORTANT NOTE ON THIRD-PARTY LICENSES:
 * This product incorporates software components licensed under various open-source components,
 * including LovyanGFX. Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#pragma once

#ifndef CONFIG_LGFX_USER_H
#define CONFIG_LGFX_USER_H

// Required for LovyanGFX library version 1.
#define LGFX_USE_V1
#include <LovyanGFX.hpp> // Core LovyanGFX library.

/**
 * @brief Display Orientation Preferences.
 *
 * Defines standard rotations for easier UI layout adaptation.
 */
enum class OrientationPreference {
    PORTRAIT_UP = 0,     ///< LGFX Rotation 0 (Portrait, USB port at top).
    LANDSCAPE_LEFT = 1,  ///< LGFX Rotation 1 (Landscape, USB port at left, default).
    PORTRAIT_DOWN = 2,   ///< LGFX Rotation 2 (Portrait, USB port at bottom, 180 deg rotated).
    LANDSCAPE_RIGHT = 3, ///< LGFX Rotation 3 (Landscape, USB port at right, 180 deg rotated).
    ADAPTIVE = 4,         ///< Special behavior: ScreenManager decides or keeps current. Layout adapts.
    CONTENT_PORTRAIT = 5, ///< Layer content is designed for a portrait aspect ratio (taller than wide).
    CONTENT_LANDSCAPE = 6 ///< Layer content is designed for a landscape aspect ratio (wider than tall).
};


/**
 * @brief Default LGFX rotation value to use for CONTENT_PORTRAIT layers
 *        when the physical display is in a landscape orientation.
 */
const int LGFX_DEFAULT_ROTATION_PORTRAIT = 2; ///< Default to PORTRAIT_UP (LGFX Rotation 2).

/**
 * @brief Default LGFX rotation value to use for CONTENT_LANDSCAPE layers
 *        when the physical display is in a portrait orientation.
 */
const int LGFX_DEFAULT_ROTATION_LANDSCAPE = 3; ///< Default to LANDSCAPE_RIGHT (LGFX Rotation 3).

/**
 * @brief Display Dimensions.
 *
 * Fixed physical dimensions of the display (e.g., WT32-SC01-Plus).
 */
#define TFT_WIDTH 320  ///< Physical width of the TFT display in pixels.
#define TFT_HEIGHT 480 ///< Physical height of the TFT display in pixels.

/**
 * @brief Display Orientation Configuration.
 *
 * Defines the default screen orientation at boot.
 */
#define DEFAULT_BOOT_ORIENTATION OrientationPreference::LANDSCAPE_RIGHT

/**
 * @brief LovyanGFX Device Configuration for WT32-SC01-Plus.
 *
 * This class configures the specific display, bus, backlight, and touch controller
 * for the WT32-SC01-Plus board. Modify these parameters if using different hardware.
 * This section must define the LGFX class.
 */
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;  ///< Panel controller instance (ST7796 for WT32-SC01-Plus).
  lgfx::Bus_Parallel8 _bus_instance;   ///< Parallel bus instance.
  lgfx::Light_PWM _light_instance;     ///< Backlight controller instance.
  lgfx::Touch_FT5x06 _touch_instance;  ///< Touch panel controller instance (FT5x06 for WT32-SC01-Plus).

public:
  /**
   * @brief Constructor for the LGFX device.
   *
   * Configures all display components based on the WT32-SC01-Plus board.
   */
  LGFX(void) {
    // Bus Configuration (Parallel8) for data transfer to the display.
    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 40000000;  ///< Write frequency (Hz) for the bus.
      cfg.pin_wr = 47;            ///< WR (Write) pin.
      cfg.pin_rd = -1;            ///< RD (Read) pin (-1 if not used, display-only).
      cfg.pin_rs = 0;             ///< RS (Register Select / DC) pin.

      // Data pins (D0-D7) for 8-bit parallel communication.
      cfg.pin_d0 = 9;  cfg.pin_d1 = 46; cfg.pin_d2 = 3;  cfg.pin_d3 = 8;
      cfg.pin_d4 = 18; cfg.pin_d5 = 17; cfg.pin_d6 = 16; cfg.pin_d7 = 15;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    // Panel Configuration (ST7796) defining display specific settings.
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = -1;    ///< CS (Chip Select) pin (-1 if not used).
      cfg.pin_rst = 4;    ///< RST (Reset) pin for the display controller.
      cfg.pin_busy = -1;  ///< BUSY pin (-1 if not used).

      cfg.panel_width = TFT_WIDTH;    ///< Actual panel width in pixels.
      cfg.panel_height = TFT_HEIGHT;  ///< Actual panel height in pixels.
      cfg.offset_x = 0;               ///< X offset for the display area.
      cfg.offset_y = 0;               ///< Y offset for the display area.
      cfg.offset_rotation = 0;        ///< Initial rotation offset for the panel.
      cfg.dummy_read_pixel = 8;       ///< Dummy read pixels for bus synchronization.
      cfg.dummy_read_bits = 1;        ///< Dummy read bits.
      cfg.readable = false;           ///< Panel read capability (typically false for display-only).
      cfg.invert = true;              ///< Invert colors (common for ST7796 panels).
      cfg.rgb_order = false;          ///< RGB color order (false = RGB, true = BGR).
      cfg.dlen_16bit = false;         ///< 16-bit data bus (false for 8-bit).
      cfg.bus_shared = false;         ///< Bus sharing (false if dedicated to display).

      _panel_instance.config(cfg);
    }

    // Backlight Configuration (PWM) for display illumination.
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 45;      ///< Backlight (BL) pin.
      cfg.invert = false;   ///< Invert backlight control signal.
      cfg.freq = 44100;     ///< PWM frequency (Hz).
      cfg.pwm_channel = 7;  ///< ESP32 PWM channel (0-15).

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    // Touch Panel Configuration (FT5x06) for touch input.
    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0; cfg.x_max = TFT_WIDTH - 1;  ///< Touch X range on display.
      cfg.y_min = 0; cfg.y_max = TFT_HEIGHT - 1; ///< Touch Y range on display.
      cfg.pin_int = 7;            ///< INT (Interrupt) pin from touch controller.
      cfg.bus_shared = true;      ///< Bus sharing (true for I2C bus with other devices).
      cfg.offset_rotation = 0;    ///< Rotation offset for touch coordinates.

      // I2C Settings for Touch Controller communication.
      cfg.i2c_port = 1;     ///< I2C port (0 or 1).
      cfg.i2c_addr = 0x38;  ///< I2C address of touch controller.
      cfg.pin_sda = 6;      ///< SDA pin.
      cfg.pin_scl = 5;      ///< SCL pin.
      cfg.freq = 400000;    ///< I2C frequency (Hz).

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance); ///< Set the configured panel as the active display.
  }

  /**
   * @brief Custom initialization method for LGFX.
   *
   * Calls the base class init and applies default orientation based on configuration.
   * @return True if initialization is successful, false otherwise.
   */
  bool init() {
    bool result = lgfx::LGFX_Device::init();
    // Apply default orientation at boot time if physical dimensions don't match.
    setRotation(static_cast<int>(DEFAULT_BOOT_ORIENTATION));
    return result;
  }
};

#endif // CONFIG_LGFX_USER_H