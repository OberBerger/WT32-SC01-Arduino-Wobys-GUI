/**
 * @file SettingsManager.h
 * @brief Manages persistent application settings, including Wi-Fi, Bluetooth, display, and audio configurations.
 *
 * This class provides a centralized mechanism for loading, saving, and managing
 * various application settings persistently using the LittleFS filesystem. It
 * handles configurations for network connectivity, device preferences, display
 * brightness, sound, screensaver, and RFID functionality, ensuring settings
 * are preserved across reboots.
 *
 * @version 1.0.1
 * @date 2025-08-22
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
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Arduino.h>    // Required for `constrain` function
#include <vector>       // Required for `std::vector`
#include <string>       // Required for `std::string`
#include <LittleFS.h>   // Required for LittleFS filesystem operations
#include <ArduinoJson.h> // Required for JSON serialization/deserialization

// --- Data Structures for Stored Settings ---
/**
 * @brief Structure to store details of a saved Wi-Fi network.
 */
struct SavedWifiNetwork {
  std::string ssid;     ///< The SSID (name) of the Wi-Fi network.
  std::string password; ///< The password for the Wi-Fi network.
  // Future extensions could include:
  // bool autoConnect;
  // uint32_t priority;
  // bool isHidden;

  /**
   * @brief Constructor for `SavedWifiNetwork`.
   * Initializes SSID and password.
   * @param s The SSID (default: empty string).
   * @param p The password (default: empty string).
   */
  SavedWifiNetwork(const std::string& s = "", const std::string& p = "")
    : ssid(s), password(p) {}
};

/**
 * @brief Structure to store details of a paired Bluetooth Low Energy (BLE) device.
 */
struct PairedDevice {
  std::string name;        ///< The user-friendly name of the BLE device.
  std::string address;     ///< The MAC address of the BLE device.
  std::string serviceUUID; ///< The primary service UUID of the BLE device (optional).

  /**
   * @brief Constructor for `PairedDevice`.
   * Initializes name, address, and service UUID.
   * @param n The device name (default: empty string).
   * @param addr The device MAC address (default: empty string).
   * @param uuid The device service UUID (default: empty string).
   */
  PairedDevice(const std::string& n = "", const std::string& addr = "", const std::string& uuid = "")
    : name(n), address(addr), serviceUUID(uuid) {}
};

/**
 * @brief Manages persistent application settings.
 *
 * The SettingsManager class provides a comprehensive interface for loading,
 * saving, and managing various configuration parameters for the application.
 * It utilizes the LittleFS filesystem to store settings persistently in a
 * JSON format, ensuring that user preferences are maintained across reboots.
 * This manager handles Wi-Fi, Bluetooth, display, sound, screensaver, and
 * RFID settings, and exposes methods for accessing and modifying them.
 */
class SettingsManager {
private:
  // --- Internal State ---
  bool _isInitialized;                 ///< True if the SettingsManager has been successfully initialized.
  const char* _settingsFilePath = "/settings.json"; ///< Path to the settings file on LittleFS.

  // --- Stored Settings (In-memory cache) ---
  std::vector<SavedWifiNetwork> _savedNetworks; ///< List of saved Wi-Fi networks with credentials.

  // Wi-Fi Module Settings
  bool _wifiEnabledLastState;          ///< Last known state of Wi-Fi (enabled/disabled).
  bool _wifiAutoConnectEnabled;        ///< True if Wi-Fi should attempt auto-connection.
  std::string _lastConnectedSsid;      ///< SSID of the last successfully connected Wi-Fi network.

  // Bluetooth Module Settings
  bool _bluetoothEnabledLastState;     ///< Last known state of Bluetooth (enabled/disabled).
  std::vector<PairedDevice> _pairedBleDevices; ///< List of paired BLE devices.

  // General Device Settings
  std::string _deviceName;             ///< User-defined name for the device.
  int _brightnessLevel;                ///< Display brightness level (0-255).
  int _soundVolume;                    ///< System sound volume (0-100).
  bool _soundEnabled;                  ///< True if system sounds are enabled.
  bool _clickSoundEnabled; ///< Flag indicating if UI click sounds are enabled.
  std::string _currentLanguageCode;    ///< Current active language code (e.g., "EN", "HU").

  // Screensaver Settings
  bool _screensaverEnabled;            ///< True if the screensaver is enabled.
  int _screensaverTimeoutSec;          ///< Screensaver activation timeout in seconds.
  int _screensaverBrightness;          ///< Screensaver dimmed brightness level (0-255).

  // RFID Settings
  bool _rfidEnabled;                   ///< True if RFID functionality is enabled.

  // --- Private Helper Methods ---
  /**
   * @brief Loads all application settings from the `/settings.json` file on LittleFS.
   * If the file does not exist or is corrupted, it returns false.
   * @return True if settings were successfully loaded, false otherwise.
   */
  bool loadSettingsFromFile();

  /**
   * @brief Saves all current in-memory application settings to the `/settings.json` file on LittleFS.
   * @return True if settings were successfully saved, false otherwise.
   */
  bool saveSettingsToFile();

public:
  // --- Constructor ---
  /**
   * @brief Constructor for the SettingsManager class.
   * Initializes internal state variables with default values.
   */
  SettingsManager();

  // --- Initialization ---
  /**
   * @brief Initializes the SettingsManager and loads settings from persistent storage.
   * This method should be called once during system setup. It initializes LittleFS
   * and attempts to load existing settings or creates defaults if none are found.
   * @return True if the manager was successfully initialized, false otherwise.
   */
  bool init();

  /**
   * @brief Checks if the SettingsManager has been successfully initialized.
   * @return True if initialized, false otherwise.
   */
  bool isInitialized() const { return _isInitialized; }

  // --- Wi-Fi Network Management ---
  /**
   * @brief Retrieves a constant reference to the vector of saved Wi-Fi networks.
   * @return A `const std::vector<SavedWifiNetwork>&` of saved networks.
   */
  const std::vector<SavedWifiNetwork>& getSavedNetworks() const;

  /**
   * @brief Adds a new Wi-Fi network or updates an existing one with its SSID and password.
   * Saves the updated settings to file.
   * @param ssid The SSID of the Wi-Fi network.
   * @param password The password for the network.
   * @return True if the network was added/updated, false if the list is full or SSID is empty.
   */
  bool addOrUpdateSavedNetwork(const std::string& ssid, const std::string& password);

  /**
   * @brief Removes a saved Wi-Fi network by its SSID.
   * Saves the updated settings to file.
   * @param ssid The SSID of the network to remove.
   * @return True if the network was found and removed, false otherwise.
   */
  bool removeSavedNetwork(const std::string& ssid);

  /**
   * @brief Finds a saved Wi-Fi network by its SSID.
   * @param ssid The SSID of the network to find.
   * @return A `const pointer` to the `SavedWifiNetwork` if found, `nullptr` otherwise.
   */
  const SavedWifiNetwork* findSavedNetwork(const std::string& ssid) const;

  /**
   * @brief Clears all saved Wi-Fi networks from the list.
   * Saves the updated settings to file.
   * @return True if successful (always true if list was already empty).
   */
  bool clearAllSavedNetworks();

  // --- Wi-Fi Module State Getters/Setters ---
  /**
   * @brief Sets the last known enabled/disabled state of the Wi-Fi module.
   * Saves the updated setting to file.
   * @param enabled True if Wi-Fi was last enabled, false otherwise.
   */
  void setWifiEnabledLastState(bool enabled);

  /**
   * @brief Retrieves the last known enabled/disabled state of the Wi-Fi module.
   * @param defaultValue A fallback value if the setting is not found (default: false).
   * @return The last known Wi-Fi enabled state.
   */
  bool getWifiEnabledLastState(bool defaultValue = false) const;

  /**
   * @brief Sets whether Wi-Fi should attempt automatic connection.
   * Saves the updated setting to file.
   * @param enabled True to enable auto-connect, false otherwise.
   */
  void setWifiAutoConnectEnabled(bool enabled);

  /**
   * @brief Retrieves whether Wi-Fi should attempt automatic connection.
   * @param defaultValue A fallback value if the setting is not found (default: true).
   * @return True if Wi-Fi auto-connect is enabled.
   */
  bool isWifiAutoConnectEnabled(bool defaultValue = true) const;

  /**
   * @brief Sets the SSID of the last successfully connected Wi-Fi network.
   * Saves the updated setting to file.
   * @param ssid The SSID string.
   */
  void setLastConnectedSsid(const std::string& ssid);

  /**
   * @brief Retrieves the SSID of the last successfully connected Wi-Fi network.
   * @return The SSID string, or an empty string if none was recorded.
   */
  std::string getLastConnectedSsid() const;

  // --- Bluetooth Module Settings ---
  /**
   * @brief Sets the last known enabled/disabled state of the Bluetooth module.
   * Saves the updated setting to file.
   * @param enabled True if Bluetooth was last enabled, false otherwise.
   */
  void setBluetoothEnabledLastState(bool enabled);

  /**
   * @brief Retrieves the last known enabled/disabled state of the Bluetooth module.
   * @param defaultValue A fallback value if the setting is not found (default: false).
   * @return The last known Bluetooth enabled state.
   */
  bool getBluetoothEnabledLastState(bool defaultValue = false) const;

  /**
   * @brief Retrieves a constant reference to the vector of paired BLE devices.
   * @return A `const std::vector<PairedDevice>&` of paired devices.
   */
  const std::vector<PairedDevice>& getPairedDevices() const;

  /**
   * @brief Adds a new BLE device or updates an existing one in the paired devices list.
   * Attempts to find by address first, then by name if address is new. Saves settings.
   * @param name The user-friendly name of the BLE device.
   * @param address The MAC address of the BLE device.
   * @param serviceUUID_str The primary service UUID as a string (can be empty).
   * @return True if the device was added/updated, false if the list is full or invalid input.
   */
  bool addOrUpdatePairedDevice(const std::string& name,
                               const std::string& address,
                               const std::string& serviceUUID_str);

  /**
   * @brief Removes a paired BLE device by its MAC address.
   * Saves the updated settings to file.
   * @param address The MAC address of the device to remove.
   * @return True if the device was found and removed, false otherwise.
   */
  bool removePairedDevice(const std::string& address);

  /**
   * @brief Removes a paired BLE device by its primary service UUID.
   * Saves the updated settings to file.
   * @param serviceUUID The primary service UUID of the device to remove.
   * @return True if the device was found and removed, false otherwise.
   */
  bool removePairedDeviceByUUID(const std::string& serviceUUID);

  /**
   * @brief Finds a paired BLE device by its MAC address.
   * @param address The MAC address of the device to find.
   * @return A `const pointer` to the `PairedDevice` if found, `nullptr` otherwise.
   */
  const PairedDevice* findPairedDeviceByAddress(const std::string& address) const;

  /**
   * @brief Finds a paired BLE device by its user-friendly name.
   * @param name The name of the device to find.
   * @return A `const pointer` to the `PairedDevice` if found, `nullptr` otherwise.
   */
  const PairedDevice* findPairedDeviceByName(const std::string& name) const;

  // --- General Device Settings ---
  /**
   * @brief Sets the user-defined name for the device.
   * Saves the updated setting to file.
   * @param name The new device name.
   */
  void setDeviceName(const std::string& name);

  /**
   * @brief Retrieves the user-defined name for the device.
   * @param defaultValue A fallback value if the setting is not found (default: "ESP32_Device").
   * @return The device name.
   */
  std::string getDeviceName(const std::string& defaultValue = "ESP32_Device") const;

  /**
   * @brief Sets the display brightness level.
   * The value is constrained between 0 and 255. Saves the updated setting to file.
   * @param level The new brightness level.
   */
  void setBrightness(int level);

  /**
   * @brief Retrieves the display brightness level.
   * @param defaultValue A fallback value if the setting is not found (default: 80).
   * @return The brightness level.
   */
  int getBrightness(int defaultValue = 80) const;

  /**
   * @brief Sets whether system sounds are enabled.
   * Saves the updated setting to file.
   * @param enabled True to enable sounds, false otherwise.
   */
  void setSoundEnabled(bool enabled);

  /**
   * @brief Retrieves whether system sounds are enabled.
   * @param defaultValue A fallback value if the setting is not found (default: true).
   * @return True if sounds are enabled.
   */
  bool isSoundEnabled(bool defaultValue = true) const;

  /**
   * @brief Sets the current system sound volume level.
   * The value is constrained between 0 and 100. Saves the updated setting to file.
   * @param volume The new volume level.
   */
  void setSoundVolume(int volume);

  /**
   * @brief Retrieves the current system sound volume level.
   * @param defaultValue A fallback value if the setting is not found (default: 100).
   * @return The volume level.
   */
  int getSoundVolume(int defaultValue = 100) const;

  /**
   * @brief Sets whether UI click sounds are enabled.
   * Saves the updated setting to file.
   * @param enabled True to enable click sounds, false otherwise.
   */
  void setClickSoundEnabled(bool enabled);

  /**
     * @brief Retrieves whether UI click sounds are enabled.
     * @param defaultValue A fallback value if the setting is not found (default: true).
     * @return True if click sounds are enabled.
     */
  bool isClickSoundEnabled(bool defaultValue = true) const;

  /**
   * @brief Sets the current language code for the UI (e.g., "EN", "HU").
   * Saves the updated setting to file.
   * @param code The language code string.
   */
  void setCurrentLanguageCode(const std::string& code);

  /**
   * @brief Retrieves the current language code for the UI.
   * @return The language code string, or "EN" if not found.
   */
  std::string getCurrentLanguageCode() const;

  // --- Screensaver Settings ---
  /**
   * @brief Sets whether the screensaver is enabled.
   * Saves the updated setting to file.
   * @param enabled True to enable the screensaver, false otherwise.
   */
  void setScreensaverEnabled(bool enabled);

  /**
   * @brief Retrieves whether the screensaver is enabled.
   * @param defaultValue A fallback value if the setting is not found (default: true).
   * @return True if the screensaver is enabled.
   */
  bool isScreensaverEnabled(bool defaultValue = true) const;

  /**
   * @brief Sets the screensaver activation timeout in seconds.
   * The value is constrained between 5 and 300 seconds. Saves the updated setting to file.
   * @param seconds The new timeout in seconds.
   */
  void setScreensaverTimeout(int seconds);

  /**
   * @brief Retrieves the screensaver activation timeout in seconds.
   * @param defaultValue A fallback value if the setting is not found (default: 30).
   * @return The timeout in seconds.
   */
  int getScreensaverTimeout(int defaultValue = 30) const;

  /**
   * @brief Sets the dimmed brightness level for the screensaver.
   * The value is constrained between 0 and 255. Saves the updated setting to file.
   * @param level The new brightness level.
   */
  void setScreensaverBrightness(int level);

  /**
   * @brief Retrieves the dimmed brightness level for the screensaver.
   * @param defaultValue A fallback value if the setting is not found (default: 10).
   * @return The brightness level.
   */
  int getScreensaverBrightness(int defaultValue = 10) const;

  // --- RFID Settings ---
  /**
   * @brief Sets whether RFID functionality is enabled.
   * Saves the updated setting to file.
   * @param enabled True to enable RFID, false otherwise.
   */
  void setRfidEnabled(bool enabled);

  /**
   * @brief Retrieves whether RFID functionality is enabled.
   * @param defaultValue A fallback value if the setting is not found (default: true).
   * @return True if RFID is enabled.
   */
  bool isRfidEnabled(bool defaultValue = true) const;

  // --- General Persistence ---
  /**
   * @brief Forces a save of all current in-memory settings to the settings file.
   * This is useful when multiple settings have been changed and a single write
   * operation is desired.
   * @return True if settings were successfully saved, false otherwise.
   */
  bool forceSave();

  // The `formatFileSystem()` method is intentionally commented out for safety
  // as formatting the entire filesystem can lead to data loss.
  // bool formatFileSystem();
};

#endif // SETTINGSMANAGER_H