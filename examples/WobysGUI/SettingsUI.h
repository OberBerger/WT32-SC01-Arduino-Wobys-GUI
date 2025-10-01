/**
 * @file SettingsUI.h
 * @brief Defines the SettingsUI class, a screen for managing various device settings.
 *
 * This file contains the declaration of the SettingsUI class, which provides a user interface
 * for configuring different aspects of the device, such as language, display brightness,
 * screensaver, sound, and RFID functionality. It interacts with various manager classes
 * to read and apply settings.
 *
 * @version 1.0.7
 * @date 2025-08-15
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
#ifndef SETTINGSUI_H
#define SETTINGSUI_H

// Core configuration
#include "Config.h"
// Manager includes
#include "ScreenManager.h"
#include "SettingsManager.h"
#include "LanguageManager.h"
#include "PowerManager.h"
#include "RFIDManager.h"
#include "ScreenSaverManager.h"
#include "StatusbarUI.h" 
#include "AudioManager.h"

// UI elements import
#include "ButtonUI.h"
#include "TextUI.h"
#include "ClickableListUI.h"
#include "ToggleButtonUI.h"
#include "ToggleSwitchUI.h" 
#include "SeekbarUI.h"
#include "IconElement.h"
#include "GridLayoutUI.h"

/**
 * @brief Manages the settings user interface, allowing users to configure device settings.
 *
 * The SettingsUI class is responsible for presenting various configurable options to the user,
 * such as language, display brightness, screensaver behavior, sound settings, and RFID.
 * It interacts with multiple manager classes to retrieve and persist these settings.
 * The UI is composed of several panels, each managing a specific category of settings.
 */
class SettingsUI {
public:
    /**
     * @brief Constructor for the SettingsUI class.
     * Initializes the SettingsUI with pointers to essential manager and UI components.
     *
     * @param lcd Pointer to the LGFX display object.
     * @param screenManager Pointer to the ScreenManager for layer management.
     * @param settingsManager Pointer to the SettingsManager for persistent settings.
     * @param languageManager Pointer to the LanguageManager for UI internationalization.
     * @param powerManager Pointer to the PowerManager for battery status and power control.
     * @param rfidManager Pointer to the RFIDManager for RFID functionality control.
     * @param screensaverManager Pointer to the ScreenSaverManager for screensaver control.
     * @param statusbar Pointer to the StatusbarUI for status bar interaction.
     * @param audioManager Pointer to the AudioManager for sound control.
     */
    SettingsUI(LGFX* lcd, ScreenManager* screenManager, SettingsManager* settingsManager, LanguageManager* languageManager, PowerManager* powerManager, RFIDManager* rfidManager, ScreenSaverManager* screensaverManager, StatusbarUI* statusbar, AudioManager* audioManager);

    /**
     * @brief Initializes the settings UI components, sets up their layout, and registers callbacks.
     * This method should be called once after the constructor.
     */
    void init();

    /**
     * @brief Opens the settings panel.
     * This method handles the transition to the settings screen, ensuring proper status bar panel closure
     * if it's open, before pushing the settings layer to the screen manager.
     */
    void openPanel();

private:
    // --- Manager Pointers ---
    LGFX* _lcd;                      ///< Pointer to the LGFX display object.
    ScreenManager* _screenManager;   ///< Pointer to the ScreenManager for layer management.
    SettingsManager* _settingsManager; ///< Pointer to the SettingsManager for persistent settings.
    LanguageManager* _languageManager; ///< Pointer to the LanguageManager for UI internationalization.
    PowerManager* _powerManager;     ///< Pointer to the PowerManager for battery status and power control.
    RFIDManager* _rfidManager;       ///< Pointer to the RFIDManager for RFID functionality control.
    ScreenSaverManager* _screensaverManager; ///< Pointer to the ScreenSaverManager for screensaver control.
    StatusbarUI* _statusbar;        ///< Pointer to the StatusbarUI for status bar interaction.
    AudioManager* _audioManager;     ///< Pointer to the AudioManager for sound control.

    // --- Main UI Elements (Header) ---
    ButtonUI _backButton;     ///< Button to navigate back from the settings screen.
    TextUI _titleText;        ///< Text label for the settings screen title.
    GridLayoutUI _gridLayout; ///< Layout manager for arranging panels and elements.

    // --- 1. Language Panel ---
    TextUI _langPanelContainer;    ///< Container for the language selection panel.
    ClickableListUI _languageList; ///< List to display available languages.

    // --- 2. Display Panel ---
    TextUI _displayPanelContainer;  ///< Container for the display settings panel.
    SeekbarUI _brightnessSeekbar;   ///< Seekbar for adjusting display brightness.

    // --- 3. Screensaver panel ---
    TextUI _screensaverPanelContainer;      ///< Container for the screensaver settings panel.
    ToggleSwitchUI _screensaverEnableToggle; ///< Toggle switch to enable/disable the screensaver.
    SeekbarUI _screensaverTimeoutSeekbar;   ///< Seekbar for adjusting screensaver timeout.
    SeekbarUI _screensaverBrightnessSeekbar;///< Seekbar for adjusting screensaver dimmed brightness.

    // --- 4. Audio panel ---
    TextUI _soundPanelContainer; ///< Container for the sound settings panel.
    ToggleSwitchUI _soundEnableToggle; ///< Toggle switch to enable/disable sound.
    SeekbarUI _volumeSeekbar;     ///< Seekbar for adjusting overall volume.

    // --- 5. RFID panel ---
    TextUI _rfidPanelContainer; ///< Container for the RFID settings panel.
    ToggleSwitchUI _rfidToggle; ///< Toggle switch to enable/disable RFID functionality.

    // --- 6. Power panel ---
    TextUI _batteryPanelContainer;  ///< Container for the battery information panel.
    TextUI _batteryVoltageLabel;    ///< Text label to display current battery voltage.

    // --- Private Helper Methods ---
    /**
     * @brief Retranslates all UI text elements based on the current language setting.
     * This method is called during initialization and whenever the language changes.
     */
    void _retranslateUI();
    
    /**
     * @brief Proceeds to open the settings panel layer after any prerequisite actions (e.g., status bar panel closure).
     * This method is typically called as a callback after the status bar panel is fully closed.
     */
    void proceedToOpenPanel();

    /**
     * @brief Handles the back button press event.
     * Pops the settings layer from the screen manager and unregisters the loop callback.
     */
    void _onBackButtonPressed();

    /**
     * @brief Callback function when a language is selected from the list.
     * Sets the new language via the LanguageManager if it's different from the current one.
     * @param index The index of the selected item.
     * @param data The ListItem data associated with the selected item.
     * @param touchX The X coordinate of the touch event.
     */
    void _onLanguageSelected(int index, const ListItem& data, int16_t touchX);

    /**
     * @brief Callback function when the RFID toggle switch state changes.
     * Updates the RFID enabled setting and notifies the RFIDManager.
     * @param newState The new state of the toggle (true for enabled, false for disabled).
     */
    void _onRfidToggleChanged(bool newState);

    /**
     * @brief Callback function when the screensaver enable toggle switch state changes.
     * Updates the screensaver enabled setting, adjusts interactivity of related seekbars,
     * and notifies the ScreenSaverManager.
     * @param newState The new state of the toggle (true for enabled, false for disabled).
     */
    void _onScreensaverToggleChanged(bool newState);

    /**
     * @brief Callback function when the screensaver timeout seekbar value changes.
     * Updates the screensaver timeout in the ScreenSaverManager immediately and
     * saves the setting if the change is final.
     * @param value The new screensaver timeout value in seconds.
     * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
     */
    void _onScreensaverTimeoutChanged(float value, bool isFinalChange);

    /**
     * @brief Callback function when the screensaver brightness seekbar value changes.
     * Updates the screensaver dimmed brightness in the ScreenSaverManager immediately and
     * saves the setting if the change is final.
     * @param value The new screensaver brightness level.
     * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
     */
    void _onScreensaverBrightnessChanged(float value, bool isFinalChange);

    /**
     * @brief Callback function when the main display brightness seekbar value changes.
     * Applies the new display brightness immediately to the LCD, adjusts the range of the
     * screensaver brightness seekbar, and saves the setting if the change is final.
     * @param value The new display brightness level.
     * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
     */
    void _onBrightnessChanged(float value, bool isFinalChange);

    /**
     * @brief Callback function when the sound enable toggle switch state changes.
     * Updates the sound enabled setting, adjusts interactivity of the volume seekbar,
     * and notifies the AudioManager.
     * @param newState The new state of the toggle (true for enabled, false for disabled).
     */
    void _onSoundToggleChanged(bool newState);

    /**
     * @brief Callback function when the volume seekbar value changes.
     * Sets the new volume in the AudioManager immediately and saves the setting
     * if the change is final.
     * @param value The new volume level.
     * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
     */
    void _onVolumeChanged(float value, bool isFinalChange);

    /**
     * @brief Loop callback for the settings UI layer.
     * This method is called periodically when the settings layer is active.
     * It's used for dynamic updates, such as battery voltage display.
     */
    void _settingsLoop();

    /**
     * @brief Loads all relevant settings from the SettingsManager and applies them to the UI elements.
     * Also updates the states of the various managers (ScreenSaverManager, AudioManager, RFIDManager).
     */
    void _loadAndApplySettings();

    /**
     * @brief Populates the language selection list with available languages from the LanguageManager.
     * Also sets the currently selected language in the list.
     */
    void _populateLanguageList();

    // --- Private Members for Periodic Updates ---
    unsigned long _lastUpdateMillis = 0; ///< Stores the last time an update occurred in _settingsLoop.
    const unsigned long UPDATE_INTERVAL_MS = 1000; ///< Interval for periodic updates in milliseconds.
};

#endif // SETTINGSUI_H