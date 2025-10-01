/**
 * @file SDManager.h
 * @brief Defines the SDManager class for managing SD card operations and status.
 *
 * This class provides functionalities for initializing and monitoring an SD card,
 * integrating with the system's SettingsManager for persistent configurations.
 * It periodically checks for card presence and updates a corresponding UI icon
 * in the status bar, ensuring robust filesystem access for other components.
 *
 * @version 1.0.1
 * @date 2025-08-20
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
#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <Arduino.h>  // Required for basic types like `int`, `unsigned long`
#include <FS.h>       // Required for `fs::FS` (Filesystem object)
#include <SD.h>       // Required for `SDClass` and `SD.begin()`
#include "Config.h"   // Required for pin definitions and DEBUG macros
// Forward declarations to avoid circular dependencies and unnecessary includes.
class SPIClass;       // Forward declaration for `SPIClass`
class SettingsManager; // Forward declaration for `SettingsManager`
class IconElement;    // Forward declaration for `IconElement`
struct SDManagerConfig; // Forward declaration for configuration struct (defined in SystemInitializer.h)

/**
 * @brief The SDManager class manages SD card operations and status.
 *
 * This manager is responsible for initializing the SD card, periodically checking
 * its presence, and updating a dedicated UI icon to reflect its status. It provides
 * an interface to the `fs::FS` object, allowing other system components (e.g.,
 * `AudioManager`, `ImageUI`) to access files on the SD card.
 */
class SDManager {
public:
    // --- Constructor & Destructor ---
    /**
     * @brief Constructor for the SDManager class.
     * Initializes the manager with a pointer to the `SettingsManager`.
     * @param settingsManager A pointer to the `SettingsManager` instance.
     */
    SDManager(SettingsManager* settingsManager);

    /**
     * @brief Destructor for the SDManager class.
     * Cleans up dynamically allocated resources, specifically the `SPIClass` instance.
     */
    ~SDManager();

    // --- Initialization & Lifecycle ---
    /**
     * @brief Initializes the SD card with the provided configuration.
     * This method must be called once during the system setup. It attempts to mount
     * the SD card and sets up the SPI bus.
     * @param config The `SDManagerConfig` structure containing pin assignments and check interval.
     * @return True if the SD card was successfully initialized and mounted, false otherwise.
     */
    bool init(const SDManagerConfig& config);

    /**
     * @brief Main loop method for the SDManager.
     * This function should be called repeatedly in the Arduino `loop()` function.
     * It periodically checks for the SD card's presence and updates the UI icon.
     */
    void loop();

    // --- Accessors ---
    /**
     * @brief Retrieves a reference to the `fs::FS` object for SD card access.
     * This allows other components to perform file operations on the SD card.
     * @return A reference to the `fs::FS` object representing the SD card filesystem.
     */
    fs::FS& getFilesystem();

    /**
     * @brief Checks if the SD card is currently present and successfully mounted.
     * @return True if the SD card is present and accessible, false otherwise.
     */
    bool isCardPresent() const { return _isCardPresent; }

    // --- UI Integration ---
    /**
     * @brief Sets the `IconElement` instance used to display the SD card's status.
     * The `SDManager` will update this icon (its character) based on its internal state.
     * @param icon A pointer to the `IconElement` instance for the SD card icon.
     */
    void setSdIconElement(IconElement* icon);

private:
    // --- Dependencies ---
    SettingsManager* _settingsManager; ///< Pointer to the `SettingsManager` instance for configuration.
    const SDManagerConfig* _config;    ///< Pointer to the configuration parameters for the SD card.
    IconElement* _sdIconElement;       ///< Pointer to the `IconElement` in the status bar for visual feedback.
    SPIClass* _spi;                    ///< Dynamically allocated `SPIClass` instance for the SD card's SPI bus.

    // --- Internal State ---
    unsigned long _lastCheckTime;      ///< Timestamp of the last SD card presence check.
    bool _isCardPresent;               ///< Current state of SD card presence (true if present, false otherwise).
    bool _isInitialized;               ///< True if the `SDManager` itself has been successfully initialized.

    // --- Private Helper Methods ---
    /**
     * @brief Periodically checks the presence and mount status of the SD card.
     * If the status changes, it updates the `_isCardPresent` flag and the `_sdIconElement`.
     */
    void _checkCardPresenceAndUpdateIcon();
};

#endif // SD_MANAGER_H