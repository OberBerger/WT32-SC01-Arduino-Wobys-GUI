/**
 * @file GlobalSystemEvents.h
 * @brief Declares global callback functions for system-wide events.
 *
 * This file provides a centralized mechanism for different parts of the application
 * to trigger core system services (e.g., AudioManager, PowerManager, ScreenManager)
 * without direct knowledge of their concrete classes or methods. Instead of
 * exposing manager pointers, it defines global `std::function` objects that
 * act as an interface. These are initialized by the `SystemInitializer` at boot-up.
 *
 * This approach allows UI elements and other components to invoke globally
 * defined function pointers for common actions and feedback, promoting a clean
 * decoupling and modular architecture.
 *
 * @version 1.0.2
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
#pragma once
#ifndef GLOBAL_SYSTEM_EVENTS_H
#define GLOBAL_SYSTEM_EVENTS_H

#include <functional> // Required for std::function
#include <string>     // Required for std::string

/**
 * @brief Global callback function to request playback of a UI click sound.
 * This function will be assigned the AudioManager's playClickSound method,
 * which internally handles settings checks. Default implementation logs a warning.
 */
extern std::function<void()> g_playClickSound;

/**
 * @brief Global callback function to display a localized message on a central message board.
 *
 * This function provides a unified way for any component to display messages to the user.
 * It is expected to handle localization and presentation via the MessageBoardElement.
 * Default implementation logs a warning.
 *
 * @param messageKey The localization key for the message.
 * @param durationMs The duration in milliseconds to display the message (0 for permanent).
 * @param isError True if the message represents an error or warning, false otherwise.
 *                This can influence visual styling (e.g., color) of the message.
 */
extern std::function<void(const std::string& messageKey, unsigned long durationMs, bool isError)> g_displayLocalizedMessage;

/**
 * @brief Global callback function to query if the screensaver is currently active.
 * This function will be assigned the ScreenSaverManager's isActive method.
 * Default implementation logs a warning and returns false.
 * @return True if the screensaver is active, false otherwise.
 */
extern std::function<bool()> g_isScreensaverActive;


#endif // GLOBAL_SYSTEM_EVENTS_H