/**
 * @file BLEUI.cpp
 * @brief Implementation of the BLEUI class, managing the Bluetooth Low Energy user interface.
 *
 * This file contains the logic for the BLE settings panel, including initialization
 * of UI elements, handling user interactions (toggles, buttons, list selections),
 * and updating the display based on BLEManager events and language changes.
 *
 * @version 1.0.8
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
 * This product incorporates software components licensed under various open-source licenses,
 * including the LovyanGFX library and FreeRTOS components (indirectly via BLEManager).
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#include "Config.h"
#include "BLEUI.h"
#include "BLEManager.h"
#include "StatusbarUI.h"
#include <Arduino.h>
#include <algorithm>

// --- Layout Constants ---
/** @brief Margin size used for UI elements. */
static const int MARGIN = 10;
/** @brief Y-coordinate for the top bar elements. */
static const int TOPBAR_Y = 5;
/** @brief Height of the top bar elements. */
static const int TOPBAR_H = 30;
/** @brief Y-coordinate for the status text. */
static const int STATUS_Y = TOPBAR_Y + TOPBAR_H + 8;
/** @brief Height of the status text area. */
static const int STATUS_H = 20;
/** @brief Y-coordinate for the device list. */
static const int LIST_Y = STATUS_Y + STATUS_H + 8;

// --- Constructors ---
/**
 * @brief Constructor for the BLEUI class.
 * Initializes UI elements and registers callbacks with `BLEManager` and `LanguageManager`.
 * @param lcd Pointer to the LGFX display object.
 * @param screenManager Pointer to the ScreenManager for UI layer management.
 * @param btManager Pointer to the BLEManager for Bluetooth logic.
 * @param statusbar Pointer to the StatusbarUI for status updates.
 * @param languageManager Pointer to the LanguageManager for internationalization.
 * @param settingsManager Pointer to the SettingsManager for persistent settings.
 */
BLEUI::BLEUI(LGFX* lcd,
             ScreenManager* screenManager,
             BLEManager* btManager,
             StatusbarUI* statusbar,
             LanguageManager* languageManager,
             SettingsManager* settingsManager)
  : _lcd(lcd),
    _screenManager(screenManager),
    _languageManager(languageManager),
    _btManager(btManager),
    _statusbarPtr(statusbar),
    _settingsManager(settingsManager), // Inicializáljuk a tagváltozót
    _backBtn(lcd, "", 0, 0, 0, 0, &iconic_all2x),
    _btToggle(lcd, "", "", 0, 0, 0, 0, &helvB12, false),
    _scanBtn(lcd, "", 0, 0, 0, 0, &helvB12),
    _nameBtn(lcd, "", 0, 0, 0, 0, &helvB12),
    _titleText(lcd, "", 0, 0),
    _statusText(lcd, "", 0, 0),
    _deviceList(lcd, 0, 0, 0, 0, 1),
    _pinKeyboard(lcd, "", KEYBOARD_DEFAULT_KEY_WIDTH_PIXELS, KEYBOARD_DEFAULT_KEY_HEIGHT_PIXELS, 0, 0, KEYBOARD_DEFAULT_TEXT_BOX_HEIGHT_PIXELS),
    _nameKeyboard(lcd, "", KEYBOARD_DEFAULT_KEY_WIDTH_PIXELS, KEYBOARD_DEFAULT_KEY_HEIGHT_PIXELS, 0, 0, KEYBOARD_DEFAULT_TEXT_BOX_HEIGHT_PIXELS),
    _confirmBackground(lcd, "", 0, 0),
    _confirmQuestion(lcd, "", 0, 0),
    _confirmDeviceText(lcd, "", 0, 0),
    _confirmNoBtn(lcd, "", 0, 0, 0, 0, &helvB18),
    _confirmYesBtn(lcd, "", 0, 0, 0, 0, &helvB18),
    _primaryConnectIdForAction(""),
    _nameForAction(""),
    _lastConnectedAddress(""),
    _lastConnectedName(""),
    _lastConnectedServiceUUID("") {
  // Callback subscriptions from BLEManager
  _btManager->setOnScanCompleteCallback(
    [this](bool success,
           const std::vector<ManagedBLEDevice>& devices) {
      handleScanComplete(success, devices);
    });
  _btManager->setOnStateChangeCallback(
    [this](BLEMgr_State_t state, const std::string& addr, const std::string& name, const std::string& serviceUUID) {
      handleStateChange(state, addr, name, serviceUUID);
    });
  // Register the new callback for paired device changes here in BLEUI's constructor
  _btManager->setOnPairedDeviceChangedCallback(
    [this](const PairedDevice& device, bool added) {
      this->handlePairedDeviceChanged(device, added);
    });

  if (_languageManager) {
    _languageManager->registerForUpdate("BLEUI", [this]() {
      this->_retranslateUI();
    });
  }
}

// --- Public Methods ---
/**
 * @brief Initializes the BLE UI panel.
 * Defines UI layers, positions and sizes elements, sets up callbacks, and retranslates the UI.
 */
void BLEUI::init() {
  DEBUG_INFO_PRINTLN("BLE UI: init()");
  _screenManager->defineLayer("bt_settings_layer",
                              UILayer(_lcd,
                                      false,
                                      true,
                                      PANEL_BACKGROUND_COLOR,
                                      OrientationPreference::CONTENT_LANDSCAPE));
  auto layer = _screenManager->getLayer("bt_settings_layer");
  if (!layer) {
    DEBUG_ERROR_PRINTLN("BLE UI: Failed to create layer.");
    return;
  }
  int layerW = TFT_HEIGHT;
  int layerH = TFT_WIDTH - STATUSBAR_HEIGHT;

  // Back Button
  _backBtn.setPosition(MARGIN, TOPBAR_Y);
  _backBtn.setSize(50, TOPBAR_H);
  _backBtn.setFont(&iconic_all2x);
  _backBtn.setOnReleaseCallback([this]() {
    closePanel();
  });
  layer->addElement(&_backBtn);

  int xName = layerW - MARGIN - 60;
  int xScan = xName - MARGIN - 80;
  int xToggle = xScan - MARGIN - 60;

  // Bluetooth Toggle Button
  _btToggle.setPosition(xToggle, TOPBAR_Y);
  _btToggle.setSize(60, TOPBAR_H);
  _btToggle.setFont(&helvB12);
  _btToggle.setOnToggleCallback([this](bool s) {
    onToggleChanged(s);
  });
  layer->addElement(&_btToggle);

  // Scan Button
  _scanBtn.setPosition(xScan, TOPBAR_Y);
  _scanBtn.setSize(80, TOPBAR_H);
  _scanBtn.setFont(&helvB12);
  _scanBtn.setOnReleaseCallback([this]() {
    onScanPressed();
  });
  layer->addElement(&_scanBtn);

  // Name Button
  _nameBtn.setPosition(xName, TOPBAR_Y);
  _nameBtn.setSize(60, TOPBAR_H);
  _nameBtn.setFont(&helvB12);
  _nameBtn.setOnReleaseCallback([this]() {
    onNamePressed();
  });
  layer->addElement(&_nameBtn);

  // Title Text
  int titleX = _backBtn.getX() + _backBtn.getWidth() + MARGIN;
  int titleW = xToggle - MARGIN - titleX;
  _titleText.setPosition(titleX, TOPBAR_Y);
  _titleText.setSize(titleW > 10 ? titleW : 10, TOPBAR_H);
  _titleText.setFont(&helvB18);
  _titleText.setTextColor(UI_COLOR_TEXT_DEFAULT);
  _titleText.setBackgroundColor(PANEL_BACKGROUND_COLOR);
  _titleText.setTextDatum(MC_DATUM);
  layer->addElement(&_titleText);

  // Status Text
  _statusText.setPosition(MARGIN, STATUS_Y);
  _statusText.setSize(layerW - 2 * MARGIN, STATUS_H);
  _statusText.setFont(&helvB12);
  _statusText.setTextColor(UI_COLOR_TEXT_DEFAULT);
  _statusText.setBackgroundColor(PANEL_BACKGROUND_COLOR);
  _statusText.setTextDatum(TC_DATUM);
  layer->addElement(&_statusText);

  // Device List
  int listH = layerH - LIST_Y - MARGIN - 6;
  _deviceList.setPosition(MARGIN, LIST_Y);
  _deviceList.setSize(layerW - 2 * MARGIN, listH);
  _deviceList.setItemHeight(35);
  _deviceList.setDrawBorder(true);
  _deviceList.setDrawDividers(true);
  _deviceList.setDrawScrollBar(true);
  _deviceList.setNumColumns(4);

  const int actionColW = 35; // Column for "X" (delete) or similar actions
  const int statusColW = 35; // Column for connection status icon (chain)
  const int macColW = 130;   // Column for MAC address
  const int BORDER_THICKNESS = 1;
  const int scrollBarWidthToConsider = _deviceList.getDrawScrollBar() ? LISTUI_SCROLL_BAR_WIDTH_PIXELS : 0;
  const int totalBorderWidthContribution = _deviceList.getDrawBorder() ? (2 * BORDER_THICKNESS) : 0;

  int availableWidthForColumns = _deviceList.getWidth() - scrollBarWidthToConsider - totalBorderWidthContribution;
  int nameColW = availableWidthForColumns - macColW - statusColW - actionColW;

  if (nameColW < 50) nameColW = 50; // Ensure a minimum width for the name column

  _deviceList.setColumnWidth(0, nameColW);
  _deviceList.setColumnWidth(1, macColW);
  _deviceList.setColumnWidth(2, statusColW);
  _deviceList.setColumnWidth(3, actionColW);

  _deviceList.setColumnDefaultAlignment(0, ML_DATUM); // Name (left-aligned)
  _deviceList.setColumnDefaultAlignment(1, MC_DATUM); // MAC address (center-aligned)
  _deviceList.setColumnDefaultAlignment(2, MC_DATUM); // Status icon (center-aligned)
  _deviceList.setColumnDefaultAlignment(3, MC_DATUM); // Action (center-aligned)

  _deviceList.setColumnDefaultFont(0, &helvR14);    // Name font
  _deviceList.setColumnDefaultFont(1, &helvR10);    // MAC address font
  _deviceList.setColumnDefaultFont(2, &iconic_all2x); // Status icon font
  _deviceList.setColumnDefaultFont(3, &helvB14);    // Action font

  _deviceList.setColumnDefaultTextColor(3, LISTUI_ITEM_DELETE_ACTION_COLOR); // Default color for delete action (red)

  _deviceList.setOnItemSelectedCallback(
    [this](int idx, const ListItem& d, int16_t tx) {
      onDeviceSelected(idx, d, tx);
    });
  _deviceList.setBackgroundColor(PANEL_BACKGROUND_COLOR);
  _deviceList.clearItems();
  layer->addElement(&_deviceList);

  // --- Confirmation Dialog Layer ---
  _screenManager->defineLayer(
    "bt_confirm_dialog_layer",
    UILayer(_lcd, false, false, DIALOG_BOX_BACKGROUND_COLOR));
  if (auto dlg = _screenManager->getLayer("bt_confirm_dialog_layer")) {
    int sw = _lcd->width(), sh = _lcd->height() - STATUSBAR_HEIGHT;
    int w = std::min((int)(sw * 0.85), 380);
    int h = std::max((int)(sh * 0.6), 160);
    int x = (sw - w) / 2, y = (sh - h) / 2, p = UI_DEFAULT_MARGIN_PIXELS;;
    _confirmBackground.setPosition(x, y);
    _confirmBackground.setSize(w, h);
    _confirmBackground.setBackgroundColor(DIALOG_BOX_BACKGROUND_COLOR);
    _confirmBackground.setBorder(DIALOG_BOX_BORDER_COLOR, 1,
                                 TextUI_BorderType::SINGLE);
    dlg->addElement(&_confirmBackground);

    int textAreaH = h - 2 * p - 40 - p;
    int qH = textAreaH * 0.45, dH = textAreaH - qH;
    _confirmQuestion.setPosition(x + p, y + p);
    _confirmQuestion.setSize(w - 2 * p, qH);
    _confirmQuestion.setFont(&helvB18);
    _confirmQuestion.setTextColor(DIALOG_TEXT_COLOR);
    _confirmQuestion.setBackgroundColor(TEXTUI_TRANSPARENT);
    _confirmQuestion.setWordWrap(true);
    _confirmQuestion.setTextDatum(MC_DATUM);
    dlg->addElement(&_confirmQuestion);

    _confirmDeviceText.setPosition(x + p, y + p + qH);
    _confirmDeviceText.setSize(w - 2 * p, dH);
    _confirmDeviceText.setFont(&helvR14);
    _confirmDeviceText.setTextColor(DIALOG_TEXT_COLOR);
    _confirmDeviceText.setBackgroundColor(TEXTUI_TRANSPARENT);
    _confirmDeviceText.setTextDatum(MC_DATUM);
    dlg->addElement(&_confirmDeviceText);

    int bh = 40, bw = (w - 3 * p) / 2;
    int yb = y + h - p - bh;
    int xNo = x + p, xYes = xNo + bw + p;
    _confirmNoBtn.setPosition(xNo, yb);
    _confirmNoBtn.setSize(bw, bh);
    _confirmNoBtn.setFont(&helvB18);
    _confirmNoBtn.setOnReleaseCallback([this]() {
      onConfirmNo();
    });
    dlg->addElement(&_confirmNoBtn);
    _confirmYesBtn.setPosition(xYes, yb);
    _confirmYesBtn.setSize(bw, bh);
    _confirmYesBtn.setFont(&helvB18);
    _confirmYesBtn.setOnReleaseCallback([this]() {
      onConfirmYes();
    });
    dlg->addElement(&_confirmYesBtn);
  }

  // --- PIN Keyboard Layer ---
  _screenManager->defineLayer("keyboardLayer_bt_pin",
                              UILayer(_lcd, false, true, TFT_BLACK));
  if (auto kl = _screenManager->getLayer("keyboardLayer_bt_pin")) {
    _pinKeyboard.setOnEnterCallback(
      [this](const std::string& t) {
        onPinEntered(t);
      });
    kl->addElement(&_pinKeyboard);
  }

  // --- Name Keyboard Layer ---
  _screenManager->defineLayer("keyboardLayer_bt_name",
                              UILayer(_lcd, false, true, TFT_BLACK));
  if (auto kl2 = _screenManager->getLayer("keyboardLayer_bt_name")) {
    _nameKeyboard.setOnEnterCallback(
      [this](const std::string& t) {
        onNameEntered(t);
      });
    kl2->addElement(&_nameKeyboard);
  }

  _retranslateUI();
}

/**
 * @brief Retranslates all text elements of the UI based on the current language setting.
 * This method is typically called after language changes or during initialization.
 */
void BLEUI::_retranslateUI() {
  DEBUG_INFO_PRINTLN("BLEUI: _retranslateUI() called.");

  if (_languageManager != nullptr) {
    _backBtn.setLabel(_languageManager->getString("PANEL_BUTTON_BACK", "\u00FA"));
    _btToggle.setLabels(_languageManager->getString("GENERAL_ON", "ON"), _languageManager->getString("GENERAL_OFF", "OFF"));
    _scanBtn.setLabel(_languageManager->getString("SCAN_BUTTON", "Scan"));
    _nameBtn.setLabel(_languageManager->getString("BLE_NAME_BUTTON", "Name"));
    _titleText.setText(_languageManager->getString("BLE_SETTINGS_TITLE", "BT Settings"));

    _pinKeyboard.setTitle(_languageManager->getString("KEYBOARD_PIN_TITLE", "PIN:"));
    _nameKeyboard.setTitle(_languageManager->getString("BLE_NAME_KEYBOARD_TITLE", "Device Name:"));

    _confirmQuestion.setText(_languageManager->getString("BLE_CONFIRM_DISCONNECT_QUESTION", "Are you sure you want to forget this device?"));
    _confirmNoBtn.setLabel(_languageManager->getString("GENERAL_NO", "No"));
    _confirmYesBtn.setLabel(_languageManager->getString("GENERAL_YES", "Yes"));
  } else {
    DEBUG_WARN_PRINTLN("BLEUI: _retranslateUI() - _languageManager is NULL! Using fallback strings.");
    // Fallback labels (should ideally not be reached if LanguageManager is set up correctly)
    _backBtn.setLabel("\u00FA");
    _btToggle.setLabels("ON", "OFF");
    _scanBtn.setLabel("Scan");
    _nameBtn.setLabel("Name");
    _titleText.setText("BT Settings");

    _pinKeyboard.setTitle("PIN:");
    _nameKeyboard.setTitle("Name:");

    _confirmQuestion.setText("Are you sure?");
    _confirmNoBtn.setLabel("No");
    _confirmYesBtn.setLabel("Yes");
  }

  // Request redraw for active layers that might have translated text
  if (_screenManager->getTopLayerName() == "bt_settings_layer") {
    _screenManager->getLayer("bt_settings_layer")->requestFullLayerRedraw();
  }
  if (_screenManager->getTopLayerName() == "bt_confirm_dialog_layer") {
    _screenManager->getLayer("bt_confirm_dialog_layer")->requestFullLayerRedraw();
  }
  if (_screenManager->getTopLayerName() == "keyboardLayer_bt_pin") {
    _screenManager->getLayer("keyboardLayer_bt_pin")->requestFullLayerRedraw();
  }
  if (_screenManager->getTopLayerName() == "keyboardLayer_bt_name") {
    _screenManager->getLayer("keyboardLayer_bt_name")->requestFullLayerRedraw();
  }

  // Update UI elements based on current BLE state and re-render device list
  handleStateChange(_btManager->getCurrentState(), _btManager->getConnectedAddress(), _btManager->getConnectedName(), _btManager->getConnectedServiceUUID());
}

/**
 * @brief Sets the state of the Bluetooth enable/disable toggle button.
 * The toggle state is derived from the BLEManager's current state, not just a simple flag.
 * @param enabled True to set the toggle to ON, false to set it to OFF.
 * @param invokeCallback True to trigger the toggle's callback, false otherwise.
 */
void BLEUI::setToggleState(bool enabled, bool invokeCallback) {
  bool isEnabled = (_btManager->getCurrentState() != BLEMgr_State_t::BLE_DISABLED);
  _btToggle.setState(isEnabled, invokeCallback);
}

/**
 * @brief Opens the BLE settings panel.
 * Handles panel transitions from the status bar if necessary, then pushes the BLE UI layer.
 */
void BLEUI::openPanel() {
  if (_screenManager->getTopLayerName() == "bt_settings_layer") {
    if (_statusbarPtr && _statusbarPtr->hasPanel()) {
      _statusbarPtr->closePanel();
    }
    return;
  }

  if (_statusbarPtr && _statusbarPtr->hasPanel()) {
    if (_statusbarPtr->isPanelOpenOrOpening()) {
      DEBUG_INFO_PRINTLN("BLEUI: Statusbar panel is open, closing and setting callback.");
      _statusbarPtr->setOnPanelFullyClosedCallback([this]() {
        this->proceedToOpenPanel();
      });
      _statusbarPtr->closePanel();
    } else {
      proceedToOpenPanel();
    }
  } else {
    proceedToOpenPanel();
  }
}

/**
 * @brief Proceeds to open the BLE settings panel after any pending transitions.
 * This is an internal helper called by `openPanel` or its callbacks.
 * It initializes UI elements based on the current BLE state and starts a scan if enabled.
 */
void BLEUI::proceedToOpenPanel() {
  DEBUG_INFO_PRINTLN("BLEUI: proceedToOpenPanel() executed (opening panel now).");

  bool isCurrentlyEnabled = _btManager->isEnabled();
  setToggleState(isCurrentlyEnabled, false);

  // Initialize status text
  if (_languageManager) {
    if (isCurrentlyEnabled) {
      if (_btManager->getCurrentState() == BLEMgr_State_t::BLE_CONNECTED) {
        _statusText.setText(_languageManager->getString("BLE_STATUS_CONNECTED", "Connected") + ": " + _btManager->getConnectedName()); // Using name for UI
      } else {
        _statusText.setText(_languageManager->getString("GENERAL_ON", "ON") + ", " + _languageManager->getString("BLE_STATUS_DISCONNECTED", "not connected") + ".");
      }
    } else {
      _statusText.setText(_languageManager->getString("BLE_STATUS_DISABLED", "Bluetooth disabled."));
      _deviceList.clearItems();
    }
  } else {
    DEBUG_WARN_PRINTLN("BLEUI: proceedToOpenPanel() - _languageManager NULL. Using fallback status strings.");
    if (isCurrentlyEnabled) {
      if (_btManager->getCurrentState() == BLEMgr_State_t::BLE_CONNECTED) {
        _statusText.setText("Connected: " + _btManager->getConnectedName());
      } else {
        _statusText.setText("ON, not connected.");
      }
    } else {
      _statusText.setText("Bluetooth disabled.");
      _deviceList.clearItems();
    }
  }


  _screenManager->pushLayer("bt_settings_layer");

  if (isCurrentlyEnabled) {
    DEBUG_INFO_PRINTLN("BLEUI: BT enabled, starting scan when panel opens.");
    // autoConnect parameter set to TRUE.
    // This ensures that an auto-connect attempt is made after a manual scan as well,
    // if there are paired devices in the manager's list.
    if (!_btManager->startScan(DEFAULT_BLE_SCAN_DURATION_SEC, false, true)) { // <-- autoConnect = true
      if (_languageManager) {
        _statusText.setText(_languageManager->getString("BLE_STATUS_SCAN_NOT_POSSIBLE", "Scan cannot be started (already running?)."));
      } else {
        _statusText.setText("Scan cannot be started (already running?).");
      }
    }
  }
}

/**
 * @brief Closes the BLE settings panel.
 * Pops the BLE UI layer from the screen manager stack.
 */
void BLEUI::closePanel() {
  _screenManager->popLayer();
}

// --- Private UI Callback Methods ---
/**
 * @brief Callback function triggered when the Bluetooth enable/disable toggle changes state.
 * Triggers the `BLEManager` to enable or disable Bluetooth accordingly and saves the state.
 * @param newState The new state of the toggle (true for ON, false for OFF).
 */
void BLEUI::onToggleChanged(bool newState) {
  DEBUG_INFO_PRINTF("BLE UI: onToggleChanged to %s\n",
               newState ? (_languageManager ? _languageManager->getString("GENERAL_ON", "ON").c_str() : "ON")
                        : (_languageManager ? _languageManager->getString("GENERAL_OFF", "OFF").c_str() : "OFF"));

  if (!_settingsManager || !_btManager) {
    DEBUG_ERROR_PRINTLN("BLEUI: SettingsManager or BLEManager is null. Cannot change Bluetooth state.");
    return;
  }

  // Save state via SettingsManager (UI is responsible for saving)
  _settingsManager->setBluetoothEnabledLastState(newState);

  // Notify BLEManager of the state change
  if (newState) _btManager->enableBluetooth(true);
  else _btManager->disableBluetooth();
}

/**
 * @brief Callback function invoked when the "Name" button is pressed.
 * Opens a keyboard for changing the device's BLE broadcast name.
 */
void BLEUI::onNamePressed() {
  DEBUG_INFO_PRINTLN("BLE UI: onNamePressed");
  _pendingNewName = true;
  if (_languageManager) {
    _nameKeyboard.setTitle(_languageManager->getString("BLE_NAME_KEYBOARD_TITLE", "Device Name:"));
  } else {
    _nameKeyboard.setTitle("Device Name:");
  }
  _nameKeyboard.clearText();
  _screenManager->pushLayer("keyboardLayer_bt_name");
}

/**
 * @brief Callback function invoked when text is entered in the PIN keyboard.
 * Currently, this feature is not active for BLE.
 * @param text The entered text from the keyboard.
 */
void BLEUI::onPinEntered(const std::string& text) {
  if (text == KEYBOARD_ESCAPE_BUTTON_ACTION_STRING) {
    _screenManager->popLayer();
    DEBUG_INFO_PRINTLN("BLEUI: PIN input canceled (Escape).");
    return;
  }
  DEBUG_INFO_PRINTF("BLEUI: onPinEntered (usually not used for BLE) text: '%s'\n", text.c_str());
  _screenManager->popLayer();
  if (_screenManager->getTopLayerName() == "bt_settings_layer") {
    if (_languageManager) {
      _statusText.setText(_languageManager->getString("BLE_STATUS_PIN_NOT_ACTIVE", "PIN function not active."));
    } else {
      _statusText.setText("PIN function not active.");
    }
  }
}

/**
 * @brief Callback function invoked when text is entered in the Name keyboard.
 * Updates the device's BLE broadcast name if a new name input was pending.
 * @param text The entered text from the keyboard.
 */
void BLEUI::onNameEntered(const std::string& text) {
  if (text == KEYBOARD_ESCAPE_BUTTON_ACTION_STRING) {
    _pendingNewName = false;
    _screenManager->popLayer();
    return;
  }
  if (_pendingNewName) {
    DEBUG_INFO_PRINTF("BLE UI: onNameEntered '%s'\n", text.c_str());
    _btManager->getSettingsManager()->setDeviceName(text);
    if (_languageManager) {
      _statusText.setText(_languageManager->getString("BLE_STATUS_DEVICE_NAME_SET", "Device name: ") + text);
    } else {
      _statusText.setText("Device name: " + text);
    }
    _pendingNewName = false;
    _screenManager->popLayer();
  }
}

/**
 * @brief Callback function invoked when a device in the list is selected (clicked).
 * Handles connection/disconnection or removal actions based on the clicked column.
 * @param index The index of the selected item in the list.
 * @param data The `ListItem` object representing the selected device.
 * @param touchX_inItem The X coordinate of the touch event relative to the item's left edge.
 */
void BLEUI::onDeviceSelected(
  int index,
  const ListItem& data,
  int16_t touchX) {
  if (data.columns.size() < 4) return;

  int clickedColumnIndex = _deviceList.getClickedColumnIndex(touchX);
  const std::string& deviceName = data.columns[0].text;
  const std::string& devicePrimaryConnectId = data.primaryConnectId; // The primary ID (MAC or UUID)

  // --- Deletion ("X") logic ---
  // Only removable if the column contains "X", meaning it's paired.
  if (clickedColumnIndex == 3 && data.isPaired) {
    DEBUG_INFO_PRINTF("BLEUI: Delete button clicked for: %s\n", deviceName.c_str());
    showConfirmDialog(devicePrimaryConnectId, deviceName);
    return;
  }

  // --- Restrict interactions for offline devices ---
  if (!data.isOnline) {
    if (_languageManager) {
      _statusText.setText(_languageManager->getString("BLE_STATUS_OFFLINE_DEVICE", "This device is not nearby."));
    } else {
      _statusText.setText("This device is not nearby.");
    }
    _deviceList.setSelectedItemIndex(-1, true); // Clear selection for offline device clicks
    return;
  }

  // --- Online device interactions ---
  // Column 2 (index 2): Connection management ("chain" icon) OR any other column (0 or 1) click
  if (clickedColumnIndex == 2 || clickedColumnIndex == 0 || clickedColumnIndex == 1) {
    DEBUG_INFO_PRINTF("BLEUI: Connect/Disconnect action for: %s (Address: %s, Primary ID: %s)\n", deviceName.c_str(), data.address.c_str(), devicePrimaryConnectId.c_str());

    bool isConnectedToThisDevice = (_btManager->getCurrentState() == BLEMgr_State_t::BLE_CONNECTED && _btManager->getConnectedAddress() == data.address);

    if (isConnectedToThisDevice) {
      // Device is already connected, do nothing but inform the user.
      if (_languageManager) {
        _statusText.setText(_languageManager->getString("BLE_STATUS_ALREADY_CONNECTED", "Already connected to this device."));
      } else {
        _statusText.setText("Already connected to this device.");
      }
      return;
    } else {
      if (_languageManager) {
        _statusText.setText(_languageManager->getString("BLE_STATUS_CONNECTING", "Connecting") + ": " + deviceName + "...");
      } else {
        _statusText.setText("Connecting: " + deviceName + "...");
      }
      _btManager->connectToDevice(devicePrimaryConnectId);
    }
    return;
  }
}

/**
 * @brief Handles the completion of a BLE scan.
 * Updates the device list display based on the scan results,
 * including device names, MAC addresses, connection status icons,
 * delete buttons for paired devices, and online/offline coloring.
 * @param success True if the scan was successful, false otherwise.
 * @param scannedDevices A vector of `ManagedBLEDevice` objects found during the scan.
 */
void BLEUI::handleScanComplete(
  bool success,
  const std::vector<ManagedBLEDevice>& scannedDevices) {
  DEBUG_INFO_PRINTF("BLE UI: handleScanComplete START. Success: %d, Devices: %d\n", success, (int)scannedDevices.size());

  if (!success) {
    if (_languageManager != nullptr) {
      _statusText.setText(_languageManager->getString("BLE_STATUS_SCAN_FAILED", "Scan failed."));
    } else {
      _statusText.setText("Scan failed (no langMgr).");
    }
    return;
  }

  // Get the current connected device's address directly from BLEManager.
  // This ensures we use the most up-to-date connection information for list rendering.
  std::string currentConnectedAddress = _btManager->getConnectedAddress();
  std::string currentConnectedServiceUUID = _btManager->getConnectedServiceUUID();
  BLEMgr_State_t bleMgrState = _btManager->getCurrentState();

  std::vector<ListItem> uiListItems;
  int connectedDeviceIndex = -1; // To store the index of the connected device in the new list.

  for (const auto& dev : scannedDevices) {
    ListItem item;
    // Store primaryConnectId and address in ListItem for later actions.
    item.primaryConnectId = dev.primaryConnectId;
    item.address = dev.address;
    item.isPaired = dev.isPaired;
    item.isOnline = dev.isOnline;

    // Populate columns:
    // Column 0: Name
    // Column 1: MAC Address
    if (_languageManager != nullptr) {
      item.columns.push_back(ColumnData(dev.name.empty() ? _languageManager->getString("BLE_DEVICE_NO_NAME", "<no name>") : dev.name));
      item.columns.push_back(ColumnData(dev.address));
    } else {
      item.columns.push_back(ColumnData(dev.name.empty() ? "<no name>" : dev.name));
      item.columns.push_back(ColumnData(dev.address));
    }

    // Column 2: Connection Icon
    // Set the "connected" icon if this device is *the* currently connected one
    // AND the BLEManager's state is actually CONNECTED.
    bool isActuallyConnectedToThisDevice = (bleMgrState == BLEMgr_State_t::BLE_CONNECTED &&
                                           (dev.address == currentConnectedAddress || dev.primaryConnectId == currentConnectedServiceUUID));

    if (isActuallyConnectedToThisDevice) {
      item.columns.push_back(ColumnData((_languageManager != nullptr) ? _languageManager->getString("ICON_BLE_CONNECTED", "\u00C6") : "\u00C6"));
    } else {
      item.columns.push_back(ColumnData((_languageManager != nullptr) ? _languageManager->getString("ICON_BLE_DISCONNECTED", "\u00C5") : "L"));
    }

    // Column 3: Delete "X" button
    // Display "X" if the device is paired.
    item.columns.push_back(ColumnData(dev.isPaired ? ((_languageManager != nullptr) ? _languageManager->getString("TEXT_DELETE_ACTION", "X") : "X") : ""));

    // --- Coloring ---
    // Apply grey color if the device is offline; otherwise, use normal color.
    if (dev.isOnline) {
      item.columns[0].textColor = 0; // Normal color (or ColumnData default)
      item.columns[1].textColor = 0;
      item.columns[2].textColor = 0; // Icon also normal color
    } else {
      item.columns[0].textColor = UI_COLOR_BACKGROUND_MEDIUM; // Grey
      item.columns[1].textColor = UI_COLOR_BACKGROUND_MEDIUM;
      item.columns[2].textColor = UI_COLOR_BACKGROUND_MEDIUM; // Icon also grey
    }

    // Set delete "X" color to red if it exists and the device is paired.
    if (dev.isPaired) {
      item.columns[3].textColor = LISTUI_ITEM_DELETE_ACTION_COLOR;
    } else {
      item.columns[3].textColor = 0; // Use default color (often transparent/background-matching)
    }

    uiListItems.push_back(item);

    // If this is the connected device, store its index
    if (isActuallyConnectedToThisDevice) {
        connectedDeviceIndex = uiListItems.size() - 1;
    }
  }

  _deviceList.setItems(uiListItems); // Update the ClickableListUI with the new items.

  // Explicitly set selection for the connected device after setting all items.
  if (connectedDeviceIndex != -1) {
    _deviceList.setSelectedItemIndex(connectedDeviceIndex, true); 
  } else {
    // Clear selection if no device is currently connected or found in the list.
    _deviceList.setSelectedItemIndex(-1, true); 
  }
  
  DEBUG_INFO_PRINTLN("BLE UI: handleScanComplete END.");
}

/**
 * @brief Handles changes in the BLEManager's state.
 * Updates the UI (toggle, status text, device list) to reflect the new state.
 * @param newState The new `BLEMgr_State_t` of the BLE manager.
 * @param address The MAC address of the affected device (empty if not applicable).
 * @param name The name of the affected device (empty if not applicable).
 * @param serviceUUID The primary service UUID of the affected device (empty if not applicable).
 */
void BLEUI::handleStateChange(BLEMgr_State_t newState,
                              const std::string& address,
                              const std::string& name,
                              const std::string& serviceUUID) {
  DEBUG_INFO_PRINTF("BLE UI: handleStateChange, New State: %d, Name: %s, Address: %s, UUID: %s\n", (int)newState, name.c_str(), address.c_str(), serviceUUID.c_str());

  _btToggle.setState(newState != BLEMgr_State_t::BLE_DISABLED, false);

  // Clear selection if not connected
  if (newState != BLEMgr_State_t::BLE_CONNECTED) {
    _deviceList.setSelectedItemIndex(-1, true);
  }

  std::string statusMsg = "";
  if (_languageManager != nullptr) {
    switch (newState) {
      case BLEMgr_State_t::BLE_SCAN_PENDING:
        statusMsg = _languageManager->getString("BLE_STATUS_SCAN_PENDING", "Device scan pending...");
        break;

      case BLEMgr_State_t::BLE_DISABLED:
        statusMsg = _languageManager->getString("BLE_STATUS_DISABLED", "Bluetooth disabled.");
        _deviceList.clearItems();  // <--- EZ TÖRLI A LISTÁT
        _lastConnectedAddress.clear();
        _lastConnectedName.clear();
        _lastConnectedServiceUUID.clear();
        break;
      case BLEMgr_State_t::BLE_DISCONNECTED:
        statusMsg = _languageManager->getString("GENERAL_ON", "ON") + ", " + _languageManager->getString("BLE_STATUS_DISCONNECTED", "not connected") + ".";

        if (_forgetActionState == ForgetActionState::FORGET_ONLINE) {
          DEBUG_INFO_PRINTLN("BLEUI: handleStateChange(DISCONNECTED): Disconnect due to online device deletion.");
          _statusText.setText(_languageManager->getString("BLE_STATUS_DEVICE_DELETED", "Device deleted."));
          _forgetActionState = ForgetActionState::NONE;
          _primaryConnectIdForAction.clear();
          _nameForAction.clear(); // Reset action data
        }
        _lastConnectedAddress.clear();
        _lastConnectedName.clear();
        _lastConnectedServiceUUID.clear();
        break;

      case BLEMgr_State_t::BLE_SCANNING:
        statusMsg = _languageManager->getString("BLE_STATUS_SCANNING", "Scanning in progress...");
        break;

      case BLEMgr_State_t::BLE_CONNECTING:
        statusMsg = _languageManager->getString("BLE_STATUS_CONNECTING", "Connecting") + ": " + name + "...";
        break;

      case BLEMgr_State_t::BLE_CONNECTED:
        statusMsg = _languageManager->getString("BLE_STATUS_CONNECTED", "Connected") + ": " + name;
        _lastConnectedAddress = address;
        _lastConnectedName = name;
        _lastConnectedServiceUUID = serviceUUID;
        break;

      case BLEMgr_State_t::BLE_FAILED:
        statusMsg = _languageManager->getString("BLE_STATUS_CONNECTION_FAILED", "Connection failed") + ": " + name;
        _lastConnectedAddress.clear();
        _lastConnectedName.clear();
        _lastConnectedServiceUUID.clear();
        break;

      default:
        statusMsg = _languageManager->getString("BLE_STATUS_UNKNOWN_STATE", "Unknown state.");
        _lastConnectedAddress.clear();
        _lastConnectedName.clear();
        _lastConnectedServiceUUID.clear();
        break;
    }
  } else {
    // Fallback constant strings if LanguageManager is null
    DEBUG_WARN_PRINTLN("BLEUI: handleStateChange() - _languageManager NULL! Using fallback strings.");
    switch (newState) {
      case BLEMgr_State_t::BLE_SCAN_PENDING: statusMsg = "Scan pending..."; break;
      case BLEMgr_State_t::BLE_DISABLED:
        statusMsg = "BT disabled.";
        _deviceList.clearItems();
        break;
      case BLEMgr_State_t::BLE_DISCONNECTED: statusMsg = "ON, disconnected."; break;
      case BLEMgr_State_t::BLE_SCANNING: statusMsg = "Scanning..."; break;
      case BLEMgr_State_t::BLE_CONNECTING: statusMsg = "Connecting: " + name + "..."; break;
      case BLEMgr_State_t::BLE_CONNECTED: statusMsg = "Connected: " + name; break;
      case BLEMgr_State_t::BLE_FAILED: statusMsg = "Connection failed: " + name; break;
      default: statusMsg = "Unknown state (no langMgr)."; break;
    }
  }
  _statusText.setText(statusMsg);

  // IMPORTANT: Only update the device list if BLE is NOT in 'DISABLED' state.
  // If 'DISABLED', the list has already been cleared above and should remain empty.
  if (newState != BLEMgr_State_t::BLE_DISABLED) {
    handleScanComplete(true, _btManager->getDisplayDevices());
  } else {
    DEBUG_INFO_PRINTLN("BLEUI: handleStateChange - BLE DISABLED, not refreshing device list with handleScanComplete.");
    _deviceList.requestRedraw(); // Ensure a fresh redraw if needed
  }
}

/**
 * @brief Handles changes in a paired BLE device (added or updated).
 * Saves the updated paired device information to the `SettingsManager`.
 * This method is a callback for `BLEManager::onPairedDeviceChangedCallback`.
 * @param device The `PairedDevice` object that was added or updated.
 * @param added True if the device was added, false if it was updated.
 */
void BLEUI::handlePairedDeviceChanged(const PairedDevice& device, bool added) {
    DEBUG_INFO_PRINTF("BLEUI: handlePairedDeviceChanged for '%s' (Address: %s), Added: %s\n", device.name.c_str(), device.address.c_str(), added ? "true" : "false");
    if (!_settingsManager) {
        DEBUG_ERROR_PRINTLN("BLEUI: SettingsManager is null. Cannot save paired device changes.");
        return;
    }
    // The UI is responsible for saving the paired device.
    _settingsManager->addOrUpdatePairedDevice(device.name, device.address, device.serviceUUID);
    DEBUG_INFO_PRINTF("BLEUI: Paired device '%s' saved/updated in SettingsManager.\n", device.name.c_str());
    handleScanComplete(true, _btManager->getDisplayDevices()); // Refresh the UI list.
}

/**
 * @brief Displays a confirmation dialog for a device action (e.g., forgetting a device).
 * Stores the primary connection ID and name of the device for the action.
 * @param primaryConnectId The primary connection ID of the device for which to show confirmation.
 * @param name The name of the device to display in the confirmation dialog.
 */
void BLEUI::showConfirmDialog(const std::string& primaryConnectId, const std::string& name) {
  _primaryConnectIdForAction = primaryConnectId;
  _nameForAction = name;
  _confirmDeviceText.setText(name);
  _screenManager->pushLayer("bt_confirm_dialog_layer");
}

/**
 * @brief Callback function invoked when the "Yes" button in the confirmation dialog is pressed.
 * Proceeds with the pending forget device action, handling online and offline devices differently.
 */
void BLEUI::onConfirmYes() {
  DEBUG_INFO_PRINTF("BLEUI: onConfirmYes, device primaryConnectId (primaryConnectIdForAction): %s\n", _primaryConnectIdForAction.c_str());

  if (!_settingsManager || !_btManager || !_languageManager) {
    DEBUG_ERROR_PRINTLN("BLEUI: SettingsManager, BLEManager or LanguageManager is null. Cannot confirm deletion.");
    return;
  }

  std::string primaryIdToForget = _primaryConnectIdForAction;
  std::string nameToForget = _nameForAction;

  _screenManager->popLayer(); // Close the dialog

  if (primaryIdToForget.empty()) {
    _statusText.setText(_languageManager->getString("BLE_STATUS_DEVICE_DELETED_ERROR", "Error: Deletion failed (empty ID)."));
    _primaryConnectIdForAction.clear();
    _nameForAction.clear(); // Reset action data
    return;
  }

  // Find the device in the _btManager's list for its current online status
  bool isDeviceCurrentlyOnline = false;
  const auto& displayDevices = _btManager->getDisplayDevices();
  for (const auto& dev : displayDevices) {
    if (dev.primaryConnectId == primaryIdToForget) {
      isDeviceCurrentlyOnline = dev.isOnline;
      break;
    }
  }

  // Delete from SettingsManager (UI is responsible for saving)
  if (_settingsManager->removePairedDevice(primaryIdToForget) || _settingsManager->removePairedDeviceByUUID(primaryIdToForget)) {
      if (isDeviceCurrentlyOnline) {
        DEBUG_INFO_PRINTF("BLEUI: onConfirmYes - Deleting online device '%s' (Primary ID: %s). Disconnecting...\n", nameToForget.c_str(), primaryIdToForget.c_str());
        _forgetActionState = ForgetActionState::FORGET_ONLINE;
        _statusText.setText(_languageManager->getString("BLE_STATUS_DELETION_IN_PROGRESS", "Deletion in progress..."));
        _btManager->disconnectFromDevice(); // Disconnect will eventually lead to _updateAllKnownDevices()
      } else {
        // OFFLINE CASE: Device not nearby, can be deleted immediately.
        DEBUG_INFO_PRINTF("BLEUI: onConfirmYes - Deleting offline device '%s' (Primary ID: %s)\n", nameToForget.c_str(), primaryIdToForget.c_str());
        _forgetActionState = ForgetActionState::FORGET_OFFLINE;
        _statusText.setText(_languageManager->getString("BLE_STATUS_DEVICE_DELETED", "Device deleted") + ": " + nameToForget);
        _btManager->removePairedDevice(primaryIdToForget); // Update internal BLEManager list.
        handleScanComplete(true, _btManager->getDisplayDevices()); // Refresh display.
      }
  } else {
    _statusText.setText(_languageManager->getString("BLE_STATUS_DELETION_ERROR", "Deletion error") + ": " + nameToForget);
  }

  _forgetActionState = ForgetActionState::NONE;
  _primaryConnectIdForAction.clear();
  _nameForAction.clear(); // Reset action data
}

/**
 * @brief Callback function invoked when the "No" button in the confirmation dialog is pressed.
 * Cancels the pending forget device action and closes the dialog.
 */
void BLEUI::onConfirmNo() {
  DEBUG_INFO_PRINTLN("BLE UI: onConfirmNo - Deletion canceled.");

  _primaryConnectIdForAction.clear();
  _nameForAction.clear();

  _screenManager->popLayer();
}

/**
 * @brief Callback function invoked when the "Scan" button is pressed.
 * Initiates a new BLE scan if Bluetooth is enabled.
 */
void BLEUI::onScanPressed() {
  DEBUG_INFO_PRINTLN("BLE UI: onScanPressed");
  if (_btManager->isEnabled()) {
    _lastScanDurationRequested = DEFAULT_BLE_SCAN_DURATION_SEC;
    // autoConnect=true is set here to allow auto-connection after a manual scan.
    if (!_btManager->startScan(_lastScanDurationRequested, false, true)) { // autoConnect=true
      if (_languageManager) {
        _statusText.setText(_languageManager->getString("BLE_STATUS_SCAN_NOT_POSSIBLE", "Scan cannot be started."));
      } else {
        _statusText.setText("Scan cannot be started.");
      }
    }
  } else {
    if (_languageManager) {
      _statusText.setText(_languageManager->getString("BLE_STATUS_DISABLED", "Bluetooth disabled."));
    } else {
      _statusText.setText("Bluetooth disabled.");
    }
  }
}