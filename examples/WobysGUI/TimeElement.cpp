/**
 * @file TimeElement.cpp
 * @brief Implements the TimeElement class, a specialized StatusbarElement for displaying the current time.
 *
 * This file provides the implementation details for drawing and managing the time display
 * within the status bar, handling time updates, colon blinking, and optimized rendering.
 *
 * @version 1.0.0
 * @date 2025-08-17
 * @author György Oberländer
 * @contact support@wobys.com
 *
 * @copyright (c) 2025 György Oberländer. All Rights Reserved.
 *
 * This software is proprietary and confidential. Unauthorized copying of this file,
 * via any medium, is strictly prohibited.
 *
 * IMPORTANT NOTE ON THIRD-PARTY LICENSES:
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */
#include "TimeElement.h"
#include "Config.h" // Required for debug macros and UI_COLOR_TEXT_DEFAULT constant

/**
 * @brief Constructor for the TimeElement.
 * @param lcd Pointer to the LGFX display instance.
 * @param width The fixed width of the time element in the status bar.
 * @param timeManager Pointer to the TimeManager instance.
 * @param elementName Optional debug name for the element.
 * @param font The font to use for rendering the time.
 */
TimeElement::TimeElement(LGFX* lcd, int32_t width, TimeManager* timeManager,
                         const std::string& elementName, 
                         const lgfx::IFont* font)    
    : StatusbarElement(lcd, width, elementName), 
      _timeManager(timeManager),
      _lastDisplayedTime("--:--"), // Initial dummy time string.
      _lastColonVisible(true),    // Initial colon state.
      _forceFullRedraw(true),     // Force full redraw on first display.
      _verticalAdjustmentPixels(TIMEELEMENT_VERTICAL_ADJUSTMENT_PIXELS), // Default vertical adjustment.
      _textColor(UI_COLOR_TEXT_DEFAULT),    // Default text color.
      _backgroundColor(PANEL_BACKGROUND_COLOR), // Default background color.
      _font(font)                 // Font for rendering.
{
    DEBUG_INFO_PRINTLN("TimeElement: Constructor executed.");
}

/**
 * @brief Updates the state of the time element.
 * This method is called repeatedly to check for time changes (minute or colon blink)
 * and request appropriate redraws.
 */
void TimeElement::update() {
    if (!_timeManager) return; // Do nothing if TimeManager is not set.

    std::string currentTime = _timeManager->getCurrentTimeString();
    bool colonVisible = _timeManager->isColonVisible();

    // Check if the minute part of the time has changed (requiring full redraw).
    if (currentTime != _lastDisplayedTime) {
        DEBUG_TRACE_PRINTF("TimeElement: Time changed from %s to %s. Forcing full redraw.\n", _lastDisplayedTime.c_str(), currentTime.c_str());
        _lastDisplayedTime = currentTime;
        _lastColonVisible = colonVisible;
        _forceFullRedraw = true; // Flag for full redraw on the next draw cycle.
        requestRedraw();         // Request a redraw.
    } 
    // If only the colon's visibility has changed (for blinking).
    else if (colonVisible != _lastColonVisible) {
        DEBUG_TRACE_PRINTLN("TimeElement: Colon visibility changed. Requesting redraw for blink.");
        _lastColonVisible = colonVisible;
        // _forceFullRedraw remains false, as only the colon needs update, not the whole text.
        requestRedraw(); // Request a redraw.
    }
}

/**
 * @brief Forces a complete redraw of the time element in the next drawing cycle.
 * This is useful when the background or other visual properties change.
 */
void TimeElement::forceFullRedraw() {
    _forceFullRedraw = true; // Set flag to force a full redraw.
    requestRedraw();         // Request a redraw.
    DEBUG_INFO_PRINTLN("TimeElement: Full redraw forced.");
}

/**
 * @brief Sets a vertical adjustment offset for centering the time text.
 * Requests a redraw if the adjustment changes.
 * @param pixels The vertical offset in pixels.
 */
void TimeElement::setVerticalAdjustment(int16_t pixels) {
    if (_verticalAdjustmentPixels != pixels) {
        _verticalAdjustmentPixels = pixels;
        requestRedraw(); // Request redraw if adjustment changes.
        DEBUG_INFO_PRINTF("TimeElement: Vertical adjustment set to %d pixels.\n", pixels);
    }
}

/**
 * @brief Sets the color of the time text.
 * Requests a redraw if the color changes.
 * @param color The new color for the time text.
 */
void TimeElement::setTextColor(uint32_t color) {
    if (_textColor != color) {
        _textColor = color;
        requestRedraw(); // Request redraw if color changes.
        DEBUG_INFO_PRINTF("TimeElement: Text color set to 0x%06X.\n", color);
    }
}

/**
 * @brief Sets the background color of the time element area.
 * Requests a redraw if the color changes.
 * @param color The new background color.
 */
void TimeElement::setBackgroundColor(uint32_t color) {
    if (_backgroundColor != color) {
        _backgroundColor = color;
        requestRedraw(); // Request redraw if color changes.
        DEBUG_INFO_PRINTF("TimeElement: Background color set to 0x%06X.\n", color);
    }
}

/**
 * @brief Sets the font used for rendering the time.
 * Requests a redraw if the font changes.
 * @param font The new font.
 */
void TimeElement::setFont(const lgfx::IFont* font) {
    if (_font != font) {
        _font = font;
        requestRedraw(); // Request redraw if font changes.
        DEBUG_INFO_PRINTLN("TimeElement: Font set.");
    }
}

/**
 * @brief Draws the time element onto the display.
 * This method renders the current time, optimizing for partial updates (e.g., colon blinking).
 * @param x The absolute X coordinate on the screen where the element should be drawn.
 * @param y The absolute Y coordinate on the screen where the element should be drawn.
 * @param actualWidth The actual width allocated for the element in the status bar.
 */
void TimeElement::draw(int32_t x, int32_t y, int32_t actualWidth) {
    // Only draw if a redraw has been requested.
    if (!needsRedraw()) {
        return;
    }

    // --- Calculations needed for both full and partial redraws ---
    _lcd->setFont(_font);
    _lcd->setTextDatum(TL_DATUM); // Set text datum for accurate width calculation.

    size_t splitPos = _lastDisplayedTime.find(':');
    // If colon is not found, or time string is invalid, clear redraw request and return.
    if (splitPos == std::string::npos) { 
        clearRedrawRequest(); 
        DEBUG_WARN_PRINTF("TimeElement: Invalid time string '%s'. Skipping draw.\n", _lastDisplayedTime.c_str());
        return; 
    }
    std::string hourPart = _lastDisplayedTime.substr(0, splitPos);
    std::string minutePart = _lastDisplayedTime.substr(splitPos + 1);

    int hourWidth = _lcd->textWidth(hourPart.c_str());
    int colonWidth = _lcd->textWidth(":");
    int minuteWidth = _lcd->textWidth(minutePart.c_str());
    int totalWidth = hourWidth + colonWidth + minuteWidth;
    
    // Calculate drawing X position to center the time string within the element's width.
    int startX = x + (actualWidth - totalWidth) / 2; // Use actualWidth for centering
    int colonX = startX + hourWidth;
    // Calculate drawing Y position, adjusted for vertical centering and custom offset.
    int textY = y + (_statusBarHeightRef + _verticalAdjustmentPixels - _lcd->fontHeight()) / 2;

    // --- Full Redraw Logic (if requested) ---
    if (_forceFullRedraw) {
        DEBUG_TRACE_PRINTLN("TimeElement: Executing full redraw.");
        // Clear the entire element area with its background color.
        _lcd->fillRect(x, y, actualWidth, _statusBarHeightRef, _backgroundColor);

        // Draw the hour part.
        _lcd->setTextColor(_textColor, _backgroundColor);
        _lcd->drawString(hourPart.c_str(), startX, textY);
        // Draw the minute part.
        _lcd->drawString(minutePart.c_str(), colonX + colonWidth, textY);
        
        _forceFullRedraw = false; // Reset the flag after full redraw.
    }

    // --- Colon-specific Redraw Logic ---
    // The colon is always redrawn if 'draw()' is called (which means 'requestRedraw()' was true).
    // This handles the blinking by drawing either text color or background color.
    uint32_t colonColor = _lastColonVisible ? _textColor : _backgroundColor;
    _lcd->setTextColor(colonColor, _backgroundColor);
    _lcd->drawString(":", colonX, textY);

    clearRedrawRequest(); // Mark that the element has been redrawn.
}