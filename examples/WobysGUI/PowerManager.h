#pragma once

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

/**
 * @file PowerManager.h
 * @brief Defines the PowerManager class for battery and system power management.
 *
 * This class is responsible for monitoring battery voltage, determining battery
 * level, and controlling system power for a safe shutdown. It integrates with
 * UI elements to display battery status and provides callbacks for critical events.
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

#include <Arduino.h>    // For basic types like unsigned long, int, float
#include <functional>   // For std::function
#include <string>       // For std::string in callbacks
#include "Config.h"     // For ALL custom configurations (e.g., DEBUG_PRINT macros)

// Forward declarations to avoid circular dependencies.
// The PowerManagerConfig struct is defined in SystemInitializer.h.
struct PowerManagerConfig;
class StatusbarUI;
class IconElement;

/**
 * @brief Manages battery status, voltage monitoring, and system power control.
 *
 * The PowerManager handles reading battery voltage, determining the corresponding
 * battery level icon, and triggering warnings or system shutdown based on
 * configurable thresholds. It also provides an interface for UI elements
 * to register callbacks for battery status updates and shutdown events.
 */
class PowerManager {
  public:
    // Constructors and Destructors
    /**
     * @brief Constructs a new PowerManager object.
     * Initializes pointers and default values for internal state.
     *
     * @param statusbar A pointer to the `StatusbarUI` instance for displaying messages.
     * @param batteryIconElement A pointer to the `IconElement` responsible for displaying the battery icon.
     */
    PowerManager(StatusbarUI* statusbar, IconElement* batteryIconElement);

    // Initialization
    /**
     * @brief Initializes the PowerManager with hardware-specific configurations.
     * This method must be called after constructing the object.
     * It sets up ADC and power control pins, voltage divider parameters,
     * and battery level thresholds based on the provided configuration.
     *
     * @param config A constant reference to the `PowerManagerConfig` struct
     *               containing all necessary hardware configuration parameters.
     */
    void init(const PowerManagerConfig& config);

    // UI Element Management
    /**
     * @brief Sets the `IconElement` instance for UI updates related to the battery.
     * This method is public so `SystemInitializer` can set it dynamically.
     *
     * @param element A pointer to the `IconElement` instance to be used for battery display.
     */
    void setBatteryIconElement(IconElement* element);

    // Main Loop and System Control
    /**
     * @brief Main loop function for PowerManager.
     *
     * This method should be called repeatedly in the Arduino `loop()` function.
     * It periodically checks the battery status and manages power-related events.
     */
    void loop();

    /**
     * @brief Requests a graceful system power-off.
     *
     * This method triggers the shutdown sequence, including calling registered
     * shutdown warning and perform shutdown callbacks, before initiating the
     * actual power-off procedure.
     */
    void requestSystemPowerOff();

    /**
     * @brief Enables the system power relay, allowing the user to release the power button.
     * This method should be called by the `SystemInitializer` at the beginning of its `init()`
     * function to maintain power after the device has booted.
     *
     * @param powerCtrlPin The GPIO pin number controlling the power relay.
     */
    void enablePowerRelay(int powerCtrlPin);

    // Getters for Current State
    /**
     * @brief Retrieves the current battery voltage.
     * @return The current battery voltage in Volts.
     */
    float getCurrentVoltage() const;

    /**
     * @brief Retrieves the character representing the current battery level icon.
     * @return The character icon corresponding to the current battery level.
     */
    char getCurrentBatteryLevelIcon() const;

    // Callback Setup Methods
    /**
     * @brief Sets the callback function to be invoked when the battery level icon changes.
     *
     * @param callback The `std::function` to register, which takes a `char` (new level icon) as an argument.
     */
    void setOnBatteryLevelChangedCallback(
      std::function<void(char newLevelIcon)> callback);

    /**
     * @brief Sets the callback function to be invoked when the raw battery voltage is updated.
     *
     * @param callback The `std::function` to register, which takes a `float` (new voltage) as an argument.
     */
    void setOnBatteryVoltageUpdateCallback(std::function<void(float newVoltage)> callback);

    /**
     * @brief Sets the callback function to be invoked for shutdown warnings (e.g., low battery).
     *
     * @param callback The `std::function` to register, which takes a `const std::string&` (message key) as an argument.
     */
    void setOnShutdownWarningCallback(
      std::function<void(const std::string& messageKey)> callback);

    /**
     * @brief Sets the callback function to be invoked just before the actual system power-off.
     *
     * This allows for final cleanup or saving operations before power is cut.
     * @param callback The `std::function<void()>` to register.
     */
    void setOnPerformShutdownCallback(std::function<void()> callback);


  private:
    // Core Dependencies
    StatusbarUI* _statusbarPtr;         ///< Pointer to the `StatusbarUI` instance for displaying status messages.
    IconElement* _batteryIconElement;   ///< Pointer to the `IconElement` instance used for displaying the battery status.

    // Internal State Variables
    unsigned long _lastBatteryCheckMillis; ///< Timestamp of the last battery voltage check in milliseconds.
    bool _lowBatteryShutdownArmed;         ///< Flag indicating if the system is armed for an automatic low battery shutdown.
    float _currentBatteryVoltage;          ///< The most recently read battery voltage in Volts.
    char _currentBatteryLevelIcon;         ///< The character icon representing the current battery charge level.

    // Configuration Parameters (set via `init()` method)
    int _battAdcPin;                     ///< The ADC (Analog-to-Digital Converter) pin used for battery voltage sensing.
    int _powerCtrlPin;                   ///< The digital GPIO pin used to control the system's power relay.
    float _r1ValueOhm;                   ///< The resistance value of R1 in the battery voltage divider circuit (in Ohms).
    float _r2ValueOhm;                   ///< The resistance value of R2 in the battery voltage divider circuit (in Ohms).
    unsigned long _batteryCheckIntervalMs; ///< The interval (in milliseconds) at which the battery voltage is checked.
    float _lowThresholdPowerOffVolts;    ///< The voltage threshold (in Volts) below which an automatic system shutdown is initiated.
    float _hysteresisVolts;              ///< The hysteresis voltage (in Volts) to prevent rapid toggling of the low battery warning.

    // Battery Icon and Voltage Level Thresholds (set via `init()` method)
    char _battIconLevel6;               ///< Icon for battery level 6 (highest).
    char _battIconLevel5;               ///< Icon for battery level 5.
    char _battIconLevel4;               ///< Icon for battery level 4.
    char _battIconLevel3;               ///< Icon for battery level 3.
    char _battIconLevel2;               ///< Icon for battery level 2.
    char _battIconLevel1;               ///< Icon for battery level 1.
    char _battIconLevel0;               ///< Icon for battery level 0 (empty/critical).
    char _battIconLevelUnknown;         ///< Icon for unknown battery level.
    float _battVoltageLevel6;           ///< Voltage threshold for battery level 6.
    float _battVoltageLevel5;           ///< Voltage threshold for battery level 5.
    float _battVoltageLevel4;           ///< Voltage threshold for battery level 4.
    float _battVoltageLevel3;           ///< Voltage threshold for battery level 3.
    float _battVoltageLevel2;           ///< Voltage threshold for battery level 2.
    float _battVoltageLevel1;           ///< Voltage threshold for battery level 1.

    // Callback Function Storage
    std::function<void(char newLevelIcon)> _batteryLevelChangedCallback;         ///< Callback for battery icon changes.
    std::function<void(const std::string& messageKey)> _shutdownWarningCallback; ///< Callback for shutdown warnings.
    std::function<void()> _performShutdownCallback;                              ///< Callback before actual power-off.
    std::function<void(float newVoltage)> _batteryVoltageUpdateCallback;         ///< Callback for raw voltage updates.

    // Private Helper Methods
    /**
     * @brief Reads the raw battery voltage from the ADC pin and converts it to Volts.
     * This method takes multiple readings and averages them for accuracy.
     * @return The calculated battery voltage in Volts.
     */
    float readBatteryVoltage();

    /**
     * @brief Determines the appropriate battery icon character based on the provided voltage.
     * This method uses the configured voltage thresholds to map voltage to an icon.
     * @param voltage The current battery voltage.
     * @return The character icon representing the battery level.
     */
    char determineBatteryLevelIcon(float voltage);

    /**
     * @brief Checks the current battery status and triggers warnings or shutdown if necessary.
     * This is the core logic for battery monitoring, invoked periodically by `loop()`.
     */
    void checkBatteryStatus();

    /**
     * @brief Executes the actual system power-off sequence.
     * This method sets the power control pin to LOW, effectively cutting power to the system.
     */
    void performActualPowerOff();
};

#endif // POWER_MANAGER_H