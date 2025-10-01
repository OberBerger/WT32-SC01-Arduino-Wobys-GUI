/**
 * @file WobysGUI.ino
 * @brief Main application entry point for the WT32-SC01-Plus GUI and system management.
 *
 * This file initializes all hardware, system managers, and UI components.
 * It orchestrates the main application loop, handling touch input,
 * refreshing UI elements, and managing system services like Wi-Fi, Bluetooth,
 * time, power, and RFID. Designed for modularity and extensibility.
 *
 * @version 1.0.0
 * @date 2025-07-04
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
 *
 * For legal advice regarding distribution, please consult with a legal professional.
 */

//=============================================================================
// 1. PROJECT INCLUDES
//    Standard library includes and custom project headers.
//=============================================================================

// The main GUI
#include <WobysGUI.h> 

// Standard Arduino and ESP32 Libraries
#include <Wire.h>  // For I2C communication (used by touch controller)
#include <SPI.h>   // For SPI communication (used by RFID reader and SD card)

// Master Configuration File (includes all other ConfigXXX.h files)
#include "Config.h" 

// Core UI and Graphics Framework
#include "UILayer.h"       // Base class for UI screens/layers
#include "ScreenManager.h" // Manages UI layers and screen transitions

// System Manager Classes (headers are needed here for global object instantiation)
#include "StatusbarUI.h"
#include "WifiManager.h"
#include "SettingsManager.h"
#include "TimeManager.h"
#include "BLEManager.h"
#include "PowerManager.h"
#include "RFIDManager.h"
#include "ScreenSaverManager.h"
#include "LanguageManager.h"
#include "AudioManager.h"
#include "SDManager.h"

// Specific UI Element Classes (headers are needed here for global object instantiation)
#include "ClockLabelUI.h"

// High-Level UI Controller Classes (Screens) (headers are needed here for global object instantiation)
#include "BLEUI.h"
#include "WifiUI.h"
#include "MainUI.h"
#include "SettingsUI.h"

// System Initializer Class
#include "SystemInitializer.h"


//=============================================================================
// 2. GLOBAL OBJECTS INSTANTIATION
//    Creating single instances of core system managers and UI components.
//    These objects will be initialized by the SystemInitializer.
//=============================================================================

// Graphics and Screen Management Components
LGFX lcd;                                             ///< LovyanGFX display driver instance
ScreenManager screenManager(&lcd, STATUSBAR_HEIGHT, LGFX_DEFAULT_ROTATION_PORTRAIT, LGFX_DEFAULT_ROTATION_LANDSCAPE);  ///< Manages UI layers and screen transitions
StatusbarUI statusbar(&lcd, &screenManager);          ///< Manages the top status bar and pull-down panel

extern WifiManager wifiManager;

// Core System Managers
SettingsManager settingsManager;                                             ///< Manages persistent application settings
BLEManager btManager(&settingsManager, &screenManager, &wifiManager);        ///< Manages Bluetooth Low Energy connectivity
WifiManager wifiManager(&settingsManager, &btManager);                       ///< Manages Wi-Fi connectivity
TimeManager timeManager(&wifiManager, DEFAULT_GMT_OFFSET_SEC, DEFAULT_DAYLIGHT_OFFSET_SEC, DEFAULT_NTP_SERVER); ///< Manages system time and NTP synchronization
PowerManager powerManager(&statusbar, nullptr);                              ///< Manages battery and system power, links to statusbar
RFIDManager rfidManager;                                                     ///< Manages RFID reader interactions
LanguageManager languageManager;                                             ///< Manages multi-language support
AudioManager audioManager(&settingsManager);                                 ///< Manages audio output
SDManager sdManager(&settingsManager);                                       ///< Manages SD card operations

// Screen Saver Components
ClockLabelUI screenSaverClock(&lcd, "00:00", 0, 0, &helvB24, UI_COLOR_TEXT_DEFAULT, MC_DATUM, 150, 40); ///< Clock UI element for screensaver
ScreenSaverManager screenSaverManager(&lcd, &screenManager, &statusbar, &timeManager, &screenSaverClock); ///< Manages screen saver activation and animations

// High-Level UI Controllers (Screens)
BLEUI btUI(&lcd, &screenManager, &btManager, &statusbar, &languageManager, &settingsManager); ///< Bluetooth UI screen controller
WifiUI wifiUI(&lcd, &screenManager, &wifiManager, &settingsManager, &statusbar, &languageManager); ///< Wi-Fi UI screen controller
SettingsUI settingsUI(&lcd, &screenManager, &settingsManager, &languageManager, &powerManager, &rfidManager, &screenSaverManager, &statusbar, &audioManager); ///< Settings UI screen controller
MainUI mainUI(&lcd, &screenManager, &powerManager, &languageManager, &audioManager);             ///< Main application UI screen controller


// System Initializer Instance
// This object orchestrates the entire system and UI setup process.
SystemInitializer initializer(
    &lcd, &screenManager, &statusbar, &settingsManager, &wifiManager, &timeManager,
    &btManager, &powerManager, &rfidManager, &screenSaverManager, &screenSaverClock,
    &btUI, &wifiUI, &mainUI, &languageManager, &settingsUI, &audioManager,
    &sdManager
);


//=============================================================================
// 3. GLOBAL VARIABLES
//    Application-wide variables.
//=============================================================================

unsigned long systemStartTimeMillis = 0; ///< Timestamp for when the system started (for uptime calculation)

//=============================================================================
// 4. SYSTEM-LEVEL CALLBACK FUNCTIONS
//    Functions defined here are passed to managers for event handling or critical operations.
//    They access global objects directly and will be called by the respective managers.
//=============================================================================

/**
 * @brief Initiates a system shutdown sequence.
 * This function is typically triggered by a UI button in the status bar panel.
 */
void initiateShutdown() {
  DEBUG_INFO_PRINTLN("Global Callback: initiateShutdown() called. Requesting system power off.");
  powerManager.requestSystemPowerOff();
}

/**
 * @brief Opens the Wi-Fi settings panel.
 * This function is typically triggered by a UI button in the status bar panel.
 */
void openWiFiSettingsPanel() {
  DEBUG_INFO_PRINTLN("Global Callback: openWiFiSettingsPanel() called. Opening Wi-Fi UI.");
  wifiUI.openPanel();
}

/**
 * @brief Opens the Bluetooth settings panel.
 * This function is typically triggered by a UI button in the status bar panel.
 */
void openBluetoothSettingsPanel() {
  DEBUG_INFO_PRINTLN("Global Callback: openBluetoothSettingsPanel() called. Opening Bluetooth UI.");
  btUI.openPanel();
}

/**
 * @brief Opens the main application settings screen.
 * This function is typically triggered by a UI button in the status bar panel.
 */
void openSettingsScreen() {
  DEBUG_INFO_PRINTLN("Global Callback: openSettingsScreen() called. Opening Settings UI.");
  settingsUI.openPanel();
}

/**
 * @brief Handles a low battery shutdown warning message.
 * Displays a localized warning on the message board.
 * @param messageKey The key for the warning message to be localized.
 */
void handleShutdownWarning(const std::string& messageKey) { 
  DEBUG_WARN_PRINTF("Global Callback: Shutdown warning (key): %s\n", messageKey.c_str());
  if (initializer._messageBoard) { // Access messageBoard via initializer's owned pointer
    std::string localizedMessage = languageManager.getString(messageKey, "Unknown Warning!"); 
    initializer._messageBoard->setText(localizedMessage, 5000);
  } else {
    DEBUG_ERROR_PRINTLN("Global Callback: MessageBoard is nullptr, cannot display shutdown warning.");
  }
}

/**
 * @brief Performs final tasks before the system powers off.
 * This function is called just before the actual power-off sequence.
 * Examples include saving critical data, stopping ongoing processes, closing connections.
 */
void handlePerformShutdownTasks() {
  DEBUG_INFO_PRINTLN("Global Callback: Performing final tasks before shutdown...");

  // 1. Stop network/radio managers.
  if (wifiManager.isWifiLogicEnabled()) {
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_STOP_WIFI", "Stopping Wi-Fi..."), 0);
          delay(50);
      }
      wifiManager.disableWifi(); // This also saves the last state.
      DEBUG_INFO_PRINTLN("Global Callback: Wi-Fi stopped.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: Wi-Fi not enabled, skipping stop.");
  }

  if (btManager.isEnabled()) {
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_STOP_BT", "Stopping Bluetooth..."), 0);
          delay(50);
      }
      btManager.disableBluetooth(); // This also saves the last state.
      DEBUG_INFO_PRINTLN("Global Callback: Bluetooth stopped.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: Bluetooth not enabled, skipping stop.");
  }

  // 2. Stop audio and RFID.
  if (audioManager.isEnabled()) {
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_STOP_AUDIO", "Stopping Audio..."), 0);
          delay(50);
      }
      audioManager.stop();
      audioManager.setEnabled(false); // This also saves the last state.
      DEBUG_INFO_PRINTLN("Global Callback: Audio stopped.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: Audio not enabled, skipping stop.");
  }

  if (rfidManager.isEnabled()) {
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_STOP_RFID", "Stopping RFID..."), 0);
          delay(50);
      }
      rfidManager.setEnabled(false); // This saves state and powers down MFRC522.
      DEBUG_INFO_PRINTLN("Global Callback: RFID stopped.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: RFID not enabled, skipping stop.");
  }

  // 3. Deactivate screensaver (if active, might still be in a transition).
  if (screenSaverManager.isActive()) {
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_STOP_SSAVER", "Stopping Screensaver..."), 0);
          delay(50);
      }
      screenSaverManager.setEnabled(false); // Deactivates fully.
      DEBUG_INFO_PRINTLN("Global Callback: Screensaver deactivated.");
      delay(50);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: Screensaver not active, skipping deactivation.");
  }


  // 4. Unmount filesystems to prevent data corruption.
  // LittleFS: Normally, LittleFS will handle unmounting automatically on restart/deep sleep.
  // However, explicit `end()` is safer before a hard power-off if there's any doubt about pending writes.
  if (LittleFS.begin()) { // Check if LittleFS is mounted
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_UNMOUNT_FS", "Unmounting FS..."), 0);
          delay(50);
      }
      LittleFS.end();
      DEBUG_INFO_PRINTLN("Global Callback: LittleFS unmounted.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: LittleFS not mounted, skipping unmount.");
  }

  // 5. SD Card: CRITICAL to unmount to prevent data corruption.
  if (sdManager.isCardPresent()) { // Check if SD is actually present/mounted
      if (initializer._messageBoard) {
          initializer._messageBoard->setText(languageManager.getString("SHUTDOWN_STATUS_UNMOUNT_SD", "Unmounting SD card..."), 0);
          delay(50);
      }
      SD.end(); // Explicitly unmount SD.
      DEBUG_INFO_PRINTLN("Global Callback: SD card unmounted.");
      delay(100);
  } else {
      DEBUG_TRACE_PRINTLN("Global Callback: SD card not present, skipping unmount.");
  }
  
  // 6. Final UI feedback before power-off.
  // Clear screen completely and display a goodbye message.
  if (initializer._lcd) {
      initializer._lcd->fillScreen(TFT_BLACK);
      initializer._lcd->setFont(&helvB18); 
      initializer._lcd->setTextColor(TFT_WHITE);
      initializer._lcd->setTextDatum(MC_DATUM);
      initializer._lcd->drawString(languageManager.getString("SHUTDOWN_MESSAGE_GOODBYE", "Goodbye!").c_str(), initializer._lcd->width()/2, initializer._lcd->height()/2);
  }
  DEBUG_INFO_PRINTLN("Global Callback: All final shutdown tasks completed.");
  delay(3000);
}

/**
 * @brief Handles RFID card scan events.
 * Displays the scanned card's UID on the message board.
 * @param cardData A structure containing details about the scanned RFID card.
 */
void handleCardScanned(const RFIDCardData& cardData) {
  DEBUG_INFO_PRINTF("Global Callback: Card detected - UID: %s\n", cardData.uid_string.c_str());
  mainUI.showRfidConfirmationDialog(cardData);
}

//=============================================================================
// 5. ARDUINO SETUP FUNCTION
//    Initializes hardware and all system/UI components.
//=============================================================================

/**
 * @brief The Arduino setup function.
 * This function is called once when the ESP32 starts up.
 * It initializes serial communication, seeds the random number generator,
 * and orchestrates the entire system and UI initialization process.
 */
void setup() {
  
  // Initialize Serial Communication for Debugging
  Serial.begin(115200);
  DEBUG_INFO_PRINTLN("\n--- WobysGUI: Setup Initialization ---");
  systemStartTimeMillis = millis(); // Record system boot time
  srand(systemStartTimeMillis);     // Seed random number generator

  // Perform all system and UI initialization via the initializer object
  initializer.init();

  // Set the initial active screen
  screenManager.switchToLayer("main_L_demo");

  // Call onShowLayer for the initially active layer to apply layout
  mainUI.onShowLayer("main_L_demo");

  DEBUG_INFO_PRINTLN("Setup complete.");
}


//=============================================================================
// 6. ARDUINO LOOP FUNCTION
//    The main application loop, continuously updating system and UI states.
//=============================================================================

/**
 * @brief The Arduino loop function.
 * This function is called repeatedly after setup() finishes.
 * It orchestrates the continuous update of all system managers and UI components,
 * processes touch input, and handles screen rendering.
 */
void loop() {

  // Get Raw Touch Coordinates and Pressure State
  int32_t tx, ty;
  bool isPressed = lcd.getTouch(&tx, &ty);

  // Update System Managers
  // NOTE: The order of updates can matter due to dependencies or processing priorities.
  screenSaverManager.onTouch(tx, ty, isPressed); // Screensaver gets first dibs on touch
  screenSaverManager.loop();                     // Updates screensaver state, brightness, clock
  timeManager.loop();                            // Updates internal time, NTP sync
  powerManager.loop();                           // Monitors battery, handles shutdown
  rfidManager.loop();                            // Scans for RFID tags
  btManager.loop();                              // Updates Bluetooth state, connections
  wifiManager.loop();                            // Updates Wi-Fi state, connections, RSSI
  mainUI.loop();                                 // Updates main UI specific elements (e.g., status label, seekbars)
  audioManager.loop();                           // Updates audio manager (e.g., timed sound playback)
  //sdManager.loop();                            // NOTE: SD card loop commented out as requested.

  // Update Main UI (ScreenManager and Statusbar)
  // Statusbar processes its own touch events first (e.g., panel drag, button presses)
  bool touchHandledByStatusbar = statusbar.loop();
  // ScreenManager updates the active UI layer, passing touch events if statusbar didn't handle them.
  screenManager.loop(touchHandledByStatusbar);

  delay(10); // Short delay to prevent busy-waiting and allow other tasks to run (e.g., FreeRTOS tasks)
}