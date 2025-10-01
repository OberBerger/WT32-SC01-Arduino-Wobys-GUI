/**
 * @file IconElement.h
 * @brief Defines the IconElement class, a specialized StatusbarElement for displaying single icons.
 *
 * This class provides a compact UI component for displaying various status icons
 * (e.g., Wi-Fi, Bluetooth, SD card, Battery, Speaker) within the status bar.
 * It allows dynamic updating of the icon character and its color.
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
 */
#pragma once
#ifndef ICONELEMENT_H
#define ICONELEMENT_H

#include <string>
#include "StatusbarElement.h"
#include "Config.h" // For LGFX_Font types and default colors

/**
 * @brief A specialized StatusbarElement for displaying a single icon.
 *
 * This class provides a compact UI component for displaying various status icons
 * within the status bar. It allows dynamic updating of the icon character and its color.
 */
class IconElement : public StatusbarElement {
private:
    const lgfx::IFont* _font;        ///< The font used to draw the icon character.
    std::string _iconString;         ///< The string representing the icon (e.g., a single UTF-8 character).
    uint32_t _iconColor;             ///< The color of the icon.
    uint32_t _backgroundColor;       ///< The background color behind the icon.

public:
    /**
     * @brief Constructor for the IconElement.
     * @param lcd Pointer to the LGFX display instance.
     * @param width The fixed width of the icon element in the status bar.
     * @param font The font to use for rendering the icon character.
     * @param initialStr The initial icon string (e.g., a single character glyph). Defaults to a space.
     * @param color The initial color of the icon. Defaults to white.
     * @param elementName Optional debug name for the element.
     */
    IconElement(LGFX* lcd, int32_t width, const lgfx::IFont* font = &helvR10, // Default font for StatusbarElement text
                const std::string& initialStr = " ", uint32_t color = TFT_WHITE,
                const std::string& elementName = "");
    
    /**
     * @brief Draws the icon element onto the display.
     * This method is called by the StatusbarUI to render the element.
     * @param x The absolute X coordinate on the screen where the element should be drawn.
     * @param y The absolute Y coordinate on the screen where the element should be drawn.
     * @param actualWidth The actual width allocated for the element in the status bar.
     */
    virtual void draw(int32_t x, int32_t y, int32_t actualWidth) override;
    
    /**
     * @brief Sets the icon character (glyph) to be displayed.
     * Requests a redraw if the icon changes.
     * @param newStr The new icon string (e.g., a single UTF-8 character).
     */
    void setIcon(const std::string& newStr);

    /**
     * @brief Sets the icon character (glyph) to be displayed.
     * Requests a redraw if the icon changes.
     * @param newChar The new icon character.
     */
    void setIcon(char newChar);

    /**
     * @brief Sets the color of the icon.
     * Requests a redraw if the color changes.
     * @param color The new color for the icon.
     */
    void setIconColor(uint32_t color);

    /**
     * @brief Sets the background color of the icon element.
     * Requests a redraw if the color changes.
     * @param color The new background color.
     */
    void setBackgroundColor(uint32_t color);

    /**
     * @brief Retrieves the current icon string.
     * @return The string representing the current icon.
     */
    std::string getIcon() const { return _iconString; }
};

#endif // ICONELEMENT_H