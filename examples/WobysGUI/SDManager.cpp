/**
 * @file SDManager.cpp
 * @brief Implements the SDManager class for managing SD card operations and status.
 *
 * This file provides the core logic for initializing the SD card's SPI bus,
 * mounting the filesystem, periodically checking for card presence, and updating
 * a dedicated UI icon in the status bar. It ensures robust interaction with the SD card
 * for reading files and integrates with the system's settings.
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
#include "SDManager.h"
#include <SPI.h> // Required for `SPI.begin()` and `SPIClass` instance
// Required for `SDManagerConfig` struct definition (defined in SystemInitializer.h).
#include "SystemInitializer.h"
#include "IconElement.h" // Required for `IconElement` class definition

// --- Constructor Implementation ---
/**
 * @brief Constructor for the SDManager class.
 * Initializes the manager with a pointer to the `SettingsManager`.
 * @param settingsManager A pointer to the `SettingsManager` instance.
 */
SDManager::SDManager(SettingsManager* settingsManager)
    : _settingsManager(settingsManager),
      _config(nullptr),
      _spi(nullptr),
      _sdIconElement(nullptr), // Initialize pointer to nullptr.
      _lastCheckTime(0),
      _isCardPresent(false),
      _isInitialized(false)
{
    DEBUG_INFO_PRINTLN("SDManager: Constructor executed.");
}

// --- Destructor Implementation ---
/**
 * @brief Destructor for the SDManager class.
 * Cleans up dynamically allocated resources, specifically the `SPIClass` instance.
 */
SDManager::~SDManager() {
    DEBUG_INFO_PRINTLN("SDManager: Destructor executed. Freeing SPI resources.");
    if (_spi) {
        _spi->end(); // Terminate the SPI bus.
        delete _spi; // Free the dynamically allocated `SPIClass` object.
        _spi = nullptr;
    }
}

// --- Initialization & Lifecycle ---
/**
 * @brief Initializes the SD card with the provided configuration.
 * This method must be called once during the system setup. It attempts to mount
 * the SD card and sets up the SPI bus.
 * @param config The `SDManagerConfig` structure containing pin assignments and check interval.
 * @return True if the SD card was successfully initialized and mounted, false otherwise.
 */
bool SDManager::init(const SDManagerConfig& config) {
    DEBUG_INFO_PRINTLN("SDManager: Initialization started...");
    _config = &config; // Store the pointer to the configuration.

    // Dynamically allocate `SPIClass` instance.
    _spi = new (std::nothrow) SPIClass(HSPI);
    if (_spi == nullptr) {
        DEBUG_ERROR_PRINTLN("SDManager: ERROR - Failed to allocate SPIClass instance! Out of memory.");
        _isInitialized = false;
        return false;
    }

    // Initialize the SPI bus with the SD card's assigned pins.
    _spi->begin(_config->sckPin, _config->misoPin, _config->mosiPin, _config->csPin);

    // Attempt to mount the SD card filesystem.
    // The `SD.begin()` function takes the CS pin and a reference to the `SPIClass` instance.
    if (!SD.begin(_config->csPin, *_spi)) {
        DEBUG_WARN_PRINTLN("SDManager: WARNING - SD.begin() failed or card not present.");
        _isCardPresent = false;
        _isInitialized = true;
        _checkCardPresenceAndUpdateIcon(); // Update icon even if init failed.
        return false;
    }

    DEBUG_INFO_PRINTLN("SDManager: SD Card successfully mounted.");
    _isCardPresent = true;
    _isInitialized = true;
    _checkCardPresenceAndUpdateIcon(); // Update icon for successful mount.
    return true;
}

/**
 * @brief Main loop method for the SDManager.
 * This function should be called repeatedly in the Arduino `loop()` function.
 * It periodically checks for the SD card's presence and updates the UI icon.
 */
void SDManager::loop() {
    if (!_isInitialized) return; // Skip if manager not initialized.

    unsigned long currentTime = millis();
    if (currentTime - _lastCheckTime >= _config->checkIntervalMs) {
        _lastCheckTime = currentTime;
        _checkCardPresenceAndUpdateIcon(); // Perform periodic check.
    }
}

// --- Accessors ---
/**
 * @brief Retrieves a reference to the `fs::FS` object for SD card access.
 * This allows other components to perform file operations on the SD card.
 * @return A reference to the `fs::FS` object representing the SD card filesystem.
 */
fs::FS& SDManager::getFilesystem() {
    return SD; // Returns a reference to the global SD filesystem object.
}

// --- UI Integration ---
/**
 * @brief Sets the `IconElement` instance used to display the SD card's status.
 * The `SDManager` will update this icon (its character) based on its internal state.
 * @param icon A pointer to the `IconElement` instance for the SD card icon.
 */
void SDManager::setSdIconElement(IconElement* icon) {
    _sdIconElement = icon;
    // The initial icon state will be set by `_checkCardPresenceAndUpdateIcon()`
    // which is called during `init()` and subsequently by `loop()`.
    DEBUG_INFO_PRINTLN("SDManager: SD icon element set.");
}

// --- Private Helper Methods ---
/**
 * @brief Periodically checks the presence and mount status of the SD card.
 * If the status changes, it updates the `_isCardPresent` flag and the `_sdIconElement`.
 */
void SDManager:: _checkCardPresenceAndUpdateIcon() {
    // DEBUG_TRACE_PRINTLN("SDManager: Checking card presence...");

    bool newCardStatus;

    // Attempt to unmount the filesystem first to ensure a clean re-initialization.
    // `SD.end()` is safe to call even if nothing is currently mounted.
    SD.end(); 
    
    // Attempt to initialize (mount) the SD card. This is the most reliable way
    // to determine if the card is present and functional.
    if (SD.begin(_config->csPin, *_spi)) {
        newCardStatus = true;
        // DEBUG_TRACE_PRINTLN("SDManager: SD.begin() successful. Card detected and mounted.");
    } else {
        newCardStatus = false;
        // DEBUG_TRACE_PRINTLN("SDManager: SD.begin() failed. Card not detected or error.");
    }

    // Update the card presence status and the UI icon only if a change occurred.
    if (_isCardPresent != newCardStatus) {
        _isCardPresent = newCardStatus;
        if (_sdIconElement) { // Safely check if the IconElement pointer is valid.
            if (_isCardPresent) {
                _sdIconElement->setIcon(SD_CARD_DEFAULT_ICON_CHAR); // Display SD card icon.
                DEBUG_INFO_PRINTLN("SDManager: SD icon updated: Card detected.");
            } else {
                _sdIconElement->setIcon(" "); // Display blank icon (hide).
                DEBUG_INFO_PRINTLN("SDManager: SD icon updated: Card removed or error.");
            }
        } else {
            DEBUG_WARN_PRINTLN("SDManager: WARNING - _sdIconElement is nullptr, cannot update icon.");
        }
    }
}