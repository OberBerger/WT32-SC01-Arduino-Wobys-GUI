/**
 * @file ClockLabelUI.h
 * @brief Defines the ClockLabelUI class, a specialized TextUI for displaying time.
 *
 * This class extends TextUI to specifically handle the display of time,
 * including a blinking colon. It is typically used for screensavers or large clock displays.
 * It fetches time data from an external source (e.g., TimeManager).
 *
 * @version 1.0.9
 * @date 2025-08-14
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
#ifndef CLOCKLABELUI_H
#define CLOCKLABELUI_H

#include "TextUI.h"  // Ensure this is the correct path.
#include <string>    // For std::string.
// LovyanGFX types like textdatum_t are already available via TextUI.h.

/**
 * @brief A specialized TextUI class designed to display time, including a blinking colon.
 *
 * This class extends the basic TextUI functionality to specifically cater to time display needs.
 * It's capable of updating parts of the display (like the colon) independently for efficiency,
 * and is typically managed by a higher-level component like the ScreenSaverManager.
 */
class ClockLabelUI : public TextUI {
private:
  bool _colonVisible; ///< Internal flag to control the visibility of the colon for blinking effect.

public:
  /**
   * @brief Constructor for the ClockLabelUI class.
   * Initializes the clock label with text, position, font, colors, and dimensions.
   * The `_colonVisible` state is initialized to true.
   * @param lcd Pointer to the LGFX display instance.
   * @param text The initial text string to display (e.g., "00:00").
   * @param x The X coordinate of the text element's origin.
   * @param y The Y coordinate of the text element's origin.
   * @param font The font to use for rendering the time text.
   * @param textColor The color of the time text.
   * @param datum The text alignment datum (default: TL_DATUM).
   * @param w The width of the text element's bounding box (default: TEXTUI_AUTO_SIZE for automatic sizing).
   * @param h The height of the text element's bounding box (default: TEXTUI_AUTO_SIZE for automatic sizing).
   * @param bgColor The background color of the text element (default: TEXTUI_TRANSPARENT).
   * @param padding Padding around the text within the element (default: 0).
   */
  ClockLabelUI(LGFX* lcd, const std::string& text, int16_t x, int16_t y,
               const lgfx::IFont* font, uint32_t textColor,
               textdatum_t datum = TL_DATUM,
               int16_t w = TEXTUI_AUTO_SIZE, int16_t h = TEXTUI_AUTO_SIZE,
               uint32_t bgColor = TEXTUI_TRANSPARENT, uint8_t padding = 0);

  /**
   * @brief Sets the visibility of the colon in the displayed time.
   * This is used to create a blinking effect. Requests a redraw if the visibility state changes.
   * @param visible True to make the colon visible, false to hide it.
   */
  void setColonVisibility(bool visible);

  /**
   * @brief Overrides the base class method to provide specific UI element type for ClockLabelUI.
   * @return The UIElementType enum value representing a TEXT element.
   */
  UIElementType getElementType() const override;

  /**
   * @brief Overrides the base class method to draw the time,
   * handling colon blinking separately from the hour/minute display.
   * Only the colon area is redrawn if only its visibility changes.
   */
  void draw() override;

  /**
   * @brief Overrides the base class method. ClockLabelUI itself does not perform
   * internal update logic; its display is driven by external components (e.g., ScreenSaverManager).
   */
  void update() override;

  /**
   * @brief Overrides the base class method. ClockLabelUI is not an interactive element.
   * @param x X coordinate of the touch.
   * @param y Y coordinate of the touch.
   * @param isPressed True if currently pressed, false otherwise.
   * @return Always returns false, as it does not handle touch events.
   */
  bool handleTouch(int32_t x, int32_t y, bool isPressed) override;
};

#endif // CLOCKLABELUI_H