/**
 * @file LanguageManager.cpp
 * @brief Implementation of the LanguageManager class.
 *
 * This file contains the detailed implementation for managing language-specific
 * string resources, including loading from filesystem, handling language changes,
 * and performing diacritic conversions.
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
 * This software utilizes various third-party open-source components.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations
 * (e.g., source code availability for LGPL components) required for compliance.
 */

#include "LanguageManager.h"
#include "Config.h"           // For DEBUG_PRINT macros
#include "SettingsManager.h"  // For SettingsManager class (forward declared)
#include "LanguageAssets.h"   // For languageAsset and default_xx_json definitions
#include <LittleFS.h>         // For filesystem operations
#include <ArduinoJson.h>      // For JSON parsing
#include <string.h>           // For strlen

/**
 * @brief Constructs a new LanguageManager object.
 *
 * Initializes internal states. The actual language loading and setup
 * happens in the `init()` method.
 */
LanguageManager::LanguageManager() {
  DEBUG_INFO_PRINTLN("LanguageManager constructor called.");
}

/**
 * @brief Initializes the language manager.
 *
 * This method syncs language files with the filesystem, loads the saved
 * language preference from `SettingsManager`, or defaults to English.
 *
 * @param settingsManager A pointer to the `SettingsManager` instance for
 *                        accessing and saving language settings.
 */
void LanguageManager::init(SettingsManager* settingsManager) {
  _settingsManager = settingsManager;
  _syncLanguageFiles();

  if (_settingsManager) {
    std::string savedCode = _settingsManager->getCurrentLanguageCode();
    // Find the Language enum based on the saved code
    for (size_t i = 0; i < languageAssetCount; ++i) {
      const LanguageAsset& asset = languageAssets[i];
      DynamicJsonDocument doc(256);  // Small doc for meta data
      DeserializationError error = deserializeJson(doc, asset.jsonContent);
      if (!error) {
        if (savedCode == (doc["meta"]["code"] | "")) {
          DEBUG_INFO_PRINTF("LanguageManager: Loading saved language with code: %s\n", savedCode.c_str());
          _loadLanguage(asset.languageEnum);
          return;  // Language found and loaded, exit init
        }
      } else {
        DEBUG_ERROR_PRINTF("LanguageManager: Failed to parse JSON metadata for asset at %s: %s\n", asset.filePath, error.c_str());
      }
    }
  }
  // If no settings manager, or saved language not found/loaded, default to English
  DEBUG_INFO_PRINTLN("LanguageManager: Saved language not found or settings manager unavailable. Defaulting to English.");
  _loadLanguage(Language::EN);
}

/**
 * @brief Synchronizes (writes/updates) language JSON files on LittleFS from embedded assets.
 *
 * This method checks if language files exist on the filesystem and writes
 * or updates them if they are missing or differ in size from the embedded content.
 */
void LanguageManager::_syncLanguageFiles() {
  DEBUG_INFO_PRINTLN("LanguageManager: Syncing/updating language files (checking for changes)...");

  for (size_t i = 0; i < languageAssetCount; ++i) {
    const LanguageAsset& asset = languageAssets[i];

    bool shouldWriteFile = true;  // Default to writing the file

    if (LittleFS.exists(asset.filePath)) {
      File existingFile = LittleFS.open(asset.filePath, "r");
      if (existingFile) {
        // Check file size. If different, rewrite.
        if (existingFile.size() == strlen(asset.jsonContent)) {
          shouldWriteFile = false;  // Size matches, assume content is also identical.
          DEBUG_TRACE_PRINTF("LanguageManager: '%s' exists, size matches. Skipping write.\n", asset.filePath);
        } else {
          DEBUG_INFO_PRINTF("LanguageManager: '%s' exists, but size differs (FS:%zu vs Asset:%zu). Rewriting.\n", asset.filePath, existingFile.size(), strlen(asset.jsonContent));
        }
        existingFile.close();
      } else {
        // Could not open the file, but it exists -> problem, rewrite.
        DEBUG_WARN_PRINTF("LanguageManager: '%s' exists but cannot be opened for reading. Rewriting.\n", asset.filePath);
      }
    } else {
      DEBUG_INFO_PRINTF("LanguageManager: '%s' does not exist. Writing file.\n", asset.filePath);
    }

    if (shouldWriteFile) {
      File langFile = LittleFS.open(asset.filePath, "w");  // Open in write mode, will truncate or create
      if (langFile) {
        langFile.print(asset.jsonContent);
        langFile.close();
        DEBUG_INFO_PRINTF("LanguageManager: Successfully wrote '%s'.\n", asset.filePath);
      } else {
        DEBUG_ERROR_PRINTF("LanguageManager: ERROR - Failed to write '%s'!\n", asset.filePath);
      }
    }
  }
}

/**
 * @brief Sets the currently active language.
 *
 * Loads the specified language's strings and updates the saved language
 * preference in `SettingsManager`. It also notifies all registered callbacks
 * about the language change.
 *
 * @param lang The `Language` enum value to set as active.
 * @return `true` if the language was successfully set, `false` otherwise.
 */
bool LanguageManager::setLanguage(Language lang) {
  if (!_loadLanguage(lang)) {
    DEBUG_ERROR_PRINTF("LanguageManager: Failed to load language %d.\n", static_cast<int>(lang));
    return false;
  }

  if (_settingsManager) {
    // Find its code and save it
    for (size_t i = 0; i < languageAssetCount; ++i) {
      const LanguageAsset& asset = languageAssets[i];
      if (asset.languageEnum == lang) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, asset.jsonContent);
        if (!error) {
          _settingsManager->setCurrentLanguageCode(doc["meta"]["code"] | "EN");
          DEBUG_INFO_PRINTF("LanguageManager: Saved language code: %s\n", std::string(doc["meta"]["code"] | "EN").c_str());
        } else {
          DEBUG_ERROR_PRINTF("LanguageManager: Failed to parse JSON for saving language code: %s\n", error.c_str());
        }
        break;
      }
    }
  }

  // Notify all registered callbacks that the language has been updated
  for (const auto& pair : _updateCallbacks) {
    if (pair.second) {  // Null pointer check for function object
      pair.second();
    }
  }
  DEBUG_INFO_PRINTF("LanguageManager: Language set to %d and callbacks invoked.\n", static_cast<int>(lang));
  return true;
}

/**
 * @brief Retrieves the currently active language.
 * @return The `Language` enum value of the active language.
 */
LanguageManager::Language LanguageManager::getCurrentLanguage() const {
  return _currentLanguage;
}

/**
 * @brief Retrieves a string resource based on its key.
 *
 * If the key is not found, it returns `defaultValue`. If `defaultValue` is also
 * empty, it returns the key enclosed in brackets `[key]` for debugging.
 * Applies diacritic conversion if enabled.
 *
 * @param key The unique identifier key for the string (e.g., "WIFI_SETTINGS_TITLE").
 * @param defaultValue An optional default string to return if the key is not found. Defaults to an empty string.
 * @return The translated string, or the `defaultValue`, or `[key]` if not found.
 */
std::string LanguageManager::getString(const std::string& key, const std::string& defaultValue) {
  std::string result;
  auto it = _stringMap.find(key);
  if (it != _stringMap.end()) {
    result = it->second;  // Found, return the value
  } else if (!defaultValue.empty()) {
    result = defaultValue;
    DEBUG_INFO_PRINTF("LanguageManager: Key '%s' not found, using default value: '%s'.\n", key.c_str(), defaultValue.c_str());
  } else {
    result = "[" + key + "]";  // Return key for debugging
    DEBUG_ERROR_PRINTF("LanguageManager: Key '%s' not found, no default value provided.\n", key.c_str());
  }

  // Apply diacritic conversion if enabled
  if (_enableDiacriticConversion) {
    result = _convertHungarianDiacritics(result);
  }

  return result;
}

/**
 * @brief Registers a callback function to be notified when the language changes or
 *        diacritic conversion setting is toggled.
 *
 * @param name A unique name for the callback registration (e.g., "MainUI_update").
 * @param callback The `std::function<void()>` to be called upon update.
 */
void LanguageManager::registerForUpdate(const std::string& name, std::function<void()> callback) {
  _updateCallbacks[name] = callback;
  DEBUG_INFO_PRINTF("LanguageManager: Callback registered: '%s'. Total callbacks: %zu\n", name.c_str(), _updateCallbacks.size());
}

/**
 * @brief Unregisters a previously registered callback function.
 * @param name The unique name used during registration.
 */
void LanguageManager::unregisterForUpdate(const std::string& name) {
  size_t erased_count = _updateCallbacks.erase(name);
  if (erased_count > 0) {
    DEBUG_INFO_PRINTF("LanguageManager: Callback unregistered: '%s'. Remaining: %zu\n", name.c_str(), _updateCallbacks.size());
  } else {
    DEBUG_WARN_PRINTF("LanguageManager: Attempted to unregister unknown callback: '%s'.\n", name.c_str());
  }
}

/**
 * @brief Sets whether Hungarian long diacritic characters (ő, Ő, ű, Ű)
 *        should be converted to their circumflex variants (ô, Ô, û, Û)
 *        due to potential font compatibility issues.
 *
 * Toggling this setting will trigger a call to all registered update callbacks.
 *
 * @param enable `true` to enable diacritic conversion, `false` to disable.
 */
void LanguageManager::setDiacriticConversionEnabled(bool enable) {
  if (_enableDiacriticConversion != enable) {
    _enableDiacriticConversion = enable;
    DEBUG_INFO_PRINTF("LanguageManager: Diacritic conversion set to: %s. Invoking callbacks.\n", enable ? "enabled" : "disabled");
    // It's good practice to immediately update the UI if this setting changes
    // (even if the language itself doesn't change), so invoke callbacks.
    for (const auto& pair : _updateCallbacks) {
      if (pair.second) {  // Null pointer check
        pair.second();
      }
    }
  } else {
    DEBUG_TRACE_PRINTF("LanguageManager: Diacritic conversion already %s. No change.\n", enable ? "enabled" : "disabled");
  }
}

/**
 * @brief Internal method to load a specific language's strings into the `_stringMap`.
 *
 * This method does not update `SettingsManager` or call update callbacks.
 * It is used internally by `init()` and `setLanguage()`.
 *
 * @param lang The `Language` enum value of the language to load.
 * @return `true` if the language was successfully loaded, `false` otherwise.
 */
bool LanguageManager::_loadLanguage(Language lang) {
  const char* langFilePath = nullptr;
  const char* langJsonContent = nullptr;  // Also get embedded content
  for (size_t i = 0; i < languageAssetCount; ++i) {
    if (languageAssets[i].languageEnum == lang) {
      langFilePath = languageAssets[i].filePath;
      langJsonContent = languageAssets[i].jsonContent;  // Store embedded content
      break;
    }
  }
  if (langFilePath == nullptr || langJsonContent == nullptr) {
    DEBUG_ERROR_PRINTF("LanguageManager: Language file path or embedded content not found for language %d.\n", static_cast<int>(lang));
    return false;
  }

  // Try to open from LittleFS first
  File langFile = LittleFS.open(langFilePath, "r");
  DynamicJsonDocument doc(2048);
  DeserializationError error = DeserializationError::Ok;

  if (langFile) {
    error = deserializeJson(doc, langFile);
    langFile.close();
    if (error) {
      DEBUG_ERROR_PRINTF("LanguageManager: Failed to parse JSON from file '%s': %s. Attempting to load from embedded content.\n", langFilePath, error.c_str());
      // If file parsing fails, try embedded content
      error = deserializeJson(doc, langJsonContent);
    }
  } else {
    DEBUG_WARN_PRINTF("LanguageManager: Failed to open language file '%s'. Attempting to load from embedded content.\n", langFilePath);
    // If file can't be opened, load from embedded content
    error = deserializeJson(doc, langJsonContent);
  }

  if (error) {
    DEBUG_ERROR_PRINTF("LanguageManager: Failed to parse JSON from embedded content for language %d: %s\n", static_cast<int>(lang), error.c_str());
    return false;
  }

  JsonObject stringsObj = doc["strings"];
  if (stringsObj) {
    _stringMap.clear();
    for (JsonPair kv : stringsObj) {
      _stringMap[kv.key().c_str()] = kv.value().as<std::string>();
    }
    _currentLanguage = lang;
    DEBUG_INFO_PRINTF("LanguageManager: Successfully loaded language %d ('%s') with %zu strings.\n", static_cast<int>(lang), std::string(doc["meta"]["name"] | "Unknown").c_str(), _stringMap.size());
    return true;
  }

  DEBUG_ERROR_PRINTF("LanguageManager: 'strings' object not found in JSON for language %d.\n", static_cast<int>(lang));
  return false;
}

/**
 * @brief Converts specific Hungarian long diacritic characters (ő, Ő, ű, Ű)
 *        to their circumflex counterparts (ô, Ô, û, Û) within a given string.
 *
 * This conversion is typically performed for font compatibility reasons,
 * where the original Hungarian characters might not be rendered correctly.
 * This method operates on UTF-8 byte sequences.
 *
 * @param text The `std::string` to convert.
 * @return The `std::string` with converted diacritics.
 */
std::string LanguageManager::_convertHungarianDiacritics(std::string text) {
  // UTF-8 encoded characters:
  // ő = 0xC5 0x91
  // Ő = 0xC5 0x90
  // ű = 0xC5 0xB1
  // Ű = 0xC5 0xB0

  // Target characters (circumflex variants, if the font supports these in place of űŰőŐ):
  // ô = 0xC3 0xB4
  // Ô = 0xC3 0x94
  // û = 0xC3 0xBB
  // Û = 0xC3 0x9B

  // Replacements:
  // 'ő' (UTF-8: 0xC5 0x91) -> 'ô' (UTF-8: 0xC3 0xB4)
  // 'Ő' (UTF-8: 0xC5 0x90) -> 'Ô' (UTF-8: 0xC3 0x94)
  // 'ű' (UTF-8: 0xC5 0xB1) -> 'û' (UTF-8: 0xC3 0xBB)
  // 'Ű' (UTF-8: 0xC5 0xB0) -> 'Û' (UTF-8: 0xC3 0x9B)

  // Important: std::string::replace works on *bytes*, not UTF-8 characters.
  // Therefore, we must directly replace the multi-byte sequences.

  // Conversion is only relevant for the Hungarian language, but the enable switch already handles this.

  size_t pos = 0;
  while ((pos = text.find("\xC5\x91", pos)) != std::string::npos) {  // ő
    text.replace(pos, 2, "\xC3\xB4");                                // ô
    pos += 2;                                                        // Advance position by length of replacement
  }
  pos = 0;
  while ((pos = text.find("\xC5\x90", pos)) != std::string::npos) {  // Ő
    text.replace(pos, 2, "\xC3\x94");                                // Ô
    pos += 2;                                                        // Advance position by length of replacement
  }
  pos = 0;
  while ((pos = text.find("\xC5\xB1", pos)) != std::string::npos) {  // ű
    text.replace(pos, 2, "\xC3\xBB");                                // û
    pos += 2;                                                        // Advance position by length of replacement
  }
  pos = 0;
  while ((pos = text.find("\xC5\xB0", pos)) != std::string::npos) {  // Ű
    text.replace(pos, 2, "\xC3\x9B");                                // Û
    pos += 2;                                                        // Advance position by length of replacement
  }

  return text;
}

/**
 * @brief Retrieves a list of all available languages with their display names and codes.
 * @return A `std::vector` of `LanguageInfo` structs containing details for each language.
 */
std::vector<LanguageInfo> LanguageManager::getAvailableLanguages() {
  std::vector<LanguageInfo> languages;
  for (size_t i = 0; i < languageAssetCount; ++i) {  // Módosítás itt
    const LanguageAsset& asset = languageAssets[i];  // Módosítás itt
    DynamicJsonDocument doc(256);                    // Small doc for meta data
    DeserializationError error = deserializeJson(doc, asset.jsonContent);
    if (!error) {
      LanguageInfo info;
      info.langEnum = asset.languageEnum;
      info.displayName = doc["meta"]["name"] | "Unknown";
      info.langCode = doc["meta"]["code"] | "";
      languages.push_back(info);
    } else {
      DEBUG_ERROR_PRINTF("LanguageManager: Failed to parse JSON metadata for available language %d: %s\n", static_cast<int>(asset.languageEnum), error.c_str());
    }
  }
  return languages;
}