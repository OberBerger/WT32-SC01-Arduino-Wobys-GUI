/**
 * @file UIElement.cpp
 * @brief Implementation of the UIElement base class.
 *
 * This file provides the default implementations for the common functionalities
 * defined in the UIElement base class, handling core UI logic
 * such as visibility, interactivity, screen offsets, and redraw requests.
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

#include "UIElement.h"
#include "ConfigUIUser.h" // Added for UI_COLOR_*_DISABLED constants

/**
 * @brief Constructs a new UIElement object.
 * @param lcd A pointer to the LGFX display object.
 */
UIElement::UIElement(LGFX* lcd)
  : _lcd(lcd), _elementDebugName("UIElement") {
  DEBUG_INFO_PRINTLN("UIElement constructor called.");
  // Default colors for disabled state initialized from compile-time constants in ConfigUIUser.h
  _disabledTextColor = UI_COLOR_TEXT_DISABLED;
  _disabledBorderColor = UI_COLOR_BORDER_DISABLED;
  _disabledBackgroundColor = UI_COLOR_BACKGROUND_DISABLED;
}

/**
 * @brief Retrieves the type of the UI element.
 * @return The `UIElementType` enum value corresponding to the element's type.
 */
UIElementType UIElement::getElementType() const { return UIElementType::GENERIC; }

/**
 * @brief Sets the visibility state of the UI element.
 * @param visible `true` to make the element visible, `false` to hide it.
 * @param redraw `true` to force a redraw if visibility changes, `false` otherwise. Defaults to `true`.
 */
void UIElement::setVisible(bool visible, bool redraw) {
  bool oldVisibility = _isVisible;
  _isVisible = visible;
  if (redraw && (oldVisibility != _isVisible)) {
    _redrawRequested = true;
    if (_isVisible) {
      // draw() is typically called by the parent UILayer's drawAll(), not directly here.
    }
  }
}

/**
 * @brief Checks if the UI element is currently visible.
 * @return `true` if the element is visible, `false` otherwise.
 */
bool UIElement::isVisible() const {
  return _isVisible;
}

/**
 * @brief Sets the interactive state of the UI element.
 * The method now exclusively sets the `_isInteractive` boolean state.
 * The `requestRedraw()` call has been removed, as a change in interactivity
 * alone does not inherently require a visual refresh. Visual updates
 * related to interactivity are handled by `setVisualState()`.
 * @param interactive `true` to enable interaction, `false` to disable it.
 */
void UIElement::setInteractive(bool interactive) {
  _isInteractive = interactive;
}

/**
 * @brief Checks if the UI element is currently interactive.
 * @return `true` if the element is interactive, `false` otherwise.
 */
bool UIElement::isInteractive() const {
  return _isInteractive;
}

/**
 * @brief Retrieves the debug name of the UI element.
 * @return A string containing the element's debug name.
 */
std::string UIElement::getElementName() const {
  return _elementDebugName;
}

/**
 * @brief Sets the visual state of the UI element.
 * The method is now responsible for requesting a redraw if the visual state changes.
 * This responsibility was moved here from `setInteractive()`.
 * @param state The new `UIVisualState` for the element.
 */
void UIElement::setVisualState(UIVisualState state) {
  if (_visualState != state) {
    _visualState = state;
    requestRedraw();
    DEBUG_INFO_PRINTF("UIElement (%s): Visual state set to %d.\n", getElementName().c_str(), static_cast<int>(_visualState));
  }
}

/**
 * @brief Retrieves the current visual state of the UI element.
 * @return The `UIVisualState` enum value.
 */
UIVisualState UIElement::getVisualState() const {
  return _visualState;
}

/**
 * @brief Assigns a debug-friendly name to the UI element.
 * @param name The string name to set.
 */
void UIElement::setElementName(const std::string& name) {
  _elementDebugName = name;
}

/**
 * @brief Requests that the parent layer performs a full redraw.
 * This is typically used when an element needs the entire background below it
 * to be refreshed (e.g., when it becomes invisible and leaves a "hole").
 * @param request `true` to request a full redraw, `false` to clear the request. Defaults to `true`.
 */
void UIElement::requestParentFullRedraw(bool request) {
  if (_requestsParentFullRedraw != request) {
    _requestsParentFullRedraw = request;
  }
}

/**
 * @brief Checks if the element has requested a full redraw of its parent layer.
 * @return `true` if a full parent redraw is requested, `false` otherwise.
 */
bool UIElement::requestsParentFullRedraw() const {
  return _requestsParentFullRedraw;
}

/**
 * @brief Notifies the element that its parent layer's background was cleared.
 * If cleared, the element requests a redraw to ensure it's drawn over the new background.
 * @param cleared `true` if the background was cleared, `false` otherwise.
 */
void UIElement::setLayerBackgroundCleared(bool cleared) {
  if (_wasLayerBackgroundCleared != cleared) {
    _wasLayerBackgroundCleared = cleared;
    if (cleared) {
      requestRedraw();
    }
  }
}

/**
 * @brief Checks if the parent layer's background was cleared recently.
 * @return `true` if the background was cleared, `false` otherwise.
 */
bool UIElement::getLayerBackgroundCleared() const {
  return _wasLayerBackgroundCleared;
}

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
bool UIElement::handleTouch(int32_t x, int32_t y, bool isPressed) {
  if (!_isVisible || !_isInteractive) {
    return false;
  }
  return false;
}

/**
 * @brief Sets the global screen offset for the element.
 * This is typically provided by the parent `UILayer` to position elements correctly
 * relative to the display's top-left corner. A redraw is requested upon offset change.
 * @param offsetX The X offset from the screen's left edge.
 * @param offsetY The Y offset from the screen's top edge.
 */
void UIElement::setScreenOffset(int32_t offsetX, int32_t offsetY) {
  _screenOffsetX = offsetX;
  _screenOffsetY = offsetY;
  _redrawRequested = true;
}

/**
 * @brief Requests that the element be redrawn in the next `drawAll()` cycle of its parent layer.
 */
void UIElement::requestRedraw() {
  _redrawRequested = true;
}

/**
 * @brief Checks if the element has a pending redraw request.
 * @return `true` if a redraw is needed, `false` otherwise.
 */
bool UIElement::needsRedraw() const {
  return _redrawRequested;
}

/**
 * @brief Clears any pending redraw requests for the element.
 */
void UIElement::clearRedrawRequest() {
  _redrawRequested = false;
}

/**
 * @brief Retrieves the width of the UI element.
 * Derived classes should override this to return their actual width.
 * @return The width of the element in pixels. Defaults to 0 for generic elements.
 */
int16_t UIElement::getWidth() const { return 0; }

/**
 * @brief Retrieves the height of the UI element.
 * Derived classes should override this to return their actual height.
 * @return The height of the element in pixels. Defaults to 0 for generic elements.
 */
int16_t UIElement::getHeight() const { return 0; }

/**
 * @brief Sets the colors to be used when the element is in a disabled visual state.
 * This allows overriding the default disabled colors defined in `ConfigUIUser.h` at runtime.
 * A redraw is requested to apply the changes if the element is currently disabled.
 * @param textColor The text color for the disabled state.
 * @param borderColor The border color for the disabled state.
 * @param bgColor The background color for the disabled state.
 */
void UIElement::setDisabledColors(uint32_t textColor, uint32_t borderColor, uint32_t bgColor) {
    _disabledTextColor = textColor;
    _disabledBorderColor = borderColor;
    _disabledBackgroundColor = bgColor;
    // Request a redraw to apply the new colors, especially if the element is currently disabled.
    requestRedraw();
}