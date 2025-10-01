#ifndef KEYBOARDUI_H
#define KEYBOARDUI_H

/**
 * @file KeyboardUI.h
 * @brief Defines an on-screen virtual keyboard UI element.
 *
 * This class provides an interactive, touch-enabled virtual keyboard for text input.
 * It supports standard characters, shift (caps lock), symbols, and accented characters.
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

#include "UIElement.h" // Provides UIElement base class (which includes Config.h)
#include <string>
#include <vector>
#include <map>
#include <functional> // For std::function

#include "ConfigUIUser.h" // For KEYBOARD_XXX_COLOR defines and font references like helvB12


/**
 * @brief Label for the Caps Lock key.
 */
constexpr const char* KEY_LABEL_CAPS_LOCK = "CL";
/**
 * @brief Label for the Symbols key.
 */
constexpr const char* KEY_LABEL_SYMBOLS   = "@?!";
/**
 * @brief Label for the Left Arrow key (represented by a glyph).
 */
constexpr const char* KEY_LABEL_LEFT      = "M";  // Arrow glyph from font
/**
 * @brief Label for the Right Arrow key (represented by a glyph).
 */
constexpr const char* KEY_LABEL_RIGHT     = "N";  // Arrow glyph from font
/**
 * @brief Label for the Backspace key (represented by a glyph).
 */
constexpr const char* KEY_LABEL_BACKSPACE = "n"; // Backspace glyph from font
/**
 * @brief Label for the Delete key (represented by a glyph).
 */
constexpr const char* KEY_LABEL_DELETE    = "o"; // Delete glyph from font


/**
 * @brief Virtual on-screen keyboard UI element.
 *
 * This class provides an interactive, touch-enabled virtual keyboard for text input
 * on an LGFX display. It manages different keyboard layouts (normal, caps lock, symbols),
 * handles character input, cursor movement, text scrolling, and provides
 * support for accented characters through long-press or hover actions.
 * It also includes an escape button for dismissing the keyboard or performing an action.
 */
class KeyboardUI : public UIElement {
public:
    /**
     * @brief Callback type for the "Enter" key press.
     *
     * This callback provides the currently entered text as a string when the
     * "Enter" key on the keyboard is pressed.
     *
     * @param enteredText The string currently present in the text input field.
     */
    using EnterCallback = std::function<void(const std::string&)>;

    // Constructors and Destructors
    /**
     * @brief Constructs a new KeyboardUI object.
     *
     * Initializes the keyboard with its position, key dimensions, and default state.
     * It also sets up the keyboard layout and accent character map.
     *
     * @param lcd A pointer to the LGFX display object.
     * @param title The title string to be displayed above the keyboard's text box.
     * @param keyW The default width of a single key in pixels. Defaults to KEYBOARD_DEFAULT_KEY_WIDTH_PIXELS.
     * @param keyH The default height of a single key in pixels. Defaults to KEYBOARD_DEFAULT_KEY_HEIGHT_PIXELS.
     * @param offsetX The X offset of the keyboard from the parent layer's left edge in pixels. Defaults to KEYBOARD_DEFAULT_OFFSET_X_PIXELS.
     * @param offsetY The Y offset of the keyboard from the parent layer's top edge in pixels. Defaults to KEYBOARD_DEFAULT_OFFSET_Y_PIXELS.
     * @param textBoxH The height of the text input box in pixels. Defaults to KEYBOARD_DEFAULT_TEXT_BOX_HEIGHT_PIXELS.
     */
    KeyboardUI(LGFX* lcd, const std::string& title,
               int keyW = KEYBOARD_DEFAULT_KEY_WIDTH_PIXELS,
               int keyH = KEYBOARD_DEFAULT_KEY_HEIGHT_PIXELS,
               int offsetX = KEYBOARD_DEFAULT_OFFSET_X_PIXELS,
               int offsetY = KEYBOARD_DEFAULT_OFFSET_Y_PIXELS,
               int textBoxH = KEYBOARD_DEFAULT_TEXT_BOX_HEIGHT_PIXELS);
    
    /**
     * @brief Destructor for KeyboardUI.
     * Handles cleanup of KeyboardUI specific resources.
     */
    ~KeyboardUI() = default;


    // Main Interface Methods
    /**
     * @brief Sets the callback function to be invoked when the "Enter" key is pressed.
     * @param callback The callback function to set. It takes a `std::string` argument
     *                 representing the text currently entered into the keyboard.
     */
    void setOnEnterCallback(EnterCallback callback);

    /**
     * @brief Sets the title displayed above the keyboard.
     * @param newTitle The new title string to display.
     */
    void setTitle(const std::string& newTitle);

    /**
     * @brief Retrieves the text currently entered in the keyboard's text box.
     * @return A `std::string` containing the entered text.
     */
    std::string getEnteredText() const;

    /**
     * @brief Clears the text input field and resets the cursor position.
     *
     * This method marks the text field as dirty, ensuring it will be redrawn
     * in the next `draw()` call.
     */
    void clearText();

    /**
     * @brief Returns the type of this UI element.
     * @return The UIElementType enum value for Keyboard.
     * @override
     */
    UIElementType getElementType() const override { return UIElementType::KEYBOARD; }

    // UI Element Overrides
    /**
     * @brief Sets the visibility state of the keyboard.
     *
     * When the keyboard is made visible, it triggers a full redraw of its background
     * and resets its internal state for a fresh appearance.
     *
     * @param visible `true` to make the keyboard visible, `false` to hide it.
     * @param redraw `true` to force a redraw immediately if becoming visible, `false` otherwise.
     * @override
     */
    void setVisible(bool visible, bool redraw = true) override;

    /**
     * @brief Draws the keyboard UI element on the display.
     *
     * This method handles the rendering of the title bar, text input field,
     * keyboard layout, and accent panel, only redrawing elements that are marked as dirty.
     * @override
     */
    void draw() override;

    /**
     * @brief Updates the internal state of the keyboard UI element.
     *
     * This method is responsible for tasks like cursor blinking.
     * @override
     */
    void update() override;

    /**
     * @brief Handles touch input events for the keyboard.
     *
     * This method processes touch events on the keyboard, including key presses,
     * long presses for accented characters, hover detection, and interaction
     * with the escape button and text input field.
     *
     * @param tx_layer The X coordinate of the touch event relative to the layer's origin.
     * @param ty_layer The Y coordinate of the touch event relative to the layer's origin.
     * @param isPressed `true` if the touch event is a press (finger down or dragging),
     *                  `false` if it's a release (finger up).
     * @return `true` if the touch event was handled by the keyboard, `false` otherwise.
     * @override
     */
    bool handleTouch(int32_t tx_layer, int32_t ty_layer,
                     bool isPressed) override;

    /**
     * @brief Notifies the UI element that its layer background has been cleared.
     *
     * This method is typically called by a parent UI manager (e.g., ScreenManager)
     * when the display area occupied by this element has been refreshed.
     * It triggers an internal reset to ensure proper redrawing upon subsequent visibility.
     * @param cleared `true` if the background has been cleared, `false` otherwise.
     * @override
     */
    void setLayerBackgroundCleared(bool cleared) override;

private:
    // Configuration Variables
    int _keyWidth;                  ///< Width of a single key in pixels.
    int _keyHeight;                 ///< Height of a single key in pixels.
    int _offsetX_kb;                ///< X offset of the keyboard from the parent layer's left edge in pixels.
    int _offsetY_kb;                ///< Y offset of the keyboard from the parent layer's top edge in pixels.
    int _textBoxHeight;             ///< Height of the text input box in pixels.
    int _escapeBtnPaddingPixels;    ///< Padding in pixels around the escape button.
    int _textBoxPaddingPixels;      ///< Padding in pixels inside the text input box.
    unsigned long _cursorBlinkIntervalMs; ///< Interval in milliseconds for the cursor to blink.
    unsigned long _accentHoverDelayMs;    ///< Delay in milliseconds before the accent panel appears on hover.
    unsigned long _longPressDurationMs;   ///< Duration in milliseconds to trigger a long press.

    // State Variables
    std::string _title;             ///< Title text displayed above the keyboard.
    bool _titleBarDirty;            ///< Flag indicating if the title bar needs to be redrawn.
    bool _capsLock;                 ///< Flag indicating if Caps Lock is active.
    bool _symbols;                  ///< Flag indicating if the symbol layout is active.
    std::string _enteredText;       ///< The text currently entered into the text box.
    size_t _cursorPos;              ///< Current position of the cursor within _enteredText (in character count).
    bool _cursorVisible;            ///< Flag indicating if the cursor is currently visible (for blinking).
    bool _longPressDetected;        ///< Flag indicating if a long press was detected.
    bool _lastTouchState;           ///< Previous touch state (true if pressed, false if released).
    bool _keyAppearanceChanged;     ///< Flag indicating if the visual appearance of any key has changed (e.g., Caps Lock state).
    bool _textFieldDirty;           ///< Flag indicating if the text input field needs to be redrawn.
    bool _keyboardLayoutDirty;      ///< Flag indicating if the entire keyboard layout needs to be redrawn.
    bool _needsFullBackgroundRedraw; ///< Flag indicating if the entire keyboard background needs a full redraw.
    int _numAccentsShown;           ///< Number of accent keys currently displayed.
    int _selectedAccentIndex;       ///< Index of the accent key currently selected from the accent panel.
    bool _isLongPressCandidate;     ///< Flag indicating if the current touch is a candidate for a long press.
    bool _isHoveringForAccent;      ///< Flag indicating if the system is currently hovering to show accent keys.
    bool _isEscapeBtnVisuallyPressed; ///< Flag indicating if the escape button is currently visually pressed.
    bool _escapeButtonWasCandidateForPress; ///< Flag indicating if the escape button was the target of a press event during the current touch interaction.
    int _textScrollOffset_px;       ///< Current horizontal scroll offset of the text in the input box in pixels.

    // UI Element Specific Members
    int _ui_width;                  ///< Calculated total width of the KeyboardUI element.
    int _ui_height;                 ///< Calculated total height of the KeyboardUI element.
    int _escapeBtnX_abs;            ///< Absolute screen X coordinate of the escape button.
    int _escapeBtnY_abs;            ///< Absolute screen Y coordinate of the escape button.
    int _escapeBtnW;                ///< Width of the escape button.
    int _escapeBtnH;                ///< Height of the escape button.
    int32_t _accentsDrawX;          ///< Absolute screen X coordinate where accent keys are drawn.
    int32_t _accentsDrawY;          ///< Absolute screen Y coordinate where accent keys are drawn.
    const char* _escapeBtnCharLabel;///< Character label displayed on the escape button (expected to be a font glyph).
    const lgfx::IFont* _escapeBtnFont; ///< Font used for the escape button label.

    // Callback Objects
    EnterCallback _onEnterCallback; ///< Callback function to be invoked when the "Enter" key is pressed.

    // Timers
    unsigned long _lastCursorBlink; ///< Millis timestamp of the last cursor blink state change.
    unsigned long _touchStartTime;  ///< Millis timestamp when the current touch was initiated.
    unsigned long _longPressTimerStart; ///< Millis timestamp when long press timer started.
    unsigned long _hoverAccentTimerStart; ///< Millis timestamp when hover for accent panel started.

    // Keyboard Layout Data
    static const int ROWS = 5;      ///< Number of rows in the keyboard layout.
    static const int COLS = 12;     ///< Number of columns in the keyboard layout.
    const char* _keys[ROWS][COLS];  ///< 2D array storing character labels for each key in the keyboard layout.
    std::map<std::string, std::vector<std::string>> _accentMap; ///< Map storing accented character variations for base characters.
    std::vector<std::string> _currentAccents; ///< Vector storing the currently displayed accented characters in the accent panel.

    // Touch & Press Tracking (Transient Visual State)
    uint16_t _startX_touch, _startY_touch; ///< Global X and Y coordinates where the touch started.
    const char* _currentlyPressedKeyVisual = nullptr; ///< Pointer to the character label of the key currently visually pressed.
    int _pressedKeyRow = -1, _pressedKeyCol = -1; ///< Row and column of the key currently visually pressed.
    int _lastDetectedKeyRow;        ///< Row of the key last detected under the finger.
    int _lastDetectedKeyCol;        ///< Column of the key last detected under the finger.
    int _lastDetectedAccentIndex;   ///< Index of the accent key last detected under the finger.
    const char* _currentlySelectedAccentVisual = nullptr; ///< Pointer to the character label of the accent key currently visually selected.
    const char* _longPressBaseKey = nullptr; ///< Pointer to the base key character for which long press is active.
    int _longPressBaseKeyRow = -1, _longPressBaseKeyCol = -1; ///< Row and column of the base key for which long press is active.
    const char* _hoverAccentBaseKey = nullptr; ///< Pointer to the base key character for which accent hover is active.
    int _hoverAccentBaseKeyRow = -1, _hoverAccentBaseKeyCol = -1; ///< Row and column of the base key for which accent hover is active.
    int _lastVisuallyActiveKeyRow = -1; ///< Row of the last key that was visually active (pressed).
    int _lastVisuallyActiveKeyCol = -1; ///< Column of the last key that was visually active (pressed).

    // Color Constants (initialized in .cpp from ConfigUIUser.h)
    const uint32_t KEY_DEFAULT_FILL_COLOR;      ///< Default fill color for regular keys.
    const uint32_t KEY_DEFAULT_TEXT_COLOR;      ///< Default text color for regular keys.
    const uint32_t KEY_DEFAULT_BORDER_COLOR;    ///< Default border color for keys.
    const uint32_t KEY_PRESSED_FILL_COLOR;      ///< Fill color for a pressed key.
    const uint32_t KEY_PRESSED_TEXT_COLOR;      ///< Text color for a pressed key.
    const uint32_t ACCENT_KEY_DEFAULT_FILL_COLOR; ///< Default fill color for accent keys.
    const uint32_t ACCENT_KEY_DEFAULT_TEXT_COLOR; ///< Default text color for accent keys.
    const uint32_t ACCENT_KEY_SELECTED_FILL_COLOR; ///< Fill color for a selected accent key.
    const uint32_t ACCENT_KEY_SELECTED_TEXT_COLOR; ///< Text color for a selected accent key.
    const uint32_t SPECIAL_KEY_ACTIVE_FILL_COLOR; ///< Fill color for an active special key (e.g., Caps Lock on).
    const uint32_t SPECIAL_KEY_ACTIVE_TEXT_COLOR; ///< Text color for an active special key.

    // Default Fonts (static const pointers)
    static const lgfx::IFont* defaultKeyFont;        ///< Pointer to the default font for regular keys.
    static const lgfx::IFont* specialFunctionKeyFont; ///< Pointer to the font for special function keys (e.g., Caps Lock, Symbols, Enter).
    static const lgfx::IFont* arrowNavigationKeyFont; ///< Pointer to the font for arrow and action keys (e.g., left, right, backspace, delete).

    // Internal Helper Methods
    /**
     * @brief Sets up the current keyboard key layout based on Caps Lock and Symbol states.
     *
     * This method copies the appropriate key character arrays into the `_keys` member
     * and sets the `_keyboardLayoutDirty` flag to true, triggering a redraw.
     */
    void setupKeys();

    // NOTE: The following setter methods for keyboard dimensions and offsets are declared
    // but not called from within the KeyboardUI class itself. They could be part of
    // a public API if external configuration is desired, or removed if unused.
    /**
     * @brief Sets the width of individual keys on the keyboard.
     * @param width The new key width in pixels.
     */
    void setKeyWidth(int width);
    /**
     * @brief Sets the height of individual keys on the keyboard.
     * @param height The new key height in pixels.
     */
    void setKeyHeight(int height);
    /**
     * @brief Sets the X offset of the entire keyboard layout.
     * @param offset The new X offset in pixels from the parent layer's left edge.
     */
    void setOffsetX(int offset);
    /**
     * @brief Sets the Y offset of the entire keyboard layout.
     * @param offset The new Y offset in pixels from the parent layer's top edge.
     */
    void setOffsetY(int offset);
    /**
     * @brief Sets the height of the text input box.
     * @param height The new text box height in pixels.
     */
    void setTextBoxHeight(int height);
    /**
     * @brief Sets the blinking interval for the cursor in the text input box.
     * @param intervalMs The new blink interval in milliseconds.
     */
    void setCursorBlinkInterval(unsigned long intervalMs);
    /**
     * @brief Sets the delay for showing the accent panel on key hover.
     * @param delayMs The new hover delay in milliseconds.
     */
    void setAccentHoverDelay(unsigned long delayMs);
    /**
     * @brief Sets the duration required for a long press to be detected.
     * @param durationMs The new long press duration in milliseconds.
     */
    void setLongPressDuration(unsigned long durationMs);
    /**
     * @brief Sets the padding around the escape button.
     * @param pixels The new padding in pixels.
     */
    void setEscapeButtonPadding(int pixels);
    /**
     * @brief Sets the internal padding of the text input box.
     * @param pixels The new padding in pixels.
     */
    void setTextBoxPadding(int pixels);


    /**
     * @brief Initializes the map of base characters to their accented variations.
     *
     * This map is used to display accent options when a key is long-pressed or hovered.
     */
    void setupAccentMap();

    /**
     * @brief Calculates the size (number of bytes) of a UTF-8 character.
     * @param s A pointer to the start of the UTF-8 character.
     * @return The size of the UTF-8 character in bytes, or 0 if it's a null terminator or invalid.
     */
    size_t getUTF8CharSize(const char* s);

    /**
     * @brief Calculates the size (number of bytes) of a UTF-8 character at a specific string index.
     * @param char_idx_in_string The byte index within the `_enteredText` string.
     * @return The size of the UTF-8 character in bytes, or 0 if the index is out of bounds or invalid.
     */
    size_t getUTF8CharSizeAtCursor(size_t char_idx_in_string);

    /**
     * @brief Draws the entire keyboard layout, including all keys and their labels.
     *
     * This method iterates through the `_keys` array and renders each key,
     * applying appropriate colors and fonts based on its state (pressed, active special key, or default).
     */
    void drawKeyboardLayout();

    /**
     * @brief Draws the text input field, including the entered text and the blinking cursor.
     *
     * This method handles text scrolling if the entered text exceeds the visible width
     * of the text box. It only redraws if the text field is marked as dirty.
     */
    void drawTextField();

    /**
     * @brief Redraws a single accent key in the accent panel.
     *
     * Used to change the visual state of an accent key (e.g., when selected).
     *
     * @param accentIndex The index of the accent character in `_currentAccents` to redraw.
     * @param isSelectedLook `true` to draw the key with a selected appearance, `false` for default.
     */
    void redrawSingleAccentKey(int accentIndex, bool isSelectedLook);

    /**
     * @brief Draws the accent panel, displaying available accented characters.
     *
     * This panel appears when a base key is long-pressed or hovered over,
     * showing its character variations.
     */
    void drawAccentPanel();

    /**
     * @brief Clears the entire accent panel area on the display.
     *
     * Resets internal state related to the accent panel.
     */
    void clearAccentPanel();

    /**
     * @brief Determines which keyboard key, if any, is currently being pressed.
     *
     * @param touchX_on_kb The X coordinate of the touch relative to the keyboard's origin (excluding global offset).
     * @param touchY_on_kb The Y coordinate of the touch relative to the keyboard's origin (excluding global offset).
     * @return A pointer to the character label of the pressed key, or `nullptr` if no key is pressed.
     */
    const char* getPressedKey(int32_t touchX_on_kb, int32_t touchY_on_kb);

    /**
     * @brief Determines which accent key, if any, is currently being pressed in the accent panel.
     *
     * @param touchX_global The absolute X coordinate of the touch on the screen.
     * @param touchY_global The absolute Y coordinate of the touch on the screen.
     * @return A pointer to the character label of the pressed accent key, or `nullptr` if no accent key is pressed.
     */
    const char* getPressedAccent(int32_t touchX_global, int32_t touchY_global);

    /**
     * @brief Redraws a single key on the keyboard, applying a specified visual style.
     *
     * This is used to change the appearance of a key (e.g., when it's pressed or active).
     *
     * @param r The row index of the key to redraw.
     * @param c The column index of the key to redraw.
     * @param isPressedLook `true` to draw the key with a pressed appearance, `false` for default.
     */
    void redrawSingleKey(int r, int c, bool isPressedLook);

    /**
     * @brief Clears the accent panel if it is currently visible.
     *
     * Resets the internal state related to the accent panel, including `_currentAccents`,
     * `_numAccentsShown`, and `_selectedAccentIndex`.
     */
    void clearAccentPanelIfNeeded();

    /**
     * @brief Draws the title bar above the text input field.
     *
     * This includes rendering the keyboard's title and the escape button.
     * It only redraws if the title bar is marked as dirty.
     */
    void drawTitleBar();

    /**
     * @brief Updates the horizontal scroll offset of the text in the input box.
     *
     * This ensures the cursor remains visible within the text box,
     * scrolling the text as needed. It sets `_textFieldDirty` if the offset changes.
     */
    void updateTextScrollOffset();

    /**
     * @brief Resets the internal state of the keyboard to prepare for its next appearance.
     *
     * This method flags various UI elements for a full redraw and resets cursor/scroll
     * states to ensure a clean presentation.
     */
    void _resetForNextAppearance();
};

#endif // KEYBOARDUI_H