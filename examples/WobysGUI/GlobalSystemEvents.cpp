/**
 * @file GlobalSystemEvents.cpp
 * @brief Defines global callback functions for system-wide events.
 *
 * This file provides the definitions (memory allocation and default initialization)
 * for the global `std::function` objects declared in `GlobalSystemEvents.h`.
 * These callbacks act as a centralized interface for triggering core system services,
 * such as playing UI click sounds, managing screen navigation, or displaying messages.
 * They are later bound to concrete manager methods by the `SystemInitializer`
 * during the system's boot-up sequence.
 *
 * The default implementations for these callbacks log a warning if they are
 * called before being properly set by the `SystemInitializer`, ensuring graceful
 * behavior and providing diagnostic feedback in uninitialized states.
 *
 * @version 1.0.1
 * @date 2025-09-13
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
 */

#include "GlobalSystemEvents.h"
#include "Config.h" // Required for DEBUG_WARN_PRINTLN

// Global variable definitions for the extern declarations in GlobalSystemEvents.h

/**
 * @brief Global callback function to request playback of a UI click sound.
 *
 * This function is defined with a default lambda that logs a warning if called
 * before being properly bound by the SystemInitializer. It ensures safe
 * execution even in uninitialized states.
 */
std::function<void()> g_playClickSound = [](){ DEBUG_WARN_PRINTLN("Global: g_playClickSound callback not yet set or AudioManager not initialized!"); };

/**
 * @brief Global callback function to display a localized message on a central message board.
 *
 * This function is defined with a default lambda that logs a warning if called
 * before being properly bound by the SystemInitializer. It provides a fallback
 * mechanism for displaying messages in early initialization stages.
 */
std::function<void(const std::string& messageKey, unsigned long durationMs, bool isError)> g_displayLocalizedMessage =
    [](const std::string& messageKey, unsigned long durationMs, bool isError) {
        DEBUG_WARN_PRINTF("Global: g_displayLocalizedMessage callback not yet set or MessageBoard/LanguageManager not initialized! Key: %s, Duration: %lu, Error: %s\n",
                           messageKey.c_str(), durationMs, isError ? "true" : "false");
    };

/**
 * @brief Global callback function to query if the screensaver is currently active.
 *
 * This function is defined with a default lambda that logs a warning if called
 * before being properly bound by the SystemInitializer. It ensures safe
 * execution and provides a default 'false' value in uninitialized states.
 */
std::function<bool()> g_isScreensaverActive = [](){
    DEBUG_WARN_PRINTLN("Global: g_isScreensaverActive callback not yet set or ScreenSaverManager not initialized!");
    return false;
};