/**
 * @file ToggleSwitchUI.h
 * @brief Defines the ToggleSwitchUI class, an interactive switch component.
 *
 * This file contains the declaration of the ToggleSwitchUI class, which provides a visual
 * toggle switch element, combining a textual label with an interactive on/off switch.
 * It supports customizable appearance, alignment, and state change callbacks,
 * including a smooth animation for the knob movement.
 *
 * @version 1.0.9
 * @date 2025-08-15
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
#ifndef TOGGLESWITCHUI_H
#define TOGGLESWITCHUI_H

#include "UIElement.h" // Base UI Element definition
#include "Config.h"    // General configuration (e.g., UI_COLOR_XXX, default dimensions)
#include <string>      // For std::string
#include <functional>  // For std::function

/**
 * @brief An interactive UI element that functions as a toggle switch.
 *
 * The ToggleSwitchUI class provides a component with a label and a visual switch
 * that can be toggled between ON and OFF states. It supports horizontal alignment
 * of its content within a given bounding box, animated knob movement, and custom
 * colors for different states.
 */
class ToggleSwitchUI : public UIElement {
public:
    /**
     * @brief Callback type for state changes.
     * @param newState The new state of the toggle switch (true for ON, false for OFF).
     */
    using StateChangedCallback = std::function<void(bool)>;

private:
    // --- Bounding Box and Alignment ---
    int16_t _x_rel;      ///< Relative X position of the toggle switch's top-left corner within its parent layer.
    int16_t _y_rel;      ///< Relative Y position of the toggle switch's top-left corner within its parent layer.
    int16_t _width;      ///< Total width of the toggle switch's bounding box.
    int16_t _height;     ///< Total height of the toggle switch's bounding box.
    textdatum_t _alignment; ///< Alignment of the content within the bounding box.

    // --- Content Properties ---
    std::string _title;             ///< Text label displayed next to the switch.
    const lgfx::IFont* _titleFont;  ///< Font used for the title text.
    uint32_t _titleColor;           ///< Color of the title text.
    int16_t _titleWidth;            ///< Calculated actual pixel width of the title text.
    int16_t _switchTrackWidth;      ///< Width of the switch track.
    int16_t _trackHeight;           ///< Height of the switch track.
    int16_t _knobRadius;            ///< Radius of the circular knob.
    uint8_t _titlePaddingY;         ///< Vertical padding for the title text within its area.

    // --- Calculated Drawing Positions ---
    int16_t _actualContentWidth;  ///< Calculated total width of the combined title and switch.
    int16_t _actualContentHeight; ///< Calculated total height of the combined title and switch.
    int16_t _drawOffsetX;         ///< X offset for drawing the content within its bounding box, based on alignment.
    int16_t _drawOffsetY;         ///< Y offset for drawing the content within its bounding box, based on alignment.

    // --- Colors ---
    uint32_t _trackColor;           ///< Color of the switch track.
    uint32_t _onColorKnob;          ///< Color of the knob when the switch is ON.
    uint32_t _onColorKnobCenter;    ///< Color of the knob's center when the switch is ON.
    uint32_t _offColorKnob;         ///< Color of the knob when the switch is OFF.
    uint32_t _offColorKnobCenter;   ///< Color of the knob's center when the switch is OFF.
    uint32_t _layerBackgroundColor; ///< Background color of the parent layer, used for clearing.

    // --- State and Interaction ---
    bool _isOn;                               ///< Current state of the switch (true for ON, false for OFF).
    bool _wasPressed;                         ///< Internal flag to track if the switch was pressed for touch handling.
    StateChangedCallback _onStateChangedCallback; ///< Callback function triggered when the state changes.

    // --- Animáció ---
    float _currentKnobX_rel_track; ///< Current X position of the knob's center, relative to the track's start.
    float _targetKnobX_rel_track;  ///< Target X position for the knob, based on the switch state.
    float _animSpeed;              ///< Animation speed for knob movement (0.0 to 1.0).

    // --- Optimized Redraw States ---
    int16_t _oldKnobAbsX, _oldKnobAbsY; ///< Previous absolute screen coordinates of the knob's center for `update()`'s partial redraw.
    bool _layoutNeedsRedraw;            ///< Flag to indicate that the static layout (text, track position) needs a full redraw.
    bool _knobAnimationActive;          ///< Flag to indicate that the knob animation is currently running.
    uint8_t _touchPadding;              ///< Padding around the visual content for a larger touch detection area.

    // --- Private Helper Methods ---
    /**
     * @brief Recalculates the internal layout and drawing positions of the switch and its label.
     * This method is called when properties like size, alignment, or title change.
     */
    void _recalculateLayout();

    /**
     * @brief Updates the target X position of the knob based on the current switch state (`_isOn`).
     * This target position is used for animation.
     */
    void _updateTargetPosition();

    /**
     * @brief Draws the static content of the toggle switch (title text and the track background).
     * @note This method does not draw the knob.
     */
    void _drawStaticContent();

    /**
     * @brief Draws the knob at a specific absolute screen position.
     * @param knobAbsX The absolute X screen coordinate for the knob's center.
     * @param knobAbsY The absolute Y screen coordinate for the knob's center.
     */
    void _drawKnob(int32_t knobAbsX, int32_t knobAbsY);


public:
    /**
     * @brief Constructor for the ToggleSwitchUI class.
     * Initializes the toggle switch with its position, size, title, and initial state.
     *
     * @param lcd Pointer to the LGFX display object.
     * @param x The relative X coordinate of the component's top-left corner.
     * @param y The relative Y coordinate of the component's top-left corner.
     * @param w The total width of the component's bounding box.
     * @param h The total height of the component's bounding box.
     * @param title The text label displayed next to the switch.
     * @param initialState The initial ON/OFF state of the switch (false for OFF, true for ON).
     * @param switchTrackWidth The pixel width of the switch track.
     * @param trackHeight The pixel height of the switch track.
     * @param knobRadius The radius of the circular knob.
     * @param titlePaddingY Vertical padding for the title text.
     */
    ToggleSwitchUI(LGFX* lcd, int16_t x, int16_t y, int16_t w, int16_t h,
                   const std::string& title,
                   bool initialState = false,
                   int16_t switchTrackWidth = TOGGLE_SWITCH_DEFAULT_TRACK_WIDTH_PIXELS,
                   int16_t trackHeight = TOGGLE_SWITCH_DEFAULT_TRACK_HEIGHT_PIXELS,
                   int16_t knobRadius = TOGGLE_SWITCH_DEFAULT_KNOB_RADIUS_PIXELS,
                   uint8_t titlePaddingY = TOGGLE_SWITCH_DEFAULT_TITLE_PADDING_Y_PIXELS);

    // --- UIElement Overrides ---
    /**
     * @brief Draws the toggle switch component on the LCD.
     * This method handles the rendering of the static content (label, track) and the knob.
     * It performs a full redraw only when the layout or background needs it.
     */
    void draw() override;

    /**
     * @brief Updates the internal state of the toggle switch, specifically for animation.
     * This method smoothly moves the knob towards its target position by performing partial redraws,
     * carefully clearing only the knob's old position.
     */
    void update() override;

    /**
     * @brief Handles touch events for the toggle switch.
     * Determines if a touch event interacts with the switch and changes its state if applicable.
     * @param x The X coordinate of the touch event relative to the parent layer.
     * @param y The Y coordinate of the touch event relative to the parent layer.
     * @param isPressed True if the touch is currently pressed, false if released.
     * @return True if the touch event was handled by the switch, false otherwise.
     */
    bool handleTouch(int32_t x, int32_t y, bool isPressed) override;

    /**
     * @brief Returns the type of the UI element.
     * @return UIElementType::TOGGLE_BUTTON.
     */
    UIElementType getElementType() const override { return UIElementType::TOGGLE_BUTTON; }

    /**
     * @brief Sets the position of the toggle switch relative to its parent layer.
     * Triggers a full redraw of the component.
     * @param x The new relative X coordinate.
     * @param y The new relative Y coordinate.
     */
    void setPosition(int16_t x, int16_t y) override;

    /**
     * @brief Sets the size of the toggle switch's bounding box.
     * Triggers a full redraw of the component.
     * @param w The new width.
     * @param h The new height.
     */
    void setSize(int16_t w, int16_t h) override;

    // --- Configuration Setters & Getters ---
    /**
     * @brief Sets the alignment of the switch's content within its bounding box.
     * Triggers a full redraw of the component.
     * @param alignment The new text alignment datum (e.g., ML_DATUM for middle-left).
     */
    void setAlignment(textdatum_t alignment);

    /**
     * @brief Sets the state of the toggle switch (ON/OFF).
     * Initiates the knob animation if the state changes.
     * @param isOn The new state (true for ON, false for OFF).
     * @param triggerCallback If true, the `_onStateChangedCallback` is invoked.
     */
    void setState(bool isOn, bool triggerCallback = false);

    /**
     * @brief Gets the current state of the toggle switch.
     * @return True if the switch is ON, false if OFF.
     */
    bool getState() const;

    /**
     * @brief Registers a callback function to be called when the switch's state changes.
     * @param callback The function to be called.
     */
    void setOnStateChangedCallback(StateChangedCallback callback);

    /**
     * @brief Sets the background color of the parent layer.
     * This color is used for clearing areas around the switch during redraws.
     * Triggers a full redraw of the component.
     * @param color The new background color.
     */
    void setLayerBackgroundColor(uint32_t color);

    /**
     * @brief Sets the font for the title text.
     * Triggers a full redraw of the component.
     * @param font Pointer to the LGFX font object.
     */
    void setTitleFont(const lgfx::IFont* font);

    /**
     * @brief Sets the title text displayed next to the switch.
     * Triggers a full redraw of the component.
     * @param newTitle The new title string.
     */
    void setTitle(const std::string& newTitle);

    /**
     * @brief Gets the current title text of the switch.
     * @return A const reference to the title string.
     */
    const std::string& getTitle() const;

    /**
     * @brief Sets the width of the switch track.
     * Triggers a full redraw of the component.
     * @param width The new track width in pixels.
     */
    void setSwitchTrackWidth(int16_t width);

    /**
     * @brief Sets the height of the switch track.
     * Also adjusts the knob radius if it's dependent on the track height.
     * Triggers a full redraw of the component.
     * @param height The new track height in pixels.
     */
    void setTrackHeight(int16_t height);

    /**
     * @brief Sets the radius of the knob.
     * Re-applies safety calculation relative to track height and triggers a full redraw of the component.
     * @param radius The new knob radius in pixels.
     */
    void setKnobRadius(int16_t radius);

    /**
     * @brief Sets the vertical padding for the title text.
     * Triggers a full redraw of the component.
     * @param padding The new vertical padding in pixels.
     */
    void setTitlePaddingY(uint8_t padding);

    /**
     * @brief Clears the entire content area occupied by the switch and its label using the layer background color.
     * This is typically called before a full redraw to remove old content.
     */
    void clearCurrentContentArea();
};

#endif // TOGGLESWITCHUI_H