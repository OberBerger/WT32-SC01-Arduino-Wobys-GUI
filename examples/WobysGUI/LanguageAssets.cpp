/**
 * @file LanguageAssets.cpp
 * @brief Defines the raw string literals and arrays for managing language-specific assets.
 *
 * This file contains the actual definitions of the `default_en_json` and `default_hu_json`
 * string literals, as well as the `languageAssets` array and `languageAssetCount` variable.
 * These definitions provide the embedded, default language data for the application,
 * complementing the declarations in `LanguageAssets.h`.
 *
 * @version 1.0.8
 * @date 2025-09-10
 * @author György Oberländer
 * @contact gyorgy.oberlander@gmail.com
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

#include "LanguageAssets.h" // Include the header for declarations

/**
 * @brief Default English language JSON content.
 *
 * This R-string literal contains all English strings used in the UI,
 * along with metadata like language name and code.
 */
const char* default_en_json = R"({
  "meta": {
    "name": "English",
    "code": "EN"
  },
  "strings": {
    "WIFI_SETTINGS_TITLE": "Wi-Fi Settings",
    "WIFI_TOGGLE_ON": "ON",
    "WIFI_TOGGLE_OFF": "OFF",
    "SCAN_BUTTON": "Scan",
    "STATUS_DISABLED": "Wi-Fi disabled.",
    "STATUS_SCANNING": "Scanning networks...",
    "STATUS_ENABLED_DISCONNECTED": "Wi-Fi ON, disconnected.",
    "STATUS_ENABLING": "Enabling Wi-Fi...",
    "STATUS_CONNECTING": "Connecting: ",
    "STATUS_CONNECTED": "Connected: ",
    "STATUS_IP_ADDRESS": " (IP: ",
    "STATUS_CONNECTION_FAILED": "Connection failed: ",
    "STATUS_DISABLING": "Disabling Wi-Fi...",
    "STATUS_UNKNOWN_STATE": "Unknown Wi-Fi state.",
    "STATUS_CONNECTING_ATTEMPT": "Connecting attempt: ",
    "STATUS_CANNOT_START_CONNECT": "Connection cannot be started.",
    "STATUS_PASS_INCORRECT": "Incorrect password: ",
    "STATUS_NO_NETWORKS_FOUND": "No networks found.",
    "STATUS_SCAN_FAILED": "Network scan failed.",
    "STATUS_WIFI_ACTIVE": "Wi-Fi active.",
    "KEYBOARD_PASSWORD_TITLE": "Password:",
    "WIFI_DELETE_DIALOG_QUESTION": "Are you sure you want to delete password for?",
    "WIFI_DELETE_DIALOG_YES": "Yes",
    "WIFI_DELETE_DIALOG_NO": "No",
    "ICON_LOCK_OPEN": "\u00CB",
    "ICON_LOCK_CLOSED": "\u00CA",
    "TEXT_DELETE_ACTION": "X",
    "TEXT_NETWORKS_FOUND_COUNT": " networks found.",
    "PANEL_BUTTON_BACK": "\u00FA",
    "STATUS_AMBIGUOUS_DATA": "Error: Ambiguous data.",
    "STATUS_DISABLED_FOR_CONNECT": "Wi-Fi disabled for connection.",
    "STATUS_PASS_PROMPT": "Password: ",
    "STATUS_PASS_DELETED": "Password deleted.",
    "STATUS_DELETION_FAILED": "Deletion failed.",
    "STATUS_CONNECTING_SAVED": "Connecting with saved password: ",
    "TEXT_LIST_CLEARED": " List cleared.",
    "WIFI_STATUS_ALREADY_CONNECTED": "Already connected to this network.",

    "PANEL_BUTTON_POWER_OFF": "\u00EB",
    "PANEL_BUTTON_WIFI": "\u00F7",
    "PANEL_BUTTON_BLUETOOTH": "\u005E",
    "PANEL_BUTTON_SETTINGS": "\u0081",

    "MESSAGE_RFID_UID": "RFID UID: ",

    "SETTINGS_SCREEN_TITLE": "Settings",
    "SETTINGS_LANG_CATEGORY": "Language",
    "SETTINGS_DISPLAY_CATEGORY": "Display",
    "SETTINGS_SCREENSAVER_CATEGORY": "Screensaver",
    "SETTINGS_SOUND_CATEGORY": "Sound",
    "SETTINGS_RFID_CATEGORY": "RFID",
    "SETTINGS_BATTERY_CATEGORY": "Battery",
    "SETTINGS_SCREENSAVER_ENABLE": "Enable Screensaver",
    "SETTINGS_SOUND_ENABLE": "Enable Sound",
    "SETTINGS_RFID_ENABLE": "Enable RFID",

    "GENERAL_ON": "ON",
    "GENERAL_OFF": "OFF",
    "GENERAL_YES": "Yes",
    "GENERAL_NO": "No",

    "SETTINGS_SCREENSAVER_TIMEOUT": "Timeout",
    "SETTINGS_SCREENSAVER_BRIGHTNESS": "Brightness",
    "SETTINGS_DISPLAY_BRIGHTNESS": "Brightness",
    "SETTINGS_SOUND_VOLUME": "Volume",

    "MAIN_STATUS_LABEL": "UI Demo",
    "MAIN_COLOR_MODE_TOGGLE": "Color Mode",
    "MAIN_LIST_CONTROL_TOGGLE": "List Control",
    "MAIN_ADD_ITEM_BUTTON": "+ Item",
    "MAIN_GRID_TOGGLE_ON": "Grid ON",
    "MAIN_GRID_TOGGLE_OFF": "Grid OFF",
    "MAIN_ROTATE_BUTTON": "Rotate",
    "KEYBOARD_TEXT_INPUT_TITLE": "Text Input:",
    "MAIN_KEYBOARD_ADD_ITEM_TITLE": "New list item:",
    "MAIN_CONFIRM_DELETE_QUESTION": "Are you sure you want to delete this item?",
    "MAIN_CONFIRM_DELETE_QUESTION_PORTRAIT": "Confirm deletion\nof this item?",
    "MAIN_CONFIRM_YES": "Yes",
    "MAIN_CONFIRM_NO": "No",
    "STATUS_KEYBOARD_INPUT_CANCELLED": "Keyboard input cancelled.",
    "STATUS_ITEM_ADDED": "Item added: ",
    "STATUS_EMPTY_ITEM_NOT_ADDED": "Empty item cannot be added.",
    "STATUS_ITEM_DELETED": "Item deleted: ",
    "STATUS_DELETION_CANCELLED": "List deletion cancelled.",
    "STATUS_ITEM_SELECTED": "Selected item: ",
    "SEEKBAR_VALUE_TOGGLE": "Seekbar Value Toggled",
    "SEEKBAR_TITLE_TOGGLE": "Seekbar Title Toggled",
    "MAIN_CONFIRM_ADD_RFID_QUESTION": "Do you want to add this RFID to the list?",
    "MAIN_CONFIRM_ADD_RFID_QUESTION_PORTRAIT": "Add this RFID\nto list?",
    "STATUS_RFID_ADDED": "RFID added: ",
    "STATUS_RFID_ADD_CANCELLED": "RFID add cancelled.",
    
    "BLE_SETTINGS_TITLE": "BT Settings",
    "BLE_NAME_BUTTON": "Name",
    "BLE_STATUS_DISABLED": "Bluetooth disabled.",
    "BLE_STATUS_SCAN_PENDING": "Device search pending...",
    "BLE_STATUS_SCANNING": "Scanning in progress...",
    "BLE_STATUS_CONNECTING": "Connecting",
    "BLE_STATUS_CONNECTED": "Connected",
    "BLE_STATUS_DISCONNECTED": "disconnected",
    "BLE_STATUS_CONNECTION_FAILED": "Connection failed",
    "BLE_STATUS_UNKNOWN_STATE": "Unknown state.",
    "BLE_STATUS_SCAN_FAILED": "Scan failed.",
    "BLE_STATUS_PIN_NOT_ACTIVE": "PIN function not active.",
    "BLE_STATUS_DEVICE_NAME_SET": "Device name: ",
    "BLE_STATUS_OFFLINE_DEVICE": "This device is not nearby.",
    "BLE_STATUS_CONNECT_NOT_POSSIBLE": "Connection not possible.",
    "BLE_STATUS_DISCONNECTING": "Disconnecting...",
    "BLE_CONFIRM_DISCONNECT_QUESTION": "Are you sure you want to disconnect?",
    "BLE_STATUS_DEVICE_DELETED": "Device deleted.",
    "BLE_STATUS_DELETION_IN_PROGRESS": "Deletion in progress...",
    "BLE_STATUS_DELETION_ERROR": "Deletion error",
    "BLE_STATUS_DEVICE_DELETED_ERROR": "Error: Deletion failed <empty UUID>.",
    "BLE_STATUS_SCAN_NOT_POSSIBLE": "Scan not possible.",
    "BLE_STATUS_ALREADY_CONNECTED": "Already connected to this device.",
    "BLE_NAME_KEYBOARD_TITLE": "Device name:",
    "BLE_DEVICE_NO_NAME": "<no name>",
    "KEYBOARD_PIN_TITLE": "PIN:",
    "ICON_BLE_CONNECTED": "\u00C6",
    "ICON_BLE_DISCONNECTED": "\u00C5",

    "POWER_WARN_LOW_BATT": "Battery low! System will shut down soon.",
    "POWER_INFO_SHUTDOWN_INIT": "Shutting down system...",

    "INIT_FS_ERROR": "FS Init Error!",
    "INIT_SD_ERROR": "SD Init Error!",
    "INIT_POWER_MGR_FAILED": "PowerMgr Init Failed!",
    "INIT_SETTINGS_MGR_FAILED": "SettingsMgr Init Failed!",
    "INIT_LANG_MGR_FAILED": "LangMgr Init Failed!",
    "INIT_RFID_MGR_FAILED": "RFIDMgr Init Failed!",
    "INIT_WIFI_MGR_FAILED": "WifiMgr Init Failed!",
    "INIT_TIME_MGR_FAILED": "TimeMgr Init Failed!",
    "INIT_BLE_MGR_FAILED": "BLEMgr Init Failed!",
    "INIT_SSAVER_MGR_FAILED": "SSaverMgr Init Failed!",
    "INIT_AUDIO_MGR_FAILED": "AudioMgr Init Failed!",
    "INIT_TIME_UI_ERROR": "Time UI Init Error!",
    "INIT_RFID_UI_ERROR": "RFID UI Init Error!",
    "INIT_SPEAKER_UI_ERROR": "Speaker UI Init Error!",
    "INIT_BT_UI_ERROR": "BT UI Init Error!",
    "INIT_WIFI_UI_ERROR": "WiFi UI Init Error!",
    "INIT_SD_UI_ERROR_UI": "SD UI Init Error!",
    "INIT_BATTERY_UI_ERROR": "Battery UI Init Error!",
    "BOOT_IMAGE_NOT_FOUND": "Boot image not found!",

    "SHUTDOWN_STATUS_SAVE_SETTINGS": "Saving settings...",
    "SHUTDOWN_STATUS_STOP_WIFI": "Stopping Wi-Fi...",
    "SHUTDOWN_STATUS_STOP_BT": "Stopping Bluetooth...",
    "SHUTDOWN_STATUS_STOP_AUDIO": "Stopping Audio...",
    "SHUTDOWN_STATUS_STOP_RFID": "Stopping RFID...",
    "SHUTDOWN_STATUS_STOP_SSAVER": "Stopping Screensaver...",
    "SHUTDOWN_STATUS_UNMOUNT_FS": "Unmounting FS...",
    "SHUTDOWN_STATUS_UNMOUNT_SD": "Unmounting SD card...",
    "SHUTDOWN_MESSAGE_GOODBYE": "Goodbye!",

    "DEMO_MODE_ACTIVATED": "Demo Mode Activated",
    "DEMO_UI_LIMIT_REACHED": "Demo: UI limit reached!",
    "DEMO_WIFI_SCAN_LIMIT_REACHED": "Demo: Wi-Fi scan limit reached!",
    "DEMO_BLE_SCAN_LIMIT_REACHED": "Demo: BLE scan limit reached!",
    "DEMO_BLE_SCAN_COUNT": "Demo: BLE scans",
    "DEMO_SCREEN_DRAW_LIMIT_REACHED": "Demo: Screen draw limit reached!"
  }
})";

/**
 * @brief Default Hungarian language JSON content.
 *
 * This R-string literal contains all Hungarian strings used in the UI,
 * along with metadata like language name and code.
 */
const char* default_hu_json = R"({
  "meta": {
    "name": "Magyar",
    "code": "HU"
  },
  "strings": {
    "WIFI_SETTINGS_TITLE": "WiFi Beállítások",
    "WIFI_TOGGLE_ON": "BE",
    "WIFI_TOGGLE_OFF": "KI",
    "SCAN_BUTTON": "Keresés",
    "STATUS_DISABLED": "WiFi kikapcsolva.",
    "STATUS_SCANNING": "Hálózatok keresése...",
    "STATUS_ENABLED_DISCONNECTED": "WiFi BE, nincs csatlakozva.",
    "STATUS_ENABLING": "WiFi bekapcsolása...",
    "STATUS_CONNECTING": "Csatlakozás: ",
    "STATUS_CONNECTED": "Csatlakoztatva: ",
    "STATUS_IP_ADDRESS": " (IP: ",
    "STATUS_CONNECTION_FAILED": "Csatlakozás sikertelen: ",
    "STATUS_DISABLING": "WiFi kikapcsolása...",
    "STATUS_UNKNOWN_STATE": "Ismeretlen WiFi állapot.",
    "STATUS_CONNECTING_ATTEMPT": "Csatlakozás kísérlete: ",
    "STATUS_CANNOT_START_CONNECT": "Csatlakozás nem indítható.",
    "STATUS_PASS_INCORRECT": "Hibás jelszó: ",
    "STATUS_NO_NETWORKS_FOUND": "Nincs hálózat a közelben.",
    "STATUS_SCAN_FAILED": "Hálózatkeresés sikertelen.",
    "STATUS_WIFI_ACTIVE": "WiFi aktív.",
    "KEYBOARD_PASSWORD_TITLE": "Jelszó:",
    "WIFI_DELETE_DIALOG_QUESTION": "Biztosan törli a jelszót ehhez?",
    "WIFI_DELETE_DIALOG_YES": "Igen",
    "WIFI_DELETE_DIALOG_NO": "Nem",
    "ICON_LOCK_OPEN": "\u00CB",
    "ICON_LOCK_CLOSED": "\u00CA",
    "TEXT_DELETE_ACTION": "X",
    "TEXT_NETWORKS_FOUND_COUNT": " hálózat található.",
    "PANEL_BUTTON_BACK": "\u00FA",
    "STATUS_AMBIGUOUS_DATA": "Hiba: Érvénytelen adat.",
    "STATUS_DISABLED_FOR_CONNECT": "WiFi kikapcsolva csatlakozáshoz.",
    "STATUS_PASS_PROMPT": "Jelszó: ",
    "STATUS_PASS_DELETED": "Jelszó törölve.",
    "STATUS_DELETION_FAILED": "Törlés sikertelen.",
    "STATUS_CONNECTING_SAVED": "Csatlakozás mentett jelszóval: ",
    "TEXT_LIST_CLEARED": " Lista törölve.",
    "WIFI_STATUS_ALREADY_CONNECTED": "Már csatlakozva ehhez a hálózathoz.",

    "PANEL_BUTTON_POWER_OFF": "\u00EB",
    "PANEL_BUTTON_WIFI": "\u00F7",
    "PANEL_BUTTON_BLUETOOTH": "\u005E",
    "PANEL_BUTTON_SETTINGS": "\u0081",

    "MESSAGE_RFID_UID": "RFID UID: ",

    "SETTINGS_SCREEN_TITLE": "Beállítások",
    "SETTINGS_LANG_CATEGORY": "Nyelv",
    "SETTINGS_DISPLAY_CATEGORY": "Képernyő",
    "SETTINGS_SCREENSAVER_CATEGORY": "Képernyővédő",
    "SETTINGS_SOUND_CATEGORY": "Hang",
    "SETTINGS_RFID_CATEGORY": "RFID",
    "SETTINGS_BATTERY_CATEGORY": "Akkumulátor",
    "SETTINGS_SCREENSAVER_ENABLE": "Engedélyezés",
    "SETTINGS_SOUND_ENABLE": "Engedélyezés",
    "SETTINGS_RFID_ENABLE": "Engedélyezés",

    "GENERAL_ON": "BE",
    "GENERAL_OFF": "KI",
    "GENERAL_YES": "Igen",
    "GENERAL_NO": "Nem",

    "SETTINGS_SCREENSAVER_TIMEOUT": "Idő",
    "SETTINGS_SCREENSAVER_BRIGHTNESS": "Fényerő",
    "SETTINGS_DISPLAY_BRIGHTNESS": "Fényerő",
    "SETTINGS_SOUND_VOLUME": "Hangerő",

    "MAIN_STATUS_LABEL": "UI Demó",
    "MAIN_COLOR_MODE_TOGGLE": "Színmód",
    "MAIN_LIST_CONTROL_TOGGLE": "Lista vezérlés",
    "MAIN_ADD_ITEM_BUTTON": "+ Elem",
    "MAIN_GRID_TOGGLE_ON": "Grid BE",
    "MAIN_GRID_TOGGLE_OFF": "Grid KI",
    "MAIN_ROTATE_BUTTON": "Forgatás",
    "KEYBOARD_TEXT_INPUT_TITLE": "Szöveg bevitel:",
    "MAIN_KEYBOARD_ADD_ITEM_TITLE": "Új listaelem:",
    "MAIN_CONFIRM_DELETE_QUESTION": "Biztosan törli ezt az elemet?",
    "MAIN_CONFIRM_DELETE_QUESTION_PORTRAIT": "Biztosan törli\nezt az elemet?",
    "MAIN_CONFIRM_YES": "Igen",
    "MAIN_CONFIRM_NO": "Nem",
    "STATUS_KEYBOARD_INPUT_CANCELLED": "Billentyűzet bevitel megszakítva.",
    "STATUS_ITEM_ADDED": "Elem hozzáadva: ",
    "STATUS_EMPTY_ITEM_NOT_ADDED": "Üres elem nem adható hozzá.",
    "STATUS_ITEM_DELETED": "Elem törölve: ",
    "STATUS_DELETION_CANCELLED": "Lista törlése megszakítva.",
    "STATUS_ITEM_SELECTED": "Kiválasztott elem: ",
    "SEEKBAR_VALUE_TOGGLE": "Seekbar érték átkapcsolva",
    "SEEKBAR_TITLE_TOGGLE": "Seekbar felirat átkapcsolva",
    "MAIN_CONFIRM_ADD_RFID_QUESTION": "Hozzáadja ezt az RFID-t a listához?",
    "MAIN_CONFIRM_ADD_RFID_QUESTION_PORTRAIT": "Hozzáadja\nezt az RFID-t?",
    "STATUS_RFID_ADDED": "RFID hozzáadva: ",
    "STATUS_RFID_ADD_CANCELLED": "RFID hozzáadás megszakítva.",

    "BLE_SETTINGS_TITLE": "BT Beállítások",
    "BLE_NAME_BUTTON": "Név",
    "BLE_STATUS_DISABLED": "Bluetooth kikapcsolva.",
    "BLE_STATUS_SCAN_PENDING": "Eszközkeresés függőben...",
    "BLE_STATUS_SCANNING": "Keresés folyamatban...",
    "BLE_STATUS_CONNECTING": "Csatlakozás",
    "BLE_STATUS_CONNECTED": "Csatlakoztatva",
    "BLE_STATUS_DISCONNECTED": "Nincs csatlakoztatva",
    "BLE_STATUS_CONNECTION_FAILED": "Csatlakozás sikertelen",
    "BLE_STATUS_UNKNOWN_STATE": "Ismeretlen állapot.",
    "BLE_STATUS_SCAN_FAILED": "Keresés sikertelen.",
    "BLE_STATUS_PIN_NOT_ACTIVE": "PIN funkció nem aktív.",
    "BLE_STATUS_DEVICE_NAME_SET": "Eszköz neve: ",
    "BLE_STATUS_OFFLINE_DEVICE": "Ez az eszköz nincs a közelben.",
    "BLE_STATUS_CONNECT_NOT_POSSIBLE": "Csatlakozás nem lehetséges.",
    "BLE_STATUS_DISCONNECTING": "Bontás...",
    "BLE_CONFIRM_DISCONNECT_QUESTION": "Biztosan le kívánja választani?",
    "BLE_STATUS_DEVICE_DELETED": "Eszköz törölve.",
    "BLE_STATUS_DELETION_IN_PROGRESS": "Törlés folyamatban...",
    "BLE_STATUS_DELETION_ERROR": "Törlési hiba",
    "BLE_STATUS_DEVICE_DELETED_ERROR": "Hiba: Törlés sikertelen <üres UUID>.",
    "BLE_STATUS_SCAN_NOT_POSSIBLE": "Keresés nem indítható.",
    "BLE_STATUS_ALREADY_CONNECTED": "Az eszköz már csatlakoztatva van.",
    "BLE_NAME_KEYBOARD_TITLE": "Eszköz neve:",
    "BLE_DEVICE_NO_NAME": "<névtelen>",
    "KEYBOARD_PIN_TITLE": "PIN:",
    "ICON_BLE_CONNECTED": "\u00C6",
    "ICON_BLE_DISCONNECTED": "\u00C5",
    
    "POWER_WARN_LOW_BATT": "Akkumulátor merülőben! Rendszer hamarosan leáll.",
    "POWER_INFO_SHUTDOWN_INIT": "Rendszer leállítása...",

    "INIT_FS_ERROR": "FS Init Hiba!",
    "INIT_SD_ERROR": "SD Init Hiba!",
    "INIT_POWER_MGR_FAILED": "PowerMgr Inicializálás Sikertelen!",
    "INIT_SETTINGS_MGR_FAILED": "SettingsMgr Inicializálás Sikertelen!",
    "INIT_LANG_MGR_FAILED": "LangMgr Inicializálás Sikertelen!",
    "INIT_RFID_MGR_FAILED": "RFIDMgr Inicializálás Sikertelen!",
    "INIT_WIFI_MGR_FAILED": "WifiMgr Inicializálás Sikertelen!",
    "INIT_TIME_MGR_FAILED": "TimeMgr Inicializálás Sikertelen!",
    "INIT_BLE_MGR_FAILED": "BLEMgr Inicializálás Sikertelen!",
    "INIT_SSAVER_MGR_FAILED": "Képernyővédő Mgr Inicializálás Sikertelen!",
    "INIT_AUDIO_MGR_FAILED": "AudioMgr Inicializálás Sikertelen!",
    "INIT_TIME_UI_ERROR": "Idő UI Init Hiba!",
    "INIT_RFID_UI_ERROR": "RFID UI Init Hiba!",
    "INIT_SPEAKER_UI_ERROR": "Hangszóró UI Init Hiba!",
    "INIT_BT_UI_ERROR": "BT UI Init Hiba!",
    "INIT_WIFI_UI_ERROR": "WiFi UI Init Hiba!",
    "INIT_SD_UI_ERROR_UI": "SD UI Init Hiba!",
    "INIT_BATTERY_UI_ERROR": "Akkumulátor UI Init Hiba!",
    "BOOT_IMAGE_NOT_FOUND": "Indító kép nem található!",

    "SHUTDOWN_STATUS_SAVE_SETTINGS": "Beállítások mentése...",
    "SHUTDOWN_STATUS_STOP_WIFI": "WiFi leállítása...",
    "SHUTDOWN_STATUS_STOP_BT": "Bluetooth leállítása...",
    "SHUTDOWN_STATUS_STOP_AUDIO": "Hang leállítása...",
    "SHUTDOWN_STATUS_STOP_RFID": "RFID leállítása...",
    "SHUTDOWN_STATUS_STOP_SSAVER": "Képernyővédő leállítása...",
    "SHUTDOWN_STATUS_UNMOUNT_FS": "Fájlrendszer leválasztása...",
    "SHUTDOWN_STATUS_UNMOUNT_SD": "SD kártya leválasztása...",
    "SHUTDOWN_MESSAGE_GOODBYE": "Viszontlátásra!",

    "DEMO_MODE_ACTIVATED": "Demó Mód Aktiválva",
    "DEMO_UI_LIMIT_REACHED": "Demó: UI limit elérve!",
    "DEMO_WIFI_SCAN_LIMIT_REACHED": "Demó: Wi-Fi szkennelés limit elérve!",
    "DEMO_BLE_SCAN_LIMIT_REACHED": "Demó: BLE szkennelés limit elérve!",
    "DEMO_BLE_SCAN_COUNT": "Demó: BLE szkennelés",
    "DEMO_SCREEN_DRAW_LIMIT_REACHED": "Demó: Képernyőrajzolási limit elérve!"
  }
})";

/**
 * @brief An array of all available language assets.
 *
 * This array provides a central collection of all supported languages,
 * linking their enum, file path, and embedded JSON content.
 */
const LanguageAsset languageAssets[] = {
    { LanguageManager::Language::EN, "/en.json", default_en_json }, ///< English language asset.
    { LanguageManager::Language::HU, "/hu.json", default_hu_json }  ///< Hungarian language asset.
};

/**
 * @brief The total number of language assets available.
 *
 * This constant is calculated at compile time to provide an easy way
 * to iterate through the `languageAssets` array.
 */
const size_t languageAssetCount = sizeof(languageAssets) / sizeof(languageAssets[0]);