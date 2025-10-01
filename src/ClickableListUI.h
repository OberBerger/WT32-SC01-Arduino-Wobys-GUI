/**
 * @file ClickableListUI.h
 * @brief Defines a scrollable, clickable list UI element with multiple columns.
 *
 * This class provides a flexible list component capable of displaying
 * tabular data, handling user selections, and managing scrolling.
 * It integrates with UI elements for consistent styling and behavior.
 *
 * @version 1.0.1
 * @date 2025-08-10
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
#ifndef CLICKABLELISTUI_H
#define CLICKABLELISTUI_H

#include "UIElement.h"    // Provides UIElement base class and includes Config.h
#include "ListItem.h"     // Required for ColumnData and ListItem structs
#include <vector>         // Required for std::vector
#include <string>         // Required for std::string
#include <functional>     // Required for std::function for callbacks

// Required for UI colors and constants like LISTUI_SCROLLBAR_COLOR
#include "ConfigUIUser.h"

/**
 * @brief A scrollable, clickable list UI element with multiple columns.
 *
 * The ClickableListUI class provides a flexible list component for displaying
 * tabular data. It supports multiple columns, customizable item height, fonts,
 * and text alignments for each column. Users can scroll through the list,
 * select individual items, and trigger a callback upon item selection.
 * The list also features a visual scroll bar, borders, and dividers, along
 * with distinct visual states for selected and interactive/non-interactive items.
 */
class ClickableListUI : public UIElement {
public:
    /**
     * @brief Callback type for when an item in the list is selected.
     * @param index The logical index of the selected item in the underlying data vector.
     * @param data A constant reference to the `ListItem` data of the selected item.
     * @param touchX_inItem The X coordinate of the touch within the bounds of the selected item, relative to its left edge.
     */
    using ItemSelectedCallback = std::function<void(int index, const ListItem& data, int16_t touchX_inItem)>;

protected:
    // --- Data Storage ---
    std::vector<ListItem> _listItems; ///< The vector containing all `ListItem` data to be displayed.

    // --- Layout & Sizing ---
    int16_t _x_rel;                   ///< Relative X position of the list's top-left corner within its parent layer.
    int16_t _y_rel;                   ///< Relative Y position of the list's top-left corner within its parent layer.
    int16_t _width;                   ///< Configured width of the list element.
    int16_t _height;                  ///< Configured height of the list element.
    int16_t _itemHeight;              ///< The fixed height of each individual list item in pixels.
    uint8_t _itemContentPadding;      ///< Padding around the text content within each list item.
    int16_t _minScrollThreshold;      ///< Minimum drag distance in pixels to initiate scrolling.

    // --- Display & Visual Properties ---
    const lgfx::IFont* _defaultTextFont; ///< The default font used for text in list items if no specific font is set.
    bool _drawDividers;               ///< True to draw horizontal divider lines between list items.
    bool _drawBorder;                 ///< True to draw a border around the entire list.
    bool _drawScrollBar;              ///< True to draw a vertical scroll bar.
    bool _isClickable;                ///< True if individual list items can be clicked and selected.

    // --- Colors ---
    uint32_t _defaultTextColor;       ///< Default text color for list items.
    uint32_t _backgroundColor;        ///< Background color of the list.
    uint32_t _selectedItemBackgroundColor; ///< Background color for a selected list item.
    uint32_t _selectedItemTextColor;  ///< Text color for a selected list item.
    uint32_t _borderColor;            ///< Color of the list's border.
    uint32_t _dividerColor;           ///< Color of the horizontal divider lines.
    uint32_t _scrollBarColor;         ///< Color of the scroll bar.
    // Inherited from UIElement: _disabledTextColor, _disabledBorderColor, _disabledBackgroundColor.

    // --- Column Management ---
    int _numColumns;                  ///< The number of columns in the list.
    std::vector<int16_t> _columnWidthsPx; ///< Vector storing the pixel width of each column.
    std::vector<const lgfx::IFont*> _defaultColumnFonts; ///< Default fonts for each column.
    std::vector<textdatum_t> _defaultColumnAlignments;   ///< Default text alignment for each column.
    std::vector<uint32_t> _defaultColumnTextColors;     ///< Default text colors for each column.

    // --- Scrolling & Selection State ---
    int _firstVisibleItemIndex;       ///< The logical index of the first item currently visible at the top of the list.
    int _maxVisibleItems;             ///< The maximum number of list items that can be displayed simultaneously.
    int _selectedItemLogicalIndex;    ///< The logical index of the currently selected list item (-1 if none selected).
    int _pressedItemVisualIndex;      ///< The visual index of the item currently being pressed (for touch feedback, -1 if none).

    // --- Callbacks ---
    ItemSelectedCallback _onItemSelectedCallback; ///< Callback function executed when a list item is selected.

    // --- Touch Handling ---
    bool _isBeingPressed;             ///< True if the list is currently being touched/dragged.
    int32_t _touchStartY_layer;       ///< Y coordinate of the touch start point (relative to the layer).
    int _firstVisibleAtTouchStart;    ///< `_firstVisibleItemIndex` at the start of a touch/drag gesture.

    // --- Protected Helper Methods ---
    /**
     * @brief Recalculates the maximum number of list items that can be displayed
     * simultaneously based on the list's height and item height.
     * This method updates the `_maxVisibleItems` member.
     */
    void calculateMaxVisibleItems();

    /**
     * @brief Recalculates the pixel widths for all columns, distributing available space.
     * This method updates the `_columnWidthsPx` vector.
     */
    void calculateColumnWidths();

    /**
     * @brief Draws a single list item onto the display.
     * This helper method handles rendering item backgrounds, columns, and text,
     * including truncation and applying selection/color states.
     * @param logicalItemIndex The logical index of the item in the `_listItems` vector.
     * @param visualItemIndex The visual index of the item on the screen (0-indexed from the top visible item).
     * @param itemAreaX_onScreen The absolute X coordinate on the screen for the item's drawing area.
     * @param itemAreaY_onScreen The absolute Y coordinate on the screen for the item's drawing area.
     * @param itemAreaWidth_onScreen The actual width available for drawing the item.
     */
    void drawItem(int logicalItemIndex, int visualItemIndex,
                  int16_t itemAreaX_onScreen, int16_t itemAreaY_onScreen,
                  int16_t itemAreaWidth_onScreen);

    /**
     * @brief Truncates a given text string with an ellipsis if it exceeds a specified maximum width.
     * @param text The input text string to potentially truncate.
     * @param font The font to use for measuring text width.
     * @param maxWidth The maximum pixel width the text should occupy.
     * @param ellipsis The string to use as an ellipsis (default: "...").
     * @return The original or truncated text with an ellipsis.
     */
    std::string truncateText(const std::string& text, const lgfx::IFont* font, int16_t maxWidth, const std::string& ellipsis = "...");

public:
    // --- Constructor ---
    /**
     * @brief Constructor for the ClickableListUI class.
     * Initializes the list with its position, size, and number of columns.
     * Sets default display and interaction properties.
     * @param lcd Pointer to the LGFX display instance.
     * @param x The X coordinate of the list's top-left corner (relative to the layer).
     * @param y The Y coordinate of the list's top-left corner (relative to the layer).
     * @param w The width of the list.
     * @param h The height of the list.
     * @param numCols The number of columns in the list (default: 1).
     */
    ClickableListUI(LGFX* lcd,
                    int16_t x, int16_t y, int16_t w, int16_t h,
                    int numCols = 1);

    // --- Data Management Methods ---
    /**
     * @brief Sets the entire list of items to be displayed.
     * Clears any existing items, resets selection and scroll position, and requests a redraw.
     * @param items A `std::vector` of `ListItem` objects to set.
     */
    void setItems(const std::vector<ListItem>& items);

    /**
     * @brief Adds a single item to the end of the list.
     * Requests a redraw.
     * @param item The `ListItem` to add.
     */
    void addItem(const ListItem& item);

    /**
     * @brief Updates an existing item in the list at a specific logical index.
     * Preserves the selection state of the updated item if it was previously selected.
     * Requests a redraw.
     * @param logicalIndex The 0-indexed logical position of the item to update.
     * @param item The new `ListItem` data to replace the existing one.
     */
    void updateItem(int logicalIndex, const ListItem& item);

    /**
     * @brief Removes a single item from the list at a specific logical index.
     * Adjusts selection and scroll positions if necessary, and requests a redraw.
     * @param logicalIndex The 0-indexed logical position of the item to remove.
     * @return True if the item was successfully removed, false if the index was invalid.
     */
    bool removeItem(int logicalIndex);

    /**
     * @brief Clears all items from the list.
     * Resets scroll position and selection, and requests a redraw.
     */
    void clearItems();

    /**
     * @brief Retrieves a constant reference to the underlying vector of `ListItem` objects.
     * @return A `const std::vector<ListItem>&` containing all list items.
     */
    const std::vector<ListItem>& getItems() const;

    /**
     * @brief Retrieves a copy of the `ListItem` data at a specific logical index.
     * @param logicalIndex The 0-indexed logical position of the item to retrieve.
     * @return A copy of the `ListItem` object, or an empty `ListItem` if the index is invalid.
     */
    ListItem getItem(int logicalIndex) const;

    /**
     * @brief Retrieves the logical index of the currently selected item.
     * @return The 0-indexed logical index of the selected item, or -1 if no item is selected.
     */
    int getSelectedItemIndex() const;

    /**
     * @brief Retrieves a copy of the `ListItem` data for the currently selected item.
     * @return A copy of the `ListItem` object, or an empty `ListItem` if no item is selected.
     */
    ListItem getSelectedItemData() const;

    // --- Column Configuration ---
    /**
     * @brief Sets the number of columns in the list.
     * Recalculates default column widths, fonts, and alignments, and requests a redraw.
     * @param numCols The new number of columns (must be > 0).
     */
    void setNumColumns(int numCols);

    /**
     * @brief Sets the pixel width of a specific column.
     * Requests a redraw. Note: Manual column width settings require careful management
     * to ensure the total width does not exceed the list's `_width`.
     * @param colIndex The 0-indexed column to configure.
     * @param widthInPixels The desired width in pixels (must be >= 0).
     */
    void setColumnWidth(int colIndex, int16_t widthInPixels);

    /**
     * @brief Sets the default font for a specific column.
     * Requests a redraw.
     * @param colIndex The 0-indexed column to configure.
     * @param font A pointer to the `lgfx::IFont` to use (nullptr reverts to `_defaultTextFont`).
     */
    void setColumnDefaultFont(int colIndex, const lgfx::IFont* font);

    /**
     * @brief Sets the default text alignment for a specific column.
     * Requests a redraw.
     * @param colIndex The 0-indexed column to configure.
     * @param alignment The `textdatum_t` alignment for the column.
     */
    void setColumnDefaultAlignment(int colIndex, textdatum_t alignment);

    /**
     * @brief Sets the default text color for a specific column.
     * Requests a redraw. A value of 0 indicates using the list's `_defaultTextColor` or `_selectedItemTextColor`.
     * @param colIndex The 0-indexed column to configure.
     * @param color The `uint32_t` color to use (0 for default/selected).
     */
    void setColumnDefaultTextColor(int colIndex, uint32_t color);

    /**
     * @brief Updates the text content of a specific column within a specific list item.
     * Requests a redraw if the text changes.
     * @param itemIndex The logical index of the list item.
     * @param columnIndex The 0-indexed column index within the item.
     * @param newText The new text string for the column.
     * @return True if the column text was successfully updated, false otherwise (e.g., invalid indices).
     */
    bool updateItemColumn(int itemIndex, int columnIndex, const std::string& newText);

    /**
     * @brief Retrieves the 0-indexed column index that was clicked within an item's touch area.
     * @param touchX_inItem The X coordinate of the touch within the item's area, relative to its left edge.
     * @return The 0-indexed column index, or -1 if the touch was outside any column.
     */
    int getClickedColumnIndex(int16_t touchX_inItem) const;

    // --- Display Settings ---
    /**
     * @brief Sets the fixed height of each list item.
     * Recalculates the maximum visible items and requests a redraw.
     * @param height The new item height in pixels (must be > 0).
     */
    void setItemHeight(int16_t height);

    /**
     * @brief Sets the padding around the text content within each list item.
     * Requests a redraw.
     * @param padding The new padding value in pixels.
     */
    void setItemContentPadding(uint8_t padding);

    /**
     * @brief Sets the minimum drag threshold in pixels required to initiate scrolling.
     * @param threshold The new minimum scroll threshold.
     */
    void setMinScrollThreshold(int16_t threshold);

    /**
     * @brief Sets the default font for text in list items.
     * Requests a redraw.
     * @param font A pointer to the `lgfx::IFont` to use (nullptr reverts to a fallback default).
     */
    void setDefaultTextFont(const lgfx::IFont* font);

    /**
     * @brief Enables or disables the drawing of horizontal divider lines between list items.
     * Requests a redraw.
     * @param draw True to draw dividers, false otherwise.
     */
    void setDrawDividers(bool draw);

    /**
     * @brief Enables or disables the drawing of a border around the entire list.
     * Requests a redraw.
     * @param draw True to draw a border, false otherwise.
     */
    void setDrawBorder(bool draw);

    /**
     * @brief Enables or disables the drawing of a vertical scroll bar.
     * Requests a redraw.
     * @param draw True to draw the scroll bar, false otherwise.
     */
    void setDrawScrollBar(bool draw);

    /**
     * @brief Retrieves the current setting for drawing the scroll bar.
     * @return True if the scroll bar is set to be drawn, false otherwise.
     */
    bool getDrawScrollBar() const;

    // --- Color Settings ---
    /**
     * @brief Sets the default text color for list items.
     * Requests a redraw.
     * @param color The new default text color.
     */
    void setDefaultTextColor(uint32_t color);

    /**
     * @brief Sets the background color of the list.
     * Requests a redraw.
     * @param color The new background color.
     */
    void setBackgroundColor(uint32_t color);

    /**
     * @brief Sets the background color for selected list items.
     * Requests a redraw.
     * @param color The new selected item background color.
     */
    void setSelectedItemBackgroundColor(uint32_t color);

    /**
     * @brief Sets the text color for selected list items.
     * Requests a redraw.
     * @param color The new selected item text color.
     */
    void setSelectedItemTextColor(uint32_t color);

    /**
     * @brief Sets the border color of the list.
     * Requests a redraw.
     * @param color The new border color.
     */
    void setBorderColor(uint32_t color);

    /**
     * @brief Sets the color of the horizontal divider lines.
     * Requests a redraw.
     * @param color The new divider color.
     */
    void setDividerColor(uint32_t color);

    /**
     * @brief Sets the color of the scroll bar.
     * Requests a redraw.
     * @param color The new scroll bar color.
     */
    void setScrollBarColor(uint32_t color);

    /**
     * @brief Sets the colors for the disabled (non-interactive) state of the list.
     * Overrides the base class method and requests a redraw.
     * @param textColor Text color when disabled.
     * @param borderColor Border color when disabled.
     * @param bgColor Background color when disabled.
     */
    void setDisabledColors(uint32_t textColor, uint32_t borderColor, uint32_t bgColor) override;

    // --- Interaction ---
    /**
     * @brief Sets the callback function to be executed when a list item is selected.
     * @param callback The `ItemSelectedCallback` function.
     */
    void setOnItemSelectedCallback(ItemSelectedCallback callback);

    /**
     * @brief Selects a specific item in the list by its logical index.
     * Scrolls the list to make the item visible if it's not already.
     * Requests a redraw.
     * @param logicalIndex The 0-indexed logical position of the item to select (-1 to deselect all).
     * @return True if the item was successfully selected, false if the index was invalid.
     */
    bool selectItem(int logicalIndex);

    /**
     * @brief Selects the next item in the list.
     * If the last item is already selected, it remains selected. Scrolls to the item if needed.
     * @return True if an item was newly selected, false otherwise.
     */
    bool selectNext();

    /**
     * @brief Selects the previous item in the list.
     * If no item is selected, it remains unselected. Scrolls to the item if needed.
     * @return True if an item was newly selected, false otherwise.
     */
    bool selectPrevious();

    /**
     * @brief Sets the currently selected item by its logical index.
     * This method is intended for external control (e.g., from a UI controller).
     * It updates the `isSelected` flag of items and optionally requests a redraw.
     * @param index The logical index of the item to select (-1 to deselect all).
     * @param redraw If true, a redraw is requested (default: true).
     */
    void setSelectedItemIndex(int index, bool redraw = true);

    // --- Scrolling ---
    /**
     * @brief Scrolls the list to make a specific item visible.
     * Adjusts `_firstVisibleItemIndex` and requests a redraw.
     * @param logicalIndex The logical index of the item to scroll to.
     */
    void scrollToItem(int logicalIndex);

    /**
     * @brief Scrolls the list down by a specified number of lines.
     * Requests a redraw.
     * @param lines The number of lines to scroll down (default: 1).
     */
    void scrollDown(int lines = 1);

    /**
     * @brief Scrolls the list up by a specified number of lines.
     * Requests a redraw.
     * @param lines The number of lines to scroll up (default: 1).
     */
    void scrollUp(int lines = 1);

    // --- Core UIElement Overrides ---
    /**
     * @brief Draws the list element onto the display.
     * Renders the list background, border, scroll bar, and visible items,
     * applying all configured styles and handling selection states.
     */
    void draw() override;

    /**
     * @brief Updates the internal state of the list element.
     * Currently, this method is empty as there is no dynamic internal logic
     * or animation implemented.
     */
    void update() override;

    /**
     * @brief Handles touch input for the list element.
     * Manages scrolling, item selection, and visual feedback for pressed items.
     * @param tx_layer The X coordinate of the touch input (relative to the layer).
     * @param ty_layer The Y coordinate of the touch input (relative to the layer).
     * @param isPressed True if the screen is currently pressed, false otherwise.
     * @return True if the list handled the touch event, false otherwise.
     */
    bool handleTouch(int32_t tx_layer, int32_t ty_layer, bool isPressed) override;

    /**
     * @brief Sets the interactivity state for the entire list element.
     * Overrides the base class method to also handle selection clearing when
     * interactivity is disabled.
     * @param interactive True to enable touch interaction for the list, false to disable.
     */
    void setInteractive(bool interactive) override;

    /**
     * @brief Sorts the list items alphabetically based on the text in the first column.
     * Resets scroll position and selection, and requests a redraw.
     * @param ascending True for ascending order (A-Z), false for descending (Z-A).
     *                  Defaults to true.
     */
    void sortItemsAlphabetically(bool ascending = true);

    // --- Getters for Layout & Sizing ---
    /**
     * @brief Sets the relative position of the list element within its parent layer.
     * Requests a redraw if the position changes.
     * @param x The new relative X coordinate.
     * @param y The new relative Y coordinate.
     */
    void setPosition(int16_t x, int16_t y) override;

    /**
     * @brief Sets the configured width and height of the list element.
     * Recalculates maximum visible items and column widths, and requests a redraw.
     * @param w The new width.
     * @param h The new height.
     */
    void setSize(int16_t w, int16_t h) override;

    /**
     * @brief Retrieves the relative X position of the list element.
     * @return The relative X coordinate in pixels.
     */
    int16_t getX() const { return _x_rel; }

    /**
     * @brief Retrieves the relative Y position of the list element.
     * @return The relative Y coordinate in pixels.
     */
    int16_t getY() const { return _y_rel; }

    /**
     * @brief Retrieves the configured width of the list element.
     * @return The width in pixels.
     */
    int16_t getWidth() const override { return _width; }

    /**
     * @brief Retrieves the configured height of the list element.
     * @return The height in pixels.
     */
    int16_t getHeight() const override { return _height; }

    /**
     * @brief Retrieves the current content padding for list items.
     * @return The content padding in pixels.
     */
    uint8_t getItemContentPadding() const { return _itemContentPadding; }

    /**
     * @brief Retrieves the current setting for drawing the border.
     * @return True if the border is set to be drawn, false otherwise.
     */
    bool getDrawBorder() const { return _drawBorder; }

    /**
     * @brief Overrides the base class method to provide the specific UI element type for ClickableListUI.
     * @return The UIElementType enum value representing a CLICKABLE_LIST element.
     */
    UIElementType getElementType() const override { return UIElementType::CLICKABLE_LIST; }
};

#endif // CLICKABLELISTUI_H