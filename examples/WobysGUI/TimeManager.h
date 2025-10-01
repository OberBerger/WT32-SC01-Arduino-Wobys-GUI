/**
 * @file TimeManager.h
 * @brief Defines the TimeManager class for system time management and NTP synchronization.
 *
 * This class is responsible for obtaining and maintaining accurate system time
 * by synchronizing with Network Time Protocol (NTP) servers. It provides the
 * current time string, manages colon blinking for UI elements, and integrates
 * with the `WifiManager` to ensure network connectivity for NTP services.
 *
 * @version 1.0.1
 * @date 2025-08-19
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
#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <stdint.h>     // Required for `int` types
#include <string>       // Required for `std::string`
#include "WifiManager.h" // Required for `WifiManager` class (NTP dependency)

// Forward declaration for configuration struct (defined in SystemInitializer.h)
struct TimeManagerConfig;

/**
 * @brief The TimeManager class manages system time and NTP synchronization.
 *
 * This manager is responsible for keeping the system clock accurate by
 * periodically synchronizing with NTP servers, leveraging the `WifiManager`
 * for network connectivity. It provides an interface to retrieve the current
 * formatted time string and a flag for colon blinking, which is useful for
 * UI clock displays.
 */
class TimeManager {
public:
  // --- Constructor ---
  /**
   * @brief Constructor for the TimeManager class.
   * Initializes internal state and sets up NTP parameters.
   * @param wifiMgr A pointer to the `WifiManager` instance (required for network access).
   * @param gmtOffsetSec The GMT offset in seconds (e.g., 3600 for +1 hour).
   * @param daylightOffsetSec The daylight saving offset in seconds (e.g., 3600 for +1 hour).
   * @param ntpServer The address of the NTP server (e.g., "pool.ntp.org").
   */
  TimeManager(WifiManager* wifiMgr,
              int gmtOffsetSec,
              int daylightOffsetSec,
              const char* ntpServer);

  // --- Initialization & Lifecycle ---
  /**
   * @brief Initializes the TimeManager.
   * This method should be called once during system setup. It resets the synchronization state.
   */
  void begin();

  /**
   * @brief Main update method for the TimeManager.
   * This function should be called repeatedly in the Arduino `loop()` function.
   * It handles periodic NTP synchronization attempts and updates the cached time string
   * and colon visibility flag for efficient UI rendering.
   */
  void loop();

  // --- Accessors ---
  /**
   * @brief Retrieves the currently cached formatted time string (e.g., "HH:MM").
   * @return The formatted time string.
   */
  std::string getCurrentTimeString() const;

  /**
   * @brief Checks if the colon in the time string should currently be visible (for blinking effect).
   * @return True if the colon should be visible, false otherwise.
   */
  bool isColonVisible() const;

private:
  // --- Dependencies ---
  WifiManager* _wifiMgr;      ///< Pointer to the `WifiManager` instance.

  // --- Configuration ---
  int _gmtOffsetSec;          ///< GMT offset in seconds.
  int _daylightOffsetSec;     ///< Daylight saving offset in seconds.
  const char* _ntpServer;     ///< NTP server address.

  // --- Internal State ---
  enum SyncState { UNSYNCED, SYNCED }; ///< Enumerates NTP synchronization states.
  SyncState _syncState;               ///< Current NTP synchronization state.
  unsigned long _lastSyncAttemptMs;   ///< Timestamp of the last NTP synchronization attempt.
  unsigned long _lastDisplayMs;       ///< Timestamp of the last display update (for colon blinking).
  bool _blink;                        ///< Current state of the colon blink (true for visible, false for hidden).
  int _lastMinute;                    ///< Stores the minute from the last time update (for change detection).

  // --- Cached Values for UI ---
  std::string _cachedTimeString;      ///< Cached formatted time string for quick access by UI.
  bool _cachedColonVisible;           ///< Cached colon visibility state for UI blinking.

  // --- Constants ---
  static constexpr unsigned long FIRST_RETRY_MS   = 30UL * 1000UL; ///< Initial retry interval for NTP sync (30 seconds).
  static constexpr unsigned long RESYNC_INTERVAL_MS = 4UL * 60UL * 60UL * 1000UL; ///< Interval for regular NTP resynchronization (4 hours).
};

#endif // TIMEMANAGER_H