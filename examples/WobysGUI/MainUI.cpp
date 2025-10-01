/**
 * @file MainUI.cpp
 * @brief Implementation of the MainUI class.
 *
 * This file contains the detailed implementation of the primary user interface
 * for the application, including layout management, event handling for various
 * UI elements, and interaction with system managers.
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

#include "MainUI.h"
#include <Arduino.h>   // For millis(), random(), randomSeed()
#include <algorithm>   // For std::min, std::max
#include <vector>      // For std::vector
#include <string>      // For std::string, std::to_string
#include <functional>  // For std::function

// Layout constants (similar to WifiUI or BLEUI, moved into MainUI for encapsulation)
static const int BUTTON_HEIGHT_STANDARD = 30;     // Standard button height

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
MainUI::MainUI(LGFX* lcd,
               ScreenManager* screenManager,
               PowerManager* powerManager,
               LanguageManager* languageManager,
               AudioManager* audioManager)
  : _lcd(lcd),
    _screenManager(screenManager),
    _powerManager(powerManager),
    _languageManager(languageManager),
    _audioManager(audioManager),
    // Seekbar array initialization (NOW 3 OBJECTS)
    _rgbSeekbars{ SeekbarUI(lcd, 0, 0, 20, 100, 0.0f, 0.0f, 255.0f, UI_COLOR_BACKGROUND_DARK), // Size and range are temporary
                  SeekbarUI(lcd, 0, 0, 20, 100, 0.0f, 0.0f, 255.0f, UI_COLOR_BACKGROUND_DARK),
                  SeekbarUI(lcd, 0, 0, 20, 100, 0.0f, 0.0f, 255.0f, UI_COLOR_BACKGROUND_DARK) },
    _rotateOrientationButton(lcd, "Rotate", 0, 0, 110, BUTTON_HEIGHT_STANDARD, &helvB12),
    // Demo UI elements
    _colorModeToggle(lcd, 0, 0, 135, 40, "Color Mode", false),
    _dynamicColorText(lcd, "Color Text", 0, 0, &helvB18, UI_COLOR_TEXT_DEFAULT, MC_DATUM, TEXTUI_AUTO_SIZE, 40),
    _listControlToggle(lcd, 0, 0, 175, 40, "List Control", true),
    _featureList(lcd, 0, 0, 300, 200, 2),
    _addListItemButton(lcd, "+ Item", 0, 0, 150, BUTTON_HEIGHT_STANDARD, &helvB12),
    _gridVisualizationToggle(lcd, "Grid ON", "Grid OFF", 0, 0, 150, BUTTON_HEIGHT_STANDARD, &helvB12, false),
    _gridVisualization(lcd),
    _keyboard(lcd, "Text Input:"),
    _statusLabel(lcd, "UI Demo", 0, 0, &helvB18, UI_COLOR_TEXT_DEFAULT, TL_DATUM, TEXTUI_AUTO_SIZE, 25),
    _confirmBackground(lcd, "", 0, 0),
    _confirmQuestion(lcd, "", 0, 0),
    _confirmItemName(lcd, "", 0, 0),
    _confirmNoBtn(lcd, "No", 0, 0, 0, 0, &helvB18),
    _confirmYesBtn(lcd, "Yes", 0, 0, 0, 0, &helvB18),
    _itemToForget(""),
    _itemIndexToForget(-1),
    _messageBoardPtr(nullptr),
    _currentConfirmationAction(ConfirmationAction::NONE)
{
    _gridVisualization.setVisible(false);
    _statusLabel.setElementName("MainUI_StatusLabel"); // For debugging purposes
    _dynamicColorText.setElementName("MainUI_DynamicColorText");
    _colorModeToggle.setElementName("MainUI_ColorModeToggle");
    _listControlToggle.setElementName("MainUI_ListControlToggle");
    _featureList.setElementName("MainUI_FeatureList");
    _addListItemButton.setElementName("MainUI_AddListItemButton");
    _gridVisualizationToggle.setElementName("MainUI_GridVisualizationToggle");
    _rotateOrientationButton.setElementName("MainUI_RotateOrientationButton");
    _keyboard.setElementName("MainUI_Keyboard");
    _confirmBackground.setElementName("MainUI_ConfirmBackground");
    _confirmQuestion.setElementName("MainUI_ConfirmQuestion");
    _confirmItemName.setElementName("MainUI_ConfirmItemName");
    _confirmNoBtn.setElementName("MainUI_ConfirmNoBtn");
    _confirmYesBtn.setElementName("MainUI_ConfirmYesBtn");
    for (int i = 0; i < MAINUI_SEEKBAR_COUNT; ++i) {
        _rgbSeekbars[i].setElementName("MainUI_Seekbar" + std::to_string(i));
    }
}

/**
 * @brief Sets the pointer to the `MessageBoardElement`.
 * This allows MainUI to display messages using the message board.
 * @param mb A pointer to the `MessageBoardElement` instance.
 */
void MainUI::setMessageBoard(MessageBoardElement* mb) {
  _messageBoardPtr = mb;
}

/**
 * @brief Initializes the MainUI, setting up UI elements, callbacks, and layers.
 * This method defines UI layers, adds elements to them, and configures initial states.
 */
void MainUI::init() {
  DEBUG_INFO_PRINTLN("MainUI: init() called.");

  if (_languageManager) {
      _languageManager->registerForUpdate("MainUI", [this]() { this->_retranslateUI(); });
  }

  // Create keyboard layer (also usable for demo)
  _screenManager->defineLayer("keyboardLayer_original", UILayer(_lcd, false, true, TFT_BLACK, OrientationPreference::CONTENT_LANDSCAPE));
  if (UILayer* keyboardLayer = _screenManager->getLayer("keyboardLayer_original")) {
    _keyboard.setOnEnterCallback([this](const std::string& t) {
      this->_onKeyboardEnter(t);
    });
    keyboardLayer->addElement(&_keyboard);
    keyboardLayer->setElementName("KeyboardLayer");
  }

  // Create confirmation dialog layer (for list item deletion)
  _screenManager->defineLayer("confirmation_dialog_mainui", UILayer(_lcd, false, false, DIALOG_BOX_BACKGROUND_COLOR, OrientationPreference::ADAPTIVE));
  if (UILayer* d_layer = _screenManager->getLayer("confirmation_dialog_mainui")) {
    // Position and size calculations are now in _applyConfirmationDialogLayout().

    // Add elements to the layer with initialization settings
    _confirmBackground.setBackgroundColor(DIALOG_BOX_BACKGROUND_COLOR);
    _confirmBackground.setBorder(DIALOG_BOX_BORDER_COLOR, 1, TextUI_BorderType::SINGLE);
    _confirmBackground.setText(""); // Empty text, just background
    d_layer->addElement(&_confirmBackground);

    _confirmQuestion.setFont(&helvB18);
    _confirmQuestion.setTextColor(DIALOG_TEXT_COLOR);
    _confirmQuestion.setBackgroundColor(TEXTUI_TRANSPARENT);
    _confirmQuestion.setTextDatum(MC_DATUM);
    _confirmQuestion.setText("Are you sure you want to delete this item?");
    _confirmQuestion.setPadding(2);
    _confirmQuestion.setWordWrap(true);
    d_layer->addElement(&_confirmQuestion);

    _confirmItemName.setFont(&helvR14);
    _confirmItemName.setTextColor(DIALOG_TEXT_COLOR);
    _confirmItemName.setBackgroundColor(TEXTUI_TRANSPARENT);
    _confirmItemName.setTextDatum(MC_DATUM);
    _confirmItemName.setPadding(5);
    d_layer->addElement(&_confirmItemName);

    _confirmNoBtn.setFont(&helvB18);
    _confirmNoBtn.setOnReleaseCallback([this]() { this->_onConfirmDeleteNo(); });
    d_layer->addElement(&_confirmNoBtn);

    _confirmYesBtn.setFont(&helvB18);
    _confirmYesBtn.setOnReleaseCallback([this]() { this->_onConfirmDeleteYes(); });
    d_layer->addElement(&_confirmYesBtn);
    
    d_layer->setElementName("ConfirmationDialogLayer");

    _retranslateUI(); // Retranslate dialog strings
  }

  // Define main application layers and add elements
  UILayer mainLandscapeLayer(_lcd, false, true, UI_COLOR_BACKGROUND_DARK, OrientationPreference::LANDSCAPE_RIGHT);
  mainLandscapeLayer.setElementName("MainLandscapeLayer");
  _addMainUIElementsToLayer(&mainLandscapeLayer); // Add elements to the landscape layer
  _screenManager->defineLayer("main_L_demo", mainLandscapeLayer);

  UILayer mainPortraitLayer(_lcd, false, true, UI_COLOR_BACKGROUND_DARK, OrientationPreference::PORTRAIT_UP);
  mainPortraitLayer.setElementName("MainPortraitLayer");
  _addMainUIElementsToLayer(&mainPortraitLayer); // Add elements to the portrait layer
  _screenManager->defineLayer("main_P_demo", mainPortraitLayer);


  // Initializing the list
  _featureList.clearItems();
  _featureList.addItem(ListItem{ ColumnData("List item 1"), ColumnData("X") });
  _featureList.addItem(ListItem{ ColumnData("List item 2"), ColumnData("X") });
  _featureList.addItem(ListItem{ ColumnData("List item 3"), ColumnData("X") });
  _featureList.setNumColumns(2);
  _featureList.setColumnDefaultFont(0, &helvB12);
  _featureList.setColumnDefaultFont(1, &helvB12);
  _featureList.setColumnDefaultAlignment(0, ML_DATUM);
  _featureList.setColumnDefaultTextColor(1, UI_COLOR_ALERT);


  // SETTING UP CALLBACKS (ONCE, HERE)
  _colorModeToggle.setOnStateChangedCallback([this](bool newState) { this->_onColorModeToggleChanged(newState); });
  // Seekbar callbacks UNIFIED
  for (int i = 0; i < MAINUI_SEEKBAR_COUNT; ++i) {
    _rgbSeekbars[i].setOnValueChangedCallback([this](float value, bool isFinalChange) {
      this->_onRGBSeekbarValueChanged(value, isFinalChange);
    });
  }
  _listControlToggle.setOnStateChangedCallback([this](bool newState) {
    _featureList.setInteractive(newState);
    _featureList.setVisualState(newState ? UIVisualState::ACTIVE : UIVisualState::NON_INTERACTIVE);
  });
  _addListItemButton.setOnReleaseCallback([this]() { this->_onAddListItemPressed(); });
  _gridVisualizationToggle.setOnToggleCallback([this](bool newState) { this->_onToggleGridVisualization(newState); });
  _featureList.setOnItemSelectedCallback([this](int index, const ListItem& data, int16_t touchX) { this->_onListItemSelected(index, data, touchX); });

  _rotateOrientationButton.setOnReleaseCallback([this]() { this->_onRotateButtonPressed(); });

  _dynamicColorText.setOnReleaseCallback([this]() { this->_onDynamicColorTextClicked(); });
  // Optionally set pressed colors
  _dynamicColorText.setPressedColors(UI_COLOR_BACKGROUND_MEDIUM, UI_COLOR_BACKGROUND_DARK);

  // Initial state settings
  _onColorModeToggleChanged(_colorModeToggle.getState()); // Apply initial state
  _listControlToggle.setState(_listControlToggle.getState(), true);
  _gridVisualizationToggle.setState(false, false); // Grid is OFF by default (false, false: don't call callback)
}

/**
 * @brief Adds all MainUI elements to a specified UILayer.
 * This helper method ensures that elements are correctly registered with the layer.
 * @param targetLayer A pointer to the `UILayer` to which elements should be added.
 */
void MainUI::_addMainUIElementsToLayer(UILayer* targetLayer) {
    if (!targetLayer) return;
    DEBUG_INFO_PRINTF("MainUI: Adding elements to layer '%s'.\n", targetLayer->getElementName().c_str());

    // Header elements
    targetLayer->addElement(&_statusLabel);
    targetLayer->addElement(&_dynamicColorText);

    // RGB Seekbars (UNIFIED: only _rgbSeekbars)
    for (int i = 0; i < MAINUI_SEEKBAR_COUNT; ++i) {
        targetLayer->addElement(&_rgbSeekbars[i]);
    }

    // Toggles
    targetLayer->addElement(&_colorModeToggle);
    targetLayer->addElement(&_listControlToggle);

    // List
    targetLayer->addElement(&_featureList);

    // List operation buttons
    targetLayer->addElement(&_addListItemButton);
    targetLayer->addElement(&_gridVisualizationToggle);

    // Orientation toggle button (both added)
    targetLayer->addElement(&_rotateOrientationButton);

    targetLayer->addElement(&_gridVisualization);

    // Seekbars configuration (min/max/format) can also be called here.
    // Configure only the _rgbSeekbars array
    _configureSeekbars(_rgbSeekbars);
}

/**
 * @brief Applies the layout and positioning of all UI elements for landscape orientation.
 * This method uses `_gridVisualization` to determine element positions.
 */
void MainUI::_applyLandscapeLayout() {
  DEBUG_INFO_PRINTLN("MainUI: Applying landscape layout...");

  const int32_t layerW = _lcd->width();
  const int32_t layerH = _lcd->height() - STATUSBAR_HEIGHT; // STATUSBAR_HEIGHT from Config.h

  _gridVisualization.setContainerSize(layerW, layerH);
  _gridVisualization.setGridParameters(12, 8, GRID_DEFAULT_H_SPACING_PIXELS, GRID_DEFAULT_V_SPACING_PIXELS);

  // RGB Seekbars (vertical)
  _rgbSeekbars[0].setOrientation(SeekbarUI::Orientation::Vertical);
  _rgbSeekbars[1].setOrientation(SeekbarUI::Orientation::Vertical);
  _rgbSeekbars[2].setOrientation(SeekbarUI::Orientation::Vertical);

  // Positions and sizes for _rgbSeekbars
  GridCellInfo seekbarBlock = _gridVisualization.getBlockRect(7, 2, 7, 6); // COL 7, ROW 2-6
  _rgbSeekbars[0].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[0].setSize(seekbarBlock.w, seekbarBlock.h);

  seekbarBlock = _gridVisualization.getBlockRect(8, 2, 9, 6); // COL 8-9, ROW 2-6
  _rgbSeekbars[1].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[1].setSize(seekbarBlock.w, seekbarBlock.h);

  seekbarBlock = _gridVisualization.getBlockRect(10, 2, 10, 6); // COL 10, ROW 2-6
  _rgbSeekbars[2].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[2].setSize(seekbarBlock.w, seekbarBlock.h);


  const int Y_OFFSET_FROM_TOP = UI_DEFAULT_PADDING_PIXELS; // Using a configurable default

  GridCellInfo statusLabelBlock = _gridVisualization.getBlockRect(0, 0, 5, 0); // COL 0-5, ROW 0
  _statusLabel.setPosition(statusLabelBlock.x, statusLabelBlock.y + Y_OFFSET_FROM_TOP);
  _statusLabel.setSize(statusLabelBlock.w, statusLabelBlock.h);
  _statusLabel.setTextDatum(TL_DATUM);


  GridCellInfo dynamicColorTextBlock = _gridVisualization.getBlockRect(6, 0, 11, 0); // COL 6-11, ROW 0
  _dynamicColorText.setPosition(dynamicColorTextBlock.x, dynamicColorTextBlock.y + Y_OFFSET_FROM_TOP);
  _dynamicColorText.setSize(dynamicColorTextBlock.w, dynamicColorTextBlock.h);
  _dynamicColorText.setFont(&helvB18); // Font is OS and configurable
  _dynamicColorText.setTextDatum(TC_DATUM);


  GridCellInfo listControlToggleBlock = _gridVisualization.getBlockRect(0, 1, 5, 1); // COL 0-5, ROW 1
  _listControlToggle.setPosition(listControlToggleBlock.x, listControlToggleBlock.y + UI_DEFAULT_PADDING_PIXELS); // Using configurable padding
  _listControlToggle.setSize(listControlToggleBlock.w, listControlToggleBlock.h);
  _listControlToggle.setAlignment(MC_DATUM);
  _listControlToggle.setTitleFont(&helvB12);


  GridCellInfo colorModeToggleBlock = _gridVisualization.getBlockRect(6, 1, 11, 1); // COL 6-11, ROW 1
  _colorModeToggle.setPosition(colorModeToggleBlock.x, colorModeToggleBlock.y + UI_DEFAULT_PADDING_PIXELS); // Using configurable padding
  _colorModeToggle.setSize(colorModeToggleBlock.w, colorModeToggleBlock.h);
  _colorModeToggle.setAlignment(MC_DATUM);
  _colorModeToggle.setTitleFont(&helvB12);


  GridCellInfo featureListBlock = _gridVisualization.getBlockRect(0, 2, 5, 5); // COL 0-5, ROW 2-5
  _featureList.setPosition(featureListBlock.x, featureListBlock.y);
  _featureList.setSize(featureListBlock.w, featureListBlock.h);

  // List column widths
  const uint16_t delete_col_width_list = 35; // This is an app-specific choice, can be OS configurable
  // Ensure that featureList.getDrawScrollBar() and featureList.getDrawBorder() are correct and not always true
  uint16_t name_col_width_list = featureListBlock.w - (_featureList.getDrawScrollBar() ? LISTUI_SCROLL_BAR_WIDTH_PIXELS : 0) - delete_col_width_list - (_featureList.getDrawBorder() ? (2 * TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS) : 0); // Using configurable border/scrollbar
  if (name_col_width_list < LISTUI_MIN_COL_WIDTH_PIXELS) name_col_width_list = LISTUI_MIN_COL_WIDTH_PIXELS; // Using configurable min width

  _featureList.setColumnWidth(0, name_col_width_list);
  _featureList.setColumnWidth(1, delete_col_width_list);
  _featureList.setColumnDefaultAlignment(1, MC_DATUM);


  GridCellInfo addListItemButtonBlock = _gridVisualization.getBlockRect(0, 6, 2, 6); // COL 0-2, ROW 6
  _addListItemButton.setPosition(addListItemButtonBlock.x, addListItemButtonBlock.y);
  _addListItemButton.setSize(addListItemButtonBlock.w, addListItemButtonBlock.h);

  GridCellInfo gridToggleBlock = _gridVisualization.getBlockRect(3, 6, 5, 6); // COL 3-5, ROW 6
  _gridVisualizationToggle.setPosition(gridToggleBlock.x, gridToggleBlock.y);
  _gridVisualizationToggle.setSize(gridToggleBlock.w, gridToggleBlock.h);


  GridCellInfo ButtonBlock = _gridVisualization.getBlockRect(4, 7, 7, 7); // COL 4-7, ROW 7
  _rotateOrientationButton.setPosition(ButtonBlock.x, ButtonBlock.y);
  _rotateOrientationButton.setSize(ButtonBlock.w, ButtonBlock.h);

  this->_applyConfirmationDialogLayout();
}

/**
 * @brief Applies the layout and positioning of all UI elements for portrait orientation.
 * This method uses `_gridVisualization` to determine element positions.
 */
void MainUI::_applyPortraitLayout() {
  DEBUG_INFO_PRINTLN("MainUI: Applying portrait layout...");

  const int32_t layerW = _lcd->width();
  const int32_t layerH = _lcd->height() - STATUSBAR_HEIGHT;

  _gridVisualization.setContainerSize(layerW, layerH);
  _gridVisualization.setGridParameters(8, 12, GRID_DEFAULT_H_SPACING_PIXELS, GRID_DEFAULT_V_SPACING_PIXELS);

  // RGB Seekbars (horizontal)
  _rgbSeekbars[0].setOrientation(SeekbarUI::Orientation::Horizontal);
  _rgbSeekbars[1].setOrientation(SeekbarUI::Orientation::Horizontal);
  _rgbSeekbars[2].setOrientation(SeekbarUI::Orientation::Horizontal);

  // Positions and sizes for _rgbSeekbars
  GridCellInfo seekbarBlock = _gridVisualization.getBlockRect(0, 1, 7, 2); // COL 0-7, ROW 1-2
  _rgbSeekbars[0].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[0].setSize(seekbarBlock.w, seekbarBlock.h);

  seekbarBlock = _gridVisualization.getBlockRect(0, 2, 7, 3); // COL 0-7, ROW 2-3
  _rgbSeekbars[1].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[1].setSize(seekbarBlock.w, seekbarBlock.h);

  seekbarBlock = _gridVisualization.getBlockRect(0, 3, 7, 4); // COL 0-7, ROW 3-4
  _rgbSeekbars[2].setPosition(seekbarBlock.x, seekbarBlock.y);
  _rgbSeekbars[2].setSize(seekbarBlock.w, seekbarBlock.h);


  const int Y_OFFSET_FROM_TOP = UI_DEFAULT_PADDING_PIXELS; // Using a configurable default

  GridCellInfo statusLabelBlock = _gridVisualization.getBlockRect(0, 0, 3, 0); // COL 0-3, ROW 0
  _statusLabel.setPosition(statusLabelBlock.x, statusLabelBlock.centerY);
  _statusLabel.setSize(statusLabelBlock.w, statusLabelBlock.h);
  _statusLabel.setTextDatum(TL_DATUM);

  GridCellInfo dynamicColorTextBlock = _gridVisualization.getBlockRect(4, 0, 7, 0); // COL 4-7, ROW 0
  _dynamicColorText.setPosition(dynamicColorTextBlock.x, dynamicColorTextBlock.centerY);
  _dynamicColorText.setSize(dynamicColorTextBlock.w, dynamicColorTextBlock.h);
  _dynamicColorText.setFont(&helvB18);
  _dynamicColorText.setTextDatum(TC_DATUM);


  GridCellInfo colorModeToggleBlock = _gridVisualization.getBlockRect(0, 4, 7, 5); // COL 0-7, ROW 4-5
  _colorModeToggle.setPosition(colorModeToggleBlock.x, colorModeToggleBlock.y);
  _colorModeToggle.setSize(colorModeToggleBlock.w, colorModeToggleBlock.h);
  _colorModeToggle.setAlignment(MC_DATUM);

  GridCellInfo listControlToggleBlock = _gridVisualization.getBlockRect(0, 5, 7, 6); // COL 0-7, ROW 5-6
  _listControlToggle.setPosition(listControlToggleBlock.x, listControlToggleBlock.y);
  _listControlToggle.setSize(listControlToggleBlock.w, listControlToggleBlock.h);
  _listControlToggle.setAlignment(MC_DATUM);
  _listControlToggle.setTitleFont(&helvB12);


  GridCellInfo featureListBlock = _gridVisualization.getBlockRect(0, 7, 7, 9); // COL 0-7, ROW 7-9
  _featureList.setPosition(featureListBlock.x, featureListBlock.y);
  _featureList.setSize(featureListBlock.w, featureListBlock.h);

  // List column widths
  const uint16_t delete_col_width_list = 35; // App-specific choice
  // Ensure that featureList.getDrawScrollBar() and featureList.getDrawBorder() are correct and not always true
  uint16_t name_col_width_list =
      featureListBlock.w -
      (_featureList.getDrawScrollBar() ? LISTUI_SCROLL_BAR_WIDTH_PIXELS : 0) - // Using configurable scrollbar width
      delete_col_width_list - (_featureList.getDrawBorder() ? (2 * TEXTUI_DEFAULT_BORDER_THICKNESS_PIXELS) : 0); // Using configurable border
  if (name_col_width_list < LISTUI_MIN_COL_WIDTH_PIXELS) name_col_width_list = LISTUI_MIN_COL_WIDTH_PIXELS; // Using configurable min width

  _featureList.setColumnWidth(0, name_col_width_list);
  _featureList.setColumnWidth(1, delete_col_width_list);
  _featureList.setColumnDefaultAlignment(1, MC_DATUM);


  GridCellInfo addListItemButtonBlock = _gridVisualization.getBlockRect(0, 10, 3, 10); // COL 0-3, ROW 10
  _addListItemButton.setPosition(addListItemButtonBlock.x, addListItemButtonBlock.y);
  _addListItemButton.setSize(addListItemButtonBlock.w, addListItemButtonBlock.h);

  GridCellInfo gridToggleBlock = _gridVisualization.getBlockRect(4, 10, 7, 10); // COL 4-7, ROW 10
  _gridVisualizationToggle.setPosition(gridToggleBlock.x, gridToggleBlock.y);
  _gridVisualizationToggle.setSize(gridToggleBlock.w, gridToggleBlock.h);


  GridCellInfo ButtonBlock = _gridVisualization.getBlockRect(2, 11, 5, 11); // COL 2-5, ROW 11
  _rotateOrientationButton.setPosition(ButtonBlock.x, ButtonBlock.y);
  _rotateOrientationButton.setSize(ButtonBlock.w, ButtonBlock.h);

  this->_applyConfirmationDialogLayout();
}

/**
 * @brief Callback invoked when a MainUI layer is shown by the `ScreenManager`.
 * This method applies the correct layout based on the layer's name (landscape or portrait).
 * @param layerName The name of the layer that has just been shown.
 */
void MainUI::onShowLayer(const std::string& layerName) {
  DEBUG_INFO_PRINTF("MainUI: onShowLayer() called - Layer name: '%s'\n", layerName.c_str());

  const int32_t currentDisplayWidth = _lcd->width();
  const int32_t currentDisplayHeight = _lcd->height() - STATUSBAR_HEIGHT; // This is considered by GridLayout

  _gridVisualization.setContainerSize(currentDisplayWidth, currentDisplayHeight);

  if (layerName == "main_L_demo") {
      _applyLandscapeLayout();
  } else if (layerName == "main_P_demo") {
      _applyPortraitLayout();
  }

  if (_screenManager) {
    _screenManager->redraw(); // Request a redraw from the ScreenManager to update the display
  }
}

/**
 * @brief Main loop method for MainUI.
 * This method is called repeatedly to perform periodic tasks, such as updating status information.
 */
void MainUI::loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - _lastStatusUpdateTimeMillis >= DEVICE_STATUS_UPDATE_INTERVAL_MS) {
    _lastStatusUpdateTimeMillis = currentMillis;
    // Here we can update _statusLabel if necessary (e.g., time, memory, battery)
    // _updateStatusLabel(); // Call if implemented
  }
}

/**
 * @brief Configures the properties of an array of SeekbarUI elements.
 * Sets titles, value ranges, limits, and initial values for the seekbars.
 * @param seekbarsToConfigure A pointer to the first element of the SeekbarUI array.
 */
void MainUI::_configureSeekbars(SeekbarUI* seekbarsToConfigure) {
  // Seekbars configuration (NOW UNIFIED)
  seekbarsToConfigure[0].setTitle("R", &helvB12);
  seekbarsToConfigure[0].setRange(0, 255);
  seekbarsToConfigure[0].setLimits(0, 254);
  seekbarsToConfigure[0].setValueFormat(3, 0); // 3 integer digits, 0 decimal
  seekbarsToConfigure[0].setCurrentValue(255, false);

  seekbarsToConfigure[1].setTitle("G", &helvB12);
  seekbarsToConfigure[1].setRange(0, 255);
  seekbarsToConfigure[1].setLimits(0, 254);
  seekbarsToConfigure[1].setValueFormat(3, 0);
  seekbarsToConfigure[1].setCurrentValue(255, false);

  seekbarsToConfigure[2].setTitle("B", &helvB12);
  seekbarsToConfigure[2].setRange(0, 255);
  seekbarsToConfigure[2].setLimits(0, 254);
  seekbarsToConfigure[2].setValueFormat(3, 0);
  seekbarsToConfigure[2].setCurrentValue(128, false);
}

/**
 * @brief Callback handler for the "Rotate" button press.
 * Changes the preferred orientation of the current layer and switches to the appropriate layout.
 */
void MainUI::_onRotateButtonPressed() {
    DEBUG_INFO_PRINTLN("MainUI: _onRotateButtonPressed() - Orientation change button pressed.");

    // Play click sound
    //if (_audioManager) { // Null pointer check
    //    _audioManager->playClickSound();
    //} else {
    //    DEBUG_WARN_PRINTLN("MainUI: AudioManager is nullptr. Cannot play click sound.");
    //}

    UILayer* currentLayer = _screenManager->getTopLayer();
    if (!currentLayer) {
        DEBUG_WARN_PRINTLN("MainUI: _onRotateButtonPressed - No top layer found in ScreenManager.");
        return;
    }

    OrientationPreference currentPreferredOrientation = currentLayer->getPreferredOrientation();
    OrientationPreference nextPreferredOrientation;
    std::string targetLayerName;

    // Cycle through the 4 orientation states:
    if (currentPreferredOrientation == OrientationPreference::LANDSCAPE_LEFT) {
        nextPreferredOrientation = OrientationPreference::PORTRAIT_UP;
        targetLayerName = "main_P_demo"; // Switch to portrait aspect layer
    } else if (currentPreferredOrientation == OrientationPreference::PORTRAIT_UP) {
        nextPreferredOrientation = OrientationPreference::LANDSCAPE_RIGHT;
        targetLayerName = "main_L_demo"; // Switch to landscape aspect layer
    } else if (currentPreferredOrientation == OrientationPreference::LANDSCAPE_RIGHT) {
        nextPreferredOrientation = OrientationPreference::PORTRAIT_DOWN;
        targetLayerName = "main_P_demo"; // Switch to portrait aspect layer
    } else if (currentPreferredOrientation == OrientationPreference::PORTRAIT_DOWN) {
        nextPreferredOrientation = OrientationPreference::LANDSCAPE_LEFT;
        targetLayerName = "main_L_demo"; // Switch to landscape aspect layer
    } else {
        nextPreferredOrientation = OrientationPreference::LANDSCAPE_LEFT;
        targetLayerName = "main_L_demo";
        DEBUG_WARN_PRINTF("MainUI: _onRotateButtonPressed - Unknown/ADAPTIVE orientation (%d), defaulting to LANDSCAPE_LEFT.\n", static_cast<int>(currentPreferredOrientation));
    }

    DEBUG_INFO_PRINTF("MainUI: _onRotateButtonPressed - Current: %d, Next: %d, Switching to layer: '%s'\n",
                 static_cast<int>(currentPreferredOrientation),
                 static_cast<int>(nextPreferredOrientation),
                 targetLayerName.c_str());

    // IMPORTANT MODIFICATION: Set the target layer's preference in the ScreenManager
    // Retrieve the layer object from ScreenManager's internal map and set its preference.
    UILayer* targetLayerInScreenManager = _screenManager->getLayer(targetLayerName);
    if (targetLayerInScreenManager) {
        targetLayerInScreenManager->setPreferredOrientation(nextPreferredOrientation);
        DEBUG_INFO_PRINTF("MainUI: _onRotateButtonPressed - Layer '%s' preference set to: %d\n", targetLayerName.c_str(), static_cast<int>(nextPreferredOrientation));
    } else {
        DEBUG_ERROR_PRINTLN("MainUI: _onRotateButtonPressed - ERROR: Target layer not found in ScreenManager to set preference!");
    }

    // Switch via ScreenManager
    _screenManager->switchToLayer(targetLayerName);
    this->onShowLayer(targetLayerName); // Manually call onShowLayer to apply layout
}

/**
 * @brief Callback handler for the virtual keyboard's Enter event.
 * Processes the text entered via the keyboard, adds it to the list, or handles cancellation.
 * @param text The string entered by the user, or a special string if cancelled.
 */
void MainUI::_onKeyboardEnter(const std::string& text) {
  if (text == KEYBOARD_ESCAPE_BUTTON_ACTION_STRING) {
    _screenManager->popLayer(); // Close keyboard layer
    if (g_displayLocalizedMessage) {
        g_displayLocalizedMessage("STATUS_KEYBOARD_INPUT_CANCELLED", 3000, false);
    }
    DEBUG_INFO_PRINTLN("MainUI: Keyboard input cancelled (Escape).");
    return;
  }
  if (!text.empty()) {
    if (_messageBoardPtr) {
      _messageBoardPtr->setText(_languageManager->getString("STATUS_ITEM_ADDED", "Item added: ") + text, 3000);
    }
    _featureList.addItem(ListItem{ ColumnData(text), ColumnData("X") });
    _featureList.sortItemsAlphabetically(true); // Call sort function for ascending order
    // Scroll to the new item, ensuring a valid index
    if (!_featureList.getItems().empty()) {
        _featureList.scrollToItem(_featureList.getItems().size() - 1);
    }

    // If the list was non-interactive, enable it and set to active visual state
    if (!_featureList.isInteractive() || _featureList.getVisualState() == UIVisualState::NON_INTERACTIVE) {
      _featureList.setInteractive(true);
      _featureList.setVisualState(UIVisualState::ACTIVE);
    }
    // If list control was OFF, switch it ON (triggers listcontroltoggle callback)
    if (!_listControlToggle.getState()) {
      _listControlToggle.setState(true, true);
    }

  } else {
    if (g_displayLocalizedMessage) {
      g_displayLocalizedMessage("STATUS_EMPTY_ITEM_NOT_ADDED", 3000, true); // Assuming this is an error/warning
    }
  }
  _screenManager->popLayer(); // Close keyboard layer
}

/**
 * @brief Callback handler for the "Add List Item" button press.
 * Triggers the virtual keyboard to allow adding a new item to the list.
 */
void MainUI::_onAddListItemPressed() {
  _keyboard.setTitle(_languageManager->getString("MAIN_KEYBOARD_ADD_ITEM_TITLE", "New list item:"));
  _keyboard.clearText();
  _screenManager->pushLayer("keyboardLayer_original");
  DEBUG_INFO_PRINTLN("MainUI: 'Add List Item' button pressed. Opening keyboard.");
}

/**
 * @brief Callback handler for the "Yes" button in the confirmation dialog.
 * Performs the appropriate action (delete list item or add RFID) based on _currentConfirmationAction.
 */
void MainUI::_onConfirmDeleteYes() {
    _screenManager->popLayer(); // Close confirmation dialog

    switch (_currentConfirmationAction) {
        case ConfirmationAction::DELETE_LIST_ITEM: {
            if (_itemIndexToForget != -1) {
                _featureList.removeItem(_itemIndexToForget);
                if (g_displayLocalizedMessage) {
                    std::string full_msg = _languageManager->getString("STATUS_ITEM_DELETED", "Item deleted: ") + _itemToForget;
                    g_displayLocalizedMessage(full_msg, 3000, false);
                }
                _itemIndexToForget = -1; // Reset
                _itemToForget = "";      // Reset
                DEBUG_INFO_PRINTF("MainUI: Item deleted: '%s'\n", _itemToForget.c_str());
            } else {
                // This path is not explicitly used by the provided code for whole list deletion,
                // but kept for completeness based on context.
                // _featureList.clearItems(); // Original comment and code
                if (g_displayLocalizedMessage) {
                    g_displayLocalizedMessage("TEXT_LIST_CLEARED", 3000, false);
                }
                DEBUG_INFO_PRINTLN("MainUI: Entire list cleared.");
            }

            // Ensure list is interactive and active if it wasn't
            if (!_featureList.isInteractive() || _featureList.getVisualState() == UIVisualState::NON_INTERACTIVE) {
                _featureList.setInteractive(true);
                _featureList.setVisualState(UIVisualState::ACTIVE);
            }
            if (!_listControlToggle.getState()) {
                _listControlToggle.setState(true, true);
            }
            break;
        }
        case ConfirmationAction::ADD_RFID: {
            if (!_pendingRfidCardData.uid_string.empty()) {
                _featureList.addItem(_pendingRfidCardData.asListItem()); // Add using the helper method
                _featureList.sortItemsAlphabetically(true); // Sort after adding
                
                if (g_displayLocalizedMessage) {
                    std::string full_msg = _languageManager->getString("STATUS_RFID_ADDED", "RFID added: ") + _pendingRfidCardData.uid_string;
                    g_displayLocalizedMessage(full_msg, 3000, false);
                }
                DEBUG_INFO_PRINTF("MainUI: RFID added: '%s'\n", _pendingRfidCardData.uid_string.c_str());
                // After adding, ensure the list interactivity/state is appropriate
                if (!_featureList.isInteractive() || _featureList.getVisualState() == UIVisualState::NON_INTERACTIVE) {
                    _featureList.setInteractive(true);
                    _featureList.setVisualState(UIVisualState::ACTIVE);
                }
                if (!_listControlToggle.getState()) {
                    _listControlToggle.setState(true, true);
                }
            }
            _pendingRfidCardData = RFIDCardData(); // Clear pending data
            break;
        }
        case ConfirmationAction::NONE: // Should not happen if dialog is shown
        default:
            DEBUG_WARN_PRINTLN("MainUI: _onConfirmDeleteYes called with NONE action or unknown action.");
            break;
    }
    _currentConfirmationAction = ConfirmationAction::NONE; // Reset action state
}

/**
 * @brief Applies the layout and positioning for the confirmation dialog elements.
 * This method dynamically calculates positions and sizes based on current screen dimensions.
 * It also sets the correct confirmation question text based on orientation and current action.
 */
void MainUI::_applyConfirmationDialogLayout() {
    // Get current screen dimensions (dynamic for orientation changes)
    int currentDisplayW = _lcd->width();
    int currentDisplayH = _lcd->height() - STATUSBAR_HEIGHT; // Available content height

    // Calculate dialog panel dimensions and position dynamically
    // Width: max 85% of available width, but max 380px, min 200px
    int dialogWidth = (std::min)((int)(currentDisplayW * 0.85), 380);
    dialogWidth = (std::max)(dialogWidth, 200);

    // Height: max 60% of available height, but max 190px, min 160px
    int dialogHeight = (std::min)((int)(currentDisplayH * 0.60), 190);
    dialogHeight = (std::max)(dialogHeight, 160);

    // Center the dialog panel
    int dialogPanelX = (currentDisplayW - dialogWidth) / 2;
    int dialogPanelY = (currentDisplayH - dialogHeight) / 2;
    int p = UI_DEFAULT_MARGIN_PIXELS; // Internal margin, using configurable default

    // Set background panel position and size
    _confirmBackground.setPosition(dialogPanelX, dialogPanelY);
    _confirmBackground.setSize(dialogWidth, dialogHeight);

    // Calculate heights for text and button sections
    const int DIALOG_BUTTON_HEIGHT = 40; // Using configurable default
    int buttonSectionHeight = DIALOG_BUTTON_HEIGHT + p; // Space for buttons + padding below them
    int totalTextSectionHeight = dialogHeight - (2 * p) - buttonSectionHeight;

    // Ensure text sections have minimum height
    if (totalTextSectionHeight < 20) totalTextSectionHeight = 20;

    int questionTextHeight = (int)(totalTextSectionHeight * 0.45); // Question text height (45%)
    int itemNameTextHeight = (int)(totalTextSectionHeight * 0.55); // Item name text height (55%)

    // Ensure individual text elements also have minimum height
    if (questionTextHeight < 10) questionTextHeight = 10;
    if (itemNameTextHeight < 10) itemNameTextHeight = 10;


    int currentTextY = dialogPanelY + p; // Y position for the first text element

    // Position and size for Question TextUI
    _confirmQuestion.setPosition(dialogPanelX + p, currentTextY);
    _confirmQuestion.setSize(dialogWidth - (2 * p), questionTextHeight);

    currentTextY += questionTextHeight;

    // Position and size for Item Name TextUI
    _confirmItemName.setPosition(dialogPanelX + p, currentTextY);
    _confirmItemName.setSize(dialogWidth - (2 * p), itemNameTextHeight);

    // Get current active layer to check orientation
    // The top layer could be the confirmation dialog itself if it's already shown and re-applying layout.
    UILayer* currentActiveLayer = _screenManager->getTopLayer();
    OrientationPreference layerOrientation = (currentActiveLayer) ? currentActiveLayer->getPreferredOrientation() : OrientationPreference::ADAPTIVE;

    // --- Dynamically set question text based on action and orientation ---
    if (_currentConfirmationAction == ConfirmationAction::ADD_RFID) {
        if (layerOrientation == OrientationPreference::PORTRAIT_UP || layerOrientation == OrientationPreference::PORTRAIT_DOWN) {
            _confirmQuestion.setText(_languageManager->getString("MAIN_CONFIRM_ADD_RFID_QUESTION_PORTRAIT", "Add this RFID\nto list?"));
        } else {
            _confirmQuestion.setText(_languageManager->getString("MAIN_CONFIRM_ADD_RFID_QUESTION", "Do you want to add this RFID to the list?"));
        }
    } else { // Default to DELETE_LIST_ITEM or NONE
        if (layerOrientation == OrientationPreference::PORTRAIT_UP || layerOrientation == OrientationPreference::PORTRAIT_DOWN) {
            _confirmQuestion.setText(_languageManager->getString("MAIN_CONFIRM_DELETE_QUESTION_PORTRAIT", "Confirm deletion\nof this item?"));
        } else { // LANDSCAPE or ADAPTIVE
            _confirmQuestion.setText(_languageManager->getString("MAIN_CONFIRM_DELETE_QUESTION", "Are you sure you want to delete this item?"));
        }
    }


    // Buttons position and size
    int buttonWidth = (dialogWidth - (3 * p)) / 2;

    if (buttonWidth < 60) buttonWidth = 60;

    int buttonsY = dialogPanelY + dialogHeight - p - DIALOG_BUTTON_HEIGHT;
    int noButtonX = dialogPanelX + p;
    int yesButtonX = dialogPanelX + p + buttonWidth + p;

    _confirmNoBtn.setPosition(noButtonX, buttonsY);
    _confirmNoBtn.setSize(buttonWidth, DIALOG_BUTTON_HEIGHT);
    _confirmYesBtn.setPosition(yesButtonX, buttonsY);
    _confirmYesBtn.setSize(buttonWidth, DIALOG_BUTTON_HEIGHT);

    // Request redraw for all dialog elements as their position/size has changed
    _confirmBackground.requestRedraw();
    _confirmQuestion.requestRedraw();
    _confirmItemName.requestRedraw();
    _confirmNoBtn.requestRedraw();
    _confirmYesBtn.requestRedraw();
    DEBUG_INFO_PRINTLN("MainUI: Confirmation dialog layout applied.");
}

/**
 * @brief Callback handler for the "No" button in the confirmation dialog.
 * Cancels the appropriate action.
 */
void MainUI::_onConfirmDeleteNo() {
    _screenManager->popLayer(); // Close confirmation dialog

    switch (_currentConfirmationAction) {
        case ConfirmationAction::DELETE_LIST_ITEM: {
            if (g_displayLocalizedMessage) {
                g_displayLocalizedMessage("STATUS_DELETION_CANCELLED", 3000, false);
            }
            DEBUG_INFO_PRINTLN("MainUI: Confirmation dialog 'No' pressed. Deletion cancelled.");
            break;
        }
        case ConfirmationAction::ADD_RFID: {
            if (g_displayLocalizedMessage) {
                g_displayLocalizedMessage("STATUS_RFID_ADD_CANCELLED", 3000, false);
            }
            DEBUG_INFO_PRINTLN("MainUI: RFID add cancelled by user.");
            break;
        }
        case ConfirmationAction::NONE: // Should not happen
        default:
            DEBUG_WARN_PRINTLN("MainUI: _onConfirmDeleteNo called with NONE action or unknown action.");
            break;
    }
    _itemIndexToForget = -1; // Reset
    _itemToForget = "";      // Reset
    _pendingRfidCardData = RFIDCardData(); // Clear pending data
    _currentConfirmationAction = ConfirmationAction::NONE; // Reset action state
}

/**
 * @brief Callback handler for an item selection in the feature list.
 * Handles actions based on which column was clicked (e.g., deleting an item).
 * @param index The index of the selected item.
 * @param data The `ListItem` data of the selected item.
 * @param touchX The X coordinate of the touch within the list item.
 */
void MainUI::_onListItemSelected(int index, const ListItem& data, int16_t touchX) {
  if (data.columns.empty()) return;

  int clickedColumnIndex = _featureList.getClickedColumnIndex(touchX);
  const int DELETE_COLUMN_INDEX = 1;

  if (clickedColumnIndex == DELETE_COLUMN_INDEX) {
    if (!data.columns[DELETE_COLUMN_INDEX].text.empty()) {
      // Prevent opening dialog if it's already active (e.g., for RFID)
      if (_screenManager->getTopLayerName() == "confirmation_dialog_mainui") {
          DEBUG_INFO_PRINTLN("MainUI: Confirmation dialog already active, ignoring new delete request.");
          return;
      }
      _itemIndexToForget = index;
      _itemToForget = data.columns[0].text;
      _currentConfirmationAction = ConfirmationAction::DELETE_LIST_ITEM;

      this->_applyConfirmationDialogLayout(); // Reapply layout for current orientation

      // Truncate long text for display in confirmation dialog
      std::string finalDisplayString = _itemToForget;
      const lgfx::IFont* itemNameFont = _confirmItemName.getFont();
      if (itemNameFont) {
        _lcd->setFont(itemNameFont); // Temporarily set font for measurement
        int availableWidth = _confirmItemName.getWidth() - 2 * _confirmItemName.getPadding();
        if (_lcd->textWidth(_itemToForget.c_str()) > availableWidth) {
          std::string ellipsis = "...";
          int ellipsisWidth = _lcd->textWidth(ellipsis.c_str());
          std::string shortenedText = "";
          for (int i = 0; i < _itemToForget.length(); ++i) {
            std::string currentSub = _itemToForget.substr(0, i + 1);
            if (_lcd->textWidth(currentSub.c_str()) <= (availableWidth - ellipsisWidth)) {
              shortenedText = currentSub;
            } else {
              break;
            }
          }
          finalDisplayString = shortenedText + ellipsis;
        }
      }
      _confirmItemName.setText(finalDisplayString);

      _screenManager->pushLayer("confirmation_dialog_mainui");
      DEBUG_INFO_PRINTF("MainUI: List item '%s' selected for deletion. Opening confirmation dialog.\n", _itemToForget.c_str());
    }
  } else {
    if (_messageBoardPtr) {
      _messageBoardPtr->setText(_languageManager->getString("STATUS_ITEM_SELECTED", "Selected item: ") + data.columns[0].text, 3000); // New string key
    }
    DEBUG_INFO_PRINTF("MainUI: List item '%s' selected.\n", data.columns[0].text.c_str());
  }
}

/**
 * @brief Callback handler for the Color Mode toggle switch state change.
 * Enables or disables the interactivity and visual state of the RGB seekbars
 * and updates the dynamic color text accordingly.
 * @param newState The new state of the toggle switch (`true` for ON, `false` for OFF).
 */
void MainUI::_onColorModeToggleChanged(bool newState) {
  if (newState) {
    for (int i = 0; i < MAINUI_SEEKBAR_COUNT; ++i) {
      _rgbSeekbars[i].setInteractive(true);
      _rgbSeekbars[i].setVisualState(UIVisualState::ACTIVE);
    }
    // Re-trigger seekbar value change to update text color, but don't force a final change
    this->_onRGBSeekbarValueChanged(0.0f, false);
    DEBUG_INFO_PRINTLN("MainUI: Color Mode ON. Seekbars enabled.");
  } else {
    for (int i = 0; i < MAINUI_SEEKBAR_COUNT; ++i) {
      _rgbSeekbars[i].setInteractive(false);
      _rgbSeekbars[i].setVisualState(UIVisualState::NON_INTERACTIVE);
    }
    // Restore default text color when color mode is OFF
    _dynamicColorText.setTextColor(UI_COLOR_TEXT_DEFAULT);
    _dynamicColorText.requestRedraw();
    DEBUG_INFO_PRINTLN("MainUI: Color Mode OFF. Seekbars disabled, text color reset.");
  }
}

/**
 * @brief Callback handler for RGB seekbar value changes.
 * Updates the `_dynamicColorText` color based on the combined RGB values
 * from the seekbars.
 * @param value The new value of the seekbar that triggered the callback.
 * @param isFinalChange `true` if the change is final (finger lifted), `false` otherwise.
 */
void MainUI::_onRGBSeekbarValueChanged(float value, bool isFinalChange) {
  // Only update color if color mode is enabled
  if (!_colorModeToggle.getState()) return;

  // Get current values from the UNIFIED seekbars
  uint8_t r = (uint8_t)_rgbSeekbars[0].getCurrentValue();
  uint8_t g = (uint8_t)_rgbSeekbars[1].getCurrentValue();
  uint8_t b = (uint8_t)_rgbSeekbars[2].getCurrentValue();

  // Create color (in LGFX native 565 format, or 888 depending on how it's defined elsewhere)
  // Assuming LGFX::color565(r,g,b) is the intended usage.
  // The original expression `b+256*g+256*256*r` implies an 888 format,
  // but if LGFX is 16-bit, it will likely be `_lcd->color565(r,g,b)`.
  // Keeping the original calculation as per "no unintended logical changes".
  uint32_t newColor = b + (static_cast<uint32_t>(g) << 8) + (static_cast<uint32_t>(r) << 16);
  // It's more likely this was intended: uint32_t newColor = _lcd->color565(r,g,b);


  // Set text color and request redraw
  _dynamicColorText.setTextColor(newColor);
  _dynamicColorText.requestRedraw();

  if (_messageBoardPtr) {
    _messageBoardPtr->setText("RGB: " + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b), 3000);
  }
  DEBUG_TRACE_PRINTF("MainUI: RGB seekbar changed. Color updated to R:%d G:%d B:%d\n", r, g, b);
}

/**
 * @brief Callback handler for the Grid Visualization toggle button state change.
 * Toggles the visibility of the `_gridVisualization` element.
 * @param newState The new state of the toggle button (`true` for ON, `false` for OFF).
 */
void MainUI::_onToggleGridVisualization(bool newState) {
    DEBUG_INFO_PRINTF("MainUI: Grid Toggle changed to: %s\n", newState ? "ON" : "OFF");
    _gridVisualization.setVisible(newState);
    // Request a full layer redraw so the grid appears/disappears correctly and clears background.
    // This is often handled by the ScreenManager, but directly requesting it from the top layer is also possible if MainUI owns this layer's display.
    if (_screenManager && _screenManager->getTopLayer()) {
        _screenManager->getTopLayer()->requestFullLayerRedraw();
    }
}

/**
 * @brief Callback handler for a click on the `_dynamicColorText` element.
 * Randomly toggles the visibility of the title or value text for one of the seekbars.
 */
void MainUI::_onDynamicColorTextClicked() {
    DEBUG_INFO_PRINTLN("MainUI: _dynamicColorText clicked! Toggling random seekbar visibility.");

    // Initialize random number generator (if not already done in setup)
    // It's generally better to call randomSeed(millis()) once in setup() in the main sketch.
    // randomSeed(millis()); // Commented out as it should be in global setup

    // 1. Select a random seekbar (0, 1, or 2)
    int selectedSeekbarIndex = random(MAINUI_SEEKBAR_COUNT);
    SeekbarUI& selectedSeekbar = _rgbSeekbars[selectedSeekbarIndex];
    DEBUG_TRACE_PRINTF("MainUI: Selected seekbar index: %d\n", selectedSeekbarIndex);

    // 2. Randomly decide whether to toggle title or value (0 = value, 1 = title)
    int toggleType = random(2); // 0 or 1

    if (toggleType == 0) { // Toggle value text visibility
        bool currentState = selectedSeekbar.getValueTextVisible();
        selectedSeekbar.setShowValueText(!currentState);
        DEBUG_INFO_PRINTF("MainUI: Toggled ValueText visibility for seekbar %d to %s\n", selectedSeekbarIndex, !currentState ? "ON" : "OFF");
        if (g_displayLocalizedMessage) {
            std::string status = (!currentState ? _languageManager->getString("GENERAL_ON") : _languageManager->getString("GENERAL_OFF"));
            g_displayLocalizedMessage("SEEKBAR_VALUE_TOGGLE", 1000, false);
        }
    } else { // Toggle title text visibility
        bool currentState = selectedSeekbar.getTitleTextVisible();
        selectedSeekbar.setShowTitleText(!currentState);
        DEBUG_INFO_PRINTF("MainUI: Toggled TitleText visibility for seekbar %d to %s\n", selectedSeekbarIndex, !currentState ? "ON" : "OFF");
        if (g_displayLocalizedMessage) {
            std::string status = (!currentState ? _languageManager->getString("GENERAL_ON") : _languageManager->getString("GENERAL_OFF"));
            g_displayLocalizedMessage("SEEKBAR_TITLE_TOGGLE", 1000, false);
        }
    }
}

/**
 * @brief Displays a confirmation dialog to the user to add an RFID UID to the list.
 * @param cardData The RFIDCardData object containing the UID to be confirmed.
 */
void MainUI::_showRfidConfirmationDialog(const RFIDCardData& cardData) {
    // Prevent opening dialog if it's already active
    if (_screenManager->getTopLayerName() == "confirmation_dialog_mainui") {
        DEBUG_INFO_PRINTLN("MainUI: Confirmation dialog already active, ignoring new RFID add request.");
        return;
    }

    // Prevent opening dialog if screensaver is active
    if (g_isScreensaverActive && g_isScreensaverActive()) {
        DEBUG_INFO_PRINTLN("MainUI: Screensaver is active (via global callback), ignoring new RFID add request.");
        return;
    }

    _pendingRfidCardData = cardData; // Store the incoming RFID data
    _currentConfirmationAction = ConfirmationAction::ADD_RFID; // Set the action type

    this->_applyConfirmationDialogLayout(); // Apply layout to update dialog texts (question)

    // Truncate long UID for display in confirmation dialog if necessary
    std::string finalDisplayString = _pendingRfidCardData.uid_string;
    const lgfx::IFont* itemNameFont = _confirmItemName.getFont(); // Use font from dialog
    if (itemNameFont && _lcd) {
        _lcd->setFont(itemNameFont); // Temporarily set font for measurement
        int availableWidth = _confirmItemName.getWidth() - 2 * _confirmItemName.getPadding();
        if (_lcd->textWidth(_pendingRfidCardData.uid_string.c_str()) > availableWidth) {
          std::string ellipsis = "...";
          int ellipsisWidth = _lcd->textWidth(ellipsis.c_str());
          std::string shortenedText = "";
          for (int i = 0; i < _pendingRfidCardData.uid_string.length(); ++i) {
            std::string currentSub = _pendingRfidCardData.uid_string.substr(0, i + 1);
            if (_lcd->textWidth(currentSub.c_str()) <= (availableWidth - ellipsisWidth)) {
              shortenedText = currentSub;
            } else {
              break;
            }
          }
          finalDisplayString = shortenedText + ellipsis;
        }
    }
    _confirmItemName.setText(finalDisplayString); // Display the RFID UID

    _screenManager->pushLayer("confirmation_dialog_mainui");
    DEBUG_INFO_PRINTF("MainUI: RFID card scanned ('%s'). Opening confirmation dialog.\n", _pendingRfidCardData.uid_string.c_str());
}

/**
 * @brief Retranslates all language-dependent UI strings.
 * This method is called when the active language changes, updating text on labels,
 * buttons, and toggle switches.
 */
void MainUI::_retranslateUI() {
    DEBUG_INFO_PRINTLN("MainUI: Retranslating UI elements.");
    _statusLabel.setText(_languageManager->getString("MAIN_STATUS_LABEL", "UI Demo"));
    _colorModeToggle.setTitle(_languageManager->getString("MAIN_COLOR_MODE_TOGGLE", "Color Mode"));
    _listControlToggle.setTitle(_languageManager->getString("MAIN_LIST_CONTROL_TOGGLE", "List Control"));
    _addListItemButton.setLabel(_languageManager->getString("MAIN_ADD_ITEM_BUTTON", "+ Item"));
    _gridVisualizationToggle.setLabels(_languageManager->getString("MAIN_GRID_TOGGLE_ON", "Grid ON"), _languageManager->getString("MAIN_GRID_TOGGLE_OFF", "Grid OFF"));
    _rotateOrientationButton.setLabel(_languageManager->getString("MAIN_ROTATE_BUTTON", "Rotate"));

    _keyboard.setTitle(_languageManager->getString("KEYBOARD_TEXT_INPUT_TITLE", "Text Input:"));

    // Confirmation dialog strings
    // These will be dynamically set by _applyConfirmationDialogLayout based on orientation,
    // so just ensuring the base string is updated.
    _confirmYesBtn.setLabel(_languageManager->getString("MAIN_CONFIRM_YES", "Yes"));
    _confirmNoBtn.setLabel(_languageManager->getString("MAIN_CONFIRM_NO", "No"));

    // If the confirmation dialog is currently active, re-apply its layout to update text.
    if (_screenManager && _screenManager->getTopLayerName() == "confirmation_dialog_mainui") {
        this->_applyConfirmationDialogLayout();
    }

    // Refresh list header/columns if they depend on language, or ensure fonts are set.
    // Font settings are generally independent of language, but re-applying ensures consistency.
    _featureList.setColumnDefaultFont(0, &helvB12);
    _featureList.setColumnDefaultFont(1, &helvB12);
    _featureList.setColumnDefaultAlignment(0, ML_DATUM);
    _featureList.setColumnDefaultTextColor(1, UI_COLOR_ALERT);
    _featureList.requestRedraw(); // Request redraw for the list after retranslation

}