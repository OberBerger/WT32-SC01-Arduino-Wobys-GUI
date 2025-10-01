/**
 * @file SettingsManager.cpp
 * @brief Implements the SettingsManager class for managing persistent application settings.
 *
 * This file provides the core logic for initializing LittleFS, loading and
 * saving settings in JSON format, and managing various configuration parameters
 * in memory. It includes methods for accessing and modifying Wi-Fi, Bluetooth,
 * display, sound, screensaver, and RFID settings.
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
#include "SettingsManager.h"
#include "Config.h"
#include <Arduino.h>   // Required for `constrain` function
#include <algorithm>   // Required for `std::remove_if`

// --- Constructor Implementation ---
/**
 * @brief Constructor for the SettingsManager class.
 * Initializes internal state variables with default values.
 */
SettingsManager::SettingsManager()
  : _isInitialized(false),
    _wifiEnabledLastState(false),
    _wifiAutoConnectEnabled(true),
    _lastConnectedSsid(""),
    _bluetoothEnabledLastState(false),
    _deviceName(""),
    _brightnessLevel(80),
    _soundEnabled(true),
    _soundVolume(100),
    _clickSoundEnabled(true),
    _currentLanguageCode("EN"),
    _screensaverEnabled(true),
    _screensaverTimeoutSec(30),
    _screensaverBrightness(10),
    _rfidEnabled(true)
{
  DEBUG_INFO_PRINTLN("SettingsManager: Constructor executed.");
}

// --- Initialization ---
/**
 * @brief Initializes the SettingsManager and loads settings from persistent storage.
 * This method should be called once during system setup. It initializes LittleFS
 * and attempts to load existing settings or creates defaults if none are found.
 * @return True if the manager was successfully initialized, false otherwise.
 */
bool SettingsManager::init() {
  DEBUG_INFO_PRINTLN("SettingsManager: Initializing LittleFS filesystem...");
  if (!LittleFS.begin(false)) { // Attempt to mount LittleFS.
    DEBUG_WARN_PRINTLN("SettingsManager: LittleFS.begin() failed. Attempting to format...");
    if (!LittleFS.begin(true)) { // If mount failed, attempt to format and then mount.
      DEBUG_ERROR_PRINTLN("SettingsManager: ERROR - LittleFS mounting failed even after formatting!");
      _isInitialized = false;
      return false;
    }
    DEBUG_INFO_PRINTLN("SettingsManager: LittleFS successfully formatted and mounted.");
  } else {
    DEBUG_INFO_PRINTLN("SettingsManager: LittleFS successfully mounted.");
  }

  _isInitialized = true;
  DEBUG_INFO_PRINTLN("SettingsManager: Loading settings from file...");
  if (!loadSettingsFromFile()) { // Attempt to load settings.
    DEBUG_INFO_PRINTLN("SettingsManager: Failed to load settings (file not found/empty/corrupted). Using defaults and attempting to save.");
    if (!saveSettingsToFile()) { // Save defaults if loading failed.
      DEBUG_ERROR_PRINTLN("SettingsManager: ERROR - Failed to save default settings!");
    }
  } else {
    DEBUG_INFO_PRINTLN("SettingsManager: Settings successfully loaded.");
  }
  return _isInitialized;
}

// --- Private Helper Methods Implementations ---
/**
 * @brief Loads all application settings from the `/settings.json` file on LittleFS.
 * If the file does not exist or is corrupted, it returns false.
 * @return True if settings were successfully loaded, false otherwise.
 */
bool SettingsManager::loadSettingsFromFile() {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot load settings.");
      return false;
  }

  File configFile = LittleFS.open(_settingsFilePath, "r");
  if (!configFile) {
      DEBUG_INFO_PRINTF("SettingsManager: Settings file '%s' not found.\n", _settingsFilePath);
      return false;
  }

  if (configFile.size() == 0) {
    DEBUG_WARN_PRINTLN("SettingsManager: Settings file is empty.");
    configFile.close();
    return false;
  }

  // Use a sufficient buffer for JSON document (adjust size as settings grow).
  DynamicJsonDocument doc(2048);

  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  if (error) {
    DEBUG_ERROR_PRINTF("SettingsManager: ERROR - JSON deserialization failed: %s.\n", error.c_str());
    return false;
  }

  DEBUG_TRACE_PRINTLN("SettingsManager: JSON successfully parsed, loading values...");

  // Load Wi-Fi settings.
  _deviceName = doc["deviceName"] | "ESP32_Device_Default";
  _wifiEnabledLastState = doc["wifiEnabledLastState"] | false;
  _wifiAutoConnectEnabled = doc["wifiAutoConnectEnabled"] | true;
  _lastConnectedSsid = doc["lastConnectedSsid"] | "";

  _savedNetworks.clear();
  JsonArray networksArray = doc["savedNetworks"].as<JsonArray>();
  if (!networksArray.isNull()) {
    for (JsonObject networkObj : networksArray) {
      SavedWifiNetwork net;
      net.ssid = networkObj["ssid"] | "";
      net.password = networkObj["password"] | "";
      if (!net.ssid.empty()) { // Only add if SSID is not empty.
        if (_savedNetworks.size() < MAX_SAVED_WIFI_NETWORKS) {
          _savedNetworks.push_back(net);
        } else {
          DEBUG_WARN_PRINTLN("SettingsManager: Max saved Wi-Fi networks reached, skipping further loading.");
          break;
        }
      }
    }
  }
  DEBUG_INFO_PRINTF("SettingsManager: Loaded %d saved Wi-Fi networks.\n", _savedNetworks.size());

  // Load Bluetooth settings.
  _bluetoothEnabledLastState = doc["btEnabledLastState"] | false;
  _pairedBleDevices.clear();
  JsonArray bleArray = doc["pairedBleDevices"].as<JsonArray>();
  if (!bleArray.isNull()) {
    for (JsonObject bleDeviceJson : bleArray) {
      PairedDevice dev;
      dev.name = bleDeviceJson["name"] | "";
      dev.address = bleDeviceJson["address"] | "";
      dev.serviceUUID = bleDeviceJson["serviceUUID"] | "";
      if (!dev.address.empty()) { // Only add if address is not empty.
        if (_pairedBleDevices.size() < MAX_PAIRED_BLE_DEVICES) {
            _pairedBleDevices.push_back(dev);
        } else {
            DEBUG_WARN_PRINTLN("SettingsManager: Max paired BLE devices reached, skipping further loading.");
            break;
        }
      }
    }
  }
  DEBUG_INFO_PRINTF("SettingsManager: Loaded %d paired BLE devices.\n", _pairedBleDevices.size());

  // Load general device settings.
  _brightnessLevel = doc["brightnessLevel"] | 80;
  _soundEnabled = doc["soundEnabled"] | true;
  _soundVolume = doc["soundVolume"] | 100;
  _clickSoundEnabled = doc["clickSoundEnabled"] | true;
  _currentLanguageCode = doc["currentLanguage"] | "EN";

  // Load Screensaver settings.
  _screensaverEnabled = doc["screensaverEnabled"] | true;
  _screensaverTimeoutSec = doc["screensaverTimeout"] | 30;
  _screensaverBrightness = doc["screensaverBrightness"] | 10;

  // Load RFID settings.
  _rfidEnabled = doc["rfidEnabled"] | true;

  return true;
}

/**
 * @brief Saves all current in-memory application settings to the `/settings.json` file on LittleFS.
 * @return True if settings were successfully saved, false otherwise.
 */
bool SettingsManager::saveSettingsToFile() {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot save settings.");
      return false;
  }

  File configFile = LittleFS.open(_settingsFilePath, "w");
  if (!configFile) {
      DEBUG_ERROR_PRINTF("SettingsManager: ERROR - Failed to open settings file '%s' for writing.\n", _settingsFilePath);
      return false;
  }

  DynamicJsonDocument doc(2048); // Ensure sufficient buffer size.

  // Save Wi-Fi settings.
  doc["deviceName"] = _deviceName;
  doc["wifiEnabledLastState"] = _wifiEnabledLastState;
  doc["wifiAutoConnectEnabled"] = _wifiAutoConnectEnabled;
  doc["lastConnectedSsid"] = _lastConnectedSsid;

  JsonArray networksArray = doc.createNestedArray("savedNetworks");
  for (const auto& net : _savedNetworks) {
    JsonObject networkObj = networksArray.createNestedObject();
    networkObj["ssid"] = net.ssid;
    networkObj["password"] = net.password; // Store empty string if no password.
  }

  // Save Bluetooth settings.
  doc["btEnabledLastState"] = _bluetoothEnabledLastState;
  JsonArray bleArray = doc.createNestedArray("pairedBleDevices");
  for (const auto& dev : _pairedBleDevices) {
    JsonObject bleDeviceJson = bleArray.createNestedObject();
    bleDeviceJson["name"] = dev.name;
    bleDeviceJson["address"] = dev.address;
    bleDeviceJson["serviceUUID"] = dev.serviceUUID; // Store empty string if no UUID.
  }

  // Save general device settings.
  doc["brightnessLevel"] = _brightnessLevel;
  doc["soundEnabled"] = _soundEnabled;
  doc["soundVolume"] = _soundVolume;
  doc["clickSoundEnabled"] = _clickSoundEnabled;
  doc["currentLanguage"] = _currentLanguageCode;

  // Save Screensaver settings.
  doc["screensaverEnabled"] = _screensaverEnabled;
  doc["screensaverTimeout"] = _screensaverTimeoutSec;
  doc["screensaverBrightness"] = _screensaverBrightness;

  // Save RFID settings.
  doc["rfidEnabled"] = _rfidEnabled;


  if (serializeJson(doc, configFile) == 0) {
    DEBUG_ERROR_PRINTLN("SettingsManager: ERROR - Failed to serialize JSON to file.");
    configFile.close();
    return false;
  }

  configFile.close();
  DEBUG_INFO_PRINTLN("SettingsManager: Settings successfully saved to file.");
  return true;
}

// --- Wi-Fi Network Management ---
/**
 * @brief Retrieves a constant reference to the vector of saved Wi-Fi networks.
 * @return A `const std::vector<SavedWifiNetwork>&` of saved networks.
 */
const std::vector<SavedWifiNetwork>& SettingsManager::getSavedNetworks() const {
  return _savedNetworks;
}

/**
 * @brief Adds a new Wi-Fi network or updates an existing one with its SSID and password.
 * Saves the updated settings to file.
 * @param ssid The SSID of the Wi-Fi network.
 * @param password The password for the network.
 * @return True if the network was added/updated, false if the list is full or SSID is empty.
 */
bool SettingsManager::addOrUpdateSavedNetwork(const std::string& ssid, const std::string& password) {
  if (!_isInitialized || ssid.empty()) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized or SSID empty, cannot add/update network.");
      return false;
  }

  for (auto& net : _savedNetworks) {
    if (net.ssid == ssid) {
      // Update if the password is different.
      if (net.password != password) {
        net.password = password;
        DEBUG_INFO_PRINTF("SettingsManager: Saved Wi-Fi network '%s' updated.\n", ssid.c_str());
        return saveSettingsToFile();
      }
      DEBUG_TRACE_PRINTF("SettingsManager: Wi-Fi network '%s' already saved with same password, no update needed.\n", ssid.c_str());
      return true; // Already exists with same password, no changes.
    }
  }

  // Add as a new network if not found and there's space.
  if (_savedNetworks.size() < MAX_SAVED_WIFI_NETWORKS) {
    _savedNetworks.push_back({ ssid, password });
    DEBUG_INFO_PRINTF("SettingsManager: New Wi-Fi network '%s' saved.\n", ssid.c_str());
    return saveSettingsToFile();
  } else {
    DEBUG_WARN_PRINTLN("SettingsManager: Max saved Wi-Fi networks reached, cannot add new network.");
    return false;
  }
}

/**
 * @brief Removes a saved Wi-Fi network by its SSID.
 * Saves the updated settings to file.
 * @param ssid The SSID of the network to remove.
 * @return True if the network was found and removed, false otherwise.
 */
bool SettingsManager::removeSavedNetwork(const std::string& ssid) {
  if (!_isInitialized || ssid.empty()) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized or SSID empty, cannot remove network.");
      return false;
  }
  bool changed = false;
  _savedNetworks.erase(
    std::remove_if(_savedNetworks.begin(), _savedNetworks.end(),
                   [&](const SavedWifiNetwork& net) {
                     if (net.ssid == ssid) {
                       changed = true;
                       return true;
                     }
                     return false;
                   }),
    _savedNetworks.end());

  if (changed) {
    DEBUG_INFO_PRINTF("SettingsManager: Wi-Fi network '%s' removed.\n", ssid.c_str());
    return saveSettingsToFile();
  }
  DEBUG_TRACE_PRINTF("SettingsManager: Wi-Fi network '%s' not found for removal.\n", ssid.c_str());
  return false; // Network not found.
}

/**
 * @brief Finds a saved Wi-Fi network by its SSID.
 * @param ssid The SSID of the network to find.
 * @return A `const pointer` to the `SavedWifiNetwork` if found, `nullptr` otherwise.
 */
const SavedWifiNetwork* SettingsManager::findSavedNetwork(const std::string& ssid) const {
  for (const auto& net : _savedNetworks) {
    if (net.ssid == ssid) {
      return &net;
    }
  }
  return nullptr;
}

/**
 * @brief Clears all saved Wi-Fi networks from the list.
 * Saves the updated settings to file.
 * @return True if successful (always true if list was already empty).
 */
bool SettingsManager::clearAllSavedNetworks() {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot clear networks.");
      return false;
  }
  if (!_savedNetworks.empty()) {
    _savedNetworks.clear();
    DEBUG_INFO_PRINTLN("SettingsManager: All saved Wi-Fi networks cleared.");
    return saveSettingsToFile();
  }
  DEBUG_TRACE_PRINTLN("SettingsManager: No saved Wi-Fi networks to clear.");
  return true; // Already empty.
}


// --- Wi-Fi Module State Getters/Setters ---
/**
 * @brief Sets the last known enabled/disabled state of the Wi-Fi module.
 * Saves the updated setting to file.
 * @param enabled True if Wi-Fi was last enabled, false otherwise.
 */
void SettingsManager::setWifiEnabledLastState(bool enabled) {
  if (_wifiEnabledLastState != enabled) {
    _wifiEnabledLastState = enabled;
    saveSettingsToFile(); // Save immediately on change.
    DEBUG_INFO_PRINTF("SettingsManager: Wi-Fi last enabled state set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves the last known enabled/disabled state of the Wi-Fi module.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The last known Wi-Fi enabled state.
 */
bool SettingsManager::getWifiEnabledLastState(bool defaultValue) const {
  // `loadSettingsFromFile` already handles populating `_wifiEnabledLastState`
  // with either saved data or its default. So `defaultValue` is primarily for
  // cases where `SettingsManager` itself wasn't initialized or loading failed.
  return _wifiEnabledLastState;
}

/**
 * @brief Sets whether Wi-Fi should attempt automatic connection.
 * Saves the updated setting to file.
 * @param enabled True to enable auto-connect, false otherwise.
 */
void SettingsManager::setWifiAutoConnectEnabled(bool enabled) {
  if (_wifiAutoConnectEnabled != enabled) {
    _wifiAutoConnectEnabled = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Wi-Fi auto-connect enabled set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves whether Wi-Fi should attempt automatic connection.
 * @param defaultValue A fallback value if the setting is not found.
 * @return True if Wi-Fi auto-connect is enabled.
 */
bool SettingsManager::isWifiAutoConnectEnabled(bool defaultValue) const {
  return _wifiAutoConnectEnabled;
}

/**
 * @brief Sets the SSID of the last successfully connected Wi-Fi network.
 * Saves the updated setting to file.
 * @param ssid The SSID string.
 */
void SettingsManager::setLastConnectedSsid(const std::string& ssid) {
  if (_lastConnectedSsid != ssid) {
    _lastConnectedSsid = ssid;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Last connected SSID set to '%s'.\n", ssid.c_str());
  }
}
/**
 * @brief Retrieves the SSID of the last successfully connected Wi-Fi network.
 * @return The SSID string, or an empty string if none was recorded.
 */
std::string SettingsManager::getLastConnectedSsid() const {
  return _lastConnectedSsid;
}

// --- Bluetooth Module Settings ---
/**
 * @brief Sets the last known enabled/disabled state of the Bluetooth module.
 * Saves the updated setting to file.
 * @param enabled True if Bluetooth was last enabled, false otherwise.
 */
void SettingsManager::setBluetoothEnabledLastState(bool enabled) {
  if (_bluetoothEnabledLastState != enabled) {
    _bluetoothEnabledLastState = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Bluetooth last enabled state set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves the last known enabled/disabled state of the Bluetooth module.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The last known Bluetooth enabled state.
 */
bool SettingsManager::getBluetoothEnabledLastState(bool defaultValue) const {
  return _bluetoothEnabledLastState;
}

/**
 * @brief Retrieves a constant reference to the vector of paired BLE devices.
 * @return A `const std::vector<PairedDevice>&` of paired devices.
 */
const std::vector<PairedDevice>& SettingsManager::getPairedDevices() const {
    return _pairedBleDevices;
}

/**
 * @brief Adds a new BLE device or updates an existing one in the paired devices list.
 * Attempts to find by address first, then by name if address is new. Saves settings.
 * @param name The user-friendly name of the BLE device.
 * @param address The MAC address of the BLE device.
 * @param serviceUUID_str The primary service UUID as a string (can be empty).
 * @return True if the device was added/updated, false if the list is full or invalid input.
 */
bool SettingsManager::addOrUpdatePairedDevice(const std::string& name,
                                              const std::string& address,
                                              const std::string& serviceUUID_str) {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot add/update paired BLE device.");
      return false;
  }
  DEBUG_INFO_PRINTF("SettingsManager: Add/Update Paired BLE Device. Name:'%s', Address:'%s', UUID:'%s'.\n", name.c_str(), address.c_str(), serviceUUID_str.c_str());

  for (auto& dev : _pairedBleDevices) {
    if (dev.address == address) { // Primary match: MAC address.
      DEBUG_TRACE_PRINTLN("SettingsManager: Existing device found by address, updating details.");
      dev.name = name;
      dev.serviceUUID = serviceUUID_str;
      return saveSettingsToFile();
    }
  }

  // If not found by address, and name is provided, try matching by name (address might have changed).
  if (!name.empty()) {
    for (auto& dev : _pairedBleDevices) {
      if (dev.name == name) {
        DEBUG_TRACE_PRINTLN("SettingsManager: Existing device found by name, updating address/UUID.");
        dev.address = address; // Update to the latest known address.
        dev.serviceUUID = serviceUUID_str;
        return saveSettingsToFile();
      }
    }
  }

  // If still not found, add as a new device if space is available.
  if (_pairedBleDevices.size() < MAX_PAIRED_BLE_DEVICES) {
    DEBUG_INFO_PRINTLN("SettingsManager: Adding new paired BLE device.");
    _pairedBleDevices.emplace_back(name, address, serviceUUID_str);
    return saveSettingsToFile();
  }
  DEBUG_WARN_PRINTLN("SettingsManager: Max paired BLE devices reached, cannot add new device.");
  return false;
}

/**
 * @brief Removes a paired BLE device by its MAC address.
 * Saves the updated settings to file.
 * @param address The MAC address of the device to remove.
 * @return True if the device was found and removed, false otherwise.
 */
bool SettingsManager::removePairedDevice(const std::string& address) {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot remove paired BLE device.");
      return false;
  }
  DEBUG_INFO_PRINTF("SettingsManager: Removing Paired BLE Device by Address: '%s'.\n", address.c_str());
  auto it = std::remove_if(_pairedBleDevices.begin(), _pairedBleDevices.end(),
                           [&](const PairedDevice& dev) {
                             return dev.address == address;
                           });
  if (it != _pairedBleDevices.end()) {
    _pairedBleDevices.erase(it, _pairedBleDevices.end());
    DEBUG_INFO_PRINTLN("SettingsManager: Paired BLE device removed.");
    return saveSettingsToFile();
  }
  DEBUG_TRACE_PRINTLN("SettingsManager: Paired BLE device not found for removal by address.");
  return false;
}

/**
 * @brief Removes a paired BLE device by its primary service UUID.
 * Saves the updated settings to file.
 * @param serviceUUID The primary service UUID of the device to remove.
 * @return True if the device was found and removed, false otherwise.
 */
bool SettingsManager::removePairedDeviceByUUID(const std::string& serviceUUID) {
  if (!_isInitialized) {
      DEBUG_WARN_PRINTLN("SettingsManager: Not initialized, cannot remove paired BLE device by UUID.");
      return false;
  }
  DEBUG_INFO_PRINTF("SettingsManager: Removing Paired BLE Device by UUID: '%s'.\n", serviceUUID.c_str());
  
  auto it = std::remove_if(_pairedBleDevices.begin(), _pairedBleDevices.end(),
    [&](const PairedDevice& dev) {
      return dev.serviceUUID == serviceUUID;
    });

  if (it != _pairedBleDevices.end()) {
    _pairedBleDevices.erase(it, _pairedBleDevices.end());
    
    DEBUG_INFO_PRINTLN("SettingsManager: Paired BLE device successfully removed by UUID.");
    return saveSettingsToFile();
  }

  DEBUG_TRACE_PRINTLN("SettingsManager: Paired BLE device not found for removal by UUID.");
  return false;
}

/**
 * @brief Finds a paired BLE device by its MAC address.
 * @param address The MAC address of the device to find.
 * @return A `const pointer` to the `PairedDevice` if found, `nullptr` otherwise.
 */
const PairedDevice* SettingsManager::findPairedDeviceByAddress(const std::string& address) const {
  for (const auto& dev : _pairedBleDevices) {
    if (dev.address == address) {
      return &dev;
    }
  }
  return nullptr;
}

/**
 * @brief Finds a paired BLE device by its user-friendly name.
 * @param name The name of the device to find.
 * @return A `const pointer` to the `PairedDevice` if found, `nullptr` otherwise.
 */
const PairedDevice* SettingsManager::findPairedDeviceByName(const std::string& name) const {
  for (const auto& dev : _pairedBleDevices) {
    if (dev.name == name) {
      return &dev;
    }
  }
  return nullptr;
}

// --- General Device Settings ---
/**
 * @brief Sets the user-defined name for the device.
 * Saves the updated setting to file.
 * @param name The new device name.
 */
void SettingsManager::setDeviceName(const std::string& name) {
  if (_deviceName != name) {
    _deviceName = name;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Device name set to '%s'.\n", name.c_str());
  }
}
/**
 * @brief Retrieves the user-defined name for the device.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The device name.
 */
std::string SettingsManager::getDeviceName(const std::string& defaultValue) const {
  return _deviceName.empty() ? defaultValue : _deviceName;
}

/**
 * @brief Sets the display brightness level.
 * The value is constrained between 0 and 255. Saves the updated setting to file.
 * @param level The new brightness level.
 */
void SettingsManager::setBrightness(int level) {
  level = constrain(level, 0, 255); // Constrain value to valid range.
  if (_brightnessLevel != level) {
    _brightnessLevel = level;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Display brightness set to %d.\n", level);
  }
}
/**
 * @brief Retrieves the display brightness level.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The brightness level.
 */
int SettingsManager::getBrightness(int defaultValue) const {
  return _brightnessLevel;
}

/**
 * @brief Sets whether system sounds are enabled.
 * Saves the updated setting to file.
 * @param enabled True to enable sounds, false otherwise.
 */
void SettingsManager::setSoundEnabled(bool enabled) {
  if (_soundEnabled != enabled) {
    _soundEnabled = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Sound enabled state set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves whether system sounds are enabled.
 * @param defaultValue A fallback value if the setting is not found.
 * @return True if sounds are enabled.
 */
bool SettingsManager::isSoundEnabled(bool defaultValue) const {
  return _soundEnabled;
}

/**
 * @brief Sets the current system sound volume level.
 * The value is constrained between 0 and 100. Saves the updated setting to file.
 * @param volume The new volume level.
 */
void SettingsManager::setSoundVolume(int volume) {
  volume = constrain(volume, 0, 100); // Constrain value to valid range.
  if (_soundVolume != volume) {
    _soundVolume = volume;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Sound volume set to %d.\n", volume);
  }
}
/**
 * @brief Retrieves the current system sound volume level.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The volume level.
 */
int SettingsManager::getSoundVolume(int defaultValue) const {
  return _soundVolume;
}

/**
 * @brief Sets the current language code for the UI (e.g., "EN", "HU").
 * Saves the updated setting to file.
 * @param code The language code string.
 */
void SettingsManager::setCurrentLanguageCode(const std::string& code) {
    if (_currentLanguageCode != code) {
        _currentLanguageCode = code;
        saveSettingsToFile();
        DEBUG_INFO_PRINTF("SettingsManager: Current language code set to '%s'.\n", code.c_str());
    }
}
/**
 * @brief Retrieves the current language code for the UI.
 * @return The language code string, or "EN" if not found.
 */
std::string SettingsManager::getCurrentLanguageCode() const {
    return _currentLanguageCode;
}

// --- Screensaver Settings ---
/**
 * @brief Sets whether the screensaver is enabled.
 * Saves the updated setting to file.
 * @param enabled True to enable the screensaver, false otherwise.
 */
void SettingsManager::setScreensaverEnabled(bool enabled) {
  if (_screensaverEnabled != enabled) {
    _screensaverEnabled = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Screensaver enabled state set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves whether the screensaver is enabled.
 * @param defaultValue A fallback value if the setting is not found.
 * @return True if the screensaver is enabled.
 */
bool SettingsManager::isScreensaverEnabled(bool defaultValue) const {
  return _screensaverEnabled;
}

/**
 * @brief Sets the screensaver activation timeout in seconds.
 * The value is constrained between 5 and 300 seconds. Saves the updated setting to file.
 * @param seconds The new timeout in seconds.
 */
void SettingsManager::setScreensaverTimeout(int seconds) {
  seconds = constrain(seconds, 5, 300); // Constrain value to valid range.
  if (_screensaverTimeoutSec != seconds) {
    _screensaverTimeoutSec = seconds;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Screensaver timeout set to %d seconds.\n", seconds);
  }
}
/**
 * @brief Retrieves the screensaver activation timeout in seconds.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The timeout in seconds.
 */
int SettingsManager::getScreensaverTimeout(int defaultValue) const {
  return _screensaverTimeoutSec;
}

/**
 * @brief Sets the dimmed brightness level for the screensaver.
 * The value is constrained between 0 and 255. Saves the updated setting to file.
 * @param level The new brightness level.
 */
void SettingsManager::setScreensaverBrightness(int level) {
  level = constrain(level, 0, 255); // Constrain value to valid range.
  if (_screensaverBrightness != level) {
    _screensaverBrightness = level;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Screensaver brightness set to %d.\n", level);
  }
}
/**
 * @brief Retrieves the dimmed brightness level for the screensaver.
 * @param defaultValue A fallback value if the setting is not found.
 * @return The brightness level.
 */
int SettingsManager::getScreensaverBrightness(int defaultValue) const {
  return _screensaverBrightness;
}

// --- RFID Settings ---
/**
 * @brief Sets whether RFID functionality is enabled.
 * Saves the updated setting to file.
 * @param enabled True to enable RFID, false otherwise.
 */
void SettingsManager::setRfidEnabled(bool enabled) {
  if (_rfidEnabled != enabled) {
    _rfidEnabled = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: RFID enabled state set to %s.\n", enabled ? "true" : "false");
  }
}
/**
 * @brief Retrieves whether RFID functionality is enabled.
 * @param defaultValue A fallback value if the setting is not found.
 * @return True if RFID is enabled.
 */
bool SettingsManager::isRfidEnabled(bool defaultValue) const {
  return _rfidEnabled;
}

/**
 * @brief Checks if UI click sounds are currently enabled.
 *
 * This method retrieves the current state of the UI click sound setting,
 * determining whether acoustic feedback is provided for interactive elements.
 *
 * @param defaultValue A fallback value provided in the method signature.
 *                     In the current implementation, the internal state
 *                     `_clickSoundEnabled` is directly returned.
 * @return True if UI click sounds are enabled, false otherwise.
 */
bool SettingsManager::isClickSoundEnabled(bool defaultValue) const {
  return _clickSoundEnabled;
}

/**
 * @brief Sets whether UI click sounds are enabled.
 *
 * This method updates the setting that controls whether acoustic feedback
 * (e.g., a click sound) is played for interactive UI elements. If the new state
 * differs from the current state, the setting is updated and saved to
 * persistent storage.
 *
 * @param enabled True to enable UI click sounds, false to disable them.
 */
void SettingsManager::setClickSoundEnabled(bool enabled) {
  if (_clickSoundEnabled != enabled) {
    _clickSoundEnabled = enabled;
    saveSettingsToFile();
    DEBUG_INFO_PRINTF("SettingsManager: Click sound enabled state set to %s.\n", enabled ? "true" : "false");
  }
}

// --- General Persistence ---
/**
 * @brief Forces a save of all current in-memory settings to the settings file.
 * This is useful when multiple settings have been changed and a single write
 * operation is desired.
 * @return True if settings were successfully saved, false otherwise.
 */
bool SettingsManager::forceSave() {
  DEBUG_INFO_PRINTLN("SettingsManager: Forced save initiated.");
  return saveSettingsToFile();
}

/*
// The `formatFileSystem()` method is intentionally commented out for safety
// as formatting the entire filesystem can lead to data loss.
bool SettingsManager::formatFileSystem() {
    if (!_isInitialized) {
        DEBUG_WARN_PRINTLN("SettingsManager: Formatting not possible, filesystem not initialized.");
        // Attempting to format here might also fail if initialization previously did.
        return false;
    }
    DEBUG_WARN_PRINTLN("SettingsManager: WARNING - Formatting LittleFS filesystem...");
    bool success = LittleFS.format();
    if (success) {
        DEBUG_INFO_PRINTLN("SettingsManager: LittleFS successfully formatted.");
        // After formatting, settings should be reset to defaults and saved.
        _savedNetworks.clear();
        _pairedBleDevices.clear();
        // Reset all other member variables to their default constructor values here.
        // For example:
        // _deviceName = "ESP32_Device_Default";
        // _brightnessLevel = 80;
        // ... (etc.)
        return saveSettingsToFile(); // Saves the reset default settings.
    } else {
        DEBUG_ERROR_PRINTLN("SettingsManager: ERROR - LittleFS formatting failed!");
    }
    return success;
}
*/