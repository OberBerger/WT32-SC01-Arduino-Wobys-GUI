/**
 * @file SettingsUI.cpp
 * @brief Implements the SettingsUI class, providing functionality for device settings management.
 *
 * This file contains the implementation details for the SettingsUI class, including
 * its initialization, layout setup, interaction handling, and integration with
 * various system managers for persistent settings and real-time control.
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

#include "SettingsUI.h"
#include <Arduino.h>   // For Arduino specific functions like millis()
#include <algorithm>   // For std::min, std::max
#include <cstdio>      // For snprintf
#include <string>      // For std::string
#include "StatusbarUI.h" // For interaction with the status bar
#include "AudioManager.h" // For audio settings

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
SettingsUI::SettingsUI(LGFX* lcd, ScreenManager* screenManager, SettingsManager* settingsManager, LanguageManager* languageManager, PowerManager* powerManager, RFIDManager* rfidManager, ScreenSaverManager* screensaverManager, StatusbarUI* statusbar, AudioManager* audioManager)
    : _lcd(lcd),
      _screenManager(screenManager),
      _settingsManager(settingsManager),
      _languageManager(languageManager),
      _powerManager(powerManager),
      _rfidManager(rfidManager),
      _screensaverManager(screensaverManager),
      _statusbar(statusbar),
      _audioManager(audioManager),
      _backButton(lcd, "", 0, 0, 0, 0, &iconic_all2x), // Initial dummy values, set in init()
      _titleText(lcd, "", 0, 0),                       // Initial dummy values, set in init()
      _gridLayout(lcd, 12, 1, 5, 5),                   // Initial dummy values for spacing, set in init()
      _langPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _languageList(lcd, 0, 0, 0, 0, 1),               // Initial dummy values, set in init()
      _displayPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _brightnessSeekbar(lcd, 0, 0, 0, 0),             // Initial dummy values, set in init()
      _screensaverPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _screensaverEnableToggle(lcd, 0, 0, 1, 1, "", false), // Initial dummy values, set in init()
      _screensaverTimeoutSeekbar(lcd, 0, 0, 0, 0),     // Initial dummy values, set in init()
      _screensaverBrightnessSeekbar(lcd, 0, 0, 0, 0),  // Initial dummy values, set in init()
      _soundPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _soundEnableToggle(lcd, 0, 0, 1, 1, "", false), // Initial dummy values, set in init()
      _volumeSeekbar(lcd, 0, 0, 0, 0),                 // Initial dummy values, set in init()
      _rfidPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _rfidToggle(lcd, 0, 0, 1, 1, "", false),         // Initial dummy values, set in init()
      _batteryPanelContainer(lcd, "", 0, 0, &helvB14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, 0, 0, PANEL_BACKGROUND_COLOR, 5), // Initial dummy size/pos, set in init()
      _batteryVoltageLabel(lcd, "", 0, 0, &helvR14, UI_COLOR_TEXT_DEFAULT, TL_DATUM, TEXTUI_AUTO_SIZE, TEXTUI_AUTO_SIZE, TEXTUI_TRANSPARENT, 0) // Initial dummy size/pos, set in init()
{
    // Constructor body is intentionally empty as member initialization happens in the initializer list.
}

/**
 * @brief Initializes the settings UI components, sets up their layout, and registers callbacks.
 * This method should be called once after the constructor.
 */
void SettingsUI::init() {
    DEBUG_INFO_PRINTLN("SettingsUI: Initializing settings UI components.");

    // Null pointer checks for essential managers
    if (!_lcd || !_screenManager || !_settingsManager || !_languageManager || !_powerManager || !_rfidManager || !_screensaverManager || !_statusbar || !_audioManager) {
        DEBUG_ERROR_PRINTLN("SettingsUI: One or more essential manager pointers are null. Initialization aborted.");
        return;
    }

    _screenManager->defineLayer("settings_layer",
                                UILayer(_lcd,
                                        false,
                                        true,
                                        PANEL_BACKGROUND_COLOR, // Using configurable background color
                                        OrientationPreference::CONTENT_LANDSCAPE));
    UILayer* layer = _screenManager->getLayer("settings_layer");
    if (!layer) {
        DEBUG_ERROR_PRINTLN("SettingsUI: Failed to create or retrieve 'settings_layer'. Initialization aborted.");
        return;
    }

    uint16_t layerWidth = TFT_HEIGHT; // Hardcoded display dimensions, consider using _lcd->width()
    uint16_t layerHeight = TFT_WIDTH - STATUSBAR_HEIGHT; // STATUSBAR_HEIGHT from Config.h

    uint8_t frameMargin = TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS; // Using configurable border thickness
    uint8_t topBarHeight = UI_DEFAULT_TOPBAR_HEIGHT_PIXELS; // Using configurable top bar height
    uint16_t panelTitleHeight = UI_DEFAULT_TOPBAR_HEIGHT_PIXELS; // Using configurable top bar height (as panel title height)
    uint16_t itemHeight = UI_DEFAULT_BUTTON_HEIGHT_PIXELS; // Using configurable button height (as item height)
    uint16_t innerPadding = UI_DEFAULT_PANEL_INNER_PADDING_PIXELS; // Using configurable inner padding

    // --- GridLayoutUI setup ---
    _gridLayout.setGridParameters(12, 15, GRID_DEFAULT_H_SPACING_PIXELS, GRID_DEFAULT_V_SPACING_PIXELS);
    _gridLayout.setContainerSize(layerWidth, layerHeight);
    _gridLayout.setGridLineColor(TFT_DARKGREY); // Color is OS configurable
    _gridLayout.setGridLineThickness(GRID_DEFAULT_LINE_THICKNESS_PIXELS);
    _gridLayout.setVisible(true);


    // Main header: Back button and Title
    GridCellInfo headerBackBtn = _gridLayout.getPhysicalBlockRect(0, 0, 1, 0); // COL 0-1, ROW 0
    _backButton.setPosition(headerBackBtn.x + innerPadding, headerBackBtn.y + innerPadding);
    _backButton.setSize(headerBackBtn.w - (2 * innerPadding), topBarHeight);
    _backButton.setFont(&iconic_all2x);
    _backButton.setOnReleaseCallback([this]() { this->_onBackButtonPressed(); });
    layer->addElement(&_backButton);

    GridCellInfo headerTitle = _gridLayout.getPhysicalBlockRect(2, 0, 9, 1); // COL 2-9, ROW 0-1
    _titleText.setPosition(headerTitle.x, headerTitle.y);
    _titleText.setSize(headerTitle.w, headerTitle.h);
    _titleText.setFont(&helvB18);
    _titleText.setTextColor(UI_COLOR_TEXT_DEFAULT);
    _titleText.setBackgroundColor(PANEL_BACKGROUND_COLOR);
    _titleText.setTextDatum(MC_DATUM);
    layer->addElement(&_titleText);

    // --- Panels placement ---

    // --- 1. Language panel (left column) ---
    GridCellInfo langPanelGrid = _gridLayout.getPhysicalBlockRect(0, 2, 5, 6); // COL 0-5, ROW 2-6
    _langPanelContainer.setPosition(langPanelGrid.x, langPanelGrid.y);
    _langPanelContainer.setSize(langPanelGrid.w - frameMargin, langPanelGrid.h - frameMargin);
    _langPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _langPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _langPanelContainer.setTextDatum(TL_DATUM);
    _langPanelContainer.setPadding(innerPadding);
    layer->addElement(&_langPanelContainer);

    _languageList.setPosition(_langPanelContainer.getX() + innerPadding, _langPanelContainer.getY() + panelTitleHeight + innerPadding);
    _languageList.setSize(_langPanelContainer.getWidth() - (2 * innerPadding), (2 * itemHeight));
    _languageList.setItemHeight(itemHeight);
    _languageList.setNumColumns(1);
    _languageList.setColumnDefaultAlignment(0,MC_DATUM);
    _languageList.setDrawBorder(false);
    _languageList.setDrawDividers(true);
    _languageList.setColumnDefaultFont(0, &helvB12);
    _languageList.setOnItemSelectedCallback([this](int i, const ListItem& d, int16_t t) {
        this->_onLanguageSelected(i, d, t);
    });
    layer->addElement(&_languageList);

    // --- 2. Display panel (right column) ---
    GridCellInfo displayPanelGrid = _gridLayout.getPhysicalBlockRect(6, 2, 11, 5); // COL 6-11, ROW 2-5
    _displayPanelContainer.setPosition(displayPanelGrid.x + frameMargin, displayPanelGrid.y);
    _displayPanelContainer.setSize(displayPanelGrid.w - frameMargin, displayPanelGrid.h - frameMargin);
    _displayPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _displayPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _displayPanelContainer.setTextDatum(TL_DATUM);
    _displayPanelContainer.setPadding(innerPadding);
    layer->addElement(&_displayPanelContainer);

    // Adjust grid cell for seekbar to be within the panel's content area
    displayPanelGrid = _gridLayout.getBlockRect(6, 3, 11, 5); // COL 6-11, ROW 3-5 (Relative to the whole grid)
    _brightnessSeekbar.setOrientation(SeekbarUI::Orientation::Horizontal);
    _brightnessSeekbar.setPosition(displayPanelGrid.x + frameMargin, displayPanelGrid.y);
    _brightnessSeekbar.setSize(displayPanelGrid.w - frameMargin, displayPanelGrid.h);
    layer->addElement(&_brightnessSeekbar);

    // --- 3. Screensaver panel (left column) ---
    GridCellInfo screensaverPanelGrid = _gridLayout.getPhysicalBlockRect(0, 7, 5, 12); // COL 0-5, ROW 7-12
    _screensaverPanelContainer.setPosition(screensaverPanelGrid.x, screensaverPanelGrid.y + frameMargin);
    _screensaverPanelContainer.setSize(screensaverPanelGrid.w - frameMargin, screensaverPanelGrid.h - (2 * frameMargin));
    _screensaverPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _screensaverPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _screensaverPanelContainer.setTextDatum(TL_DATUM);
    _screensaverPanelContainer.setPadding(innerPadding);
    layer->addElement(&_screensaverPanelContainer);

    screensaverPanelGrid = _gridLayout.getBlockRect(0, 8, 5, 9); // COL 0-5, ROW 8-9
    _screensaverEnableToggle.setPosition(screensaverPanelGrid.x + frameMargin, screensaverPanelGrid.y);
    _screensaverEnableToggle.setSize(screensaverPanelGrid.w - frameMargin, screensaverPanelGrid.h);
    _screensaverEnableToggle.setAlignment(ML_DATUM);
    _screensaverEnableToggle.setVisualState(UIVisualState::ACTIVE);
    _screensaverEnableToggle.setOnStateChangedCallback([this](bool newState) { this->_onScreensaverToggleChanged(newState); });
    layer->addElement(&_screensaverEnableToggle);

    screensaverPanelGrid = _gridLayout.getBlockRect(0, 10, 5, 10); // COL 0-5, ROW 10
    _screensaverTimeoutSeekbar.setOrientation(SeekbarUI::Orientation::Horizontal);
    _screensaverTimeoutSeekbar.setPosition(screensaverPanelGrid.x + frameMargin, screensaverPanelGrid.y);
    _screensaverTimeoutSeekbar.setSize(screensaverPanelGrid.w - frameMargin, screensaverPanelGrid.h);
    layer->addElement(&_screensaverTimeoutSeekbar);

    screensaverPanelGrid = _gridLayout.getBlockRect(0, 11, 5, 12); // COL 0-5, ROW 11-12
    _screensaverBrightnessSeekbar.setOrientation(SeekbarUI::Orientation::Horizontal);
    _screensaverBrightnessSeekbar.setPosition(screensaverPanelGrid.x + frameMargin, screensaverPanelGrid.y);
    _screensaverBrightnessSeekbar.setSize(screensaverPanelGrid.w - frameMargin, screensaverPanelGrid.h);
    layer->addElement(&_screensaverBrightnessSeekbar);

    // --- 4. Sound panel (right column) ---
    GridCellInfo soundPanelGrid = _gridLayout.getPhysicalBlockRect(6, 6, 11, 10); // COL 6-11, ROW 6-10
    _soundPanelContainer.setPosition(soundPanelGrid.x + frameMargin, soundPanelGrid.y + frameMargin);
    _soundPanelContainer.setSize(soundPanelGrid.w - frameMargin, soundPanelGrid.h - (2 * frameMargin));
    _soundPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _soundPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _soundPanelContainer.setTextDatum(TL_DATUM);
    _soundPanelContainer.setPadding(innerPadding);
    layer->addElement(&_soundPanelContainer);

    soundPanelGrid = _gridLayout.getBlockRect(6, 7, 11, 9); // COL 6-11, ROW 7-9
    _soundEnableToggle.setPosition(soundPanelGrid.x + frameMargin, soundPanelGrid.y);
    _soundEnableToggle.setSize(soundPanelGrid.w - frameMargin, soundPanelGrid.h);
    _soundEnableToggle.setAlignment(ML_DATUM);
    _soundEnableToggle.setVisualState(UIVisualState::ACTIVE);
    _soundEnableToggle.setOnStateChangedCallback([this](bool newState) { this->_onSoundToggleChanged(newState); });
    layer->addElement(&_soundEnableToggle);

    soundPanelGrid = _gridLayout.getBlockRect(6, 9, 11, 10); // COL 6-11, ROW 9-10 (Adjusted for padding)
    _volumeSeekbar.setOrientation(SeekbarUI::Orientation::Horizontal);
    _volumeSeekbar.setPosition(soundPanelGrid.x + frameMargin, soundPanelGrid.y);
    _volumeSeekbar.setSize(soundPanelGrid.w - frameMargin, soundPanelGrid.h);
    layer->addElement(&_volumeSeekbar);

    // --- 5. RFID panel (right column) ---
    GridCellInfo rfidPanelGrid = _gridLayout.getPhysicalBlockRect(6, 11, 11, 14); // COL 6-11, ROW 11-14
    _rfidPanelContainer.setPosition(rfidPanelGrid.x + frameMargin, rfidPanelGrid.y + frameMargin);
    _rfidPanelContainer.setSize(rfidPanelGrid.w - frameMargin, rfidPanelGrid.h - frameMargin);
    _rfidPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _rfidPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _rfidPanelContainer.setTextDatum(TL_DATUM);
    _rfidPanelContainer.setPadding(innerPadding);
    layer->addElement(&_rfidPanelContainer);

    rfidPanelGrid = _gridLayout.getBlockRect(6, 12, 11, 14); // COL 6-11, ROW 12-14 (Adjusted for padding)
    _rfidToggle.setPosition(rfidPanelGrid.x + frameMargin, rfidPanelGrid.y);
    _rfidToggle.setSize(rfidPanelGrid.w - frameMargin, rfidPanelGrid.h);
    _rfidToggle.setAlignment(ML_DATUM);
    _rfidToggle.setVisualState(UIVisualState::ACTIVE);
    _rfidToggle.setOnStateChangedCallback([this](bool newState) { this->_onRfidToggleChanged(newState); });
    layer->addElement(&_rfidToggle);

    // --- 6. Battery panel (right column) ---
    GridCellInfo batteryPanelGrid = _gridLayout.getPhysicalBlockRect(0, 13, 5, 14); // COL 0-5, ROW 13-14
    _batteryPanelContainer.setPosition(batteryPanelGrid.x, batteryPanelGrid.y + frameMargin);
    _batteryPanelContainer.setSize(batteryPanelGrid.w - frameMargin, batteryPanelGrid.h - frameMargin);
    _batteryPanelContainer.setBorder(DIALOG_BOX_BORDER_COLOR, TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS, TextUI_BorderType::SINGLE); // Using configurable border
    _batteryPanelContainer.setBackgroundColor(UI_COLOR_BACKGROUND_DARK);
    _batteryPanelContainer.setTextDatum(TL_DATUM);
    _batteryPanelContainer.setPadding(innerPadding);
    layer->addElement(&_batteryPanelContainer);

    batteryPanelGrid = _gridLayout.getBlockRect(0, 13, 5, 14); // COL 0-5, ROW 13-14 (Adjusted for padding)

    _batteryVoltageLabel.setPosition(batteryPanelGrid.x + frameMargin, batteryPanelGrid.y);
    _batteryVoltageLabel.setSize(batteryPanelGrid.w - frameMargin, batteryPanelGrid.h);
    _batteryVoltageLabel.setTextDatum(MR_DATUM);
    layer->addElement(&_batteryVoltageLabel);

    // The grid itself is typically not added as an element to the layer if it only serves for layout.
    // layer->addElement(&_gridLayout);

    // Initialize elements with callbacks and content
    _screensaverTimeoutSeekbar.setOnValueChangedCallback([this](float value, bool isFinalChange) {
        this->_onScreensaverTimeoutChanged(value, isFinalChange);
    });
    _screensaverBrightnessSeekbar.setOnValueChangedCallback([this](float value, bool isFinalChange) {
        this->_onScreensaverBrightnessChanged(value, isFinalChange);
    });
    _brightnessSeekbar.setOnValueChangedCallback([this](float value, bool isFinalChange) {
        this->_onBrightnessChanged(value, isFinalChange);
    });
    _volumeSeekbar.setOnValueChangedCallback([this](float value, bool isFinalChange) {
        this->_onVolumeChanged(value, isFinalChange);
    });

    // Seekbar ranges and formats
    _screensaverTimeoutSeekbar.setRange(10, 300);
    _screensaverTimeoutSeekbar.setValueFormat(3, 0, &helvR14);
    _screensaverTimeoutSeekbar.setLimits(10, 300);
    _screensaverBrightnessSeekbar.setRange(0, SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE);
    _screensaverBrightnessSeekbar.setValueFormat(3, 0, &helvR14);
    // Corrected min limit from 0 to 1 for screensaver brightness, assuming 0 is off and not a valid dimming level.
    _screensaverBrightnessSeekbar.setLimits(0, SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE); // Reverted to 0 based on user feedback
    _brightnessSeekbar.setRange(20, 255);
    _brightnessSeekbar.setShowValueText(false);
    // _brightnessSeekbar.setValueFormat(3, 0, &helvR14); // This line was commented out in original.
    _brightnessSeekbar.setLimits(0, 255);
    _volumeSeekbar.setRange(1, 100);
    _volumeSeekbar.setValueFormat(3, 0, &helvR14);
    _volumeSeekbar.setLimits(1, 100);

    _screensaverBrightnessSeekbar.setElementName("ScreensaverBrightnessSeekbar");
    _brightnessSeekbar.setElementName("DisplayBrightnessSeekbar");
    _screensaverTimeoutSeekbar.setElementName("ScreensaverTimeoutSeekbar");
    _volumeSeekbar.setElementName("VolumeSeekbar");

    // Register for language change notifications
    if (_languageManager) { // Null pointer check
        _languageManager->registerForUpdate("SettingsUI", [this]() { this->_retranslateUI(); });
    }

    _retranslateUI(); // Initial text and list content population
}

/**
 * @brief Opens the settings panel.
 * This method handles the transition to the settings screen, ensuring proper status bar panel closure
 * if it's open, before pushing the settings layer to the screen manager.
 */
void SettingsUI::openPanel() {
    DEBUG_INFO_PRINTLN("SettingsUI: openPanel() called from global handler.");

    if (!_screenManager || !_statusbar) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: ScreenManager or Statusbar pointer is null. Cannot open panel.");
        return;
    }

    // 1. CHECK IF THE SETTINGS LAYER IS ALREADY OPEN.
    // If "settings_layer" is already at the top of the ScreenManager stack,
    // do not open a new one; just close the status bar panel if it's open.
    if (_screenManager->getTopLayerName() == "settings_layer") {
        DEBUG_INFO_PRINTLN("SettingsUI: Settings layer is already open, not opening a new one.");
        // If the status bar panel is open or opening, close it.
        if (_statusbar->hasPanel() && _statusbar->isPanelOpenOrOpening()) {
            _statusbar->closePanel();
        }
        return; // Exit as the settings layer is already active.
    }

    // 2. IF THE SETTINGS LAYER IS NOT YET OPEN, HANDLE THE STATUS BAR PANEL, THEN OPEN IT.
    if (_statusbar->hasPanel()) {
        if (_statusbar->isPanelOpenOrOpening()) {
            DEBUG_INFO_PRINTLN("SettingsUI: Statusbar panel open/opening, closing and setting callback.");
            _statusbar->setOnPanelFullyClosedCallback([this]() { this->proceedToOpenPanel(); });
            _statusbar->closePanel();
        } else { // The status bar panel is already closed or does not exist
            proceedToOpenPanel();
        }
    } else { // No pull-down status bar panel exists
        proceedToOpenPanel();
    }

    UILayer* settingsLayer = _screenManager->getLayer("settings_layer");
    if (settingsLayer) { // Null pointer check
        settingsLayer->setOnLoopCallback([this]() {
            this->_settingsLoop();
        });
    } else {
        DEBUG_ERROR_PRINTLN("SettingsUI: Failed to get 'settings_layer' for loop callback.");
    }
    _settingsLoop(); // Initial call to update immediately upon opening
}

/**
 * @brief Proceeds to open the settings panel layer after any prerequisite actions (e.g., status bar panel closure).
 * This method is typically called as a callback after the status bar panel is fully closed.
 */
void SettingsUI::proceedToOpenPanel() {
    DEBUG_INFO_PRINTLN("SettingsUI: proceedToOpenPanel() executing (opening Settings panel now).");

    if (!_screenManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("SettingsUI: ScreenManager pointer is null. Cannot proceed to open panel.");
        return;
    }

    _screenManager->pushLayer("settings_layer");
    _loadAndApplySettings(); // Load and apply settings to UI elements
}

/**
 * @brief Loads all relevant settings from the SettingsManager and applies them to the UI elements.
 * Also updates the states of the various managers (ScreenSaverManager, AudioManager, RFIDManager).
 */
void SettingsUI::_loadAndApplySettings() {
    DEBUG_INFO_PRINTLN("SettingsUI: Loading and applying settings to the UI...");

    if (!_settingsManager || !_languageManager || !_screensaverManager || !_audioManager || !_rfidManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: One or more manager pointers are null. Cannot load/apply settings.");
        return;
    }

    // Language
    LanguageManager::Language currentLang = _languageManager->getCurrentLanguage();
    // Assuming HU is index 1, others (EN) is index 0 based on original code logic
    int langIndex = (currentLang == LanguageManager::Language::HU) ? 1 : 0;
    _languageList.setSelectedItemIndex(langIndex, true);

    // Display brightness
    int loadedDisplayBrightness = _settingsManager->getBrightness(80); // Default 80
    _brightnessSeekbar.setCurrentValue(loadedDisplayBrightness, false);

    // Adjust max range for screensaver brightness based on current display brightness
    int maxScreensaverBrightnessRange = std::min(loadedDisplayBrightness, SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE);
    _screensaverBrightnessSeekbar.setRange(0, maxScreensaverBrightnessRange);
    _screensaverBrightnessSeekbar.setLimits(0, maxScreensaverBrightnessRange); // Update limits as well

    // Screensaver
    bool screensaverOn = _settingsManager->isScreensaverEnabled(true); // Default true
    _screensaverEnableToggle.setState(screensaverOn, false);
    _screensaverTimeoutSeekbar.setCurrentValue(_settingsManager->getScreensaverTimeout(30), false); // Default 30 sec

    float loadedScreensaverBrightness = _settingsManager->getScreensaverBrightness(10); // Default 10

    // If loaded screensaver brightness is higher than the current main brightness, cap it.
    if (loadedScreensaverBrightness > loadedDisplayBrightness) {
        loadedScreensaverBrightness = loadedDisplayBrightness;
        _settingsManager->setScreensaverBrightness(loadedScreensaverBrightness); // Save the new, capped value
        DEBUG_INFO_PRINTF("SettingsUI: Screensaver brightness capped from %.0f to %d (due to main display brightness).\n", _screensaverBrightnessSeekbar.getCurrentValue(), loadedDisplayBrightness);
    }
    _screensaverBrightnessSeekbar.setCurrentValue(loadedScreensaverBrightness, false);

    // Set interactivity and visual state for screensaver seekbars
    _screensaverTimeoutSeekbar.setInteractive(screensaverOn);
    if (screensaverOn) { _screensaverTimeoutSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _screensaverTimeoutSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    _screensaverBrightnessSeekbar.setInteractive(screensaverOn);
    if (screensaverOn) { _screensaverBrightnessSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _screensaverBrightnessSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    // Apply settings to ScreenSaverManager
    _screensaverManager->setEnabled(screensaverOn);
    _screensaverManager->setTimeout(_settingsManager->getScreensaverTimeout(30) * 1000UL); // Convert seconds to milliseconds
    _screensaverManager->setDimmedBrightness(_settingsManager->getScreensaverBrightness(10)); // Use the potentially modified value

    // Sound
    bool soundOn = _settingsManager->isSoundEnabled(true); // Default true
    _soundEnableToggle.setState(soundOn, false);
    _volumeSeekbar.setCurrentValue(_settingsManager->getSoundVolume(100), false); // Default 100
    _volumeSeekbar.setInteractive(soundOn);
    if (soundOn) { _volumeSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _volumeSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    // Apply settings to AudioManager
    _audioManager->setEnabled(soundOn);
    _audioManager->setVolume(_settingsManager->getSoundVolume(100)); // Apply volume immediately

    // RFID
    bool rfidOn = _settingsManager->isRfidEnabled(true); // Default true
    _rfidToggle.setState(rfidOn, false);
    // Apply settings to RFIDManager
    _rfidManager->setEnabled(rfidOn);
}

/**
 * @brief Retranslates all UI text elements based on the current language setting.
 * This method is called during initialization and whenever the language changes.
 */
void SettingsUI::_retranslateUI() {
    if (!_languageManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("SettingsUI: LanguageManager pointer is null. Cannot retranslate UI.");
        return;
    }

    // Main header
    // The original code uses a special character for 'back' in Hungarian.
    // Ensure the language manager correctly handles this or provide a suitable icon/text.
    _backButton.setLabel(_languageManager->getString("PANEL_BUTTON_BACK", "\xC3\xBA")); // Assuming "\xC3\xBA" is a back arrow or similar
    _titleText.setText(_languageManager->getString("SETTINGS_SCREEN_TITLE", "Settings"));

    // Panel titles (TextUI containers)
    _langPanelContainer.setText(_languageManager->getString("SETTINGS_LANG_CATEGORY", "Language"));
    _displayPanelContainer.setText(_languageManager->getString("SETTINGS_DISPLAY_CATEGORY", "Display"));
    _screensaverPanelContainer.setText(_languageManager->getString("SETTINGS_SCREENSAVER_CATEGORY", "Screensaver"));
    _soundPanelContainer.setText(_languageManager->getString("SETTINGS_SOUND_CATEGORY", "Sound"));
    _rfidPanelContainer.setText(_languageManager->getString("SETTINGS_RFID_CATEGORY", "RFID"));
    _batteryPanelContainer.setText(_languageManager->getString("SETTINGS_BATTERY_CATEGORY", "Battery"));

    // Populate language selection list
    _populateLanguageList();

    // --- ToggleSwitchUI labels ---
    // Update the title for ToggleSwitchUI elements.
    _soundEnableToggle.setTitle(_languageManager->getString("SETTINGS_SOUND_ENABLE", "Enable Sound"));
    _rfidToggle.setTitle(_languageManager->getString("SETTINGS_RFID_ENABLE", "Enable RFID"));
    _screensaverEnableToggle.setTitle(_languageManager->getString("SETTINGS_SCREENSAVER_ENABLE", "Enable Screensaver"));

    _screensaverTimeoutSeekbar.setTitle(_languageManager->getString("SETTINGS_SCREENSAVER_TIMEOUT", "Timeout"), &helvB12);
    _screensaverBrightnessSeekbar.setTitle(_languageManager->getString("SETTINGS_SCREENSAVER_BRIGHTNESS", "Brightness"), &helvB12);
    _brightnessSeekbar.setTitle(_languageManager->getString("SETTINGS_DISPLAY_BRIGHTNESS", "Brightness"), &helvB12);
    _volumeSeekbar.setTitle(_languageManager->getString("SETTINGS_SOUND_VOLUME", "Volume"), &helvB12);
}

/**
 * @brief Handles the back button press event.
 * Pops the settings layer from the screen manager and unregisters the loop callback.
 */
void SettingsUI::_onBackButtonPressed() {
    if (!_screenManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("SettingsUI: ScreenManager pointer is null. Cannot handle back button.");
        return;
    }
    UILayer* settingsLayer = _screenManager->getLayer("settings_layer");
    if (settingsLayer) { // Null pointer check
        settingsLayer->setOnLoopCallback(nullptr); // Deregister the loop callback
        DEBUG_INFO_PRINTLN("SettingsUI: _onBackButtonPressed() - Settings layer loop callback unregistered.");
    } else {
        DEBUG_WARN_PRINTLN("SettingsUI: Settings layer not found when unregistering loop callback.");
    }
    _screenManager->popLayer();
}

/**
 * @brief Callback function when a language is selected from the list.
 * Sets the new language via the LanguageManager if it's different from the current one.
 * @param index The index of the selected item.
 * @param data The ListItem data associated with the selected item.
 * @param touchX The X coordinate of the touch event.
 */
void SettingsUI::_onLanguageSelected(int index, const ListItem& data, int16_t touchX) {
    if (!_languageManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("SettingsUI: LanguageManager pointer is null. Cannot change language.");
        return;
    }
    LanguageManager::Language selectedLang = static_cast<LanguageManager::Language>(data.tag);
    if (_languageManager->getCurrentLanguage() != selectedLang) {
        _languageManager->setLanguage(selectedLang);
    }
}

/**
 * @brief Callback function when the RFID toggle switch state changes.
 * Updates the RFID enabled setting and notifies the RFIDManager.
 * @param newState The new state of the toggle (true for enabled, false for disabled).
 */
void SettingsUI::_onRfidToggleChanged(bool newState) {
    DEBUG_INFO_PRINTF("SettingsUI: RFID Toggle changed to %s\n", newState ? "ON" : "OFF");
    if (!_settingsManager || !_rfidManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or RFIDManager pointer is null. Cannot change RFID state.");
        return;
    }
    _settingsManager->setRfidEnabled(newState);
    _rfidManager->setEnabled(newState); // Actually enables/disables the RFID Manager
}

/**
 * @brief Callback function when the screensaver enable toggle switch state changes.
 * Updates the screensaver enabled setting, adjusts interactivity of related seekbars,
 * and notifies the ScreenSaverManager.
 * @param newState The new state of the toggle (true for enabled, false for disabled).
 */
void SettingsUI::_onScreensaverToggleChanged(bool newState) {
    DEBUG_INFO_PRINTF("SettingsUI: Screensaver Toggle changed to %s\n", newState ? "ON" : "OFF");
    if (!_settingsManager || !_screensaverManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or ScreenSaverManager pointer is null. Cannot change screensaver state.");
        return;
    }
    _settingsManager->setScreensaverEnabled(newState);

    // Adjust UI interactivity for screensaver seekbars
    _screensaverTimeoutSeekbar.setInteractive(newState);
    if (newState) { _screensaverTimeoutSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _screensaverTimeoutSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    _screensaverBrightnessSeekbar.setInteractive(newState);
    if (newState) { _screensaverBrightnessSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _screensaverBrightnessSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    // Enable/disable ScreenSaverManager
    _screensaverManager->setEnabled(newState);
}

/**
 * @brief Callback function when the screensaver timeout seekbar value changes.
 * Updates the screensaver timeout in the ScreenSaverManager immediately and
 * saves the setting if the change is final.
 * @param value The new screensaver timeout value in seconds.
 * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
 */
void SettingsUI::_onScreensaverTimeoutChanged(float value, bool isFinalChange) {
    DEBUG_INFO_PRINTF("SettingsUI: Screensaver Timeout changed to %.0f sec (Final: %s)\n", value, isFinalChange ? "true" : "false");

    if (!_settingsManager || !_screensaverManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or ScreenSaverManager pointer is null. Cannot change screensaver timeout.");
        return;
    }

    // Immediate application always occurs
    int timeoutSeconds = static_cast<int>(value);
    _screensaverManager->setTimeout(static_cast<unsigned long>(timeoutSeconds) * 1000UL); // Convert seconds to milliseconds

    // Save only if the change is final (seekbar released)
    if (isFinalChange) {
        _settingsManager->setScreensaverTimeout(timeoutSeconds);
        DEBUG_INFO_PRINTLN("SettingsUI: Screensaver timeout SAVED.");
    }
}

/**
 * @brief Callback function when the screensaver brightness seekbar value changes.
 * Updates the screensaver dimmed brightness in the ScreenSaverManager immediately and
 * saves the setting if the change is final.
 * @param value The new screensaver brightness level.
 * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
 */
void SettingsUI::_onScreensaverBrightnessChanged(float value, bool isFinalChange) {
    DEBUG_INFO_PRINTF("SettingsUI: Screensaver Brightness changed to %.0f (Final: %s)\n", value, isFinalChange ? "true" : "false");

    if (!_settingsManager || !_screensaverManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or ScreenSaverManager pointer is null. Cannot change screensaver brightness.");
        return;
    }

    // Immediate application always occurs
    uint8_t brightnessLevel = static_cast<uint8_t>(value);
    _screensaverManager->setDimmedBrightness(brightnessLevel);

    // Save only if the change is final
    if (isFinalChange) {
        _settingsManager->setScreensaverBrightness(brightnessLevel);
        DEBUG_INFO_PRINTLN("SettingsUI: Screensaver brightness SAVED.");
    }
}


/**
 * @brief Callback function when the main display brightness seekbar value changes.
 * Applies the new display brightness immediately to the LCD, adjusts the range of the
 * screensaver brightness seekbar, and saves the setting if the change is final.
 * @param value The new display brightness level.
 * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
 */
void SettingsUI::_onBrightnessChanged(float value, bool isFinalChange) {
    DEBUG_INFO_PRINTF("SettingsUI: Display Brightness changed to %.0f (Final: %s)\n", value, isFinalChange ? "true" : "false");

    if (!_lcd || !_settingsManager || !_screensaverManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: LCD, SettingsManager, or ScreenSaverManager pointer is null. Cannot change display brightness.");
        return;
    }

    int newDisplayBrightness = static_cast<int>(value);
    _lcd->setBrightness(static_cast<uint8_t>(newDisplayBrightness)); // Apply brightness immediately

    // Synchronize screensaver brightness with main display brightness, but cap at SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE
    int maxScreensaverBrightnessRange = std::min(newDisplayBrightness, SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE);
    _screensaverBrightnessSeekbar.setRange(0, maxScreensaverBrightnessRange);
    _screensaverBrightnessSeekbar.setLimits(0, maxScreensaverBrightnessRange);

    float currentScreensaverBrightness = _screensaverBrightnessSeekbar.getCurrentValue(); // This is the *value* of the screensaver seekbar

    bool screensaverBrightnessWasCapped = false; // Flag to track if the screensaver value was capped
    if (currentScreensaverBrightness > maxScreensaverBrightnessRange) {
        DEBUG_INFO_PRINTF("SettingsUI: Screensaver brightness reduced from %.0f to %d (due to main brightness change).\n", currentScreensaverBrightness, maxScreensaverBrightnessRange);
        _screensaverBrightnessSeekbar.setCurrentValue(static_cast<float>(maxScreensaverBrightnessRange), isFinalChange); // This also calls its own callback
        screensaverBrightnessWasCapped = true; // Value was capped
    }

    // Save the main display brightness only if the change is final (DisplayBrightnessSeekbar released)
    if (isFinalChange) {
        _settingsManager->setBrightness(newDisplayBrightness);
        DEBUG_INFO_PRINTLN("SettingsUI: Display brightness SAVED.");

        // IMPORTANT: Now we also save/update the screensaver brightness,
        // IF THE SCREENSAVER SEEKBAR VALUE DID NOT CHANGE (i.e., it wasn't "capped")
        // BUT ITS RANGE CHANGED (moved upwards).
        if (!screensaverBrightnessWasCapped) {
            // The Seekbar's setCurrentValue(value, true) CALLS THE CALLBACK, which saves.
            // Here, `isFinalChange` is inherited from the main (display) seekbar.
            // Since `setCurrentValue` only calls the callback if `_currentValue != constrainedValue`,
            // and `currentScreensaverBrightness` (e.g., 50) probably doesn't change here,
            // we need to manually trigger the `_onScreensaverBrightnessChanged` callback with `isFinalChange=true`.
            // The simplest way is to call the `_onScreensaverBrightnessChanged` method directly.
            _onScreensaverBrightnessChanged(currentScreensaverBrightness, true); // <--- DIRECT CALL FOR FINAL SAVE
            DEBUG_INFO_PRINTLN("SettingsUI: Screensaver brightness (passive update) SAVED.");
        }
    }
}

/**
 * @brief Callback function when the sound enable toggle switch state changes.
 * Updates the sound enabled setting, adjusts interactivity of the volume seekbar,
 * and notifies the AudioManager.
 * @param newState The new state of the toggle (true for enabled, false for disabled).
 */
void SettingsUI::_onSoundToggleChanged(bool newState) {
    DEBUG_INFO_PRINTF("SettingsUI: Sound Toggle changed to %s\n", newState ? "ON" : "OFF");
    if (!_settingsManager || !_audioManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or AudioManager pointer is null. Cannot change sound state.");
        return;
    }
    _settingsManager->setSoundEnabled(newState);
    _volumeSeekbar.setInteractive(newState);
    if (newState) { _volumeSeekbar.setVisualState(UIVisualState::ACTIVE); }
    else { _volumeSeekbar.setVisualState(UIVisualState::NON_INTERACTIVE); }

    _audioManager->setEnabled(newState);
}

/**
 * @brief Callback function when the volume seekbar value changes.
 * Sets the new volume in the AudioManager immediately and saves the setting
 * if the change is final.
 * @param value The new volume level.
 * @param isFinalChange True if this is the final value after dragging, false for continuous changes.
 */
void SettingsUI::_onVolumeChanged(float value, bool isFinalChange) {
    DEBUG_INFO_PRINTF("SettingsUI: Volume changed to %.0f (Final: %s)\n", value, isFinalChange ? "true" : "false");

    if (!_settingsManager || !_audioManager) { // Null pointer checks
        DEBUG_ERROR_PRINTLN("SettingsUI: SettingsManager or AudioManager pointer is null. Cannot change volume.");
        return;
    }

    // Immediate application always occurs
    _audioManager->setVolume(static_cast<int>(value));

    // Save only if the change is final
    if (isFinalChange) {
        _settingsManager->setSoundVolume(static_cast<int>(value));
        DEBUG_INFO_PRINTLN("SettingsUI: Volume SAVED.");
    }
}

/**
 * @brief Populates the language selection list with available languages from the LanguageManager.
 * Also sets the currently selected language in the list.
 */
void SettingsUI::_populateLanguageList() {
    if (!_languageManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("SettingsUI: LanguageManager pointer is null. Cannot populate language list.");
        return;
    }
    std::vector<LanguageInfo> availableLangs = _languageManager->getAvailableLanguages();
    LanguageManager::Language currentLang = _languageManager->getCurrentLanguage();
    int indexToSelect = -1;
    _languageList.clearItems();
    for (int i = 0; i < availableLangs.size(); ++i) {
        const auto& langInfo = availableLangs[i];
        ListItem item;
        item.columns.push_back(ColumnData(langInfo.displayName));
        item.tag = static_cast<intptr_t>(langInfo.langEnum);
        _languageList.addItem(item);
        if (langInfo.langEnum == currentLang) {
            indexToSelect = i;
        }
    }
    if (indexToSelect != -1) {
        _languageList.setSelectedItemIndex(indexToSelect, true);
    }
}

/**
 * @brief Loop callback for the settings UI layer.
 * This method is called periodically when the settings layer is active.
 * It's used for dynamic updates, such as battery voltage display.
 */
void SettingsUI::_settingsLoop() {
    // This is where dynamic updates can happen when the SettingsUI layer is ACTIVE
    // For example, updating battery voltage.

    // Update battery voltage (originally from _onBatteryVoltageUpdate method)
    if (_powerManager && (millis() - _lastUpdateMillis >= UPDATE_INTERVAL_MS)) { // Null pointer check and interval check
        float newVoltage = _powerManager->getCurrentVoltage(); // Get voltage
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%.2fV", newVoltage);
        _batteryVoltageLabel.setText(buffer); // Update the UI element
        // DEBUG_TRACE_PRINTF("SettingsUI: _settingsLoop - Battery voltage updated: %.2fV\n", newVoltage);
        _lastUpdateMillis = millis(); // Update last update time
    }
    // Other periodic updates can be added here, e.g., WiFi/BLE status query if relevant in SettingsUI.
}