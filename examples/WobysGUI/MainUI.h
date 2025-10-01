#ifndef MAINUI_H
#define MAINUI_H

/**
 * @file MainUI.h
 * @brief Defines the MainUI class, the primary user interface for the application.
 *
 * This class orchestrates the main application screen, containing various UI elements
 * such as seekbars, toggle switches, a clickable list, and buttons. It manages
 * layout adjustments for different screen orientations, handles user input,
 * and interacts with other managers (ScreenManager, PowerManager, LanguageManager)
 * to provide core application functionality.
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

// Required includes for UI elements and managers declarations
#include "Config.h"            // Main configuration file, basic definitions
#include "ScreenManager.h"     // For ScreenManager class
#include "PowerManager.h"      // For PowerManager class
#include "SeekbarUI.h"         // For SeekbarUI class
#include "ButtonUI.h"          // For ButtonUI class
#include "KeyboardUI.h"        // For KeyboardUI class
#include "TextUI.h"            // For TextUI class
#include "ToggleSwitchUI.h"    // For ToggleSwitchUI class
#include "ClickableListUI.h"   // For ClickableListUI class
#include "MessageBoardElement.h" // IMPORTANT: This include is needed for _messageBoardPtr type
#include "GridLayoutUI.h"      // For GridLayoutUI class
#include "ToggleButtonUI.h"    // For ToggleButtonUI class
#include "LanguageManager.h"   // For LanguageManager class
#include "AudioManager.h"      // For AudioManager class
#include "RFIDManager.h"       // For RFIDManager class

/**
 * @brief Defines the number of seekbar objects used in the MainUI.
 * Currently set to 3 for R, G, B color components.
 */
#define MAINUI_SEEKBAR_COUNT 3

/**
 * @brief Manages the primary user interface of the application.
 *
 * The MainUI class is responsible for displaying and managing various UI elements
 * that constitute the main application screen. It handles screen layout changes
 * based on orientation, processes user interactions, and integrates with other
 * core system managers to provide a cohesive user experience.
 */
class MainUI {
private:
    // Core Dependencies
    LGFX* _lcd;                         ///< Pointer to the LGFX display object.
    ScreenManager* _screenManager;      ///< Pointer to the ScreenManager for layer navigation.
    PowerManager* _powerManager;        ///< Pointer to the PowerManager for power-related actions (unused in provided code).
    MessageBoardElement* _messageBoardPtr; ///< Pointer to the MessageBoardElement for displaying temporary messages.
    LanguageManager* _languageManager;  ///< Pointer to the LanguageManager for multi-language support.
    AudioManager* _audioManager;        ///< Pointer to the AudioManager for sound control.

    // UI Elements
    SeekbarUI _rgbSeekbars[MAINUI_SEEKBAR_COUNT]; ///< Array of seekbar UIs for RGB color adjustment.
    ButtonUI _rotateOrientationButton;            ///< Button to rotate screen orientation.
    ToggleSwitchUI _colorModeToggle;              ///< Toggle switch to enable/disable color adjustment mode.
    TextUI _dynamicColorText;                     ///< Text UI element whose color dynamically changes with seekbar values.
    ToggleSwitchUI _listControlToggle;            ///< Toggle switch to enable/disable interactivity of the feature list.
    ClickableListUI _featureList;                 ///< A clickable list UI element for managing items.
    ButtonUI _addListItemButton;                  ///< Button to add new items to the feature list.
    ToggleButtonUI _gridVisualizationToggle;      ///< Toggle button to show/hide the grid layout visualization.
    GridLayoutUI _gridVisualization;              ///< Grid layout visualization element.
    KeyboardUI _keyboard;                         ///< Virtual keyboard UI element for text input.
    TextUI _statusLabel;                          ///< Label to display general status information.

    // Confirmation Dialog Elements
    TextUI _confirmBackground;                    ///< Background panel for the confirmation dialog.
    TextUI _confirmQuestion;                      ///< Text UI to display the confirmation question.
    TextUI _confirmItemName;                      ///< Text UI to display the name of the item being confirmed.
    ButtonUI _confirmNoBtn;                       ///< Button to cancel the confirmation.
    ButtonUI _confirmYesBtn;                      ///< Button to confirm the action.

    // Internal State Variables
    std::string _itemToForget;                    ///< Stores the name of the item subject to confirmation/deletion.
    int _itemIndexToForget;                       ///< Stores the index of the item subject to confirmation/deletion.
    unsigned long _lastStatusUpdateTimeMillis = 0; ///< Timestamp of the last status label update.
    const unsigned long DEVICE_STATUS_UPDATE_INTERVAL_MS = 1000; ///< Interval for updating device status.

    /**
     * @brief Enumerates the different types of actions a confirmation dialog can be used for.
     */
    enum class ConfirmationAction {
        NONE,             ///< No active confirmation dialog.
        DELETE_LIST_ITEM, ///< Confirming deletion of an item from _featureList.
        ADD_RFID          ///< Confirming addition of an RFID UID to _featureList.
    };
    ConfirmationAction _currentConfirmationAction = ConfirmationAction::NONE; ///< Stores the type of action currently being confirmed.

    RFIDCardData _pendingRfidCardData; ///< Stores the RFID data awaiting user confirmation.

    /**
     * @brief Configures the properties of an array of SeekbarUI elements.
     * Sets titles, value ranges, limits, and initial values for the seekbars.
     * @param seekbarsToConfigure A pointer to the first element of the SeekbarUI array.
     */
    void _configureSeekbars(SeekbarUI* seekbarsToConfigure);

    /**
     * @brief Callback handler for the virtual keyboard's Enter event.
     * Processes the text entered via the keyboard, adds it to the list, or handles cancellation.
     * @param text The string entered by the user, or a special string if cancelled.
     */
    void _onKeyboardEnter(const std::string& text);

    /**
     * @brief Updates the status label display.
     * This method is intended to update general device status information (e.g., time, memory).
     * (Currently not fully implemented in the provided code snippet.)
     */
    void _updateStatusLabel();

    /**
     * @brief Callback handler for the "Add List Item" button press.
     * Triggers the virtual keyboard to allow adding a new item to the list.
     */
    void _onAddListItemPressed();

    /**
     * @brief Callback handler for the Grid Visualization toggle button state change.
     * Toggles the visibility of the `_gridVisualization` element.
     * @param newState The new state of the toggle button (`true` for ON, `false` for OFF).
     */
    void _onToggleGridVisualization(bool newState);

    /**
     * @brief Callback handler for an item selection in the feature list.
     * Handles actions based on which column was clicked (e.g., deleting an item).
     * @param index The index of the selected item.
     * @param data The `ListItem` data of the selected item.
     * @param touchX The X coordinate of the touch within the list item.
     */
    void _onListItemSelected(int index, const ListItem& data, int16_t touchX);

    /**
     * @brief Callback handler for the "Yes" button in the confirmation dialog.
     * Performs the appropriate action (delete list item or add RFID) based on _currentConfirmationAction.
     */
    void _onConfirmDeleteYes();

    /**
     * @brief Callback handler for the "No" button in the confirmation dialog.
     * Cancels the appropriate action.
     */
    void _onConfirmDeleteNo();

    /**
     * @brief Displays a confirmation dialog to the user to add an RFID UID to the list.
     * @param cardData The RFIDCardData object containing the UID to be confirmed.
     */
    void _showRfidConfirmationDialog(const RFIDCardData& cardData);

    /**
     * @brief Callback handler for the Color Mode toggle switch state change.
     * Enables or disables the interactivity and visual state of the RGB seekbars
     * and updates the dynamic color text accordingly.
     * @param newState The new state of the toggle switch (`true` for ON, `false` for OFF).
     */
    void _onColorModeToggleChanged(bool newState);

    /**
     * @brief Callback handler for RGB seekbar value changes.
     * Updates the `_dynamicColorText` color based on the combined RGB values
     * from the seekbars.
     * @param value The new value of the seekbar that triggered the callback.
     * @param isFinalChange `true` if the change is final (finger lifted), `false` otherwise.
     */
    void _onRGBSeekbarValueChanged(float value, bool isFinalChange);

    /**
     * @brief Callback handler for a click on the `_dynamicColorText` element.
     * Randomly toggles the visibility of the title or value text for one of the seekbars.
     */
    void _onDynamicColorTextClicked();

    /**
     * @brief Applies the layout and positioning for the confirmation dialog elements.
     * This method dynamically calculates positions and sizes based on current screen dimensions.
     * It also sets the correct confirmation question text based on orientation.
     */
    void _applyConfirmationDialogLayout();

    /**
     * @brief Callback handler for the "Rotate" button press.
     * Changes the preferred orientation of the current layer and switches to the appropriate layout.
     */
    void _onRotateButtonPressed();

    // New Private Methods for Layout and Initialization:
    /**
     * @brief Adds all MainUI elements to a specified UILayer.
     * This helper method ensures that elements are correctly registered with the layer.
     * @param targetLayer A pointer to the `UILayer` to which elements should be added.
     */
    void _addMainUIElementsToLayer(UILayer* targetLayer);

    /**
     * @brief Applies the layout and positioning of all UI elements for landscape orientation.
     * This method uses `_gridVisualization` to determine element positions.
     */
    void _applyLandscapeLayout();

    /**
     * @brief Applies the layout and positioning of all UI elements for portrait orientation.
     * This method uses `_gridVisualization` to determine element positions.
     */
    void _applyPortraitLayout();

    /**
     * @brief Retranslates all language-dependent UI strings.
     * This method is called when the active language changes, updating text on labels,
     * buttons, and toggle switches.
     */
    void _retranslateUI();


public:
    /**
     * @brief Constructs a new MainUI object.
     *
     * Initializes all UI elements and their dependencies.
     * The actual layout and callbacks are set up in the `init()` method.
     *
     * @param lcd A pointer to the LGFX display object.
     * @param screenManager A pointer to the `ScreenManager` instance.
     * @param powerManager A pointer to the `PowerManager` instance.
     * @param languageManager A pointer to the `LanguageManager` instance.
     */
    MainUI(LGFX* lcd, ScreenManager* screenManager, PowerManager* powerManager, LanguageManager* languageManager, AudioManager* audioManager);

    /**
     * @brief Sets the pointer to the `MessageBoardElement`.
     * This allows MainUI to display messages using the message board.
     * @param mb A pointer to the `MessageBoardElement` instance.
     */
    void setMessageBoard(MessageBoardElement* mb);

    /**
     * @brief Initializes the MainUI, setting up UI elements, callbacks, and layers.
     * This method defines UI layers, adds elements to them, and configures initial states.
     */
    void init();

    /**
     * @brief Callback invoked when a MainUI layer is shown by the `ScreenManager`.
     * This method applies the correct layout based on the layer's name (landscape or portrait).
     * @param layerName The name of the layer that has just been shown.
     */
    void onShowLayer(const std::string& layerName);

    /**
     * @brief Displays a confirmation dialog to the user to add an RFID UID to the list.
     * This is a public wrapper for the internal _showRfidConfirmationDialog.
     * @param cardData The RFIDCardData object containing the UID to be confirmed.
     */
    void showRfidConfirmationDialog(const RFIDCardData& cardData) {
        _showRfidConfirmationDialog(cardData);
    }

    /**
     * @brief Main loop method for MainUI.
     * This method is called repeatedly to perform periodic tasks, such as updating status information.
     */
    void loop();
};

#endif // MAINUI_H