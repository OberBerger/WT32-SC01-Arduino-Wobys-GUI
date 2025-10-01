/**
 * @file TextUI.h
 * @brief Defines the TextUI class, a versatile UI element for displaying and interacting with text.
 *
 * This class provides a flexible text display component with extensive customization
 * options for font, color, alignment, borders, padding, and word wrapping. It can also
 * function as a basic button by handling touch interactions and executing a callback.
 * It integrates with the UI framework for consistent styling and behavior.
 *
 * @version 1.0.1
 * @date 2025-08-05
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
#ifndef TEXTUI_H
#define TEXTUI_H

#include "UIElement.h"
#include "Config.h"
#include <string>
#include <algorithm>
#include <vector>
#include <functional> // Required for std::function in callbacks

/// @brief Special value for automatic UI element sizing, indicating content-based sizing.
#define TEXTUI_AUTO_SIZE -1
/// @brief Special color value indicating transparency for UI elements.
#define TEXTUI_TRANSPARENT ((uint32_t)0xFF000001)

/**
 * @brief Enumerates the types of borders that can be applied to a TextUI element.
 */
enum class TextUI_BorderType : uint8_t {
  NONE = 0,   ///< No border is drawn.
  SINGLE,     ///< A single-line border is drawn around the element.
  // Add more border types here as needed (e.g., DOUBLE, DOTTED, etc.)
};

/**
 * @brief A versatile UI element for displaying and interacting with text.
 *
 * The TextUI class provides a flexible text display component with extensive
 * customization options. It can display single or multi-line text with word
 * wrapping, custom fonts, colors, and alignments. It also supports optional
 * borders and padding. Furthermore, it can be configured to act as a basic
 * button by responding to touch interactions and executing a user-defined callback.
 */
class TextUI : public UIElement {
protected:
  // --- Text Content & Visual Properties ---
  std::string _text;                      ///< The current text string displayed by the element.
  const lgfx::IFont* _font;               ///< The font used for rendering the text.
  uint32_t _textColor;                    ///< The color of the displayed text.
  uint32_t _backgroundColor;              ///< The background color of the text element.
  textdatum_t _textDatum;                 ///< The alignment datum for the text within its bounding box.
  uint8_t _padding;                       ///< Padding around the text content inside the border.
  bool _wordWrap;                         ///< True if text should word-wrap within its width, false otherwise.

  // --- Layout & Sizing ---
  int16_t _x_rel;                         ///< Relative X position of the element's top-left corner within its parent layer.
  int16_t _y_rel;                         ///< Relative Y position of the element's top-left corner within its parent layer.
  int16_t _width;                         ///< Configured width of the element (can be TEXTUI_AUTO_SIZE).
  int16_t _height;                        ///< Configured height of the element (can be TEXTUI_AUTO_SIZE).
  int16_t _actualWidth;                   ///< The actual calculated width of the element (including content, padding, border).
  int16_t _actualHeight;                  ///< The actual calculated height of the element (including content, padding, border).

  // --- Border Properties ---
  uint32_t _borderColor;                  ///< Color of the element's border.
  uint8_t _borderThickness;               ///< Thickness of the border in pixels.
  TextUI_BorderType _borderType;          ///< Type of border to draw (e.g., single line).

  // --- Interactive State & Callbacks ---
  bool _isPressedState;                   ///< True if the element is currently visually in a pressed state.
  bool _wasEverPressedInThisCycle;        ///< Flag indicating if the element was ever pressed during the current touch cycle (press to release).
  uint32_t _pressedTextColor;             ///< Text color when the element is in a pressed state.
  uint32_t _pressedBackgroundColor;       ///< Background color when the element is in a pressed state.
  std::function<void()> _onReleaseCallback; ///< Callback function executed when the element is released (if interactive).

  // --- Last Drawn State (for optimized redraws) ---
  // These variables store the state of the element as it was last successfully drawn
  // to facilitate partial redraws or background clearing.
  std::string _lastDrawnText;             ///< Text content from the last draw cycle.
  int16_t _lastDrawnActualWidth;          ///< Actual width from the last draw cycle.
  int16_t _lastDrawnActualHeight;         ///< Actual height from the last draw cycle.
  textdatum_t _lastDrawnTextDatum;        ///< Text datum from the last draw cycle.
  const lgfx::IFont* _lastDrawnFont;      ///< Font used in the last draw cycle.
  uint8_t _lastDrawnBorderThickness;      ///< Border thickness from the last draw cycle.
  TextUI_BorderType _lastDrawnBorderType; ///< Border type from the last draw cycle.
  uint32_t _lastDrawnBorderColor;         ///< Border color from the last draw cycle.
  uint8_t _lastDrawnPadding;              ///< Padding from the last draw cycle.
  int16_t _lastDrawnX_rel;                ///< Relative X position from the last draw cycle.
  int16_t _lastDrawnY_rel;                ///< Relative Y position from the last draw cycle.
  uint32_t _lastDrawnBackgroundColor;     ///< Background color from the last draw cycle.
  bool _lastDrawnWordWrap;                ///< Word wrap setting from the last draw cycle.


  // --- Protected Helper Methods ---
  /**
   * @brief Calculates the actual width and height of the element based on its content,
   * configured width/height, padding, and border.
   * This method updates the `_actualWidth` and `_actualHeight` members.
   */
  void calculateActualSize();

  /**
   * @brief Wraps the given text into a vector of lines, respecting word boundaries
   * and an effective maximum width.
   * @param text The input text string to wrap.
   * @param font The font to use for measuring text width.
   * @param effectiveTextWidthForWrap The maximum pixel width available for text content (excluding padding/border).
   * @param wordWrap True to enable word wrapping, false to truncate.
   * @return A `std::vector` of strings, where each string represents a wrapped line.
   */
  std::vector<std::string> getWrappedLines(const std::string& text,
                                           const lgfx::IFont* font,
                                           int16_t effectiveTextWidthForWrap,
                                           bool wordWrap);

  /**
   * @brief Draws the text content (a vector of wrapped lines) within a specified area.
   * Handles line positioning and horizontal alignment based on the text datum.
   * @param linesToDraw A vector of strings, each representing a line of text to draw.
   * @param font The font to use for drawing.
   * @param textColor The color of the text.
   * @param datum The text alignment datum.
   * @param contentAreaX The absolute X coordinate of the content area's top-left corner.
   * @param contentAreaY The absolute Y coordinate of the content area's top-left corner.
   * @param contentAreaW The width of the content area.
   * @param contentAreaH The height of the content area.
   */
  void drawTextContent(const std::vector<std::string>& linesToDraw,
                       const lgfx::IFont* font,
                       uint32_t textColor,
                       textdatum_t datum,
                       int16_t contentAreaX,
                       int16_t contentAreaY,
                       int16_t contentAreaW,
                       int16_t contentAreaH);


public:
  // --- Constructors ---
  /**
   * @brief Constructor for the TextUI class.
   * Initializes the text element with specified properties and default colors.
   * @param lcd Pointer to the LGFX display instance.
   * @param text The initial text string to display.
   * @param x The X coordinate of the text element's top-left corner (relative to the layer).
   * @param y The Y coordinate of the text element's top-left corner (relative to the layer).
   * @param font The font to use for rendering the text (default: profont12).
   * @param textColor The color of the text (default: UI_COLOR_TEXT_DEFAULT).
   * @param datum The text alignment datum (default: TL_DATUM).
   * @param width The width of the text element's bounding box (default: TEXTUI_AUTO_SIZE for automatic sizing).
   * @param height The height of the text element's bounding box (default: TEXTUI_AUTO_SIZE for automatic sizing).
   * @param backgroundColor The background color of the text element (default: TEXTUI_TRANSPARENT).
   * @param padding Padding around the text within the element (default: TEXTUI_DEFAULT_PADDING_PIXELS).
   */
  TextUI(LGFX* lcd,
         const std::string& text,
         int16_t x, int16_t y,
         const lgfx::IFont* font = &profont12,
         uint32_t textColor = UI_COLOR_TEXT_DEFAULT,
         textdatum_t datum = TL_DATUM,
         int16_t width = TEXTUI_AUTO_SIZE,
         int16_t height = TEXTUI_AUTO_SIZE,
         uint32_t backgroundColor = TEXTUI_TRANSPARENT,
         uint8_t padding = TEXTUI_DEFAULT_PADDING_PIXELS);

  // --- Setters for Text Content & Visual Properties ---
  /**
   * @brief Sets the text string displayed by the element.
   * Requests a redraw if the text changes and recalculates actual size.
   * @param newText The new text string.
   */
  void setText(const std::string& newText);

  /**
   * @brief Sets the font used for rendering the text.
   * Requests a redraw if the font changes and recalculates actual size.
   * @param newFont The new font.
   */
  void setFont(const lgfx::IFont* newFont);

  /**
   * @brief Sets the color of the displayed text.
   * Requests a redraw if the color changes.
   * @param newColor The new text color.
   */
  void setTextColor(uint32_t newColor);

  /**
   * @brief Sets the background color of the text element.
   * Requests a redraw if the color changes.
   * @param newBgColor The new background color.
   */
  void setBackgroundColor(uint32_t newBgColor);

  /**
   * @brief Sets the alignment datum for the text within its bounding box.
   * Requests a redraw if the datum changes.
   * @param newDatum The new text alignment datum.
   */
  void setTextDatum(textdatum_t newDatum);

  // --- Setters for Layout & Sizing ---
  /**
   * @brief Sets the relative position of the element's top-left corner within its parent layer.
   * Requests a redraw and clears the last drawn state for a full visual refresh.
   * @param x The new relative X coordinate.
   * @param y The new relative Y coordinate.
   */
  void setPosition(int16_t x, int16_t y) override;

  /**
   * @brief Sets the configured width and height of the element.
   * Requests a redraw, recalculates actual size, and clears the last drawn state.
   * @param w The new configured width.
   * @param h The new configured height.
   */
  void setSize(int16_t w, int16_t h) override;

  /**
   * @brief Sets the border properties for the element.
   * Requests a redraw if any border property changes and recalculates actual size.
   * @param color Color of the border.
   * @param thickness Thickness of the border in pixels.
   * @param type Type of border to draw (default: TextUI_BorderType::SINGLE).
   */
  void setBorder(uint32_t color,
                 uint8_t thickness,
                 TextUI_BorderType type = TextUI_BorderType::SINGLE);

  /**
   * @brief Sets the padding around the text content inside the border.
   * Requests a redraw if the padding changes and recalculates actual size.
   * @param padding The new padding value in pixels.
   */
  void setPadding(uint8_t padding);

  /**
   * @brief Enables or disables word wrapping for the text.
   * Requests a redraw if the setting changes and recalculates actual size.
   * @param wrap True to enable word wrapping, false to disable.
   */
  void setWordWrap(bool wrap);

  // --- Setters for Interactive Properties ---
  /**
   * @brief Sets the text and background colors for when the element is in a pressed state.
   * Requests a redraw to reflect potential visual changes.
   * @param textColor The text color for the pressed state.
   * @param backgroundColor The background color for the pressed state.
   */
  void setPressedColors(uint32_t textColor, uint32_t backgroundColor);

  /**
   * @brief Sets the callback function to be executed when the element is released after a press.
   * This makes the TextUI element interactive, behaving like a button.
   * @param callback The callback function (can be nullptr to remove interactivity).
   */
  void setOnReleaseCallback(std::function<void()> callback) override;

  // --- Getters ---
  /**
   * @brief Retrieves the current text string.
   * @return A const reference to the text string.
   */
  const std::string& getText() const;

  /**
   * @brief Retrieves the current font.
   * @return A pointer to the current font.
   */
  const lgfx::IFont* getFont() const;

  /**
   * @brief Retrieves the current padding.
   * @return The padding value in pixels.
   */
  uint8_t getPadding() const;

  /**
   * @brief Retrieves the actual calculated width of the element.
   * @return The actual width in pixels.
   */
  int16_t getWidth() const override;

  /**
   * @brief Retrieves the actual calculated height of the element.
   * @return The actual height in pixels.
   */
  int16_t getHeight() const override;

  /**
   * @brief Retrieves the relative X position of the element's top-left corner.
   * @return The relative X coordinate.
   */
  int16_t getX() const;

  /**
   * @brief Retrieves the relative Y position of the element's top-left corner.
   * @return The relative Y coordinate.
   */
  int16_t getY() const;


  // --- Core UIElement Overrides ---
  /**
   * @brief Draws the text element onto the display.
   * Renders the text with its current visual state (normal, pressed, disabled)
   * and applies appropriate colors, borders, and wrapping.
   */
  void draw() override;

  /**
   * @brief Updates the internal state of the text element.
   * For TextUI, this method is typically empty, as its state changes are
   * driven by external events or touch input.
   */
  void update() override;

  /**
   * @brief Handles touch input for the text element.
   * Manages visual feedback (pressed state) and triggers the `onReleaseCallback`
   * if configured.
   * @param x The X coordinate of the touch input (relative to the layer).
   * @param y The Y coordinate of the touch input (relative to the layer).
   * @param isPressed True if the screen is currently pressed, false otherwise.
   * @return True if the element handled the touch event, false otherwise.
   */
  bool handleTouch(int32_t x, int32_t y, bool isPressed) override;

  /**
   * @brief Overrides the base class method to provide the specific UI element type for TextUI.
   * @return The UIElementType enum value representing a TEXT element.
   */
  UIElementType getElementType() const override { return UIElementType::TEXT; }

  /**
   * @brief Resets the internal state variables that store the last drawn element's properties.
   * This forces a full redraw in the next cycle, ensuring visual consistency if external
   * factors (like parent layer position or scale) implicitly change the element's rendering.
   */
  void clearLastDrawnState();
};

#endif // TEXTUI_H