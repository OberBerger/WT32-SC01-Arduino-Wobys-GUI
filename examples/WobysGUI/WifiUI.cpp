/**
 * @file WifiUI.cpp
 * @brief Implements the WifiUI class for managing Wi-Fi settings and connections.
 *
 * This file contains the implementation details for the WifiUI class, including
 * its initialization, layout setup, interaction handling, and integration with
 * various system managers for Wi-Fi operations and persistent settings.
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
#include "WifiUI.h"
#include <Arduino.h>   // For Arduino specific functions like millis()
#include <algorithm>   // For std::min, std::find_if
#include <cstdio>      // For snprintf
#include <string>      // For std::string, std::to_string
#include <set>

// --- Constructor ---
/**
 * @brief Constructor for the WifiUI class.
 * Initializes the WifiUI with pointers to essential manager and UI components.
 *
 * @param lcd Pointer to the LGFX display object.
 * @param screenManager Pointer to the ScreenManager for layer management.
 * @param wifiManager Pointer to the WifiManager for Wi-Fi operations.
 * @param settingsManager Pointer to the SettingsManager for persistent settings.
 * @param statusbar Pointer to the StatusbarUI for status bar interaction.
 * @param languageManager Pointer to the LanguageManager for UI internationalization.
 */
WifiUI::WifiUI(LGFX* lcd,
               ScreenManager* screenManager,
               WifiManager* wifiManager,
               SettingsManager* settingsManager,
               StatusbarUI* statusbar,
               LanguageManager* languageManager)
  : _lcd(lcd),
    _screenManager(screenManager),
    _wifiManager(wifiManager),
    _settingsManager(settingsManager),
    _statusbarPtr(statusbar),
    _languageManager(languageManager), // Initialize new member variable
    _backButton(lcd, "", 0, 0, 0, 0, &iconic_all2x), // Label set to empty, will be set by _retranslateUI
    _titleText(lcd, "", 0, 0),
    _wifiToggle(lcd, "", "", 0, 0, 0, 0, &helvB12, false),
    _scanButton(lcd, "", 0, 0, 0, 0, &helvB12),
    _statusText(lcd, "", 0, 0),
    _networkList(lcd, 0, 0, 0, 0, 1),
    _passwordKeyboard(lcd, ""), // Title will be set by _retranslateUI
    _dialogBackground(lcd, "", 0, 0),
    _dialogQuestion(lcd, "", 0, 0),
    _dialogSsid(lcd, "", 0, 0),
    _dialogYesButton(lcd, "", 0, 0, 0, 0, &helvB18), // Label will be set by _retranslateUI
    _dialogNoButton(lcd, "", 0, 0, 0, 0, &helvB18),   // Label will be set by _retranslateUI
    _pendingSavedPasswordAttempt(false),
    _g_connectAfterScan_flag(false),
    _g_isNewPasswordPendingSave(false) {
}

// --- Public Methods ---
/**
 * @brief Initializes the Wi-Fi UI components, sets up their layout, and registers callbacks.
 * This method should be called once after the constructor.
 */
void WifiUI::init() {
  DEBUG_INFO_PRINTLN("WifiUI: Initializing Wi-Fi UI components.");

  // Null pointer checks for essential managers
  if (!_lcd || !_screenManager || !_wifiManager || !_settingsManager || !_statusbarPtr || !_languageManager) {
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential manager pointers are null. Initialization aborted.");
      return;
  }

  // Register for language change notifications
  _languageManager->registerForUpdate("WifiUI", [this]() { this->_retranslateUI(); });

  // Setup manager callbacks
  _wifiManager->setOnScanCompleteCallback(
    [this](bool success, const std::vector<WifiListItemData>& networks) {
      this->_handleScanComplete(success, networks);
    });
  _wifiManager->setOnConnectionStateChangedCallback(
    [this](WifiMgr_State_t state,
           const std::string& ssid,
           const std::string& ip) {
      this->_handleWifiStateChange(state, ssid, ip);
    });
  _wifiManager->setOnRssiChangeCallback([this](int32_t rssi) {
  });

  // --- Wi-Fi Settings Layer ("wifi_settings_layer") ---
  _screenManager->defineLayer("wifi_settings_layer",
                              UILayer(_lcd,
                                      false,
                                      true,
                                      PANEL_BACKGROUND_COLOR,
                                      OrientationPreference::CONTENT_LANDSCAPE));
  UILayer* layer = _screenManager->getLayer("wifi_settings_layer");
  if (!layer) {
    DEBUG_ERROR_PRINTLN("WifiUI: Failed to create or retrieve 'wifi_settings_layer'. Initialization aborted.");
    return;
  }

  uint16_t layerWidth = TFT_HEIGHT; // Consider using _lcd->width() / _lcd->height()
  uint16_t layerHeight = TFT_WIDTH - STATUSBAR_HEIGHT;
  uint8_t margin = 10;
  uint8_t topBarHeight = 30;
  uint8_t topBarY_inLayer = 5;

  _backButton.setPosition(margin, topBarY_inLayer);
  _backButton.setSize(50, topBarHeight);
  _backButton.setFont(&iconic_all2x);
  _backButton.setOnReleaseCallback([this]() { this->_closePanel(); });
  layer->addElement(&_backButton);

  uint16_t scanButtonWidth = 90;
  _scanButton.setPosition(layerWidth - margin - scanButtonWidth,
                          topBarY_inLayer);
  _scanButton.setSize(scanButtonWidth, topBarHeight);
  _scanButton.setFont(&helvB12);
  _scanButton.setOnReleaseCallback([this]() { this->_onScanButtonPressed(); });
  layer->addElement(&_scanButton);

  uint16_t toggleButtonWidth = 80;
  _wifiToggle.setPosition(
    _scanButton.getX() - margin - toggleButtonWidth, topBarY_inLayer);
  _wifiToggle.setSize(toggleButtonWidth, topBarHeight);
  _wifiToggle.setFont(&helvB12);
  _wifiToggle.setOnToggleCallback([this](bool s) { this->_onToggleChanged(s); });
  layer->addElement(&_wifiToggle);

  int titleStartX = _backButton.getX() + _backButton.getWidth() + margin;
  int titleAvailableWidth = _wifiToggle.getX() - margin - titleStartX;
  _titleText.setPosition(titleStartX, topBarY_inLayer);
  _titleText.setSize(titleAvailableWidth > 10 ? titleAvailableWidth : 10,
                     topBarHeight);
  _titleText.setFont(&helvB18);
  _titleText.setTextColor(UI_COLOR_TEXT_DEFAULT);
  _titleText.setBackgroundColor(PANEL_BACKGROUND_COLOR);
  _titleText.setTextDatum(MC_DATUM);
  layer->addElement(&_titleText);

  uint8_t statusTextY_inLayer = topBarY_inLayer + topBarHeight + 8;
  uint8_t statusTextHeight = 20;
  _statusText.setPosition(margin, statusTextY_inLayer);
  _statusText.setSize(layerWidth - (2 * margin), statusTextHeight);
  _statusText.setFont(&helvB12);
  _statusText.setTextColor(UI_COLOR_TEXT_DEFAULT);
  _statusText.setBackgroundColor(PANEL_BACKGROUND_COLOR);
  _statusText.setTextDatum(TC_DATUM);
  layer->addElement(&_statusText);

  uint8_t listY_inLayer = statusTextY_inLayer + statusTextHeight + 8;
  uint16_t listHeight_inLayer = layerHeight - listY_inLayer - margin - 6;
  _networkList.setPosition(margin, listY_inLayer);
  _networkList.setSize(layerWidth - (2 * margin), listHeight_inLayer);
  _networkList.setItemHeight(35);
  _networkList.setNumColumns(4);
  _networkList.setDrawBorder(true);
  _networkList.setDrawDividers(true);

  uint16_t iconColWidth = 35;
  uint16_t deleteColWidth = 35;
  uint16_t totalFixedWidth = iconColWidth + iconColWidth + deleteColWidth;
  uint16_t ssidColWidth =
    _networkList.getWidth() - totalFixedWidth - LISTUI_SCROLL_BAR_WIDTH_PIXELS - 2;
  if (ssidColWidth < 50) ssidColWidth = 50;

  _networkList.setColumnWidth(0, ssidColWidth);
  _networkList.setColumnWidth(1, iconColWidth);
  _networkList.setColumnWidth(2, iconColWidth);
  _networkList.setColumnWidth(3, deleteColWidth);
  _networkList.setColumnDefaultAlignment(0, ML_DATUM);
  _networkList.setColumnDefaultAlignment(1, MC_DATUM);
  _networkList.setColumnDefaultAlignment(2, MC_DATUM);
  _networkList.setColumnDefaultAlignment(3, MC_DATUM);
  _networkList.setColumnDefaultFont(0, &helvR14);
  _networkList.setColumnDefaultFont(1, &iconic_all2x);
  _networkList.setColumnDefaultFont(2, &battery);
  _networkList.setColumnDefaultFont(3, &helvB14);
  _networkList.setColumnDefaultTextColor(3, LISTUI_ITEM_DELETE_ACTION_COLOR);
  _networkList.setOnItemSelectedCallback(
    [this](int i, const ListItem& d, int16_t t) {
      this->_onNetworkSelected(i, d, t);
    });
  _networkList.clearItems();
  layer->addElement(&_networkList);

  // --- Password Keyboard Layer ---
  _screenManager->defineLayer("keyboardLayer_wifi_password",
                              UILayer(_lcd, false, true, TFT_BLACK));
  UILayer* keyboardLayer = _screenManager->getLayer("keyboardLayer_wifi_password");
  if (keyboardLayer) { // Null pointer check
    _passwordKeyboard.setOnEnterCallback(
      [this](const std::string& t) { this->_onPasswordEntered(t); });
    keyboardLayer->addElement(&_passwordKeyboard);
  } else {
      DEBUG_ERROR_PRINTLN("WifiUI: Failed to get 'keyboardLayer_wifi_password' for adding keyboard.");
  }


  // --- Confirmation Dialog Layer ---
  _screenManager->defineLayer("confirmation_dialog_layer",
                              UILayer(_lcd, false, false, DIALOG_BOX_BACKGROUND_COLOR));
  UILayer* dialogLayer = _screenManager->getLayer("confirmation_dialog_layer");
  if (dialogLayer) { // Null pointer check
    uint16_t screenWidth = _lcd->width();
    uint16_t availableContentHeight = _lcd->height() - STATUSBAR_HEIGHT;
    uint16_t dialogWidth = std::min(static_cast<int16_t>(screenWidth * 0.85), static_cast<int16_t>(380));
    uint16_t dialogHeight = std::min(static_cast<int16_t>(availableContentHeight * 0.6), static_cast<int16_t>(190));
    if (dialogHeight < 160) dialogHeight = 160;
    uint16_t dialogPanelX = (screenWidth - dialogWidth) / 2;
    uint16_t dialogPanelY = (availableContentHeight - dialogHeight) / 2;
    uint8_t p = UI_DEFAULT_MARGIN_PIXELS;;

    _dialogBackground.setSize(dialogWidth, dialogHeight);
    _dialogBackground.setPosition(dialogPanelX, dialogPanelY);
    _dialogBackground.setBackgroundColor(DIALOG_BOX_BACKGROUND_COLOR);
    _dialogBackground.setBorder(DIALOG_BOX_BORDER_COLOR, 1, TextUI_BorderType::SINGLE);
    _dialogBackground.setText(""); // No text directly on background
    dialogLayer->addElement(&_dialogBackground);

    uint16_t buttonSectionHeight = 40 + p;
    uint16_t totalTextSectionHeight = dialogHeight - (2 * p) - buttonSectionHeight;
    uint16_t questionTextHeight = static_cast<uint16_t>(totalTextSectionHeight * 0.45);
    uint16_t ssidTextHeight = static_cast<uint16_t>(totalTextSectionHeight * 0.55);
    uint16_t currentY_forText = dialogPanelY + p;

    _dialogQuestion.setWordWrap(true);
    _dialogQuestion.setFont(&helvB18);
    _dialogQuestion.setTextColor(DIALOG_TEXT_COLOR);
    _dialogQuestion.setBackgroundColor(TEXTUI_TRANSPARENT);
    _dialogQuestion.setTextDatum(MC_DATUM);
    _dialogQuestion.setSize(dialogWidth - (2 * p), questionTextHeight);
    _dialogQuestion.setPosition(dialogPanelX + p, currentY_forText);
    _dialogQuestion.setPadding(2);
    dialogLayer->addElement(&_dialogQuestion);

    currentY_forText += questionTextHeight;

    _dialogSsid.setFont(&helvR14);
    _dialogSsid.setTextColor(DIALOG_TEXT_COLOR);
    _dialogSsid.setBackgroundColor(TEXTUI_TRANSPARENT);
    _dialogSsid.setTextDatum(MC_DATUM);
    _dialogSsid.setSize(dialogWidth - (2 * p), ssidTextHeight);
    _dialogSsid.setPosition(dialogPanelX + p, currentY_forText);
    _dialogSsid.setPadding(5);
    dialogLayer->addElement(&_dialogSsid);

    uint16_t buttonHeight = 40;
    uint16_t buttonWidth = (dialogWidth - (3 * p)) / 2;
    uint16_t buttonsY = dialogPanelY + dialogHeight - p - buttonHeight;
    uint16_t noButtonX = dialogPanelX + p;
    uint16_t yesButtonX = dialogPanelX + p + buttonWidth + p;

    _dialogNoButton.setSize(buttonWidth, buttonHeight);
    _dialogNoButton.setPosition(noButtonX, buttonsY);
    _dialogNoButton.setFont(&helvB18);
    _dialogNoButton.setColors(BUTTON_OUTLINE_COLOR,
                              BUTTON_FILL_COLOR_NORMAL,
                              BUTTON_FILL_COLOR_PRESSED,
                              BUTTON_TEXT_COLOR_NORMAL,
                              BUTTON_TEXT_COLOR_PRESSED);
    _dialogNoButton.setOnReleaseCallback([this]() { this->_onConfirmNo(); });
    dialogLayer->addElement(&_dialogNoButton);

    _dialogYesButton.setSize(buttonWidth, buttonHeight);
    _dialogYesButton.setPosition(yesButtonX, buttonsY);
    _dialogYesButton.setFont(&helvB18);
    _dialogYesButton.setColors(BUTTON_OUTLINE_COLOR,
                               BUTTON_FILL_COLOR_NORMAL,
                               BUTTON_FILL_COLOR_PRESSED,
                               BUTTON_TEXT_COLOR_NORMAL,
                               BUTTON_TEXT_COLOR_PRESSED);
    _dialogYesButton.setOnReleaseCallback([this]() { this->_onConfirmYes(); });
    dialogLayer->addElement(&_dialogYesButton);
  } else {
      DEBUG_ERROR_PRINTLN("WifiUI: Failed to get 'confirmation_dialog_layer' for adding dialog elements.");
  }
  _retranslateUI(); // Crucial to be at the end of init to set all texts
}

/**
 * @brief Retranslates all UI text elements based on the current language setting.
 * This method is called during initialization and whenever the language changes.
 */
void WifiUI::_retranslateUI() {
    if (!_languageManager) { // Null pointer check
        DEBUG_ERROR_PRINTLN("WifiUI: LanguageManager pointer is null. Cannot retranslate UI.");
        return;
    }
    _passwordKeyboard.setTitle(_languageManager->getString("KEYBOARD_PASSWORD_TITLE", "Password:"));
    _backButton.setLabel(_languageManager->getString("PANEL_BUTTON_BACK", "\xC3\xBA")); // Back arrow character
    _titleText.setText(_languageManager->getString("WIFI_SETTINGS_TITLE", "Wi-Fi Settings"));
    _wifiToggle.setLabels(_languageManager->getString("WIFI_TOGGLE_ON", "ON"), _languageManager->getString("WIFI_TOGGLE_OFF", "OFF"));
    _scanButton.setLabel(_languageManager->getString("SCAN_BUTTON", "Scan"));
    _dialogQuestion.setText(_languageManager->getString("WIFI_DELETE_DIALOG_QUESTION", "Are you sure you want to delete password for?"));
    _dialogYesButton.setLabel(_languageManager->getString("WIFI_DELETE_DIALOG_YES", "Yes"));
    _dialogNoButton.setLabel(_languageManager->getString("WIFI_DELETE_DIALOG_NO", "No"));

    // Retrieve the actual last scanned networks from WifiManager to repopulate the list with the new language.
    //const std::vector<WifiListItemData>& lastNetworks = _wifiManager->getLastScannedNetworks();
    //_handleScanComplete(true, lastNetworks);

    //Update status text based on current Wi-Fi state
    _handleWifiStateChange(_wifiManager->getCurrentState(), _wifiManager->getConnectedSsid(), _wifiManager->getIpAddress());
}

/**
 * @brief Opens the Wi-Fi settings panel.
 * This method handles the transition to the Wi-Fi screen, ensuring proper status bar panel closure
 * if it's open, before pushing the Wi-Fi settings layer to the screen manager.
 */
void WifiUI::openPanel() {
  if (!_screenManager || !_statusbarPtr || !_wifiManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: ScreenManager, Statusbar, or WifiManager pointer is null. Cannot open panel.");
      return;
  }

  // If the Wi-Fi settings layer is already at the top, just close the status bar panel and return.
  if (_screenManager->getTopLayerName() == "wifi_settings_layer") {
    if (_statusbarPtr->hasPanel() && _statusbarPtr->isPanelOpenOrOpening()) {
        _statusbarPtr->closePanel();
    }
    return;
  }

  // Handle status bar panel interaction before opening Wi-Fi panel
  if (_statusbarPtr->hasPanel()) {
    if (_statusbarPtr->isPanelOpenOrOpening()) {
      _statusbarPtr->setOnPanelFullyClosedCallback(
        [this]() { this->proceedToOpenPanel(); });
      _statusbarPtr->closePanel();
    } else if (_statusbarPtr->isPanelFullyClosedOrNotPresent()) {
      proceedToOpenPanel();
    } else { // Fallback for other states, ensure callback is set
      _statusbarPtr->setOnPanelFullyClosedCallback(
        [this]() { this->proceedToOpenPanel(); });
    }
  } else { // No pull-down status bar panel exists
    proceedToOpenPanel();
  }
}

/**
 * @brief Proceeds to open the Wi-Fi settings panel layer after any prerequisite actions (e.g., status bar panel closure).
 * This method is typically called as a callback after the status bar panel is fully closed.
 */
void WifiUI::proceedToOpenPanel() {
  DEBUG_INFO_PRINTLN("WifiUI: Proceeding to open Wi-Fi panel.");

  if (!_screenManager || !_wifiManager || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: ScreenManager, WifiManager, or LanguageManager pointer is null. Cannot proceed to open panel.");
      return;
  }

  bool actualWifiLogicState = _wifiManager->isWifiLogicEnabled();
  _wifiToggle.setState(actualWifiLogicState, false);
  _screenManager->pushLayer("wifi_settings_layer");

  if (actualWifiLogicState) {
    // If scanning cannot be started, indicate in status text.
    if (!_wifiManager->startScan()) {
        DEBUG_WARN_PRINTLN("WifiUI::proceedToOpenPanel: Scan cannot be started.");
        _statusText.setText(_languageManager->getString("STATUS_SCAN_NOT_POSSIBLE", "Scan not possible.")); // Feedback to user
    }
  } else {
    _networkList.clearItems();
    _networkList.setSelectedItemIndex(-1, true);
    _statusText.setText(_languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled."));
  }
}

// --- Private Methods (Previously Global Callbacks) ---

/**
 * @brief Closes the Wi-Fi settings panel by popping its layer from the ScreenManager.
 */
void WifiUI::_closePanel() {
  if (!_screenManager) { // Null pointer check
      DEBUG_ERROR_PRINTLN("WifiUI: ScreenManager pointer is null. Cannot close panel.");
      return;
  }
  _screenManager->popLayer();
}

/**
 * @brief Callback function triggered when the Wi-Fi enable/disable toggle changes state.
 * Enables or disables Wi-Fi logic and saves the new state.
 * @param newState The new state of the Wi-Fi toggle (true for ON, false for OFF).
 */
void WifiUI::_onToggleChanged(bool newState) {
  if (!_wifiManager || !_settingsManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: WifiManager or SettingsManager pointer is null. Cannot change Wi-Fi toggle state.");
      return;
  }
  if (newState) {
    _wifiManager->enableWifi(true);
  } else {
    _wifiManager->disableWifi();
  }
  _settingsManager->setWifiEnabledLastState(newState); // UI saves the state
}

/**
 * @brief Callback function triggered when the "Scan" button is pressed.
 * Initiates a Wi-Fi network scan if Wi-Fi is enabled.
 */
void WifiUI::_onScanButtonPressed() {
  if (!_wifiManager || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: WifiManager or LanguageManager pointer is null. Cannot start scan.");
      return;
  }

  if (_wifiManager->isWifiLogicEnabled()) {
    // If startScan returns false, indicate this in the status text.
    if (!_wifiManager->startScan()) {
        _statusText.setText(_languageManager->getString("STATUS_SCAN_NOT_POSSIBLE", "Scan not possible."));
    }
  } else {
    _statusText.setText(_languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled.") + _languageManager->getString("STATUS_SCAN_NOT_POSSIBLE", " Scan not possible."));
    _networkList.clearItems();
  }
}

/**
 * @brief Callback function triggered when a network item in the list is selected.
 * Handles connection attempts, password entry, or "forget network" actions based on user interaction.
 * @param index The index of the selected item in the list.
 * @param data The ListItem data associated with the selected network.
 * @param touchX The X coordinate of the touch event within the list item.
 */
void WifiUI::_onNetworkSelected(int index,
                                const ListItem& data,
                                int16_t touchX_inItem) {
  if (!_wifiManager || !_settingsManager || !_languageManager || !_screenManager || !_lcd) { // Extensive null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot handle network selection.");
      return;
  }

  if (data.columns.empty()) {
    _statusText.setText(_languageManager->getString("STATUS_AMBIGUOUS_DATA", "Error: Ambiguous data."));
    return;
  }

  std::string selectedSSID = data.columns[0].text;
  int clickedColumn = _networkList.getClickedColumnIndex(touchX_inItem);
  const int DELETE_COLUMN_INDEX = 3;

  // Handle "forget network" action
  if (clickedColumn == DELETE_COLUMN_INDEX &&
      data.columns.size() > DELETE_COLUMN_INDEX &&
      data.columns[DELETE_COLUMN_INDEX].text == _languageManager->getString("TEXT_DELETE_ACTION", "X")) {
    _showConfirmationDialog(selectedSSID, selectedSSID);
    return;
  }

  // If Wi-Fi logic is disabled, prevent connection attempts.
  if (!_wifiManager->isWifiLogicEnabled()) {
    _statusText.setText(_languageManager->getString("STATUS_DISABLED_FOR_CONNECT", "Wi-Fi disabled for connection."));
    return;
  }

  // Check if the selected network is ALREADY CONNECTED
  if (_wifiManager->getCurrentState() == WifiMgr_State_t::CONNECTED &&
      _wifiManager->getConnectedSsid() == selectedSSID) {
    if (_languageManager) {
      _statusText.setText(_languageManager->getString("WIFI_STATUS_ALREADY_CONNECTED", "Already connected to this network."));
    } else {
      _statusText.setText("Already connected to this network.");
    }
    // Maintain the selection on the currently connected item.
    // The list's default behavior might clear selection on a click that doesn't change anything,
    // so explicitly setting it again ensures it stays.
    _networkList.setSelectedItemIndex(index, true); 
    return; 
  }

  // Check if network is protected (has a lock icon)
  bool isProtected = (data.columns.size() > 1 &&
                      !data.columns[1].text.empty() &&
                      data.columns[1].text == _languageManager->getString("ICON_LOCK_CLOSED", "\u00CA"));

  // Check if a password for this network is already saved
  auto saved = _settingsManager->getSavedNetworks();
  auto it = std::find_if(saved.begin(),
                         saved.end(),
                         [&](const SavedWifiNetwork& net) {
                           return net.ssid == selectedSSID;
                         });

  // If password is saved and not empty, attempt connection with saved password
  if (it != saved.end() && !it->password.empty()) {
    _pendingSavedPasswordAttempt = true;
    _pendingSavedPasswordSsid = selectedSSID;
    _statusText.setText(_languageManager->getString("STATUS_CONNECTING_SAVED", "Connecting with saved password: ") + selectedSSID);
    _wifiManager->connectToNetwork(selectedSSID, it->password);
    return;
  }

  // If network is protected and no saved password, prompt for password
  if (isProtected) {
    _pendingSavedPasswordAttempt = false;
    _ssidForPasswordEntry = selectedSSID;
    _passwordKeyboard.clearText();
    _statusText.setText(_languageManager->getString("STATUS_PASS_PROMPT", "Password: ") + selectedSSID);
    std::string keyboardTitle = _languageManager->getString("KEYBOARD_PASSWORD_TITLE", "Password:") + " (" + selectedSSID + "):";
    _passwordKeyboard.setTitle(keyboardTitle);
    _screenManager->pushLayer("keyboardLayer_wifi_password");
  } else {
    // If not protected and no saved password, attempt direct connection
    _pendingSavedPasswordAttempt = false;
    _statusText.setText(_languageManager->getString("STATUS_CONNECTING", "Connecting: ") + selectedSSID + "...");
    _wifiManager->connectToNetwork(selectedSSID);
  }
}

/**
 * @brief Callback function triggered when a password is entered via the keyboard.
 * Handles connection attempts with the entered password or closes the keyboard.
 * @param textFromKeyboard The password string entered by the user, or a special escape string.
 */
void WifiUI::_onPasswordEntered(const std::string& textFromKeyboard) {
  if (!_screenManager || !_wifiManager || !_languageManager || !_settingsManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot handle password entry.");
      return;
  }

  // Handle escape (cancel) from keyboard
  if (textFromKeyboard == KEYBOARD_ESCAPE_BUTTON_ACTION_STRING) {
    _screenManager->popLayer();
    _ssidForPasswordEntry = "";

    // If Wi-Fi panel is still active, update its status text and selected item
    bool wifiPanelIsActive =
      (_screenManager->getTopLayerName() == "wifi_settings_layer");
    if (wifiPanelIsActive) {
      WifiMgr_State_t currentState = _wifiManager->getCurrentState();
      std::string currentConnectedSsid = _wifiManager->getConnectedSsid();
      std::string statusMsgForEscape = "";

      switch (currentState) {
        case WifiMgr_State_t::CONNECTED:
          statusMsgForEscape = _languageManager->getString("STATUS_CONNECTED", "Connected: ") + currentConnectedSsid +
                               _languageManager->getString("STATUS_IP_ADDRESS", " (IP: ") + _wifiManager->getIpAddress() + ")";
          break;
        case WifiMgr_State_t::DISCONNECTED:
          statusMsgForEscape = _wifiManager->isWifiLogicEnabled()
                                 ? _languageManager->getString("STATUS_ENABLED_DISCONNECTED", "Wi-Fi ON, disconnected.")
                                 : _languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled.");
          break;
        case WifiMgr_State_t::SCANNING:
          statusMsgForEscape = _languageManager->getString("STATUS_SCANNING", "Scanning networks...");
          break;
        default:
          if (_wifiManager->isWifiLogicEnabled()) {
            statusMsgForEscape = _languageManager->getString("STATUS_WIFI_ACTIVE", "Wi-Fi active.");
          } else {
            statusMsgForEscape = _languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled.");
          }
          break;
      }
      if (!statusMsgForEscape.empty()) {
        _statusText.setText(statusMsgForEscape);
      }

      int indexToSelectAfterEscape = -1;
      if (currentState == WifiMgr_State_t::CONNECTED &&
          !currentConnectedSsid.empty()) {
        const auto& items = _networkList.getItems();
        for (int i = 0; i < items.size(); ++i) {
          if (items[i].columns.size() > 0 &&
              items[i].columns[0].text == currentConnectedSsid) {
            indexToSelectAfterEscape = i;
            break;
          }
        }
      }
      _networkList.setSelectedItemIndex(indexToSelectAfterEscape, true);
    }
    return;
  }

  // Process entered password for connection
  std::string currentSsid = _ssidForPasswordEntry;
  _screenManager->popLayer(); // Close keyboard layer

  if (!currentSsid.empty()) {
    _g_ssidToConnectAfterScan = currentSsid;
    _g_passwordForConnectionAfterScan = textFromKeyboard;
    _g_isNewPasswordPendingSave = true; // Mark password for saving on successful connection

    if (_screenManager->getTopLayerName() == "wifi_settings_layer") {
      _statusText.setText(_languageManager->getString("STATUS_CONNECTING_ATTEMPT", "Connecting attempt: ") + currentSsid + "...");
    }

    // Attempt to connect with the provided SSID and password
    if (!_wifiManager->connectToNetwork(currentSsid, textFromKeyboard)) {
      if (_screenManager->getTopLayerName() == "wifi_settings_layer") {
        _statusText.setText(_languageManager->getString("STATUS_CANNOT_START_CONNECT", "Connection cannot be started."));
      }
      // Reset flags if connection attempt failed to start
      _g_isNewPasswordPendingSave = false;
      _g_ssidToConnectAfterScan = "";
      _g_passwordForConnectionAfterScan = "";
    }
  }
  _ssidForPasswordEntry = ""; // Clear pending SSID for password entry
}

/**
 * @brief Callback function triggered when the "Yes" button in the confirmation dialog is pressed.
 * Proceeds to forget the selected network's password.
 */
void WifiUI::_onConfirmYes() {
  if (!_screenManager || !_settingsManager || !_wifiManager || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot confirm network deletion.");
      return;
  }

  std::string ssidToForget = _ssidToForget;
  _screenManager->popLayer(); // Close dialog layer
  _ssidToForget = "";         // Clear pending SSID to forget

  bool wifiPanelIsActive =
    (_screenManager->getTopLayerName() == "wifi_settings_layer");
  if (wifiPanelIsActive && !ssidToForget.empty()) {
    // Check if the network to be forgotten is currently connected
    bool wasConnected =
      (_wifiManager->getCurrentState() == WifiMgr_State_t::CONNECTED &&
       _wifiManager->getConnectedSsid() == ssidToForget);

    // Attempt to remove saved network credentials
    if (_settingsManager->removeSavedNetwork(ssidToForget)) {
      _statusText.setText(_languageManager->getString("STATUS_PASS_DELETED", "Password deleted."));
      if (wasConnected) {
        _wifiManager->disconnectFromNetwork(); // Disconnect if it was the active network
      }

      // This section duplicates logic found in _handleScanComplete() or a dedicated list builder.
      std::vector<ListItem> uiListItems;
      const auto& lastScannedNetworks = _wifiManager->getLastScannedNetworks(); // Get the last scan results
      auto currentSavedNetworks = _settingsManager->getSavedNetworks(); // Get the *current* saved networks (after deletion)
      std::set<std::string> addedSsids; // To avoid duplicate entries if a network is both scanned and saved

      // 1. Add all networks from the last scanned list
      for (const auto& netMgrData : lastScannedNetworks) {
          ListItem uiItem;
          uiItem.columns.push_back(ColumnData(netMgrData.ssid));
          std::string encryptionChar =
              (netMgrData.encryptionType == WIFI_AUTH_OPEN) ? _languageManager->getString("ICON_LOCK_OPEN", "\u00CB") : _languageManager->getString("ICON_LOCK_CLOSED", "\u00CA");
          uiItem.columns.push_back(ColumnData(encryptionChar));
          char signalStrengthChar = _mapRssiToIcon(netMgrData.rssi);
          uiItem.columns.push_back(ColumnData(std::string(1, signalStrengthChar)));

          std::string deleteActionText = "";
          // Check if this scanned network has a currently saved password (after deletion)
          for (const auto& savedNet : currentSavedNetworks) {
              if (savedNet.ssid == netMgrData.ssid && !savedNet.password.empty()) {
                  deleteActionText = _languageManager->getString("TEXT_DELETE_ACTION", "X");
                  break;
              }
          }
          uiItem.columns.push_back(ColumnData(deleteActionText));
          uiListItems.push_back(uiItem);
          addedSsids.insert(netMgrData.ssid); // Mark this SSID as added
      }

      // 2. Add any saved networks that were *not* present in the last scanned list (e.g., out of range)
      //    This is important to ensure saved but currently unseen networks also show up.
      for (const auto& savedNet : currentSavedNetworks) {
          if (addedSsids.find(savedNet.ssid) == addedSsids.end()) { // If not already added from scanned list
              ListItem uiItem;
              uiItem.columns.push_back(ColumnData(savedNet.ssid));
              uiItem.columns.push_back(ColumnData(_languageManager->getString("ICON_LOCK_CLOSED", "\u00CA")));
              uiItem.columns.push_back(ColumnData(" ")); // No RSSI for unseen saved networks
              // Only show 'X' for saved networks if they actually have a password
              uiItem.columns.push_back(ColumnData(savedNet.password.empty() ? "" : _languageManager->getString("TEXT_DELETE_ACTION", "X")));
              uiListItems.push_back(uiItem);
              // No need to insert into addedSsids here, as this is the last step for list building for this function.
          }
      }

      _networkList.setItems(uiListItems); // Update the UI list immediately with the new items

      _regenerateListItemsAfterAction(); // Then start a new scan, which will refresh again later
    } else {
      _statusText.setText(_languageManager->getString("STATUS_DELETION_FAILED", "Deletion failed."));
    }
  }
}

/**
 * @brief Callback function triggered when the "No" button in the confirmation dialog is pressed.
 * Dismisses the confirmation dialog.
 */
void WifiUI::_onConfirmNo() {
  if (!_screenManager) { // Null pointer check
      DEBUG_ERROR_PRINTLN("WifiUI: ScreenManager pointer is null. Cannot dismiss dialog.");
      return;
  }
  _screenManager->popLayer(); // Close dialog layer
  _ssidToForget = "";         // Clear pending SSID to forget
}

/**
 * @brief Displays a confirmation dialog to the user.
 * @param ssid The SSID of the network in question.
 * @param displayName The display name of the network for the dialog.
 */
void WifiUI::_showConfirmationDialog(const std::string& ssid,
                                     const std::string& displayName) {
  if (!_screenManager || !_lcd || !_dialogSsid.getFont() || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot show confirmation dialog.");
      return;
  }
  // Prevent opening dialog if it's already active
  if (_screenManager->getTopLayerName() == "confirmation_dialog_layer") {
    return;
  }
  _ssidToForget = ssid;
  std::string finalDisplayString = displayName;

  const lgfx::IFont* ssidFont = _dialogSsid.getFont();
  if (ssidFont) { // Null pointer check
    _lcd->setFont(ssidFont);
    int availablePixelWidth =
      _dialogSsid.getWidth() - 2 * _dialogSsid.getPadding();
    // Shorten SSID if it's too long to fit in the dialog box
    if (_lcd->textWidth(displayName.c_str()) > availablePixelWidth) {
      std::string ellipsis = "...";
      int ellipsisWidth = _lcd->textWidth(ellipsis.c_str());
      int maxTextWidth = availablePixelWidth - ellipsisWidth;
      std::string shortenedText = "";
      for (int i = 0; i < displayName.length(); ++i) {
        std::string currentSub = displayName.substr(0, i + 1);
        if (_lcd->textWidth(currentSub.c_str()) <= maxTextWidth) {
          shortenedText = currentSub;
        } else {
          break;
        }
      }
      finalDisplayString = shortenedText + ellipsis;
    }
  }
  _dialogSsid.setText(finalDisplayString);
  _dialogQuestion.setText(_languageManager->getString("WIFI_DELETE_DIALOG_QUESTION", "Are you sure you want to delete password for?"));
  _screenManager->pushLayer("confirmation_dialog_layer");
}

/**
 * @brief Regenerates the list of Wi-Fi networks, typically after an action like forgetting a password.
 * This method initiates a new scan to ensure the UI list reflects the latest network and saved states.
 * If Wi-Fi is not enabled, it clears the list.
 */
void WifiUI::_regenerateListItemsAfterAction() {
  if (!_wifiManager || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: WifiManager or LanguageManager pointer is null. Cannot regenerate list after action.");
      return;
  }
  if (_wifiManager->isWifiLogicEnabled()) {
    // Initiate a new scan, and after completion, attempt to auto-connect to the strongest saved network.
    // This ensures that after forgetting a network, the device tries to connect to another available saved network.
    _wifiManager->startScanAndAttemptAutoConnect();
    _statusText.setText(_languageManager->getString("STATUS_SCANNING", "Scanning networks...")); // Immediate feedback
  } else {
    // If Wi-Fi is disabled, clear the list and update status.
    _networkList.clearItems();
    _networkList.setSelectedItemIndex(-1, true);
    _statusText.setText(_languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled.") + _languageManager->getString("TEXT_LIST_CLEARED", " List cleared."));
  }
}

// --- Manager Callback Implementations ---
/**
 * @brief Handles changes in the Wi-Fi connection state.
 * Updates the UI status text and toggle state accordingly.
 * @param newState The new `WifiMgr_State_t` of the Wi-Fi manager.
 * @param ssid The SSID of the connected/connecting network (if applicable).
 * @param ip The IP address of the device (if connected).
 */
void WifiUI::_handleWifiStateChange(WifiMgr_State_t newState,
                                    const std::string& ssid,
                                    const std::string& ip) {
  if (!_wifiManager || !_settingsManager || !_screenManager || !_languageManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot handle Wi-Fi state change.");
      return;
  }

  std::string statusMsg = "";
  bool wifiPanelIsActive = true;
    //(_screenManager->getTopLayerName() == "wifi_settings_layer");


  switch (newState) {
    case WifiMgr_State_t::WIFI_MGR_DISABLED:
      statusMsg = _languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled.");
      if (wifiPanelIsActive) {
        _wifiToggle.setState(false, false); // Update toggle state
        _networkList.clearItems();          // Clear network list
      }
      _networkList.setSelectedItemIndex(-1, true); // Deselect all items
      break;

    case WifiMgr_State_t::DISCONNECTED:
      statusMsg = _languageManager->getString("STATUS_ENABLED_DISCONNECTED", "Wi-Fi ON, disconnected.");
      if (wifiPanelIsActive) {
        _wifiToggle.setState(true, false); // Update toggle state
      }
      _networkList.setSelectedItemIndex(-1, true); // Deselect all items
      break;

    case WifiMgr_State_t::ENABLING:
      statusMsg = _languageManager->getString("STATUS_ENABLING", "Enabling Wi-Fi...");
      if (wifiPanelIsActive) {
        _wifiToggle.setState(true, false); // Update toggle state
      }
      _networkList.setSelectedItemIndex(-1, true); // Deselect all items
      break;

    case WifiMgr_State_t::SCANNING:
      statusMsg = _languageManager->getString("STATUS_SCANNING", "Scanning networks...");
      break;

    case WifiMgr_State_t::CONNECTING:
      statusMsg = _languageManager->getString("STATUS_CONNECTING", "Connecting: ") + ssid + "...";
      break;

    case WifiMgr_State_t::CONNECTED:
      statusMsg = _languageManager->getString("STATUS_CONNECTED", "Connected: ") + ssid + _languageManager->getString("STATUS_IP_ADDRESS", " (IP: ") + ip + ")";
      _settingsManager->setLastConnectedSsid(ssid); // UI saves the last connected SSID
      // If a new password was pending save and connection was successful, save it.
      if (_g_isNewPasswordPendingSave && ssid == _g_ssidToConnectAfterScan &&
          !_g_passwordForConnectionAfterScan.empty()) {
        _settingsManager->addOrUpdateSavedNetwork(
          _g_ssidToConnectAfterScan, _g_passwordForConnectionAfterScan);
        if (wifiPanelIsActive) {
          _handleScanComplete(true, _wifiManager->getLastScannedNetworks());
        }
      }
      // Reset pending connection flags
      _g_isNewPasswordPendingSave = false;
      _g_ssidToConnectAfterScan = "";
      _g_passwordForConnectionAfterScan = "";
      // Update selected item in the list if Wi-Fi panel is active and an SSID is provided
      if (wifiPanelIsActive) {
        if (!ssid.empty()) {
          const auto& items = _networkList.getItems();
          int indexToSelectAfterConnect = -1;
          for (int i = 0; i < items.size(); ++i) {
            if (items[i].columns.size() > 0 && items[i].columns[0].text == ssid) {
              indexToSelectAfterConnect = i;
              break;
            }
          }
          _networkList.setSelectedItemIndex(indexToSelectAfterConnect, true);
        } else {
          _networkList.setSelectedItemIndex(-1, true);
        }
      }
      break;

    case WifiMgr_State_t::CONNECTION_FAILED:
    {
      statusMsg = _languageManager->getString("STATUS_CONNECTION_FAILED", "Connection failed: ") + ssid;
      bool wasNewPasswordAttempt =
        (_g_isNewPasswordPendingSave && ssid == _g_ssidToConnectAfterScan);
      bool wasSavedPasswordAttempt =
        (_pendingSavedPasswordAttempt && ssid == _pendingSavedPasswordSsid);

      // Reset pending connection/password flags
      if (ssid == _g_ssidToConnectAfterScan) {
        _g_ssidToConnectAfterScan = "";
        _g_passwordForConnectionAfterScan = "";
      }
      _g_isNewPasswordPendingSave = false;
      
      if (wifiPanelIsActive) {
        _networkList.setSelectedItemIndex(-1, true); // Deselect all items
        // If connection failed with a saved password, prompt for password again
        if (wasSavedPasswordAttempt) {
          _pendingSavedPasswordAttempt = false;
          _ssidForPasswordEntry = ssid;
          _passwordKeyboard.clearText();
          _passwordKeyboard.setTitle(_languageManager->getString("STATUS_PASS_INCORRECT", "Incorrect password: ") + ssid + ":");
          _screenManager->pushLayer("keyboardLayer_wifi_password");
        }
      }
      break;
    }

    case WifiMgr_State_t::DISABLING:
      statusMsg = _languageManager->getString("STATUS_DISABLING", "Disabling Wi-Fi...");
      _networkList.setSelectedItemIndex(-1, true); // Deselect all items
      break;
    default:
      statusMsg = _languageManager->getString("STATUS_UNKNOWN_STATE", "Unknown Wi-Fi state.");
      break;
  }

  // Update status text on the UI if the Wi-Fi panel is active.
  // Special handling for connection failed state if keyboard is currently open.
  if (wifiPanelIsActive) {
    if (newState == WifiMgr_State_t::CONNECTION_FAILED &&
        _screenManager->getTopLayerName() == "keyboardLayer_wifi_password") {
        // Do not overwrite status text if keyboard is open with specific message.
    } else {
      _statusText.setText(statusMsg);
    }
  }
}

/**
 * @brief Handles the completion of a Wi-Fi network scan.
 * Updates the network list with scan results and displays relevant status messages.
 * @param success True if the scan was successful, false otherwise.
 * @param networks A vector of `WifiListItemData` containing details of scanned networks.
 */
void WifiUI::_handleScanComplete(
  bool success, const std::vector<WifiListItemData>& networksFromManager) {

  if (!_settingsManager || !_languageManager || !_wifiManager || !_screenManager) { // Null pointer checks
      DEBUG_ERROR_PRINTLN("WifiUI: One or more essential pointers are null. Cannot handle scan complete.");
      return;
  }

  WifiMgr_State_t currentState = _wifiManager->getCurrentState(); // Get current state for conditional logic.

  std::vector<ListItem> uiListItems;
  auto savedNetworks = _settingsManager->getSavedNetworks(); // Retrieve saved networks

  if (success) {
    for (const auto& netMgrData : networksFromManager) {
      ListItem uiItem;
      uiItem.columns.push_back(ColumnData(netMgrData.ssid));
      // Determine encryption icon
      std::string encryptionChar =
        (netMgrData.encryptionType == WIFI_AUTH_OPEN) ? _languageManager->getString("ICON_LOCK_OPEN", "\u00CB") : _languageManager->getString("ICON_LOCK_CLOSED", "\u00CA");
      uiItem.columns.push_back(ColumnData(encryptionChar));
      // Map RSSI to signal strength icon
      char signalStrengthChar = _mapRssiToIcon(netMgrData.rssi);
      uiItem.columns.push_back(ColumnData(std::string(1, signalStrengthChar)));
      std::string deleteActionText = "";
      // Check if network is saved and has a password (for "X" delete icon)
      for (const auto& savedNet : savedNetworks) {
        if (savedNet.ssid == netMgrData.ssid && !savedNet.password.empty()) {
          deleteActionText = _languageManager->getString("TEXT_DELETE_ACTION", "X");
          break;
        }
      }
      uiItem.columns.push_back(ColumnData(deleteActionText));
      uiListItems.push_back(uiItem);
    }
  } else { // Scan failed
      if (_wifiManager->isWifiLogicEnabled()) {
        for (const auto& savedNet : savedNetworks) {
            ListItem uiItem;
            uiItem.columns.push_back(ColumnData(savedNet.ssid));
            uiItem.columns.push_back(ColumnData(_languageManager->getString("ICON_LOCK_CLOSED", "\u00CA")));
            uiItem.columns.push_back(ColumnData(" ")); // No RSSI, so empty.
            uiItem.columns.push_back(ColumnData(_languageManager->getString("TEXT_DELETE_ACTION", "X"))); // Saved, so 'X'.
            uiListItems.push_back(uiItem);
        }
      }
  }

  _networkList.setItems(uiListItems); // Update UI list - this will likely clear selection internally

  // Update status message based on scan success and results
  if (!success) {
    _statusText.setText(_languageManager->getString("STATUS_SCAN_FAILED", "Network scan failed."));
    // DO NOT clear selection here if we are currently connecting or already connected.
    // The _handleWifiStateChange callback should handle setting the correct selection.
    if (currentState != WifiMgr_State_t::CONNECTING && currentState != WifiMgr_State_t::CONNECTED) {
        _networkList.setSelectedItemIndex(-1, true); // Deselect items only if not connecting/connected
    }
  } else if (networksFromManager.empty() && uiListItems.empty()) { // If no items from scan, nor from saved list.
    _statusText.setText(_wifiManager->isWifiLogicEnabled()
                          ? _languageManager->getString("STATUS_NO_NETWORKS_FOUND", "No networks found.")
                          : _languageManager->getString("STATUS_DISABLED", "Wi-Fi disabled."));
    // Again, do not clear selection here if in connecting/connected state.
    if (currentState != WifiMgr_State_t::CONNECTING && currentState != WifiMgr_State_t::CONNECTED) {
        _networkList.setSelectedItemIndex(-1, true);
    }
  } else {
    // If connected or connecting, do not overwrite connection status message
    if (currentState != WifiMgr_State_t::CONNECTED &&
        currentState != WifiMgr_State_t::CONNECTING) {
      _statusText.setText(std::to_string(uiListItems.size()) +
                          _languageManager->getString("TEXT_NETWORKS_FOUND_COUNT", " networks found."));
    }
  }

  // Handle pending connection after scan (if set)
  if (_g_connectAfterScan_flag && !_g_ssidToConnectAfterScan.empty()) {
    if (!_wifiManager->connectToNetwork(_g_ssidToConnectAfterScan,
                                        _g_passwordForConnectionAfterScan)) {
      // Clear pending connection if connection attempt failed to start
      _g_ssidToConnectAfterScan = "";
      _g_passwordForConnectionAfterScan = "";
    }
    _g_connectAfterScan_flag = false; // Reset flag
  }
}

/**
 * @brief Maps an RSSI (Received Signal Strength Indication) value to a character icon.
 * @param rssi The RSSI value.
 * @return A character representing the signal strength icon.
 */
char WifiUI::_mapRssiToIcon(int32_t rssi) {
  if (rssi >= -55) return 'f'; // Strongest signal
  if (rssi >= -65) return 'e';
  if (rssi >= -75) return 'd';
  if (rssi >= -85) return 'c';
  return 'b'; // Weakest signal
}