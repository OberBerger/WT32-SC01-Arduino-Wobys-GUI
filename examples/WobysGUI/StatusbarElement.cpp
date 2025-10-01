/**
 * @file StatusbarElement.cpp
 * @brief Implements the base class for all UI elements displayed within the application's status bar.
 *
 * This file provides the implementation details for common functionalities
 * such as constructor initialization, parent linking, redraw requests,
 * and height referencing for status bar elements.
 *
 * @version 1.0.7
 * @date 2025-08-25
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
#include "StatusbarElement.h"
#include "StatusbarUI.h" // Required for StatusbarUI class definition.

/**
 * @brief Constructor for the StatusbarElement.
 * @param lcd Pointer to the LGFX display instance.
 * @param width The fixed width of the element in the status bar.
 * @param elementName Optional debug name for the element.
 */
StatusbarElement::StatusbarElement(LGFX* lcd, int32_t width, const std::string& elementName)
    : _lcd(lcd), _width(width), _elementName(elementName) {
    // Parent pointer is initialized to nullptr by default.
    // _statusBarHeightRef is initialized to 0 by default.
}

/**
 * @brief Sets the parent StatusbarUI for this element.
 * @param parent Pointer to the parent StatusbarUI instance.
 */
void StatusbarElement::setParent(StatusbarUI* parent) {
    _parent = parent;
}

/**
 * @brief Sets a reference to the actual height of the status bar.
 * This is used for vertical positioning and sizing within the status bar.
 * Requests a redraw if the height reference changes.
 * @param height The height of the status bar in pixels.
 */
void StatusbarElement::setStatusBarHeightRef(int32_t height) {
    if (_statusBarHeightRef != height) {
        _statusBarHeightRef = height;
        requestRedraw(); // Request redraw if height reference changes.
    }
}

/**
 * @brief Requests a redraw for this element.
 * Sets the `_redrawNeeded` flag to true.
 */
void StatusbarElement::requestRedraw() {
    _redrawNeeded = true;
}