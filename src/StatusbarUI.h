/**
 * @file StatusbarUI.h
 * @brief Manages the application's top status bar, its elements, and a pull-down control panel.
 *
 * This class orchestrates the display, updates, and touch interactions for all elements
 * residing in the fixed top status bar area, and also controls a dynamic,
 * animated pull-down panel containing interactive buttons for quick access to settings.
 * It coordinates touch processing with the ScreenManager to ensure proper UI flow.
 *
 * @version 1.0.3
 * @date 2025-08-09
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
#ifndef STATUSBARUI_H
#define STATUSBARUI_H

#include "Config.h" // For LGFX and general configuration defines.
#include <vector>
#include <functional>
#include <string>
#include <LovyanGFX.hpp> // For LGFX_Sprite

// Forward declarations to avoid circular dependencies.
class ScreenManager;
class StatusbarElement;

/**
 * @brief Manages the application's top status bar, its elements, and a pull-down control panel.
 *
 * This class orchestrates the display, updates, and touch interactions for all elements
 * residing in the fixed top status bar area, and also controls a dynamic,
 * animated pull-down panel containing interactive buttons for quick access to settings.
 * It coordinates touch processing with the ScreenManager to ensure proper UI flow.
 */
class StatusbarUI {
public:
    /**
     * @brief Enum for element alignment within the status bar.
     */
    enum class Alignment { LEFT, RIGHT };
    
    /**
     * @brief Enum representing the current state of the pull-down panel.
     */
    enum PanelState { 
        CLOSED,  ///< The panel is fully closed and out of sight.
        OPENING, ///< The panel is currently animating to the open state.
        OPEN,    ///< The panel is fully open and visible.
        CLOSING  ///< The panel is currently animating to the closed state.
    };

    /**
     * @brief Constructor for the StatusbarUI.
     * @param lcdRef Pointer to the LGFX display instance.
     * @param smPtr Pointer to the ScreenManager instance for coordination.
     */
    StatusbarUI(LGFX* lcdRef, ScreenManager* smPtr);

    /**
     * @brief Initializes the StatusbarUI, including its internal sprite for the panel.
     */
    void init();

    /**
     * @brief Main update loop for the StatusbarUI.
     * Processes touch events, updates internal state, and manages panel animations.
     * @return True if the status bar or its panel handled the touch event, false otherwise.
     */
    bool loop();
    
    /**
     * @brief Draws the status bar and its elements, as well as the pull-down panel.
     */
    void draw();

    /**
     * @brief Adds a StatusbarElement to either the left or right side of the status bar.
     * @param element Pointer to the StatusbarElement to add.
     * @param align The desired alignment (LEFT or RIGHT).
     */
    void addElement(StatusbarElement* element, Alignment align);

    /**
     * @brief Adds a button to the pull-down panel.
     * This method defines a button's properties and registers its click callback.
     * @param id A unique identifier for the button.
     * @param x The X coordinate of the button (relative to the panel sprite, will be recalculated later).
     * @param y The Y coordinate of the button (relative to the panel sprite, will be recalculated later).
     * @param w The width of the button (will be recalculated by `finalizePanelSetup`).
     * @param h The height of the button.
     * @param label The text label displayed on the button.
     * @param font The font used for the button's label.
     * @param onClickCallback The callback function to execute when the button is released.
     * @param cornerRadius The radius for rounded corners of the button (default: 4).
     * @param stickyMargin The margin around the button for "sticky" touch detection (default: -1 for automatic).
     */
    void addButtonToPanel(
        int16_t id, int16_t x, int16_t y, int16_t w, int16_t h,
        const std::string& label, const lgfx::IFont* font,
        std::function<void()> onClickCallback,
        uint8_t cornerRadius = 4, int16_t stickyMargin = -1
    );

    /**
     * @brief Finalizes the panel setup, recalculating button positions and widths.
     * Must be called after all panel buttons have been added.
     */
    void finalizePanelSetup();
    
    /**
     * @brief Initiates the closing animation for the pull-down panel.
     */
    void closePanel();
    
    /**
     * @brief Checks if the pull-down panel is currently open or animating to the open state.
     * @return True if the panel is OPEN or OPENING, false otherwise.
     */
    bool isPanelOpenOrOpening() const;
    
    /**
     * @brief Checks if the pull-down panel is fully closed or if no panel exists.
     * @return True if the panel is CLOSED or if `hasPanel()` is false, false otherwise.
     */
    bool isPanelFullyClosedOrNotPresent() const;
    
    /**
     * @brief Checks if a pull-down panel has been configured for the status bar.
     * @return True if a panel exists (i.e., its height is greater than 0), false otherwise.
     */
    bool hasPanel() const;
    
    /**
     * @brief Sets a callback function to be executed when the pull-down panel is fully closed.
     * @param callback The callback function. It will be set to `nullptr` after execution.
     */
    void setOnPanelFullyClosedCallback(std::function<void()> callback);

    /**
     * @brief Retrieves the fixed height of the status bar area.
     * @return The height of the status bar in pixels.
     */
    int32_t getHeight() const;
    
    /**
     * @brief Handles display orientation changes.
     * Forces redraws for status bar elements and recalculates panel layout.
     */
    void handleOrientationChange();
    
    /**
     * @brief Enables or disables touch processing for the status bar and its panel.
     * When disabled, touch events are ignored.
     * @param enabled True to enable touch processing, false to disable.
     */
    void setTouchProcessingEnabled(bool enabled);
    
    /**
     * @brief Resets the internal touch state variables (e.g., dragging status).
     */
    void resetTouchState();
    
    /**
     * @brief Retrieves the current state of the pull-down panel.
     * @return The current `PanelState` enum value.
     */
    PanelState getPanelState() const { return _panelState; }
    
    /**
     * @brief Retrieves the current absolute Y coordinate of the top edge of the pull-down panel.
     * @return The Y coordinate in pixels.
     */
    float getPanelDrawY() const { return _panelDrawStartY; }
    
    /**
     * @brief Forces a redraw of the pull-down panel in the next drawing cycle.
     */
    void forcePanelRedraw();

    /**
     * @brief Sets the background color of the status bar area.
     * Also triggers a redraw of the background.
     * @param color The new background color.
     */
    void setBackgroundColor(uint32_t color);

private:
    /**
     * @brief Internal structure representing a button within the pull-down panel.
     * Handles its position, size, label, and touch states.
     */
    struct PanelButton {
        int16_t id;                       ///< Unique identifier for the button.
        int16_t x, y, w, h;               ///< Position and dimensions relative to the panel sprite.
        std::string label;                ///< Text label displayed on the button.
        const lgfx::IFont* font;          ///< Font used for the label.
        uint32_t outlineColor;            ///< Color of the button's outline.
        uint32_t fillColorNormal;         ///< Fill color when the button is in normal state.
        uint32_t fillColorPressed;        ///< Fill color when the button is pressed.
        uint32_t textColorNormal;         ///< Text color when the button is in normal state.
        uint32_t textColorPressed;        ///< Text color when the button is pressed.
        bool isCurrentlyPressedState;     ///< True if the button is currently visually pressed.
        bool wasEverPressedInThisCycle;   ///< Flag for "sticky" touch logic.
        uint8_t cornerRadius;             ///< Radius for rounded corners.
        int16_t stickyMargin;             ///< Margin for sticky touch detection.
        std::function<void()> onClickCallback; ///< Callback function on button release.

        /**
         * @brief Constructor for PanelButton.
         * @param _id Unique identifier.
         * @param _x X position.
         * @param _y Y position.
         * @param _w Width.
         * @param _h Height.
         * @param _label Text label.
         * @param _font Font.
         * @param _callback Click callback.
         * @param _cornerRadius Corner radius.
         * @param _stickyMargin Sticky touch margin.
         */
        PanelButton(int16_t _id, int16_t _x, int16_t _y, int16_t _w, int16_t _h,
                    const std::string& _label, const lgfx::IFont* _font,
                    std::function<void()> _callback, uint8_t _cornerRadius, int16_t _stickyMargin);
        
        /**
         * @brief Checks if given coordinates are within the button's original rectangle.
         * @param tx Touch X coordinate (relative to panel sprite).
         * @param ty Touch Y coordinate (relative to panel sprite).
         * @return True if coordinates are inside, false otherwise.
         */
        bool contains(int16_t tx, int16_t ty) const;
        
        /**
         * @brief Checks if given coordinates are within the button's "sticky" touch zone.
         * @param tx Touch X coordinate (relative to panel sprite).
         * @param ty Touch Y coordinate (relative to panel sprite).
         * @return True if coordinates are inside the sticky zone, false otherwise.
         */
        bool containsSticky(int16_t tx, int16_t ty) const;
        
        /**
         * @brief Draws the button onto the provided LGFX_Sprite.
         * @param sprite Pointer to the sprite where the button should be drawn.
         */
        void draw(LGFX_Sprite* sprite) const;
    };

    LGFX* _lcd;                            ///< Pointer to the LGFX display instance.
    ScreenManager* _screenManagerPtr;      ///< Pointer to the ScreenManager for coordination.
    int32_t _height;                       ///< Fixed height of the status bar area.
    uint32_t _backgroundColor;             ///< Background color of the status bar.

    std::vector<StatusbarElement*> _leftElements;  ///< Elements aligned to the left of the status bar.
    std::vector<StatusbarElement*> _rightElements; ///< Elements aligned to the right of the status bar.

    // Pull-down panel related members.
    PanelState _panelState;                ///< Current animation state of the panel.
    PanelState _prevPanelStateForDraw;     ///< Previous panel state, used for redraw logic.
    float _panelDrawStartY;                ///< Current Y position of the panel's top edge during animation.
    float _panelTargetY;                   ///< Target Y position for panel animation.
    float _panelAnimSpeed;                 ///< Animation speed factor for panel movement.
    int32_t _panelWidth;                   ///< Width of the panel.
    int32_t _panelHeight;                  ///< Height of the panel (fixed).
    bool _isPanelSpriteValid;              ///< True if the internal panel sprite was successfully created.
    LGFX_Sprite _panelSprite;              ///< Internal sprite for rendering the panel content.
    std::vector<PanelButton> _panelButtons;///< Buttons within the pull-down panel.
    bool _panelSpriteRedrawNeeded;         ///< Flag to indicate if the panel sprite needs a full redraw.
    bool _forcePanelRedraw;                ///< Flag to force a panel redraw on the main screen.

    // Touch handling for the panel.
    bool _touchProcessingEnabled;          ///< True if status bar/panel can process touch events.
    bool _isDraggingPanel;                 ///< True if the user is currently dragging the panel.
    int32_t _touchStartX_raw;              ///< Raw X coordinate of touch start.
    int32_t _touchStartY_raw;              ///< Raw Y coordinate of touch start.
    unsigned long _lastPanelTouchTime;     ///< Timestamp of the last touch event on the panel.
    float _panelDrawYAtDragStart;          ///< Panel's Y position at the start of a drag.
    int _activeButtonId;                   ///< ID of the button currently being pressed/interacted with.
    bool _buttonCallbackJustExecuted;      ///< Flag to indicate if a button's callback was just executed.
    std::function<void()> _onPanelFullyClosedCallback; ///< Callback to run once panel animation completes.

    /**
     * @brief Draws the fixed elements (icons, time, message board) in the status bar area.
     */
    void drawFixedElements();
    
    /**
     * @brief Draws the pull-down panel.
     * This involves rendering the internal sprite to the main display.
     */
    void drawPanel();
    
    /**
     * @brief Initializes the content of the internal panel sprite.
     * Renders all panel buttons onto the sprite.
     */
    void initPanelSpriteContent();
    
    /**
     * @brief Draws the background of the status bar area.
     */
    void drawBackground();

    /**
     * @brief Handles the initial touch press to determine if it's a button press or drag.
     * @param isPressed Current touch state (true if pressed).
     * @param tx_raw Raw X coordinate of touch.
     * @param ty_raw Raw Y coordinate of touch.
     */
    void handleInitialTouch(bool isPressed, int32_t tx_raw, int32_t ty_raw);
    
    /**
     * @brief Processes touch logic specifically for panel buttons.
     * @param isPressed Current touch state.
     * @param tx_raw Raw X coordinate.
     * @param ty_raw Raw Y coordinate.
     * @param outNeedsSpriteRedraw Output flag, true if sprite needs redraw.
     * @return True if a button handled the touch, false otherwise.
     */
    bool processPanelButtonLogic(bool isPressed, int32_t tx_raw, int32_t ty_raw, bool& outNeedsSpriteRedraw);
    
    /**
     * @brief Processes touch logic specifically for dragging the panel.
     * @param isPressed Current touch state.
     * @param tx_raw Raw X coordinate.
     * @param ty_raw Raw Y coordinate.
     * @return True if the panel drag handled the touch, false otherwise.
     */
    bool processPanelDraggingLogic(bool isPressed, int32_t tx_raw, int32_t ty_raw);
    
    /**
     * @brief Determines the panel's target state (open/closed) when touch is released.
     */
    void determinePanelStateOnRelease();
    
    /**
     * @brief Animates the pull-down panel's movement towards its target Y position.
     */
    void animatePanelMovement();
    
    /**
     * @brief Recalculates the layout and positions of buttons within the panel.
     */
    void _recalculatePanelButtonLayout();
};

#endif // STATUSBARUI_H