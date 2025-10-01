/**
 * @file TimeElement.h
 * @brief Defines the TimeElement class, a specialized StatusbarElement for displaying the current time.
 *
 * This class fetches time information from a TimeManager and renders it in the status bar.
 * It supports a blinking colon and optimized redraws for efficiency.
 *
 * @version 1.0.0
 * @date 2025-08-17
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
#ifndef TIMEELEMENT_H
#define TIMEELEMENT_H

#include "StatusbarElement.h"
#include "TimeManager.h"
#include <string>

/**
 * @brief A specialized StatusbarElement for displaying the current time.
 *
 * This class is responsible for fetching time data from a TimeManager and
 * rendering it in the status bar. It optimizes drawing by only updating
 * parts of the display that have changed (e.g., blinking colon).
 */
class TimeElement : public StatusbarElement {
private:
    TimeManager* _timeManager;                  ///< Pointer to the TimeManager instance to get time data.
    std::string _lastDisplayedTime;             ///< Stores the last displayed time string for change detection.
    bool _lastColonVisible;                     ///< Stores the last colon visibility state for blinking.
    bool _forceFullRedraw;                      ///< Flag to force a complete redraw of the element.
    int16_t _verticalAdjustmentPixels;          ///< Vertical adjustment for centering the time text.
    uint32_t _textColor;                        ///< Color of the time text.
    uint32_t _backgroundColor;                  ///< Background color of the time element area.
    const lgfx::IFont* _font;                   ///< The font used for rendering the time.

public:
    /**
     * @brief Constructor for the TimeElement.
     * @param lcd Pointer to the LGFX display instance.
     * @param width The fixed width of the time element in the status bar.
     * @param timeManager Pointer to the TimeManager instance.
     * @param elementName Optional debug name for the element.
     * @param font The font to use for rendering the time (default: helvR10).
     */
    TimeElement(LGFX* lcd, int32_t width, TimeManager* timeManager,
                const std::string& elementName = "Time", 
                const lgfx::IFont* font = &helvR10); // Default font

    /**
     * @brief Draws the time element onto the display.
     * This method renders the current time, optimizing for partial updates (e.g., colon blinking).
     * @param x The absolute X coordinate on the screen where the element should be drawn.
     * @param y The absolute Y coordinate on the screen where the element should be drawn.
     * @param actualWidth The actual width allocated for the element in the status bar.
     */
    virtual void draw(int32_t x, int32_t y, int32_t actualWidth) override;
    
    /**
     * @brief Updates the state of the time element.
     * This method is called repeatedly to check for time changes (minute or colon blink)
     * and request appropriate redraws.
     */
    void update() override;

    /**
     * @brief Forces a complete redraw of the time element in the next drawing cycle.
     * This is useful when the background or other visual properties change.
     */
    void forceFullRedraw() override;

    /**
     * @brief Sets a vertical adjustment offset for centering the time text.
     * Requests a redraw if the adjustment changes.
     * @param pixels The vertical offset in pixels.
     */
    void setVerticalAdjustment(int16_t pixels);

    /**
     * @brief Sets the color of the time text.
     * Requests a redraw if the color changes.
     * @param color The new color for the time text.
     */
    void setTextColor(uint32_t color); 

    /**
     * @brief Sets the background color of the time element area.
     * Requests a redraw if the color changes.
     * @param color The new background color.
     */
    void setBackgroundColor(uint32_t color);

    /**
     * @brief Sets the font used for rendering the time.
     * Requests a redraw if the font changes.
     * @param font The new font.
     */
    void setFont(const lgfx::IFont* font);
};

#endif // TIMEELEMENT_H