/**
 * @file ToggleButtonUI.h
 * @brief Defines the ToggleButtonUI class, an interactive button with two distinct states (ON/OFF).
 *
 * This class provides a customizable toggle button component that visually represents
 * an ON or OFF state. It supports dynamic labels, custom fonts and colors for both
 * states, and robust touch interaction with "sticky" detection. It integrates with
 * the UI framework for consistent styling and behavior, and triggers a callback
 * function upon state changes.
 *
 * @version 1.0.9
 * @date 2025-08-07
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
 
#ifndef TOGGLEBUTTONUI_H
#define TOGGLEBUTTONUI_H

#include "UIElement.h"
#include "Config.h" // Includes LGFX, colors, fonts, DEBUG macros
#include <string>
#include <functional> // Required for std::function

/**
 * @brief Callback type for state changes of the ToggleButtonUI.
 * Passes the new state of the button (true for ON, false for OFF).
 */
using ToggleCallback = std::function<void(bool newState)>;

/**
 * @brief A versatile UI element for interactive toggle buttons with two states.
 *
 * The ToggleButtonUI class provides a customizable button that can switch
 * between two states, typically "ON" and "OFF". It visually indicates its
current state and supports dynamic labels, custom fonts, and distinct color
schemes for each state. The button incorporates "sticky" touch detection for
robust interaction and executes a user-defined callback when its state
changes.
 */
class ToggleButtonUI : public UIElement {
protected:
  // --- Text Content & Visual Properties ---
  std::string _labelOn;           ///< Label displayed when the button is in the ON state.
  std::string _labelOff;          ///< Label displayed when the button is in the OFF state.
  const lgfx::IFont* _font;       ///< The font used for rendering the button's labels.

  // --- Layout & Sizing ---
  int16_t _x_rel;                 ///< Relative X position of the button's top-left corner within its parent layer.
  int16_t _y_rel;                 ///< Relative Y position of the button's top-left corner within its parent layer.
  int16_t _width;                 ///< Configured width of the button.
  int16_t _height;                ///< Configured height of the button.
  uint8_t _cornerRadius;          ///< Radius for rounded corners of the button.

  // --- Interactive State & Logic ---
  int16_t _stickyMargin;          ///< Margin around the button for "sticky" touch detection.
  bool _isOn;                     ///< The current logical state of the button (true for ON, false for OFF).
  bool _isPressedState;           ///< True if the button is currently visually in a pressed state.
  bool _wasEverPressedInThisCycle;///< Flag indicating if the button was ever pressed during the current touch cycle (press to release).
  ToggleCallback _onToggleCallback; ///< Callback function executed when the button's state changes.

  // --- Color Schemes for OFF State ---
  uint32_t _outlineColorOff;      ///< Outline color when the button is OFF.
  uint32_t _fillColorOffNormal;   ///< Fill color when the button is OFF and in its normal state.
  uint32_t _fillColorOffPressed;  ///< Fill color when the button is OFF and visually pressed.
  uint32_t _textColorOffNormal;   ///< Text color when the button is OFF and in its normal state.

  // --- Color Schemes for ON State ---
  uint32_t _outlineColorOn;       ///< Outline color when the button is ON.
  uint32_t _fillColorOnNormal;    ///< Fill color when the button is ON and in its normal state.
  uint32_t _fillColorOnPressed;   ///< Fill color when the button is ON and visually pressed.
  uint32_t _textColorOnNormal;    ///< Text color when the button is ON and in its normal state.

public:
  // --- Constructor ---
  /**
   * @brief Constructor for the ToggleButtonUI class.
   * Initializes the toggle button with labels, position, size, font, and initial state.
   * Default colors are set from configuration constants.
   * @param lcd Pointer to the LGFX display instance.
   * @param labelOn The label text for the ON state.
   * @param labelOff The label text for the OFF state.
   * @param x The X coordinate of the button's top-left corner (relative to the layer).
   * @param y The Y coordinate of the button's top-left corner (relative to the layer).
   * @param w The width of the button.
   * @param h The height of the button.
   * @param font The font to use for rendering the labels.
   * @param initialState The initial logical state of the button (true for ON, false for OFF, default: false).
   * @param cornerRadius The radius for rounded corners (default: BUTTON_DEFAULT_CORNER_RADIUS_PIXELS).
   * @param stickyMargin The margin for "sticky" touch detection (default: -1 for automatic calculation based on height).
   */
  ToggleButtonUI(LGFX* lcd,
                 const std::string& labelOn,
                 const std::string& labelOff,
                 int16_t x, int16_t y, int16_t w, int16_t h,
                 const lgfx::IFont* font,
                 bool initialState = false,
                 uint8_t cornerRadius = BUTTON_DEFAULT_CORNER_RADIUS_PIXELS,
                 int16_t stickyMargin = -1);

  // --- Setters for Text Content & Visual Properties ---
  /**
   * @brief Sets the labels for both the ON and OFF states of the button.
   * Requests a redraw if either label changes.
   * @param labelOn The new label text for the ON state.
   * @param labelOff The new label text for the OFF state.
   */
  void setLabels(const std::string& labelOn, const std::string& labelOff);

  /**
   * @brief Sets the font for rendering the button's labels.
   * Requests a redraw if the font changes.
   * @param font The new font.
   */
  void setFont(const lgfx::IFont* font);

  // --- Setters for Layout & Sizing ---
  /**
   * @brief Sets the relative position of the button within its parent layer.
   * Requests a redraw if the position changes.
   * @param x The new relative X coordinate.
   * @param y The new relative Y coordinate.
   */
  void setPosition(int16_t x, int16_t y) override;

  /**
   * @brief Sets the size (width and height) of the button.
   * Adjusts the sticky margin proportionally if it was set to default.
   * Requests a redraw if the size changes.
   * @param w The new width.
   * @param h The new height.
   */
  void setSize(int16_t w, int16_t h) override;

  /**
   * @brief Sets the corner radius for the button.
   * Requests a redraw if the radius changes.
   * @param r The new corner radius in pixels.
   */
  void setCornerRadius(uint8_t r);

  /**
   * @brief Sets the margin around the button for "sticky" touch detection.
   * @param margin The sticky margin in pixels.
   */
  void setStickyMargin(int16_t margin);

  // --- Setters for Color Schemes ---
  /**
   * @brief Sets the color scheme for the OFF state of the button.
   * Requests a redraw if any color changes.
   * @param outline Outline color for the OFF state.
   * @param fillNormal Fill color when OFF and in normal (unpressed) state.
   * @param fillPressed Fill color when OFF and visually pressed.
   * @param textNormal Text color when OFF and in normal state.
   */
  void setColorsOff(uint32_t outline, uint32_t fillNormal, uint32_t fillPressed, uint32_t textNormal);

  /**
   * @brief Sets the color scheme for the ON state of the button.
   * Requests a redraw if any color changes.
   * @param outline Outline color for the ON state.
   * @param fillNormal Fill color when ON and in normal (unpressed) state.
   * @param fillPressed Fill color when ON and visually pressed.
   * @param textNormal Text color when ON and in normal state.
   */
  void setColorsOn(uint32_t outline, uint32_t fillNormal, uint32_t fillPressed, uint32_t textNormal);

  // --- Setters & Getters for Logical State ---
  /**
   * @brief Sets the logical state of the button (ON/OFF).
   * Requests a redraw if the state changes. Optionally triggers the state change callback.
   * @param isOn The new state (true for ON, false for OFF).
   * @param triggerCallback True to execute the `onToggleCallback`, false otherwise (default: false).
   */
  void setState(bool isOn, bool triggerCallback = false);

  /**
   * @brief Retrieves the current logical state of the button.
   * @return True if the button is ON, false if it is OFF.
   */
  bool getState() const;

  // --- Getters for Layout & Sizing ---
  /**
   * @brief Retrieves the relative X position of the button.
   * @return The relative X coordinate in pixels.
   */
  int16_t getX() const { return _x_rel; }

  /**
   * @brief Retrieves the relative Y position of the button.
   * @return The relative Y coordinate in pixels.
   */
  int16_t getY() const { return _y_rel; }

  /**
   * @brief Retrieves the width of the button.
   * @return The width in pixels.
   */
  int16_t getWidth() const override { return _width; }

  /**
   * @brief Retrieves the height of the button.
   * @return The height in pixels.
   */
  int16_t getHeight() const override { return _height; }

  // --- Setters for Callbacks ---
  /**
   * @brief Sets the callback function to be executed when the button's state changes.
   * @param callback The `ToggleCallback` function.
   */
  void setOnToggleCallback(ToggleCallback callback);

  // --- Core UIElement Overrides ---
  /**
   * @brief Draws the toggle button onto the display.
   * Renders the button with its current logical state (ON/OFF) and visual state
   * (normal, pressed, disabled), applying appropriate labels and colors.
   */
  void draw() override;

  /**
   * @brief Updates the internal state of the toggle button.
   * For ToggleButtonUI, this method is typically empty, as its state changes
   * are driven by touch input.
   */
  void update() override;

  /**
   * @brief Handles touch input for the toggle button.
   * Manages visual feedback (pressed state) and triggers the state change
   * and its associated callback upon release within the "sticky" touch zone.
   * @param tx_layer The X coordinate of the touch input (relative to the layer).
   * @param ty_layer The Y coordinate of the touch input (relative to the layer).
   * @param isCurrentlyPressed True if the screen is currently pressed, false otherwise.
   * @return True if the button handled the touch event, false otherwise.
   */
  bool handleTouch(int32_t tx_layer, int32_t ty_layer, bool isCurrentlyPressed) override;

  /**
   * @brief Overrides the base class method to provide the specific UI element type for ToggleButtonUI.
   * @return The UIElementType enum value representing a TOGGLE_BUTTON element.
   */
  UIElementType getElementType() const override { return UIElementType::TOGGLE_BUTTON; }
};

#endif // TOGGLEBUTTONUI_H