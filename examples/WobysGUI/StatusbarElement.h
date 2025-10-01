/**
 * @file StatusbarElement.h
 * @brief Defines the base class for all UI elements displayed within the application's status bar.
 *
 * This abstract class provides the common interface and basic functionalities
 * required by any element to be managed and rendered by the StatusbarUI.
 * It handles properties like width, redraw requests, and provides a reference
 * to its parent StatusbarUI.
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
#pragma once
#ifndef STATUSBARELEMENT_H
#define STATUSBARELEMENT_H

#include "Config.h" // For LGFX and general configuration defines.
#include <string>

// Forward declaration to avoid circular dependency.
class StatusbarUI;

/**
 * @brief Base abstract class for all UI elements displayed within the application's status bar.
 *
 * This class provides the common interface and basic functionalities like
 * redraw management and width for elements intended to be managed and
 * rendered by the StatusbarUI. Derived classes must implement the `draw` method.
 */
class StatusbarElement {
protected:
    LGFX* _lcd;                      ///< Pointer to the LGFX display instance.
    StatusbarUI* _parent;            ///< Pointer to the parent StatusbarUI, for requesting redraws.
    int32_t _width;                  ///< The fixed width of the element in pixels.
    bool _redrawNeeded = true;       ///< Flag indicating if the element needs to be redrawn.
    std::string _elementName;        ///< A debug name for the element, useful for logging.
    int32_t _statusBarHeightRef;     ///< Reference to the actual height of the status bar.

public:
    /**
     * @brief Constructor for the StatusbarElement.
     * @param lcd Pointer to the LGFX display instance.
     * @param width The fixed width of the element in the status bar.
     * @param elementName Optional debug name for the element.
     */
    StatusbarElement(LGFX* lcd, int32_t width, const std::string& elementName = "");

    /**
     * @brief Virtual destructor for StatusbarElement.
     * Ensures proper cleanup for derived classes.
     */
    virtual ~StatusbarElement() {}

    /**
     * @brief Pure virtual method to draw the element onto the display.
     * Derived classes must implement this method to render their specific content.
     * @param x The absolute X coordinate on the screen where the element should be drawn.
     * @param y The absolute Y coordinate on the screen where the element should be drawn.
     * @param actualWidth The actual width allocated for the element in the status bar.
     */
    virtual void draw(int32_t x, int32_t y, int32_t actualWidth) = 0;
    
    /**
     * @brief Updates the state of the element.
     * This method is called repeatedly to allow elements to update their internal state
     * (e.g., animations, data fetching). Default implementation does nothing.
     */
    virtual void update() {}

    /**
     * @brief Retrieves the fixed width of the element.
     * @return The width of the element in pixels.
     */
    virtual int32_t getWidth() const { return _width; }
    
    /**
     * @brief Checks if the element needs to be redrawn.
     * @return True if a redraw is requested, false otherwise.
     */
    bool needsRedraw() const { return _redrawNeeded; }
    
    /**
     * @brief Clears the redraw request flag.
     * This should be called by the `draw` method after rendering is complete.
     */
    void clearRedrawRequest() { _redrawNeeded = false; }
    
    /**
     * @brief Forces a complete redraw of the element in the next drawing cycle.
     * Sets the `_redrawNeeded` flag to true.
     */
    virtual void forceFullRedraw() { requestRedraw(); }

    /**
     * @brief Sets the parent StatusbarUI for this element.
     * @param parent Pointer to the parent StatusbarUI instance.
     */
    void setParent(StatusbarUI* parent);
    
    /**
     * @brief Requests a redraw for this element.
     * Sets the `_redrawNeeded` flag to true.
     */
    void requestRedraw();
    
    /**
     * @brief Retrieves the debug name of the element.
     * @return The debug name as a string.
     */
    const std::string& getElementName() const { return _elementName; }
    
    /**
     * @brief Sets a reference to the actual height of the status bar.
     * This is used for vertical positioning and sizing within the status bar.
     * @param height The height of the status bar in pixels.
     */
    void setStatusBarHeightRef(int32_t height);
};

#endif // STATUSBARELEMENT_H