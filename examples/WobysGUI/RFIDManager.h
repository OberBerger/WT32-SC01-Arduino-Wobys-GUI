/**
 * @file RFIDManager.h
 * @brief Defines the RFIDManager class for managing RFID reader operations.
 *
 * This class provides functionalities for initializing an MFRC522 RFID reader,
 * scanning for new cards, reading their UID and type, and triggering a callback
 * upon successful card detection. It integrates with the system's SettingsManager
 * for enabling/disabling functionality and manages a dedicated status bar icon.
 *
 * @version 1.0.2
 * @date 2025-08-28
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
#pragma once
#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <string>
#include <functional>
#include "Config.h"
#include "ListItem.h"

// Required MFRC522 library headers.
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// For std::unique_ptr
#include <memory> 

// Forward declarations to avoid circular dependencies and unnecessary includes.
struct RFIDManagerConfig;
class IconElement;

/**
 * @brief Structure to hold data read from an RFID card.
 */
struct RFIDCardData {
  std::vector<byte> uid_bytes;    ///< The Unique IDentifier (UID) of the card as raw bytes.
  std::string uid_string;         ///< The UID of the card as a hexadecimal string.
  std::string card_type_string;   ///< A human-readable string representing the PICC (card) type.
  MFRC522Constants::PICC_Type picc_type; ///< The raw PICC type enum from the MFRC522 library.
  
  // --- New: ListItem representation for easy UI integration ---
  ListItem asListItem() const {
      // Create a ListItem with the UID in the first column, and a '+' for add action in the second.
      return ListItem({ ColumnData(uid_string), ColumnData("X") });
  }
};
    
/**
 * @brief The RFIDManager class manages RFID reader operations.
 *
 * This manager is responsible for initializing an MFRC522 RFID reader,
 * continuously scanning for new cards, reading their unique identifiers (UIDs)
 * and types, and triggering a registered callback upon successful card detection.
 * It provides methods to enable/disable RFID scanning functionality and integrates
 * with a status bar icon to indicate its active state.
 */
class RFIDManager {
public:
  // --- Callback Types ---
  /**
   * @brief Callback type for when an RFID card is successfully scanned.
   * @param cardData A constant reference to the `RFIDCardData` struct containing the scanned card's information.
   */
  using CardScannedCallback = std::function<void(const RFIDCardData& cardData)>;

  // --- Constructor ---
  /**
   * @brief Constructor for the RFIDManager class.
   * Initializes internal state variables to their default values.
   * `std::unique_ptr` members are default-constructed to `nullptr`.
   */
  RFIDManager();

  // --- Destructor ---
  /**
   * @brief Destructor for the RFIDManager class.
   * Cleans up dynamically allocated MFRC522 driver objects.
   * `std::unique_ptr` automatically handles memory deallocation.
   */
  ~RFIDManager();
  
  // --- Initialization & Lifecycle ---
  /**
   * @brief Initializes the MFRC522 RFID module with hardware-specific configurations.
   * This method must be called once during system setup. It sets up the SPI communication
   * and performs a self-test of the MFRC522 chip.
   * @param config The `RFIDManagerConfig` structure containing pin assignments for SPI.
   * @return True if initialization is successful, false otherwise.
   */
  bool init(const RFIDManagerConfig& config);
  
  /**
   * @brief Main loop method for the RFIDManager.
   * This function should be called repeatedly in the Arduino `loop()` function.
   * It continuously checks for the presence of new RFID cards and attempts to read them.
   * It respects the enabled/disabled state of the manager and incorporates debouncing logic.
   */
  void loop();

  // --- Configuration & Control ---
  /**
   * @brief Enables or disables the RFID scanning functionality.
   * When enabled, the MFRC522 chip is initialized or brought out of power-down mode.
   * When disabled, the chip is put into a soft power-down state, and its status bar icon is cleared.
   * @param enabled True to enable RFID scanning, false to disable.
   */
  void setEnabled(bool enabled);

  /**
   * @brief Checks if RFID scanning functionality is currently enabled.
   * @return True if RFID scanning is enabled, false otherwise.
   */
  bool isEnabled() const;

  // --- UI Integration ---
  /**
   * @brief Sets the `IconElement` instance used to display the RFID module's status.
   * The `RFIDManager` will update this icon (its character) based on its enabled/disabled state.
   * @param icon A pointer to the `IconElement` instance for the RFID icon.
   */
  void setRfidIconElement(IconElement* icon);

  // --- Callback Registration ---
  /**
   * @brief Sets the callback function to be executed when an RFID card is successfully scanned.
   * @param callback The `CardScannedCallback` function.
   */
  void setOnCardScannedCallback(CardScannedCallback callback);

private:
  // --- MFRC522 Hardware Drivers & Instance ---
  std::unique_ptr<MFRC522DriverPinSimple> _ss_pin_obj; ///< Driver for the Slave Select (SS) pin.
  std::unique_ptr<MFRC522DriverSPI> _spi_driver;       ///< Driver for SPI communication.
  std::unique_ptr<MFRC522> _mfrc522;                   ///< Main MFRC522 object for chip interaction.

  // --- Configuration Parameters ---
  int _ssPin;    ///< Slave Select (SS) pin number.
  int _sckPin;   ///< Serial Clock (SCK) pin number.
  int _mosiPin;  ///< Master Out Slave In (MOSI) pin number.
  int _misoPin;  ///< Master In Slave Out (MISO) pin number.

  // --- Internal State ---
  bool _isEnabled;              ///< Flag indicating if RFID scanning is currently enabled.
  IconElement* _rfidIconElement;///< Pointer to the status bar icon element for RFID.
  CardScannedCallback _cardScannedCallback; ///< Registered callback for scanned card events.

  // --- Timing & Debouncing ---
  unsigned long _lastCardCheckTime;  ///< Timestamp of the last RFID card presence check.
  std::vector<byte> _previousUID;    ///< Stores the UID of the last successfully read card for debouncing.
  unsigned long _lastSuccessfulReadTime; ///< Timestamp of the last successful, debounced card read.
  const unsigned long CARD_CHECK_INTERVAL = 200; ///< Interval in milliseconds between checking for new cards.
  const unsigned long DEBOUNCE_READ_INTERVAL = 1500; ///< Interval in milliseconds to debounce repeated reads of the same card.


  // --- Private Helper Methods ---
  /**
   * @brief Cleans up allocated MFRC522 driver objects and resets internal state.
   * This helper is used during `init()` failure or in the destructor.
   * `std::unique_ptr` automatically handles memory deallocation.
   */
  void _cleanup();

  /**
   * @brief Converts a `MFRC522Constants::PICC_Type` enum and SAK value to a human-readable string.
   * @param piccType The `PICC_Type` enum value representing the card type.
   * @param sakValue The SAK (Select Acknowledge) byte from the card.
   * @return A `std::string` containing the human-readable card type name.
   */
  std::string getPICCTypeName(MFRC522Constants::PICC_Type piccType, byte sakValue);
};

#endif // RFID_MANAGER_H