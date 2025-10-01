/**
 * @file ScreenManager.h
 * @brief Manages UI layers, screen transitions, and touch input distribution.
 *
 * This class serves as the central orchestrator for the application's user interface,
 * handling a stack of UILayer objects. It manages screen rotation, visibility,
 * and distributes touch events to the currently active layer or the status bar.
 *
 * @version 1.1.5
 * @date 2025-09-01
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
 * This product incorporates software components licensed under various open-source components.
 * Your use of this product is subject to the terms of these licenses.
 * Please refer to the accompanying "LICENSES.txt" file for detailed information
 * on these components, their copyrights, and the obligations required for compliance.
 */

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <string>
#include <vector>
#include <map>

// UILayer definition (which includes UIElement.h and Config.h)
#include "UILayer.h"  

// Forward declaration for StatusbarUI to allow StatusbarUI* pointers
// without including the full StatusbarUI.h here (to avoid circular dependencies).
class StatusbarUI;

/**
 * @brief Manages UI layers, screen transitions, and touch input distribution.
 * The ScreenManager maintains a stack of UILayer objects, controls display rotation,
 * and ensures touch events are routed to the appropriate active UI component.
 */
class ScreenManager {
private:
  //=========================================================================
  // Private Members
  //=========================================================================
  LGFX*                       _lcd;                 ///< Pointer to the LGFX display instance.
  std::map<std::string, UILayer> _definedLayers;    ///< Map of all defined layers by name.
  std::vector<UILayer*>       _layerStack;          ///< Stack of active layers (top is currently visible).
  std::map<const UILayer*, std::string> _layerToNameMap; ///< Maps layer pointers to their names.
  int32_t                     _statusBarHeight;     ///< Height of the fixed status bar area.
  StatusbarUI*                _statusbarPtr;        ///< Pointer to the StatusbarUI instance.

  int32_t                     _lastKnownTouchX_raw; ///< Last known raw X coordinate of touch input.
  int32_t                     _lastKnownTouchY_raw; ///< Last known raw Y coordinate of touch input.
  unsigned long               _updateCycleCount = 0; ///< Counter for update cycles, used for timing.

  int                         _currentRotation;     ///< The display's current physical rotation (LGFX rotation value).
  std::vector<int>            _rotationStack;       ///< Stack of rotations corresponding to the layer stack.
  
  // Default LGFX rotations for content-based layers, passed during construction.
  const int _defaultPortraitLgfxRotation; ///< Default LGFX rotation for CONTENT_PORTRAIT.
  const int _defaultLandscapeLgfxRotation; ///< Default LGFX rotation for CONTENT_LANDSCAPE.

  /*******************************************************************************
   *                      DEMO MODE - START
   ******************************************************************************/
#ifdef DEMO_MODE
    /**
     * @brief Draws a visual watermark or "DEMO LIMIT REACHED" message on the screen.
     *
     * This method is called in demo mode only when the screen draw limit is reached,
     * to provide visual feedback to the user.
     */
    void _drawDemoWatermark();
#endif // DEMO_MODE
  /*******************************************************************************
   *                      DEMO MODE - END
   ******************************************************************************/

public:
  /**
   * @brief Constructor for ScreenManager.
   * @param lcdRef Pointer to the LGFX display instance.
   * @param statusBarHeight The fixed height of the status bar area.
   * @param defaultPortraitLgfxRot The LGFX rotation value to use for CONTENT_PORTRAIT layers
   *                               when the physical display is in a landscape orientation.
   * @param defaultLandscapeLgfxRot The LGFX rotation value to use for CONTENT_LANDSCAPE layers
   *                                when the physical display is in a portrait orientation.
   */
  ScreenManager(LGFX* lcdRef, int32_t statusBarHeight, 
                int defaultPortraitLgfxRot = LGFX_DEFAULT_ROTATION_PORTRAIT, 
                int defaultLandscapeLgfxRot = LGFX_DEFAULT_ROTATION_LANDSCAPE);

  /**
   * @brief Destructor for ScreenManager.
   * Clears internal layer containers. Does NOT delete UILayer pointers.
   */
  ~ScreenManager();


  /**
   * @brief Defines and registers a new UI layer with the ScreenManager.
   * If a layer with the same name already exists, it will be overwritten.
   * @param name The unique name for the layer.
   * @param layerConfigInput The UILayer object to register.
   */
  void defineLayer(const std::string& name, const UILayer& layerConfigInput);

  /**
   * @brief Returns a pointer to the currently active layer on top of the stack.
   * @return Pointer to the top UILayer, or nullptr if the stack is empty.
   */
  UILayer* getTopLayer();

  /**
   * @brief Returns a pointer to a defined layer by its name.
   * @param name The name of the layer to retrieve.
   * @return Pointer to the UILayer, or nullptr if not found.
   */
  UILayer* getLayer(const std::string& name);

  /**
   * @brief Pushes a layer onto the active layer stack and makes it visible.
   * Handles display rotation and hides underlying non-overlay layers.
   * @param name The name of the layer to push.
   */
  void pushLayer(const std::string& name);

  /**
   * @brief Pops the top layer from the stack, making the previous layer active.
   * Restores display rotation and visibility of the underlying layer.
   */
  void popLayer();

  /**
   * @brief Switches directly to a specified layer, clearing the current stack.
   * @param name The name of the layer to switch to.
   */
  void switchToLayer(const std::string& name);

  /**
   * @brief Sets the interactivity state of the currently active (top) layer.
   * @param interactive True to enable touch interaction for the top layer, false to disable.
   */
  void setTopLayerInteractive(bool interactive);

  /**
   * @brief Returns the name of the currently active layer on top of the stack.
   * @return The name of the top layer, or an empty string if the stack is empty.
   */
  std::string getTopLayerName() const;

  /**
   * @brief Main update cycle for the ScreenManager.
   * Handles touch events and updates active layer(s).
   * @param touchAlreadyHandledByStatusbar True if the touch event was already processed by the status bar.
   */
  void loop(bool touchAlreadyHandledByStatusbar = false); 
  
  /**
   * @brief Forces a redraw of the entire screen area covered by the top layer.
   * @note This is typically used when a major layout change occurs or an element
   * requests a full redraw of its parent layer.
   */
  void redraw();

  /**
   * @brief Forces a redraw of a specific rectangular area on the top layer.
   * @param x The X coordinate of the top-left corner of the area (absolute screen coordinates).
   * @param y The Y coordinate of the top-left corner of the area (absolute screen coordinates).
   * @param w The width of the area.
   * @param h The height of the area.
   */
  void forceRedrawArea(int32_t x, int32_t y, int32_t w, int32_t h);
  
  /**
   * @brief Links the StatusbarUI instance to the ScreenManager.
   * Enables coordination of touch processing between the main screen and the status bar.
   * @param sb Pointer to the StatusbarUI instance.
   */
  void setStatusbar(StatusbarUI* sb);

  /**
   * @brief Returns the current update cycle count.
   * @return The number of update cycles processed since system start.
   */
  unsigned long getUpdateCycleCount() const;

  /**
   * @brief Updates the touchability state of the status bar based on the top layer's elements.
   * For example, disables status bar touch when a keyboard is active on the main screen.
   */
  void updateStatusbarTouchability();
};

#endif  // SCREENMANAGER_H