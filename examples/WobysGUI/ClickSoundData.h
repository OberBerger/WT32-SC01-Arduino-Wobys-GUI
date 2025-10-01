/**
 * @file ClickSoundData.h
 * @brief Declares raw audio data and size for a system click sound.
 *
 * This header file provides external declarations for the raw WAV format
 * data of a click sound and its size, which are defined in `ClickSoundData.cpp`.
 * This allows other modules (e.g., AudioManager) to access the sound data for playback.
 *
 * @version 1.0.0
 * @date 2025-08-07
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
#ifndef CLICK_SOUND_DATA_H
#define CLICK_SOUND_DATA_H

#include <Arduino.h> // For size_t type

/**
 * @brief External declaration for the raw click sound data array (WAV format).
 *
 * This array contains the binary content of a short WAV file (e.g., 16kHz, 16-bit, mono)
 * used for UI feedback. The actual definition is in `ClickSoundData.cpp`.
 */
extern const unsigned char click_sound_raw_data[];

/**
 * @brief External declaration for the size of the raw click sound data array.
 *
 * This constant specifies the total number of bytes in the `click_sound_raw_data` array.
 */
extern const size_t CLICK_SOUND_DATA_SIZE;

#endif // CLICK_SOUND_DATA_H