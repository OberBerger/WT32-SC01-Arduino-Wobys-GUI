/**
 * @file RFIDManager.cpp
 * @brief Implements the RFIDManager class for managing RFID reader operations.
 *
 * This file provides the core logic for initializing the MFRC522 RFID reader,
 * communicating via SPI, continuously checking for new cards, and processing
 * card read events. It handles debouncing for repeated card reads, converts
 * card data into a structured format, and integrates with a status bar icon
 * to reflect its operational status.
 *
 * @version 1.0.1
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
#include "RFIDManager.h"
#include <SPI.h> // Required for `SPI.begin()` and `SPI` object
// Required for `RFIDManagerConfig` struct definition (defined in SystemInitializer.h).
#include "SystemInitializer.h"
#include "IconElement.h" // Required for `IconElement` class definition

// --- Constructor Implementation ---
/**
 * @brief Constructor for the RFIDManager class.
 * Initializes internal state variables to their default values.
 * `std::unique_ptr` members (`_ss_pin_obj`, `_spi_driver`, `_mfrc522`) are
 * default-constructed to `nullptr` and do not require explicit initialization here.
 */
RFIDManager::RFIDManager()
  : _ssPin(0), _sckPin(0), _mosiPin(0), _misoPin(0), // Initialize pins.
    _isEnabled(true),
    _rfidIconElement(nullptr), // Initialize pointer to nullptr.
    _cardScannedCallback(nullptr),
    _lastCardCheckTime(0),
    _previousUID(), // Empty vector.
    _lastSuccessfulReadTime(0)
{
  DEBUG_INFO_PRINTLN("RFIDManager: Constructor executed.");
}

// --- Destructor Implementation ---
/**
 * @brief Destructor for the RFIDManager class.
 * Cleans up dynamically allocated MFRC522 driver objects.
 * `std::unique_ptr` automatically handles memory deallocation when the `RFIDManager`
 * object is destroyed. `_cleanup()` is called to ensure proper state reset.
 */
RFIDManager::~RFIDManager() {
    DEBUG_INFO_PRINTLN("RFIDManager: Destructor executed. Releasing MFRC522 driver resources.");
    _cleanup(); // Call cleanup to ensure any active MFRC522 features are stopped/reset.
}

// --- Initialization & Lifecycle ---
/**
 * @brief Initializes the MFRC522 RFID module with hardware-specific configurations.
 * This method must be called once during system setup. It sets up the SPI communication
 * and performs a self-test of the MFRC522 chip.
 * @param config The `RFIDManagerConfig` structure containing pin assignments for SPI.
 * @return True if initialization is successful, false otherwise.
 */
bool RFIDManager::init(const RFIDManagerConfig& config) {
  DEBUG_INFO_PRINTLN("RFIDManager: Initialization started (based on ConfigStruct)...");

  // Store pin configurations.
  _ssPin = config.ssPin;
  _sckPin = config.sckPin;
  _mosiPin = config.mosiPin;
  _misoPin = config.misoPin;

  // Dynamically allocate MFRC522 driver objects using std::make_unique.
  // Check for allocation failures and clean up if necessary.
  _ss_pin_obj = std::make_unique<MFRC522DriverPinSimple>(_ssPin);
  if (!_ss_pin_obj) {
    DEBUG_ERROR_PRINTLN("RFIDManager: ERROR - Failed to allocate MFRC522DriverPinSimple! Out of memory.");
    _cleanup(); // Clean up potentially partial initialization.
    return false;
  }

  _spi_driver = std::make_unique<MFRC522DriverSPI>(*_ss_pin_obj, SPI, SPISettings());
  if (!_spi_driver) {
    DEBUG_ERROR_PRINTLN("RFIDManager: ERROR - Failed to allocate MFRC522DriverSPI! Out of memory.");
    _cleanup(); // Clean up prior successful allocations.
    return false;
  }

  _mfrc522 = std::make_unique<MFRC522>(*_spi_driver);
  if (!_mfrc522) {
    DEBUG_ERROR_PRINTLN("RFIDManager: ERROR - Failed to allocate MFRC522! Out of memory.");
    _cleanup(); // Clean up prior successful allocations.
    return false;
  }

  // Initialize the MFRC522 chip.
  _mfrc522->PCD_Init();
  delay(4); // Short delay after initialization.

  DEBUG_INFO_PRINTLN("RFIDManager: MFRC522 Version Information:");
  // Ensure the MFRC522Debug function is compatible with unique_ptr (takes a reference).
  MFRC522Debug::PCD_DumpVersionToSerial(*_mfrc522, Serial); // Dump version info to serial.

  // Perform self-test.
  // The user explicitly requested to continue even if self-test fails.
  if (_mfrc522->PCD_PerformSelfTest()) {
    DEBUG_INFO_PRINTLN("RFIDManager: MFRC522 Self-test: SUCCESSFUL.");
  } else {
    // This warning (and continuation) is the requested behavior.
    DEBUG_WARN_PRINTLN("RFIDManager: WARNING - MFRC522 Self-test: FAILED. Check wiring!");
  }
  _mfrc522->PCD_Init(); // Re-init after self-test.
  
  _isEnabled = true; // Set to true after successful allocation and init.
  DEBUG_INFO_PRINTLN("RFIDManager: Initialization completed. Searching for cards...");
  return true;
}

/**
 * @brief Main loop method for the RFIDManager.
 * This function should be called repeatedly in the Arduino `loop()` function.
 * It continuously checks for the presence of new RFID cards and attempts to read them.
 * It respects the enabled/disabled state of the manager and incorporates debouncing logic.
 */
void RFIDManager::loop() {
  // If RFID is disabled, or MFRC522 object not initialized, skip loop logic.
  if (!_isEnabled || !_mfrc522) { // _mfrc522 will be nullptr if init failed.
      return;
  }

  unsigned long currentTime = millis();

  // Apply check interval cooldown.
  if (currentTime - _lastCardCheckTime < CARD_CHECK_INTERVAL) {
    return;
  }
  _lastCardCheckTime = currentTime;

  // Check for new cards.
  if (!_mfrc522->PICC_IsNewCardPresent()) {
    return;
  }

  // Read card serial (UID).
  if (!_mfrc522->PICC_ReadCardSerial()) {
    return;
  }

  // Debounce logic: check if the same card was read recently.
  bool sameAsPrevious = true;
  if (_mfrc522->uid.size != _previousUID.size()) {
    sameAsPrevious = false;
  } else {
    for (byte i = 0; i < _mfrc522->uid.size; i++) { 
      if (_mfrc522->uid.uidByte[i] != _previousUID[i]) {
        sameAsPrevious = false;
        break;
      }
    }
  }

  // If same card and within debounce interval, ignore.
  if (sameAsPrevious && (currentTime - _lastSuccessfulReadTime < DEBOUNCE_READ_INTERVAL)) {
    DEBUG_TRACE_PRINTLN("RFIDManager: Same card detected within debounce interval, ignoring.");
    return;
  }

  // Process newly detected card.
  RFIDCardData cardData;
  cardData.uid_bytes.assign(_mfrc522->uid.uidByte, _mfrc522->uid.uidByte + _mfrc522->uid.size);

  // Convert UID bytes to hexadecimal string.
  // MFRC522::uid.uidByte array has a maximum capacity of 10 bytes.
  // Each byte is converted to 2 hexadecimal characters (e.g., "0A", "FF").
  // Plus, 1 byte for the null terminator. So, 10 bytes * 2 chars/byte + 1 null terminator = 21 bytes.
  // A buffer size of 32 is used for safety and potential future formatting changes.
  char uidBuffer[32]; // Sufficient for 10 UID bytes (20 hex chars) + separators/null.
  char* ptr = uidBuffer; 
  for (byte i = 0; i < _mfrc522->uid.size; i++) {
    // Check for buffer overflow before writing.
    if ((ptr - uidBuffer) + 2 < sizeof(uidBuffer)) {
        sprintf(ptr, "%02X", _mfrc522->uid.uidByte[i]);
        ptr += 2;
    } else {
        DEBUG_ERROR_PRINTLN("RFIDManager: UID buffer overflow prevented during string conversion.");
        break; // Stop conversion if buffer limit reached.
    }
  }
  *ptr = '\0'; // Null-terminate the string.
  cardData.uid_string = std::string(uidBuffer);

  // Determine PICC type.
  cardData.picc_type = _mfrc522->PICC_GetType(_mfrc522->uid.sak);
  cardData.card_type_string = getPICCTypeName(cardData.picc_type, _mfrc522->uid.sak);

  DEBUG_INFO_PRINTF("RFIDManager: Card read! UID: %s, Type: %s.\n",
               cardData.uid_string.c_str(), cardData.card_type_string.c_str());

  // Trigger callback if registered.
  if (_cardScannedCallback) {
    _cardScannedCallback(cardData);
  }

  // Update debouncing state.
  _previousUID = cardData.uid_bytes;
  _lastSuccessfulReadTime = currentTime;

  _mfrc522->PICC_HaltA(); // Halt PICC to prevent multiple reads of the same card in short interval.
}

// --- Configuration & Control ---
/**
 * @brief Enables or disables the RFID scanning functionality.
 * When enabled, the MFRC522 chip is initialized or brought out of power-down mode.
 * When disabled, the chip is put into a soft power-down state, and its status bar icon is cleared.
 * @param enabled True to enable RFID scanning, false to disable.
 */
void RFIDManager::setEnabled(bool enabled) {
    if (_isEnabled != enabled) {
        _isEnabled = enabled;
        if (_isEnabled) {
            DEBUG_INFO_PRINTLN("RFIDManager: Scanning enabled. Re-initializing MFRC522...");
            if (_mfrc522) { // Ensure MFRC522 object is valid before using.
                _mfrc522->PCD_Init(); // Bring out of power down.
            } else {
                DEBUG_WARN_PRINTLN("RFIDManager: WARNING - _mfrc522 is nullptr, cannot init PCD.");
            }
            if (_rfidIconElement) { // Update icon.
                _rfidIconElement->setIcon(RFID_DEFAULT_ICON_CHAR); // Display active icon.
            } else {
                DEBUG_WARN_PRINTLN("RFIDManager: WARNING - _rfidIconElement is nullptr, cannot set active icon.");
            }
        } else {
            DEBUG_INFO_PRINTLN("RFIDManager: Scanning disabled. Putting MFRC522 into SoftPowerDown...");
            if (_mfrc522) { // Ensure MFRC522 object is valid before using.
                _mfrc522->PCD_SoftPowerDown();
            } else {
                DEBUG_WARN_PRINTLN("RFIDManager: WARNING - _mfrc522 is nullptr, cannot soft power down.");
            }
            if (_rfidIconElement) { // Update icon.
                _rfidIconElement->setIcon(" "); // Hide icon.
            } else {
                DEBUG_WARN_PRINTLN("RFIDManager: WARNING - _rfidIconElement is nullptr, cannot clear icon.");
            }
        }
    }
}

/**
 * @brief Checks if RFID scanning functionality is currently enabled.
 * @return True if RFID scanning is enabled, false otherwise.
 */
bool RFIDManager::isEnabled() const {
    return _isEnabled;
}

// --- UI Integration ---
/**
 * @brief Sets the `IconElement` instance used to display the RFID module's status.
 * The `RFIDManager` will update this icon (its character) based on its enabled/disabled state.
 * @param icon A pointer to the `IconElement` instance for the RFID icon.
 */
void RFIDManager::setRfidIconElement(IconElement* icon) {
    _rfidIconElement = icon;
    // The icon's initial state (active/inactive) will be set by `setEnabled()`
    // which is called during system initialization after settings are loaded.
    DEBUG_INFO_PRINTLN("RFIDManager: RFID icon element set.");
}

// --- Callback Registration ---
/**
 * @brief Sets the callback function to be executed when an RFID card is successfully scanned.
 * @param callback The `CardScannedCallback` function.
 */
void RFIDManager::setOnCardScannedCallback(CardScannedCallback callback) {
  _cardScannedCallback = callback;
  DEBUG_INFO_PRINTLN("RFIDManager: OnCardScannedCallback set.");
}

// --- Private Helper Methods ---
/**
 * @brief Cleans up allocated MFRC522 driver objects and resets internal state.
 * This helper is used during `init()` failure or in the destructor.
 * `std::unique_ptr` automatically handles memory deallocation.
 */
void RFIDManager::_cleanup() {
    // Resetting unique_ptrs will free the memory they manage.
    if (_mfrc522) {
        // Attempt to put the MFRC522 into a safe state before releasing its resources.
        _mfrc522->PCD_SoftPowerDown();
        _mfrc522.reset(); // Release MFRC522 object.
    }
    _spi_driver.reset();    // Release SPI driver object.
    _ss_pin_obj.reset();    // Release pin driver object.

    _isEnabled = false; // Ensure RFID is marked as disabled.
    if (_rfidIconElement) {
        _rfidIconElement->setIcon(' '); // Clear icon if it was set.
    }
    DEBUG_INFO_PRINTLN("RFIDManager: Cleaned up MFRC522 driver resources.");
}

/**
 * @brief Converts a `MFRC522Constants::PICC_Type` enum and SAK value to a human-readable string.
 * @param piccType The `PICC_Type` enum value representing the card type.
 * @param sakValue The SAK (Select Acknowledge) byte from the card.
 * @return A `std::string` containing the human-readable card type name.
 */
std::string RFIDManager::getPICCTypeName(MFRC522Constants::PICC_Type piccType, byte sakValue) {
  switch (piccType) {
    case MFRC522Constants::PICC_TYPE_ISO_14443_4: return "ISO/IEC 14443-4";
    case MFRC522Constants::PICC_TYPE_ISO_18092: return "ISO/IEC 18092 (NFC)";
    case MFRC522Constants::PICC_TYPE_MIFARE_MINI: return "MIFARE Mini";
    case MFRC522Constants::PICC_TYPE_MIFARE_1K: return "MIFARE 1K";
    case MFRC522Constants::PICC_TYPE_MIFARE_4K: return "MIFARE 4K";
    case MFRC522Constants::PICC_TYPE_MIFARE_UL: return "MIFARE Ultralight";
    case MFRC522Constants::PICC_TYPE_MIFARE_PLUS: return "MIFARE Plus";
    case MFRC522Constants::PICC_TYPE_MIFARE_DESFIRE: return "MIFARE DESFire";
    case MFRC522Constants::PICC_TYPE_NOT_COMPLETE: return "SAK_NOT_COMPLETE";
    case MFRC522Constants::PICC_TYPE_UNKNOWN:
    default:
      char buffer[20]; // Sufficient buffer for "Unknown (SAK:0xXX)\0".
      sprintf(buffer, "Unknown (SAK:0x%02X)", sakValue);
      return std::string(buffer);
  }
}