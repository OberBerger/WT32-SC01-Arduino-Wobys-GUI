/**
 * @file SeekbarUI.h
 * @brief Defines the SeekbarUI class, a slider UI element for value selection.
 *
 * This file contains the declaration of the SeekbarUI class, which provides an interactive
 * slider component for selecting values within a defined range. It supports both vertical
 * and horizontal orientations, customizable appearance, and value change callbacks.
 * This software is proprietary and confidential. Unauthorized copying or distribution is prohibited.
 * Refer to LICENSES.txt for details on third-party components.
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
 * This product incorporates software components licensed under various open-source licenses.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#ifndef SEEKBAR_UI_H
#define SEEKBAR_UI_H

// Base UI Element definition
#include "UIElement.h"

// Standard library includes required for the header
#include <functional> // For std::function
#include <string>     // For std::string

// Include UIConfigUser.h for UI_COLOR_XXX defines and constants.
#include "ConfigUIUser.h"

/**
 * @brief Slider UI element for value selection with vertical or horizontal orientation.
 *
 * The SeekbarUI class provides a customizable slider control that allows users to select
 * a floating-point value within a specified range. It supports different orientations,
 * visual feedback for values outside defined limits, and includes optional title and
 * current value text displays. It handles touch interactions for dragging the slider knob
 * and triggers callbacks upon value changes.
 */
class SeekbarUI : public UIElement {
public:
  /**
   * @brief Callback type for value changes.
   * @param value The new current value of the seekbar.
   * @param isFinalChange True if this is the final change (e.g., finger lifted), false for continuous changes during dragging.
   */
  using ValueChangedCallback = std::function<void(float value, bool isFinalChange)>;

  /**
   * @brief Defines the orientation of the seekbar.
   */
  enum class Orientation {
    Vertical,   ///< Vertical orientation, knob moves up/down.
    Horizontal  ///< Horizontal orientation, knob moves left/right.
  };

  /**
   * @brief Constructor for the SeekbarUI class.
   * Initializes the basic properties, color constants, and sets up the seekbar's initial state.
   *
   * @param lcd Pointer to the LGFX display object.
   * @param x The relative X coordinate of the seekbar's top-left corner within its parent layer.
   * @param y The relative Y coordinate of the seekbar's top-left corner within its parent layer.
   * @param overallWidth The total width allocated for the seekbar component (including text).
   * @param overallHeight The total height allocated for the seekbar component (including text).
   * @param initialValue The initial value of the seekbar.
   * @param minValue The minimum selectable value for the seekbar.
   * @param maxValue The maximum selectable value for the seekbar.
   * @param layerBgColor The background color of the parent layer, used for clearing areas during redraws.
   */
  SeekbarUI(LGFX* lcd,
            int16_t x, int16_t y, int16_t overallWidth, int16_t overallHeight,
            float initialValue = 50.0f,
            float minValue = 0.0f, float maxValue = 100.0f,
            uint32_t layerBgColor = 0x000000U);

  // --- UIElement Overrides ---
  /**
   * @brief Sets the total size of the seekbar component.
   * This method recalculates the internal layout based on the new dimensions.
   * @param width The new total width of the seekbar component.
   * @param height The new total height of the seekbar component.
   */
  void setSize(int16_t width, int16_t height) override;

  /**
   * @brief Returns the total width of the seekbar component.
   * @return The total width.
   */
  int16_t getWidth() const override { return _totalWidth; }

  /**
   * @brief Returns the total height of the seekbar component.
   * @return The total height.
   */
  int16_t getHeight() const override { return _totalHeight; }

  /**
   * @brief Returns the type of the UI element.
   * @return UIElementType::SEEKBAR.
   */
  UIElementType getElementType() const override { return UIElementType::SEEKBAR; }

  /**
   * @brief Draws the seekbar component on the LCD.
   * This method handles full and granular redraws based on internal flags.
   */
  void draw() override;

  /**
   * @brief Updates the internal state of the seekbar.
   * Currently, this method does not perform any significant updates but is part of the UIElement interface.
   */
  void update() override;

  /**
   * @brief Handles touch events for the seekbar.
   * Determines if a touch event interacts with the seekbar and updates its value accordingly.
   * @param tx_layer The X coordinate of the touch event relative to the parent layer.
   * @param ty_layer The Y coordinate of the touch event relative to the parent layer.
   * @param isCurrentlyPressed True if the touch is currently pressed, false if released.
   * @return True if the touch event was handled by the seekbar, false otherwise.
   */
  bool handleTouch(int32_t tx_layer, int32_t ty_layer, bool isPressed) override;

  /**
   * @brief Checks if the seekbar needs to be redrawn.
   * This method checks both the base UIElement redraw flag and internal granular redraw flags.
   * @return True if any part of the seekbar needs redrawing, false otherwise.
   */
  bool needsRedraw() const override;

  // --- Position & Dimension Getters ---
  /**
   * @brief Gets the relative X position of the seekbar within its parent layer.
   * @return The relative X coordinate.
   */
  int16_t getX() const { return _x_rel; }

  /**
   * @brief Gets the relative Y position of the seekbar within its parent layer.
   * @return The relative Y coordinate.
   */
  int16_t getY() const { return _y_rel; }

  /**
   * @brief Gets the calculated main length of the track (the part where the knob moves).
   * @return The main length of the track in pixels.
   */
  int16_t getTrackMainLength() const { return _trackMainLength; }

  /**
   * @brief Gets the radius of the slider knob.
   * @return The knob radius in pixels.
   */
  uint8_t getKnobRadius() const { return _knobRadius; }

  /**
   * @brief Gets the padding value used between elements.
   * @return The padding in pixels.
   */
  uint8_t getPadding() const { return _padding; }

  // --- Configuration Setters & Getters ---
  /**
   * @brief Sets the orientation of the seekbar (Vertical or Horizontal).
   * Changing orientation triggers a layout recalculation.
   * @param newOrientation The new orientation for the seekbar.
   */
  void setOrientation(Orientation newOrientation);

  /**
   * @brief Sets the main length of the track.
   * This method should ideally not be called directly, as it's calculated internally by setSize().
   * If called, it forces a recalculation of the layout.
   * @param mainLength The new main length of the track.
   */
  void setTrackMainLength(int16_t mainLength);

  /**
   * @brief Sets the title text displayed for the seekbar.
   * @param title The new title string.
   * @param font An optional font to use for the title. If nullptr, the current font is retained.
   */
  void setTitle(const std::string& title, const lgfx::IFont* font = nullptr);

  /**
   * @brief Sets the format for displaying the current value.
   * @param digits The number of digits before the decimal point.
   * @param decimals The number of decimal places to display.
   * @param font An optional font to use for the value text. If nullptr, the current font is retained.
   */
  void setValueFormat(uint8_t digits, uint8_t decimals, const lgfx::IFont* font = nullptr);

  /**
   * @brief Sets the position of the seekbar relative to its parent layer.
   * @param x The new relative X coordinate.
   * @param y The new relative Y coordinate.
   */
  void setPosition(int16_t x, int16_t y);

  /**
   * @brief Sets the visual state of the seekbar (e.g., ACTIVE, NON_INTERACTIVE).
   * Overrides the base UIElement method and triggers a redraw.
   * @param state The new visual state.
   */
  void setVisualState(UIVisualState state) override;

  /**
   * @brief Sets the background color of the parent layer.
   * This color is used for clearing areas around the seekbar during redraws.
   * @param bgColor The new background color.
   */
  void setLayerBackgroundColor(uint32_t bgColor);

  /**
   * @brief Sets the lower and upper warning limits for the seekbar's value.
   * If the current value falls outside these limits, the knob and track may change color.
   * @param lower The new lower limit.
   * @param upper The new upper limit.
   */
  void setLimits(float lower, float upper);

  /**
   * @brief Sets the minimum and maximum selectable value range for the seekbar.
   * This also constrains the current value to fit within the new range.
   * @param minValue The new minimum value.
   * @param maxValue The new maximum value.
   */
  void setRange(float minValue, float maxValue);

  /**
   * @brief Sets the current value of the seekbar.
   * The value is constrained to the defined min/max range.
   * @param value The new current value.
   * @param triggerCallback If true, the `_onValueChangedCallback` is invoked.
   */
  void setCurrentValue(float value, bool triggerCallback = true);

  /**
   * @brief Gets the current value of the seekbar.
   * @return The current float value.
   */
  float getCurrentValue() const;

  /**
   * @brief Gets the font used for displaying the current value.
   * @return Pointer to the LGFX font object.
   */
  const lgfx::IFont* getValueFont() const { return _valueFont; }

  /**
   * @brief Gets the font used for displaying the title.
   * @return Pointer to the LGFX font object.
   */
  const lgfx::IFont* getTitleFont() const { return _titleFont; }

  /**
   * @brief Sets the colors for the knob and track in normal and limit states, and the track background.
   * @param normalKnob Color of the knob when within limits.
   * @param normalTrack Color of the active track portion when within limits.
   * @param limitKnob Color of the knob when outside limits.
   * @param limitTrack Color of the active track portion when outside limits.
   * @param trackBg Color of the inactive track background.
   */
  void setColors(uint32_t normalKnob, uint32_t normalTrack, uint32_t limitKnob, uint32_t limitTrack, uint32_t trackBg);

  /**
   * @brief Registers a callback function to be called when the seekbar's value changes.
   * @param callback The function to be called.
   */
  void setOnValueChangedCallback(ValueChangedCallback callback);

  /**
   * @brief Sets the visibility of the current value text.
   * @param show True to show the value text, false to hide it.
   */
  void setShowValueText(bool show);

  /**
   * @brief Checks if the current value text is visible.
   * @return True if value text is shown, false otherwise.
   */
  bool getValueTextVisible() const { return _showValueText; }

  /**
   * @brief Sets the visibility of the title text.
   * @param show True to show the title text, false to hide it.
   */
  void setShowTitleText(bool show);

  /**
   * @brief Checks if the title text is visible.
   * @return True if title text is shown, false otherwise.
   */
  bool getTitleTextVisible() const { return _showTitleText; }


private:
  // --- Position and Dimensions ---
  int16_t _x_rel;           ///< Relative X position of the component's top-left corner within its parent.
  int16_t _y_rel;           ///< Relative Y position of the component's top-left corner within its parent.
  int16_t _totalWidth;      ///< Total width of the component, including title/value text.
  int16_t _totalHeight;     ///< Total height of the component, including title/value text.
  Orientation _orientation = Orientation::Vertical; ///< Current orientation of the seekbar (Vertical/Horizontal).

  // --- Track Properties ---
  int16_t _trackMainLength;  ///< Length of the main track (along which the knob slides).
  int16_t _trackCrossLength; ///< Perpendicular thickness of the track (e.g., height for horizontal track).
  uint8_t _knobRadius;       ///< Radius of the circular slider knob.
  int16_t _currentPixelPos;  ///< Current pixel position of the knob's center along the track, relative to track start.
  int16_t _oldPixelPos;      ///< Previous pixel position of the knob's center for optimized redraws.

  // --- Value and Limits ---
  float _minValue;        ///< Minimum selectable value of the seekbar.
  float _maxValue;        ///< Maximum selectable value of the seekbar.
  float _currentValue;    ///< Current selected value of the seekbar.
  float _lowerLimit;      ///< Lower threshold for limit warning (value below this is "over limit").
  float _upperLimit;      ///< Upper threshold for limit warning (value above this is "over limit").

  // --- Text Display Properties ---
  std::string _title;       ///< Title text displayed for the seekbar.
  const lgfx::IFont* _titleFont;     ///< Font used for the title text.
  const lgfx::IFont* _valueFont;     ///< Font used for the current value text.
  uint8_t _numDigits;       ///< Number of digits before the decimal point for value display.
  uint8_t _numDecimals;     ///< Number of decimal places for value display.
  char _valueFormatBuffer[10]; ///< Buffer for snprintf to format the value string.
  uint8_t _padding;         ///< Padding in pixels between internal elements (e.g., text and track).
  bool _showValueText;      ///< Flag to control visibility of the current value text.
  bool _showTitleText;      ///< Flag to control visibility of the title text.

  // --- Colors ---
  uint32_t _normalColorKnob;      ///< Color of the knob when the value is within normal limits.
  uint32_t _normalColorTrack;     ///< Color of the active track portion when the value is within normal limits.
  uint32_t _limitColorKnob;       ///< Color of the knob when the value is outside defined limits.
  uint32_t _limitColorTrack;      ///< Color of the active track portion when the value is outside defined limits.
  uint32_t _trackBgColor;         ///< Background color of the inactive track portion.
  uint32_t _textColor;            ///< Color of the value text when active.
  uint32_t _titleColor;           ///< Color of the title text when active.
  uint32_t _layerBackgroundColor; ///< Background color of the parent layer, used for clearing.

  uint32_t _disabledTextColor;      ///< Color of text when the seekbar is in a non-interactive state.
  uint32_t _disabledBorderColor;    ///< Color of borders/track when the seekbar is in a non-interactive state.
  uint32_t _disabledBackgroundColor;///< Color of background elements when the seekbar is in a non-interactive state.

  // --- Interaction State ---
  bool _isDragging;               ///< True if the knob is currently being dragged.
  bool _valueHasChangedWhileDragging; ///< True if the value changed at least once during the current drag session.

  // --- Callbacks ---
  ValueChangedCallback _onValueChangedCallback; ///< Callback function for value changes.

  // --- Render Positions (Relative to SeekbarUI's _x_rel, _y_rel) ---
  int16_t _trackRenderX_rel;     ///< Relative X position for drawing the track.
  int16_t _trackRenderY_rel;     ///< Relative Y position for drawing the track.
  int16_t _valueTextRenderX_rel; ///< Relative X position for drawing the value text.
  int16_t _valueTextRenderY_rel; ///< Relative Y position for drawing the value text.
  int16_t _titleTextRenderX_rel; ///< Relative X position for drawing the title text.
  int16_t _titleTextRenderY_rel; ///< Relative Y position for drawing the title text.

  // --- Optimized Redraw States ---
  std::string _oldValueString; ///< Previous string representation of the value for redraw comparison.
  float _oldValue;             ///< Previous float value for redraw comparison (less critical but useful).
  std::string _oldTitle;       ///< Previous title string for redraw comparison.

  bool _knobNeedsRedraw = false;       ///< Flag: True if the knob's position or color changed.
  bool _valueTextNeedsRedraw = false;  ///< Flag: True if the value text content or visibility changed.
  bool _titleTextNeedsRedraw = false;  ///< Flag: True if the title text content or visibility changed.

  // --- Absolute Screen Positions for Clearing Old Content ---
  int16_t _oldKnobAbsX, _oldKnobAbsY; ///< Previous absolute screen coordinates of the knob's center.
  int16_t _oldValueTextAbsX, _oldValueTextAbsY, _oldValueTextAbsW, _oldValueTextAbsH; ///< Bounding box of the previous value text.
  int16_t _oldTitleTextAbsX, _oldTitleTextAbsY, _oldTitleTextAbsW, _oldTitleTextAbsH; ///< Bounding box of the previous title text.

  // --- Private Helper Methods ---
  /**
   * @brief Converts the current value to a pixel position along the track.
   * The result is stored in `_currentPixelPos`.
   */
  void valueToPixelPos();

  /**
   * @brief Converts a pixel position along the track to a corresponding float value.
   * Sets the `_currentValue` and optionally triggers the value changed callback.
   * @param pixelPos The pixel position along the track (relative to track start).
   */
  void pixelPosToValue(int16_t pixelPos);

  /**
   * @brief Draws the seekbar track.
   * @param drawX The absolute X screen coordinate for the track's reference point.
   * @param drawY The absolute Y screen coordinate for the track's reference point.
   * @param normalTrackColor Color of the active track portion when in normal state.
   * @param trackBgColor Color of the inactive track portion.
   * @param effectiveLimitTrackColor Color of the active track portion when over limits.
   */
  void drawTrack(int32_t drawX, int32_t drawY, uint32_t normalTrackColor, uint32_t trackBgColor, uint32_t effectiveLimitTrackColor);

  /**
   * @brief Draws the slider knob.
   * @param drawX The absolute X screen coordinate for the knob's reference point.
   * @param drawY The absolute Y screen coordinate for the knob's reference point.
   * @param normalKnobColor Color of the knob when in normal state.
   * @param normalKnobCenterColor Color of the knob's center when in normal state.
   * @param effectiveLimitKnobColor Color of the knob when over limits.
   * @param effectiveLimitKnobCenterColor Color of the knob's center when over limits.
   */
  void drawKnob(int32_t drawX, int32_t drawY, uint32_t normalKnobColor, uint32_t normalKnobCenterColor, uint32_t effectiveLimitKnobColor, uint32_t effectiveLimitKnobCenterColor);

  /**
   * @brief Draws the current value text.
   * @param drawX The absolute X screen coordinate for the text.
   * @param drawY The absolute Y screen coordinate for the text.
   * @param textColor Color of the text.
   */
  void drawValueText(int32_t drawX, int32_t drawY, uint32_t textColor);

  /**
   * @brief Draws the title text.
   * @param drawX The absolute X screen coordinate for the text.
   * @param drawY The absolute Y screen coordinate for the text.
   * @param titleColor Color of the text.
   */
  void drawTitleText(int32_t drawX, int32_t drawY, uint32_t titleColor);

  /**
   * @brief Calculates the relative rendering positions for the track, value text, and title text.
   * These positions are relative to the seekbar component's top-left corner (`_x_rel`, `_y_rel`).
   */
  void _calculateRenderPositions();

  /**
   * @brief Recalculates the entire layout of the seekbar, including track length and component positions.
   * This method is called when the size or orientation changes.
   */
  void _recalculateLayout();

  /**
   * @brief Calculates the bounding box of a given text string.
   * @param text The string for which to calculate bounds.
   * @param font The font used for the text.
   * @param drawX_abs The absolute X screen coordinate where the text would be drawn (before datum adjustment).
   * @param drawY_abs The absolute Y screen coordinate where the text would be drawn (before datum adjustment).
   * @param datum The text alignment datum.
   * @param outX Output: Absolute X coordinate of the top-left corner of the bounding box.
   * @param outY Output: Absolute Y coordinate of the top-left corner of the bounding box.
   * @param outW Output: Width of the bounding box.
   * @param outH Output: Height of the bounding box.
   */
  void _getTextBounds(const std::string& text, const lgfx::IFont* font,
                      int16_t drawX_abs, int16_t drawY_abs, lgfx::textdatum_t datum,
                      int16_t& outX, int16_t& outY, int16_t& outW, int16_t& outH);

  /**
   * @brief Calculates the effective content rectangle for the entire seekbar element.
   * This rectangle encompasses all visible parts (track, knob, texts) and is used for clearing/redrawing the full area.
   * @param outX Output: Absolute X coordinate of the top-left corner of the content rectangle.
   * @param outY Output: Absolute Y coordinate of the top-left corner of the content rectangle.
   * @param outW Output: Width of the content rectangle.
   * @param outH Output: Height of the content rectangle.
   * @param screenOffsetX The X offset of the parent layer on the screen.
   * @param screenOffsetY The Y offset of the parent layer on the screen.
   */
  void _getEffectiveContentRect(int16_t& outX, int16_t& outY, int16_t& outW,
                                int16_t& outH, int16_t screenOffsetX, int16_t screenOffsetY);
};

#endif // SEEKBAR_UI_H