/**
 * @file ConfigAudioUser.h
 * @brief User-configurable audio output parameters and system sound settings.
 *
 * This file defines default parameters for I2S audio output and references
 * for system sound files, such as the click sound. These settings are consumed
 * by the AudioManager.
 *
 * @version 1.0.8
 * @date 2025-08-05
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
#ifndef CONFIG_AUDIO_USER_H
#define CONFIG_AUDIO_USER_H

/**
 * @brief I2S Audio Output Default Parameters.
 *
 * NOTE: The WT32-SC01 Plus integrated NS4168 chip is mono.
 */
#define AUDIO_I2S_SAMPLE_RATE 16000 ///< Sample rate in Hz.
#define AUDIO_I2S_CHANNELS    1     ///< Number of channels (1 = mono).

/**
 * @brief Volume Default Settings.
 */
#define AUDIO_DEFAULT_VOLUME_PERCENT 50 ///< Default volume (on a 0-100 UI scale).

/**
 * @brief System Sounds (e.g., click sound) File Data.
 */
#define CLICK_SOUND_FILENAME "/click.wav" ///< Name of the click sound file stored on LittleFS.

#endif // CONFIG_AUDIO_USER_H