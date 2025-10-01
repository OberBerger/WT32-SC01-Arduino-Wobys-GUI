/**
 * @file GridLayoutUI.h
 * @brief Defines the GridLayoutUI class, a UI element for visualizing and organizing content in a grid.
 *
 * This class provides a flexible grid layout system for arranging other UI elements.
 * It allows defining a grid with a specified number of columns and rows,
 * along with horizontal and vertical spacing. It can calculate the dimensions
 * and positions of individual cells or blocks of cells, facilitating structured
 * UI design. The grid itself can be visualized for debugging purposes.
 *
 * @version 1.0.1
 * @date 2025-08-01
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
#ifndef GRID_LAYOUT_UI_H
#define GRID_LAYOUT_UI_H

#include <Arduino.h>
#include <string>
#include "UIElement.h"
#include "Config.h" // Required for debug macros and GRID_DEFAULT_XXX constants

/**
 * @brief Structure to hold information about a single grid cell or a block of cells.
 */
struct GridCellInfo {
    int16_t x;       ///< The X coordinate of the top-left corner (relative to the grid's origin).
    int16_t y;       ///< The Y coordinate of the top-left corner (relative to the grid's origin).
    int16_t w;       ///< The width of the cell/block.
    int16_t h;       ///< The height of the cell/block.
    int16_t centerX; ///< The X coordinate of the center of the cell/block (relative to the grid's origin).
    int16_t centerY; ///< The Y coordinate of the center of the cell/block (relative to the grid's origin).
};

/**
 * @brief A UI element for visualizing and organizing content in a grid.
 *
 * The GridLayoutUI class provides a flexible system to define and manage a grid
 * structure on the display. It can calculate layout parameters for individual cells
 * or contiguous blocks of cells, including internal padding/spacing. While primarily
 * a layout helper, it can also visualize the grid for debugging purposes.
 */
class GridLayoutUI : public UIElement {
public:
    // --- Constructors ---
    /**
     * @brief Constructor for the GridLayoutUI class with full parameters.
     * Initializes a grid with specified columns, rows, and spacing.
     * @param lcd Pointer to the LGFX display instance.
     * @param numCols The number of columns in the grid.
     * @param numRows The number of rows in the grid.
     * @param hSpacing The horizontal spacing (internal padding) between cells in pixels (default: GRID_DEFAULT_H_SPACING_PIXELS).
     * @param vSpacing The vertical spacing (internal padding) between cells in pixels (default: GRID_DEFAULT_V_SPACING_PIXELS).
     */
    GridLayoutUI(LGFX* lcd, int numCols, int numRows,
                 int hSpacing = GRID_DEFAULT_H_SPACING_PIXELS,
                 int vSpacing = GRID_DEFAULT_V_SPACING_PIXELS);

    /**
     * @brief Default constructor for the GridLayoutUI class.
     * Initializes a 1x1 grid with default spacing. Delegates to the full parameter constructor.
     * @param lcd Pointer to the LGFX display instance.
     */
    GridLayoutUI(LGFX* lcd);

    // --- Grid Configuration ---
    /**
     * @brief Sets the grid's parameters, including columns, rows, and spacing.
     * Requests a redraw if any parameter changes.
     * @param numCols The new number of columns (must be > 0).
     * @param numRows The new number of rows (must be > 0).
     * @param hSpacing The new horizontal spacing between cells (must be >= 0).
     * @param vSpacing The new vertical spacing between cells (must be >= 0).
     */
    void setGridParameters(int numCols, int numRows, int hSpacing, int vSpacing);

    /**
     * @brief Sets the total width and height of the container that the grid occupies.
     * Requests a redraw if the size changes.
     * @param width The new container width in pixels (must be >= 0).
     * @param height The new container height in pixels (must be >= 0).
     */
    void setContainerSize(int32_t width, int32_t height);

    // --- Layout & Sizing (Overrides from UIElement) ---
    /**
     * @brief Sets the relative position of the grid's top-left corner within its parent layer.
     * Requests a redraw if the position changes.
     * @param x The new relative X coordinate.
     * @param y The new relative Y coordinate.
     */
    void setPosition(int16_t x, int16_t y) override;

    /**
     * @brief Sets the total width and height of the grid's container.
     * This method delegates to `setContainerSize`.
     * @param w The new container width.
     * @param h The new container height.
     */
    void setSize(int16_t w, int16_t h) override;

    // --- Cell & Block Dimension Getters ---
    /**
     * @brief Calculates the bounding rectangle of the *content area* for a specific cell.
     * The content area excludes the internal horizontal and vertical spacing (`_hSpacing`, `_vSpacing`).
     * @param col The column index of the cell (0-indexed).
     * @param row The row index of the cell (0-indexed).
     * @return A `GridCellInfo` struct containing the coordinates and dimensions of the content area.
     */
    GridCellInfo getCellRect(int col, int row) const;

    /**
     * @brief Calculates the bounding rectangle of the *content area* for a block of cells.
     * The content area excludes internal spacing. The block is defined by its top-left (`startCol`, `startRow`)
     * and bottom-right (`endCol`, `endRow`) cell indices.
     * @param startCol The starting column index (0-indexed).
     * @param startRow The starting row index (0-indexed).
     * @param endCol The ending column index (0-indexed).
     * @param endRow The ending row index (0-indexed).
     * @return A `GridCellInfo` struct containing the coordinates and dimensions of the content area for the block.
     */
    GridCellInfo getBlockRect(int startCol, int startRow, int endCol, int endRow) const;

    /**
     * @brief Calculates the bounding rectangle of the *physical space* occupied by a specific cell.
     * The physical space *includes* the internal horizontal and vertical spacing.
     * @param col The column index of the cell (0-indexed).
     * @param row The row index of the cell (0-indexed).
     * @return A `GridCellInfo` struct containing the coordinates and dimensions of the physical cell area.
     */
    GridCellInfo getPhysicalCellRect(int col, int row) const;

    /**
     * @brief Calculates the bounding rectangle of the *physical space* occupied by a block of cells.
     * The physical space *includes* internal spacing. The block is defined by its top-left (`startCol`, `startRow`)
     * and bottom-right (`endCol`, `endRow`) cell indices.
     * @param startCol The starting column index (0-indexed).
     * @param startRow The starting row index (0-indexed).
     * @param endCol The ending column index (0-indexed).
     * @param endRow The ending row index (0-indexed).
     * @return A `GridCellInfo` struct containing the coordinates and dimensions of the physical block area.
     */
    GridCellInfo getPhysicalBlockRect(int startCol, int startRow, int endCol, int endRow) const;

    // --- Visualization & Debugging ---
    /**
     * @brief Sets the color of the grid lines for visualization.
     * Requests a redraw.
     * @param color The new color for the grid lines.
     */
    void setGridLineColor(uint32_t color) { _gridLineColor = color; requestRedraw(); }

    /**
     * @brief Sets the thickness of the grid lines for visualization.
     * Requests a redraw.
     * @param thickness The new thickness in pixels.
     */
    void setGridLineThickness(uint8_t thickness) { _gridLineThickness = thickness; requestRedraw(); }

    /**
     * @brief Enables or disables the drawing of grid cell outlines for visualization.
     * Requests a redraw.
     * @param draw True to draw cell outlines, false otherwise.
     */
    void setDrawCells(bool draw) { _drawCells = draw; requestRedraw(); }

    /**
     * @brief Sets the visibility of the grid visualization.
     * Overrides the base class method to request a full parent layer redraw when visibility changes.
     * @param visible True to make the grid visible, false to hide it.
     * @param redraw True to force an immediate redraw if visibility changes (default: true).
     */
    void setVisible(bool visible, bool redraw = true) override;

    // --- Core UIElement Overrides ---
    /**
     * @brief Draws the grid visualization onto the display.
     * Renders outlines for physical cells, content areas, and spacing if enabled.
     * This method is primarily for debugging and visualizing the grid structure.
     */
    void draw() override;

    /**
     * @brief Updates the internal state of the grid layout.
     * For GridLayoutUI, this method is typically empty as its state is static once configured.
     */
    void update() override;

    /**
     * @brief Handles touch input for the grid layout.
     * GridLayoutUI is typically not interactive and this method will always return false.
     * @param tx_layer The X coordinate of the touch input (relative to the layer).
     * @param ty_layer The Y coordinate of the touch input (relative to the layer).
     * @param isPressed True if the screen is currently pressed, false otherwise.
     * @return Always returns false, as GridLayoutUI typically does not handle touch events.
     */
    bool handleTouch(int32_t tx_layer, int32_t ty_layer, bool isPressed) override;

    /**
     * @brief Retrieves the width of the grid's container.
     * @return The container width in pixels.
     */
    int16_t getWidth() const override { return _containerWidth; }

    /**
     * @brief Retrieves the height of the grid's container.
     * @return The container height in pixels.
     */
    int16_t getHeight() const override { return _containerHeight; }

    /**
     * @brief Retrieves the horizontal spacing between cells.
     * @return The horizontal spacing in pixels.
     */
    int getHSpacing() const { return _hSpacing; }

    /**
     * @brief Retrieves the vertical spacing between cells.
     * @return The vertical spacing in pixels.
     */
    int getVSpacing() const { return _vSpacing; }

private:
    // --- Grid Dimensions & Spacing ---
    int _numCols;              ///< Number of columns in the grid.
    int _numRows;              ///< Number of rows in the grid.
    int _hSpacing;             ///< Horizontal spacing (internal padding) between cells.
    int _vSpacing;             ///< Vertical spacing (internal padding) between cells.

    // --- Container & Positioning ---
    int32_t _containerWidth;   ///< Total width of the area the grid occupies.
    int32_t _containerHeight;  ///< Total height of the area the grid occupies.
    int16_t _x_rel;            ///< Relative X position of the grid's top-left corner within its parent layer.
    int16_t _y_rel;            ///< Relative Y position of the grid's top-left corner within its parent layer.

    // --- Visualization Properties ---
    uint32_t _gridLineColor;   ///< Color of the grid lines when drawn for visualization.
    uint8_t _gridLineThickness;///< Thickness of the grid lines in pixels.
    bool _drawCells;           ///< True if grid cell outlines should be drawn for visualization.

    // --- Cached Physical Dimensions (mutable for const methods) ---
    mutable float _physicalCellWidth; ///< Cached calculated width of a single physical cell.
    mutable float _physicalCellHeight;///< Cached calculated height of a single physical cell.
    mutable bool _recalcPhysicalDims; ///< Flag indicating if physical dimensions need recalculation.

    // --- Private Helper Methods ---
    /**
     * @brief Recalculates the physical dimensions (width and height) of a single cell.
     * This method updates `_physicalCellWidth` and `_physicalCellHeight` and sets
     * `_recalcPhysicalDims` to false. It is typically called from const methods,
     * hence `mutable` members are used.
     */
    void _recalcPhysicalDimensions() const;
};

#endif // GRID_LAYOUT_UI_H