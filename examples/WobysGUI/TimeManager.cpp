/**
 * @file TimeManager.cpp
 * @brief Implements the TimeManager class for system time management and NTP synchronization.
 *
 * This file provides the core logic for synchronizing the system clock with NTP
 * servers, handling time zone offsets, and preparing formatted time strings for UI
 * display. It includes mechanisms for periodic synchronization attempts and manages
 * a blinking colon state for visual feedback.
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
#include "TimeManager.h"
#include <time.h>     // Required for `struct tm` and `getLocalTime()`
#include <sys/time.h> // Required for `settimeofday()` or underlying time functions
#include <Arduino.h>  // Required for `millis()`, `configTime()`

// --- Constructor Implementation ---
/**
 * @brief Constructor for the TimeManager class.
 * Initializes internal state and sets up NTP parameters.
 * @param wifiMgr A pointer to the `WifiManager` instance.
 * @param gmtOffsetSec The GMT offset in seconds.
 * @param daylightOffsetSec The daylight saving offset in seconds.
 * @param ntpServer The address of the NTP server.
 */
TimeManager::TimeManager(WifiManager* wifiMgr,
                         int gmtOffsetSec,
                         int daylightOffsetSec,
                         const char* ntpServer)
  : _wifiMgr(wifiMgr),
    _gmtOffsetSec(gmtOffsetSec),
    _daylightOffsetSec(daylightOffsetSec),
    _ntpServer(ntpServer),
    _syncState(UNSYNCED),          // Initial state is unsynchronized.
    _lastSyncAttemptMs(0),
    _lastDisplayMs(0),
    _blink(false),                 // Colon initially not blinking.
    _lastMinute(-1),               // Initialize with invalid minute to force first update.
    _cachedTimeString("--:--"),    // Initial cached time string.
    _cachedColonVisible(true)      // Colon visible by default for initial display.
{
  DEBUG_INFO_PRINTLN("TimeManager: Constructor executed.");
}

// --- Initialization & Lifecycle ---
/**
 * @brief Initializes the TimeManager.
 * This method should be called once during system setup. It resets the synchronization state.
 */
void TimeManager::begin() {
  DEBUG_INFO_PRINTLN("TimeManager: Begin called. Resetting sync state.");
  _syncState = UNSYNCED;
  _lastSyncAttemptMs = 0;
  _lastDisplayMs = 0;
  _blink = false;
  _lastMinute = -1;
  _cachedTimeString = "--:--";
  _cachedColonVisible = true;
}

/**
 * @brief Retrieves the currently cached formatted time string (e.g., "HH:MM").
 * @return The formatted time string.
 */
std::string TimeManager::getCurrentTimeString() const {
    return _cachedTimeString;
}

/**
 * @brief Checks if the colon in the time string should currently be visible (for blinking effect).
 * @return True if the colon should be visible, false otherwise.
 */
bool TimeManager::isColonVisible() const {
    return _cachedColonVisible;
}

/**
 * @brief Main update method for the TimeManager.
 * This function should be called repeatedly in the Arduino `loop()` function.
 * It handles periodic NTP synchronization attempts and updates the cached time string
 * and colon visibility flag for efficient UI rendering.
 */
void TimeManager::loop() {
  unsigned long now = millis();

  // 1) NTP Synchronization Logic.
  // Attempt initial sync after a short delay, or resync periodically.
  unsigned long sinceSync = now - _lastSyncAttemptMs;
  if ((_syncState == UNSYNCED && sinceSync >= FIRST_RETRY_MS) || (_syncState == SYNCED && sinceSync >= RESYNC_INTERVAL_MS)) {
    // Only attempt sync if Wi-Fi is connected.
    if (_wifiMgr && _wifiMgr->getCurrentState() == WifiMgr_State_t::CONNECTED) {
      DEBUG_INFO_PRINTLN("TimeManager: Attempting NTP synchronization...");
      configTime(_gmtOffsetSec, _daylightOffsetSec, _ntpServer);
      struct tm timeinfo;
      // Wait up to 2 seconds for time to become available.
      if (getLocalTime(&timeinfo, 2000)) {
        _syncState = SYNCED;
        _lastDisplayMs = now;
        _blink = (timeinfo.tm_sec % 2 == 0); // Initialize blink state based on current second.
        _lastMinute = timeinfo.tm_min;       // Store current minute for change detection.

        // Immediately update cached time string and colon visibility after successful sync.
        char buf[6]; // Sufficient for "HH:MM\0".
        snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        _cachedTimeString = std::string(buf);
        _cachedColonVisible = _blink;
        DEBUG_INFO_PRINTF("TimeManager: NTP sync successful. Time: %s (Colon visible: %s).\n", _cachedTimeString.c_str(), _cachedColonVisible ? "true" : "false");

      } else {
        DEBUG_WARN_PRINTLN("TimeManager: NTP sync failed to get local time.");
        // If sync fails, `_syncState` remains `UNSYNCED` if it was, or we try again if `SYNCED` and resync interval passes.
      }
      _lastSyncAttemptMs = now; // Record time of sync attempt.
    } else if (_wifiMgr) {
      DEBUG_TRACE_PRINTF("TimeManager: Wi-Fi not connected (State: %d), deferring NTP sync attempt.\n", (int)_wifiMgr->getCurrentState());
    }
  }

  // 2) Update cached time string and colon visibility for UI.
  if (_syncState == SYNCED) {
    struct tm timeinfo;
    // Get local time with a short timeout to avoid blocking.
    if (!getLocalTime(&timeinfo, 50)) {
      DEBUG_TRACE_PRINTLN("TimeManager: Failed to get local time during update, skipping.");
      return; // Skip update if time not available.
    }

    char buf[6]; // Sufficient for "HH:MM\0".
    snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    
    // Update cached time string if minute or hour changes.
    if (_lastMinute != timeinfo.tm_min) {
        _cachedTimeString = std::string(buf);
        _lastMinute = timeinfo.tm_min;
        DEBUG_TRACE_PRINTF("TimeManager: Cached time string updated to %s.\n", _cachedTimeString.c_str());
    }

    // Update colon visibility every second for blinking effect.
    bool currentColonVisible = (timeinfo.tm_sec % 2 == 0);
    if (_cachedColonVisible != currentColonVisible) {
        _cachedColonVisible = currentColonVisible;
        DEBUG_TRACE_PRINTF("TimeManager: Cached colon visibility updated to %s.\n", _cachedColonVisible ? "true" : "false");
    }
  }
}