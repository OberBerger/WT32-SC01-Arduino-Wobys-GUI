#ifndef UIELEMENT_H
#define UIELEMENT_H

/**
 * @file UIElement.h
 * @brief Defines the base class for all User Interface elements.
 *
 * This file provides the fundamental structure and common functionalities
 * for all interactive and visual components within the UI framework.
 * It establishes a contract for drawing, updating, handling touch input,
 * managing visibility, and reporting state changes to parent layers.
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

#include <string>
#include <functional> // Required for std::function callbacks
#include "Config.h"   // Required for LGFX, DEBUG_PRINT macros, and uint32_t
#include "GlobalSystemEvents.h" // To access g_audioManager and g_settingsManager

/**
 * @brief Enum to categorize different types of UI elements.
 * This allows for type-specific handling in generic UI logic.
 */
enum class UIElementType {
    GENERIC,       ///< A generic, undefined UI element.
    BUTTON,        ///< A clickable button element.
    LABEL,         ///< A static text label.
    SEEKBAR,       ///< A slider for value adjustment.
    KEYBOARD,      ///< A virtual on-screen keyboard.
    TEXT,          ///< A multiline text display.
    TOGGLE_BUTTON, ///< A button that toggles between two states.
    CLICKABLE_LIST,///< A scrollable list of clickable items.
    GRID_LAYOUT,   ///< A container that arranges elements in a grid.
    IMAGE          ///< A display element for images.
};

/**
 * @brief Enum to represent the visual state of a UI element.
 * This can influence how an element is rendered.
 */
enum class UIVisualState {
    ACTIVE,        ///< The element is fully active and responsive.
    NON_INTERACTIVE///< The element is not interactive and may be visually grayed out or dimmed.
};

/**
 * @brief Base class for all User Interface elements.
 *
 * This abstract class defines the common interface and basic properties
 * for all UI components. Derived classes must implement `draw()` and `update()`
 * methods to provide their specific rendering and logic. It manages visibility,
 * interactivity, screen offsets, and requests for redraws.
 */
class UIElement {
protected:
  LGFX* _lcd;                                    ///< Pointer to the LGFX display object for drawing.
  int32_t _screenOffsetX = 0;                    ///< X offset of the element relative to the screen.
  int32_t _screenOffsetY = 0;                    ///< Y offset of the element relative to the screen.
  bool _isVisible = false;                       ///< Flag indicating if the element is currently visible.
  bool _redrawRequested = true;                  ///< Flag indicating if the element needs to be redrawn.
  bool _isInteractive = true;                    ///< Flag indicating if the element can receive touch input.
  bool _wasLayerBackgroundCleared = false;       ///< Flag indicating if the parent layer's background was cleared recently.
  bool _requestsParentFullRedraw = false;        ///< Flag indicating if the element requests a full redraw of its parent layer.

  std::string _elementDebugName;                 ///< A debug-friendly name for this UI element instance.
  UIVisualState _visualState = UIVisualState::ACTIVE; ///< The current visual state of the element.

  // New protected member variables:
  // Store colors for the disabled (NON_INTERACTIVE) visual state.
  // Initialized by the constructor from ConfigUIUser.h and can be modified at runtime by setDisabledColors().
  uint32_t _disabledTextColor;      ///< Text color to use when the element is in a disabled visual state.
  uint32_t _disabledBorderColor;    ///< Border color to use when the element is in a disabled visual state.
  uint32_t _disabledBackgroundColor;///< Background color to use when the element is in a disabled visual state.

public:
  /**
   * @brief Constructs a new UIElement object.
   * @param lcd A pointer to the LGFX display object.
   */
  UIElement(LGFX* lcd);

  /**
   * @brief Virtual destructor for UIElement.
   * Ensures proper cleanup of derived class objects when deleted via a base class pointer.
   */
  virtual ~UIElement() = default;

  /**
   * @brief Retrieves the type of the UI element.
   * @return The `UIElementType` enum value corresponding to the element's type.
   */
  virtual UIElementType getElementType() const;

  /**
   * @brief Sets the visibility state of the UI element.
   * @param visible `true` to make the element visible, `false` to hide it.
   * @param redraw `true` to force a redraw if visibility changes, `false` otherwise. Defaults to `true`.
   */
  virtual void setVisible(bool visible, bool redraw = true);

  /**
   * @brief Checks if the UI element is currently visible.
   * @return `true` if the element is visible, `false` otherwise.
   */
  virtual bool isVisible() const;

  /**
   * @brief Sets the interactive state of the UI element.
   * @param interactive `true` to enable interaction, `false` to disable it.
   */
  virtual void setInteractive(bool interactive);

  /**
   * @brief Checks if the UI element is currently interactive.
   * @return `true` if the element is interactive, `false` otherwise.
   */
  virtual bool isInteractive() const;

  /**
   * @brief Retrieves the debug name of the UI element.
   * @return A string containing the element's debug name.
   */
  virtual std::string getElementName() const;

  /**
   * @brief Sets the visual state of the UI element.
   * If the state changes, a redraw is requested.
   * @param state The new `UIVisualState` for the element.
   */
  virtual void setVisualState(UIVisualState state);

  /**
   * @brief Retrieves the current visual state of the UI element.
   * @return The `UIVisualState` enum value.
   */
  virtual UIVisualState getVisualState() const;

  /**
   * @brief Assigns a debug-friendly name to the UI element.
   * @param name The string name to set.
   */
  void setElementName(const std::string& name);

  /**
   * @brief Notifies the element that its parent layer's background was cleared.
   * If cleared, the element requests a redraw to ensure it's drawn over the new background.
   * @param cleared `true` if the background was cleared, `false` otherwise.
   */
  virtual void setLayerBackgroundCleared(bool cleared);

  /**
   * @brief Checks if the parent layer's background was cleared recently.
   * @return `true` if the background was cleared, `false` otherwise.
   */
  bool getLayerBackgroundCleared() const;

  /**
   * @brief Requests that the parent layer performs a full redraw.
   * This is typically used when an element needs the entire background below it
   * to be refreshed (e.g., when it becomes invisible and leaves a "hole").
   * @param request `true` to request a full redraw, `false` to clear the request. Defaults to `true`.
   */
  void requestParentFullRedraw(bool request = true);

  /**
   * @brief Checks if the element has requested a full redraw of its parent layer.
   * @return `true` if a full parent redraw is requested, `false` otherwise.
   */
  bool requestsParentFullRedraw() const;

  /**
   * @brief Retrieves the current Y offset of the element on the screen.
   * @return The Y offset in pixels.
   */
  int32_t getScreenOffsetY() const;

  // New virtual methods for unified API:
  // These methods provide a unified interface for positioning, sizing,
  // and a basic "release" event (if relevant) for all derived elements.
  /**
   * @brief Sets the position of the UI element.
   * Derived classes should implement this to update their internal coordinates.
   * @param x The X coordinate relative to the layer's origin.
   * @param y The Y coordinate relative to the layer's origin.
   */
  virtual void setPosition(int16_t x, int16_t y) {}

  /**
   * @brief Sets the size of the UI element.
   * Derived classes should implement this to update their internal dimensions.
   * @param w The width of the element in pixels.
   * @param h The height of the element in pixels.
   */
  virtual void setSize(int16_t w, int16_t h) {}

  /**
   * @brief Sets a callback function to be invoked when the element is released (touched and then finger lifted).
   * Derived classes that support click-like interactions should implement this.
   * @param callback The `std::function<void()>` to register.
   */
  virtual void setOnReleaseCallback(std::function<void()> callback) {}

  // New virtual method for runtime color configuration:
  /**
   * @brief Sets the colors to be used when the element is in a disabled visual state.
   * This allows overriding the default disabled colors defined in `ConfigUIUser.h` at runtime.
   * A redraw is requested to apply the changes if the element is currently disabled.
   * @param textColor The text color for the disabled state.
   * @param borderColor The border color for the disabled state.
   * @param bgColor The background color for the disabled state.
   */
  virtual void setDisabledColors(uint32_t textColor, uint32_t borderColor, uint32_t bgColor);

  /**
   * @brief Pure virtual method to draw the UI element on the display.
   * Derived classes MUST implement this to define their rendering logic.
   */
  virtual void draw() = 0;

  /**
   * @brief Pure virtual method to update the internal state of the UI element.
   * Derived classes MUST implement this for animation, state checks, etc.
   */
  virtual void update() = 0;

  /**
   * @brief Handles touch input events for the UI element.
   *
   * By default, returns `false` if not visible or not interactive.
   * Derived classes should override this to provide specific touch handling logic.
   *
   * @param x The X coordinate of the touch event relative to the screen.
   * @param y The Y coordinate of the touch event relative to the screen.
   * @param isPressed `true` if the touch event is a press (finger down or dragging), `false` if it's a release (finger up).
   * @return `true` if the touch event was handled by this element, `false` otherwise.
   */
  virtual bool handleTouch(int32_t x, int32_t y, bool isPressed);

  /**
   * @brief Sets the global screen offset for the element.
   * This is typically provided by the parent `UILayer` to position elements correctly
   * relative to the display's top-left corner. A redraw is requested upon offset change.
   * @param offsetX The X offset from the screen's left edge.
   * @param offsetY The Y offset from the screen's top edge.
   */
  virtual void setScreenOffset(int32_t offsetX, int32_t offsetY);

  /**
   * @brief Requests that the element be redrawn in the next `drawAll()` cycle of its parent layer.
   */
  virtual void requestRedraw();

  /**
   * @brief Checks if the element has a pending redraw request.
   * @return `true` if a redraw is needed, `false` otherwise.
   */
  virtual bool needsRedraw() const;

  /**
   * @brief Clears any pending redraw requests for the element.
   */
  virtual void clearRedrawRequest();

  /**
   * @brief Retrieves the width of the UI element.
   * Derived classes should override this to return their actual width.
   * @return The width of the element in pixels. Defaults to 0 for generic elements.
   */
  virtual int16_t getWidth() const;

  /**
   * @brief Retrieves the height of the UI element.
   * Derived classes should override this to return their actual height.
   * @return The height of the element in pixels. Defaults to 0 for generic elements.
   */
  virtual int16_t getHeight() const;
};

#endif // UIELEMENT_H