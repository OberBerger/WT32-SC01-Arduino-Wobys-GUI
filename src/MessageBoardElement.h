/**
 * @file MessageBoardElement.h
 * @brief Defines a dynamic message board UI element for the status bar.
 *
 * This class provides a compact, text-based display area in the status bar
 * for temporary messages, alerts, or status updates. Messages can be
 * displayed for a set duration or cleared manually. It supports dynamic width
 * adjustment and ellipsis for long messages. It now supports a LIFO
 * (Last-In, First-Out) message stack for displaying multiple messages sequentially.
 *
 * @version 1.0.1
 * @date 2025-09-15
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
#ifndef MESSAGEBOARDELEMENT_H
#define MESSAGEBOARDELEMENT_H

#include "StatusbarElement.h" // Provides StatusbarElement base class (which includes Config.h)
#include <string>
#include <vector> // Required for std::vector

// Include UIConfigUser.h for UI_COLOR_TEXT_DEFAULT and PANEL_BACKGROUND_COLOR
// These are used as default constructor parameters.
#include "ConfigUIUser.h"

/**
 * @brief Represents a message queued for display on the MessageBoardElement.
 */
struct QueuedMessage {
    std::string text;              ///< The message string.
    unsigned long durationMs;      ///< Remaining duration in milliseconds (0 for permanent).
    unsigned long displayStartTime;///< Timestamp when the message became active on display. 0 if currently inactive.
    uint32_t color;                ///< Color of the message text.
};

/**
 * @brief A dynamic message board UI element for displaying temporary messages in the status bar.
 *
 * This element can display text messages for a specified duration or permanently
 * until cleared. It supports dynamic width based on available space and truncates
 * long messages with an ellipsis. It uses a LIFO (Last-In, First-Out) stack
 * to manage multiple incoming messages, ensuring the latest message is always
 * shown first, and previous messages reappear when the current one expires.
 */
class MessageBoardElement : public StatusbarElement {
private:
    std::vector<QueuedMessage> _messageStack;  ///< Stack to hold messages, LIFO order.

    uint8_t _padding;                  ///< Padding around the text within the element.
    uint32_t _textColor;               ///< Default text color for messages when not specified in pushMessage.
    uint32_t _backgroundColor;         ///< Background color of the message area.
    const lgfx::IFont* _font;          ///< Font used for displaying the message.

    int32_t _actualDrawWidth;          ///< The actual width in pixels available for drawing the message.

public:
    /**
     * @brief Constructor for MessageBoardElement.
     * @param lcd Pointer to the LGFX display instance.
     * @param font Font to use for displaying messages.
     * @param textColor Color of the message text (default: UI_COLOR_TEXT_DEFAULT).
     * @param backgroundColor Background color of the message area (default: PANEL_BACKGROUND_COLOR).
     * @param elementName Optional name for debugging.
     */
    MessageBoardElement(LGFX* lcd, 
                        const lgfx::IFont* font, 
                        uint32_t textColor = UI_COLOR_TEXT_DEFAULT, 
                        uint32_t backgroundColor = PANEL_BACKGROUND_COLOR, 
                        const std::string& elementName = "MessageBoard");

    

    /**
     * @brief Draws the message board element onto the display.
     * This method renders the current message, applying padding, truncation, and background.
     * @param x The absolute X coordinate on the screen where the element should be drawn.
     * @param y The absolute Y coordinate on the screen where the element should be drawn.
     * @param availableWidth The actual width in pixels available for drawing the message.
     */
    virtual void draw(int32_t x, int32_t y, int32_t availableWidth) override;
    
    /**
     * @brief Updates the state of the message board element.
     * This method is called repeatedly to check for message expiry and trigger clearing.
     */
    virtual void update() override;
    
    /**
     * @brief Retrieves the width of the message board element.
     * @return 0, as this is a dynamic width element. The actual width is set via `setActualDrawWidth`.
     */
    virtual int32_t getWidth() const override { return 0; } // Dynamic width element

    /**
     * @brief Sets the internal padding around the message text.
     * Requests a redraw if the padding changes.
     * @param padding The new padding value in pixels.
     */
    void setPadding(uint8_t padding);

    /**
     * @brief Pushes a message onto the display stack.
     * The new message becomes immediately visible. If another message was active,
     * its display timer is paused and it is moved down the stack.
     * @param message The string to display.
     * @param durationMs Optional duration for the message to be visible (0 for permanent).
     * @param messageColor Optional color for the message text (defaults to the element's current default text color, `_textColor`).
     */
    void pushMessage(const std::string& message, unsigned long durationMs = 0, uint32_t messageColor = UI_COLOR_TEXT_DEFAULT);

    /**
     * @brief Sets the message to display, clearing any existing messages from the stack first.
     * This effectively replaces all current and queued messages with a single new one.
     * @param message The string to display.
     * @param durationMs Optional duration for the message to be visible (0 for permanent).
     */
    void setText(const std::string& message, unsigned long durationMs = 0);
    
    /**
     * @brief Clears the currently displayed message by removing it from the stack.
     * If there are other messages in the stack, the previous one becomes active.
     */
    void clearText();

    /**
     * @brief Sets the actual drawing width available for the element.
     * This is typically called by the StatusbarUI to inform the element about its allocated space.
     * Requests a redraw if the width changes.
     * @param width The actual width in pixels.
     */
    void setActualDrawWidth(int32_t width);

    /**
     * @brief Sets the default text color for new messages when no specific color is provided to `pushMessage`.
     * If there's a message currently displayed, calling this will trigger a redraw to apply the new default color
     * to that message, unless it was pushed with an explicit color.
     * @param color The new default text color.
     */
    void setTextColor(uint32_t color);
};

#endif // MESSAGEBOARDELEMENT_H