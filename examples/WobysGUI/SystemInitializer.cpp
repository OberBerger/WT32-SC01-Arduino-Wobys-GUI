/**
 * @file SystemInitializer.cpp
 * @brief Implementation of the SystemInitializer class.
 *
 * This file contains the detailed implementation of the system and UI
 * initialization logic, reading configurations from Config.h and
 * orchestrating the setup of all global objects.
 *
 * @version 1.0.9
 * @date 2025-08-15
 * @author György Oberländer. All Rights Reserved.
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

#include "SystemInitializer.h"

// All custom configuration defines are now centralized in Config.h.
#include "Config.h" 

// Include all necessary headers for the types being initialized or passed around
#include <SPI.h> // For SPI.begin()
#include <vector>

#include "ConfigLGFXUser.h" 
#include "LanguageManager.h"
#include "ScreenManager.h"
#include "StatusbarUI.h"
#include "SettingsManager.h"
#include "WifiManager.h"
#include "TimeManager.h"
#include "BLEManager.h"
#include "PowerManager.h"
#include "RFIDManager.h"
#include "ScreenSaverManager.h"
#include "ClockLabelUI.h"
#include "MessageBoardElement.h"
#include "BLEUI.h"
#include "WifiUI.h"
#include "MainUI.h"
#include "SettingsUI.h"
#include "AudioManager.h"
#include "SDManager.h"
#include "GlobalSystemEvents.h" // Include the new global event header

// Specific UI Elements used in setup (dynamically allocated)
#include "TimeElement.h"
#include "IconElement.h"

//=============================================================================
// Global System Status (new)
// This enum tracks the overall operational state of the system, allowing
// the main loop to react gracefully to critical initialization failures.
//=============================================================================

SystemStatus g_systemStatus = SystemStatus::BOOTING;    ///< Declare as extern to access the global variable defined in .ino
                                                        ///< Or if g_systemStatus is local to this file, then initialize here:
                                                        ///< SystemStatus g_systemStatus = SystemStatus::BOOTING; 
                                                        ///< However, the .ino suggests it's global, so `extern` is appropriate.


//=============================================================================
// Global Callback Function Declarations
// These functions are defined in the .ino file and are linked here.
//=============================================================================
extern void initiateShutdown();
extern void openWiFiSettingsPanel();
extern void openBluetoothSettingsPanel();
extern void openSettingsScreen();
extern void handleShutdownWarning(const std::string& messageKey);
extern void handlePerformShutdownTasks();
extern void handleCardScanned(const RFIDCardData& cardData);


//=============================================================================
// Class SystemInitializer Implementation
//=============================================================================

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
SystemInitializer::SystemInitializer(
    LGFX* lcdRef, ScreenManager* sm, StatusbarUI* sb,
    SettingsManager* settingsMgr, WifiManager* wm, TimeManager* tm,
    BLEManager* bm, PowerManager* pm, RFIDManager* rm,
    ScreenSaverManager* ssm, ClockLabelUI* ssc,
    BLEUI* bui, WifiUI* wui, MainUI* mui,
    LanguageManager* lm,
    SettingsUI* sui, AudioManager* am, SDManager* sdm)
    : _lcd(lcdRef), _screenManager(sm), _statusbar(sb),
      _settingsManager(settingsMgr), _wifiManager(wm), _timeManager(tm),
      _btManager(bm), _powerManager(pm), _rfidManager(rm),
      _screenSaverManager(ssm), _screenSaverClock(ssc),
      _btUI(bui), _wifiUI(wui), _mainUI(mui),
      _languageManager(lm),
      _settingsUI(sui),
      _audioManager(am), _sdManager(sdm),
      // Raw pointers (temporarily nullptr, will be assigned during _setupUIElements).
      _messageBoard(nullptr), _timeElement(nullptr), _rfidElement(nullptr),
      _speakerElement(nullptr), _btElement(nullptr), _wifiElement(nullptr), _sdElement(nullptr),
      _batteryElement(nullptr),
      // `std::unique_ptr` members are default-initialized to `nullptr`.
      // Configuration structs are initialized here.
      _sdConfig({
        .csPin = SD_CS_PIN, .mosiPin = SD_MOSI_PIN, .misoPin = SD_MISO_PIN, .sckPin = SD_SCK_PIN,
        .checkIntervalMs = DEFAULT_SD_CHECK_INTERVAL_MS
      }),
      _audioConfig({
            .bclkPin = AUDIO_I2S_BCLK_PIN, .lrckPin = AUDIO_I2S_LRCK_PIN, .doutPin = AUDIO_I2S_DOUT_PIN,
            .sampleRate = AUDIO_I2S_SAMPLE_RATE, .channels = AUDIO_I2S_CHANNELS,
            .initialVolume_0_100 = (uint8_t)AUDIO_DEFAULT_VOLUME_PERCENT,
            .initialEnabledState = true
      })
{
    DEBUG_INFO_PRINTLN("SystemInitializer: Constructor called.");
}

/**
 * @brief Destructor for SystemInitializer.
 * Frees all dynamically allocated UI elements owned by this initializer.
 * `std::unique_ptr` automatically handles memory deallocation when the `SystemInitializer`
 * object is destroyed. Raw pointers (`_messageBoard`, `_timeElement` etc.)
 * are merely views and do not own the memory, so no explicit `delete` calls are needed for them.
 */
SystemInitializer::~SystemInitializer() {
    DEBUG_INFO_PRINTLN("SystemInitializer: Destructor called. Dynamically allocated UI elements will be automatically freed by unique_ptr.");
    // `std::unique_ptr` members are automatically destroyed and memory freed.
    // No explicit `delete` calls are needed for `_messageBoardUnique` etc.
}

/**
 * @brief Performs the complete system and UI initialization sequence.
 * This method is called once in the Arduino `setup()` function.
 * It orchestrates hardware setup, manager initialization, and UI layer definitions.
 */
void SystemInitializer::init() {
    DEBUG_INFO_PRINTLN("\n--- SystemInitializer: Starting Initialization ---");

    // Critical: Check if the LGFX LCD pointer is valid. Without it, no UI can function.
    if (!_lcd) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - LGFX LCD pointer is nullptr! Cannot initialize GUI. Halting system.");
        g_systemStatus = SystemStatus::CRITICAL_ERROR;
        while (true) delay(100); // Halting in a critical error state.
    }

    // Enable power relay early.
    // This is not considered a critical failure to halt the system, as power may already be on.
    if (_powerManager) {
        _powerManager->enablePowerRelay(POWER_CTRL_PIN);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - PowerManager is nullptr. Cannot enable power relay.");
    }

    // Step 1: Initialize absolute minimum for boot screen display.
    _lcd->init();
    
    // Initialize SD Manager early to potentially display boot screen from SD.
    bool sd_init_ok = false;
    if (_sdManager) {
        if (!_sdManager->init(_sdConfig)) {
            DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - SDManager failed to initialize.");
            // Error message for SD will be displayed on the message board later, after `_messageBoard` is ready.
        } else {
            sd_init_ok = true;
            if (_audioManager) {
                _audioManager->setSdFilesystem(&SD);
                DEBUG_INFO_PRINTLN("SystemInitializer: AudioManager SD filesystem set.");
            } else {
                DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - AudioManager pointer is nullptr. Cannot set SD filesystem for audio.");
            }
        }
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - SDManager pointer is nullptr. Skipping SD initialization.");
    }


    // Step 2: Display the boot screen.
    _displayBootScreen();

    delay(1000); 

    // Step 3: Initialize the rest of the system.
    SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN);
    
    // Initialize LittleFS for settings. This is critical.
    if (!LittleFS.begin()) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: ERROR - LittleFS initialization failed. Critical.");
        // Error message for LittleFS will be displayed on the message board later.
    } else {
        DEBUG_INFO_PRINTLN("SystemInitializer: LittleFS initialized.");
    }
    
    // First, dynamically create and setup essential UI elements.
    // This is a critical step, as managers rely on these for status display.
    if (!_setupUIElements()) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - Failed to setup essential UI elements! Halting system.");
        g_systemStatus = SystemStatus::CRITICAL_ERROR;
        while (true) delay(100); // Halting in a critical error state.
    }

    // Pass `_messageBoard` raw pointer to main UI controller.
    // `_messageBoard` is guaranteed to be non-null after `_setupUIElements` if no critical error occurred.
    if (_mainUI) {
        _mainUI->setMessageBoard(_messageBoard);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - MainUI pointer is nullptr. Cannot set message board.");
    }

    // Then, initialize managers that use these UI elements.
    // This is a critical step.
    if (!_initManagers()) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - Failed to initialize essential managers! Halting system.");
        g_systemStatus = SystemStatus::CRITICAL_ERROR;
        while (true) delay(100); // Halting in a critical error state.
    }

    // Bind the global g_displayLocalizedMessage callback after MessageBoard and LanguageManager are initialized.
    // This enables centralized message display across the system. This must happen only once, here.
    if (_messageBoard && _languageManager) {
        g_displayLocalizedMessage = [this](const std::string& messageKey, unsigned long durationMs, bool isError) {
            std::string localizedMessage = _languageManager->getString(messageKey, messageKey);
            uint32_t messageColor = isError ? UI_COLOR_WARNING : UI_COLOR_TEXT_DEFAULT; 
            this->_messageBoard->pushMessage(localizedMessage, durationMs, messageColor);
        };
        DEBUG_INFO_PRINTLN("SystemInitializer: Global g_displayLocalizedMessage callback linked.");
    } else {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - MessageBoard or LanguageManager not ready for g_displayLocalizedMessage initialization.");
    }


    /*******************************************************************************
     *                      DEMO MODE - START
     ******************************************************************************/
    #ifdef DEMO_MODE
    DEBUG_INFO_PRINTLN("SystemInitializer: Activating DEMO MODE features using g_displayLocalizedMessage.");

    // Directly use g_displayLocalizedMessage for demo messages
    if (g_displayLocalizedMessage) {
        g_displayLocalizedMessage("DEMO_MODE_ACTIVATED", 5000, false); // Show initial demo activated message
        DEBUG_INFO_PRINTLN("SystemInitializer: 'Demo Mode Activated' message sent via g_displayLocalizedMessage.");
    } else {
        // Fallback if g_displayLocalizedMessage somehow failed to initialize (shouldn't happen here)
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - g_displayLocalizedMessage not ready for demo message.");
    }
    #endif // DEMO_MODE
    /*******************************************************************************
     *                      DEMO MODE - END
     ******************************************************************************/  
    
    // Now that `_messageBoard` and `g_displayLocalizedMessage` are guaranteed to be initialized,
    // display any deferred error messages using the global callback.
    // If LittleFS failed earlier, display an error message on the message board.
    if (!LittleFS.begin() && g_displayLocalizedMessage) { 
        g_displayLocalizedMessage("INIT_FS_ERROR", 5000, true);
    }

    // If SD card initialization failed earlier, display an error message on the message board.
    if (!sd_init_ok && g_displayLocalizedMessage) {
        g_displayLocalizedMessage("INIT_SD_ERROR", 5000, true); 
    }
    
    // Setup other UI layers (screens).
    // The return type of _setupUILayers is now bool, so we check it
    if (!_setupUILayers()) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - Failed to setup UI Layers! Halting system.");
        g_systemStatus = SystemStatus::CRITICAL_ERROR;
        while (true) delay(100);
    }
    _setupStatusbarPanelButtons();

    // Apply saved display brightness.
    if (_settingsManager && _lcd) {
        int savedBrightness = _settingsManager->getBrightness(80);
        _lcd->setBrightness(savedBrightness);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - SettingsManager or LCD pointer is nullptr. Cannot apply saved brightness.");
        if (_lcd) _lcd->setBrightness(80); // Fallback brightness if LCD is valid.
    }

    DEBUG_INFO_PRINTLN("SystemInitializer: Setting final statusbar color.");
    if (_statusbar) {
        _statusbar->setBackgroundColor(PANEL_BACKGROUND_COLOR); 
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Statusbar pointer is nullptr. Cannot set background color.");
    }

    // Play boot sound if audio is enabled
    if (_audioManager && _audioManager->isEnabled() && sd_init_ok) {
        _audioManager->playFile("/Boot_sound.wav");
        DEBUG_INFO_PRINTLN("SystemInitializer: Attempting to play boot sound.");
    }

    g_systemStatus = SystemStatus::OPERATIONAL; // System is now fully operational.
    DEBUG_INFO_PRINTLN("--- SystemInitializer: Initialization Complete ---");
}

//=============================================================================
// Private Helper Methods for Initialization
//=============================================================================

/**
 * @brief Displays the boot screen image from the SD card or a fallback text.
 * This method uses block-reading for optimized image loading.
 */
void SystemInitializer::_displayBootScreen() {
    const char* bootImagePath = "/Boot_logo.jpg";
    DEBUG_INFO_PRINTLN("SystemInitializer: Attempting to display boot screen...");

    // Critical null pointer check. These should be available by this point for a usable boot screen.
    if (!_lcd || !_languageManager) { 
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - LCD or LanguageManager pointer is nullptr during boot screen display. Cannot proceed gracefully.");
        // Fallback to basic LCD clear and red text if possible.
        if (_lcd) {
            _lcd->fillScreen(TFT_BLACK);
            _lcd->setFont(&profont29);
            _lcd->setTextColor(TFT_RED);
            _lcd->setTextDatum(MC_DATUM);
            _lcd->drawString("FATAL ERROR: No Lang/LCD", _lcd->width()/2, _lcd->height()/2);
        }
        return;
    }

    // Check if SD Manager is initialized and SD card is present.
    if (_sdManager && _sdManager->isCardPresent()) {
        File imgFile = SD.open(bootImagePath, FILE_READ);
        if (imgFile && imgFile.size() > 0) {
            size_t fileSize = imgFile.size();
            DEBUG_INFO_PRINTF("SystemInitializer: Boot image file opened, size: %u bytes.\n", fileSize);
            std::vector<uint8_t> jpgBuffer;
            jpgBuffer.resize(fileSize); // Resize vector to file size.
            imgFile.read(jpgBuffer.data(), fileSize);
            imgFile.close();
            _lcd->drawJpg(jpgBuffer.data(), jpgBuffer.size(), 0, 0);
            DEBUG_INFO_PRINTLN("SystemInitializer: Boot screen drawn from RAM.");
        } else {
            DEBUG_ERROR_PRINTF("SystemInitializer: ERROR - Failed to open boot image: %s\n", bootImagePath);
            // Fallback: If SD card is present but file not found.
            _lcd->fillScreen(UI_COLOR_BACKGROUND_DARK);
            _lcd->setFont(&helvB24);
            _lcd->setTextColor(UI_COLOR_TEXT_DEFAULT);
            _lcd->setTextDatum(MC_DATUM);
            _lcd->drawString(_languageManager->getString("BOOT_IMAGE_NOT_FOUND", "Boot image not found!").c_str(), _lcd->width() / 2, _lcd->height() / 2);
        }
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - SD card not found or not initialized. Displaying custom fallback text.");
        
        _lcd->fillScreen(UI_COLOR_BACKGROUND_DARK);
        _lcd->setFont(&profont29); // Using a robust built-in font for fallback.

        // Branding text, typically not localized unless system-wide language changes it.
        const char* text1 = "Wobys.com";
        const char* text2 = " // GUI";

        const uint32_t color1 = 0xF09619; // A distinct color.
        const uint32_t color2 = 0x7F8C8D; // Another distinct color.

        int16_t width1 = _lcd->textWidth(text1);
        int16_t width2 = _lcd->textWidth(text2);
        int16_t totalWidth = width1 + width2;
        int16_t startX = (_lcd->width() - totalWidth) / 2;
        int16_t y = (_lcd->height() - _lcd->fontHeight()) / 2;
        _lcd->setTextDatum(TL_DATUM); // Top-Left datum for drawing.

        _lcd->setTextColor(color1);
        _lcd->drawString(text1, startX, y);

        _lcd->setTextColor(color2);
        _lcd->drawString(text2, startX + width1, y);
    }
}

/**
 * @brief Initializes core system managers by passing configuration data.
 * Reads configurations from Config.h and passes them via structs to managers.
 * Includes robust error handling for manager initialization failures.
 * @return True if all *critical* managers initialize successfully, false otherwise.
 */
bool SystemInitializer::_initManagers() {
    DEBUG_INFO_PRINTLN("SystemInitializer: Initializing Managers...");

    // Critical null pointer check for LanguageManager.
    // If _languageManager is null, localization won't work, affecting error messages.
    if (!_languageManager) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - LanguageManager is nullptr. Cannot localize manager initialization messages! Critical.");
        return false; // Critical failure.
    }

    // --- PowerManager Configuration ---
    // Not strictly critical to halt if battery management fails, but important to log.
    if (_powerManager && _lcd && _settingsManager) {
        PowerManagerConfig pmConfig = {
            .battAdcPin = BATT_ADC_PIN, .powerCtrlPin = POWER_CTRL_PIN,
            .r1ValueOhm = R1_VALUE_OHM, .r2ValueOhm = R2_VALUE_OHM,
            .batteryCheckIntervalMs = BATTERY_CHECK_INTERVAL_MS,
            .lowThresholdPowerOffVolts = BATT_LOW_THRESHOLD_POWER_OFF_VOLTS,
            .hysteresisVolts = BATT_HYSTERESIS_VOLTS,
            .battIconLevel6 = BATT_ICON_LEVEL_6, .battIconLevel5 = BATT_ICON_LEVEL_5,
            .battIconLevel4 = BATT_ICON_LEVEL_4, .battIconLevel3 = BATT_ICON_LEVEL_3,
            .battIconLevel2 = BATT_ICON_LEVEL_2, .battIconLevel1 = BATT_ICON_LEVEL_1,
            .battIconLevel0 = BATT_ICON_LEVEL_0, .battIconLevelUnknown = BATT_ICON_LEVEL_UNKNOWN,
            .battVoltageLevel6 = BATT_VOLTAGE_LEVEL_6, .battVoltageLevel5 = BATT_VOLTAGE_LEVEL_5,
            .battVoltageLevel4 = BATT_VOLTAGE_LEVEL_4, .battVoltageLevel3 = BATT_VOLTAGE_LEVEL_3,
            .battVoltageLevel2 = BATT_VOLTAGE_LEVEL_2, .battVoltageLevel1 = BATT_VOLTAGE_LEVEL_1
        };
        // PowerManager init itself doesn't return bool, its state reflects later.
        _powerManager->init(pmConfig);
        _powerManager->setOnShutdownWarningCallback(handleShutdownWarning);
        _powerManager->setOnPerformShutdownCallback(handlePerformShutdownTasks);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - PowerManager, LCD or SettingsManager pointer is nullptr. Skipping PowerManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_POWER_MGR_FAILED", "PowerMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }


    // --- SettingsManager init (Critical for all persistent settings) ---
    if (_settingsManager) {
        if (!_settingsManager->init()) { // SettingsManager init returns bool, indicating true failure.
            DEBUG_ERROR_PRINTLN("SystemInitializer: ERROR - SettingsManager failed to initialize. Critical.");
            if (_messageBoard) { 
                std::string localizedMsg = _languageManager->getString("INIT_SETTINGS_MGR_FAILED", "SettingsMgr Init Failed!");
                _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
            }
            return false; // Critical failure.
        }
    } else {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - SettingsManager pointer is nullptr. Critical. Halting system.");
        return false; // Critical failure.
    }

    // --- LanguageManager init (Critical for UI localization) ---
    // LanguageManager init depends on SettingsManager, which must be successful.
    if (_languageManager && _settingsManager) {
        // LanguageManager init doesn't return bool, it just attempts to load.
        _languageManager->init(_settingsManager);
        _languageManager->setDiacriticConversionEnabled(true);
    } else {
        // This branch should ideally not be hit if prior _settingsManager check passed.
        DEBUG_ERROR_PRINTLN("SystemInitializer: ERROR - LanguageManager or SettingsManager pointer is nullptr. Critical. Halting system.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_LANG_MGR_FAILED", "LangMgr Init Failed!"); // Uses existing _languageManager as fallback if possible.
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
        return false; // Critical failure.
    }

    // --- WifiManager Configuration (Not critical to halt the system) ---
    // WifiManager init depends on BLEManager for coexistence, and SettingsManager for saved networks.
    if (_wifiManager && _settingsManager && _btManager) {
        WifiManagerConfig wifiConfig = {
            .defaultScanDurationSec = DEFAULT_WIFI_SCAN_DURATION_SEC,
            .connectTimeoutMs = DEFAULT_WIFI_CONNECT_TIMEOUT_MS
        };
        // WifiManager::init() configures basics, but does not *enable/disable* based on settings itself anymore.
        // It's the responsibility of SystemInitializer to call enableWifi/disableWifi.
        _wifiManager->init(wifiConfig); 
        
        // Now, apply the actual enable/disable state based on settings.
        bool wasWifiEnabledInSettings = _settingsManager->getWifiEnabledLastState(false);
        bool autoConnectWifi = _settingsManager->isWifiAutoConnectEnabled(true);

        if (wasWifiEnabledInSettings) {
            _wifiManager->enableWifi(autoConnectWifi);
        } else {
            _wifiManager->disableWifi();
        }
        // No warning here if _wifiManager->isWifiLogicEnabled() is false, as it could be intentional.
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - WifiManager, SettingsManager or BLEManager pointer is nullptr. Skipping WifiManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_WIFI_MGR_FAILED", "WifiMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }

    // --- TimeManager Configuration (Not critical if no network time needed) ---
    if (_timeManager && _wifiManager) { // TimeManager depends on WifiManager.
        // TimeManager::begin() does not return bool, it just starts the process.
        _timeManager->begin(); 
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - TimeManager or WifiManager pointer is nullptr. Skipping TimeManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_TIME_MGR_FAILED", "TimeMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }
    // --- BLEManager init (Not critical to halt the system) ---
    if (_btManager && _settingsManager && _screenManager && _btUI) {
        // BLEManager::init() now only initializes the BLE stack, does not enable/disable based on settings.
        _btManager->init(); 

        // Now, apply the actual enable/disable state based on settings.
        bool wasBleEnabledInSettings = _settingsManager->getBluetoothEnabledLastState(false);
        // `enableBluetooth` in BLEManager handles its own auto-scan/connect logic based on its parameter.
        // `disableBluetooth` also handles cleanup correctly.
        if (wasBleEnabledInSettings) {
            _btManager->enableBluetooth(true); // Attempt auto-scan/connect if enabled.
        } else {
            _btManager->disableBluetooth(); // Explicitly disable if settings say so.
        }
        // No warning here if _btManager->isEnabled() is false, as it could be intentional.

        // Register the BLEUI's member function as the callback
        if (_btUI) { 
            _btManager->setOnPairedDeviceChangedCallback(
                [this](const PairedDevice& device, bool added) {
                    this->_btUI->handlePairedDeviceChanged(device, added);
                }
            );
        }
        _btManager->setAutoReconnectEnabled(true);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - BLEManager, SettingsManager, ScreenManager or BLEUI pointer is nullptr. Skipping BLEManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_BLE_MGR_FAILED", "BLEMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }

    // --- ScreenSaverManager Configuration (Not critical to halt the system) ---
    if (_screenSaverManager && _settingsManager && _screenSaverClock && _screenManager && _statusbar && _timeManager) {
        ScreenSaverManagerConfig screensaverConfig = {
            .timeoutMs = SCREENSAVER_TIMEOUT_MS, .brightDurationMs = SCREENSAVER_BRIGHT_DURATION_MS,
            .saverBrightnessDim = SCREENSAVER_BRIGHTNESS_DIM, .saverBrightnessBright = SCREENSAVER_BRIGHTNESS_BRIGHT,
            .swipeThresholdY = SCREENSAVER_SWIPE_THRESHOLD_Y, .tapMaxDurationMs = SCREENSAVER_TAP_MAX_DURATION_MS,
            .brightenDurationMs = SCREENSAVER_BRIGHTEN_DURATION_MS, .dimDurationMs = SCREENSAVER_DIM_DURATION_MS,
            .brightHoldDurationMs = SCREENSAVER_BRIGHT_HOLD_DURATION_MS, .fadeOutDurationMs = SCREENSAVER_FADE_OUT_DURATION_MS,
            .pulseCycleDurationMs = SCREENSAVER_PULSE_CYCLE_DURATION_MS, .minArrowIntensity = SCREENSAVER_MIN_ARROW_INTENSITY,
            .arrowWidthPixels = SCREENSAVER_ARROW_WIDTH_PIXELS, .arrowHeightPixels = SCREENSAVER_ARROW_HEIGHT_PIXELS,
            .arrowOffsetBottomPixels = SCREENSAVER_ARROW_OFFSET_BOTTOM_PIXELS, .arrowBaseColor = SCREENSAVER_ARROW_BASE_COLOR
        };
        // ScreenSaverManager init doesn't return bool.
        _screenSaverManager->init(screensaverConfig);
        // Apply enabled state based on settings.
        _screenSaverManager->setEnabled(_settingsManager->isScreensaverEnabled(true));
        _screenSaverManager->setTimeout(_settingsManager->getScreensaverTimeout(30) * 1000UL); 
        _screenSaverManager->setDimmedBrightness(_settingsManager->getScreensaverBrightness(10));
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - ScreensaverManager or its dependencies are nullptr. Skipping ScreenSaverManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_SSAVER_MGR_FAILED", "SSaverMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }

    // Bind the global g_isScreensaverActive callback
    if (_screenSaverManager) {
        g_isScreensaverActive = [this]() { return this->_screenSaverManager->isActive(); };
        DEBUG_INFO_PRINTLN("SystemInitializer: Global g_isScreensaverActive callback linked.");
    } else {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - ScreenSaverManager not ready for g_isScreensaverActive initialization.");
    }

    // --- AudioManager Configuration (Not critical to halt the system) ---
    if (_audioManager && _settingsManager) { 
        _audioConfig.initialVolume_0_100 = (uint8_t)_settingsManager->getSoundVolume(AUDIO_DEFAULT_VOLUME_PERCENT);
        _audioConfig.initialEnabledState = _settingsManager->isSoundEnabled(true);
        // AudioManager init returns bool, indicating true hardware init failure.
        if (!_audioManager->init(_audioConfig)) {
            DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - AudioManager failed to initialize.");
            if (_messageBoard) { 
                std::string localizedMsg = _languageManager->getString("INIT_AUDIO_MGR_FAILED", "AudioMgr Init Failed!");
                _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
            }
        }
        g_playClickSound = [this]() { _audioManager->playClickSound(); };
        DEBUG_INFO_PRINTLN("SystemInitializer: Global g_playClickSound callback linked.");
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - AudioManager or SettingsManager pointer is nullptr. Skipping AudioManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_AUDIO_MGR_FAILED", "AudioMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }

    delay(250);

    // --- RFIDManager Configuration (Not critical to halt the system) ---
    if (_rfidManager && _settingsManager) {
        RFIDManagerConfig rfidConfig = {
            .ssPin = RFID_SS_PIN, .sckPin = RFID_SCK_PIN,
            .mosiPin = RFID_MOSI_PIN, .misoPin = RFID_MISO_PIN
        };
        // RFIDManager init returns bool, indicating true hardware init failure.
        if (!_rfidManager->init(rfidConfig)) { 
             DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - RFIDManager hardware initialization failed. RFID functionality may be limited.");
             if (_messageBoard) { 
                std::string localizedMsg = _languageManager->getString("INIT_RFID_MGR_FAILED", "RFIDMgr Init Failed!");
                _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
             }
        }
        // Apply enabled state based on settings, not init status.
        _rfidManager->setEnabled(_settingsManager->isRfidEnabled(true)); 
        _rfidManager->setOnCardScannedCallback(handleCardScanned);
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - RFIDManager or SettingsManager pointer is nullptr. Skipping RFIDManager initialization.");
        if (_messageBoard) { 
            std::string localizedMsg = _languageManager->getString("INIT_RFID_MGR_FAILED", "RFIDMgr Init Failed!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    }

    DEBUG_INFO_PRINTLN("SystemInitializer: All Managers Initialized.");
    return true; // All critical managers initialized successfully.
}

/**
 * @brief Dynamically allocates and sets up status bar elements and links managers.
 * Includes robust error handling for memory allocation failures.
 * @return True if all *essential* status bar UI elements were successfully allocated, false otherwise.
 */
bool SystemInitializer::_setupUIElements() {
    DEBUG_INFO_PRINTLN("SystemInitializer: Setting up UI Elements...");

    // Critical: Check `_statusbar` and `_languageManager`. These are fundamental.
    if (!_statusbar) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - StatusbarUI pointer is nullptr! Cannot setup statusbar elements. Critical.");
        return false;
    }
    // _languageManager must be valid to localize messages here. Checked at _initManagers.
    if (!_languageManager) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - LanguageManager pointer is nullptr! Cannot localize UI element errors. Critical.");
        return false;
    }
    _statusbar->init(); // Initialize the StatusbarUI itself.

    // --- Allocate Statusbar Elements using `std::make_unique` and add to Statusbar ---
    // Essential elements (`_messageBoardUnique`, `_timeElementUnique`) must be allocated successfully.
    // Others are optional (warnings if fail, but system can still run).

    // `TimeElement` (Essential for time display).
    _timeElementUnique = std::make_unique<TimeElement>(_lcd, 45, _timeManager);
    if (!_timeElementUnique) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: ERROR - Failed to allocate TimeElement! Out of memory. Critical.");
        // Use `_messageBoard` to display localized error directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) { 
            std::string localizedMsg = _languageManager->getString("INIT_TIME_UI_ERROR", "Time UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
        return false; // Critical failure.
    }
    _timeElement = _timeElementUnique.get(); // Assign raw pointer.
    _timeElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    _statusbar->addElement(_timeElement, StatusbarUI::Alignment::LEFT); // Add element.

    // `MessageBoardElement` (Critical, as other errors might rely on it for feedback).
    _messageBoardUnique = std::make_unique<MessageBoardElement>(_lcd, &helvR10, UI_COLOR_TEXT_DEFAULT, PANEL_BACKGROUND_COLOR);
    if (!_messageBoardUnique) {
        DEBUG_ERROR_PRINTLN("SystemInitializer: ERROR - Failed to allocate MessageBoardElement! Out of memory. Critical.");
        return false; // Critical failure.
    }
    _messageBoard = _messageBoardUnique.get(); // Assign raw pointer for convenience.
    _messageBoard->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    // Add element to `_statusbar`. Ownership remains with `_messageBoardUnique`.
    _statusbar->addElement(_messageBoard, StatusbarUI::Alignment::LEFT); 
    
    // `RFID IconElement` (Optional).
    char initialRfidIconChar = (_settingsManager && _settingsManager->isRfidEnabled(true)) ? RFID_DEFAULT_ICON_CHAR : ' ';
    _rfidElementUnique = std::make_unique<IconElement>(_lcd, 25, &iconic_all2x, std::string(1, initialRfidIconChar), UI_COLOR_TEXT_DEFAULT, "RFID");
    if (!_rfidElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate RFID IconElement! Out of memory. Skipping RFID icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_RFID_UI_ERROR", "RFID UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _rfidElement = _rfidElementUnique.get(); // Assign raw pointer.
        _rfidElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }

    // `Speaker IconElement` (Optional).
    _speakerElementUnique = std::make_unique<IconElement>(_lcd, 25, &iconic_all2x, "\xC4\x97", UI_COLOR_TEXT_DEFAULT, "Speaker");
    if (!_speakerElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate Speaker IconElement! Out of memory. Skipping Speaker icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_SPEAKER_UI_ERROR", "Speaker UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _speakerElement = _speakerElementUnique.get(); // Assign raw pointer.
        _speakerElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }

    // `Bluetooth IconElement` (Optional).
    char initialBtIconChar = (_settingsManager && _settingsManager->getBluetoothEnabledLastState(false)) ? (char)0x5E : ' ';
    _btElementUnique = std::make_unique<IconElement>(_lcd, 25, &iconic_all2x, std::string(1, initialBtIconChar), UI_COLOR_TEXT_DEFAULT, "Bluetooth");
    if (!_btElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate Bluetooth IconElement! Out of memory. Skipping Bluetooth icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_BT_UI_ERROR", "BT UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _btElement = _btElementUnique.get(); // Assign raw pointer.
        _btElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }

    // `WiFi IconElement` (Optional).
    char initialWifiIconChar = (_settingsManager && _settingsManager->getWifiEnabledLastState(false)) ? 'a' : ' ';
    _wifiElementUnique = std::make_unique<IconElement>(_lcd, 25, &battery, std::string(1, initialWifiIconChar), UI_COLOR_TEXT_DEFAULT, "WiFi");
    if (!_wifiElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate WiFi IconElement! Out of memory. Skipping WiFi icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_WIFI_UI_ERROR", "WiFi UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _wifiElement = _wifiElementUnique.get(); // Assign raw pointer.
        _wifiElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }

    // `SD Card IconElement` (Optional).
    char initialSdIconChar = (_sdManager && _sdManager->isCardPresent()) ? SD_CARD_DEFAULT_ICON_CHAR : ' ';
    _sdElementUnique = std::make_unique<IconElement>(_lcd, 25, &iconic_all2x, std::string(1, initialSdIconChar), UI_COLOR_TEXT_DEFAULT, "SD Card");
    if (!_sdElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate SD Card IconElement! Out of memory. Skipping SD Card icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_SD_UI_ERROR_UI", "SD UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _sdElement = _sdElementUnique.get(); // Assign raw pointer.
        _sdElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }

    // `Battery IconElement` (Optional, but essential for battery-powered devices).
    _batteryElementUnique = std::make_unique<IconElement>(_lcd, 25, &battery, std::string(1, BATT_ICON_LEVEL_0), UI_COLOR_TEXT_DEFAULT, "Battery");
    if (!_batteryElementUnique) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - Failed to allocate Battery IconElement! Out of memory. Skipping Battery icon setup.");
        // Use _messageBoard directly as g_displayLocalizedMessage is not yet defined.
        if (_messageBoard && _languageManager) {
            std::string localizedMsg = _languageManager->getString("INIT_BATTERY_UI_ERROR", "Battery UI Init Error!");
            _messageBoard->pushMessage(localizedMsg, 5000, UI_COLOR_WARNING);
        }
    } else {
        _batteryElement = _batteryElementUnique.get(); // Assign raw pointer.
        _batteryElement->setStatusBarHeightRef(STATUSBAR_HEIGHT);
    }


    // --- Add Right-Aligned Elements to Statusbar (Visual Order: [SD Card] [RFID] [Speaker] [Bluetooth] [WiFi] [Battery]) ---
    // As `StatusbarUI::drawFixedElements()` iterates `_rightElements` in reverse (from rbegin() to rend()),
    // the visual order (left to right) will be the inverse of the addition order.
    // Therefore, to achieve the visual order: [SD Card] [RFID] [Speaker] [Bluetooth] [WiFi] [Battery],
    // elements must be added in this order (first element is rightmost visually, last element is leftmost visually).
    
    // Visually Rightmost
    if (_batteryElement) _statusbar->addElement(_batteryElement, StatusbarUI::Alignment::RIGHT);
    if (_wifiElement) _statusbar->addElement(_wifiElement, StatusbarUI::Alignment::RIGHT);
    if (_btElement) _statusbar->addElement(_btElement, StatusbarUI::Alignment::RIGHT);
    if (_speakerElement) _statusbar->addElement(_speakerElement, StatusbarUI::Alignment::RIGHT);
    if (_rfidElement) _statusbar->addElement(_rfidElement, StatusbarUI::Alignment::RIGHT);
    if (_sdElement) _statusbar->addElement(_sdElement, StatusbarUI::Alignment::RIGHT);


    // --- Link System Managers to their respective Statusbar Elements for updates ---
    // These links use the raw pointers, assuming managers only hold views, not ownership.
    // Note: No DEMO_MODE-specific message passing is done here to UI elements.
    if (_powerManager && _batteryElement) _powerManager->setBatteryIconElement(_batteryElement);
    else if (_powerManager) DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _batteryElement is nullptr, cannot set PowerManager icon.");

    if (_wifiManager && _wifiElement) _wifiManager->setWifiIconElement(_wifiElement);
    else if (_wifiManager) DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _wifiElement is nullptr, cannot set WifiManager icon.");

    if (_btManager && _btElement) _btManager->setBluetoothIconElement(_btElement);
    else if (_btManager) DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _btElement is nullptr, cannot set BLEManager icon.");

    if (_rfidManager && _rfidElement) _rfidManager->setRfidIconElement(_rfidElement);
    else if (_rfidManager) DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _rfidElement is nullptr, cannot set RFIDManager icon.");

    if (_sdManager && _sdElement) _sdManager->setSdIconElement(_sdElement);
    else if (_sdManager) DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _sdElement is nullptr, cannot set SDManager icon.");
    
    if (_audioManager && _speakerElement) {
        _audioManager->setSpeakerIconElement(_speakerElement);
    } else if (_audioManager) {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _speakerElement is nullptr, cannot set AudioManager icon!");
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _audioManager is nullptr, cannot set AudioManager icon!");
    }

    // --- Link ScreenManager to Statusbar (for touch processing coordination) ---
    // This is critical for overall UI interaction.
    if (_screenManager && _statusbar) _screenManager->setStatusbar(_statusbar);
    else {
        DEBUG_ERROR_PRINTLN("SystemInitializer: FATAL ERROR - _screenManager or _statusbar is nullptr, cannot link! Critical.");
        return false; // Critical failure.
    }

    DEBUG_INFO_PRINTLN("SystemInitializer: UI Elements Set up.");
    return true; // All essential UI elements allocated and setup.
}

/**
 * @brief Defines and populates UI layers (screens) for the application.
 * Initializes UI controller classes, which in turn define their specific layers.
 * @return True if UI layers were successfully setup, false otherwise.
 */
bool SystemInitializer::_setupUILayers() {
    DEBUG_INFO_PRINTLN("SystemInitializer: Setting up UI Layers...");

    // Initialize UI Controller Classes (they define their own layers during init).
    // Non-critical if one UI screen fails to init, but log warnings.
    if (_btUI) _btUI->init(); else DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _btUI is nullptr. Skipping BLEUI init.");
    if (_wifiUI) _wifiUI->init(); else DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _wifiUI is nullptr. Skipping WifiUI init.");
    if (_mainUI) _mainUI->init(); else DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _mainUI is nullptr. Skipping MainUI init.");
    if (_settingsUI) _settingsUI->init(); else DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _settingsUI is nullptr. Skipping SettingsUI init.");

    // Define Screensaver Layer.
    if (_screenManager && _screenSaverClock && _lcd) {
        // `_screenSaverClock` is a raw pointer to a globally instantiated object in .ino,
        // so its ownership is external to `SystemInitializer`.
        UILayer screenSaverLayer(_lcd, false, true, UI_COLOR_BACKGROUND_DARK); 
        screenSaverLayer.addElement(_screenSaverClock); 
        _screenManager->defineLayer("screensaver", screenSaverLayer); 
    } else {
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _screenManager, _screenSaverClock or LCD is nullptr. Skipping Screensaver Layer setup.");
        // A dedicated error for _setupUILayers might be desirable here if this part is critical.
        // For now, we allow non-critical UI layers to fail gracefully.
    }
    
    DEBUG_INFO_PRINTLN("SystemInitializer: UI Layers Set up.");
    return true; // All UI layers were successfully setup (or critical ones managed failures gracefully).
}

/**
 * @brief Adds buttons to the pull-down status bar panel.
 * These buttons trigger various UI screens or system functions.
 */
void SystemInitializer::_setupStatusbarPanelButtons() {
    DEBUG_INFO_PRINTLN("SystemInitializer: Setting up Statusbar Panel Buttons...");

    if (!_statusbar || !_languageManager) { 
        DEBUG_WARN_PRINTLN("SystemInitializer: WARNING - _statusbar or _languageManager is nullptr. Skipping Statusbar Panel Buttons setup.");
        return;
    }

    const lgfx::IFont* panelFont = &iconic_all2x; // Using an iconic font for panel buttons.

    _statusbar->addButtonToPanel(101, 0, PANEL_BUTTON_POS_Y, 0, PANEL_BUTTON_HEIGHT, _languageManager->getString("PANEL_BUTTON_POWER_OFF", "\u00EB").c_str(), panelFont, initiateShutdown);
    _statusbar->addButtonToPanel(102, 0, PANEL_BUTTON_POS_Y, 0, PANEL_BUTTON_HEIGHT, _languageManager->getString("PANEL_BUTTON_WIFI", "\u00F7").c_str(), panelFont, openWiFiSettingsPanel);
    _statusbar->addButtonToPanel(103, 0, PANEL_BUTTON_POS_Y, 0, PANEL_BUTTON_HEIGHT, _languageManager->getString("PANEL_BUTTON_BLUETOOTH", "\u005E").c_str(), panelFont, openBluetoothSettingsPanel);
    _statusbar->addButtonToPanel(104, 0, PANEL_BUTTON_POS_Y, 0, PANEL_BUTTON_HEIGHT, _languageManager->getString("PANEL_BUTTON_SETTINGS", "\u0081").c_str(), panelFont, openSettingsScreen);
    _statusbar->finalizePanelSetup(); 

    DEBUG_INFO_PRINTLN("SystemInitializer: Statusbar Panel Buttons Set up.");
}