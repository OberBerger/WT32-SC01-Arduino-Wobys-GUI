/**
 * @file BLEManager.h
 * @brief Manages Bluetooth Low Energy (BLE) scanning, connection, and device pairing.
 *
 * This class provides comprehensive control over the ESP32's BLE functionalities,
 * including initiating network scans, managing connections to peripheral devices,
 * and storing/retrieving paired device information. It integrates with the
 * `SettingsManager` for persistent storage and the `ScreenManager` for UI updates,
 * offering callbacks for state changes and scan completion events.
 *
 * @version 1.1.0
 * @date 2025-09-17
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
 * including the Arduino Core for ESP32 and NimBLE (used by BLEDevice).
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#pragma once

#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include "Config.h"           // Project general configurations
#include "SettingsManager.h"  // Manages settings
#include "IconElement.h"      // UI icon element

// Standard C++ and STL includes
#include <vector>
#include <string>
#include <functional>  // For std::function
#include <algorithm>   // For std::sort, std::find
#include <map>         // For std::map

 // ESP32 BLE and FreeRTOS includes
#include <BLEDevice.h>              // BLEDevice, BLEScan
#include <BLEScan.h>                // BLEScan, BLEScanResults
#include <BLEAdvertisedDevice.h>    // BLEAdvertisedDevice
#include <BLEClient.h>              // BLEClient
#include <BLEAddress.h>             // BLEAddress
#include <BLEUtils.h>               // BLEUUID (possibly other utility functions)

#include <freertos/FreeRTOS.h>  // FreeRTOS basics
#include <freertos/semphr.h>    // Semaphores (mutex)
#include <freertos/queue.h>     // Queues (QueueHandle_t)
#include <freertos/task.h>      // Tasks (TaskHandle_t)

#include <atomic>

// Forward declarations to avoid circular dependencies
class ScreenManager;
class BLEManager;
class WifiManager;

/**
 * @brief Plain Old Data (POD) structure for advertised BLE devices.
 * Designed to be trivially copyable for use with FreeRTOS queues.
 */
struct AdvertisedPOD {
  char address[18];        ///< MAC address of the advertised device as a null-terminated string.
  char name[48];           ///< Name of the advertised device as a null-terminated string.
  int16_t rssi;            ///< RSSI (Received Signal Strength Indicator) of the device.
  uint8_t addressType;     ///< Address type (e.g., public, random).
  uint8_t serviceCount;    ///< Number of advertised service UUIDs.
  char serviceUUIDs[3][40];///< Array of advertised service UUIDs as null-terminated strings.
};

/**
 * @brief Data structure for BLE device information.
 * Used for detailed information in scan callbacks and internal management.
 */
struct BLEDeviceData {
  std::string name;                      ///< Name of the BLE device.
  std::string address;                   ///< MAC address of the BLE device.
  int16_t rssi;                          ///< RSSI (Received Signal Strength Indicator).
  uint8_t addressType;                   ///< Address type.
  std::vector<std::string> advertisedServiceUUIDs; ///< List of advertised service UUIDs.
  BLEAdvertisedDevice advertisedDevice;  ///< Original advertised device object (for full data access).
};

/**
 * @brief Represents a managed BLE device, combining scan data and paired status.
 * Used for display in UI lists.
 */
struct ManagedBLEDevice {
  std::string primaryConnectId; ///< Primary identifier for connection (e.g., UUID or MAC address).
  std::string name;             ///< User-friendly name of the device.
  std::string address;          ///< MAC address of the device.
  std::string serviceUUID;      ///< Primary service UUID (if available).
  int16_t rssi;                 ///< Last known RSSI.
  bool isOnline;                ///< True if the device is currently detected as online.
  bool isPaired;                ///< True if the device is paired/saved in settings.
  uint8_t addressType;          ///< Address type.

  /**
   * @brief Default constructor.
   * Initializes all members to default/empty values.
   */
  ManagedBLEDevice() : rssi(0), isOnline(false), isPaired(false), addressType(0) {}

  /**
   * @brief Parameterized constructor.
   * @param pId Primary connection ID.
   * @param n Name of the device.
   * @param addr MAC address.
   * @param uuid Primary service UUID.
   * @param r RSSI value.
   * @param online True if online.
   * @param paired True if paired.
   * @param addrType Address type.
   */
  ManagedBLEDevice(const std::string& pId,
                   const std::string& n,
                   const std::string& addr,
                   const std::string& uuid,
                   int16_t r,
                   bool online,
                   bool paired,
                   uint8_t addrType)
      : primaryConnectId(pId),
        name(n),
        address(addr),
        serviceUUID(uuid),
        rssi(r),
        isOnline(online),
        isPaired(paired),
        addressType(addrType) {}
};

/**
 * @brief Enumerates the various states of the BLE Manager.
 */
enum class BLEMgr_State_t {
  BLE_DISABLED,       ///< BLE manager is inactive and BLE radio is off.
  BLE_DISCONNECTED,   ///< BLE radio is on, but not connected to any device.
  BLE_SCAN_PENDING,   ///< BLE scan is requested but pending (e.g., waiting for UI to be ready).
  BLE_SCANNING,       ///< BLE is actively scanning for devices.
  BLE_CONNECTING,     ///< BLE is attempting to connect to a specific device.
  BLE_CONNECTED,      ///< BLE is successfully connected to a device.
  BLE_FAILED          ///< BLE operation (scan/connect/enable) failed.
};

/**
 * @brief Declares a utility function to sanitize strings, defined in `BLEManager.cpp`.
 * @param input_cstr C-style string to sanitize.
 * @param max_len Maximum length to process from the input string.
 * @return A new string containing only printable ASCII characters.
 */
std::string sanitizeString(const char* input_cstr, size_t max_len);


/**
 * @brief Manages Bluetooth Low Energy (BLE) scanning, connection, and device pairing.
 */
class BLEManager {
 public:
  // --- Public Type Definitions ---
  /**
   * @brief Callback type for when a BLE scan completes.
   * @param success True if the scan was successful, false otherwise.
   * @param devices A constant vector of `ManagedBLEDevice` objects found during the scan.
   */
  using ScanCompleteCb = std::function<void(
      bool success, const std::vector<ManagedBLEDevice>& devices)>;
  /**
   * @brief Callback type for when the BLE connection or manager state changes.
   * @param newState The new `BLEMgr_State_t` of the manager.
   * @param address The MAC address of the affected device (empty if not applicable).
   * @param name The name of the affected device (empty if not applicable).
   * @param serviceUUID The primary service UUID of the affected device (empty if not applicable).
   */
  using StateChangeCb =
      std::function<void(BLEMgr_State_t,
                         const std::string& address,
                         const std::string& name,
                         const std::string& serviceUUID)>;
  /**
   * @brief Callback type executed just before a blocking BLE scan starts.
   * Allows the UI to update its state or perform actions (e.g., disable touch).
   */
  using BeforeScanCb = std::function<void()>;

  /**
   * @brief Callback type for when a paired device is added or updated internally.
   * @param device The `PairedDevice` object that was added or updated.
   * @param added True if the device was added, false if it was updated.
   */
  using PairedDeviceChangedCallback = std::function<void(const PairedDevice& device, bool added)>;


  /**
   * @brief Parameter structure for the `scanWorkerTask` FreeRTOS task.
   */
  struct ScanWorkerParam {
    BLEManager* mgr;    ///< Pointer to the `BLEManager` instance.
    uint32_t duration;  ///< Duration of the scan in seconds.
  };

  /**
   * @brief Parameter structure for the `connectWorkerTask` FreeRTOS task.
   */
  struct ConnectWorkerParam {
    BLEManager* mgr;           ///< Pointer to the `BLEManager` instance.
    BLEAddress address;        ///< Target BLE device address.
    std::string name;          ///< Target device name.
    std::string serviceUUID;   ///< Target device primary service UUID.
    uint8_t addressType;       ///< Target device address type.
    BLEClient* bleClient;      ///< Pointer to the BLEClient instance for the task.
  };

  // --- Constructors & Destructor ---
  /**
   * @brief Constructor for the BLEManager class.
   * @param settingsMgr Pointer to the `SettingsManager` instance for persistent settings.
   * @param screenManager Pointer to the `ScreenManager` instance for UI coordination.
   * @param wifiManager Pointer to the `WifiManager` instance for radio coexistence. 
   */
  BLEManager(SettingsManager* settingsMgr, ScreenManager* screenManager, WifiManager* wifiManager);

  /**
   * @brief Destructor for the BLEManager class.
   * Cleans up allocated resources, including BLEClient, callbacks, and FreeRTOS primitives.
   */
  ~BLEManager();

  // --- Public Interface Methods ---
  // Initialization & Lifecycle
  /**
   * @brief Initializes the BLEManager.
   * Loads saved Bluetooth settings, configures the BLE radio state, and performs
   * an initial update of known devices.
   */
  void init();

  /**
   * @brief Enables the Bluetooth Low Energy (BLE) radio and manager logic.
   * Initializes the BLE stack, sets device name, power level, and registers callbacks.
   * If BLE is already enabled, it ensures callbacks are re-registered.
   * @param autoScan True to initiate an automatic scan for known devices after enabling.
   */
  void enableBluetooth(bool autoScan = true);

  /**
   * @brief Disables the Bluetooth Low Energy (BLE) radio and manager logic.
   * Disconnects from any active connection and deinitializes the BLE stack.
   */
  void disableBluetooth();

  /**
   * @brief Main loop function for the BLEManager.
   * This method should be called repeatedly in the Arduino `loop()` function.
   * It handles state transitions, worker task notifications, and pending requests,
   * such as auto-connection after initialization, pending scans, and disconnect timeouts.
   */
  void loop();

  // Status Getters
  /**
   * @brief Checks if the BLE manager logic is currently enabled.
   * @return True if BLE logic is enabled, false otherwise.
   */
  bool isEnabled() const;

  /**
   * @brief Retrieves the current state of the BLEManager.
   * @return The current `BLEMgr_State_t` enum value.
   */
  BLEMgr_State_t getCurrentState() const;

  /**
   * @brief Retrieves the MAC address of the currently connected BLE device.
   * @return The connected device's MAC address, or an empty string if not connected.
   */
  std::string getConnectedAddress() const;

  /**
   * @brief Retrieves the name of the currently connected BLE device.
   * @return The connected device's name, or an empty string if not connected.
   */
  std::string getConnectedName() const;

  /**
   * @brief Retrieves the primary service UUID of the currently connected BLE device.
   * @return The connected device's service UUID, or an empty string if not connected.
   */
  std::string getConnectedServiceUUID() const;

  /**
   * @brief Retrieves the duration of the current BLE scan in seconds.
   * @return The current scan duration.
   */
  uint32_t getCurrentScanDuration() const;

  /**
   * @brief Retrieves the duration of a pending BLE scan in seconds.
   * @return The pending scan duration.
   */
  uint32_t getPendingScanDuration() const;

  /**
   * @brief Retrieves a pointer to the `SettingsManager` instance.
   * @return Pointer to the `SettingsManager`.
   */
  SettingsManager* getSettingsManager() const;

  // Device Management & Control
  /**
   * @brief Starts an asynchronous BLE scan for available devices.
   * @param durationSec Duration of the scan in seconds (default: `DEFAULT_BLE_SCAN_DURATION_SEC`).
   * @param blocking True to block until the scan completes, false for asynchronous operation (default: false).
   * @param autoConnect True to attempt to auto-connect to the strongest known device after scan (default: false).
   * @return True if the scan was successfully initiated (or deferred), false otherwise.
   */
  bool startScan(uint32_t durationSec = DEFAULT_BLE_SCAN_DURATION_SEC,
                 bool blocking = false,
                 bool autoConnect = false);

  /**
   * @brief Disconnects from the currently connected BLE device.
   * If `requestDeinitOnComplete` is true, the BLE stack will be fully deinitialized after disconnect.
   * @param requestDeinitOnComplete True to deinitialize BLE after disconnect, false for a soft disconnect (default: false).
   */
  void disconnectFromDevice(bool requestDeinitOnComplete);

  /**
   * @brief Disconnects from the currently connected BLE device.
   * This is a convenience overload that does not request deinitialization.
   */
  void disconnectFromDevice();

  /**
   * @brief Initiates a connection to a BLE device using its primary connection ID (MAC or Service UUID).
   * If already connected to a different device, it will first disconnect.
   * @param primaryConnectId The MAC address or the primary Service UUID of the target device.
   * @return True if the connection attempt was initiated, false otherwise.
   */
  bool connectToDevice(const std::string& primaryConnectId);

  /**
   * @brief Retrieves a list of paired BLE devices from the `SettingsManager`.
   * @return A vector of `PairedDevice` objects.
   */
  std::vector<PairedDevice> getPairedDevices() const;

  /**
   * @brief Removes a paired BLE device from internal lists and persistent storage by primary connection ID.
   * Disconnects from the device if it is currently connected.
   * @param primaryConnectId The primary ID (MAC or Service UUID) of the device to remove.
   * @return True if the device was successfully removed, false otherwise.
   */
  bool removePairedDevice(const std::string& primaryConnectId);

  // Notification Methods (called by worker tasks)
  /**
   * @brief Notifies the BLEManager that a scan worker task has finished.
   * This method is called from within the `scanWorkerTask`.
   */
  void notifyScanWorkerFinished();

  /**
   * @brief Retrieves a list of known (scanned or paired) BLE devices, sorted for UI display.
   * @return A vector of `ManagedBLEDevice` objects.
   */
  std::vector<ManagedBLEDevice> getDisplayDevices() const;

  /**
   * @brief Notifies the BLEManager that a connect worker task has finished.
   * This method processes the connection result, updates internal state,
   * saves/updates paired device information if successful, and triggers callbacks.
   * @param success True if the connection attempt was successful, false otherwise.
   * @param address The MAC address of the target device.
   * @param name The name of the target device.
   * @param serviceUUID The primary service UUID of the target device.
   * @param clientFromTask Pointer to the BLEClient instance used by the worker task.
   * @param connectParam Pointer to the ConnectWorkerParam used by the worker task.
   */
  void notifyConnectWorkerFinished(bool success,
                                   const std::string& address,
                                   const std::string& name,
                                   const std::string& serviceUUID,
                                   BLEClient* clientFromTask,
                                   ConnectWorkerParam* connectParam);

  /**
   * @brief Retrieves a pointer to the internal `BLEClient` instance.
   * Used by connection worker tasks.
   * @return Pointer to the `BLEClient` object.
   */
  BLEClient* getBLEClient() { return _pClient; }


  // --- Callback Registration Methods ---
  /**
   * @brief Sets the callback function to be executed when a BLE scan completes.
   * @param cb The `ScanCompleteCb` function.
   */
  void setOnScanCompleteCallback(ScanCompleteCb cb);
  /**
   * @brief Sets the callback function to be executed when the BLE connection or manager state changes.
   * @param cb The `StateChangeCb` function.
   */
  void setOnStateChangeCallback(StateChangeCb cb);
  /**
   * @brief Sets the callback function to be executed just before a blocking BLE scan starts.
   * @param cb The `BeforeScanCb` function.
   */
  void setOnBeforeBlockingScanStartsCallback(BeforeScanCb cb);
  /**
   * @brief Sets the callback function to be executed when a paired device is added or updated.
   * @param cb The `PairedDeviceChangedCallback` function.
   */
  void setOnPairedDeviceChangedCallback(PairedDeviceChangedCallback cb) { _onPairedDeviceChangedCallback = cb; }


  // --- UI Element Setter ---
  /**
   * @brief Sets the `IconElement` instance used to display the Bluetooth status.
   * The `BLEManager` will update this icon (its character) based on its internal state.
   * @param element A pointer to the `IconElement` instance for the Bluetooth icon.
   */
  void setBluetoothIconElement(IconElement* element);

  /**
   * @brief Sets whether auto-reconnect functionality is enabled.
   * @param enabled True to enable auto-reconnect, false to disable.
   */
  void setAutoReconnectEnabled(bool enabled);

  /**
   * @brief Checks if auto-reconnect functionality is currently enabled.
   * @return True if auto-reconnect is enabled, false otherwise.
   */
  bool isAutoReconnectEnabled() const { return _autoReconnectEnabled; }

  /**
   * @brief Sets the interval for auto-reconnect attempts.
   * @param intervalMs The new interval in milliseconds.
   */
  void setReconnectInterval(unsigned long intervalMs);  

 private:
  // --- Private Type Definitions ---
  /**
   * @brief Enumerates the purpose of a BLE scan.
   */
  enum class ScanPurpose { General, ShortConnect };

  /**
   * @brief Stores criteria for a pending connection attempt.
   */
  struct PendingConnectCriteria {
    std::string address;     ///< MAC address of the target device.
    std::string name;        ///< Name of the target device.
    std::string serviceUUID; ///< Primary service UUID.
    uint8_t addressType;     ///< Address type.
  };

  /**
   * @brief Callback handler for BLE client events (connect, disconnect).
   */
  class MyClientCallbacks : public BLEClientCallbacks {
    BLEManager* _pManagerInstance; ///< Pointer to the owning `BLEManager` instance.
   public:
    /**
     * @brief Constructor for MyClientCallbacks.
     * @param manager Pointer to the owning `BLEManager`.
     */
    MyClientCallbacks(BLEManager* manager) : _pManagerInstance(manager) {}
    /**
     * @brief Callback method invoked when a BLE client connects.
     * @param pclient Pointer to the connected `BLEClient` instance.
     */
    void onConnect(BLEClient* pclient) override;
    /**
     * @brief Callback method invoked when a BLE client disconnects.
     * Notifies the `BLEManager` of the disconnection.
     * @param pclient Pointer to the disconnected `BLEClient` instance.
     */
    void onDisconnect(BLEClient* pclient) override;
  };

  /**
   * @brief Callback handler for advertised BLE device results during a scan.
   */
  class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    BLEManager* _pManagerInstance; ///< Pointer to the owning `BLEManager` instance.
   public:
    /**
     * @brief Constructor for MyAdvertisedDeviceCallbacks.
     * @param manager Pointer to the owning `BLEManager`.
     */
    MyAdvertisedDeviceCallbacks(BLEManager* manager) : _pManagerInstance(manager) {}
    /**
     * @brief Callback method invoked when a new BLE advertised device is found.
     * Adds the device information to a FreeRTOS queue for asynchronous processing.
     * @param advertisedDevice The `BLEAdvertisedDevice` object containing device details.
     */
    void onResult(BLEAdvertisedDevice advertisedDevice) override;
  };

  // --- Private Member Variables ---
  // Dependencies
  SettingsManager* _settingsMgr = nullptr;      ///< Pointer to the `SettingsManager` instance.
  ScreenManager* _screenManagerPtr = nullptr;   ///< Pointer to the `ScreenManager` instance.
  BLEClient* _pClient = nullptr;                ///< Pointer to the active BLE client instance.
  WifiManager* _wifiManager = nullptr;          ///< Pointer to the `WifiManager` instance for radio coexistence.

  // Internal State Flags & Variables
  bool _bleEnabled = false;                     ///< True if BLE logic is enabled.
  volatile BLEMgr_State_t _currentState = BLEMgr_State_t::BLE_DISABLED; ///< Current state of the BLE Manager.
  std::string _connectingAddress;               ///< MAC address of the device currently attempting connection.
  std::string _connectedAddress;                ///< MAC address of the currently connected device.
  std::string _connectedName;                   ///< Name of the currently connected device.
  std::string _connectedServiceUUID;            ///< Service UUID of the currently connected device.
  bool _isManuallyDisconnecting = false;        ///< True if a disconnect was initiated by user action.
  bool _isDisconnectingInternally = false;      ///< True if an internal disconnect is being processed.
  bool _connectRequestPending = false;          ///< True if a connection request is pending execution.
  std::string _pendingConnectionServiceUUID;    ///< Service UUID for a deferred connection.
  std::string _pendingConnectionName;           ///< Name for a deferred connection.
  std::atomic<bool> _clientDisconnectedFlag{false}; ///< Atomic flag set by client callback on disconnect.
  char _lastDisconnectAddrBuf[18];              ///< Buffer to store MAC of last disconnected device.
  std::atomic<bool> _isLocalDisconnectInitiated{false}; ///< True if disconnect was initiated locally.
  std::string _justLocallyDisconnectedPrimaryId;///< Primary ID of device disconnected locally.
  std::string _pendingConnectionTargetPrimaryId;///< Primary ID of a device for a pending connection attempt.
  std::atomic<bool> _suppressSetConnectedOnlineDuringUpdate{false}; ///< True to suppress `isOnline` update for connected device during `_updateAllKnownDevices`.

  // Scan & Connection Flow Variables
  MyAdvertisedDeviceCallbacks* _myAdvertisedDeviceCallbacksInstance = nullptr; ///< Instance of scan callback handler.
  ScanPurpose _scanPurpose = ScanPurpose::General; ///< Purpose of the current scan.
  bool _suppressUiOnScanComplete = false;           ///< True to suppress UI updates on scan completion.
  bool _shortConnectInProgress = false;             ///< True if a short scan for connection is in progress.
  PendingConnectCriteria _pendingConnectCriteria;   ///< Criteria for a pending short connect.
  unsigned long _scanEndTime = 0;                   ///< Timestamp when the current scan is expected to end.
  unsigned long _smCycleCountAtScanRequest = 0;     ///< ScreenManager update cycle count at scan request.
  uint32_t _pendingScanDuration = 0;                ///< Duration of a pending scan.
  BLEMgr_State_t _stateBeforeScan = BLEMgr_State_t::BLE_DISABLED; ///< BLE state before the current scan started.
  unsigned long _initTimestamp = 0;                 ///< Timestamp of manager initialization.
  bool _autoConnectPendingOnInit = false;           ///< True if an auto-connect is pending after init.
  bool _pendingAutoConnect = false;                 ///< True if a general auto-connect is pending.
  unsigned long _disconnectStartTime = 0;           ///< Timestamp when a disconnect process started.
  unsigned long _disconnectTimeoutMs = 5000;        ///< Timeout for disconnect completion.
  std::string _connectedAddressBeforeScan;          ///< Stores connected address before scan.
  std::string _connectedNameBeforeScan;             ///< Stores connected name before scan.
  std::string _connectedServiceUUIDBeforeScan;      ///< Stores connected UUID before scan.
  uint32_t _currentScanDurationSec = 0;             ///< Duration of the current scan in seconds.
  unsigned long _lastScanTimestamp = 0;             ///< Timestamp of the last scan initiation.
  bool _scanInProgress = false;
  bool _isBleOperationDeferred = false;             ///< True if a BLE scan/connect operation was deferred due to Wi-Fi activity.

  // Autoreconnect specific variables
  bool _autoReconnectEnabled;                 ///< True if auto-reconnect is enabled, false otherwise.
  unsigned long _lastReconnectAttemptMs;         ///< Timestamp of the last auto-reconnect attempt.
  unsigned long _reconnectIntervalMs; ///< Interval for auto-reconnect attempts.


  // FreeRTOS Primitives
  SemaphoreHandle_t _knownDevicesMutex = nullptr;     ///< Mutex to protect `_allKnownDevices`.
  QueueHandle_t _advertisedDeviceQueue = nullptr;     ///< Queue for `AdvertisedPOD` objects from scan results.
  SemaphoreHandle_t _scanCompleteSemaphore = nullptr; ///< Semaphore to signal scan completion.
  TaskHandle_t _scanWorkerHandle = nullptr;           ///< Handle for the BLE scan worker task.
  TaskHandle_t _connectWorkerHandle = nullptr;        ///< Handle for the BLE connect worker task.

  // Data Structures
  std::map<std::string, ManagedBLEDevice> _allKnownDevices; ///< Map of all known BLE devices.

  // Callbacks
  ScanCompleteCb _onScanComplete = nullptr;           ///< Registered callback for scan completion.
  StateChangeCb _onStateChange = nullptr;             ///< Registered callback for state changes.
  BeforeScanCb _onBeforeBlockingScanStartsCb = nullptr; ///< Registered callback before blocking scan.
  PairedDeviceChangedCallback _onPairedDeviceChangedCallback; ///< Registered callback for paired device changes.

  // UI Elements
  IconElement* _btIconElement = nullptr; ///< Pointer to the status bar icon element for Bluetooth.


  // --- Private Helper Methods ---
  /**
   * @brief Changes the internal state of the BLEManager.
   * @param newState The new `BLEMgr_State_t` to transition to.
   * @param address The MAC address of the affected device (empty if not applicable).
   * @param name The name of the affected device (empty if not applicable).
   * @param serviceUUID The primary service UUID of the affected device (empty if not applicable).
   */
  void changeState(BLEMgr_State_t newState,
                   const std::string& address = "",
                   const std::string& name = "",
                   const std::string& serviceUUID = "");

  /**
   * @brief Performs a hard deinitialization of the BLE stack.
   * This is a low-level operation to shut down the BLE controller.
   */
  void _doDeinitNow();

  /**
   * @brief Handles a soft disable of BLE without deinitializing the underlying stack.
   * Stops scans, deletes client, and updates state.
   */
  void handleSoftDisable();

  /**
   * @brief Initiates an internal BLE scan.
   * @param durationSec Duration of the scan in seconds.
   * @param purpose Purpose of the scan (e.g., General, ShortConnect).
   */
  void _startScan(uint32_t durationSec, ScanPurpose purpose = ScanPurpose::General);

  /**
   * @brief Stops an active BLE scan and processes results.
   * Updates internal device lists and triggers callbacks.
   */
  void _stopScan();

  /**
   * @brief Attempts to connect to a BLE device after a short scan for its latest address.
   * This is part of the auto-connect flow.
   * @return True if connection was initiated, false otherwise.
   */
  bool _attemptConnectAfterShortScan();

  /**
   * @brief Handles internal disconnect events, cleaning up resources and updating state.
   */
  void handleInternalDisconnect();

  /**
   * @brief Attempts to connect to a BLE device given its `BLEDeviceData`.
   * This method starts a short scan if needed before attempting the actual connection.
   * @param deviceToConnect The data of the device to connect to.
   * @return True if the connection process was initiated, false otherwise.
   */
  bool attemptConnection(const BLEDeviceData& deviceToConnect);

  /**
   * @brief Connects to a BLE device using its MAC address.
   * @param address The MAC address of the device.
   * @return True if connection was initiated, false otherwise.
   */
  bool _connectToDeviceByMac(const std::string& address);

  /**
   * @brief Connects to a BLE device using its primary service UUID.
   * @param serviceUUID_str The service UUID string.
   * @param targetName The name of the target device (optional).
   * @return True if connection was initiated, false otherwise.
   */
  bool _connectToDeviceByServiceUUID(const std::string& serviceUUID_str, const std::string& targetName = "");

  /**
   * @brief Removes a paired device from internal lists by MAC address.
   * Disconnects from the device if it is currently connected.
   * @param address The MAC address of the device to remove.
   * @return True if successful, false otherwise.
   */
  bool _removePairedDevice(const std::string& address);

  /**
   * @brief Removes a paired device from internal lists by Service UUID.
   * Disconnects from the device if it is currently connected.
   * @param serviceUUID The Service UUID of the device to remove.
   * @return True if successful, false otherwise.
   */
  bool _removePairedDeviceByUUID(const std::string& serviceUUID);

  /**
   * @brief Updates the internal `_allKnownDevices` map.
   * Synchronizes with `SettingsManager` for paired devices and updates online status based on scans.
   */
  void _updateAllKnownDevices();
};

#endif // BLEMANAGER_H