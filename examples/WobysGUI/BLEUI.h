/**
 * @file BLEUI.h
 * @brief User Interface (UI) panel for managing Bluetooth Low Energy (BLE) settings and devices.
 *
 * This class provides the graphical interface for enabling/disabling BLE,
 * initiating scans, displaying found devices, managing paired devices,
 * and handling connection/disconnection requests. It integrates with
 * `BLEManager` for BLE logic, `ScreenManager` for UI layering,
 * `StatusbarUI` for status updates, and `LanguageManager` for localization.
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
 * including the LovyanGFX library and FreeRTOS components (indirectly).
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#ifndef BLEUI_H
#define BLEUI_H

#include "Config.h"
#include <LovyanGFX.hpp>
#include <vector>
#include <string>
#include <functional>
#include "ScreenManager.h"
#include "BLEManager.h"
#include "StatusbarUI.h"
#include "LanguageManager.h"
#include "TextUI.h"
#include "ButtonUI.h"
#include "ToggleButtonUI.h"
#include "ClickableListUI.h"
#include "KeyboardUI.h"
#include "ListItem.h"
#include "SettingsManager.h"

/**
 * @brief User Interface (UI) panel for managing Bluetooth Low Energy (BLE) settings and devices.
 *
 * This class handles the display and interaction logic for the BLE configuration screen.
 * It provides controls for enabling/disabling Bluetooth, scanning for devices,
 * connecting/disconnecting from devices, and managing paired devices.
 */
class BLEUI {
public:
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
  BLEUI(LGFX* lcd,
        ScreenManager* screenManager,
        BLEManager* btManager,
        StatusbarUI* statusbar,
        LanguageManager* languageManager,
        SettingsManager* settingsManager);

  // --- Public Methods ---
  /**
   * @brief Initializes the BLE UI panel.
   * Defines UI layers, positions and sizes elements, sets up callbacks, and retranslates the UI.
   */
  void init();

  /**
   * @brief Opens the BLE settings panel.
   * Handles panel transitions from the status bar if necessary, then pushes the BLE UI layer.
   */
  void openPanel();

  /**
   * @brief Proceeds to open the BLE settings panel after any pending transitions.
   * This is an internal helper called by `openPanel` or its callbacks.
   */
  void proceedToOpenPanel();

  /**
   * @brief Closes the BLE settings panel.
   * Pops the BLE UI layer from the screen manager stack.
   */
  void closePanel();

  /**
   * @brief Handles the completion of a BLE scan.
   * Updates the device list display based on the scan results.
   * @param success True if the scan was successful, false otherwise.
   * @param devices A vector of `ManagedBLEDevice` objects found during the scan.
   */
  void handleScanComplete(bool success,
                          const std::vector<ManagedBLEDevice>& devices);

  /**
   * @brief Handles changes in the BLEManager's state.
   * Updates the UI (toggle, status text, device list) to reflect the new state.
   * @param newState The new `BLEMgr_State_t` of the BLE manager.
   * @param address The MAC address of the affected device (empty if not applicable).
   * @param name The name of the affected device (empty if not applicable).
   * @param serviceUUID The primary service UUID of the affected device (empty if not applicable).
   */
  void handleStateChange(BLEMgr_State_t newState,
                         const std::string& address,
                         const std::string& name,
                         const std::string& serviceUUID);

  /**
   * @brief Callback function invoked when the "Scan" button is pressed.
   * Initiates a new BLE scan if Bluetooth is enabled.
   */
  void onScanPressed();

  /**
   * @brief Sets the state of the Bluetooth enable/disable toggle button.
   * @param enabled True to set the toggle to ON, false to set it to OFF.
   * @param invokeCallback True to trigger the toggle's callback, false otherwise.
   */
  void setToggleState(bool enabled, bool invokeCallback);

  /**
   * @brief Retrieves a reference to the status text UI element.
   * @return Reference to the `_statusText` object.
   */
  TextUI& getStatusText() { return _statusText; }

  /**
   * @brief Handles changes in a paired BLE device (added or updated).
   * Saves the updated paired device information to the `SettingsManager`.
   * This method is a callback for `BLEManager::onPairedDeviceChangedCallback`.
   * @param device The `PairedDevice` object that was added or updated.
   * @param added True if the device was added, false if it was updated.
   */
  void handlePairedDeviceChanged(const PairedDevice& device, bool added);

private:
  // --- Member Variables ---
  // Dependencies
  LGFX*             _lcd;                   ///< Pointer to the LGFX display object.
  ScreenManager*    _screenManager;         ///< Pointer to the ScreenManager for UI layer management.
  LanguageManager*  _languageManager;       ///< Pointer to the LanguageManager for internationalization.
  BLEManager*       _btManager;             ///< Pointer to the BLEManager for Bluetooth logic.
  StatusbarUI*      _statusbarPtr;          ///< Pointer to the StatusbarUI for displaying status.
  SettingsManager*  _settingsManager;       ///< Pointer to the SettingsManager for persistent settings.

  /**
   * @brief Enumerates the state of a "forget device" action.
   */
  enum class ForgetActionState {
        NONE,           ///< No forget device action is in progress.
        FORGET_ONLINE,  ///< An online device is being forgotten (requires disconnect).
        FORGET_OFFLINE  ///< An offline device is being forgotten (can be immediate).
  };

  // UI Elements
  ButtonUI          _backBtn;               ///< Back button to close the panel.
  ToggleButtonUI    _btToggle;              ///< Toggle switch for enabling/disabling Bluetooth.
  ButtonUI          _scanBtn;               ///< Button to initiate a BLE scan.
  ButtonUI          _nameBtn;               ///< Button to change the device's BLE broadcast name.
  TextUI            _titleText;             ///< Title text for the BLE settings panel.
  TextUI            _statusText;            ///< Text display for current BLE status messages.
  ClickableListUI   _deviceList;            ///< List UI to display found and paired BLE devices.

  KeyboardUI        _pinKeyboard;           ///< Keyboard UI for PIN input (if needed).
  KeyboardUI        _nameKeyboard;          ///< Keyboard UI for device name input.

  TextUI            _confirmBackground;     ///< Background for the confirmation dialog.
  TextUI            _confirmQuestion;       ///< Question text in the confirmation dialog.
  TextUI            _confirmDeviceText;     ///< Device name text in the confirmation dialog.
  ButtonUI          _confirmNoBtn;          ///< "No" button in the confirmation dialog.
  ButtonUI          _confirmYesBtn;         ///< "Yes" button in the confirmation dialog.

  // State Variables for UI Logic
  std::string       _primaryConnectIdForAction; ///< Primary identifier for a device involved in a UI action (e.g., forget).
  std::string       _nameForAction;             ///< Name of the device involved in a UI action.
  bool              _pendingNewName = false;    ///< Flag indicating if a new device name input is pending.

  bool _triggerScanAfterNextDisconnect = false; ///< @deprecated Not used in current logic.
  bool _needsScanAfterConfirm = false;          ///< @deprecated Not used in current logic.
  int  _scanDelayCounter = 0;                   ///< @deprecated Not used in current logic.
  uint32_t _lastScanDurationRequested;          ///< Stores the duration of the last requested scan.
  ForgetActionState _forgetActionState = ForgetActionState::NONE; ///< Current state of the "forget device" action.

  // Variables to store last connected device info for display/context.
  std::string _lastConnectedAddress;            ///< MAC address of the last connected device.
  std::string _lastConnectedName;               ///< Name of the last connected device.
  std::string _lastConnectedServiceUUID;        ///< Service UUID of the last connected device.

  // --- Private UI Callback Methods ---
  /**
   * @brief Callback function invoked when the back button is pressed.
   * Closes the BLE settings panel.
   */
  void onBackButtonPressed();

  /**
   * @brief Callback function invoked when the Bluetooth enable/disable toggle changes state.
   * @param newState The new state of the toggle (true for ON, false for OFF).
   */
  void onToggleChanged(bool newState);

  /**
   * @brief Callback function invoked when the "Name" button is pressed.
   * Opens a keyboard for changing the device's BLE broadcast name.
   */
  void onNamePressed();

  /**
   * @brief Callback function invoked when a device in the list is selected (clicked).
   * Handles connection/disconnection or removal actions based on the clicked column.
   * @param index The index of the selected item in the list.
   * @param data The `ListItem` object representing the selected device.
   * @param touchX_inItem The X coordinate of the touch event relative to the item's left edge.
   */
  void onDeviceSelected(int index, const ListItem& data, int16_t touchX_inItem);

  /**
   * @brief Callback function invoked when text is entered in the PIN keyboard.
   * @param text The entered text from the keyboard.
   */
  void onPinEntered(const std::string& text);

  /**
   * @brief Callback function invoked when text is entered in the Name keyboard.
   * Updates the device's BLE broadcast name if a new name is pending.
   * @param text The entered text from the keyboard.
   */
  void onNameEntered(const std::string& text);

  /**
   * @brief Callback function invoked when the "Yes" button in the confirmation dialog is pressed.
   * Proceeds with the pending forget device action.
   */
  void onConfirmYes();

  /**
   * @brief Callback function invoked when the "No" button in the confirmation dialog is pressed.
   * Cancels the pending forget device action.
   */
  void onConfirmNo();

  /**
   * @brief Displays a confirmation dialog for a device action (e.g., forgetting a device).
   * @param primaryConnectId The primary connection ID of the device for which to show confirmation.
   * @param name The name of the device to display in the confirmation dialog.
   */
  void showConfirmDialog(const std::string& primaryConnectId, const std::string& name);

  /**
   * @brief Retranslates all text elements of the UI based on the current language setting.
   * This method is typically called after language changes or during initialization.
   */
  void _retranslateUI();
};

#endif // BLEUI_H