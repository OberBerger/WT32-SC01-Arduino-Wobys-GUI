#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

/**
 * @file LanguageManager.h
 * @brief Defines the LanguageManager class for handling multi-language support.
 *
 * This class provides functionality for loading, managing, and retrieving
 * language-specific string resources. It interacts with `SettingsManager`
 * to persist language preferences and offers diacritic conversion for
 * specific languages (e.g., Hungarian). It also allows UI elements to
 * register callbacks for language update notifications.
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

#include <string>
#include <functional>
#include <map>
#include <vector> // Required for std::vector in getAvailableLanguages

// Forward declaration of SettingsManager to avoid circular dependencies
class SettingsManager;

// Forward declaration of LanguageInfo struct, defined later
struct LanguageInfo;

/**
 * @brief Manages language-specific string resources for the UI.
 *
 * The LanguageManager handles loading language JSON files, storing strings
 * in a key-value map, providing translated strings based on a key, and
 * managing language update notifications for UI elements. It also supports
 * optional diacritic conversion for improved font compatibility.
 */
class LanguageManager {
public:
    /**
     * @brief Enum to identify available languages.
     * Easily extensible with new languages later.
     */
    enum class Language {
        EN, ///< English language.
        HU  ///< Hungarian language.
    };

    /**
     * @brief Constructs a new LanguageManager object.
     *
     * Initializes internal states. The actual language loading and setup
     * happens in the `init()` method.
     */
    LanguageManager();

    /**
     * @brief Initializes the language manager.
     *
     * This method syncs language files with the filesystem, loads the saved
     * language preference from `SettingsManager`, or defaults to English.
     *
     * @param settingsManager A pointer to the `SettingsManager` instance for
     *                        accessing and saving language settings.
     */
    void init(SettingsManager* settingsManager);

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
    bool setLanguage(Language lang);

    /**
     * @brief Retrieves the currently active language.
     * @return The `Language` enum value of the active language.
     */
    Language getCurrentLanguage() const;

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
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Registers a callback function to be notified when the language changes or
     *        diacritic conversion setting is toggled.
     *
     * @param name A unique name for the callback registration (e.g., "MainUI_update").
     * @param callback The `std::function<void()>` to be called upon update.
     */
    void registerForUpdate(const std::string& name, std::function<void()> callback);

    /**
     * @brief Unregisters a previously registered callback function.
     * @param name The unique name used during registration.
     */
    void unregisterForUpdate(const std::string& name);

    /**
     * @brief Retrieves a list of all available languages with their display names and codes.
     * @return A `std::vector` of `LanguageInfo` structs containing details for each language.
     */
    std::vector<struct LanguageInfo> getAvailableLanguages();

    /**
     * @brief Sets whether Hungarian long diacritic characters (ő, Ő, ű, Ű)
     *        should be converted to their circumflex variants (ô, Ô, û, Û)
     *        due to potential font compatibility issues.
     *
     * Toggling this setting will trigger a call to all registered update callbacks.
     *
     * @param enable `true` to enable diacritic conversion, `false` to disable.
     */
    void setDiacriticConversionEnabled(bool enable);

private:
    SettingsManager* _settingsManager = nullptr;                         ///< Pointer to the settings manager for persistence.
    Language _currentLanguage = Language::EN;                            ///< The currently active language. Defaults to English.
    std::map<std::string, std::string> _stringMap;                       ///< Map storing key-value pairs of string resources for the current language.
    std::map<std::string, std::function<void()>> _updateCallbacks;       ///< Map of registered callbacks to be invoked on language updates.
    bool _enableDiacriticConversion = false;                             ///< Flag to enable/disable Hungarian diacritic conversion.

    /**
     * @brief Synchronizes (writes/updates) language JSON files on LittleFS from embedded assets.
     *
     * This method checks if language files exist on the filesystem and writes
     * or updates them if they are missing or differ in size from the embedded content.
     */
    void _syncLanguageFiles();

    /**
     * @brief Internal method to load a specific language's strings into the `_stringMap`.
     *
     * This method does not update `SettingsManager` or call update callbacks.
     * It is used internally by `init()` and `setLanguage()`.
     *
     * @param lang The `Language` enum value of the language to load.
     * @return `true` if the language was successfully loaded, `false` otherwise.
     */
    bool _loadLanguage(Language lang);

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
    std::string _convertHungarianDiacritics(std::string text);
};

/**
 * @brief Structure to provide information about an available language.
 *
 * Used by `getAvailableLanguages()` to return details for display and selection.
 */
struct LanguageInfo {
    LanguageManager::Language langEnum; ///< The `Language` enum value for the language.
    std::string displayName;            ///< The human-readable name of the language (e.g., "English", "Magyar").
    std::string langCode;               ///< The two-letter ISO 639-1 code for the language (e.g., "EN", "HU").
};

#endif // LANGUAGEMANAGER_H