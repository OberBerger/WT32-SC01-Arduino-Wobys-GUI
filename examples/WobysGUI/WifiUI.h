/**
 * @file WifiUI.h
 * @brief Defines the WifiUI class for managing Wi-Fi settings and connections.
 *
 * This file contains the declaration of the WifiUI class, which provides a user interface
 * for enabling/disabling Wi-Fi, scanning for networks, connecting to them (with password
 * entry), and forgetting saved networks. It integrates with `WifiManager`, `SettingsManager`,
 * `StatusbarUI`, and `LanguageManager`.
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
#ifndef WIFIUI_H
#define WIFIUI_H

// Core configuration
#include "Config.h"
// Manager includes
#include "ScreenManager.h"
#include "WifiManager.h"
#include "SettingsManager.h"
#include "StatusbarUI.h"
#include "LanguageManager.h"

// UI elements includes
#include "ButtonUI.h"
#include "TextUI.h"
#include "ToggleButtonUI.h"
#include "ClickableListUI.h"
#include "KeyboardUI.h"

/**
 * @brief Manages the Wi-Fi settings user interface, allowing users to configure Wi-Fi connections.
 *
 * The WifiUI class provides an interactive screen where users can enable/disable Wi-Fi,
 * scan for available networks, connect to secured or open networks (prompting for a password
 * if needed), and forget previously saved network credentials. It communicates with the
 * `WifiManager` for network operations and `SettingsManager` for persistent storage of credentials.
 */
class WifiUI {
private:
  // --- Pointers to External Managers ---
  LGFX* _lcd;                            ///< Pointer to the LGFX display object.
  ScreenManager* _screenManager;         ///< Pointer to the ScreenManager for layer management.
  WifiManager* _wifiManager;             ///< Pointer to the WifiManager for Wi-Fi operations.
  SettingsManager* _settingsManager;     ///< Pointer to the SettingsManager for persistent settings.
  StatusbarUI* _statusbarPtr;           ///< Pointer to the StatusbarUI for status bar interaction.
  LanguageManager* _languageManager;     ///< Pointer to the LanguageManager for UI internationalization.

  // --- UI Elements ---
  ButtonUI _backButton;      ///< Button to navigate back from the Wi-Fi settings screen.
  TextUI _titleText;         ///< Text label for the Wi-Fi settings screen title.
  ToggleButtonUI _wifiToggle; ///< Toggle button to enable/disable Wi-Fi logic.
  ButtonUI _scanButton;      ///< Button to initiate a Wi-Fi network scan.
  TextUI _statusText;        ///< Text label to display current Wi-Fi status messages.
  ClickableListUI _networkList; ///< List to display available and saved Wi-Fi networks.
  KeyboardUI _passwordKeyboard; ///< Virtual keyboard for entering Wi-Fi passwords.

  // --- Confirmation Dialog Elements ---
  TextUI _dialogBackground;   ///< Background panel for the confirmation dialog.
  TextUI _dialogQuestion;     ///< Text label for the question in the dialog.
  TextUI _dialogSsid;         ///< Text label to display the SSID in the dialog.
  ButtonUI _dialogYesButton;  ///< "Yes" button in the confirmation dialog.
  ButtonUI _dialogNoButton;   ///< "No" button in the confirmation dialog.

  // --- Internal State Variables (Previously Global) ---
  std::string _ssidForPasswordEntry;         ///< Stores the SSID for which a password is being entered.
  std::string _ssidToForget;                 ///< Stores the SSID to be forgotten (used in confirmation dialog).
  bool _pendingSavedPasswordAttempt;         ///< Flag: True if an attempt to connect with a saved password is ongoing.
  std::string _pendingSavedPasswordSsid;     ///< Stores the SSID used in a saved password attempt.
  bool _g_connectAfterScan_flag;             ///< Flag: True if a connection attempt should follow a scan.
  std::string _g_ssidToConnectAfterScan;     ///< Stores the SSID for connection after a scan.
  std::string _g_passwordForConnectionAfterScan; ///< Stores the password for connection after a scan.
  bool _g_isNewPasswordPendingSave;          ///< Flag: True if a new password needs to be saved after successful connection.

  // --- Private Methods (Previously Global Callback Functions) ---
  /**
   * @brief Closes the Wi-Fi settings panel by popping its layer from the ScreenManager.
   */
  void _closePanel();

  /**
   * @brief Callback function triggered when the Wi-Fi enable/disable toggle changes state.
   * Enables or disables Wi-Fi logic and saves the new state.
   * @param newState The new state of the Wi-Fi toggle (true for ON, false for OFF).
   */
  void _onToggleChanged(bool newState);

  /**
   * @brief Callback function triggered when the "Scan" button is pressed.
   * Initiates a Wi-Fi network scan if Wi-Fi is enabled.
   */
  void _onScanButtonPressed();

  /**
   * @brief Callback function triggered when a network item in the list is selected.
   * Handles connection attempts, password entry, or "forget network" actions based on user interaction.
   * @param index The index of the selected item in the list.
   * @param data The ListItem data associated with the selected network.
   * @param touchX The X coordinate of the touch event within the list item.
   */
  void _onNetworkSelected(int index, const ListItem& data, int16_t touchX);

  /**
   * @brief Callback function triggered when a password is entered via the keyboard.
   * Handles connection attempts with the entered password or closes the keyboard.
   * @param password The password string entered by the user, or a special escape string.
   */
  void _onPasswordEntered(const std::string& password);

  /**
   * @brief Callback function triggered when the "Yes" button in the confirmation dialog is pressed.
   * Proceeds to forget the selected network's password.
   */
  void _onConfirmYes();

  /**
   * @brief Callback function triggered when the "No" button in the confirmation dialog is pressed.
   * Dismisses the confirmation dialog.
   */
  void _onConfirmNo();

  /**
   * @brief Displays a confirmation dialog to the user.
   * @param ssid The SSID of the network in question.
   * @param displayName The display name of the network for the dialog.
   */
  void _showConfirmationDialog(const std::string& ssid,
                               const std::string& displayName);

  /**
   * @brief Regenerates the list of Wi-Fi networks, typically after an action like forgetting a password.
   */
  void _regenerateListItemsAfterAction();

  /**
   * @brief Retranslates all UI text elements based on the current language setting.
   * This method is called during initialization and whenever the language changes.
   */
  void _retranslateUI();

  // --- Manager Callback Implementations ---
  /**
   * @brief Handles changes in the Wi-Fi connection state.
   * Updates the UI status text and toggle state accordingly.
   * @param newState The new `WifiMgr_State_t` of the Wi-Fi manager.
   * @param ssid The SSID of the connected/connecting network (if applicable).
   * @param ip The IP address of the device (if connected).
   */
  void _handleWifiStateChange(WifiMgr_State_t newState,
                              const std::string& ssid,
                              const std::string& ip);

  /**
   * @brief Handles the completion of a Wi-Fi network scan.
   * Updates the network list with scan results and displays relevant status messages.
   * @param success True if the scan was successful, false otherwise.
   * @param networks A vector of `WifiListItemData` containing details of scanned networks.
   */
  void _handleScanComplete(bool success,
                           const std::vector<WifiListItemData>& networks);

  /**
   * @brief Maps an RSSI (Received Signal Strength Indication) value to a character icon.
   * @param rssi The RSSI value.
   * @return A character representing the signal strength icon.
   */
  static char _mapRssiToIcon(int32_t rssi);

public:
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
  WifiUI(LGFX* lcd,
         ScreenManager* screenManager,
         WifiManager* wifiManager,
         SettingsManager* settingsManager,
         StatusbarUI* statusbar,
         LanguageManager* languageManager);

  /**
   * @brief Initializes the Wi-Fi UI components, sets up their layout, and registers callbacks.
   * This method should be called once after the constructor.
   */
  void init();

  /**
   * @brief Opens the Wi-Fi settings panel.
   * This method handles the transition to the Wi-Fi screen, ensuring proper status bar panel closure
   * if it's open, before pushing the Wi-Fi settings layer to the screen manager.
   */
  void openPanel();

  /**
   * @brief Proceeds to open the Wi-Fi settings panel layer after any prerequisite actions (e.g., status bar panel closure).
   * This method is typically called as a callback after the status bar panel is fully closed.
   */
  void proceedToOpenPanel();
};

#endif // WIFIUI_H