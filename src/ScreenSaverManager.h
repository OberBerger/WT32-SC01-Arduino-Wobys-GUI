/**
 * @file ScreenSaverManager.h
 * @brief Manages display dimming, screensaver activation, and unlock gestures.
 *
 * This class orchestrates the automatic dimming of the display and activation
 * of a screensaver based on user inactivity. It handles brightness transitions,
 * displays a customizable clock, and processes touch inputs for user interaction
 * and deactivation of the screensaver.
 *
 * @version 1.0.3
 * @date 2025-08-20
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
 * This product incorporates software components licensed under various open-source licenses,
 * including the LovyanGFX library and FreeRTOS components (indirectly).
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#ifndef SCREENSAVERMANAGER_H
#define SCREENSAVERMANAGER_H

#include <LovyanGFX.hpp>
#include <string>

// Project-specific includes
#include "ScreenManager.h"
#include "TimeManager.h"
#include "ClockLabelUI.h"
#include "Config.h"        // General project configuration
#include "ConfigUIUser.h"  // UI-specific configuration for colors, sizes etc.
#include "SystemInitializer.h" // Forward declaration for ScreenSaverManagerConfig

// Forward declarations to break circular dependencies or for external types
class StatusbarUI;
struct ScreenSaverManagerConfig;

/**
 * @brief Manages display dimming, screensaver activation, and unlock gestures.
 *
 * This class is responsible for monitoring user inactivity, dimming the display,
 * activating a screensaver (displaying a clock and an unlock arrow), and
 * deactivating the screensaver based on touch gestures (swipe or tap).
 */
class ScreenSaverManager {
private:
    // --- Dependencies ---
    LGFX* _lcd;                     ///< Pointer to the LGFX display object.
    ScreenManager* _screenManager;  ///< Pointer to the ScreenManager for UI layer management.
    StatusbarUI* _statusbar;        ///< Pointer to the StatusbarUI for touch handling and panel management.
    TimeManager* _timeManager;      ///< Pointer to the TimeManager for current time and colon visibility.
    ClockLabelUI* _screenSaverClock; ///< Pointer to the ClockLabelUI used for the screensaver display.

    // --- Internal State ---
    /**
     * @brief Enumerates the different states of the screensaver manager.
     */
    enum class State {
        INACTIVE,       ///< Screensaver is off, display at normal brightness.
        FADING_OUT,     ///< Display is dimming towards screensaver brightness.
        SAVER_DIM,      ///< Screensaver is active, display is at dimmed brightness.
        BRIGHTENING,    ///< Display is brightening towards temporary bright screensaver level.
        SAVER_BRIGHT,   ///< Screensaver is active, display is at temporary bright brightness.
        DIMMING         ///< Display is dimming from temporary bright to dimmed screensaver level.
    };
    State _currentState;            ///< The current state of the screensaver manager.

    // --- Animation & Timing Variables ---
    unsigned long _animStartTime;       ///< Timestamp when the current animation (fade, brighten, dim) started.
    unsigned long _lastInteractionTime; ///< Timestamp of the last user interaction (touch).
    unsigned long _brightStateStartTime;///< Timestamp when `SAVER_BRIGHT` state was entered.
    unsigned long _arrowAnimationTime;  ///< Timestamp when the unlock arrow animation started.
    uint8_t _arrowPulseValue;           ///< Current pulse value for the unlock arrow animation (unused, kept for reference).
    bool _wasArrowDrawnLastFrame;       ///< Flag to track if the arrow was drawn in the previous frame (for clearing).

    // --- Clock Display Cache ---
    std::string _lastDisplayedScreenSaverTime; ///< Cached time string to optimize clock redraws.
    bool _lastScreenSaverColonVisible;         ///< Cached colon visibility state to optimize clock redraws.
    bool _forceScreenSaverClockFullRedraw;     ///< Flag to force a full redraw of the clock (e.g., after activation).

    // --- Touch Interaction Variables ---
    bool _isTouchActive;        ///< True if a touch is currently active on the screen.
    int32_t _touchStartX, _touchStartY; ///< Coordinates where the touch interaction started.
    unsigned long _touchStartTime;      ///< Timestamp when the touch interaction started.

    // --- Brightness State ---
    uint8_t _originalBrightness; ///< Stores the display brightness before screensaver activation.

    // --- Configuration Parameters (from ScreenSaverManagerConfig) ---
    unsigned long _timeoutMs;           ///< Inactivity timeout before screensaver activation (milliseconds).
    unsigned long _brightDurationMs;    ///< Duration for the screen to stay bright after a tap (milliseconds, unused).
    uint8_t _saverBrightnessDim;        ///< Brightness level for the dimmed screensaver state (0-255).
    uint8_t _saverBrightnessBright;     ///< Brightness level for the temporarily bright screensaver state (0-255).
    int32_t _swipeThresholdY;           ///< Vertical swipe threshold for unlock gesture (pixels).
    unsigned long _tapMaxDurationMs;    ///< Maximum duration for a touch to be considered a tap (milliseconds).
    unsigned long _brightenDurationMs;  ///< Duration for display to brighten from dimmed to bright screensaver (milliseconds).
    unsigned long _dimDurationMs;       ///< Duration for display to dim from bright to dimmed screensaver (milliseconds).
    unsigned long _brightHoldDurationMs;///< Duration to hold the bright screensaver state after a tap (milliseconds).
    unsigned long _fadeOutDurationMs;   ///< Duration for display to fade out from original to dimmed brightness (milliseconds).
    unsigned long _pulseCycleDurationMs;///< Duration of one complete pulse cycle for the unlock arrow (milliseconds).
    uint8_t _minArrowIntensity;         ///< Minimum intensity (0-255) for the pulsing unlock arrow.
    int32_t _arrowWidth;                ///< Width of the unlock arrow in pixels.
    int32_t _arrowHeight;               ///< Height of the unlock arrow in pixels.
    int32_t _arrowOffsetBottom;         ///< Offset of the unlock arrow from the bottom of the screen in pixels.
    uint32_t _arrowBaseColor;           ///< Base color of the unlock arrow.
    bool _isEnabled;                    ///< Flag indicating if the screensaver functionality is enabled.

    // --- Private Helper Methods ---
    /**
     * @brief Activates the screensaver, pushing its UI layer and setting the initial state.
     * This method is called after the display has faded out.
     */
    void activate();

    /**
     * @brief Deactivates the screensaver, restoring original brightness and UI control.
     * This method handles popping the screensaver UI layer and re-enabling touch
     * for the status bar and the underlying UI layer (e.g., MainUI).
     */
    void deactivate();

    /**
     * @brief Processes unlock gestures detected upon touch release.
     * Checks for specific swipe or tap patterns to deactivate the screensaver or
     * modify its bright hold timer.
     * @param x Absolute X-coordinate of the touch release.
     * @param y Absolute Y-coordinate of the touch release.
     */
    void handleUnlockGesture(int32_t x, int32_t y);

    /**
     * @brief Draws a static triangular arrow at the bottom of the screen.
     * This arrow serves as a visual cue for unlocking the device.
     * @param color The color to draw the arrow with.
     */
    void drawStaticUnlockArrow(uint32_t color);

    /**
     * @brief Calculates the pulsing color for the unlock arrow.
     * The arrow's intensity pulsates over time based on a sine wave.
     * @param currentTime The current timestamp in milliseconds, used for animation timing.
     * @return The calculated 24-bit RGB color for the arrow.
     */
    uint32_t calculatePulsingArrowColor(unsigned long currentTime);

    /**
     * @brief Updates the screensaver clock display.
     * Fetches the current time and colon visibility from the `TimeManager`
     * and updates the `_screenSaverClock` UI element if changes are detected.
     */
    void updateScreensaverClock();

public:
    // --- Constructor ---
    /**
     * @brief Constructor for the ScreenSaverManager class.
     * Initializes member variables with default values and registers dependencies.
     * @param lcd Pointer to the LGFX display object.
     * @param sm Pointer to the ScreenManager for UI layer management.
     * @param sb Pointer to the StatusbarUI for touch interaction and panel management.
     * @param tm Pointer to the TimeManager for time-related operations.
     * @param clock Pointer to the ClockLabelUI instance used for the screensaver display.
     */
    ScreenSaverManager(LGFX* lcd, ScreenManager* sm, StatusbarUI* sb, TimeManager* tm, ClockLabelUI* clock);

    // --- Initialization & Main Loop ---
    /**
     * @brief Initializes the ScreenSaverManager with configuration parameters.
     * Sets various timeouts, brightness levels, gesture thresholds, and animation parameters.
     * @param config A constant reference to a `ScreenSaverManagerConfig` struct.
     */
    void init(const ScreenSaverManagerConfig& config);

    /**
     * @brief Handles touch input events for the screensaver.
     * This method is called from the main touch handler to process interactions
     * relevant to screensaver activation and deactivation.
     * @param x Absolute X-coordinate of the touch event.
     * @param y Absolute Y-coordinate of the touch event.
     * @param isPressed True if the screen is currently being pressed, false if released.
     */
    void onTouch(int32_t x, int32_t y, bool isPressed);

    /**
     * @brief Main loop function for the ScreenSaverManager.
     * This method should be called repeatedly in the main application loop.
     * It manages state transitions, display brightness adjustments, and animations.
     */
    void loop();

    // --- Status Getters ---
    /**
     * @brief Checks if the screensaver is currently in an active state (i.e., not INACTIVE).
     * @return True if the screensaver is active, false otherwise.
     */
    bool isActive() const;

    /**
     * @brief Checks if the screensaver functionality is currently enabled via settings.
     * @return True if enabled, false otherwise.
     */
    bool isEnabled() const;

    // --- Configuration Setters ---
    /**
     * @brief Enables or disables the screensaver functionality.
     * If disabled while active, it will immediately deactivate the screensaver.
     * @param enabled True to enable the screensaver, false to disable.
     */
    void setEnabled(bool enabled);

    /**
     * @brief Sets the inactivity timeout duration for screensaver activation.
     * @param timeoutMs The new timeout duration in milliseconds.
     */
    void setTimeout(unsigned long timeoutMs);

    /**
     * @brief Sets the dimmed brightness level for the screensaver.
     * If the screensaver is currently in a dimmed or fading state, the new brightness is applied immediately.
     * @param brightness The new dimmed brightness level (0-255).
     */
    void setDimmedBrightness(uint8_t brightness);

    /**
     * @brief Sets the width of the unlock arrow in pixels.
     * Forces a redraw of the arrow in the next frame to reflect the change.
     * @param width The new width in pixels.
     */
    void setArrowWidth(int32_t width);

    /**
     * @brief Sets the height of the unlock arrow in pixels.
     * Forces a redraw of the arrow in the next frame to reflect the change.
     * @param height The new height in pixels.
     */
    void setArrowHeight(int32_t height);

    /**
     * @brief Sets the offset of the unlock arrow from the bottom of the screen.
     * Forces a redraw of the arrow in the next frame to reflect the change.
     * @param offset The new offset in pixels from the bottom.
     */
    void setArrowOffsetBottom(int32_t offset);

    /**
     * @brief Sets the minimum intensity (brightness) for the pulsing unlock arrow.
     * The arrow's brightness will pulse between this minimum and full brightness.
     * Forces a redraw of the arrow in the next frame.
     * @param intensity The new minimum intensity (0-255).
     */
    void setMinArrowIntensity(uint8_t intensity);

    /**
     * @brief Sets the base color of the unlock arrow.
     * Forces a redraw of the arrow in the next frame to reflect the change.
     * @param color The new base color (24-bit RGB).
     */
    void setArrowBaseColor(uint32_t color);
};

#endif // SCREENSAVERMANAGER_H