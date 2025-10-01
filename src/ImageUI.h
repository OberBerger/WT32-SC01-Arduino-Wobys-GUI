/**
 * @file ImageUI.h
 * @brief Defines the ImageUI class, a UI element for displaying images from a filesystem.
 *
 * This class provides a versatile image display component that can load and render
 * JPG images from a specified filesystem (e.g., SD card, LittleFS). It supports
 * customizable positioning and sizing, and can optionally act as a button by
 * responding to touch interactions and executing a callback. Error handling for
 * image loading is included, with a fallback visual indicator.
 *
 * @version 1.0.1
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
#ifndef IMAGE_UI_H
#define IMAGE_UI_H

#include "UIElement.h"
#include <FS.h>           // Required for fs::FS filesystem object
#include <string>         // Required for std::string
#include <functional>     // Required for std::function for callbacks

/**
 * @brief A versatile UI element for displaying images from a filesystem.
 *
 * The ImageUI class allows displaying JPG images loaded from a specified
 * filesystem (e.g., SD card, LittleFS). It offers flexible positioning and
 * sizing, and can be configured to act as an interactive button by responding
 * to touch input and executing a callback function. It includes basic error
 * handling for image loading, providing visual feedback in case of failure.
 */
class ImageUI : public UIElement {
protected:
    // --- Image Data & Source ---
    std::string _filePath;            ///< The path to the image file on the filesystem.
    fs::FS* _fileSystem;              ///< Pointer to the filesystem instance (e.g., &SD, &LittleFS).

    // --- Layout & Sizing ---
    int16_t _x_rel;                   ///< Relative X position of the image's top-left corner within its parent layer.
    int16_t _y_rel;                   ///< Relative Y position of the image's top-left corner within its parent layer.
    int16_t _width;                   ///< Configured width of the image display area.
    int16_t _height;                  ///< Configured height of the image display area.

    // --- Interactive State & Callbacks ---
    bool _isPressedState;             ///< True if the image element is currently visually in a pressed state.
    bool _wasEverPressedInThisCycle;  ///< Flag indicating if the element was ever pressed during the current touch cycle.
    std::function<void()> _onReleaseCallback; ///< Callback function executed when the element is released (if interactive).

    // --- Fallback & Error Display ---
    uint32_t _fallbackBorderColor;    ///< Color for the border drawn when image loading fails.
    int16_t _fallbackTextOffsetPixels;///< Offset for fallback text (e.g., "NO FILE") within the bounding box.

public:
    // --- Constructor ---
    /**
     * @brief Constructor for the ImageUI class.
     * Initializes the image element with its file path, filesystem, position, and size.
     * Sets default fallback colors and internal state.
     * @param lcd Pointer to the LGFX display instance.
     * @param filePath The initial path to the image file.
     * @param fsPtr Pointer to the filesystem instance.
     * @param x The X coordinate of the image element's top-left corner (relative to the layer).
     * @param y The Y coordinate of the image element's top-left corner (relative to the layer).
     * @param width The configured width of the image display area.
     * @param height The configured height of the image display area.
     */
    ImageUI(LGFX* lcd,
            const std::string& filePath,
            fs::FS* fsPtr,
            int16_t x, int16_t y,
            int16_t width, int16_t height);

    // --- Setters for Image Data & Source ---
    /**
     * @brief Sets a new image file path and/or filesystem for the element.
     * Requests a redraw if the image source changes.
     * @param filePath The new path to the image file.
     * @param fsPtr The new pointer to the filesystem instance.
     */
    void setImage(const std::string& filePath, fs::FS* fsPtr);

    // --- Setters for Layout & Sizing (Overrides from UIElement) ---
    /**
     * @brief Sets the relative position of the image element within its parent layer.
     * Requests a redraw if the position changes.
     * @param x The new relative X coordinate.
     * @param y The new relative Y coordinate.
     */
    void setPosition(int16_t x, int16_t y) override;

    /**
     * @brief Sets the configured width and height of the image display area.
     * Requests a redraw if the size changes.
     * @param w The new width.
     * @param h The new height.
     */
    void setSize(int16_t w, int16_t h) override;

    // --- Setters for Callbacks ---
    /**
     * @brief Sets the callback function to be executed when the element is released after a press.
     * This makes the ImageUI element interactive, behaving like a button.
     * @param callback The callback function (can be nullptr to remove interactivity).
     */
    void setOnReleaseCallback(std::function<void()> callback) override;

    // --- Setters for Fallback & Error Display ---
    /**
     * @brief Sets the border color used when the image fails to load.
     * Requests a redraw.
     * @param color The new fallback border color.
     */
    void setFallbackBorderColor(uint32_t color);

    /**
     * @brief Sets the offset for fallback text displayed when an image fails to load.
     * Requests a redraw.
     * @param offsetPixels The new offset in pixels.
     */
    void setFallbackTextOffset(int16_t offsetPixels);

    // --- Getters for Layout & Sizing ---
    /**
     * @brief Retrieves the configured width of the image display area.
     * @return The width in pixels.
     */
    int16_t getWidth() const override { return _width; }

    /**
     * @brief Retrieves the configured height of the image display area.
     * @return The height in pixels.
     */
    int16_t getHeight() const override { return _height; }

    /**
     * @brief Retrieves the relative X position of the image element.
     * @return The relative X coordinate in pixels.
     */
    int16_t getX() const { return _x_rel; }

    /**
     * @brief Retrieves the relative Y position of the image element.
     * @return The relative Y coordinate in pixels.
     */
    int16_t getY() const { return _y_rel; }

    // --- Core UIElement Overrides ---
    /**
     * @brief Draws the image element onto the display.
     * Loads the image from the specified filesystem and renders it. In case of
     * loading errors, it displays a fallback visual indicator.
     */
    void draw() override;

    /**
     * @brief Updates the internal state of the image element.
     * For ImageUI, this method is typically empty, as its state is static once configured,
     * unless dynamic image changes or animations are implemented.
     */
    void update() override;

    /**
     * @brief Handles touch input for the image element.
     * Manages visual feedback (pressed state) and triggers the `onReleaseCallback`
     * if configured.
     * @param x The X coordinate of the touch input (relative to the layer).
     * @param y The Y coordinate of the touch input (relative to the layer).
     * @param isPressed True if the screen is currently pressed, false otherwise.
     * @return True if the element handled the touch event, false otherwise.
     */
    bool handleTouch(int32_t x, int32_t y, bool isPressed) override;

    /**
     * @brief Overrides the base class method to provide the specific UI element type for ImageUI.
     * @return The UIElementType enum value representing an IMAGE element.
     */
    UIElementType getElementType() const override { return UIElementType::IMAGE; }
};

#endif // IMAGE_UI_H