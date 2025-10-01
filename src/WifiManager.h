/**
 * @file WifiManager.h
 * @brief Defines the WifiManager class for comprehensive Wi-Fi network management.
 *
 * This file provides the core logic for controlling Wi-Fi functionality,
 * including enabling/disabling the radio, performing network scans, managing
 * connection attempts, and handling state transitions. It integrates with
 * `SettingsManager` for persistent configurations, `BLEManager` for radio
 * coexistence, and provides callbacks for various Wi-Fi events, along with
 * managing a dedicated status bar icon.
 *
 * @version 1.0.1
 * @date 2025-08-27
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
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>           // Required for Arduino Wi-Fi library functions and types
#include <vector>           // Required for std::vector
#include <string>           // Required for std::string
#include <functional>       // Required for std::function for callbacks
#include <cstdint>          // Required for int32_t
#include <limits>           // Required for std::numeric_limits
#include "Config.h"         // Required for default constants (e.g., scan duration)
#include "ListItem.h"       // Required for WifiListItemData struct
#include "BLEManager.h"     // Required for BLEManager to coordinate radio usage

// Forward declarations to avoid circular dependencies and unnecessary includes.
class SettingsManager; // Forward declaration for `SettingsManager`
class IconElement;     // Forward declaration for `IconElement`
struct WifiManagerConfig; // Forward declaration for configuration struct (defined in SystemInitializer.h)

/**
 * @brief Enumerates the various states of the Wi-Fi Manager.
 */
enum class WifiMgr_State_t {
  WIFI_MGR_DISABLED,    ///< Wi-Fi manager is inactive and Wi-Fi radio is off.
  DISCONNECTED,         ///< Wi-Fi radio is on, but not connected to any network.
  ENABLING,             ///< Wi-Fi is in the process of being enabled.
  SCANNING,             ///< Wi-Fi is actively scanning for networks.
  CONNECTING,           ///< Wi-Fi is attempting to connect to a specific network.
  CONNECTED,            ///< Wi-Fi is successfully connected to a network.
  CONNECTION_FAILED,    ///< Wi-Fi failed to connect to the target network.
  DISABLING             ///< Wi-Fi is in the process of being disabled.
};

/**
 * @brief Manages comprehensive Wi-Fi network operations.
 *
 * The WifiManager class provides a robust interface for controlling Wi-Fi
 * functionality, including enabling/disabling the radio, scanning for networks,
 * and managing connections. It integrates with the `SettingsManager` for
 * persistent storage of network credentials and coordinates with the
 * `BLEManager` to optimize radio usage when both Wi-Fi and Bluetooth
 * are active. It provides event-driven callbacks for status updates and
 * manages a dedicated UI icon.
 */
class WifiManager {
public:
  // --- Callback Types ---
  /**
   * @brief Callback type for when a Wi-Fi scan completes.
   * @param success True if the scan was successful, false otherwise.
   * @param networks A `const std::vector<WifiListItemData>&` of scanned networks.
   */
  using ScanCompleteCallback = std::function<void(bool success, const std::vector<WifiListItemData>& networks)>;

  /**
   * @brief Callback type for when the Wi-Fi connection state changes.
   * @param newState The new `WifiMgr_State_t` of the manager.
   * @param ssid The SSID of the affected network (empty if not applicable).
   * @param ip The IP address of the device (empty if not connected).
   */
  using ConnectionStateCallback = std::function<void(WifiMgr_State_t newState, const std::string& ssid, const std::string& ip)>;

  /**
   * @brief Callback type for when the RSSI (signal strength) of the connected network changes.
   * @param newRssi The new RSSI value in dBm.
   */
  using RssiChangeCallback = std::function<void(int32_t newRssi)>;

  // --- Constructor ---
  /**
   * @brief Constructor for the WifiManager class.
   * Initializes internal state and sets up pointers to dependency managers.
   * @param settingsManager A pointer to the `SettingsManager` instance.
   * @param bleManager A pointer to the `BLEManager` instance for radio coordination.
   */
  WifiManager(SettingsManager* settingsManager, BLEManager* bleManager);

  // --- Initialization & Lifecycle ---
  /**
   * @brief Initializes the WifiManager with configuration parameters.
   * This method should be called once during system setup. It configures default
   * scan durations, connection timeouts, and initializes Wi-Fi based on saved settings.
   * @param config The `WifiManagerConfig` structure containing configuration parameters.
   */
  void init(const WifiManagerConfig& config);

  /**
   * @brief Enables the Wi-Fi radio and manager logic.
   * Attempts to connect to a saved network automatically if `attemptAutoConnect` is true.
   * @param attemptAutoConnect True to trigger an auto-connect attempt after enabling (default: true).
   */
  void enableWifi(bool attemptAutoConnect = true);

  /**
   * @brief Disables the Wi-Fi radio and manager logic.
   * Disconnects from any active network and turns off the Wi-Fi module.
   */
  void disableWifi();

  /**
   * @brief Main loop method for the WifiManager.
   * This function should be called repeatedly in the Arduino `loop()` function.
   * It monitors Wi-Fi states, handles scan results, manages connection attempts,
   * and coordinates with the BLEManager for radio resource sharing.
   */
  void loop();

  // --- Wi-Fi Network Operations ---
  /**
   * @brief Initiates an asynchronous scan for available Wi-Fi networks.
   * @return True if the scan was successfully initiated (or deferred), false otherwise.
   */
  bool startScan();

  /**
   * @brief Initiates an asynchronous scan and attempts to auto-connect to the strongest saved network.
   * Sets flags to trigger auto-connection logic after scan completion.
   * @return True if the scan was successfully initiated (or deferred), false otherwise.
   */
  bool startScanAndAttemptAutoConnect();

  /**
   * @brief Attempts to connect to a specified Wi-Fi network.
   * @param ssid The SSID of the target Wi-Fi network.
   * @param password The password for the network (empty string for open networks).
   * @return True if the connection attempt was successfully started, false otherwise.
   */
  bool connectToNetwork(const std::string& ssid, const std::string& password = "");

  /**
   * @brief Disconnects from the currently connected Wi-Fi network.
   * Resets internal connection state.
   */
  void disconnectFromNetwork();

  // --- Status Getters ---
  /**
   * @brief Checks if the Wi-Fi manager logic is currently enabled.
   * @return True if Wi-Fi logic is enabled, false if disabled.
   */
  bool isWifiLogicEnabled() const;

  /**
   * @brief Retrieves the current state of the WifiManager.
   * @return The current `WifiMgr_State_t` enum value.
   */
  WifiMgr_State_t getCurrentState() const;

  /**
   * @brief Retrieves the SSID of the currently connected Wi-Fi network.
   * @return The SSID string if connected, an empty string otherwise.
   */
  std::string getConnectedSsid() const;

  /**
   * @brief Retrieves the local IP address of the device if connected.
   * @return The IP address string if connected, an empty string otherwise.
   */
  std::string getIpAddress() const;

  /**
   * @brief Retrieves the RSSI (Received Signal Strength Indicator) of the connected network.
   * @return The RSSI value in dBm if connected, -100 otherwise.
   */
  int8_t getRssi() const;

  // --- UI Integration ---
  /**
   * @brief Sets the `IconElement` instance used to display the Wi-Fi status.
   * The `WifiManager` will update this icon (its character) based on its internal state.
   * @param element A pointer to the `IconElement` instance for the Wi-Fi icon.
   */
  void setWifiIconElement(IconElement* element);

  // --- Callback Registration ---
  /**
   * @brief Sets the callback function to be executed when a Wi-Fi scan completes.
   * @param cb The `ScanCompleteCallback` function.
   */
  void setOnScanCompleteCallback(ScanCompleteCallback cb);

  /**
   * @brief Retrieves the currently set `ScanCompleteCallback`.
   * @return The `ScanCompleteCallback` function.
   */
  ScanCompleteCallback getOnScanCompleteCallback() const;

  /**
   * @brief Sets the callback function to be executed when the Wi-Fi connection state changes.
   * @param cb The `ConnectionStateCallback` function.
   */
  void setOnConnectionStateChangedCallback(ConnectionStateCallback cb);

  /**
   * @brief Sets the callback function to be executed when the RSSI of the connected network changes.
   * @param cb The `RssiChangeCallback` function.
   */
  void setOnRssiChangeCallback(RssiChangeCallback cb);

  /**
   * @brief Sets whether auto-reconnect functionality is enabled.
   * @param enabled True to enable auto-reconnect, false to disable.
   */
  void setAutoReconnectEnabled(bool enabled);

  /**
   * @brief Sets the interval for auto-reconnect attempts.
   * @param intervalMs The new interval in milliseconds.
   */
  void setReconnectInterval(unsigned long intervalMs);   

  /**
   * @brief Checks if auto-reconnect functionality is currently enabled.
   * @return True if auto-reconnect is enabled, false otherwise.
   */
  bool isAutoReconnectEnabled() const { return _autoReconnectEnabled; }

  /**
   * @brief Retrieves the results of the last successful Wi-Fi scan.
   * @return A const reference to a vector of `WifiListItemData` containing scanned networks.
   */
  const std::vector<WifiListItemData>& getLastScannedNetworks() const;

private:
  // --- Dependencies ---
  SettingsManager* _settingsMgr; ///< Pointer to the `SettingsManager` for persistent settings.
  BLEManager* _bleManager;       ///< Pointer to the `BLEManager` for radio coexistence.

  // --- Wi-Fi State Management ---
  WifiMgr_State_t _currentState;     ///< The current operational state of the `WifiManager`.
  WifiMgr_State_t _stateBeforeScan;  ///< Manager state before initiating a scan (for restoration).
  bool _wifiLogicEnabled;            ///< Flag indicating if Wi-Fi manager logic is active.
  
  // --- Scanning ---
  unsigned long _scanStartTime;      ///< Timestamp when the current scan started.
  bool _scanInProgress;              ///< True if a Wi-Fi scan is currently ongoing.
  std::vector<WifiListItemData> _lastScannedNetworks; ///< Cache of the results from the last scan.
  unsigned long _defaultScanDurationSec; ///< Configured default duration for Wi-Fi scans.
  unsigned long _lastScanRetryAttemptTime; ///< Timestamp of the last failed scan attempt (for cooldown).
  const unsigned long SCAN_RETRY_COOLDOWN_MS = 5000; ///< Cooldown period before re-attempting a scan after immediate failure.
  bool _isWifiScanDeferred;          ///< True if a Wi-Fi scan request was deferred due to BLE activity.
  bool _pendingAutoScan;             ///< True if an auto-scan and connect should be attempted in the loop.

  // --- Connection ---
  unsigned long _connectStartTime;   ///< Timestamp when the current connection attempt started.
  unsigned long _connectTimeoutMs;   ///< Configured timeout for connection attempts.
  std::string _targetSsid;           ///< The SSID of the network currently being targeted for connection.
  std::string _targetPassword;       ///< The password for the target network.
  std::string _currentConnectedSsid; ///< The SSID of the currently connected network.
  std::string _currentIpAddress;     ///< The IP address of the device when connected.
  int32_t _lastRssi;                 ///< Last known RSSI of the connected network.
  bool _attemptAutoConnectToStrongest; ///< True if auto-connect to the strongest saved network should be attempted.
  unsigned long _enableStartTime;    ///< Timestamp when Wi-Fi enabling process started.
  bool _connectRequestPending = false; ///< True if a manual connection request is pending (e.g., from UI).

  // Autoreconnect specific variables
  bool _autoReconnectEnabled;                 ///< True if auto-reconnect is enabled, false otherwise.
  unsigned long _lastReconnectAttemptMs;         ///< Timestamp of the last auto-reconnect attempt.
  unsigned long _reconnectIntervalMs; ///< Interval for auto-reconnect attempts.


  // --- UI Elements ---
  IconElement* _wifiIconElement;     ///< Pointer to the `IconElement` in the status bar for Wi-Fi status.

  // --- Callbacks ---
  ScanCompleteCallback _onScanCompleteCb;           ///< Registered callback for scan completion.
  ConnectionStateCallback _onConnectionStateChangedCb; ///< Registered callback for connection state changes.
  RssiChangeCallback _onRssiChangeCb;               ///< Registered callback for RSSI changes.

  // --- Private Helper Methods ---
  /**
   * @brief Changes the internal state of the `WifiManager`.
   * Updates the `_currentState` and triggers the `_onConnectionStateChangedCb` if registered.
   * Also updates the Wi-Fi status icon.
   * @param newState The new `WifiMgr_State_t` to transition to.
   */
  void changeState(WifiMgr_State_t newState);

  /**
   * @brief Internal method to initiate a Wi-Fi scan.
   * Handles immediate failures, deferrals due to BLE, and updates internal scan flags.
   * @param durationSec The duration of the scan in seconds.
   * @return True if scan was initiated (or deferred), false if it couldn't be started.
   */
  bool _startScanInternal(uint32_t durationSec);

  /**
   * @brief Processes the results obtained after a Wi-Fi scan completes.
   * Populates `_lastScannedNetworks` and triggers `_onScanCompleteCb`.
   * @param n The number of networks found.
   */
  void processScanResults(int n);

  /**
   * @brief Checks the current Wi-Fi connection status and updates internal state.
   * Handles successful connections, connection failures, and timeouts.
   */
  void checkConnectionStatus();

  /**
   * @brief Attempts to auto-connect to the strongest available network from the saved list.
   * Iterates through scanned networks, finds the best match from saved credentials, and attempts connection.
   */
  void tryAutoConnectToStrongest();

  /**
   * @brief Updates the Wi-Fi status icon in the status bar based on `_currentState` and RSSI.
   */
  void updateWifiIcon();
};

#endif // WIFIMANAGER_H