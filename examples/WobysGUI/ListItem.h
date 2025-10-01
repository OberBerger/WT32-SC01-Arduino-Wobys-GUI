/**
 * @file ListItem.h
 * @brief Defines data structures for representing items in a clickable list UI, including column data and various states.
 *
 * This header file provides the `ColumnData` struct for individual column properties
 * and the `ListItem` struct for a full list entry. `ListItem` includes fields for
 * general UI state (selection, ID), as well as specific data relevant to common
 * application contexts like Wi-Fi networks or Bluetooth devices (SSID, MAC address,
 * online status, paired status). It also defines an enum for unset text datum values.
 *
 * @version 1.0.1
 * @date 2025-08-12
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
#ifndef LISTITEM_H
#define LISTITEM_H

#include <string>           // Required for std::string
#include <vector>           // Required for std::vector
#include <LovyanGFX.hpp>    // Required for textdatum_t and IFont

/**
 * @brief An explicit "unset" value for `textdatum_t` when used in `ColumnData`.
 * This allows distinguishing between `TL_DATUM` (actual top-left) and an unconfigured state.
 */
enum ListUiDatumOverride_ListItem : uint8_t {
    DATUM_ITEM_UNSET = 255 ///< Sentinel value indicating that text datum is not explicitly set for an item.
};

/**
 * @brief Represents the data and styling for a single column within a list item.
 */
struct ColumnData {
    std::string text;           ///< The text content for this column.
    const lgfx::IFont* font;    ///< Optional: specific font for this column (nullptr to use default).
    textdatum_t textAlign;      ///< Optional: specific text alignment for this column (DATUM_ITEM_UNSET to use default).
    uint32_t textColor;         ///< Optional: specific text color for this column (0 to use default/selected color).

    /**
     * @brief Constructor for `ColumnData`.
     * Initializes a column with provided text, font, alignment, and color.
     * @param t The text content (default: empty string).
     * @param f A pointer to the font (default: nullptr).
     * @param align The text alignment (default: DATUM_ITEM_UNSET).
     * @param tc The text color (default: 0).
     */
    ColumnData(const std::string& t = "",
               const lgfx::IFont* f = nullptr,
               textdatum_t align = (textdatum_t)DATUM_ITEM_UNSET,
               uint32_t tc = 0)
        : text(t), font(f), textAlign(align), textColor(tc) {}
};

/**
 * @brief Represents a single item in a clickable list, capable of holding multiple columns
 * and additional metadata for different application contexts (e.g., Wi-Fi, BLE).
 */
struct ListItem {
    std::vector<ColumnData> columns; ///< A vector of `ColumnData` objects representing the columns of this item.
    bool isSelected;                 ///< True if this list item is currently selected in the UI.
    int id;                          ///< A generic integer ID for the item (default: -1).
    intptr_t tag;                    ///< A generic pointer-sized tag for custom data association (default: 0).

    // --- Wi-Fi / BLE Specific Data (Optional) ---
    std::string serviceUUID;         ///< Service UUID string, relevant for BLE devices.
    std::string address;             ///< MAC address or BLE address string for network/device items.
    bool isOnline;                   ///< True if the item represents an online/detectable device/network.
    std::string primaryConnectId;    ///< The primary identifier used for connection (e.g., UUID or MAC).
    bool isPaired;                   ///< True if the item represents a paired/saved device/network.

    /**
     * @brief Default constructor for `ListItem`.
     * Initializes all members to their default values.
     */
    ListItem()
        : isSelected(false), id(-1), tag(0), serviceUUID(""), address(""), isOnline(false),
          primaryConnectId(""), isPaired(false) {}

    /**
     * @brief Constructor for `ListItem` using an initializer list for columns.
     * Initializes columns and other members to their default values.
     * @param cols An initializer list of `ColumnData` objects for the item's columns.
     */
    ListItem(std::initializer_list<ColumnData> cols)
        : columns(cols), isSelected(false), id(-1), tag(0), serviceUUID(""), address(""), isOnline(false),
          primaryConnectId(""), isPaired(false) {}

    /**
     * @brief Constructor for `ListItem` using a vector of columns.
     * Initializes columns and other members to their default values.
     * @param cols A vector of `ColumnData` objects for the item's columns.
     */
    ListItem(const std::vector<ColumnData>& cols)
        : columns(cols), isSelected(false), id(-1), tag(0), serviceUUID(""), address(""), isOnline(false),
          primaryConnectId(""), isPaired(false) {}
};

/**
 * @brief Represents basic data for a Wi-Fi list item before conversion to `ListItem`.
 * This struct is typically used internally by `WifiManager` for scanned network results.
 */
struct WifiListItemData {
    std::string ssid;             ///< The SSID (name) of the Wi-Fi network.
    int32_t rssi;                 ///< The RSSI (signal strength) of the network.
    uint8_t encryptionType;       ///< The encryption type of the network (e.g., WIFI_AUTH_OPEN).

    /**
     * @brief Constructor for `WifiListItemData`.
     * Initializes network data.
     * @param s The SSID (default: empty string).
     * @param r The RSSI (default: -100).
     * @param enc The encryption type (default: 0).
     */
    WifiListItemData(const std::string& s = "", int32_t r = -100, uint8_t enc = 0)
        : ssid(s), rssi(r), encryptionType(enc) {}
};

#endif // LISTITEM_H