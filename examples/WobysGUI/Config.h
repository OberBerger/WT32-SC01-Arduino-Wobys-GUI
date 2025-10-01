/**
 * @file Config.h
 * @brief Main configuration file for the WobysGUI firmware.
 *
 * This file serves as the central point for including all other configuration files.
 * By modifying the included files, users can adapt the software to different
 * hardware setups and personalize the user experience. It also defines global
 * debug settings and application-wide default parameters.
 *
 * @version 1.0.2
 * @date 2025-08-28
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
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#pragma once

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Primary Configuration File Imports.
 *
 * This section includes all other configuration files, acting as the master include.
 * Ensure correct include order to satisfy dependencies.
 */
// Include hardware pin assignments and parameters specific to the user's board.
#include "ConfigHardwareUser.h"

// Include LovyanGFX display and touch controller configuration for user's hardware.
#include "ConfigLGFXUser.h"

// Include UI styling, colors, and default sizes for User Interface elements.
#include "ConfigUIUser.h"

// Include font declarations and static instances for all fonts used in the UI.
#include "ConfigFonts.h"

// Include audio configuration parameters.
#include "ConfigAudioUser.h"

// Standard Arduino Library includes for base types (e.g., uint8_t)
#include <Arduino.h>

//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************

#define DEMO_MODE //********************************************************************************************

#ifdef DEMO_MODE
// Define demo limits here
#define MAX_UI_MODIFICATIONS_DEMO     50   ///< Max allowed UI interactions (e.g., button presses, seekbar changes)
#define MAX_WIFI_SCANS_DEMO           20   ///< Max allowed Wi-Fi scans
#define MAX_BLE_SCANS_DEMO            20   ///< Max allowed BLE scans
#define MAX_UI_INTERACTION_DEMO       100  ///< Max UI Interaction (for ScreenManager)
#endif

//**************************************************************************************************************
//**************************************************************************************************************
//**************************************************************************************************************

/**
 * @brief Debug Mode Configuration.
 *
 * This section enables/disables serial debug output across the application.
 * It provides fine-grained control over which debug messages are compiled
 * based on defined DEBUG_LEVEL_XXXX macros.
 */
#define DEBUG_MODE                  ///< Uncomment to enable debug messages system-wide
#define DEBUG_LEVEL_ERROR           ///< Enable ERROR level debugs
#define DEBUG_LEVEL_WARNING         ///< Enable WARNING level debugs
//#define DEBUG_LEVEL_INFO            ///< Enable INFO level debugs
//#define DEBUG_LEVEL_TRACE           ///< Enable TRACE level debugs (most verbose)


#ifdef DEBUG_MODE
  #ifdef DEBUG_LEVEL_ERROR
    #define DEBUG_ERROR_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_ERROR_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_ERROR_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_ERROR_PRINT(...)
    #define DEBUG_ERROR_PRINTLN(...)
    #define DEBUG_ERROR_PRINTF(...)
  #endif

  #ifdef DEBUG_LEVEL_WARNING
    #define DEBUG_WARN_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_WARN_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_WARN_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_WARN_PRINT(...)
    #define DEBUG_WARN_PRINTLN(...)
    #define DEBUG_WARN_PRINTF(...)
  #endif

  #ifdef DEBUG_LEVEL_INFO
    #define DEBUG_INFO_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_INFO_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_INFO_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_INFO_PRINT(...)
    #define DEBUG_INFO_PRINTLN(...)
    #define DEBUG_INFO_PRINTF(...)
  #endif

  #ifdef DEBUG_LEVEL_TRACE
    #define DEBUG_TRACE_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_TRACE_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_TRACE_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_TRACE_PRINT(...)
    #define DEBUG_TRACE_PRINTLN(...)
    #define DEBUG_TRACE_PRINTF(...)
  #endif

  // Generic DEBUG_PRINT aliases
  #if defined(DEBUG_LEVEL_ERROR) || defined(DEBUG_LEVEL_WARNING) || defined(DEBUG_LEVEL_INFO) || defined(DEBUG_LEVEL_TRACE)
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
    #define DEBUG_PRINTF(...)
#endif

  // Generic DEBUG_PRINT aliases (only active if any specific level is active)
  // These are here for backwards compatibility if a specific level is not desired,
  // but it's recommended to use the level-specific macros for clarity.
  #if defined(DEBUG_LEVEL_ERROR) || defined(DEBUG_LEVEL_WARNING) || defined(DEBUG_LEVEL_INFO) || defined(DEBUG_LEVEL_TRACE)
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(fmt, ...) do { if (Serial) { Serial.printf(fmt __VA_OPT__(,) __VA_ARGS__); Serial.println(); } } while(0)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
  #else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
    #define DEBUG_PRINTF(...)
  #endif

#else // DEBUG_MODE is not defined
  // If DEBUG_MODE is globally disabled, all debug macros compile to nothing.
  #define DEBUG_ERROR_PRINT(...)
  #define DEBUG_ERROR_PRINTLN(...)
  #define DEBUG_ERROR_PRINTF(...)
  #define DEBUG_WARN_PRINT(...)
  #define DEBUG_WARN_PRINTLN(...)
  #define DEBUG_WARN_PRINTF(...)
  #define DEBUG_INFO_PRINT(...)
  #define DEBUG_INFO_PRINTLN(...)
  #define DEBUG_INFO_PRINTF(...)
  #define DEBUG_TRACE_PRINT(...)
  #define DEBUG_TRACE_PRINTLN(...)
  #define DEBUG_TRACE_PRINTF(...)
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
  #define DEBUG_PRINTF(...)
#endif

/**
 * @brief Application Default Settings.
 *
 * This section defines general application parameters that can be user-defined.
 * These are compile-time defaults; some might be overridden by runtime settings
 * loaded from SettingsManager (e.g., device name, brightness).
 */

// Wi-Fi Manager Defaults
#define DEFAULT_WIFI_SCAN_DURATION_SEC 10     ///< Default duration for Wi-Fi scans in seconds.
#define DEFAULT_WIFI_CONNECT_TIMEOUT_MS 15000 ///< Default timeout for Wi-Fi connection attempts in milliseconds.
#define DEFAULT_NTP_SERVER "pool.ntp.org"     ///< Default NTP server address for time synchronization.
#define DEFAULT_GMT_OFFSET_SEC 3600           ///< Default GMT offset in seconds (e.g., +1 hour for CET).
#define DEFAULT_DAYLIGHT_OFFSET_SEC 3600      ///< Default daylight saving offset in seconds (e.g., +1 hour for CEST).

// Bluetooth Manager Defaults
#define DEFAULT_BLE_SCAN_DURATION_SEC 5       ///< Default duration for Bluetooth Low Energy scans in seconds.

// Autoreconnect Default for Wi-Fi and BLE
#define DEFAULT_AUTORECONNECT_INTERVAL_MS 30000 ///< Default interval in milliseconds for auto-reconnect attempts (30 seconds).

// SD Manager Defaults
#define DEFAULT_SD_CHECK_INTERVAL_MS 2000     ///< Default interval for SD card presence checks in milliseconds.

// Max number of saved items (e.g., Wi-Fi networks, BLE paired devices)
// These define array sizes or vector limits in SettingsManager.
#define MAX_SAVED_WIFI_NETWORKS 5             ///< Maximum number of Wi-Fi networks that can be saved.
#define MAX_PAIRED_BLE_DEVICES 5              ///< Maximum number of BLE devices that can be paired.

#endif // CONFIG_H