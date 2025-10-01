/**
 * @file ConfigHardwareUser.h
 * @brief User-configurable hardware pin definitions and parameters for the WobysGUI.
 *
 * This file allows integrators and advanced users to adapt the firmware to
 * specific hardware revisions or custom boards that differ from the
 * standard WT32-SC01-Plus setup. These settings are consumed by the
 * main application code and passed to system managers.
 *
 * @version 1.0.1
 * @date 2025-08-03
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
 * This product incorporates software components licensed under various open-source components.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#pragma once

#ifndef CONFIG_HARDWARE_USER_H
#define CONFIG_HARDWARE_USER_H

// Required for basic types like uint8_t.
#include <Arduino.h>

/**
 * @brief Power Management Pin Definitions.
 *
 * Pins used for battery voltage monitoring and system power control.
 */
#define BATT_ADC_PIN 10   ///< Analog input pin for battery voltage sensing.
#define POWER_CTRL_PIN 14 ///< Digital output pin to control system power (e.g., via a relay).

/**
 * @brief RFID Module Pin Definitions.
 *
 * Pins used for SPI communication with the MFRC522 RFID reader.
 */
#define RFID_SS_PIN 21   ///< Slave Select (SS) pin for RFID module.
#define RFID_SCK_PIN 12  ///< Serial Clock (SCK) pin for RFID module.
#define RFID_MOSI_PIN 13 ///< Master Out Slave In (MOSI) pin for RFID module.
#define RFID_MISO_PIN 11 ///< Master In Slave Out (MISO) pin for RFID module.

/**
 * @brief SD Card Module Pin Definitions.
 *
 * Pins used for SPI communication with the SD card module.
 * Based on WT32-SC01 Plus Datasheet (GPIO 41, 40, 39, 38).
 */
#define SD_CS_PIN   41 ///< Chip Select (CS) pin for SD card module.
#define SD_MOSI_PIN 40 ///< Master Out Slave In (MOSI) pin for SD card module.
#define SD_MISO_PIN 38 ///< Master In Slave Out (MISO) pin for SD card module.
#define SD_SCK_PIN  39 ///< Serial Clock (SCK) pin for SD card module.

/**
 * @brief Battery Monitoring Parameters.
 *
 * Configuration for battery voltage calculation and low power shutdown.
 * These parameters are hardware-specific for the battery and voltage divider.
 */
const float R1_VALUE_OHM = 220000.0f; ///< Resistance of the top resistor in the voltage divider (Ohms).
const float R2_VALUE_OHM = 220000.0f; ///< Resistance of the bottom resistor in the voltage divider (Ohms).

const unsigned long BATTERY_CHECK_INTERVAL_MS = 5000; ///< Interval for battery voltage checks (milliseconds).
const float BATT_LOW_THRESHOLD_POWER_OFF_VOLTS = 3.15f; ///< Voltage threshold for automatic system shutdown (Volts).
const float BATT_HYSTERESIS_VOLTS = 0.1f; ///< Hysteresis to prevent rapid on/off cycling around shutdown threshold (Volts).

/**
 * @brief Battery level icons and thresholds.
 *
 * These definitions should match the glyphs available in the 'battery' font
 * (u8g2_font_battery19) used in StatusbarUI. Users can redefine these
 * characters based on their chosen font glyphs.
 */
#define BATT_ICON_LEVEL_6 '6' ///< Icon for highest battery level (e.g., full bars).
#define BATT_ICON_LEVEL_5 '5' ///< Icon for high battery level.
#define BATT_ICON_LEVEL_4 '4' ///< Icon for medium-high battery level.
#define BATT_ICON_LEVEL_3 '3' ///< Icon for medium-low battery level.
#define BATT_ICON_LEVEL_2 '2' ///< Icon for low battery level.
#define BATT_ICON_LEVEL_1 '1' ///< Icon for critically low battery level (before shutdown).
#define BATT_ICON_LEVEL_0 '0' ///< Icon for extremely low battery (can be used below LEVEL_1 but above 0.5V).
#define BATT_ICON_LEVEL_UNKNOWN '?' ///< Icon for unknown or error battery level.

const float BATT_VOLTAGE_LEVEL_6 = 3.90f; ///< Voltage threshold for LEVEL_6 icon.
const float BATT_VOLTAGE_LEVEL_5 = 3.77f; ///< Voltage threshold for LEVEL_5 icon.
const float BATT_VOLTAGE_LEVEL_4 = 3.70f; ///< Voltage threshold for LEVEL_4 icon.
const float BATT_VOLTAGE_LEVEL_3 = 3.63f; ///< Voltage threshold for LEVEL_3 icon.
const float BATT_VOLTAGE_LEVEL_2 = 3.45f; ///< Voltage threshold for LEVEL_2 icon.
const float BATT_VOLTAGE_LEVEL_1 = 3.35f; ///< Voltage threshold for LEVEL_1 icon.

/**
 * @brief Audio Module Pin Definitions.
 *
 * Pins used for I2S communication with the audio amplifier.
 * Based on WT32-SC01 Plus Datasheet (NS4168 chip).
 */
#define AUDIO_I2S_BCLK_PIN 36 ///< Bit Clock pin for I2S audio.
#define AUDIO_I2S_LRCK_PIN 35 ///< Left/Right Clock (Frame Clock) pin for I2S audio.
#define AUDIO_I2S_DOUT_PIN 37 ///< Data Out pin for I2S audio (ESP32 -> Amplifier).

#endif // CONFIG_HARDWARE_USER_H