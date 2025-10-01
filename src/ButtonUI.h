/**
 * @file ButtonUI.h
 * @brief Defines the ButtonUI class, a fundamental UI element for interactive buttons.
 *
 * This class provides a customizable button component with various visual states
 * (normal, pressed, disabled) and robust touch interaction logic, including
 * "sticky" touch detection and callback functionality. It is designed to be flexible
 * for use across different UI layouts.
 *
 * @version 1.0.4
 * @date 2025-08-23
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
#ifndef BUTTONUI_H
#define BUTTONUI_H

#include "UIElement.h"
#include "Config.h" // For UI colors, fonts, LGFX, DEBUG macros.
#include <string>
#include <functional> // For std::function.

/**
 * @brief A versatile UI element for interactive buttons.
 *
 * The ButtonUI class provides a customizable button component with various
 * visual states (normal, pressed, disabled) and robust touch interaction logic.
 * It features "sticky" touch detection, allowing a wider release area for
 * triggering the button, and executes a user-defined callback on release.
 */
class ButtonUI : public UIElement {
public:
    /**
     * @brief Callback type for button press/release events.
     */
    using ButtonCallback = std::function<void()>;

protected:
    // --- Layout & Sizing ---
    int16_t _x_rel;                     ///< Relative X position of the button's top-left corner within its parent layer.
    int16_t _y_rel;                     ///< Relative Y position of the button's top-left corner within its parent layer.
    int16_t _width;                     ///< Configured width of the button.
    int16_t _height;                    ///< Configured height of the button.
    uint8_t _cornerRadius;              ///< Radius for rounded corners of the button.

    // --- Visual Properties ---
    std::string _label;                 ///< Text label displayed on the button.
    const lgfx::IFont* _font;           ///< The font used for rendering the button's label.
    uint32_t _outlineColor;             ///< Color of the button's outline.
    uint32_t _fillColorNormal;          ///< Fill color when the button is in its normal (unpressed) state.
    uint32_t _fillColorPressed;         ///< Fill color when the button is in its pressed state.
    uint32_t _textColorNormal;          ///< Text color when the button is in its normal state.
    uint32_t _textColorPressed;         ///< Text color when the button is in its pressed state.
    
    // --- Interactive State & Logic ---
    bool _isPressedState;               ///< True if the button is currently visually in a pressed state.
    bool _wasEverPressedInThisCycle;    ///< Flag indicating if the button was ever pressed during the current touch cycle (press to release).
    int16_t _stickyMargin;              ///< Margin around the button for "sticky" touch detection.
    int16_t _pressDetectionTolerance;   ///< Pixel tolerance for initial press detection (hotspot area).
    ButtonCallback _onPressCallback;    ///< Callback function executed when the button is initially pressed.
    ButtonCallback _onReleaseCallback;  ///< Callback function executed when the button is released.

public:
    // --- Constructor ---
    /**
     * @brief Constructor for the ButtonUI class.
     * Initializes the button with default or specified properties.
     * @param lcd Pointer to the LGFX display instance.
     * @param label The initial text label for the button.
     * @param x The X coordinate of the button's origin.
     * @param y The Y coordinate of the button's origin.
     * @param w The width of the button.
     * @param h The height of the button.
     * @param font The font to use for rendering the label (default: helvB12).
     * @param cornerRadius The radius for rounded corners (default: BUTTON_DEFAULT_CORNER_RADIUS_PIXELS).
     */
    ButtonUI(LGFX* lcd,
             const std::string& label,
             int16_t x, int16_t y, int16_t w, int16_t h,
             const lgfx::IFont* font = &helvB12, 
             uint8_t cornerRadius = BUTTON_DEFAULT_CORNER_RADIUS_PIXELS);

    // --- Visual Property Setters ---
    /**
     * @brief Sets all color properties for the button.
     * Requests a redraw.
     * @param outline Color of the button's outline.
     * @param fillNormal Fill color when normal.
     * @param fillPressed Fill color when pressed.
     * @param textNormal Text color when normal.
     * @param textPressed Text color when pressed.
     */
    void setColors(uint32_t outline, uint32_t fillNormal, uint32_t fillPressed,
                   uint32_t textNormal, uint32_t textPressed);
    
    /**
     * @brief Sets the text label for the button.
     * Requests a redraw if the label changes.
     * @param label The new text label.
     */
    void setLabel(const std::string& label);
    
    /**
     * @brief Sets the font for rendering the button's label.
     * Requests a redraw if the font changes.
     * @param font The new font.
     */
    void setFont(const lgfx::IFont* font);

    // --- Layout & Sizing Setters (Overrides from UIElement) ---
    /**
     * @brief Sets the relative position of the button within its parent layer.
     * Requests a redraw.
     * @param x The new relative X coordinate.
     * @param y The new relative Y coordinate.
     */
    void setPosition(int16_t x, int16_t y) override;
    
    /**
     * @brief Sets the size (width and height) of the button.
     * Adjusts the sticky margin proportionally if it was set to default.
     * Requests a redraw.
     * @param w The new width.
     * @param h The new height.
     */
    void setSize(int16_t w, int16_t h) override;

    // --- Interactive Property Setters ---
    /**
     * @brief Sets the margin around the button for "sticky" touch detection.
     * @param margin The sticky margin in pixels.
     */
    void setStickyMargin(int16_t margin);
    
    /**
     * @brief Sets the pixel tolerance for initial press detection.
     * @param tolerance The press detection tolerance in pixels.
     */
    void setPressDetectionTolerance(int16_t tolerance);

    /**
     * @brief Sets the callback function to be executed when the button is initially pressed.
     * @param callback The callback function.
     */
    void setOnPressCallback(ButtonCallback callback);
    
    /**
     * @brief Sets the callback function to be executed when the button is released.
     * @param callback The callback function.
     */
    void setOnReleaseCallback(ButtonCallback callback) override;

    // --- Visual Property Getters ---
    /**
     * @brief Retrieves the current relative X coordinate of the button.
     * @return The relative X coordinate.
     */
    int16_t getX() const;
    
    /**
     * @brief Retrieves the current relative Y coordinate of the button.
     * @return The relative Y coordinate.
     */
    int16_t getY() const;
    
    /**
     * @brief Retrieves the current width of the button.
     * @return The width in pixels.
     */
    int16_t getWidth() const override;
    
    /**
     * @brief Retrieves the current height of the button.
     * @return The height in pixels.
     */
    int16_t getHeight() const override;
    
    /**
     * @brief Retrieves the current corner radius of the button.
     * @return The corner radius in pixels.
     */
    uint8_t getCornerRadius() const;
    
    /**
     * @brief Retrieves the current font used for the button's label.
     * @return A pointer to the current font.
     */
    const lgfx::IFont* getFont() const;
    
    /**
     * @brief Retrieves the current text label of the button.
     * @return A const reference to the label string.
     */
    const std::string& getLabel() const;
    
    /**
     * @brief Retrieves the current outline color of the button.
     * @return The outline color.
     */
    uint32_t getOutlineColor() const;
    
    /**
     * @brief Retrieves the current normal fill color of the button.
     * @return The normal fill color.
     */
    uint32_t getFillColorNormal() const;
    
    /**
     * @brief Retrieves the current pressed fill color of the button.
     * @return The pressed fill color.
     */
    uint32_t getFillColorPressed() const;
    
    /**
     * @brief Retrieves the current normal text color of the button.
     * @return The normal text color.
     */
    uint32_t getTextColorNormal() const;
    
    /**
     * @brief Retrieves the current pressed text color of the button.
     * @return The pressed text color.
     */
    uint32_t getTextColorPressed() const;

    // --- Core UIElement Overrides ---
    /**
     * @brief Draws the button onto the display.
     * Renders the button with its current visual state (normal, pressed, disabled)
     * and applies appropriate colors.
     */
    void draw() override;
    
    /**
     * @brief Updates the internal state of the button.
     * For ButtonUI, this method is typically empty, as its state changes are
     * driven by touch input.
     */
    void update() override;
    
    /**
     * @brief Handles touch input for the button.
     * Manages visual feedback (pressed state) and triggers callbacks on release
     * within the "sticky" touch zone.
     * @param tx_layer The X coordinate of the touch input (relative to the layer).
     * @param ty_layer The Y coordinate of the touch input (relative to the layer).
     * @param isCurrentlyPressed True if the screen is currently pressed, false otherwise.
     * @return True if the button handled the touch event, false otherwise.
     */
    bool handleTouch(int32_t tx_layer, int32_t ty_layer, bool isCurrentlyPressed) override;
    
    /**
     * @brief Overrides the base class method to provide the specific UI element type for ButtonUI.
     * @return The UIElementType enum value representing a BUTTON element.
     */
    UIElementType getElementType() const override { return UIElementType::BUTTON; }
};

#endif // BUTTONUI_H