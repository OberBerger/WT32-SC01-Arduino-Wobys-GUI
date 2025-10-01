/**
 * @file UILayer.h
 * @brief Defines the UILayer class, a container for UI elements.
 *
 * This class serves as a logical grouping for UI elements, managing their visibility,
 * drawing, updating, and touch interaction within a specific region of the screen.
 * It provides mechanisms for screen clearing, overlay behavior, and touch event propagation.
 *
 * @version 1.0.7
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
 * This software utilizes various third-party open-source components.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations
 * (e.g., source code availability for LGPL components) required for compliance.
 */

#ifndef UILAYER_H
#define UILAYER_H

#include <vector>
#include <map>
#include <string>
#include <functional>
#include "UIElement.h" // Essential for UIElement* pointers
#include "Config.h"    // For LGFX, OrientationPreference, and TFT_BLACK

/**
 * @brief Represents a user interface layer, acting as a container for UI elements.
 *
 * A UILayer manages a collection of UIElement objects, handling their rendering,
 * updates, and touch input. It can be configured as an overlay or a full-screen
 * layer, with options for background clearing and preferred screen orientation.
 * UILayers are typically managed by a ScreenManager.
 */
class UILayer {
public:
    // Member Variables
    std::vector<UIElement*> _elements;             ///< Collection of UI elements managed by this layer.
    bool _isOverlay;                               ///< Flag indicating if this layer is an overlay (doesn't clear screen automatically).
    bool _clearScreenOnShow;                       ///< Flag indicating if the layer should clear its background when shown.
    uint32_t _backgroundColor;                     ///< The background color of the layer if `_clearScreenOnShow` is true.
    LGFX* _lcd;                                    ///< Pointer to the LGFX display object used for drawing.
    int32_t _screenOffsetX = 0;                    ///< X offset of the layer's content relative to the screen.
    int32_t _screenOffsetY = 0;                    ///< Y offset of the layer's content relative to the screen (e.g., below a status bar).
    bool _isActive = false;                        ///< Flag indicating if the layer is currently active and visible.
    bool _hadActiveTouchOnLayer = false;           ///< Flag indicating if there's currently an active touch press on this layer.
    UIElement* _activeTouchElement = nullptr;      ///< Pointer to the UI element that currently "owns" the touch input.
    OrientationPreference _preferredOrientation = OrientationPreference::ADAPTIVE; ///< The preferred screen orientation for this layer.
    std::map<UIElement*, bool> _savedInteractiveStates; ///< Stores the interactive states of elements when the layer's interactivity is toggled.
    std::string _layerDebugName;                   ///< A debug-friendly name for this UILayer instance.
    bool _needsFullLayerRedraw = false;            ///< Flag indicating if the entire layer (background and all elements) needs to be redrawn.
    bool _redrawRequested = true;                  ///< Flag indicating if the layer itself (e.g., its background) needs a partial redraw.
    std::function<void()> _onLoopCallback;         ///< Optional callback function to be executed during the layer's `updateAll()` method.

    // Constructors and Destructors
    /**
     * @brief Constructs a new UILayer object.
     *
     * Initializes the layer with display reference, overlay status, background clearing preference,
     * background color, and preferred screen orientation.
     *
     * @param lcdRef A pointer to the LGFX display object.
     * @param isOverlay `true` if the layer is an overlay (doesn't clear underlying content), `false` otherwise. Defaults to `false`.
     * @param clearScreen `true` if the layer should clear its background when shown, `false` otherwise. Defaults to `true`.
     * @param bgColor The background color of the layer. Defaults to `TFT_BLACK`.
     * @param orientation The preferred screen orientation for this layer. Defaults to `OrientationPreference::ADAPTIVE`.
     */
    UILayer(LGFX* lcdRef,
            bool isOverlay = false,
            bool clearScreen = true,
            uint32_t bgColor = TFT_BLACK,
            OrientationPreference orientation = OrientationPreference::ADAPTIVE);

    /**
     * @brief Destructor for the UILayer object.
     *
     * Clears the internal vector of UI elements. Note that the UILayer does not
     * own the UIElement objects themselves; it only manages their pointers.
     * The elements must be deleted elsewhere to avoid memory leaks.
     */
    ~UILayer();

    // Element Management
    /**
     * @brief Adds a UI element to this layer.
     *
     * @param el A pointer to the UIElement to add. If `nullptr`, no action is taken.
     */
    void addElement(UIElement* el);

    /**
     * @brief Retrieves a constant reference to the vector of UI elements on this layer.
     * @return A constant reference to the `std::vector<UIElement*>`.
     */
    const std::vector<UIElement*>& getElements() const;

    // Layer State and Visibility
    /**
     * @brief Activates and displays the layer.
     *
     * Sets the layer as active, applies screen offsets, and clears the background
     * if configured to do so. It also informs its elements about the new screen offset
     * and requests their redraw.
     *
     * @param globalOffsetY The global Y offset on the screen for this layer's content (e.g., status bar height).
     */
    void show(int32_t globalOffsetY);

    /**
     * @brief Deactivates and hides the layer.
     *
     * Resets internal touch state and clears redraw requests for all elements.
     */
    void hide();

    /**
     * @brief Checks if the layer is currently active.
     * @return `true` if the layer is active, `false` otherwise.
     */
    bool isActive() const;

    /**
     * @brief Checks if there's currently an active touch (finger down) on this layer.
     * @return `true` if an active touch has been detected on this layer, `false` otherwise.
     */
    bool hadActiveTouch() const;

    // Drawing and Updating
    /**
     * @brief Draws all visible UI elements on the layer.
     *
     * If a full layer redraw is requested, it clears the entire layer background
     * and forces all visible elements to redraw. Otherwise, it only redraws elements
     * that have specifically requested an update.
     */
    void drawAll();

    /**
     * @brief Updates the internal state of all visible UI elements on the layer.
     *
     * This method also checks if any element requests a full parent layer redraw
     * and triggers it accordingly.
     */
    void updateAll();

    /**
     * @brief Requests a redraw of the layer's own background.
     *
     * This sets the `_redrawRequested` flag to `true`, indicating the layer itself
     * needs to be redrawn (e.g., its background or specific layer-level visuals).
     */
    void requestRedraw();

    /**
     * @brief Checks if the layer has a pending redraw request.
     * @return `true` if a redraw is needed, `false` otherwise.
     */
    bool needsRedraw() const;

    /**
     * @brief Clears any pending redraw requests for the layer.
     */
    void clearRedrawRequest();

    /**
     * @brief Requests a full redraw of the entire layer, including its background and all elements.
     *
     * This is typically used when the layer's properties (like orientation) change
     * or when an element requires a complete refresh of the layer below it.
     */
    void requestFullLayerRedraw();

    /**
     * @brief Clears the full layer redraw request flag.
     */
    void clearFullLayerRedrawRequest();

    /**
     * @brief Checks if a full layer redraw has been requested.
     * @return `true` if a full layer redraw is needed, `false` otherwise.
     */
    bool needsFullLayerRedraw() const;

    // Touch Input Handling
    /**
     * @brief Processes touch input events for the layer and its elements.
     *
     * This method determines which UI element, if any, handles the touch event.
     * It prioritizes elements that have "captured" the touch (e.g., on a long press).
     *
     * @param x The X coordinate of the touch event relative to the screen.
     * @param y The Y coordinate of the touch event relative to the screen.
     * @param isPressed `true` if the touch event is a press (finger down or dragging), `false` if it's a release (finger up).
     */
    void processTouch(int32_t x, int32_t y, bool isPressed);

    // Interactivity Management
    /**
     * @brief Toggles the interactive state of all UI elements on this layer.
     *
     * When `interactive` is `false`, it saves the current interactive state of each element
     * and then sets all elements to non-interactive. When `interactive` is `true`,
     * it restores the previously saved interactive states.
     *
     * @param interactive `true` to enable interactivity (or restore previous states), `false` to disable.
     */
    void setInteractive(bool interactive);

    /**
     * @brief Explicitly sets the interactive state for all UI elements on this layer.
     *
     * Unlike `setInteractive`, this method does not save or restore previous states;
     * it directly applies the given `interactive` value to all elements.
     *
     * @param interactive `true` to make all elements interactive, `false` to make them non-interactive.
     */
    void setAllInteractive(bool interactive);

    // Getters and Setters
    /**
     * @brief Retrieves the debug name of this layer.
     * @return A constant reference to the layer's debug name string.
     */
    const std::string& getElementName() const;

    /**
     * @brief Sets a debug name for this layer.
     * @param name The new debug name for the layer.
     */
    void setElementName(const std::string& name);

    /**
     * @brief Retrieves the current Y offset of the layer's content on the screen.
     * @return The Y offset in pixels.
     */
    int32_t getScreenOffsetY() const;

    /**
     * @brief Retrieves the preferred orientation of this layer.
     * @return The `OrientationPreference` enum value.
     */
    OrientationPreference getPreferredOrientation() const { return _preferredOrientation; }

    /**
     * @brief Sets the preferred screen orientation for this layer.
     *
     * If the new preference differs from the current one, it requests a redraw of the layer.
     *
     * @param newPreference The new preferred `OrientationPreference`.
     */
    void setPreferredOrientation(OrientationPreference newPreference);

    /**
     * @brief Sets an optional callback function to be executed during the layer's `updateAll()` method.
     *
     * This callback can be used for layer-specific logic that needs to run
     * continuously during the layer's active state.
     *
     * @param callback The function to set as the on-loop callback.
     */
    void setOnLoopCallback(std::function<void()> callback);
};

#endif // UILAYER_H