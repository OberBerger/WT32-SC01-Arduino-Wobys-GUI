/**
 * @file ConfigUIUser.h
 * @brief User-configurable UI styling and thematic definitions for the WobysGUI.
 *
 * This file provides a centralized place to customize the visual appearance
 * of UI elements, including color palettes, default element sizes, and fonts.
 * Modifying these definitions allows for easy theme changes across the application.
 *
 * @version 1.0.5
 * @date 2025-08-20
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

#pragma once

#ifndef CONFIG_UI_USER_H
#define CONFIG_UI_USER_H

/**
 * @brief Primary Configuration File Imports.
 * Ensure all necessary base configurations are included for UI definitions.
 */
#include "Config.h"
#include "ConfigHardwareUser.h"
#include "ConfigLGFXUser.h"
#include "ConfigFonts.h"
#include <LovyanGFX.hpp> // Required for UI element rendering types.

/**
 * @brief UI Color Palette Definitions.
 *
 * Defines a consistent color scheme for the application's user interface.
 */
#define UI_COLOR_PRIMARY          0x33b5e5U ///< Primary accent color (e.g., a shade of blue).
#define UI_COLOR_PRIMARY_DARK     0x2f7d99U ///< Darker shade of primary color.
#define UI_COLOR_ALERT            0xff0000U ///< Alert/Error color (e.g., red).
#define UI_COLOR_ALERT_DARK       0xa20000U ///< Darker shade of alert color.
#define UI_COLOR_TEXT_DEFAULT     0xFFFFFFU ///< Default text color (white).
#define UI_COLOR_TEXT_DIM         0xD3D3D3U ///< Dimmed text color (light gray).
#define UI_COLOR_BACKGROUND_DARK  0x000000U ///< Darkest background color (black).
#define UI_COLOR_BACKGROUND_MEDIUM 0x383838U ///< Medium gray for backgrounds/separators.
#define UI_COLOR_BACKGROUND_DEEP  0x101010U ///< Very dark gray for deep backgrounds.

#define UI_COLOR_TEXT_DISABLED       0x808080U ///< Text color for disabled/non-interactive elements.
#define UI_COLOR_BORDER_DISABLED     0x404040U ///< Border color for disabled/non-interactive elements.
#define UI_COLOR_BACKGROUND_DISABLED 0x202020U ///< Background color for disabled/non-interactive elements.
#define UI_COLOR_WARNING             0xFFD700U ///< Warning/Error color (e.g., golden yellow).

/**
 * @brief Default Layout Values (in Pixels, optimized for WT32-SC01 Plus).
 *
 * These values are used as defaults by UI elements and can be overridden by setters.
 */

// --- General Layout ---
#define UI_DEFAULT_MARGIN_PIXELS                10  ///< Default margin for high-level UI layouts (e.g., in BLEUI, WifiUI).
#define UI_DEFAULT_PADDING_PIXELS               5   ///< Default padding for high-level UI layouts.
#define UI_DEFAULT_PANEL_INNER_PADDING_PIXELS   5   ///< Default inner padding for container panels (e.g., in SettingsUI).
#define UI_DEFAULT_TOPBAR_HEIGHT_PIXELS         30  ///< Default height for top bars in UI panels.
#define UI_DEFAULT_BUTTON_HEIGHT_PIXELS         30  ///< Default height for standard buttons.

// --- Statusbar and Panel ---
#define STATUSBAR_HEIGHT                        20  ///< Height reserved for the top status bar in pixels.
#define MESSAGEBOARD_DEFAULT_PADDING_PIXELS     5   ///< Padding for the message board element in the status bar.
#define PANEL_FIXED_HEIGHT                      60  ///< Total fixed height of the pull-down statusbar panel.
#define PANEL_BUTTON_HEIGHT                     30  ///< Fixed height for statusbar panel buttons.
#define PANEL_BUTTON_MARGIN_X                   10  ///< Horizontal spacing between statusbar panel buttons.
#define PANEL_PADDING_X                         11  ///< Horizontal padding from statusbar panel edges to buttons.
#define PANEL_BUTTON_POS_Y                      10  ///< Y-position of buttons within the statusbar panel (relative to panel top).
#define PANEL_CLOSE_OFFSET                      0.0f ///< Animation offset for panel closing (float value).

// --- TextUI ---
#define TEXTUI_AUTO_SIZE                        -1  ///< Special value for automatic UI element sizing.
#define TEXTUI_TRANSPARENT                      ((uint32_t)0xFF000001) ///< Special color value indicating transparency for UI elements.
#define TEXTUI_DEFAULT_PADDING_PIXELS           2   ///< Default padding for TextUI elements.
#define TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS  1   ///< Default border thickness for TextUI elements.

// --- ButtonUI & ToggleButtonUI ---
#define BUTTON_DEFAULT_CORNER_RADIUS_PIXELS     6   ///< Default corner radius for buttons.
#define BUTTON_PRESS_DETECTION_THRESHOLD_PIXELS 5   ///< Touch tolerance for press detection.
#define BUTTON_DEFAULT_STICKY_MARGIN_FACTOR     1.0f ///< Default sticky margin factor relative to button height.

// --- ClickableListUI ---
#define LISTUI_DEFAULT_ITEM_HEIGHT_PIXELS       30  ///< Default height for items in a list.
#define LISTUI_DEFAULT_ITEM_CONTENT_PADDING_PIXELS 3 ///< Default content padding for items in a list.
#define LISTUI_SCROLL_BAR_WIDTH_PIXELS          4   ///< Fixed width of scroll bars in list UI elements.
#define LISTUI_MIN_COL_WIDTH_PIXELS             50  ///< Minimum column width in lists.
#define LISTUI_DRAG_THRESHOLD_PIXELS            5   ///< Touch drag detection threshold for scrolling.

// --- SeekbarUI ---
#define SEEKBAR_DEFAULT_KNOB_RADIUS_PIXELS      10  ///< Default radius of the seekbar knob.
#define SEEKBAR_DEFAULT_TRACK_CROSS_LENGTH_PIXELS 3 ///< Default thickness of the seekbar track.
#define SEEKBAR_DEFAULT_PADDING_PIXELS          10  ///< Default padding around the seekbar track.
#define SEEKBAR_DRAG_INIT_THRESHOLD_PIXELS      8   ///< Touch drag detection threshold for seekbar interaction.

// --- KeyboardUI ---
#define KEYBOARD_DEFAULT_KEY_WIDTH_PIXELS       46  ///< Default width of a keyboard key.
#define KEYBOARD_DEFAULT_KEY_HEIGHT_PIXELS      40  ///< Default height of a keyboard key.
#define KEYBOARD_DEFAULT_OFFSET_X_PIXELS        0   ///< Default X offset of the keyboard from its layer's origin. 
#define KEYBOARD_DEFAULT_OFFSET_Y_PIXELS        0   ///< Default Y offset of the keyboard from its layer's origin. 
#define KEYBOARD_DEFAULT_TEXT_BOX_HEIGHT_PIXELS 50  ///< Default height of the keyboard's text box.
#define KEYBOARD_CURSOR_BLINK_INTERVAL_MS       500 ///< Cursor blink interval in milliseconds.
#define KEYBOARD_LONG_PRESS_DURATION_MS         800 ///< Duration for a long press to trigger accents.
#define KEYBOARD_ACCENT_HOVER_DELAY_MS          400 ///< Delay before an accent panel appears on hover.
#define KEYBOARD_ESC_BUTTON_PADDING_PIXELS      8   ///< Touch hitbox padding for the escape button.
#define KEYBOARD_TEXTBOX_PADDING_PIXELS         5   ///< Inner padding for the text box.
#define KEYBOARD_ESCAPE_BUTTON_ACTION_STRING    "__KB_ESCAPE_BTN_PRESSED__" ///< Internal string to identify keyboard escape action.

// --- ToggleSwitchUI ---
#define TOGGLE_SWITCH_DEFAULT_TRACK_WIDTH_PIXELS 50  ///< Default width of the toggle switch track.
#define TOGGLE_SWITCH_DEFAULT_TRACK_HEIGHT_PIXELS 4  ///< Default height of the toggle switch track.
#define TOGGLE_SWITCH_DEFAULT_KNOB_RADIUS_PIXELS 10  ///< Default radius of the toggle switch knob.
#define TOGGLE_SWITCH_DEFAULT_TITLE_PADDING_Y_PIXELS 2 ///< Default vertical padding for the title text.
#define TOGGLE_SWITCH_DEFAULT_TOUCH_PADDING_PIXELS 8

// --- ScreenSaverManager ---
#define SCREENSAVER_TIMEOUT_MS 30000          ///< Inactivity timeout before screensaver activates (milliseconds).
#define SCREENSAVER_BRIGHT_DURATION_MS 3000   ///< Duration for screensaver to stay bright (milliseconds).
#define SCREENSAVER_BRIGHTNESS_DIM 2          ///< Dim brightness level for screensaver (0-255).
#define SCREENSAVER_BRIGHTNESS_BRIGHT 200     ///< Bright brightness level for screensaver (0-255).
#define SCREENSAVER_SWIPE_THRESHOLD_Y -50     ///< Vertical swipe threshold for screensaver unlock (pixels).
#define SCREENSAVER_TAP_MAX_DURATION_MS 200   ///< Max duration for a tap gesture on screensaver (milliseconds).
#define SCREENSAVER_BRIGHTEN_DURATION_MS 2000 ///< Duration for screensaver to brighten up (milliseconds).
#define SCREENSAVER_DIM_DURATION_MS 2000      ///< Duration for screensaver to dim down (milliseconds).
#define SCREENSAVER_BRIGHT_HOLD_DURATION_MS 3000 ///< Duration screensaver stays bright before dimming (milliseconds).
#define SCREENSAVER_FADE_OUT_DURATION_MS 4000 ///< Duration of initial fade out (milliseconds).
#define SCREENSAVER_PULSE_CYCLE_DURATION_MS 1000 ///< Cycle duration for pulsing elements (milliseconds).
#define SCREENSAVER_BRIGHTNESS_MAX_UI_VALUE 30 ///< Maximum UI value for screensaver brightness slider.
#define SCREENSAVER_MIN_ARROW_INTENSITY 0     ///< Minimum brightness for pulsing elements (0-255).
#define SCREENSAVER_ARROW_WIDTH_PIXELS          100 ///< Width of the unlock arrow.
#define SCREENSAVER_ARROW_HEIGHT_PIXELS         20  ///< Height of the unlock arrow.
#define SCREENSAVER_ARROW_OFFSET_BOTTOM_PIXELS  20  ///< Offset of the unlock arrow from the bottom of the screen.
#define SCREENSAVER_ARROW_BASE_COLOR            0x0000FFU ///< Base color for the pulsing unlock arrow (e.g., blue).

// --- TimeElement ---
#define TIMEELEMENT_VERTICAL_ADJUSTMENT_PIXELS  2   ///< Vertical adjustment for centering the time text in the status bar.

// --- ImageUI ---
#define IMAGEUI_FALLBACK_TEXT_OFFSET_PIXELS     5   ///< Offset for the fallback text when an image fails to load.

// --- GridLayoutUI ---
#define GRID_DEFAULT_LINE_THICKNESS_PIXELS      1   ///< Default thickness of grid lines.
#define GRID_DEFAULT_H_SPACING_PIXELS           5   ///< Default horizontal spacing in the grid.
#define GRID_DEFAULT_V_SPACING_PIXELS           5   ///< Default vertical spacing in the grid.

/**
 * @brief Default UI Element Styling (Colors).
 *
 * Defines consistent color schemes for various UI components.
 */

// --- General Panel Colors ---
#define PANEL_BACKGROUND_COLOR      UI_COLOR_BACKGROUND_DEEP

// --- ButtonUI ---
#define BUTTON_OUTLINE_COLOR        UI_COLOR_TEXT_DEFAULT
#define BUTTON_FILL_COLOR_NORMAL    UI_COLOR_TEXT_DIM
#define BUTTON_TEXT_COLOR_NORMAL    UI_COLOR_BACKGROUND_DARK
#define BUTTON_FILL_COLOR_PRESSED   UI_COLOR_BACKGROUND_MEDIUM
#define BUTTON_TEXT_COLOR_PRESSED   UI_COLOR_TEXT_DEFAULT

// --- Statusbar Panel Buttons ---
#define PANEL_BTN_OUTLINE           UI_COLOR_TEXT_DEFAULT
#define PANEL_BTN_FILL_NORMAL       UI_COLOR_TEXT_DIM
#define PANEL_BTN_TEXT_NORMAL       UI_COLOR_BACKGROUND_DARK
#define PANEL_BTN_FILL_PRESSED      UI_COLOR_BACKGROUND_MEDIUM
#define PANEL_BTN_TEXT_PRESSED      UI_COLOR_TEXT_DEFAULT

// --- KeyboardUI ---
#define KEYBOARD_OUTLINE_COLOR              UI_COLOR_TEXT_DEFAULT
#define KEYBOARD_FILL_COLOR_NORMAL          UI_COLOR_TEXT_DIM
#define KEYBOARD_TEXT_COLOR_NORMAL          UI_COLOR_BACKGROUND_DARK
#define KEYBOARD_FILL_COLOR_PRESSED         UI_COLOR_BACKGROUND_MEDIUM
#define KEYBOARD_TEXT_COLOR_PRESSED         UI_COLOR_TEXT_DEFAULT
#define KEYBOARD_SPECIAL_KEY_ACTIVE_FILL_COLOR UI_COLOR_BACKGROUND_MEDIUM
#define KEYBOARD_SPECIAL_KEY_ACTIVE_TEXT_COLOR UI_COLOR_TEXT_DEFAULT

// --- ToggleButtonUI ---
#define TOGGLE_ON_OUTLINE_COLOR   UI_COLOR_PRIMARY
#define TOGGLE_ON_FILL_NORMAL     UI_COLOR_PRIMARY_DARK
#define TOGGLE_ON_FILL_PRESSED    UI_COLOR_PRIMARY
#define TOGGLE_ON_TEXT_NORMAL     UI_COLOR_TEXT_DEFAULT
#define TOGGLE_OFF_OUTLINE_COLOR  BUTTON_OUTLINE_COLOR
#define TOGGLE_OFF_FILL_NORMAL    BUTTON_FILL_COLOR_NORMAL
#define TOGGLE_OFF_FILL_PRESSED   BUTTON_FILL_COLOR_PRESSED
#define TOGGLE_OFF_TEXT_NORMAL    BUTTON_TEXT_COLOR_NORMAL

// --- ClickableListUI ---
#define LISTUI_BACKGROUND_COLOR               PANEL_BACKGROUND_COLOR
#define LISTUI_BORDER_COLOR                   UI_COLOR_TEXT_DEFAULT
#define LISTUI_DIVIDER_COLOR                  UI_COLOR_BACKGROUND_MEDIUM
#define LISTUI_SCROLLBAR_COLOR                UI_COLOR_TEXT_DIM
#define LISTUI_ITEM_DEFAULT_TEXT_COLOR        UI_COLOR_TEXT_DEFAULT
#define LISTUI_ITEM_DEFAULT_BACKGROUND_COLOR  LISTUI_BACKGROUND_COLOR
#define LISTUI_SELECTED_ITEM_BACKGROUND_COLOR UI_COLOR_PRIMARY_DARK
#define LISTUI_SELECTED_ITEM_TEXT_COLOR       UI_COLOR_TEXT_DEFAULT
#define LISTUI_ITEM_DELETE_ACTION_COLOR       UI_COLOR_ALERT

// --- Dialog Box Colors ---
#define DIALOG_BOX_BACKGROUND_COLOR UI_COLOR_BACKGROUND_DEEP
#define DIALOG_BOX_BORDER_COLOR     UI_COLOR_TEXT_DIM
#define DIALOG_TEXT_COLOR           UI_COLOR_TEXT_DEFAULT

// --- Statusbar Icon Characters ---
// Icons for RFID status
#define RFID_DEFAULT_ICON_CHAR      ((char)0x54) ///< Default RFID icon character ('T').
// Icons for SD Card status
#define SD_CARD_DEFAULT_ICON_CHAR   ((char)0xA7) ///< Default SD Card icon character.
// Icons for Audio Volume status
#define AUDIO_ICON_VOLUME_LOW       "\xC4\x97"   ///< Icon for low audio volume.
#define AUDIO_ICON_VOLUME_MID       "\xC4\x96"   ///< Icon for medium audio volume.
#define AUDIO_ICON_VOLUME_HIGH      "\xC4\x95"   ///< Icon for high audio volume.
#define AUDIO_ICON_DISABLED         " "          ///< Icon when audio is disabled.

#endif // CONFIG_UI_USER_H