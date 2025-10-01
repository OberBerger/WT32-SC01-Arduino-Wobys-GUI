/**
 * @file SystemInitializer.h
 * @brief Declares the SystemInitializer class, the central point for system and UI initialization.
 *
 * Copyright 2025 György Oberländer
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

#ifndef SYSTEM_INITIALIZER_H
#define SYSTEM_INITIALIZER_H

// Core Includes (for types used in initialization)
#include <Arduino.h>
#include "Config.h" // For access to ALL Config.h defines and enums

// For std::unique_ptr
#include <memory>

// Forward Declarations of all Manager and UI classes that need initialization
class LGFX;
class ScreenManager;
class StatusbarUI;
class SettingsUI;
class SettingsManager;
class WifiManager;
class TimeManager;
class BLEManager;
class PowerManager;
class RFIDManager;
class ScreenSaverManager;
class LanguageManager;
class ClockLabelUI;
class MessageBoardElement;
class BLEUI;
class WifiUI;
class MainUI;
class AudioManager;
class SDManager;

// Forward Declaration for statusbar elements (dynamically allocated and owned by SystemInitializer)
class TimeElement;
class IconElement;


//=============================================================================
// Configuration Structs for System Managers
// These structs define the configuration parameters that will be read from
// Config.h and passed to the respective Manager classes during initialization.
// This keeps the Manager APIs clean and centralizes parameter passing.
// These structs are now defined globally for clarity and direct use in _initManagers.
//=============================================================================

/**
 * @brief Configuration parameters for the PowerManager.
 * These will be passed to PowerManager::init().
 */
struct PowerManagerConfig {
    int battAdcPin;                     ///< ADC pin connected to the battery voltage divider.
    int powerCtrlPin;                   ///< Pin controlling the main power relay.
    float r1ValueOhm;                   ///< Resistance of R1 in the voltage divider (top resistor).
    float r2ValueOhm;                   ///< Resistance of R2 in the voltage divider (bottom resistor).
    unsigned long batteryCheckIntervalMs; ///< Interval in milliseconds for checking battery voltage.
    float lowThresholdPowerOffVolts;    ///< Voltage threshold below which the system initiates shutdown.
    float hysteresisVolts;              ///< Hysteresis voltage for battery level changes.
    char battIconLevel6;                ///< Icon for battery level 6.
    char battIconLevel5;                ///< Icon for battery level 5.
    char battIconLevel4;                ///< Icon for battery level 4.
    char battIconLevel3;                ///< Icon for battery level 3.
    char battIconLevel2;                ///< Icon for battery level 2.
    char battIconLevel1;                ///< Icon for battery level 1.
    char battIconLevel0;                ///< Icon for battery level 0 (empty).
    char battIconLevelUnknown;          ///< Icon for unknown battery level.
    float battVoltageLevel6;            ///< Voltage threshold for battery level 6.
    float battVoltageLevel5;            ///< Voltage threshold for battery level 5.
    float battVoltageLevel4;            ///< Voltage threshold for battery level 4.
    float battVoltageLevel3;            ///< Voltage threshold for battery level 3.
    float battVoltageLevel2;            ///< Voltage threshold for battery level 2.
    float battVoltageLevel1;            ///< Voltage threshold for battery level 1.
};

/**
 * @brief Configuration parameters for the RFIDManager.
 * These will be passed to RFIDManager::init().
 */
struct RFIDManagerConfig {
    int ssPin;                          ///< SPI Slave Select pin for the RFID module.
    int sckPin;                         ///< SPI Clock pin for the RFID module.
    int mosiPin;                        ///< SPI Master Out Slave In pin for the RFID module.
    int misoPin;                        ///< SPI Master In Slave Out pin for the RFID module.
};

/**
 * @brief Configuration parameters for the WifiManager.
 */
struct WifiManagerConfig {
    unsigned long defaultScanDurationSec; ///< Default duration for Wi-Fi scans in seconds.
    unsigned long connectTimeoutMs;     ///< Timeout for Wi-Fi connection attempts in milliseconds.
};

/**
 * @brief Configuration parameters for the TimeManager.
 */
struct TimeManagerConfig {
    int gmtOffsetSec;                   ///< GMT offset in seconds.
    int daylightOffsetSec;              ///< Daylight saving offset in seconds.
    const char* ntpServer;              ///< NTP server address.
};

/**
 * @brief Configuration parameters for the ScreenSaverManager.
 */
struct ScreenSaverManagerConfig {
    unsigned long timeoutMs;            ///< Timeout before screensaver activates in milliseconds.
    unsigned long brightDurationMs;     ///< Duration the screen stays bright after interaction in milliseconds.
    uint8_t saverBrightnessDim;         ///< Dimmed brightness level (0-255) for screensaver.
    uint8_t saverBrightnessBright;      ///< Bright brightness level (0-255) after interaction.
    int32_t swipeThresholdY;            ///< Vertical swipe threshold in pixels to dismiss screensaver.
    unsigned long tapMaxDurationMs;     ///< Maximum duration for a tap gesture in milliseconds.
    unsigned long brightenDurationMs;   ///< Duration for brightening animation in milliseconds.
    unsigned long dimDurationMs;        ///< Duration for dimming animation in milliseconds.
    unsigned long brightHoldDurationMs; ///< Duration to hold bright state after gesture in milliseconds.
    unsigned long fadeOutDurationMs;    ///< Duration for fade-out animation in milliseconds.
    unsigned long pulseCycleDurationMs; ///< Duration of one pulse cycle for screensaver elements.
    uint8_t minArrowIntensity;          ///< Minimum intensity for animated arrows.
    int32_t arrowWidthPixels;           ///< Width of the screensaver arrow icons.
    int32_t arrowHeightPixels;          ///< Height of the screensaver arrow icons.
    int32_t arrowOffsetBottomPixels;    ///< Offset from bottom for arrow icons.
    uint32_t arrowBaseColor;            ///< Base color for the screensaver arrows.
};


/**
 * @brief Configuration parameters for the AudioManager.
 */
struct AudioManagerConfig {
    int bclkPin;                        ///< I2S BCLK pin for audio output.
    int lrckPin;                        ///< I2S LRCK pin for audio output.
    int doutPin;                        ///< I2S DOUT pin for audio output.
    int sampleRate;                     ///< I2S sample rate.
    int channels;                       ///< Number of audio channels.
    uint8_t initialVolume_0_100;        ///< Initial volume percentage (0-100).
    bool initialEnabledState;           ///< Initial enabled state for audio.
};

/**
 * @brief Configuration parameters for the SDManager.
 */
struct SDManagerConfig {
    int csPin;                          ///< SPI Chip Select pin for the SD card.
    int mosiPin;                        ///< SPI Master Out Slave In pin for the SD card.
    int misoPin;                        ///< SPI Master In Slave Out pin for the SD card.
    int sckPin;                         ///< SPI Clock pin for the SD card.
    unsigned long checkIntervalMs;      ///< Interval in milliseconds for checking SD card presence.
};

/**
 * @brief Defines the overall system status.
 * This enum tracks the general operational state of the embedded system,
 * allowing for graceful handling of initialization failures and different modes of operation.
 */
enum class SystemStatus {
    BOOTING,        ///< System is currently in the boot-up phase, initializing components.
    OPERATIONAL,    ///< System has initialized successfully and is fully functional.
    CRITICAL_ERROR  ///< A critical error occurred during initialization, leading to a degraded or halted state.
};

/**
 * @brief Declare the global system status variable as extern.
 * Its definition will be in SystemInitializer.cpp.
 */
extern SystemStatus g_systemStatus;


/**
 * @brief Centralized class to handle all system and UI initialization.
 * Reads configurations and passes them to the appropriate objects.
 */
class SystemInitializer {
public:
    //=========================================================================
    // Pointers to System & UI Managers (Globally Instantiated, Not Owned by SystemInitializer)
    // These are managed externally (e.g., in the .ino file).
    //=========================================================================
    LGFX*                 _lcd;                ///< Pointer to the LGFX display instance.
    ScreenManager*        _screenManager;      ///< Pointer to the ScreenManager instance.
    StatusbarUI*          _statusbar;          ///< Pointer to the StatusbarUI instance.
    SettingsManager*      _settingsManager;    ///< Pointer to the SettingsManager instance.
    WifiManager*          _wifiManager;        ///< Pointer to the WifiManager instance.
    TimeManager*          _timeManager;        ///< Pointer to the TimeManager instance.
    BLEManager*           _btManager;          ///< Pointer to the BLEManager instance.
    PowerManager*         _powerManager;       ///< Pointer to the PowerManager instance.
    RFIDManager*          _rfidManager;        ///< Pointer to the RFIDManager instance.
    ScreenSaverManager*   _screenSaverManager; ///< Pointer to the ScreenSaverManager instance.
    LanguageManager*      _languageManager;    ///< Pointer to the LanguageManager instance.
    SettingsUI*           _settingsUI;         ///< Pointer to the SettingsUI instance.
    ClockLabelUI*         _screenSaverClock;   ///< Pointer to the ClockLabelUI instance for the screensaver.
    AudioManager*         _audioManager;       ///< Pointer to the AudioManager instance.
    SDManager*            _sdManager;          ///< Pointer to the SDManager instance.
    
    //=========================================================================
    // Pointers to High-Level UI Controllers (Globally Instantiated, Not Owned by SystemInitializer)
    // These are managed externally (e.g., in the .ino file).
    //=========================================================================
    BLEUI*                _btUI;               ///< Pointer to the BLEUI instance.
    WifiUI*               _wifiUI;             ///< Pointer to the WifiUI instance.
    MainUI*               _mainUI;             ///< Pointer to the MainUI instance.

    //=========================================================================
    // Pointers to Statusbar UI Elements (Dynamically Allocated and Owned by SystemInitializer via unique_ptr)
    // These `unique_ptr` members ensure that memory is automatically freed when SystemInitializer is destroyed.
    // Raw pointers are provided for external components that only need to view the objects.
    //=========================================================================
    std::unique_ptr<MessageBoardElement>  _messageBoardUnique;       ///< Dynamically allocated MessageBoardElement for status messages.
    std::unique_ptr<TimeElement>          _timeElementUnique;        ///< Dynamically allocated TimeElement for displaying time.
    std::unique_ptr<IconElement>          _rfidElementUnique;        ///< Dynamically allocated IconElement for RFID status.
    std::unique_ptr<IconElement>          _speakerElementUnique;     ///< Dynamically allocated IconElement for speaker/audio status.
    std::unique_ptr<IconElement>          _btElementUnique;          ///< Dynamically allocated IconElement for Bluetooth status.
    std::unique_ptr<IconElement>          _wifiElementUnique;        ///< Dynamically allocated IconElement for Wi-Fi status.
    std::unique_ptr<IconElement>          _batteryElementUnique;     ///< Dynamically allocated IconElement for battery status.
    std::unique_ptr<IconElement>          _sdElementUnique;          ///< Dynamically allocated IconElement for SD card status.

    // Raw pointers to the owned elements (for passing to other classes that don't take ownership).
    MessageBoardElement*  _messageBoard;       ///< Raw pointer to the dynamically allocated MessageBoardElement.
    TimeElement*          _timeElement;        ///< Raw pointer to the dynamically allocated TimeElement.
    IconElement*          _rfidElement;        ///< Raw pointer to the dynamically allocated RFID IconElement.
    IconElement*          _speakerElement;     ///< Raw pointer to the dynamically allocated Speaker IconElement.
    IconElement*          _btElement;          ///< Raw pointer to the dynamically allocated Bluetooth IconElement.
    IconElement*          _wifiElement;        ///< Raw pointer to the dynamically allocated Wi-Fi IconElement.
    IconElement*          _batteryElement;     ///< Raw pointer to the dynamically allocated Battery IconElement.
    IconElement*          _sdElement;          ///< Raw pointer to the dynamically allocated SD Card IconElement.

    //=========================================================================
    // Configuration Structs for Managers (Owned by SystemInitializer, Passed by const ref to Managers)
    // These hold the configuration data that is read from Config.h and passed to managers.
    //=========================================================================
    SDManagerConfig       _sdConfig;           ///< Configuration parameters for the SDManager.
    AudioManagerConfig    _audioConfig;        ///< Configuration parameters for the AudioManager.


    /**
     * @brief Constructor for SystemInitializer.
     * Initializes pointers to all global objects that will be managed during initialization.
     * `std::unique_ptr` members are default-constructed to `nullptr`.
     *
     * @param lcdRef Pointer to the LGFX display instance.
     * @param sm Pointer to the ScreenManager instance.
     * @param sb Pointer to the StatusbarUI instance.
     * @param settingsMgr Pointer to the SettingsManager instance.
     * @param wm Pointer to the WifiManager instance.
     * @param tm Pointer to the TimeManager instance.
     * @param bm Pointer to the BLEManager instance.
     * @param pm Pointer to the PowerManager instance.
     * @param rm Pointer to the RFIDManager instance.
     * @param ssm Pointer to the ScreenSaverManager instance.
     * @param ssc Pointer to the ClockLabelUI instance for the screensaver.
     * @param bui Pointer to the BLEUI instance.
     * @param wui Pointer to the WifiUI instance.
     * @param mui Pointer to the MainUI instance.
     * @param lm Pointer to the LanguageManager instance.
     * @param sui Pointer to the SettingsUI instance.
     * @param am Pointer to the AudioManager instance.
     * @param sdm Pointer to the SDManager instance.
     */
    SystemInitializer(
        LGFX* lcdRef, ScreenManager* sm, StatusbarUI* sb,
        SettingsManager* settingsMgr, WifiManager* wm, TimeManager* tm,
        BLEManager* bm, PowerManager* pm, RFIDManager* rm,
        ScreenSaverManager* ssm, ClockLabelUI* ssc,
        BLEUI* bui, WifiUI* wui, MainUI* mui,
        LanguageManager* lm,
        SettingsUI* sui, AudioManager* am,
        SDManager* sdm);

    /**
     * @brief Destructor for SystemInitializer.
     * Frees all dynamically allocated UI elements owned by this initializer.
     * `std::unique_ptr` automatically handles memory deallocation when the `SystemInitializer`
     * object is destroyed.
     */
    ~SystemInitializer(); 

    /**
     * @brief Performs the complete system and UI initialization sequence.
     * This method is called once in the Arduino setup() function.
     * It orchestrates hardware setup, manager initialization, and UI layer definitions.
     */
    void init();

private:
    /**
     * @brief Initializes core system managers by passing configuration data.
     * Reads configurations from Config.h and passes them via structs to managers.
     * Includes robust error handling for manager initialization failures.
     * @return True if all *critical* managers initialize successfully, false otherwise.
     */
    bool _initManagers();

    /**
     * @brief Dynamically allocates and sets up status bar elements.
     * Links system managers to their respective status bar UI elements for updates.
     * Includes robust error handling for memory allocation failures.
     * @return True if all *essential* status bar UI elements were successfully allocated, false otherwise.
     */
    bool _setupUIElements();

    /**
     * @brief Defines and populates UI layers (screens) for the application.
     * Initializes UI controller classes, which in turn define their specific layers.
     * @return True if UI layers were successfully setup, false otherwise.
     */
    bool _setupUILayers();

    /**
     * @brief Adds buttons to the pull-down status bar panel.
     * These buttons trigger various UI screens or system functions.
     */
    void _setupStatusbarPanelButtons();

    /**
     * @brief Displays the boot screen image from the SD card or a fallback text.
     * This method uses block-reading for optimized image loading.
     */
    void _displayBootScreen();
};

#endif // SYSTEM_INITIALIZER_H